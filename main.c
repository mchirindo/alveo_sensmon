#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/stat.h>

#include "sensors.h" // lookup table for sensor info
#include "card.h"
#include "rw.h"

int max_num_of_sensors = 63, num_of_sensors = 63;
const char* alveo_card;
//int map_size = 4096UL;
int map_size = 65536UL;

/* Function prototypes */
void reg_sensors(void);

/* start of main */

int main(int argc, char **argv) {

	char *filename;
        off_t offset_size = 0x0004, offset, target_addr = 0x000000, base_addr;
	int type_width, sret, fd, ret;
	void *map_base, *virt_addr;
	uint32_t read_result, write_value = 0;
	fd_set read_sensors;
//        const char *serial_number = "serial_number";

        /* managing command line arguments */
        switch (argc) {
        case 1: /* when the process only is specified eg ./main */
             filename = "/dev/xdma0_user";
             target_addr = 0x028000;
             type_width = 'w';
             fprintf(stdout, "running %s, arguments captured:-\n device file: %s\n base address: 0x%x\n type_width: %c\n", argv[0], filename, (int)target_addr, type_width);
             break;
        case 2: /* when the process and the device file only are specified eg ./main /dev/xdma0_user */
             filename = argv[1];
             target_addr = 0x028000;
             type_width = 'w';
             fprintf(stdout, "running %s, arguments captured:-\n device file: %s\n base address: 0x%x\n type_width: %c\n", argv[0], filename, (int)target_addr, type_width);
             break;
        case 3: /* when the process, device file and the start address only are specified eg ./main /dev/xdma0_user 0x028004 */
             filename = argv[1];
             target_addr = strtoul(argv[2], 0, 0);
             type_width = 'w';
             num_of_sensors = 1;
             fprintf(stdout, "running %s, arguments captured:-\n device file: %s\n base address: 0x%x\n type_width: %c\n", argv[0], filename, (int)target_addr, type_width);
             break;
        case 4: /* when the process, device file, start address and type width only are specified eg ./main /dev/xdma0_user 0x028004 w */
             filename = argv[1];
             target_addr = strtoul(argv[2], 0, 0);
             type_width = tolower(argv[3][0]);
             if (argv[3][1] == '*') {
                num_of_sensors = strtoul(argv[3]+2, 0, 0);
                if (num_of_sensors > max_num_of_sensors)
                   num_of_sensors = max_num_of_sensors;
             }
             else
                num_of_sensors = 1;
             fprintf(stdout, "running %s, arguments captured:-\n device file: %s\n base address: 0x%x\n type_width: %c\n", argv[0], filename, (int)target_addr, type_width);
             break;
        case 5: /* when the process, device file, start address, type width and the write value only are specified eg ./main /dev/xdma0_user 0x028004 w 1 */
              filename = argv[1];
              target_addr = strtoul(argv[2], 0, 0);
              type_width = tolower(argv[3][0]);
              write_value = strtoull(argv[4], NULL, 0);
              num_of_sensors = 1;
              fprintf(stdout, "running %s, arguments captured:-\n device file: %s\n base address: 0x%x\n type_width: %c\n write_value: %d\n", argv[0], filename, (int)target_addr, type_width, write_value);
        };


	/* get file descriptor */
	fd = open(filename, O_RDWR | O_SYNC);
	if (fd < 0) {
	   fprintf(stderr, "error opening file %s\n", filename);
           fprintf(stdout, "exiting...\n");
           exit(EXIT_FAILURE);
        }
	else {
	   fprintf(stdout, "%s opened\n", filename);
	}

        base_addr = target_addr & ~(sysconf(_SC_PAGE_SIZE)-1);
        if (target_addr + max_num_of_sensors*type_width - base_addr > map_size)
           map_size = target_addr + max_num_of_sensors*type_width - base_addr;

        /* create memory map */
        map_base = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr);
        printf("PCI Memory mapped to address 0x%08lx.\n", (unsigned long) map_base);
	fflush(stdout);

        get_card_info(map_base, base_addr);

//        log_message(KATCP_LEVEL_INFO, "\ncard name:   %s\nserial No.:  %s\n", get_card_name(), get_serial_number());
        fprintf(stdout, "\ncard name:   %s\nserial No.:  %s\nmac address: %s\n             %s\n             %s\n             %s\ncard rev:    %s\nfan present: %s\npower avail: %s\nconfig mode: %s\nsat version: %s\n\n", get_card_name(), get_serial_number(), get_mac_address0(),get_mac_address1(), get_mac_address2(), get_mac_address3(), get_card_revision(), get_fan_presence(), get_total_power_available(), get_config_mode(), get_satelite_version());

//        char *name = get_card_name();
//        char *serial_number = get_serial_number();

        if (strcmp(get_serial_number(), "501211101AC2") == 0)
           alveo_card = "U50_0";
        else if (strcmp(get_serial_number(), "501211207V5X") == 0)
           alveo_card = "U50_1";
        else
           fprintf(stdout, "alveo card not found\n");
//        log_addsensor(name, serial_number, 0, 0);
//        log_update_sensor(name, serial_number, KATCP_STATUS_NOMINAL, 0); 
        
        if (argc < 3) {
          /* registering the sensors */
          reg_sensors();

          /* main loop - reading only */
          while(1) {
		FD_ZERO(&read_sensors);
		FD_SET(fd, &read_sensors);

		/* waiting for read file descriptor to be ready */
		sret = select(8, &read_sensors, NULL, NULL, NULL);
	

		if (sret == -1)
			fprintf(stderr, "error in select function\n");
		else if (sret == 0)
			fprintf(stderr, "timeout");
		else {
			/* read alveo sensor values and match the sensor values to some names and units */
			for (int i = 0; i < num_of_sensors; i++){
			    /* calculate the virtual address to be accessed */
                              virt_addr = map_base + target_addr + i*offset_size - base_addr;
				
			    switch (type_width) {
			    case 'b':
				    /* reading 8-bit values - byte */
				    read_result = *((uint8_t *) virt_addr);
				    /* updating sensor values in katcp */
				    log_update_sensor(alveo_card, get_name(i), KATCP_STATUS_NOMINAL, read_result);
				    break;
			    case 'h':
				    /* reading 16-bit values - half-word */
				    read_result = *((uint16_t *) virt_addr);
				    /* updating sensor values in katcp */
				    log_update_sensor(alveo_card, get_name(i), KATCP_STATUS_NOMINAL, read_result);
				    break;
			    case 'w':
				    /* reading 32-bit values - word */
				    read_result = *((uint32_t *) virt_addr);
				    /* updating sensor values in katcp */
				    log_update_sensor(alveo_card, get_name(i), KATCP_STATUS_NOMINAL, read_result);
				    break;
			    default:
				   fprintf(stderr, "Illegal data type '%c'.\n", type_width);
				   exit(2);
			    }
                         }	
		  }	
        	  sleep(1);
           }
       }
       else if (argc == 3 || argc == 4) {
          /* get card name */
      //    if (strcmp(argv[2], "CARDNAME") == 0) {
      //       char *string; 
      //       char *token;
      //       string = get_card_name();
      //       token = strtok(string, " ");
          
      //       if (strcmp(token, "ALVEO") == 0) {
      //          fprintf(stdout, "%s CARD\n", token);
      //          exit(EXIT_SUCCESS);
      //       }
      //    }      
          /* once-off read */
          for (int i = 0; i < num_of_sensors; i++) {
              offset = i*offset_size;
              read_result = read_reg(map_base, target_addr, base_addr, type_width, offset);
              fprintf(stdout, "read_result is:\n0x%lx 0x%x\n", target_addr, read_result);
          }
          exit(EXIT_SUCCESS);           
        }

       else if (argc > 4) {
           /* once-off write */
             ret = write_reg(map_base, target_addr, base_addr, type_width, write_value);
           if (ret < 0)
              fprintf(stdout, "write failed\n");
           else
              fprintf(stdout, "written 0x%lx:  0x%x\nreadback 0x%lx: 0x%x\n", target_addr, write_value, target_addr, write_value);
           exit(EXIT_SUCCESS);
        }
        return(0);	 
}

void reg_sensors(void) {

for (int i = 0; i < num_of_sensors; i++) {
    log_addsensor(alveo_card, get_name(i), getMin(i), getMax(i));
    }
}


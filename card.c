/*
 * card.c
 *
 *  Created on: 28 October 2021
 *      Author: mathews
 */

#include "card.h"
#include "rw.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int card_info[500];


void *get_card_info(void *mp_base, off_t bas_addr) {

    
     off_t mailbox_offset; 
     static const off_t contrl_addr = 0x028018, host_msg_offset_addr = 0x028300, host_msg_err_addr = 0x028304, reg_map_base_addr = 0x028000;
     unsigned int num_of_bytes;

     int width_w = 'w';
     int width_b = 'b';

     /*  check the mailbox availability by confirming CONTROL_REG[5] is 0 */
     if ((0x20 & (uint8_t)read_reg(mp_base, contrl_addr, bas_addr, width_w, 0)) == 0) {
        /* get value of HOST_MSG_OFFSET_REG offset */
   //     printf("testt1\n");
        mailbox_offset = read_reg(mp_base, host_msg_offset_addr, bas_addr, width_w, 0);
        /* write request message header into MAILBOX Word 0 (Opcode = 0x04) */
        write_reg(mp_base, reg_map_base_addr + mailbox_offset, bas_addr, width_w, 0x04000000);
        /* set CONTROL_REG[5] to 1 to indicate a new request message available */
        write_reg(mp_base, contrl_addr, bas_addr, width_w, 0x20);
        /* poll CONTROL_REG bit 5 until ‘0’ is received indicating CMS response is available */
        while ((0x20 & (uint8_t)read_reg(mp_base, contrl_addr, bas_addr, width_w, 0)) != 0) {
        }
        /* confirm no errors in HOST_MSG_ERR_REG */
        if (0x08000000 & (read_reg(mp_base, host_msg_err_addr, bas_addr, width_w, 0))) {
           fprintf(stderr, "MSP432 communication error\nexiting...\n");
           exit(EXIT_FAILURE);
        }
        else {
           /* read header bits 11:0 to determine the number of bytes in CMC response payload */
//           printf("Address here is 0x%lx\n", bas_addr + mailbox_offset);
           num_of_bytes = (0x0FFF & ((uint16_t)read_reg(mp_base, reg_map_base_addr + mailbox_offset, bas_addr, width_w, 0)));
           /* read the card information data */
           for (int i = 0; i < num_of_bytes; i++) {
               card_info[i] = read_reg(mp_base, reg_map_base_addr + mailbox_offset + 4 + i, bas_addr, width_b, 0);
           }

        }
     }
     else
        fprintf(stdout, "mailbox not available - try again!\n");
     return(0);
}

char *get_card_name(void) {

     static char card_name[20];
     int length;

     for (int i = 0; i < 132; i++) {
        if (card_info[i] == 0x27) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               card_name[j] = card_info[i + 2 + j];
           }
         }
     }
     return card_name;
}

char *get_serial_number(void) {

     static char card_sn[20];
     int length;

     for (int i = 0; i < 132; i++) {
         if (card_info[i] == 0x21) {
            length = card_info[i + 1];
              for (int j = 0; j < length; j++) {
                    card_sn[j] = card_info[i + 2 + j];
              }
          }
     }
     return card_sn;
}

char *get_mac_address0(void) {

     static char mac_address0[40];
     int length;

     for (int i = 0; i < 132; i++) {
         if (card_info[i] == 0x22) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               mac_address0[j] = card_info[i + 2 + j];
           }
         }
      }
      return mac_address0;
}

char *get_mac_address1(void) {

     static char mac_address1[40];
     int length;

     for (int i = 0; i < 132; i++) {
         if (card_info[i] == 0x23) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               mac_address1[j] = card_info[i + 2 + j];
           }
         }
      }
      return mac_address1;
}

char *get_mac_address2(void) {

     static char mac_address2[40];
     int length;

     for (int i = 0; i < 132; i++) {
         if (card_info[i] == 0x24) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               mac_address2[j] = card_info[i + 2 + j];
           }
         }
      }
      return mac_address2;
}

char *get_mac_address3(void) {

     static char mac_address3[40];
     int length;

     for (int i = 0; i < 132; i++) {
         if (card_info[i] == 0x25) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               mac_address3[j] = card_info[i + 2 + j];
           }
         }
      }
      return mac_address3;
}

char *get_card_revision(void) {

    static char card_rev[5];
    int length;

    for (int i = 0; i < 132; i++) {
        if (card_info[i] == 0x26) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               card_rev[j] = card_info[i + 2 + j];
           }
        }
     }
     return card_rev;
}

char *get_satelite_version(void) {

    static char sat_version[10];
    int length;

    for (int i = 0; i < 132; i++) {
        if (card_info[i] == 0x28) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               sat_version[j] = card_info[i + 2 + j];
           }
        }
    }
    return sat_version;
}


char *get_fan_presence(void) {

    static char fan_presence[5];
    int length;

    for (int i = 0; i < 132; i++) {
        if (card_info[i] == 0x2a) {
           length = card_info[i + 1];
           for (int j = 0; j < length; j++) {
               fan_presence[j] = card_info[i + 2 + j];
           }
        }
     }
     return fan_presence;
}

char *get_total_power_available(void) {

     static char total_power_avail[10];
     int length;
     static char *power0 = "75W";
     static char *power1 = "150W";
     static char *power2 = "225";
     static char *power3 = "300W";

     for (int i = 0; i < 132; i++) {
         if (card_info[i] == 0x29) {
            length = card_info[i + 1];
            for (int j = 0; j < length; j++) {
                total_power_avail[j] = card_info[i + 2 + j];
            }

            if (total_power_avail[0] == 0x00)
               return power0;
            else if (total_power_avail[0] == 0x01)
               return power1;
            else if (total_power_avail[0] == 0x02)
               return power2;
            else if (total_power_avail[0] == 0x03)
               return power3;
            else
               fprintf(stderr, "total available power not found");
          }
      }
      return(0);
}


char *get_config_mode(void) {

     static char config_mode[20];
     int length;
     static char *mode0 = "Slave_Serial_x1";
     static char *mode1 = "Slave_Serial_x1";
     static char *mode2 = "Slave_Map_x16";
     static char *mode3 = "Slave_Select_Map_x32";
     static char *mode4 = "JTag_Boundary_Scan_x1";
     static char *mode5 = "Master_SPI_x1";
     static char *mode6 = "Master_SPI_x2";
     static char *mode7 = "Master_SPI_x4";
     static char *mode8 = "Master_SPI_x8";
     static char *mode9 = "Master_BPI_x8";
     static char *mode10 = "Master_BPI_x16";
     static char *mode11 = "Master_Serial_x1";
     static char *mode12 = "Master_Select_Map_x8";
     static char *mode13 = "Master_Select_Map_x16";

     for (int i = 0; i < 132; i++) {
         if (card_info[i] == 0x2b) {
            length = card_info[i + 1];
            for (int j = 0; j < length; j++) {
                config_mode[j] = card_info[i + 2 + j];
            }
      
            if (config_mode[0] == 0x00)
               return mode0;
            else if (config_mode[0] == 0x01)
               return mode1;
            else if (config_mode[0] == 0x02)
               return mode2;
            else if (config_mode[0] == 0x03)
               return mode3;
            else if (config_mode[0] == 0x04)
               return mode4;
            else if (config_mode[0] == 0x05)
               return mode5;
            else if (config_mode[0] == 0x06)
               return mode6;
            else if (config_mode[0] == 0x07)
               return mode7;
            else if (config_mode[0] == 0x08)
               return mode8;
            else if (config_mode[0] == 0x09)
               return mode9;
            else if (config_mode[0] == 0x0a)
               return mode10;
            else if (config_mode[0] == 0x0b)
               return mode11;
            else if (config_mode[0] == 0x0c)
               return mode12;
            else if (config_mode[0] == 0x0d)
               return mode13;
            else
               fprintf(stderr, "configuration mode not found");
         }
     }
     return(0);
}

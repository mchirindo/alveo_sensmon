/*
 * rw.c
 *
 *  Created on: 28 October 2021
 *      Author: mathews
 */

#include "rw.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void *virt_address;


int read_reg(void *mapbase, off_t targetaddr, off_t baseaddr, int typ_width,  off_t offset_siz) {

       uint32_t read_result = 0;
       int access_type;

       access_type = typ_width;
       virt_address = mapbase + targetaddr + offset_siz - baseaddr;
//       printf("mapbase is 0x%p\n", mapbase);
//       printf("virt_address is 0x%p\n", virt_address);

        switch(access_type) {
               case 'b':
                       read_result = *((uint8_t *) virt_address);
                       break;
               case 'h':
                       read_result = *((uint16_t *) virt_address);
                       break;
               case 'w':
                       read_result = *((uint32_t *) virt_address);
                       break;
       }

//    fprintf(stdout, "read_result is: %x\n", read_result);
     //  fflush(stdout);
       return read_result;
}


int write_reg(void *mapbase, off_t targetaddr, off_t baseaddr, int typ_width, uint32_t write_val) {

    int access_type;
    uint32_t read_result = 0;

    access_type = typ_width;

    virt_address = mapbase + targetaddr - baseaddr;
    switch(access_type) {
          case 'b':
                  *((uint8_t *) virt_address) = write_val;
                  read_result = *((uint8_t *) virt_address);
                  break;
          case 'h':
                  *((uint16_t *) virt_address) = write_val;
                  read_result = *((uint16_t *) virt_address);
                  break;
          case 'w':
                  *((uint32_t *) virt_address) = write_val;
                  read_result = *((uint32_t *) virt_address);
                  break;
     }

     if (write_val == read_result)
        return (0);
     else
        return (-1);
//     printf("Written 0x%0*X; readback 0x%*X\n", typ_width, write_val, typ_width, read_result);
//     fflush(stdout);
//     return(0);
}


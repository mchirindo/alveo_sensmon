/*
 * rw.h
 *
 *  Created on: 28 October 2021
 *      Author: mathews
 */

#ifndef _RW_H_
#define _RW_H_

#include <stdint.h>
#include <stdlib.h>

int read_reg(void *mapbase, off_t targetaddr, off_t baseaddr, int typ_width, off_t offset_siz);
int write_reg(void *mapbase, off_t targetaddr, off_t baseaddr, int typ_width, uint32_t write_val);



#endif /* _RW_H */

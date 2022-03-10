/*
 * card.h
 *
 *  Created on: 28 October 2021
 *      Author: mathews
 */

#ifndef _CARD_H_
#define _CARD_H_

#include <stdint.h>
#include <stdlib.h>

void *get_card_info(void *mp_base, off_t bas_addr);
char *decode_card_info(void *map_bse, off_t base_address, int key);
char *get_card_name(void);
char *get_serial_number(void);
char *get_mac_address0(void);
char *get_mac_address1(void);
char *get_mac_address2(void);
char *get_mac_address3(void);
char *get_card_revision(void);
char *get_satelite_version(void);
char *get_fan_presence(void);
char *get_total_power_available(void);
char *get_config_mode(void);


#endif /* _CARD_H */

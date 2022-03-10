/*
 * sensors.h
 *
 *  Created on: 23 August 2021
 *      Author: mathews
 */

#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "katcp.h"
#include "katcl.h"

#ifdef __cplusplus
extern "C" {
#endif

int log_init(void);

int log_message(int loglevel, char *fmt, ...);

int log_sensorlist(void);

int log_addsensor(const char *card, const char *name, double min, double max);

int log_sensorstatus(void);

int log_update_sensor(const char *card, const char *name, int status, double val);


const char *get_name(int index);
double getMin(int index);
double getMax(int index);
//const char *get_unit(int index);

#endif /* _SENSOR_H */

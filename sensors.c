/*
 * sensors.c
 *
 *  Created on: 23 August 2021
 *      Author: mathews
 */

#include "sensors.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>


//#include "log.h"

struct sensor_lookup {
        const char *card;
        const char *name;
        const char *sensor;
        const char *desc;
        const char *unit;
        double min;
        double max;
//        float mult;
//        float div;
};

static const struct sensor_lookup sensor_reg[] = {
{"U50_0","reg1", "REG_MAP_ID", "register map ID", NULL, 0, 2000000000},{"U50_0", "reg2","FW_VERSION", "firmware version",  NULL, 0, 300000000},{"U50_0", "reg3","STATUS_REG",  "CMS status register", NULL, 0, 400000000},{"U50_0", "reg4", "ERROR_REG",  "CMS error register", NULL, 0, 0},
{"U50_0", "reg5", "RESERVED",  "reserved register", NULL, 0, 0},{"U50_0", "reg6", "PROFILE_NAME",  "software profile", NULL, 0, 2000000000},{"U50_0", "reg7", "CONTROL_REG", "CMS control register", NULL, 0, 0},{"U50_0", "reg8", "RESERVED",  "reserved register", NULL, 0, 0},
{"U50_0", "volt1", "12V_PEX_MAX",  "12V_PEX Max Voltage", "mV", 10000.0, 13000.0},{"U50_0", "volt2", "12V_PEX_AVG",  "12V_PEX Average Voltage", "mV", 10000.0, 13000.0},{"U50_0", "volt3", "12V_PEX_INS",  "12V_PEX Instantaneous Voltage","mV", 10000.0, 13000.0},{"U50_0", "volt4", "3V3_PEX_MAX",  "3V3_PEX Max Voltage", "mV", 3000.0, 3500.0},
{"U50_0", "volt5", "3V3_PEX_AVG",  "3V3_PEX Average Voltage", "mV", 3000.0, 3500.0},{"U50_0", "volt6", "3V3_PEX_INS",  "3V3_PEX Instantaneous Voltage", "mV", 3000.0, 3500.0},{"U50_0", "volt7", "3V3_AUX_MAX",  "3V3_AUX Max Voltage", "mV", 0, 3500.0},{"U50_0", "volt8", "3V3_AUX_AVG",  "3V3_AUX Average Voltage", "mV", 0.0, 3500.0},
{"U50_0", "volt9", "3V3_AUX_INS",  "3V3_AUX Instantaneous Voltage", "mV", 0.0, 3500.0},{"U50_0", "volt10", "12V_AUX_MAX",  "12V_AUX Max Voltage", "mV", 0.0, 13000.0},{"U50_0", "volt11", "12V_AUX_AVG",  "12V_AUX Average Voltage", "mV", 0.0, 13000.0},{"U50_0", "volt12", "12V_AUX_INS",  "12V_AUX Instantaneous Voltage", "mV", 0.0, 13000.0},
{"U50_0", "volt13", "DDR4_VPP_BTM_MAX",  "DDR4 VPP BTM Max Voltage", "mV", 0.0, 0.0},{"U50_0", "volt14", "DDR4_VPP_BTM_AVG",  "DDR4 VPP BTM Average Voltage", "mV", 0.0, 0.0},{"U50_0", "volt15", "DDR4_VPP_BTM_INS",  "DDR4 VPP BTM Instantaneous Voltage", "mV", 0.0, 0.0},
{"U50_0", "volt16", "SYS_5V5_MAX",  "SYS_5V5 Max Voltage", "mV", 4000.0, 6000.0},{"U50_0", "volt17", "SYS_5V5_AVG",  "SYS_5V5 Average Voltage", "mV", 4000.0, 6000.0},{"U50_0", "volt18", "SYS_5V5_INS",  "SYS_5V5 Instantaneous Voltage", "mV", 4000.0, 6000.0},{"U50_0", "volt19", "VCC1V2_TOP_MAX",  "VCC1V2_TOP Max Voltage", "mV", 0.0, 1250.0},
{"U50_0", "volt20", "VCC1V2_TOP_AVG",  "VCC1V2_TOP Average Voltage", "mV", 0.0, 1250.0},{"U50_0", "volt21", "VCC1V2_TOP_INS",  "VCC1V2_TOP Instantaneous Voltage", "mV", 0.0, 1250.0},{"U50_0", "volt22", "VCC1V8_MAX",  "VCC1V8 Max Voltage", "mV", 1500.0, 2000.0},{"U50_0", "volt23", "VCC1V8_AVG",  "VCC1V8 Average Voltage", "mV", 1500.0, 2000.0},
{"U50_0", "volt24", "VCC1V8_INS",  "VCC1V8 Instantaneous Voltage", "mV", 1500.0, 2000.0},{"U50_0", "volt25", "VCC0V85_MAX",  "VCC0V85 Max Voltage", "mV", 0.0, 1000.0},{"U50_0", "volt26", "VCC0V85_AVG",  "VCC0V85 Average Voltage", "mV", 0.0, 1000.0},{"U50_0", "volt27", "VCC0V85_INS",  "VCC0V85 Instantaneous Voltage", "mV", 0.0, 1000.0},
{"U50_0","volt28", "DDR4_VPP_TOP_MAX",  "DDR4_VPP_TOP Max Voltage", "mV", 0.0, 0.0},{"U50_0", "volt29", "DDR4_VPP_TOP_AVG",  "DDR4_VPP_TOP Average Voltage", "mV", 0.0, 0.0},{"U50_0", "volt30", "DDR4_VPP_TOP_INS",  "DDR4_VPP_TOP Instantaneous Voltage", "mV", 0.0, 0.0},{"U50_0", "volt31", "MGT0V9AVCC_MAX",  "MGT0V9AVCC Max Voltage", "mV", 800.0, 1000.0},
{"U50_0", "volt32", "MGT0V9AVCC_AVG",  "MGT0V9AVCC Average Voltage", "mV", 800.0, 1000.0},{"U50_0", "volt33", "MGT0V9AVCC_INS",  "MGT0V9AVCC Instantaneous Voltage", "mV", 800.0, 1000.0},{"U50_0", "volt34", "12V_SW_MAX",  "12V_SW Max Voltage", "mV", 0.0, 13000.0},{"U50_0", "volt35", "12V_SW_AVG",  "12V_SW Average Voltage", "mV", 0.0, 13000.0},
{"U50_0", "volt36", "12V_SW_INS",  "12V_SW Instantaneous Voltage", "mV", 0.0, 13000.0},{"U50_0", "volt37", "MGTAVTT_MAX",  "MGTAVTT Max Voltage", "mV", 1000.0, 1300.0},{"U50_0", "volt38", "MGTAVTT_AVG",  "MGTAVTT Average Voltage", "mV", 1000.0, 1300.0},{"U50_0", "volt39", "MGTAVTT_INS",  "MGTAVTT Instantaneous Voltage", "mV", 1000.0, 1300.0},
{"U50_0", "volt40", "VCC1V2_BTM_MAX",  "VCC1V2_BTM Max Voltage", "mV", 0.0, 13000.0},{"U50_0", "volt41", "VCC1V2_BTM_AVG",  "VCC1V2_BTM Average Voltage", "mV", 0.0, 13000.0},{"U50_0", "volt42", "VCC1V2_BTM_INS",  "VCC1V2_BTM Instantaneous Voltage", "mV", 0.0, 13000.0},{"U50_0", "curr1", "12VPEX_I_IN_MAX",  "12VPEX_I_IN Max Current", "mA", 0.0, 1500.0},
{"U50_0", "curr2", "12VPEX_I_IN_AVG",  "12VPEX_I_IN Average Current", "mA", 0.0, 1500.0},{"U50_0", "curr3", "12VPEX_I_IN_INS_REG",  "12VPEX_I_IN Instantaneous Current", "mA", 0.0, 1500.0},{"U50_0", "curr4", "12V_AUX_I_IN_MAX", "12V_AUX_I_IN Max Current", "mA", 0.0, 0.0},{"U50_0", "curr5", "12V_AUX_I_IN_AVG",  "12V_AUX_I_IN Average Current", "mA", 0.0, 0.0},
{"U50_0", "curr6", "12V_AUX_I_IN_INS",  "12V_AUX_I_IN Instantaneous Current", "mA", 0.0, 0.0},{"U50_0", "volt43", "VCCINT_MAX",  "VCCINT Max Voltage", "mV", 500.0, 1000.0},{"U50_0", "volt44", "VCCINT_AVG",  "VCCINT Average Voltage", "mV", 500.0, 1000.0},{"U50_0", "vol45", "VCCINT_INS",  "VCCINT Instantaneous Voltage", "mV", 500.0, 1000.0},
{"U50_0", "curr7", "VCCINT_I_MAX", "VCCINT_I Max Current", "mA", 0.0, 7000.0},{"U50_0", "curr8", "VCCINT_I_AVG",  "VCCINT_I Average Current", "mA", 0.0, 7000.0},{"U50_0", "curr9", "VCCINT_I_INS",  "VCCINT_I Instantaneous Current", "mA", 0.0, 7000.0},{"U50_0", "temp1", "FPGA_TEMP_MAX", "FPGA_TEMP Max Temperature", "C", 0.0, 70.0}
};

static struct katcl_line *k = NULL;
static char *app = "roach2hwmon";
static volatile int logEnabled = 0;

const char *get_sensor(int index)
{
        if (index >= 0) {
                return sensor_reg[index].sensor;
        }

        return NULL;
}

const char *get_unit(int index)
{
        if (index >= 0) {
                return sensor_reg[index].unit;
        }

        return NULL;
}

const char *get_name(int index)
{
         if (index >= 0) {
                 return sensor_reg[index].name;
         }
         return NULL;
}


//static int getSensorIndex(const char *card, const char *sensor)
static int getSensorIndex(const char *card, const char *name)
{
        int i = 0;

        while (sensor_reg[i].card != NULL) {
                if (strcmp(card, sensor_reg[i].card) == 0) {
                        if (strcmp(name, sensor_reg[i].name) == 0) {
                                return i;
                        }
                }
                i++;
        }

        return -1;
}

static const char *getDescription(int index)
{
        if (index >= 0) {
                return sensor_reg[index].desc;
        }

        return NULL;
}


//static float getDiv(int index)
//{
//        if (index >= 0) {
//                return table[index].div;
//        }

//        return 1;
//}

double getMin(int index)
{
        if (index >= 0) {
                return sensor_reg[index].min;
        }

        return 1;
}

double getMax(int index)
{
        if (index >= 0) {
                return sensor_reg[index].max;
        }

        return 1;
}


int log_init(void)
{
    k = create_katcl(STDOUT_FILENO);
    
   // k = create_katcl(fds);

    if (k == NULL) {
        fprintf(stderr, "unable to create katcl message logic.");
        return -1;
    }

    logEnabled = 1;

    return 0;
}

int log_message(int loglevel, char *fmt, ...)
{
    va_list args;
    int ret = -1;

    if (k == NULL || !logEnabled) {
        return -1;
    }
    va_start(args, fmt);
    ret = vlog_message_katcl(k, loglevel, app, fmt, args);
    va_end(args);

    /* write the log data out */
    while ((ret = write_katcl(k)) == 0);

    if (ret < 0) {
        return -1;
    }

    return ret;
}

//int log_addsensor(const char *chip, char *name, double min, double max)
int log_addsensor(const char *card, const char *name, double min, double max)
{
  //int log_addsensor(const char *chip, char *name, double min, double max)      
        long minVal = (long)min, maxVal = (long)max;
        int sensorIndex = -1;
        int ret = 0;

        log_init();

        if (k == NULL || !logEnabled) {
                return -1;
        }

        /* check if the sensor exists in the table */
        sensorIndex = getSensorIndex(card, name);

        /* register sensor(s) */
        if (sensorIndex >= 0) {
            ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, KATCP_SENSOR_LIST_INFORM);
            ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)get_sensor(sensorIndex));
            ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)getDescription(sensorIndex));
            ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)get_unit(sensorIndex));
            ret += append_string_katcl(k, KATCP_FLAG_STRING, "float");

            /* multiply with a 1000 since our units are in milli's */
   //         minVal = (min * 1000) * getMult(sensorIndex) / getDiv(sensorIndex);
   //         maxVal = (max * 1000) * getMult(sensorIndex) / getDiv(sensorIndex);

            ret += append_signed_long_katcl(k, KATCP_FLAG_SLONG, minVal);
            ret += append_signed_long_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_SLONG, maxVal);

        /* write the log data out */
        while ((ret = write_katcl(k)) == 0);
        }

        return ret;
}


int log_update_sensor(const char *card, const char *name, int status, double val)
//int log_update_sensor(const char *chip, char *name, int status, double val)
{
        struct timeval now;
        int sensorIndex = -1;
       // long value = 0;
        int ret = 0;

        log_init();
	
        if (k == NULL || !logEnabled) {
                return -1;
        }

        /* check if the sensor exists in the table */
        sensorIndex = getSensorIndex(card, name);

        if (sensorIndex >= 0) {
        gettimeofday(&now, NULL);

            /* update sensor status */   
            ret += append_string_katcl(k, KATCP_FLAG_FIRST | KATCP_FLAG_STRING, KATCP_SENSOR_STATUS_INFORM);
	    ret += append_unsigned_long_katcl(k, KATCP_FLAG_ULONG, (unsigned long)now.tv_sec);
	    ret += append_string_katcl(k, KATCP_FLAG_STRING, "1");
            ret += append_string_katcl(k, KATCP_FLAG_STRING, (char*)get_sensor(sensorIndex));

            if (status == KATCP_STATUS_NOMINAL) {
                    ret += append_string_katcl(k, KATCP_FLAG_STRING, "nominal");
            } else if (status == KATCP_STATUS_WARN) {
                    ret += append_string_katcl(k, KATCP_FLAG_STRING, "warn");
            } else if (status == KATCP_STATUS_ERROR) {
                    ret += append_string_katcl(k, KATCP_FLAG_STRING, "error");
            } else if (status == KATCP_STATUS_FAILURE) {
                    ret += append_string_katcl(k, KATCP_FLAG_STRING, "failure");
            } else {
                    ret += append_string_katcl(k, KATCP_FLAG_STRING, "unknown");
            }

            append_signed_long_katcl(k, KATCP_FLAG_LAST | KATCP_FLAG_SLONG, val);

        /* write the log data out */
 	while ((ret = write_katcl(k)) == 0);
    }
        destroy_katcl(k, 0);

        return ret;
}


/*
 * Copyright (c) 2009 Sven Bachmann (dev@mcbachmann.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __WS444_H_
#define __WS444_H_

#include <linux/types.h>
#include <semaphore.h>

/* debug functions */
#if WS_DEBUG == 1
#  define DBG(type, fmt, str...)	fprintf(stderr, "%s (%i) - %s: " fmt, __FILE__, __LINE__, type, ##str)
#else
#  define DBG(type, fmt, str...)	{}
#endif
#define DEBUG(fmt, str...)	DBG("DEBUG", fmt, ##str)
#define ERR(fmt, str...)	DBG("ERROR", fmt, ##str)
#define INFO(fmt, str...)	DBG("INFO", fmt, ##str)

/* error reporting */
#define check(func, action)      if ((ret = func) < 0) { ERR("Function call \""#func"\" failed: %i\n", ret); action; }
#define check_ftdi(func, action) if ((ret = func) < 0) { ERR("Function call \""#func"\" failed: %d (%s)\n", ret, ftdi_get_error_string(&ftdic)); action; }
#define check_cond(func, cond, action) if ((ret = func) cond) { ERR("Function call \""#func"\" failed (Condition %s): %i\n", #cond, ret); action; }
#define check_func(func, func_name, action) if ((ret = func) < 0) { ERR("Function call \"%s\" failed: %i\n", func_name, ret); action; }

/* define global constants */
#define CMD_ESCAPE	    0xF8
#define CMD_BEG		    0xFE
#define CMD_END		    0xFC

#define CMD_CONFIG_SAVE	    0x30
#define CMD_RECORD_NEXT	    0x31
#define CMD_CONFIG_READ	    0x32
#define CMD_RECORD_CURRENT  0x33
#define CMD_VERSION	    0x34

#define WS_BUFFER_LEN	    1024

#define WS_OK		    0
#define WS_ERR		    -1

#define WS_RECV_TIMEOUT	    100000	    /* 0.1s */

#define WS_SENSOR_AVAIL	    0x10

/* define structs */
struct version_s {
    __u8    nr;
} __attribute__ ((__packed__));

struct config_s {
    __u8	    sensor[9];
    __u8	    tx;
    __u8	    m;
    __u8	    onz;		    /* over normal zero */
    __u16	    rainfall_mm;	    /* mm/Wippe - mm/compensator */
} __attribute__ ((__packed__));

struct record_s {
    struct __attribute__ ((__packed__)) {
	__s16	temp;
	__u8	humidity;
    } sensor[9];

    __u16	rain;
    __u16	wind;

    struct __attribute__ ((__packed__)) {
	__s16	temp;
	__u8	humidity;
	__u16	pressure;
	__u8	forecast;
    } station;
} __attribute__ ((__packed__));

struct record_next_s {
    __u16	tvoid;
    __u16	time;
    struct __attribute__ ((__packed__)) {
	__s16	temp;
	__u8	humidity;
    } sensor[9];

    __u16	rain;
    __u16	wind;

    struct __attribute__ ((__packed__)) {
	__s16	temp;
	__u8	humidity;
	__u16	pressure;
	__u8	forecast;
    } station;
} __attribute__ ((__packed__));



struct receive_s {
    int     tries;			    /* sleeps WS_RECV_TIMEOUT between each try */
    sem_t*  wait;
    u_char* buffer;
    u_char  command;			    /* eg. CMD_VERSION */
    u_int   length;
    u_int   pos;
};

/* define prototypes */

/* ws_device.c */
int  ws_device_open(void);
void ws_device_close(void);

/* ws_send.c */
int ws_send(unsigned char ws_cmd);

/* ws_receive.c */
int  ws_recv_init(struct receive_s*);
void ws_recv_stop(struct receive_s*);

/* ws_config.c */
int ws_config_read(void*);

/* ws_version.c */
int ws_version(void*);

/* ws_record.c */
int ws_record_current(void *);
int ws_record_next(void *);

/* ws_misc.c */
int ws_send_repeat(int (*)(void*), void *, char*);
void ws_dump(void*, int);
u_long ws_timestamp(void);

/* ws_convert.c */
double ws_conv_rainfall(struct config_s*, struct record_s*);
char * ws_conv_forecast(struct record_s*);
double ws_conv_wind_speed(struct record_s*);
double ws_conv_temperature(int);
double ws_conv_pressure(struct config_s*, struct record_s*);

#endif /* __WS444_H_ */


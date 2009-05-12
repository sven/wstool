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

#include <stdio.h>
#include "ws.h"

/* variables */
struct config_s config;
struct record_s record;
struct version_s version;

/* prototypes */
void ws_print_human(void);
void ws_print_human_sensor(int, int, int);
void ws_print_json(void);
void ws_print_json_sensor(int, int, int, int);
void ws_print_raw(void);

/*
 * main
 *
 * Demonstration of weatherstation polling.
 */
int main(int argc, char **argv)
{
    int ret;

    /* initialize USB connection */
    check(ws_device_open(), goto bye);

    /* read stations version */
    check(ws_send_repeat(ws_version, &version, "ws_version(version)"), goto bye_close);

    /* read configuration */
    check(ws_send_repeat(ws_config_read, &config, "ws_config_read(config)"), goto bye_close);

    /* read current record */
    check(ws_send_repeat(ws_record_current, &record, "ws_record_current(config)"), goto bye_close);

    /* present the data */
    ws_print_json();

    /* all went fine */
    ret = 0;

bye_close:
    ws_device_close();

bye:
    return ret;
}

/*
 * ws_print_human
 *
 * Print data human readable.
 */
void ws_print_human(void)
{
    int cnt;

    printf("Version:                  %s\n",   VERSION);
    printf("Timestamp:                %lu\n",  ws_timestamp());
    printf("Version Station:          %i\n",   version.nr);
    printf("Wind Speed:               %.1f km/h\n", ws_conv_wind_speed(&record));
    printf("Rain Sensor:              %.1f mm\n", ws_conv_rainfall(&config, &record));
    printf("Forecast:                 %s\n",   ws_conv_forecast(&record));

    ws_print_human_sensor(0, record.station.temp, record.station.humidity);

    for (cnt = 0; cnt < 9; cnt++) {
	/* check if sensor is available */
	if ((config.sensor[cnt] & 0xF0) != WS_SENSOR_AVAIL) {
	    continue;
	}

	ws_print_human_sensor(cnt + 1, record.sensor[cnt].temp, record.sensor[cnt].humidity);
    }
}

/*
 * ws_print_human_sensor
 *
 * Print sensor specific data.
 */
void ws_print_human_sensor(int id, int temp, int hum)
{
    printf("Sensor %i\n", id);
    printf("  Temperature:            %.1f °C\n", ws_conv_temperature(temp));
    printf("  Humiditiy:              %i %%\n",   hum);
}

/*
 * ws_print_json
 *
 * Print data in json format.
 */
void ws_print_json(void)
{
    int cnt;
    int sensor_avail = 0;

    printf("{\n");
    printf("  \"version\":         \"%s\",\n",   VERSION);
    printf("  \"timestamp\":       \"%lu\",\n",  ws_timestamp());
    printf("  \"version_station\": \"%i\",\n",   version.nr);
    printf("  \"wind_speed\":      \"%.1f\",\n", ws_conv_wind_speed(&record));
    printf("  \"rain_sensor\":     \"%.1f\",\n", ws_conv_rainfall(&config, &record));
    printf("  \"forecast\":        \"%s\",\n",   ws_conv_forecast(&record));

    printf("  \"sensor\":          {\n");

    /* check if other sensors are available */
    for (cnt = 0; cnt < 9; cnt++) {
	if ((config.sensor[cnt] & 0xF0) != WS_SENSOR_AVAIL) {
	    continue;
	}
	sensor_avail++;
    }

    /* print local station data */
    ws_print_json_sensor(0, record.station.temp, record.station.humidity, sensor_avail);

    /* show available sensors */
    for (cnt = 0; cnt < 9; cnt++) {

	/* check if sensor is available */
	if ((config.sensor[cnt] & 0xF0) != WS_SENSOR_AVAIL) {
	    continue;
	}

	ws_print_json_sensor(cnt + 1, record.sensor[cnt].temp, record.sensor[cnt].humidity, --sensor_avail);
    }

    printf("  }\n");
    printf("}\n");
}

/*
 * ws_print_json_sensor
 *
 * Print sensor specific data.
 */
void ws_print_json_sensor(int id, int temp, int hum, int comma)
{
    printf("    \"%i\":               {\n", id);
    printf("      \"temperature\":    \"%.1f\",\n", ws_conv_temperature(temp));
    printf("      \"humidity\":       \"%i\"\n", hum);
    printf("    }%s\n", (comma) ? "," : "");
}

/*
 * ws_print_raw
 *
 * Print raw data structures.
 */
void ws_print_raw(void)
{
    printf("Version: ");
    ws_dump(&version, sizeof(struct version_s));
    printf("\n");

    printf("Config:  ");
    ws_dump(&config, sizeof(struct config_s));
    printf("\n");

    printf("Record:  ");
    ws_dump(&record, sizeof(struct record_s));
    printf("\n");
}



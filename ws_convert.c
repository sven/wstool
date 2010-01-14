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

#include "ws.h"

char* forecast[] = { "sunny", "clear", "cloudy", "rain" };

/*
 * ws_conv_rainfall
 *
 * Make rainfall value human readable.
 */
double ws_conv_rainfall(struct config_s* config, struct record_s* record)
{
    double mult = (double) config->rainfall_mm / 1000;
    return (mult * ((double) record->rain));
}

/*
 * ws_conv_forecast
 *
 * Make forecast value human readable.
 */
char * ws_conv_forecast(struct record_s* record)
{
    return forecast[record->station.forecast & 0xF];
}

/*
 * ws_conv_wind_speed
 *
 * Make wind speed human readable.
 */
double ws_conv_wind_speed(struct record_s* record)
{
    return ((double) record->wind / 10);
}

/*
 * ws_conv_temperature
 *
 * Make temperature human readable.
 */
double ws_conv_temperature(int temp)
{
    if (temp > 0xFF00)
        temp -= 0x10000;
    return ((double) temp / 10);
}

/*
 * ws_conv_pressure
 *
 * Make atmospheric pressure human readable.
 */
double ws_conv_pressure(struct config_s* config, struct record_s* record)
{
    return (double)record->station.pressure + (double)config->onz/8 - 1;
}

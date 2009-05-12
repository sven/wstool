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
#include <time.h>
#include "ws.h"

/*
 * ws_send_repeat
 *
 * Repeat the send function if it fails.
 */
int ws_send_repeat(int (*func)(void*), void* parm, char* func_name)
{
    int cnt;
    int ret;

    for (cnt = 5; cnt > 0; cnt--) {
	check_func(func(parm), func_name, continue);

	ret = WS_OK;
	break;
    }

    return ret;
}

/*
 * ws_dump
 *
 * Dump received values in hex.
 */
void ws_dump(void* buf, int len)
{
    int cnt;

    for (cnt = 0; cnt < len; cnt++) {
	printf("0x%02X ", ((u_char*) buf)[cnt]);
    }
}

/*
 * ws_timestamp
 *
 * Get current time as timestamp.
 */
u_long ws_timestamp(void)
{
    return((u_long) time(NULL));
}


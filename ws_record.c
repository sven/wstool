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
#include <semaphore.h>
#include <asm/byteorder.h>
#include "ws.h"

/*
 * ws_record_current
 *
 * Read current record.
 */
int ws_record_current(void *record_void)
{
    int cnt;
    int ret;
    struct receive_s recv;
    struct record_s *record = record_void;

    /* initialize receiving thread */
    recv.tries = 10;
    recv.buffer = (u_char*) record;
    recv.length = sizeof(struct record_s);
    recv.command = CMD_RECORD_CURRENT;
    
    check(ws_recv_init(&recv), goto bye);

    /* send current record command */
    check(ws_send(CMD_RECORD_CURRENT), goto bye_recv);

    /* wait for data and end thread */
    sem_wait(recv.wait);

    /* check if data is valid */
    if (recv.tries < 1) {
	ret = WS_ERR;
	goto bye_recv;
    }

    /* convert data to CPU endianess */
    INFO("Received %i record bytes.\n", recv.pos);
    for (cnt = 0; cnt < 9; cnt++) {
	record->sensor[cnt].temp = __be16_to_cpu(record->sensor[cnt].temp);
    }
    record->rain = __be16_to_cpu(record->rain);
    record->wind = __be16_to_cpu(record->wind);
    record->station.pressure = __be16_to_cpu(record->station.pressure);
    record->station.temp = __be16_to_cpu(record->station.temp);

    ret = WS_OK;

bye_recv:
    ws_recv_stop(&recv);

bye:
    return ret;
}


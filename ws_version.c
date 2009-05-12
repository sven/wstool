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

/*
 * ws_version
 *
 * Detect the weatherstation.
 */
int ws_version(void *version_void)
{
    int ret;
    struct receive_s recv;
    struct version_s *version = version_void;

    /* initialize receiving thread */
    recv.tries = 5;
    recv.buffer = (u_char*) version;
    recv.length = sizeof(struct version_s);
    recv.command = CMD_VERSION;
    check(ws_recv_init(&recv), goto bye);

    /* send version command */
    check(ws_send(CMD_VERSION), goto bye_recv);

    /* wait for data and end thread */
    sem_wait(recv.wait);

    /* check if data is valid */
    if (recv.tries > 0) {
	INFO("Received %i version bytes (tries left: %i).\n", recv.pos, recv.tries);
	ret = WS_OK;
    } else {
	ret = WS_ERR;
    }

bye_recv:
    ws_recv_stop(&recv);

bye:
    return ret;
}


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
 * ws_config_read
 *
 * Read configuration from station.
 */
int ws_config_read(void *conf_void)
{
    int ret;
    struct receive_s recv;
    struct config_s *conf = conf_void;

    /* initialize receiving thread */
    recv.tries = 10;
    recv.buffer = (u_char*) conf;
    recv.length = sizeof(struct config_s);
    recv.command = CMD_CONFIG_READ;
    
    check(ws_recv_init(&recv), goto bye);

    /* send config command */
    check(ws_send(CMD_CONFIG_READ), goto bye_recv);

    /* wait for data and end thread */
    sem_wait(recv.wait);

    /* check if data is valid */
    if (recv.tries < 1) {
	ret = WS_ERR;
	goto bye_recv;
    }

    conf->rainfall_mm = __be16_to_cpu(conf->rainfall_mm);
    ret = WS_OK;

bye_recv:
    ws_recv_stop(&recv);

bye:
    return ret;
}


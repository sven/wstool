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

#include <ftdi.h>
#include <stdio.h>
#include "ws.h"

/* define global variables */
extern struct ftdi_context ftdic;

/*
 * ws_send
 *
 * Send a command to the weatherstation.
 */
int ws_send(unsigned char ws_cmd)
{
    int ret;
    unsigned char cmd[] = {CMD_BEG, ws_cmd, CMD_END};

    /* purge all ftdi buffers */
    check_ftdi(ftdi_usb_purge_buffers(&ftdic), goto bye);

    /* send command */
    check_ftdi(ftdi_write_data(&ftdic, cmd, sizeof(cmd)), goto bye);

    usleep(100000);

    /* purge send buffer */
    check_ftdi(ftdi_usb_purge_tx_buffer(&ftdic), goto bye);

    ret = 0;
bye:
    return ret;
}


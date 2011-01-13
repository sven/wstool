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

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ftdi.h>
#include "ws.h"

/* define supported devices */
int usb_devs[][2] = {
    { 0x0403, 0xe0f6 },		/* ELV WS 300 PC */
    { 0x1b1f, 0xc006 },		/* ELV WS 300 PC-II */
    { 0x0403, 0xe0f7 },		/* Conrad WS 444 PC */
    { 0, 0 },			/* End Marker */
};

/* define global variables */
struct ftdi_context ftdic;

/*
 * ws_device_open
 *
 * Initialize the usb serial device.
 */
int ws_device_open(void)
{
    int cnt;
    int ret;
    int found = 0;

    /* initialize ftdi structure */
    check(ftdi_init(&ftdic), goto bye);
    
    /* set type to AM */
    ftdic.type = TYPE_AM;

    /* try to open a supported device */
    for (cnt = 0; usb_devs[cnt][0] != 0; cnt++) {
	INFO("Probing for device: 0x%0x:0x%0x.\n", usb_devs[cnt][0], usb_devs[cnt][1]);
	check_ftdi(ftdi_usb_open(&ftdic, usb_devs[cnt][0], usb_devs[cnt][1]), continue);
	found = 1;
	break;
    }

    /* if no device found, exit */
    if (!found) {
	ret = -1;
	goto bye_init;
    }

    /* reset device */
    check_ftdi(ftdi_usb_reset(&ftdic), goto bye_close);

    /* set baudrate */
    check_ftdi(ftdi_set_baudrate(&ftdic, 19200), goto bye_close);

    /* set line property */
    check_ftdi(ftdi_set_line_property(&ftdic, BITS_8, STOP_BIT_1, EVEN), goto bye_close);

    /* clear RTS */
    check_ftdi(ftdi_setrts(&ftdic, 0), goto bye_close);

    /* purge RX buffer */
    check_ftdi(ftdi_usb_purge_rx_buffer(&ftdic), goto bye_close);

    /* set latency timer */
    check_ftdi(ftdi_set_latency_timer(&ftdic, 16), goto bye_close);

    /* set flow control */
    check_ftdi(ftdi_setflowctrl(&ftdic, SIO_DISABLE_FLOW_CTRL), goto bye_close);

    return 0;

bye_close:
    ftdi_usb_close(&ftdic);

bye_init:
    ftdi_deinit(&ftdic);

bye:
    return ret;
}

/*
 * ws_device_close
 *
 * Close ws handler.
 */
void ws_device_close(void)
{
    /* close usb device */
    ftdi_usb_close(&ftdic);

    /* free ftdi structure */
    ftdi_deinit(&ftdic);
}


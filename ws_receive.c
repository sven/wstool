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
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "ws.h"

/* variables */
extern struct ftdi_context ftdic;
int thread_exit;
pthread_t ws_recv_thread_data;

/* prototypes */
void* ws_recv_thread(void*);

/*
 * ws_recv_init
 *
 * Initialize the receiving thread.
 */
int ws_recv_init(struct receive_s* recv)
{
    int ret;

    /* clear thread exit condition */
    thread_exit = 0;

    /* initialize semaphore */
    sem_unlink("ws_receive");
    recv->wait = sem_open("ws_receive", O_CREAT, 0660, 0);

    check_cond(pthread_create(&ws_recv_thread_data, NULL, ws_recv_thread, recv), != 0, return -1);

    return 0; 
}

/*
 * ws_recv_stop
 *
 * Stop the receiving thread.
 */
void ws_recv_stop(struct receive_s* recv)
{
    int ret;

    /* send exit signal to thread */
    thread_exit = 1;
    
    /* wait for thread to exit */
    check_cond(pthread_join(ws_recv_thread_data, NULL), != 0, return);

    /* close semaphore */
    sem_close(recv->wait);
}

/*
 * ws_recv_thread
 *
 * Receive data from weatherstation.
 */
void* ws_recv_thread(void* recv_void)
{
    int ret;
    int data_beg = 0;
    int data_end = 0;
    int data_last = 0;
    u_char data;
    struct receive_s* recv = recv_void;
    
    /* check for buffer with zero or less length */
    check_cond(recv->length, <= 0, goto bye);

    /* set buffer position to zero */
    recv->pos = 0;

    /* exit condition (pos <= lenght to have one byte extra for record end checking) */
    while ((!thread_exit) && (recv->pos <= (recv->length))) {

	/* receive one byte */
	check_ftdi(ftdi_read_data(&ftdic, &data, 1), break);

	/* no byte received? sleep a bit */
	if (ret == 0) {
	    if (recv->tries-- > 0) {		
		usleep(WS_RECV_TIMEOUT);
		continue;
	    }
	    break;
	}

	/* bytestream begin */
	if ((data == CMD_BEG) && (data_last != CMD_ESCAPE)) {
	    recv->pos = 0;
	    data_last = data;
	    continue;
	}

	/* command byte */
	if ((data == recv->command) && (data_last == CMD_BEG)) {
	    data_beg = 1;
	    data_last = 0;
	    continue;
	}

	/* bytestream end */
	if ((data == CMD_END) && (data_last != CMD_ESCAPE) && (data_beg)) {
	    data_end = 1;
	    break;
	}

	/* escape character handling
	 *
	 * if found and escape is already set, take char as it is,
	 * otherwise only set escape flag
	 */
	if ((data == CMD_ESCAPE) && (data_last != CMD_ESCAPE)) {
	    data_last = data;
	    continue;
	}

	/* add character to stream if this was not the "end-of-record" check */
	if (recv->pos >= recv->length) {
	    break;
	}
	recv->buffer[recv->pos++] = data;
    }

    /* if record end is not reached, invalidate data */
    if (!data_end) {
	recv->pos = 0;
    }

bye:
    /* release waiter */
    sem_post(recv->wait);

    return NULL;
}


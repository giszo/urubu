/* PS/2 keyboard & mouse driver.
 *
 * Copyright (c) 2013 Zoltan Kovacs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <libdevman/server/device.h>

#include <string.h>
#include <inttypes.h>

#include <urubu/ipc.h>
#include <urubu/irq.h>
#include <urubu/debug.h>

#include <arch/io.h>

#define PS2_IRQ 1

#define PS2_BUF_SIZE 1024

static int s_is_open = 0;

static char s_buffer[PS2_BUF_SIZE];
static size_t s_avail_cnt = 0;

static struct device* s_ps2_dev = NULL;

// =====================================================================================================================
static int ps2_open(struct device* dev)
{
    // allow only one endpoint to open the PS2 device
    if (s_is_open)
	return -1;

    if (irq_register(PS2_IRQ, libdevman_server_get_port()) != 0)
	return -1;

    s_is_open = 1;

    return 0;
}

// =====================================================================================================================
static void do_read_request(struct device_request* req)
{
    size_t s = req->size < s_avail_cnt ? req->size : s_avail_cnt;
    memcpy(req->data, s_buffer, s);
    memmove(req->data, req->data + s, s_avail_cnt - s);
    s_avail_cnt -= s;

    device_request_finished(req, (int)s);
}

// =====================================================================================================================
static int ps2_read(struct device* dev, struct device_request* req)
{
    // currently there is no data to be read
    if (s_avail_cnt == 0)
	return 0;

    do_read_request(req);

    return 0;
}

// =====================================================================================================================
static struct device_ops s_ps2_ops =
{
    .open = ps2_open,
    .read = ps2_read,
    .write = NULL
};

// =====================================================================================================================
static void ps2_handler(struct ipc_message* msg)
{
    switch (msg->data[0])
    {
	case 1 /* irq */ :
	    s_buffer[s_avail_cnt++] = inb(0x60);

	    if (s_ps2_dev->request)
		do_read_request(s_ps2_dev->request);

	    // finish the handling of the interrupt
	    irq_finished(msg->data[1] /* the IRQ number parameter */);

	    break;
    }
}

// =====================================================================================================================
int main(int argc, char** argv)
{
    libdevman_server_init();
    device_announce(INPUT, &s_ps2_ops, NULL, &s_ps2_dev);
    libdevman_server_run(ps2_handler);

    return 0;
}

/* Terminal driver.
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

#include "pty.h"

#include <libdevman/client/device.h>
#include <libdevman/client/async.h>
#include <libdevman/server/device.h>

#include <urubu/ipc.h>

int init_input();
int init_screen();

// =====================================================================================================================
static void do_create(struct ipc_message* msg)
{
    // create a new pty device
    struct device* dev = pty_create_device();

    struct ipc_message rep;

    if (dev)
    {
	rep.data[0] = 0;
	rep.data[1] = libdevman_server_get_port();
	rep.data[2] = dev->id;
    }
    else
	rep.data[0] = -1;

    ipc_port_send(msg->data[1], &rep);
}

// =====================================================================================================================
static void terminal_handler(struct ipc_message* msg)
{
    switch (msg->data[0])
    {
	case MSG_TERMINAL_CREATE :
	    do_create(msg);
	    break;

	default :
	    // let the async device manager library handle the rest
	    libdevman_client_async_process(msg);
	    break;
    }
}

// =====================================================================================================================
int main(int argc, char** argv)
{
    libdevman_server_init();
    libdevman_client_init();
    libdevman_client_async_init(libdevman_server_get_port());

    init_screen();
    init_input();
    init_pty();

    // register the terminal server
    ipc_server_register("terminal", libdevman_server_get_port());

    // run the mainloop
    libdevman_server_run(terminal_handler);

    return 0;
}

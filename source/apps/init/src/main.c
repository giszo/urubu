/* Init application.
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

#include <libdevman/client/device.h>

#include <urubu/ipc.h>

// =====================================================================================================================
int main(int argc, char** argv)
{
    libdevman_client_init();

    // wait for the terminal server to start
    int term = ipc_server_lookup("terminal", 1);

    int p = ipc_port_create();

    // request a new tty from the terminal
    struct ipc_message msg;
    msg.data[0] = MSG_TERMINAL_CREATE;
    msg.data[1] = p;
    ipc_port_send(term, &msg);

    struct ipc_message rep;
    ipc_port_receive(p, &rep);

    if (rep.data[0] != 0)
	return -1;

    struct device_info info;
    info.port = rep.data[1];
    info.id = rep.data[2];

    struct device_conn dev;
    device_open(&dev, &info);
    device_write(&dev, "hello from init", 15);

    while (1)
    {
	char c;
	device_read(&dev, &c, 1);
	device_write(&dev, &c, 1);
    }

    return 0;
}

/* Device manager library.
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

#include <libdevman/device.h>

#include <urubu/ipc.h>

// =====================================================================================================================
int device_announce(enum device_type type, int port)
{
    struct ipc_message msg;
    msg.data[0] = MSG_ANNOUNCE_DEVICE;
    msg.data[1] = type;
    msg.data[2] = port;

    return ipc_port_send_broadcast(IPC_BROADCAST_DEVICE, &msg);
}

// =====================================================================================================================
int device_lookup(enum device_type type, int wait, struct device_info* info)
{
    int p;
    struct ipc_message req;
    struct ipc_message rep;

    // create a port for getting the reply
    p = ipc_port_create();

    if (p < 0)
	return -1;

    req.data[0] = MSG_LOOKUP_DEVICE;
    req.data[1] = type;
    req.data[2] = wait;
    req.data[3] = p;

    if (ipc_port_send_broadcast(IPC_BROADCAST_DEVICE, &req) != 0)
	goto err;

    if (ipc_port_receive(p, &rep) != 0)
	goto err;

    ipc_port_delete(p);

    // check result code
    if (rep.data[0] != 0)
	return -1;

    // fill device information structure
    info->port = rep.data[1];

    return 0;

err:
    ipc_port_delete(p);
    return -1;
}

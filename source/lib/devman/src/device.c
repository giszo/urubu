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

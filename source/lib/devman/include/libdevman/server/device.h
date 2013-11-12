/* Device manager server library.
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

#ifndef _LIBDEVMAN_SERVER_DEVICE_H_
#define _LIBDEVMAN_SERVER_DEVICE_H_

#include <libdevman/type.h>

#include <stddef.h>

struct device_ops
{
    int (*read)(void*, void*, size_t);
    int (*write)(void*, const void*, size_t);
};

struct ipc_message;

typedef void msg_handler(struct ipc_message*);

/**
 * Announces a new device with the given type on the specified port.
 */
int device_announce(enum device_type type, struct device_ops* ops, void* data, int* id);

/**
 * Returns the port ID allocated by the library.
 */
int libdevman_server_get_port();

/**
 * The main loop of a device.
 */
int libdevman_server_run(msg_handler* handler);

/**
 * Initializes the device manager library.
 */
int libdevman_server_init();

#endif

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

#include <libslab/cache.h>

#include <stddef.h>

struct device_ops
{
    int (*read)(void* data, size_t size);
    int (*write)(const void* data, size_t size);
};

struct device
{
    int port;
    struct device_ops* ops;

    struct slab_cache conn_cache;
};

struct device_conn
{
    int id;
    // the ID of the shared memory region used for data transfer
    int shmem;
    size_t size;
    void* data;
    struct device* dev;
};

/**
 * Announces a new device with the given type on the specified port.
 */
int device_announce(struct device* dev, enum device_type type, int port, struct device_ops* ops);

/**
 * The main loop of a device.
 */
int device_run(struct device* dev);

/**
 * Initializes the device manager library.
 */
int libdevman_server_init();

#endif

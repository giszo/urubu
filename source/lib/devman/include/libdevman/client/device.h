/* Device manager client library.
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

#ifndef _LIBDEVMAN_CLIENT_DEVICE_H_
#define _LIBDEVMAN_CLIENT_DEVICE_H_

#include <libdevman/type.h>

#include <stddef.h>

struct device_info
{
    int port;
};

struct device
{
    // the target port of the device
    int port;
    // our own reply port
    int reply;
    // the id of the shared memory region
    int shmem;
    // the base address of the shared memory region
    void* data;
    // size of the shared memory region
    size_t size;
    unsigned long conn_id;
};

/**
 * Looks for the first device with the given type.
 */
int device_lookup(enum device_type type, int wait, struct device_info* info);

/**
 * Opens the device selected by the provided device information.
 */
int device_open(struct device* dev, struct device_info* info);

/**
 * Writes the given data to the opened device.
 */
int device_write(struct device* dev, const void* data, size_t size);

#endif

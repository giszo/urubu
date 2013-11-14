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

#include <libsupport/slab.h>

#include <string.h>

static struct slab_cache s_pty_cache;

static struct device* s_active_pty = NULL;

extern struct device_conn s_screen;

// =====================================================================================================================
void pty_input_available(char c)
{
    if (!s_active_pty || !s_active_pty->request)
	return;

    memcpy(s_active_pty->request->data, &c, 1);
    device_request_finished(s_active_pty->request, 1);
}

// =====================================================================================================================
static int pty_read(struct device* dev, struct device_request* req)
{
    return 0;
}

// =====================================================================================================================
static int pty_write(struct device* dev, struct device_request* req)
{
    // TODO: screen writing should be performed asynchronously
    device_write(&s_screen, req->data, req->size);
    device_request_finished(req, 0);
    return 0;
}

// =====================================================================================================================
static struct device_ops s_pty_ops =
{
    .open = NULL,
    .read = pty_read,
    .write = pty_write
};

// =====================================================================================================================
struct device* pty_create_device()
{
    struct pty_device* pty  = (struct pty_device*)slab_cache_alloc(&s_pty_cache);

    if (!pty)
	return NULL;

    // announce the two new device we just created now
    struct device* dev;
    device_announce(PTY, &s_pty_ops, (void*)pty, &dev);

    if (!s_active_pty)
	s_active_pty = dev;

    return dev;
}

// =====================================================================================================================
void init_pty()
{
    slab_cache_init(&s_pty_cache, sizeof(struct pty_device));
}

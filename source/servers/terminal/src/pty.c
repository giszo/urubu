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

#include <libsupport/slab.h>

static struct slab_cache s_pty_cache;

extern struct device s_screen;

// =====================================================================================================================
static int pty_write(void* p, const void* data, size_t size)
{
    device_write(&s_screen, data, size);
    return 0;
}

// =====================================================================================================================
struct device_ops s_pty_ops = {
    .read = NULL,
    .write = pty_write
};

// =====================================================================================================================
struct pty_device* pty_create_device()
{
    struct pty_device* pty  = (struct pty_device*)slab_cache_alloc(&s_pty_cache);

    if (!pty)
	return NULL;

    return pty;
}

// =====================================================================================================================
void pty_init()
{
    slab_cache_init(&s_pty_cache, sizeof(struct pty_device));
}

/* Messages for inter process communication.
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

#include <kernel/ipc/message.h>
#include <kernel/mm/slab.h>

static struct slab_cache s_message_cache;

// =====================================================================================================================
struct ipc_message* ipc_message_create()
{
    struct ipc_message* m = (struct ipc_message*)slab_cache_alloc(&s_message_cache);

    if (!m)
	return NULL;

    m->next = NULL;

    return m;
}

// =====================================================================================================================
void ipc_message_destroy(struct ipc_message* m)
{
    slab_cache_free(&s_message_cache, (void*)m);
}

// =====================================================================================================================
void ipc_message_init()
{
    slab_cache_init(&s_message_cache, sizeof(struct ipc_message));
}

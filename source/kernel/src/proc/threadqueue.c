/* Thread queue.
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

#include <kernel/proc/threadqueue.h>

// =====================================================================================================================
void threadqueue_add(struct threadqueue* queue, struct thread* t)
{
    t->sched_next = NULL;

    if (queue->last)
	queue->last->sched_next = t;

    queue->last = t;

    if (!queue->first)
	queue->first = t;
}

// =====================================================================================================================
struct thread* threadqueue_pop(struct threadqueue* queue)
{
    struct thread* t = queue->first;

    if (t)
	queue->first = t->sched_next;

    if (!queue->first)
	queue->last = NULL;

    return t;
}

// =====================================================================================================================
void threadqueue_init(struct threadqueue* queue)
{
    queue->first = NULL;
    queue->last = NULL;
}

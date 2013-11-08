/* Mutex implementation.
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

#include <kernel/sync/mutex.h>

// =====================================================================================================================
void mutex_lock(struct mutex* m)
{
    spinlock_disable(&m->lock);

    while (m->locker != NULL)
    {
	// put the current thread into the list of waiters
	threadqueue_add(&m->waiters, thread_current());
	spinunlock(&m->lock);
	// sleep until we have a chance to get the ownership of the lock
	thread_sleep();
	spinlock_disable(&m->lock);
    }

    // update the owner of the lock
    m->locker = thread_current();

    spinunlock_enable(&m->lock);
}

// =====================================================================================================================
void mutex_unlock(struct mutex* m)
{
    struct thread* t;

    spinlock_disable(&m->lock);

    // release the lock
    m->locker = NULL;
    // get the first waiter to wake it up
    t = threadqueue_pop(&m->waiters);

    spinunlock_enable(&m->lock);

    if (t)
	thread_wake_up(t);
}

// =====================================================================================================================
void mutex_init(struct mutex* m)
{
    spinlock_init(&m->lock, "mutex");
    m->locker = NULL;
    threadqueue_init(&m->waiters);
}

/* Scheduler.
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

#include <kernel/kernel.h>
#include <kernel/proc/sched.h>
#include <kernel/proc/threadqueue.h>

struct spinlock s_sched_lock = SPINLOCK_INIT("scheduler");

static struct threadqueue s_ready_queue;

// =====================================================================================================================
void sched_add_ready(struct thread* t)
{
    sched_lock();

    t->state = READY;
    threadqueue_add(&s_ready_queue, t);

    sched_unlock();
}

// =====================================================================================================================
struct thread* sched_next(struct cpu* curr_cpu, struct thread* curr)
{
    sched_lock();

    if (curr && curr != curr_cpu->thread_idle)
    {
	switch (curr->state)
	{
	    case RUNNING :
		sched_add_ready(curr);
		break;

	    case SLEEPING :
	    case DEAD :
		break;

	    default :
		panic("sched: invalid thread state (%d) while entering to the scheduler!\n", curr->state);
	}
    }

    struct thread* next = threadqueue_pop(&s_ready_queue);

    if (!next)
        next = curr_cpu->thread_idle;

    // Set the current thread on the CPU.
    curr_cpu->thread_current = next;
    next->state = RUNNING;

    sched_unlock();

    return next;
}

// =====================================================================================================================
static void sched_idle_thread(void* arg)
{
    while (1)
	cpu_arch_idle();
}

// =====================================================================================================================
void sched_init()
{
    sched_arch_init();

    struct cpu* curr_cpu = cpu_get_current();
    curr_cpu->thread_idle = thread_create_kernel("idle", sched_idle_thread, NULL);
}

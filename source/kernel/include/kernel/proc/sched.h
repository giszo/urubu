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

#ifndef _KERNEL_PROC_SCHED_H_
#define _KERNEL_PROC_SCHED_H_

#include <kernel/cpu/cpu.h>
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>
#include <kernel/sync/spinlock.h>

extern struct spinlock s_sched_lock;

#define sched_lock() spinlock_disable(&s_sched_lock)
#define sched_unlock() spinunlock_enable(&s_sched_lock)

void sched_arch_init();

void sched_add_ready(struct thread* t);

struct thread* sched_next(struct cpu* curr_cpu, struct thread* curr);

void sched_yield();

void sched_init();

#endif

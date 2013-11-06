/* Spinlock implementation.
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

#ifndef _KERNEL_SYNC_SPINLOCK_H_
#define _KERNEL_SYNC_SPINLOCK_H_

#include <kernel/cpu/atomic.h>

#define SPINLOCK_INIT(name) {0, 0, name}

struct spinlock
{
    atomic_t locked;
    volatile int int_state;
    const char* name;
};

void spinunlock(struct spinlock* sl);

void spinlock_disable(struct spinlock* sl);
void spinunlock_enable(struct spinlock* sl);

void spinlock_init(struct spinlock* sl, const char* name);

#endif

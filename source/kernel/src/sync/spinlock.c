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

#include <kernel/cpu/cpu.h>
#include <kernel/cpu/atomic.h>
#include <kernel/sync/spinlock.h>

// =====================================================================================================================
void spinunlock(struct spinlock* sl)
{
    atomic_set(&sl->locked, 0);
}

// =====================================================================================================================
void spinlock_disable(struct spinlock* sl)
{
    int ints = cpu_arch_disable_int();
    while (atomic_swap(&sl->locked, 1) == 1) ;
    sl->int_state = ints;
}

// =====================================================================================================================
void spinunlock_enable(struct spinlock* sl)
{
    int is = sl->int_state;
    atomic_set(&sl->locked, 0);
    if (is)
	cpu_arch_enable_int();
}

// =====================================================================================================================
void spinlock_init(struct spinlock* sl, const char* name)
{
    atomic_set(&sl->locked, 0);
    sl->int_state = 0;
    sl->name = name;
}

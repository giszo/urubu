/* Thread handling.
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

#include <urubu/thread.h>

#include <syscall.h>

// =====================================================================================================================
void thread_exit(int code)
{
    syscall1(SYS_thread_exit, code);
}

// =====================================================================================================================
void thread_get_statistics(struct thread_stat* stat)
{
    syscall1(SYS_thread_get_statistics, (unsigned long)stat);
    memory_barrier();
}

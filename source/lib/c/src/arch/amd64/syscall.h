/* amd64 specific system call handling for userspace.
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

#ifndef SYSCALL_AMD64_SYSCALL_H_INCLUDED
#define SYSCALL_AMD64_SYSCALL_H_INCLUDED

#include "syscall_table.h"

#define memory_barrier() __asm__ __volatile__("" ::: "memory");

static inline unsigned long syscall0(unsigned long num)
{
    register unsigned long res;
    __asm__ __volatile__("int $0x80" : "=a" (res) : "a" (num));
    return res;
}

static inline unsigned long syscall1(unsigned long num, unsigned long p1)
{
    register unsigned long res;
    __asm__ __volatile__("int $0x80" : "=a" (res) : "a" (num), "b" (p1));
    return res;
}

static inline unsigned long syscall2(unsigned long num, unsigned long p1, unsigned long p2)
{
    register unsigned long res;
    __asm__ __volatile__("int $0x80" : "=a" (res) : "a" (num), "b" (p1), "c" (p2));
    return res;
}

static inline unsigned long syscall3(unsigned long num, unsigned long p1, unsigned long p2, unsigned long p3)
{
    register unsigned long res;
    __asm__ __volatile__("int $0x80" : "=a" (res) : "a" (num), "b" (p1), "c" (p2), "d" (p3));
    return res;
}

#endif

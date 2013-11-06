/* amd64 specific I/O functions.
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

#ifndef _X86_COMMON_IO_H_
#define _X86_COMMON_IO_H_

#include <kernel/types.h>

static inline uint8_t inb(uint16_t port)
{
    register uint8_t value;

    __asm__ __volatile__(
	"inb %1, %0\n"
	: "=a" (value)
	: "dN" (port)
    );

    return value;
}

static inline void outb(uint8_t data, uint16_t port)
{
    __asm__ __volatile__(
	"outb %1, %0\n"
	:
	: "dN" (port), "a" (data)
    );
}

#endif

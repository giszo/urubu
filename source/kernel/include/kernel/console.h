/* Generic console interface definition.
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

#ifndef _KERNEL_CONSOLE_H_
#define _KERNEL_CONSOLE_H_

#include <kernel/lib/stdarg.h>

struct console
{
    void (*clear)();
    void (*write)(char c);
};

void console_set_screen(struct console* c);
void console_set_debug(struct console* c);

void kprintf(const char* format, ...);
void kprintf_unlocked(const char* format, ...);
void kvprintf_unlocked(const char* format, va_list args);

void dprintf(const char* format, ...);

#endif

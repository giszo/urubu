/* Kernel panic handler.
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
#include <kernel/console.h>
#include <kernel/cpu/cpu.h>

// =====================================================================================================================
void __panic_handler(const char* file, int line, const char* format, ...)
{
    va_list args;

    // disable interrupts to avoid a further task switching ...
    cpu_arch_disable_int();

    // TODO: other CPUs in an SMP system need to be halted too

    kprintf_unlocked("Kernel panic at %s:%d:\n", file, line);
    va_start(args, format);
    kvprintf_unlocked(format, args);
    va_end(args);

    while (1) ;
}

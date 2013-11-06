/* Serial debug console.
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

#include <kernel/console.h>

#include <arch/io.h>

#include <pc/serial.h>

// =====================================================================================================================
static void pc_serial_write(char c)
{
    while ((inb(0x3f8 + 5) & 0x20) == 0)
	;

    outb(c, 0x3f8);
}

// =====================================================================================================================
static struct console pc_serial =
{
    .clear = NULL,
    .write = pc_serial_write
};

// =====================================================================================================================
void pc_serial_init()
{
    outb(0x00, 0x3f8 + 1);
    outb(0x80, 0x3f8 + 3);
    outb(0x03, 0x3f8);
    outb(0x00, 0x3f8 + 1);
    outb(0x03, 0x3f8 + 3);
    outb(0xc7, 0x3f8 + 2);
    outb(0x0b, 0x3f8 + 4);

    // register the serial debug console
    console_set_debug(&pc_serial);
}

/* Screen handling functions.
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
#include <kernel/lib/string.h>

#include <arch/io.h>

#include <pc/screen.h>

static ptr_t s_addr;
static unsigned s_x = 0;
static unsigned s_y = 0;

// =====================================================================================================================
static void update_cursor()
{
    unsigned tmp = s_y * 80 + s_x;

    // Write the new cursor position to the VGA registers.
    outb(14, 0x3d4);
    outb(tmp >> 8, 0x3d5);
    outb(15, 0x3d4);
    outb(tmp, 0x3d5);
}

// =====================================================================================================================
static void scroll_up()
{
    memmove((void*)s_addr, (void*)(s_addr + 80 * 2), 80 * 24 * 2);
    memsetl((void*)(s_addr + 80 * 24 * 2), 0x07200720, 80 / 2);

    s_y--;
}

// =====================================================================================================================
static void pc_screen_clear()
{
    memsetl((void*)s_addr, 0x07200720, 80 * 25 / 2);

    s_x = 0;
    s_y = 0;

    update_cursor();
}

// =====================================================================================================================
static void pc_screen_write(char c)
{
    switch (c)
    {
	case '\r' :
	    s_x = 0;
	    break;

	case '\n' :
	    s_x = 0;
	    s_y++;
	    break;

	default :
	{
	    char* p = (char*)s_addr + (s_y * 80 + s_x) * 2;
	    *p = c;
	    s_x++;
	    break;
	}
    }

    if (s_x == 80)
    {
        s_x = 0;
	s_y++;
    }

    if (s_y == 25)
	scroll_up();

    update_cursor();
}

// =====================================================================================================================
static struct console pc_screen =
{
    .clear = pc_screen_clear,
    .write = pc_screen_write
};

// =====================================================================================================================
void pc_screen_init(ptr_t addr)
{
    s_addr = addr;
    console_set_screen(&pc_screen);
}

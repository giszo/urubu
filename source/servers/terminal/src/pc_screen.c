/* Terminal driver.
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

#include <stdint.h>
#include <string.h>

#include <arch/io.h>

#include <urubu/mm.h>

#include "pc_screen.h"
#include "screen.h"

#define WIDTH  80
#define HEIGHT 25

static void* s_base = NULL;

static unsigned s_x = 0;
static unsigned s_y = 0;

// =====================================================================================================================
static inline void update_cursor()
{
    unsigned tmp = s_y * 80 + s_x;

    // Write the new cursor position to the VGA registers.
    outb(14, 0x3d4);
    outb(tmp >> 8, 0x3d5);
    outb(15, 0x3d4);
    outb(tmp, 0x3d5);
}

// =====================================================================================================================
static inline void clear(unsigned offset, unsigned count)
{
    uint8_t* p = (uint8_t*)s_base + offset * 2 /* a character contains an attribute byte too */;

    for (unsigned i = 0; i < count; ++i)
    {
	*p++ = ' ';
	*p++ = 7;
    }
}

// =====================================================================================================================
static inline void scroll_up()
{
    memmove(s_base, (uint8_t*)(s_base + WIDTH * 2), WIDTH * (HEIGHT - 1) * 2);
    clear(WIDTH * (HEIGHT - 1), WIDTH);
    s_y--;
}

// =====================================================================================================================
static void pc_screen_clear()
{
    clear(0, WIDTH * HEIGHT);
    s_x = 0;
    s_y = 0;
    update_cursor();
}

// =====================================================================================================================
static void pc_screen_putc(char c)
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
	    char* p = (char*)s_base + (s_y * WIDTH + s_x) * 2;
	    *p = c;
	    s_x++;
	    break;
	}
    }

    if (s_x == WIDTH)
    {
        s_x = 0;
	s_y++;
    }

    if (s_y == HEIGHT)
	scroll_up();

    update_cursor();
}

// =====================================================================================================================
static struct screen s_pc_screen =
{
    .clear = pc_screen_clear,
    .putc = pc_screen_putc
};

// =====================================================================================================================
int pc_screen_install()
{
    s_base = mm_map((void*)0xb8000, 0x1000);

    if (!s_base)
	return -1;

    screen_install(&s_pc_screen);

    return 0;
}

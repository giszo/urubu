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

#include <stdio.h>

#include <urubu/mm.h>
#include <urubu/debug.h>

#include "screen.h"
#include "pc_screen.h"

static struct screen* s_screen = NULL;

// =====================================================================================================================
void screen_install(struct screen* s)
{
    s_screen = s;
}

// =====================================================================================================================
void screen_printf(const char* fmt, ...)
{
    va_list args;
    char buf[512];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    for (unsigned i = 0; buf[i]; ++i)
	s_screen->putc(buf[i]);
}

// =====================================================================================================================
int main(int argc, char** argv)
{
    // install the PC screen backend
    if (pc_screen_install() != 0)
    {
	dbprintf("terminal: unable to install screen backend!\n");
	return -1;
    }

    // clear the screen now ...
    s_screen->clear();

    // display some kind of system statistics for now on the screen ...
    struct mm_phys_stat ps;
    mm_get_phys_stat(&ps);

    screen_printf("System memory: %llu bytes\n", ps.memory_size);
    screen_printf("Used memory: %llu bytes\n", ps.used_size);

    return 0;
}

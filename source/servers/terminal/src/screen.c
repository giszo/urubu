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

#include <libdevman/client/device.h>

#include <urubu/thread.h>
#include <urubu/mm.h>

#include <stdio.h>

struct device_conn s_screen;

// =====================================================================================================================
static void screen_printf(const char* fmt, ...)
{
    va_list args;
    char buf[512];

    va_start(args, fmt);
    int r = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    device_write(&s_screen, buf, r);
}

// =====================================================================================================================
int init_screen()
{
    struct device_info info;

    if (device_lookup(SCREEN, 1, &info) != 0)
	return -1;
    if (device_open(&s_screen, &info) != 0)
	return -1;

    // display some kind of system statistics for now on the screen ...
    struct mm_phys_stat ps;
    mm_get_phys_stat(&ps);
    screen_printf("System memory: %llu bytes\n", ps.memory_size);
    screen_printf("Used memory: %llu bytes\n", ps.used_size);
    struct thread_stat ts;
    thread_get_statistics(&ts);
    screen_printf("Number of threads: %llu\n", ts.number);

    return 0;
}

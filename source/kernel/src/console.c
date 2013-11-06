/* Kernel console handling.
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
#include <kernel/types.h>
#include <kernel/sync/spinlock.h>
#include <kernel/lib/printf.h>

static struct console* s_screen = NULL;
static struct console* s_debug = NULL;

static struct spinlock s_console_lock = SPINLOCK_INIT("console");

// =====================================================================================================================
void console_set_screen(struct console* c)
{
    if (c->clear)
	c->clear();

    s_screen = c;
}

// =====================================================================================================================
void console_set_debug(struct console* c)
{
    if (c->clear)
	c->clear();

    s_debug = c;
}

// =====================================================================================================================
static int kprintf_helper(void* data, char c)
{
    if (s_screen)
	s_screen->write(c);

    if (s_debug)
	s_debug->write(c);

    return 0;
}

// =====================================================================================================================
static int dprintf_helper(void* data, char c)
{
    if (s_debug)
	s_debug->write(c);

    return 0;
}

// =====================================================================================================================
void kprintf(const char* format, ...)
{
    va_list args;

    spinlock_disable(&s_console_lock);

    va_start(args, format);
    do_printf(kprintf_helper, NULL, format, args);
    va_end(args);

    spinunlock_enable(&s_console_lock);
}

// =====================================================================================================================
void kprintf_unlocked(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    do_printf(kprintf_helper, NULL, format, args);
    va_end(args);
}

// =====================================================================================================================
void kvprintf_unlocked(const char* format, va_list args)
{
    do_printf(kprintf_helper, NULL, format, args);
}

// =====================================================================================================================
void dprintf(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    do_printf(dprintf_helper, NULL, format, args);
    va_end(args);
}

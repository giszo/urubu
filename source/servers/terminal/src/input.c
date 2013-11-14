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

#include "pty.h"

#include <libdevman/client/async.h>

static struct device_conn s_input;

static uint8_t s_keycode = 0;
static uint8_t s_last_keycode = 0;

enum {
    KEY_TAB = 7,
    KEY_BACKSPACE = 8,
    KEY_ENTER = 13,
    KEY_ESCAPE = 27,
};

static char s_keymap[128] =
{
    /* 0-9 */ 0, KEY_ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8',
    /* 10-19 */ '9', '0', '-', '=', KEY_BACKSPACE, KEY_TAB, 'q', 'w', 'e', 'r',
    /* 20-29 */ 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
    /* 30-39 */ 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    /* 40-49 */ '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    /* 50-59 */ 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,
    /* 60-69 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 70-79 */ 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    /* 80-89 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 90-99 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 100-109 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 110-119 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 120-127 */ 0, 0, 0, 0, 0, 0, 0, 0
};

// =====================================================================================================================
static void input_read_cb(int ret, void* p)
{
    int up = s_keycode & 0x80;

    if (s_last_keycode == 0xe0)
    {
	// this is an extended key ...
    }
    else
    {
	char c = s_keymap[s_keycode & 0x7f];

	if (!up)
	    pty_input_available(c);
    }

    s_last_keycode = s_keycode;
    device_read_async(&s_input, &s_keycode, 1, input_read_cb, NULL);
}

// =====================================================================================================================
int init_input()
{
    struct device_info info;

    if (device_lookup(INPUT, 1, &info) != 0)
	return -1;
    if (device_open(&s_input, &info) != 0)
	return -1;

    // start reading input
    device_read_async(&s_input, &s_keycode, 1, input_read_cb, NULL);

    return 0;
}

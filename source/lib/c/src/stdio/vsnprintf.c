/* Urubu C library.
 *
 * Copyright (c) 2012 Zoltan Kovacs
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
#include <stdarg.h>

#include "_printf.h"

// =====================================================================================================================
int vsnprintf(char *str, size_t size, const char* format, va_list args)
{
    struct snprintf_data data;
    data.buf = str;
    data.size = 0;
    data.max_size = size;

    _printf_helper(_snprintf_out_helper, &data, format, args);

    // make space for the '\0' character at the end of the buffer if it is full
    if (data.size == data.max_size)
	data.size--;

    data.buf[data.size] = 0;

    return 0;
}

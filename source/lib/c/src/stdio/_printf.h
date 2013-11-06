/* Urubu C library.
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

#ifndef STDIO__PRINTF_H_INCLUDED
#define STDIO__PRINTF_H_INCLUDED

#include <stddef.h>
#include <stdarg.h>

typedef struct snprintf_data snprintf_data_t;

struct snprintf_data
{
    char* buf;
    size_t size;
    size_t max_size;
};

int _snprintf_out_helper(void* _data, char c);

typedef int _printf_output_t(void* data, char c);

void _printf_helper(_printf_output_t* out, void* data, const char* fmt, va_list args);

#endif

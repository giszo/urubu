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

#include <stdint.h>

#include "_printf.h"

enum printf_state
{
    NORMAL,
    FORMAT
};

enum printf_flags
{
    NEGATIVE = (1 << 0),
    LONG = (1 << 1),
    LONG_LONG = (1 << 2)
};

// =====================================================================================================================
int _snprintf_out_helper(void* _data, char c)
{
    snprintf_data_t* data = (snprintf_data_t*)_data;

    if (data->size == data->max_size)
	return 0;

    data->buf[data->size++] = c;

    return 0;
}

// =====================================================================================================================
static inline void _printf_string(_printf_output_t* out, void* data, const char* s)
{
    while (*s)
	out(data, *s++);
}

// =====================================================================================================================
static inline void _printf_number(_printf_output_t* out, void* data, uint64_t n, unsigned base, unsigned flags)
{
    char b[32];
    unsigned i = sizeof(b);
    const char* digits = "0123456789abcdef";

    b[--i] = 0;

    while (n != 0)
    {
	b[--i] = digits[n % base];
	n /= base;
    }

    if (i == sizeof(b) - 1)
	b[--i] = '0';

    if (flags & NEGATIVE)
	b[--i] = '-';

    _printf_string(out, data, &b[i]);
}

// =====================================================================================================================
void _printf_helper(_printf_output_t* out, void* data, const char* fmt, va_list args)
{
    char ch;
    unsigned flags = 0;
    enum printf_state state = NORMAL;

    for (; (ch = *fmt); ++fmt)
    {
	switch (state)
	{
	    case NORMAL :
		if (ch == '%')
		{
		    state = FORMAT;
		    flags = 0;
		}
		else
		    out(data, ch);
		break;

	    case FORMAT :
		switch (ch)
		{
		    case 'd' :
		    {
			int64_t n;

			if (flags & LONG)
			    n = va_arg(args, long);
			else if (flags & LONG_LONG)
			    n = va_arg(args, long long);
			else
			    n = va_arg(args, int);

			if (n < 0)
			{
			    n = -n;
			    flags |= NEGATIVE;
			}

			_printf_number(out, data, n, 10, flags);
			state = NORMAL;
			break;
		    }

		    case 'x' :
		    case 'u' :
		    {
			uint64_t n;

			if (flags & LONG)
			    n = va_arg(args, unsigned long);
			else if (flags & LONG_LONG)
			    n = va_arg(args, unsigned long long);
			else
			    n = va_arg(args, unsigned);

			_printf_number(out, data, n, (ch == 'x') ? 16 : 10, 0);
			state = NORMAL;
			break;
		    }

		    case 's' :
		    {
			char* s = va_arg(args, char*);
			_printf_string(out, data, s);
			state = NORMAL;
		    }

		    case 'l' :
			if (flags & LONG)
			{
			    flags &= ~LONG;
			    flags |= LONG_LONG;
			}
			else if (flags & LONG_LONG)
			{ /* this should be long enough ;) */ }
			else
			    flags |= LONG;
			break;

		    default :
			state = NORMAL;
			break;
		}
		break;
	}
    }
}

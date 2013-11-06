/* printf implementation.
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

#include <kernel/types.h>
#include <kernel/lib/printf.h>

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
static inline void _printf_string(printf_helper* helper, void* data, const char* s)
{
    while (*s)
	helper(data, *s++);
}

// =====================================================================================================================
static inline void _printf_number(printf_helper* helper, void* data, uint64_t n, unsigned base, unsigned flags)
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

    _printf_string(helper, data, &b[i]);
}

// =====================================================================================================================
void do_printf(printf_helper* helper, void* data, const char* format, va_list args)
{
    char ch;
    unsigned flags = 0;
    enum printf_state state = NORMAL;

    for (; (ch = *format) != 0; ++format)
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
		    helper(data, ch);
		break;

	    case FORMAT :
		switch (ch)
		{
		    case 's' :
		    {
			char* s = va_arg(args, char*);
			_printf_string(helper, data, s);
			state = NORMAL;
			break;
		    }

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

			_printf_number(helper, data, n, 10, flags);
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

			_printf_number(helper, data, n, (ch == 'x') ? 16 : 10, 0);
			state = NORMAL;
			break;
		    }

		    case 'p' :
		    {
			ptr_t p = va_arg(args, ptr_t);
			_printf_number(helper, data, p, 16, 0);
			state = NORMAL;
			break;
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

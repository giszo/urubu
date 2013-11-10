/* Memory and string handling functions.
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

#include <kernel/lib/string.h>

// =====================================================================================================================
void* memcpy(void* d, const void* s, size_t n)
{
    char* dest = (char*)d;
    char* src = (char*)s;

    while (n--)
	*dest++ = *src++;

    return d;
}

// =====================================================================================================================
void* memmove(void* dest, const void* src, size_t n)
{
    char* d;
    char* s;

    if (dest < src)
    {
	d = (char*)dest;
	s = (char*)src;

	while (n--)
	    *d++ = *s++;
    }
    else
    {
	d = (char*)dest + n;
	s = (char*)src + n;

	while (n--)
	    *--d = *--s;
    }

    return dest;
}

// =====================================================================================================================
void memset(void* s, int c, size_t n)
{
    char* p = (char*)s;

    while (n--)
	*p++ = c;
}

// =====================================================================================================================
void memsetl(void* s, int c, size_t n)
{
    int* p = (int*)s;

    while (n--)
	*p++ = c;
}

// =====================================================================================================================
size_t strlen(const char* s)
{
    size_t r = 0;
    for (; *s++; r++) ;
    return r;
}

// =====================================================================================================================
char* strrchr(const char* s, int c)
{
    const char* e = s + strlen(s);

    do
    {
	if (*e == (char)c)
	    return (char*)e;
    } while (--e >= s);

    return NULL;
}

// =====================================================================================================================
char* strncpy(char* d, const char* s, size_t n)
{
    size_t i;

    for (i = 0; i < n && s[i]; ++i)
	d[i] = s[i];
    for (; i < n; ++i)
	d[i] = 0;

    return d;
}

// =====================================================================================================================
int strcmp(const char* s1, const char* s2)
{
    int r;

    while (1)
    {
        r = *s1 - *s2++;

        if ((r != 0) || (*s1++ == 0))
            break;
    }

    return r;
}

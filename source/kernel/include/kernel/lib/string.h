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

#ifndef _KERNEL_LIB_STRING_H_
#define _KERNEL_LIB_STRING_H_

#include <kernel/types.h>

void* memcpy(void* d, const void* s, size_t n);
void* memmove(void* d, const void* s, size_t n);

void memset(void* s, int c, size_t n);
void memsetl(void* s, int c, size_t n);

size_t strlen(const char* s);
char* strrchr(const char* s, int c);
char* strncpy(char* d, const char* s, size_t n);

#endif

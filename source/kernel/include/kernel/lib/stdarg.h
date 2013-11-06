/* stdarg macro definitions.
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

#ifndef _KERNEL_LIB_STDARG_H_
#define _KERNEL_LIB_STDARG_H_

typedef __builtin_va_list va_list;

#define va_start(a, b) __builtin_va_start(a, b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a, b)   __builtin_va_arg(a, b)

#endif

/* amd64 paging definitions.
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

#ifndef _AMD64_MM_PAGING_H_
#define _AMD64_MM_PAGING_H_

#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITE   (1 << 1)
#define PAGE_USER    (1 << 2)
#define PAGE_2MB     (1 << 7)

#define PML4_SHIFT 39
#define PDP_SHIFT  30
#define PD_SHIFT   21
#define PT_SHIFT   12

#define PML4_INDEX(a) (((a) >> PML4_SHIFT) & 0x1ff)
#define PDP_INDEX(a)  (((a) >> PDP_SHIFT) & 0x1ff)
#define PD_INDEX(a)   (((a) >> PD_SHIFT) & 0x1ff)
#define PT_INDEX(a)   (((a) >> PT_SHIFT) & 0x1ff)

#endif

/* amd64 virtual memory manager.
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

#ifndef _AMD64_MM_VMM_H_INCLUDED_
#define _AMD64_MM_VMM_H_INCLUDED_

#include <kernel/types.h>

#include <arch/proc/process.h>

void vmm_arch_init_process(struct amd64_process* p);

void vmm_arch_pre_init();
void vmm_arch_init(size_t mem_size);

#endif

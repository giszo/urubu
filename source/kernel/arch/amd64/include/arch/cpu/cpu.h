/* amd64 CPU definitions.
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

#ifndef _AMD64_CPU_CPU_H_
#define _AMD64_CPU_CPU_H_

#define CR0_PG (1 << 31)

#define CR4_PAE (1 << 5)

#define EFLAG_IF    (1 << 9)
#define EFLAG_IOPL ((1 << 12) | (1 << 13))
#define EFLAG_ID    (1 << 21)

#define MSR_EFER 0xc0000080

#define EFER_LME (1 << 8)

#ifndef __ASSEMBLER__

#include <arch/cpu/tss.h>

struct amd64_cpu
{
    struct amd64_tss tss;
};

void cpu_flush_cr3();
void cpu_set_cr3(uint64_t cr3);

void cpu_arch_init();

#endif /* __ASSEMBLER__ */

#endif

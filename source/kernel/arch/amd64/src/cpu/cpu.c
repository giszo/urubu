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

#include <kernel/cpu/cpu.h>
#include <kernel/lib/string.h>

#include <arch/cpu/cpu.h>
#include <arch/cpu/gdt.h>

static struct amd64_cpu s_arch_cpu;

// =====================================================================================================================
void cpu_arch_init()
{
    struct cpu* c = cpu_get_current();
    c->arch_data = (void*)&s_arch_cpu;

    // simply clear the TSS for now, the scheduler will update the required parts ...
    struct amd64_tss* tss = &s_arch_cpu.tss;
    memset(tss, 0, sizeof(struct amd64_tss));

    // setup the GDT entry for the TSS
    gdt_arch_set_tss(tss);

    // set the TSS descriptor on the CPU
    __asm__ __volatile__(
	"ltr %%ax\n"
	:
	: "a" (5 * 8)
    );
}

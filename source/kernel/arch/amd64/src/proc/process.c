/* amd64 specific process handling.
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

#include <kernel/proc/process.h>
#include <kernel/mm/slab.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/lib/string.h>

#include <arch/proc/process.h>
#include <arch/mm/vmm.h>
#include <arch/mm/config.h>

static struct slab_cache s_arch_proc_cache;

// =====================================================================================================================
int process_arch_create(struct process* p)
{
    struct amd64_process* arch_p = (struct amd64_process*)slab_cache_alloc(&s_arch_proc_cache);

    if (!arch_p)
       goto err1;

    arch_p->cr3 = pmm_alloc();

    if (!arch_p->cr3)
       goto err2;

    memsetl((void*)vmm_provide_phys(arch_p->cr3), 0, PAGE_SIZE / 4);

    // Initialize the new address space
    vmm_arch_init_process(arch_p);

    p->arch_data = (void*)arch_p;

    return 0;

err2:
    slab_cache_free(&s_arch_proc_cache, (void*)arch_p);
err1:
    return -1;
}

// =====================================================================================================================
void process_arch_destroy(struct process* p)
{
    struct amd64_process* arch_p = (struct amd64_process*)p->arch_data;

    // TODO: clean up the WHOLE virtual address space ... :-[
    slab_cache_free(&s_arch_proc_cache, (void*)arch_p);
}

// =====================================================================================================================
void process_arch_init()
{
    slab_cache_init(&s_arch_proc_cache, sizeof(struct amd64_process));
}

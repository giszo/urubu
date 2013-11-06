/* amd64 physical memory manager.
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

#include <arch/mm/pmm.h>
#include <arch/mm/config.h>

#define INITIAL_ENTRIES 32
static size_t _used_entries = 0;
static struct memory_map_entry _entry_cache[INITIAL_ENTRIES];

size_t s_memory_size = 0;
static struct memory_map s_arch_free_map;

extern int __kernel_end;

// =====================================================================================================================
static struct memory_map_entry* pmm_arch_early_entry_alloc()
{
    if (_used_entries == INITIAL_ENTRIES)
	return NULL;

    return &_entry_cache[_used_entries++];
}

// =====================================================================================================================
static void pmm_arch_early_entry_free(struct memory_map_entry* e)
{
}

// =====================================================================================================================
static struct memory_map_allocator s_pmm_early_map_allocator =
{
    .alloc = pmm_arch_early_entry_alloc,
    .free = pmm_arch_early_entry_free
};

// =====================================================================================================================
struct memory_map* pmm_arch_build_free_map(struct mb_info* info)
{
    memory_map_init(&s_arch_free_map, &s_pmm_early_map_allocator);

    // Calculate the size of the system memory.
    s_memory_size = info->mem_upper * 1024 + 1024 * 1024;

    // Build the list of available free regions.
    uint8_t* data = (uint8_t*)(uint64_t)info->mmap_info.addr;
    uint8_t* end = data + info->mmap_info.length;

    while (data < end)
    {
	struct mb_mmap_entry* entry = (struct mb_mmap_entry*)data;

	switch (entry->type)
	{
	    case MEM_AVAILABLE :
		memory_map_add(&s_arch_free_map, entry->base, entry->length);
		break;
	}

	data += entry->size + 4;
    }

    // Reserve already known used memory regions:
    //   - the first 1Mb of physical memory
    //   - the kernel binary
    //   - the modules loaded by GRUB
    memory_map_exclude(&s_arch_free_map, 0, 0x100000);
    memory_map_exclude(&s_arch_free_map, KERNEL_PHYS_BASE, MM_V2P((ptr_t)&__kernel_end) - KERNEL_PHYS_BASE);

    struct mb_mod_entry* mods = (struct mb_mod_entry*)(uint64_t)info->mod_info.addr;
    for (size_t i = 0; i < info->mod_info.length; ++i)
    {
	struct mb_mod_entry* mod = &mods[i];
	memory_map_exclude(&s_arch_free_map, mod->start, PAGE_ALIGN(mod->end) - mod->start);
    }

    return &s_arch_free_map;
}

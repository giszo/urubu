/* amd64 specific thread handling.
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

#include <kernel/proc/thread.h>
#include <kernel/mm/slab.h>
#include <kernel/lib/string.h>

#include <arch/cpu/gdt.h>
#include <arch/cpu/cpu.h>
#include <arch/proc/thread.h>
#include <arch/mm/config.h>

static struct slab_cache s_arch_thread_cache;

// =====================================================================================================================
int thread_arch_create(struct thread* t)
{
    struct amd64_thread* arch_t = (struct amd64_thread*)slab_cache_alloc(&s_arch_thread_cache);

    if (!arch_t)
	return -1;

    arch_t->rsp = 0;

    t->arch_data = (void*)arch_t;

    return 0;
}

// =====================================================================================================================
void thread_arch_destroy(struct thread* t)
{
    slab_cache_free(&s_arch_thread_cache, t->arch_data);
}

// =====================================================================================================================
int thread_arch_create_kernel(struct thread* t, void* entry, void* arg)
{
    ptr_t p = (ptr_t)t->kernel_stack + t->kernel_stack_size;

    // allocate space for the return address on the stack
    p -= sizeof(uint64_t);
    ((uint64_t*)p)[0] = (uint64_t)thread_kernel_exit;

    // allocate an IRQ context on the stack
    p -= sizeof(struct irq_context);
    struct irq_context* ctx = (struct irq_context*)p;

    // initialize the IRQ context
    memset(ctx, 0, sizeof(struct irq_context));
    ctx->rdi = (uint64_t)arg;
    ctx->cs = CS_KERNEL;
    ctx->ip = (uint64_t)entry;
    ctx->rflags = EFLAG_IF | EFLAG_IOPL;
    ctx->rsp = p + sizeof(struct irq_context);
    ctx->ss = DS_KERNEL;

    struct amd64_thread* arch_t = (struct amd64_thread*)t->arch_data;
    arch_t->rsp = p;

    return 0;
}

// =====================================================================================================================
int thread_arch_create_user(struct thread* t, void* entry, void* arg)
{
    ptr_t p = (ptr_t)t->kernel_stack + t->kernel_stack_size;

    // allocate space for the IRQ context
    p -= sizeof(struct irq_context);
    struct irq_context* ctx = (struct irq_context*)p;

    // initialize the IRQ context
    memset(ctx, 0, sizeof(struct irq_context));
    ctx->rdi = (uint64_t)arg;
    ctx->cs = CS_USER | 3;
    ctx->ip = (uint64_t)entry;
    ctx->rflags = EFLAG_IF | EFLAG_IOPL;
    ctx->rsp = (ptr_t)t->user_stack + t->user_stack_size;
    ctx->ss = DS_USER | 3;

    struct amd64_thread* arch_t = (struct amd64_thread*)t->arch_data;
    arch_t->rsp = (uint64_t)ctx;

    return 0;
}

// =====================================================================================================================
void thread_arch_init()
{
    slab_cache_init(&s_arch_thread_cache, sizeof(struct amd64_thread));
}

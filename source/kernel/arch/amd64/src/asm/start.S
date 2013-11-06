/* Entry point of amd64 kernel.
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

#include <arch/cpu/cpu.h>
#include <arch/mm/config.h>
#include <arch/mm/paging.h>

#include <pc/multiboot.h>

.global _start
.global _vmm_kpml4
.global _vmm_kpdp
.global _vmm_kpd
.global _vmm_phys_pdp
.global _vmm_phys_pd

.section .mb_header

#define MB_HEADER_FLAGS (MB_FLAG_ALIGN_MODULES | MB_FLAG_MEMORY_INFO | MB_FLAG_AOUT_KLUDGE)

_mb_header:
.long MB_HEADER_MAGIC
.long MB_HEADER_FLAGS
.long -(MB_HEADER_MAGIC + MB_HEADER_FLAGS)
.long MM_V2P(_mb_header)
.long MM_V2P(__text_start)
.long MM_V2P(__data_end)
.long MM_V2P(__kernel_end)
.long MM_V2P(_start)

.section .text

.code32
_start:
    // Setup stack pointer
    movl $MM_V2P(_boot_stack), %esp

    // Save the pointer of the multiboot info structure, it will be passed to the C entry point of the kernel
    pushl $0
    pushl %ebx

    // Check CPUID support
    pushfl
    popl %eax
    movl %eax, %ebx
    xorl $EFLAG_ID, %eax
    pushl %eax
    popfl
    pushfl
    popl %eax
    cmpl %eax, %ebx
    jz _err_cpuid

    // Check PAE support.
    movl $0x1, %eax
    cpuid
    bt $6, %edx
    jnc _err_pae

    // Check long mode support.
    movl $0x80000001, %eax
    cpuid
    bt $29, %edx
    jnc _err_longmode

    movl $MM_V2P(_vmm_kpd), %edi

    // Map the first 4Mb of the memory.
    movl $(PAGE_PRESENT | PAGE_WRITE | PAGE_2MB), (%edi)
    movl $0, 4(%edi)
    movl $(0x200000 /* 2mb */ | PAGE_PRESENT | PAGE_WRITE | PAGE_2MB), 8(%edi)
    movl $0, 12(%edi)

    // Set the PDP entries.
    movl $MM_V2P(_vmm_kpd) + (PAGE_PRESENT | PAGE_WRITE), MM_V2P(_vmm_kpdp)
    movl $MM_V2P(_vmm_kpd) + (PAGE_PRESENT | PAGE_WRITE), MM_V2P(_vmm_kpdp) + PDP_INDEX(KERNEL_VIRT_BASE) * 8

    // Set the PML4 entries.
    movl $MM_V2P(_vmm_kpdp) + (PAGE_PRESENT | PAGE_WRITE), MM_V2P(_vmm_kpml4)
    movl $MM_V2P(_vmm_kpdp) + (PAGE_PRESENT | PAGE_WRITE), MM_V2P(_vmm_kpml4) + PML4_INDEX(KERNEL_VIRT_BASE) * 8

    // Enable PAE.
    movl %cr4, %eax
    orl $CR4_PAE, %eax
    movl %eax, %cr4

    // Load the pml4 address to cr3.
    movl $MM_V2P(_vmm_kpml4), %eax
    movl %eax, %cr3

    // Enable long mode.
    movl $MSR_EFER, %ecx
    rdmsr
    orl $EFER_LME, %eax
    wrmsr

    // Enable paging.
    movl %cr0, %eax
    orl $CR0_PG, %eax
    movl %eax, %cr0

    // Load GDT.
    movl $MM_V2P(_gdt), MM_V2P(_gdtr_addr)
    lgdt MM_V2P(_gdtr)

    // Jump to the 64 bit code.
    ljmp $8, $MM_V2P(2f)

.code64
2:
    // Jump to the virtual address of the kernel.
    leaq 3f, %rax
    jmp *%rax

3:
    // Load GDT with the virtual address.
    movq $_gdt, _gdtr_addr
    lgdt _gdtr

    // Update stack pointer.
    addq $KERNEL_VIRT_BASE, %rsp

    // Clear base pointer
    xorq %rbp, %rbp

    // Multiboot info pointer
    popq %rdi

    // Call the C entry point of the kernel
    call arch_start

4:
    hlt
    jmp 4b

.code32

_err_cpuid:
    movl $MM_V2P(_err_str_cpuid), %esi
    jmp _err_print

_err_pae:
    movl $MM_V2P(_err_str_pae), %esi
    jmp _err_print

_err_longmode:
    movl $MM_V2P(_err_str_lm), %esi
    jmp _err_print

_err_big_mmap:
    movl $MM_V2P(_err_str_big_mmap), %esi

_err_print:
    movl $0xb8000, %edi
5:
    lodsb
    testb %al, %al
    jz 6f
    stosb
    movb $7, %al
    stosb
    jmp 5b
6:
    hlt
    jmp 6b

.section .rodata

_err_str_cpuid:
.asciz "The CPU does not support the CPUID instruction."
_err_str_pae:
.asciz "The CPU does not support physical address extension (PAE)."
_err_str_lm:
.asciz "The CPU does not support long mode."
_err_str_big_mmap:
.asciz "The memory map provided by GRUB is too big."

.section .data

_gdt:
.quad 0
.quad 0x0020980000000000
_gdt_end:
_gdtr:
.word _gdt_end - _gdt - 1
_gdtr_addr:
.quad 0

.section .boot_bss

.fill 4096
_boot_stack:

_vmm_kpml4:
.fill 4096
_vmm_kpdp:
.fill 4096
_vmm_kpd:
.fill 4096

// A pre-allocated PDP table for the first part of the physical map area.
_vmm_phys_pdp:
.fill 4096
_vmm_phys_pd:
.fill 4096

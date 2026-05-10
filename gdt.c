#include "gdt.h"
// This file is used to setup the GDT in C for the kernel.
// This is required for user space, and TSS.

tss_t tss;  // global, zeroed by default

typedef struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_mid;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr {
    unsigned short limit;
    unsigned long  base;
} __attribute__((packed)) gdt_ptr_t;


gdt_entry_t gdt[6] __attribute__((aligned(8)));
gdt_ptr_t gdt_ptr __attribute__((aligned(4)));

void gdt_set_entry(gdt_entry_t *entry, unsigned long base, unsigned long limit, unsigned char access, unsigned char granularity) {
    entry->base_low    = base & 0xFFFF;
    entry->base_mid    = (base >> 16) & 0xFF;
    entry->base_high   = (base >> 24) & 0xFF;
    entry->limit_low   = limit & 0xFFFF;
    entry->granularity = (granularity & 0xF0) | ((limit >> 16) & 0x0F);
    entry->access = access;
}


void setup_tss(void) {
    unsigned long base  = (unsigned long)&tss;
    unsigned long limit = sizeof(tss_t) - 1;

    // patch GDT entry 5 with TSS descriptor
    gdt[5].limit_low   = limit & 0xFFFF;
    gdt[5].base_low    = base & 0xFFFF;
    gdt[5].base_mid    = (base >> 16) & 0xFF;
    gdt[5].access      = 0x89;   // present, ring 0, 32-bit TSS
    gdt[5].granularity = 0x00;   // byte granularity, limit as-is
    gdt[5].base_high   = (base >> 24) & 0xFF;

    // fill in the only fields we care about
    tss.ss0  = 0x10;   // kernel data segment
    tss.esp0 = 0;      // set on first schedule

    // tell CPU where TSS is
    asm volatile("ltr %0" :: "r"((unsigned short)0x28));
}

void gdt_init(void) {
    // null
    gdt[0] = (gdt_entry_t){0};

    // kernel code, data
    gdt_set_entry(&gdt[1], 0, 0xFFFFF, 0x9A, 0xCF);
    gdt_set_entry(&gdt[2], 0, 0xFFFFF, 0x92, 0xCF);

    // User code, data
    gdt_set_entry(&gdt[3], 0, 0xFFFFF, 0xFA, 0xCF);
    gdt_set_entry(&gdt[4], 0, 0xFFFFF, 0xF2, 0xCF);

    // Setting up tss

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base  = (unsigned long)&gdt;

    asm volatile("lgdt %0" :: "m"(gdt_ptr));

    // reload segments
    asm volatile(
        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss\n"
        "ljmp $0x08, $1f\n"  // far jump to reload cs
        "1:\n"
    );

}



#ifndef GDT
#define GDT

typedef struct tss {
    unsigned short link;        // previous TSS selector (unused)
    unsigned short link_h;      // reserved

    unsigned long esp0;         // ← kernel stack pointer (YOU USE THIS)
    unsigned short ss0;         // ← kernel stack segment (YOU USE THIS)
    unsigned short ss0_h;       // reserved

    unsigned long esp1;         // ring 1 stack (unused)
    unsigned short ss1;         // ring 1 segment (unused)
    unsigned short ss1_h;       // reserved

    unsigned long esp2;         // ring 2 stack (unused)
    unsigned short ss2;         // ring 2 segment (unused)
    unsigned short ss2_h;       // reserved

    unsigned long cr3;          // page directory (unused until paging)
    unsigned long eip;          // unused (hardware task switching)
    unsigned long eflags;       // unused
    unsigned long eax;          // unused
    unsigned long ecx;          // unused
    unsigned long edx;          // unused
    unsigned long ebx;          // unused
    unsigned long esp;          // unused
    unsigned long ebp;          // unused
    unsigned long esi;          // unused
    unsigned long edi;          // unused
    unsigned long es;           // unused
    unsigned long cs;           // unused
    unsigned long ss;           // unused
    unsigned long ds;           // unused
    unsigned long fs;           // unused
    unsigned long gs;           // unused
    unsigned long ldt;          // unused
    unsigned short trap;        // unused
    unsigned short iomap_base;  // unused
} __attribute__((packed)) tss_t;


void gdt_init(void);
void setup_tss(void);

#endif
#include "mem.h"
#include "kprintf.h"

#define PAGE_MASK  0xFFFFF000
#define FLAG_MASK  0x00000FFF
#define PAGE_SIZE  4096


// File is for managing memory for kernel.
//
//
extern unsigned char bitmap[512];
extern unsigned long page_directory[1024];
extern unsigned long first_page_table[1024];
extern unsigned long second_page_table[1024];

/* Virtual address
 * 32 Bits -> [31-22 Page directory idx][21-12 Ptable idx][11-0] Offset
 *
 *
 * Each Directory entry has the address of a Page Table.
 * [31-12 Page table address][11-0 flags]
 *
 * Each Page table has 1024 entries.
 * Where each entry has a 32 bit (unsigned long)
 * [31-12 Physical address][11-0 flags]
 *
 *
 * Example flag bits:
 * Bit 0  (P)   → Present
 * Bit 1  (RW)  → Read/Write
 * Bit 2  (US)  → User/Supervisor
 * Bit 3  (PWT) → Page-level write-through
 * Bit 4  (PCD) → Page-level cache disable
 * Bit 5  (A)   → Accessed        (set by CPU)
 * Bit 6  (D)   → Dirty           (set by CPU, only in PTE)
 * Bit 7  (PAT) → Page Attribute Table
 * Bit 8  (G)   → Global (if CR4.PGE enabled)
 * Bit 9–11     → Available to OS (MY SPACE, can define whatever)
 *
 * Awesome note!
 * Since each directory and page table 
 * is a array of (32bit, unsigned long) values. Each directory,
 * and page table is going to take up exactly 1 page of pysical memeory each!
 * So we can use the pysical page allocator to fetch pages to use for these!
 *
 * Process to 
 *
 */

void *alloc_page(void);

unsigned long bitmap_to_address(unsigned long idx) {
 // Physical address = page_idx * 4096
  return (unsigned long)(idx * 4096);
}


void print_alloc_pages() {
  kprintf("\nPHYSICAL PAGES ALLOCATED\n");
  for (int i = 0; i < 512; i++) {
    unsigned char byte = bitmap[i];
    unsigned long base_idx = i * 8;
    for (int j = 0; j < 8; j++) {
      if (byte & (1 << j)) {
        unsigned long addr = bitmap_to_address(base_idx + j);
        kprintf("%ul, %p\n", addr, (base_idx + j));
      }
    }
  }
  kprintf("END\n");
}

void *memset(void *ptr, int c, unsigned long size) {
  for (int i = 0; i < size; i++) {
    int *ptr = (int*)(ptr + i);
    *ptr = c;
  }
}

void memcp(void *dest, void *src, unsigned long size) {
  for (unsigned long i = 0; i < size; i++) {
    *((unsigned char *)dest + i) = *((unsigned char *)src + i);
  }
}

void print_page_dir() {

}

int v_addr_pde_idx(unsigned long v_addr) {
  return (int)(v_addr >> 22);
}

int v_addr_pte_idx(unsigned long v_addr) {
  return (v_addr >> 12) & 0x3FF;
}


void *map_page(unsigned long v_addr, unsigned long phy_addr, unsigned long flags) {
  // When mapping pages we are going to assume you are a great programmer,
  // and have called alloc_page which will give you the physical page that you can make you're
  // virtual address to.
  //  vaddr = 32 Bits -> [31-22 Page directory idx][21-12 Ptable idx][11-0] Offset
  // 
  // 32 Bits -> [31-22 Page directory idx][21-12 Ptable idx][11-0] Offset
  // Mapping address 0x FE E0 00 00 lapic
  //
  // Virtual address for page dir idx 2
  // 00     00     00    00
  // 31-24  23-16  15-8  7-0
  //
  // 00 
  //
  // 00 00 00 00 | 01 00 00 00 00 | 00 00 00 00 | 00 00 00 11 
  // 
  // 0000000001000000000000000000000011
  // 0000000000010000000000000000000000
  //
  //pde = 1
  //pte = 0
  //0x400000
  //pde = page directory entry
  //pte = page table entry.
  //take apart virtual address to get pde, if pde doesn't have entry.
  //Allocate page for page table.
  //Add entry.
  //Then we need to get page table idx from virtual addr.
  //If page table has entry PANIC I guess.
  //Else create PTE that has physical address with flags.
  int pde = v_addr_pde_idx(v_addr);
  int pte = v_addr_pte_idx(v_addr);

  unsigned long *page_directory_entry = (page_directory + pde);
  if (!(*page_directory_entry & 0x1)) {
    // If entry doesn't exist we need to 
    // - Allocate page for new p_table.
    // - Zero out allocated page.
    // - Create p_dir entry for page table.
    unsigned long *new_page_table_addr = (unsigned long*)alloc_page();
    memset(new_page_table_addr, 0, PAGE_SIZE);
    unsigned long flag_mask = 0xFFFFF000;
    unsigned long pt_addr = ((unsigned long)new_page_table_addr & flag_mask);
    pt_addr = pt_addr | flags; //Adding p, w/r
    *page_directory_entry = pt_addr;
  } else {
    // Updating the entry to be user writable
    *page_directory_entry = (*page_directory & PAGE_MASK) | (flags & (0x1 | 0x2 | 0x4));
  }


   
  unsigned long *page_table_addr = (unsigned long*)((*page_directory_entry) & 0xFFFFF000);
  unsigned long *page_table_entry = page_table_addr + pte;
  if ((*page_table_entry) & 0x1) {
    kprintf("\nPAGE TABLE Entry already exists: PTE:%d->%d, %ul, VADDR:%ul\n", pde, pte, page_table_entry, v_addr);
    // If it exists we are going to override the flags.
    // Just for now for testing purposes.
    *page_table_entry = (*page_table_entry & PAGE_MASK) | (flags & FLAG_MASK);
    return (void*)-1;
  }

  unsigned long physical_address = phy_addr & 0xFFFFF000;
  physical_address = physical_address | flags;
  kprintf("\nPHY: %ul", physical_address);
  *page_table_entry = physical_address;
  kprintf("%d, %d", pde, pte);

  return (void*)v_addr;
}







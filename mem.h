#ifndef MEM
#define MEM

#define PAGE_PRESENT  0x1   // page exists
#define PAGE_WRITE    0x2   // writable
#define PAGE_USER     0x4   // ring 3 accessible ← critical

void print_alloc_pages();
void *memset(void *ptr, int c, unsigned long size);
void *map_page(unsigned long v_addr, unsigned long phy_addr, unsigned long flags);
void memcp(void *dest, void *src, unsigned long size);

#endif // !MEM

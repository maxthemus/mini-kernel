#ifndef MEM
#define MEM

void print_alloc_pages();
void *memset(void *ptr, int c, unsigned long size);
void *map_page(unsigned long v_addr, unsigned long phy_addr);

#endif // !MEM

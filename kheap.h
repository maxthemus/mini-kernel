#ifndef K_HEAP
#define K_HEAP


#define HEAP_SIZE 0x1000

typedef struct block {
    unsigned long size;
    unsigned long free;
    struct block *next;
} block_t;


void kmalloc_init(void);
void *kmalloc (unsigned long size);
void kfree (void *ptr);
void k_heap_dump(void);

#endif // ! HEAP_SIZE

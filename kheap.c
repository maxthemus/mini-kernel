#include "kheap.h"
#include "kprintf.h"


char heap[HEAP_SIZE];
block_t *heap_head = 0;

block_t *split_free_block(block_t *block, unsigned long rounded_size);
void print_block(block_t *blockPtr);

void kmalloc_init(void) {
  heap_head = (block_t *)heap;

  heap_head->size = HEAP_SIZE - sizeof(block_t);
  heap_head->free = 1;
  heap_head->next = 0;
  return;
}

void *kmalloc (unsigned long size) {
  unsigned long rounded_size = ((size + (3)) / 4) * 4; // Next greatest multiple of 4.

  block_t *ptr = heap_head;
  while (!ptr->free || ptr->size < rounded_size) {
    if (!ptr->next) {
      return 0; // No more blocks
    }
    ptr = ptr->next;
  }

  // Double check if used
  if (!ptr->free) {
    return 0;
  }

  // If we have made it here then we have a block_t that we can allocate
  // Splitting if blocks are larger then bytes
  // We are onlyhing goind to split if the split remaining size is going to be larger then 64
  if (ptr->size - rounded_size >= sizeof(block_t) + 64) {
    // Unused but here to just see I can use it.
    block_t *next_block = split_free_block(ptr, rounded_size);
  }
  ptr->free = 0;

  return ptr + 1;
}

void kfree (void *ptr) {
  if (!ptr) return;

  block_t *free_ptr = ((block_t *)ptr)-1;
  block_t *block_ptr = free_ptr;

  free_ptr->free = 1;

  // Checking how many blocks I can clean up. 
  // Currently only going to look forward
  unsigned long free_size = block_ptr->size;
  while (block_ptr->next && block_ptr->next->free) {
    free_size += block_ptr->next->size + sizeof(block_t);
    block_ptr = block_ptr->next;
  }

  // Now we need to resize free_block and free it.
  free_ptr->size = free_size;
  free_ptr->next = block_ptr->next;
}

block_t *split_free_block(block_t *block, unsigned long rounded_size) {
  // We are assuming that size is already rounded_size

  char *next_block_location = ((char *)block + sizeof(block_t) + rounded_size);
  block_t *next_block = (block_t *)next_block_location;
  next_block->size = block->size - sizeof(block_t) - rounded_size;
  next_block->free = 1;
  next_block->next = block->next;

  block->size = rounded_size;
  block->next = next_block;

  return next_block;
}


/* -------------------------------
* Kernel heap utils functions
*------------------------------- */
void print_block(block_t *blockPtr) {
  kprintf("size: %ul, free: %ul", blockPtr->size, blockPtr->free);
}

void k_heap_dump(void) {
  block_t *blockPtr = heap_head;
  kprintf("\nK_HEAP DUMP\n");

  if (!blockPtr) return;
  int i = 0;
  kprintf("%d-> ", i++);
  print_block(blockPtr);
  kprintf("\n");

  while (blockPtr->next) {
    blockPtr = blockPtr->next;
    kprintf("%d-> ", i);
    print_block(blockPtr);
    kprintf("\n");
    i++;
  }

  kprintf("DONE");
}




#include "kproc.h"
#include "kheap.h"
#include "kprintf.h"

void run_sti(void);

int current_task = 0;
proc tasks[2];
context *sched_ctx;
int running = 0;

void task_a(void) {
  while (1) {
    //run_sti();
    kprintf("A");
    for (int i = 0; i < 99999999; i++) {}
  }
}


void task_b(void) {
  while (1) {
    //run_sti();
    kprintf("B");
    for (int i = 0; i < 99999999; i++) {}
  }
}

// 1 2 3 4 5 6 7 8 9
//               A  x

void schedule_task(int idx) {
  proc *cur = (tasks +idx);
  cur->pid = idx;
  cur->run_tick_count = 0;

  void *stack = kmalloc(STACK_SIZE);
  unsigned long *stack_ptr = (unsigned long *)((char *)stack + STACK_SIZE);
  if (idx == 0) {
    *--stack_ptr = (unsigned long)task_a;
  } else {
    *--stack_ptr = (unsigned long)task_b;
  }
  *--stack_ptr = 0; // ebp
  *--stack_ptr = 0; // ebx
  *--stack_ptr = 0; // esi
  *--stack_ptr = 0; // edi
  cur->trap.esp = (unsigned long)stack_ptr;
  cur->context = (context *)stack_ptr;
 }

void sched(void) {
  if (running) {
    kprintf("\nSCHED: %d", current_task);
    switch_proc(&tasks[current_task].context, sched_ctx);
  }
}

void start_task(void) {
  sched_ctx = (context *)kmalloc(sizeof(context));

  running = 1;
  kprintf("Starting task");
  while (1) {
    for (int i = 0; i < 2; i++) {
      kprintf("\nRUNNING TASK: %d", i);
      current_task = i;
      proc *task = tasks+i;
      context *ctx = task->context;
      kprintf("\nME %d", i);
      switch_proc(&sched_ctx, ctx);
      kprintf("\nAFTER %d", i);
    }
  }
}


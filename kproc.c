#include "kproc.h"
#include "kheap.h"
#include "kprintf.h"

void run_sti(void);
void start_tasks(unsigned long stackPtr);

/**
 * Logic for context switching using PIC interrupt handler.
 * 
 * INT
 * Store registers on stack.
 * Store stack ptr.
 *
 * clear_interrupt send EOI.
 *
 * Call Interrupt handler
 * - Tick++
 * - call sched
 *   - Find PROC to run.
 *   - Return (unsigned long)stack ptr <- for new program to run.
 * - Return str ptr
 *
 * Store current stack pointer in context for current running proc.
 * Switch Stack ptr to ret ptr from sched.
 * Iretd
 *
 */

int current_idx = 0;
int num_tasks = 2;
int task_count = 0;

int current_task = 0;
proc tasks[2];
context *sched_ctx;
int running = 0;

void task_a(void) {
  while (1) {
    //run_sti();
    kprintf("A");
    for (int i = 0; i < 999999; i++) {}
  }
}


void task_b(void) {
  while (1) {
    //run_sti();
    kprintf("B");
    for (int i = 0; i < 999999; i++) {}
  }
}

// 1 2 3 4 5 6 7 8 9
//               A  x

/*
void schedule_task(int idx) {
  proc *cur = (tasks +idx);
  cur->pid = idx;
  cur->run_tick_count = 0;

  void *stack = kmalloc(STACK_SIZE);
  unsigned long *stack_top = (unsigned long *)((char *)stack + STACK_SIZE);
  unsigned long *sp = stack_top;


  *--sp = 0x202;                         // EFLAGS
  *--sp = 0x08;                          // CS
                                         //
  --sp = (unsigned long)task_entry;     // EIP
  if (idx == 0) {
    *--sp = (unsigned long)task_a;
  } else {
    *--sp = (unsigned long)task_b;
  }

  
  *--sp = 0; // eax
  *--sp = 0; // ecx
  *--sp = 0; // edx
  *--sp = 0; // ebx
  *--sp = 0; // esp (ignored)
  *--sp = 0; // ebp
  *--sp = 0; // esi
  *--sp = 0; // edi

  cur->trap.esp = (unsigned long)sp;
  cur->context = (context *)sp;
  cur->stackAddr = (unsigned long)sp;
 }
 */

/*
void sched(void) {
  if (running) {
    kprintf("\nSCHED: %d", current_task);
    switch_proc(&tasks[current_task].context, sched_ctx);
  }
}

*/
void start_task(void) {
  start_tasks(tasks[0].esp);
}

/*
void store_current_proc(unsigned long stackAddr) {
  tasks[current_task].stackAddr = stackAddr;
}
 

unsigned long load_next_proc(void) {
  for (int i = 0; i < 2; i++) {
    if (i == current_task) {
      continue;
    }

    return tasks[i].stackAddr;
  }
  kprintf("\nNO PROCESS TO SWITCH TO\n");
  return tasks[current_task].stackAddr;
}
*/


unsigned long schedule(unsigned long current_esp) {
  kprintf("\nTIMER");
  // save current task state
  tasks[current_idx].esp = current_esp;

  // simple round-robin
  current_idx++;
  if (current_idx >= num_tasks) {
    current_idx = 0;
  }

  // return next task's saved stack
  kprintf("%ul", tasks[current_idx].esp);
  return tasks[current_idx].esp;
}

void schedule_task(int idx) {
    proc *t = &tasks[idx];

    void *stack = kmalloc(STACK_SIZE);
    unsigned long *sp = (unsigned long *)((char *)stack + STACK_SIZE);

    // -------------------------
    // IRET frame
    // -------------------------
    *--sp = 0x202;              // EFLAGS
    *--sp = 0x08;              // CS
    if (idx == 0) {
      *--sp = (unsigned long)task_a;        // EIP
    } else {
      *--sp = (unsigned long)task_b;        // EIP
    }

    // -------------------------
    // PUSHA frame (must match popa)
    // -------------------------
    *--sp = 0; // EAX
    *--sp = 0; // ECX
    *--sp = 0; // EDX
    *--sp = 0; // EBX
    *--sp = 0; // ESP (ignored by popa)
    *--sp = 0; // EBP
    *--sp = 0; // ESI
    *--sp = 0; // EDI

    t->esp = (unsigned long)sp;
    kprintf("\nSETTING UP STACK: %ul", (unsigned long)sp);
    t->stack_base = stack;
    t->pid = task_count;
    t->run_tick_count = 0;

    task_count++;
}



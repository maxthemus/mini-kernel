#include "kproc.h"
#include "kheap.h"
#include "kprintf.h"


void run_sti(void);
void start_tasks(unsigned long stackPtr);
void halt_system(void);

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
int task_count = 0;

int current_task = 0;
proc tasks[5];
proc *idle_task;
context *sched_ctx;
int running = 0;

void idle_task_func(void) {
  kprintf("IDLE TASK");
  while (1) {
    halt_system();
  }
}



// 1 2 3 4 5 6 7 8 9
//               A  x


void start_task(void) {
  tasks[0].task_state = TASK_RUNNING;
  start_tasks(tasks[0].esp);
}

unsigned long schedule(unsigned long current_esp) {
  kprintf("\nTIMER, A%d->", current_idx); // save current task state
  proc *cur_p = tasks + current_idx;
  cur_p->esp = current_esp;
  cur_p->task_state = TASK_READY;

  for (int i = 0; i < task_count + 1; i++) {
    int idx = (current_idx + i + 1) % (task_count);
    kprintf("B%d->", idx);
    proc *p = tasks + idx;
    if (p->task_state == TASK_READY) {
      p->task_state == TASK_RUNNING;
      
      current_idx = idx;
      kprintf("C%d", idx);
      return p->esp;
    }
  } 

  // All procs are currently blocked or no tasks in loop
  return idle_task->esp;
}

void schedule_task(void (*func)(void)) {
    proc *t = &tasks[task_count];

    void *stack = kmalloc(STACK_SIZE);
    unsigned long *sp = (unsigned long *)((char *)stack + STACK_SIZE);

    // -------------------------
    // IRET frame
    // -------------------------
    *--sp = 0x202;              // EFLAGS
    *--sp = 0x08;              // CS
    *--sp = (unsigned long)func;        // EIP
                                        //
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
    t->stack_base = stack;
    t->pid = task_count;
    t->run_tick_count = 0;
    t->task_state = TASK_READY;

    task_count++;
}


void init_scheduler(void) {
  // Settingup idle task 
  proc *t = kmalloc(sizeof(proc));
  void *stack = kmalloc(STACK_SIZE);
  unsigned long *sp = (unsigned long *)((char *)stack + STACK_SIZE);
  *--sp = 0x202; // EFLAGS
  *--sp = 0x08;  // CS
  *--sp = (unsigned long)idle_task_func; // EIP
  *--sp = 0; // EAX
  *--sp = 0; // ECX
  *--sp = 0; // EDX
  *--sp = 0; // EBX
  *--sp = 0; // ESP (ignored by popa)
  *--sp = 0; // EBP
  *--sp = 0; // ESI
  *--sp = 0; // EDI
             //
  t->esp = (unsigned long)stack;
  t->stack_base = stack;
  t->pid = 10;
  t->run_tick_count = 0;
  t->task_state = TASK_READY;

  idle_task = t; //Storing idle task
}



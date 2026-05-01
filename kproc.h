#ifndef PROC
#define PROC

#define STACK_SIZE 1024

typedef struct {
  unsigned long eax;
  unsigned long ebx;
  unsigned long ecx;
  unsigned long edx;
  unsigned long esi;
  unsigned long edi;
  unsigned long ebp;

  unsigned long esp;
  unsigned long eip;
} trap_frame;

typedef struct {
  unsigned long ebp;
  unsigned long ebx;
  unsigned long esi;
  unsigned long edi;
}context;

void switch_proc(context **old, context *new);
void save_proc(trap_frame *proc);
void sched(void);

typedef struct {
  unsigned long pid;
  trap_frame trap;
  unsigned long run_tick_count;
  context *context;
}proc;


void schedule_task(int idx);
void start_task(void);

#endif // !DEBUG

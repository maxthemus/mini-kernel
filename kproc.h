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
    unsigned long esp;
    void *stack_base;
    int pid;
    int run_tick_count;
} proc;


void schedule_task(int idx);
void start_task(void);
void store_current_proc(unsigned long stackAddr);
unsigned long load_next_proc(void);
unsigned long schedule(unsigned long current_esp);

#endif // !DEBUG

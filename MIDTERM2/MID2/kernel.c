// kernel.c file

#define NPROC 9
#define N 5
/*********** in type.h ***************
typedef struct proc{
  struct proc *next;
  int    *ksp;

  int    pid;
  int    ppid;
  int    priority;
  int    status;
  int    event;
  int    exitCode;

  struct proc *parent;
  struct proc *child;
  struct proc *sibling;

  int    kstack[SSIZE];
}PROC;
***************************************/
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

int body();

int kernel_init()
{
  int i;
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++)
  {
    p = &proc[i];
    p->pid = i;
    p->ppid = 0;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0];    // freeList = ALL free procs
  //printList("freeList", freeList);

  readyQueue = 0;
  sleepList = 0;

  // creat P0 as running process
  p = dequeue(&freeList); // take proc[0] off freeList
  p->priority = 0;
  p->status = READY;
  p->ppid = 0;
  p->parent = p;
  p->child = p->sibling = 0;
  running = p;           // running -> proc[0] with pid=0

  kprintf("running = %d\n", running->pid);
  kprintf("P0 kfork %d processes\n", N);
  for (int i = 0; i < N; ++i)
    kfork((int)body, 1);
  printList("readyQueue", readyQueue);
  printf("P0 running\n");
  printf("next running = 1\n");
  tswitch();
  //printList("freeList", freeList);
}

int kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0)
  {
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;
  p->child = 0;
  p->sibling = running->child;
  running->child = p;

  // set kstack for new proc to resume to func()
  // stmfd sp!, {r0-r12, lr} saves regs in stack as
  // stack = lr r12 r11 r10 r9 r8 r7 r6 r5  r4  r3  r2  r1  r0
  // HIGH    -1 -2  -3  -4  -5 -6 -7 -8 -9 -10 -11 -12 -13 -14   LOW
  for (i=1; i<15; i++)
    p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-1] = (int)func;  // saved regs in dec address ORDER !!!

  p->ksp = &(p->kstack[SSIZE-14]);

  enqueue(&readyQueue, p);

  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  return p->pid;
}

int scheduler()
{
  // kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  // kprintf("next running = %d\n", running->pid);
  if (running->pid)
  {
    color = running->pid;
  }
}

// code of processes
int body()
{
  while(1)
  {
    printList("readyQueue", readyQueue);

    printf("proc %d running, enter a timer value: ", running->pid);
    int time = geti();
    printf("\n");
    ksleep(time);
  }
}


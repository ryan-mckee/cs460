// kernel.c file

#define NPROC 9
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
  printList("freeList", freeList);

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
  printList("freeList", freeList);
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
  printList("readyQueue", readyQueue);
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

int ps();

void print_status(int status)
{
  switch (status)
  {
  case FREE:
    printf("FREE");
    break;
  case READY:
    printf("READY");
    break;
  case SLEEP:
    printf("SLEEP");
    break;
  case BLOCK:
    printf("BLOCK");
    break;
  case ZOMBIE:
    printf("ZOMBIE");
    break;
  default:
    break;
  }
}

// code of processes
int body()
{
  char c, cmd[64];

  printf("proc %d resume to body()\n", running->pid);

  while(1)
  {
    printf("P%d running  parent=%d ", running->pid, running->ppid);

    // write code to print childList=[pid, status]->...->NULL
    printChildList(running->child);

    printList("freeList  ", freeList);
    printList("readyQueue", readyQueue);
    printsleepList(sleepList);

    printf("Enter a command [ps|switch|fork|sleep|wakeup|exit|wait] : ");
    kgets(cmd);
    printf("\n");

    if (strcmp(cmd, "switch")==0)
      tswitch();
    else if (strcmp(cmd, "fork")==0)
      kfork((int)body, 1);
    else if (strcmp(cmd, "ps")==0)
      ps();
    else if (strcmp(cmd, "sleep")==0)
    {
      if (running->pid == 1)
        printf("P1 can't sleep by command\n");
      else
      {
        printf("input an event value to sleep : ");
        int event = geti();
        printf("\n");
        ksleep(event);
      }
    }
    else if (strcmp(cmd, "wakeup")==0)
    {
      printf("input an event value to wakeup : ");
      int event = geti();
      printf("\n");
      kwakeup(event);
    }
    else if (strcmp(cmd, "exit")==0)
    {
      if (running->pid == 1)
        printf("P1 can't die\n");
      else
      {
        printf("input an exitCode value : ");
        int exitCode = geti();
        printf("\n");
        kexit(exitCode);
      }
    }
    else if (strcmp(cmd, "wait")==0)
    {
      int status = 0;
      int pid = kwait(&status);
    }
  }
}

int ps()
{
  printf("PID    PPID   status\n");
  printf("---    ----   ------\n");
  for (int i = 0; i < NPROC; ++i)
  {
    printf(" %d     %d    ", proc[i].pid, proc[i].ppid);
    if (proc + i == running)
      printf("RUNNING");
    else
      print_status(proc[i].status);
    printf("\n");
  }
}

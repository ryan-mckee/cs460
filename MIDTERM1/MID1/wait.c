// wait.c file

extern PROC *freeList;
extern PROC *readyQueue;
extern PROC *sleepList;
extern PROC *running;

int ksleep(int event)
{
  int SR = int_off(); // disable IRQ and return CPSR
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  tswitch(); // switch process
  int_on(SR); // restore original CPSR
}

//need to implement removing from sleeplist
int kwakeup(int event)
{
  int SR = int_off(); // disable IRQ and return CPSR
  //remove matches at start of sleep list
  for (PROC *p = sleepList; p; p = sleepList)
  {
    if (p->event==event)
    {
      dequeue(&sleepList);
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else
      break;
  }
  //remove other matches
  PROC *prev = 0;
  for (PROC *p = sleepList; p; p = p->next)
  {
    if (p->event==event)
    {
      prev->next = p->next;
      p->status = READY;
      enqueue(&readyQueue, p);
      p = prev;
    }
    else
      prev = p;
  }
  int_on(SR); // restore original CPSR
}

int kexit(int exitCode)
{
  // implement this
  PROC *p = running->child;
  while (p)
  {
    PROC *q = p->sibling;
    p->ppid = proc[1].pid;
    p->sibling = proc[1].child;
    p->parent = &proc[1];
    proc[1].child = p;
    p = q;
  }
  running->exitCode = exitCode;
  running->status = ZOMBIE;
  kwakeup(running->ppid);
  tswitch();
}

int kwait(int *status)
{
  // implement this
  int k = 0;
  for (; k < NPROC; ++k)
  {
    if (running->pid == proc[k].ppid)
      break;
  }
  if (k == NPROC)
    return -1;
  while(1)  // caller has children
  {
    //search for a (any) ZOMBIE child;
    for (int i = 0; i < NPROC; ++i)
      if (proc[i].status == ZOMBIE && running->pid == proc[i].ppid)
      {
        //get ZOMBIE child pid;
        int pid = proc[i].pid;
        //copy ZOMBIE child exitCode to *status;
        *status = proc[i].exitCode;
        //release child PROC to freeList as FREE;
        proc[i].status = FREE;
        enqueue(&freeList, &proc[i]);
        //return ZOMBIE child pid;
        return pid;
      }
    ksleep(running->pid); // sleep on its PROC address
  }
}

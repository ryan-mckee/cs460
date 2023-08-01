// wait.c file

extern PROC *readyQueue;
extern PROC *sleepList;
extern PROC *running;

int ksleep(int event)
{
  int SR = int_off(); // disable IRQ and return CPSR
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printsleepList(sleepList);
  printf("next running = %d\n", running->pid % N + 1);
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
      printf("kwakeup %d\n", p->pid);
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
      printf("kwakeup %d\n", p->pid);
      enqueue(&readyQueue, p);
      p = prev;
    }
    else
      prev = p;
  }
  int_on(SR); // restore original CPSR
}

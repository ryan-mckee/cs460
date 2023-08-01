extern PROC *readyQueue;
extern PROC *running;

int P(struct semaphore *s)
{
  int SR = int_off();
  s->value--;
  if (s->value < 0)
    block(s);
  printList("semaphore queue", s->queue);
  int_on(SR);
}

int V(struct semaphore *s)
{
  int SR = int_off();
  s->value++;
  if (s->value <= 0)
    signal(s);
  for (int i = 1; i < 4; ++i)
    if (proc[i].status == READY)
      printf("proc %d is unblocked\n", i);
  int_on(SR);
}


int block(struct semaphore *s)
{
  running->status = BLOCK;
  enqueue(&s->queue, running);
  tswitch();
}

int signal(struct semaphore *s)
{
  PROC *p = dequeue(&s->queue);
  p->status = READY;
  enqueue(&readyQueue, p);
}

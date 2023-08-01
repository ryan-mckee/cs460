/************** queue.c file ***************/

int enqueue(PROC **queue, PROC *p)
{
  PROC *q  = *queue;
  if (q==0 || p->priority > q->priority){
    *queue = p;
    p->next = q;
    return;
  }
  while (q->next && p->priority <= q->next->priority){
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
}

PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  if (p)
    *queue = p->next;
  return p;
}

int printList(char *name, PROC *p)
{
  printf("%s=", name);
  while(p){
    printf("[%d%d]->", p->pid, p->priority);
    p = p->next;
  }
  printf("NULL\n");
}

int printsleepList(PROC *p)
{
  printf("sleepList=");
  while(p){
    printf("[%d%d]->", p->pid, p->event);
    p = p->next;
  }
  printf("NULL\n");
}


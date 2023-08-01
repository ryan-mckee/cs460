int color;

#include "type.h"
#include "string.c"
#include "queue.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"
#include "pv.c"
#include "message.c"

void copy_vectors(void)
{
  extern u32 vectors_start;
  extern u32 vectors_end;
  u32 *vectors_src = &vectors_start;
  u32 *vectors_dst = (u32 *)0;

  while(vectors_src < &vectors_end)
    *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void IRQ_handler()
{
  int vicstatus, sicstatus;
  vicstatus = *(VIC_BASE + VIC_STATUS); // VIC_STATUS=0x10140000=status reg
  sicstatus = *(SIC_BASE + SIC_STATUS);

  if (vicstatus & (1 << 31))
  {
    if (sicstatus & (1 << 3))
    {
      kbd_handler();
    }
  }
}

int sender()
{
  char line[128];
  printf("Enter a line for task%d to get\n", running->pid);

  while(1)           // use printf() to show actions
  {
    //get a line;
    kgetline(line);
    printf("task%d got a line=%s\n", running->pid, line);
    send(line, NSENDERS + 1);
    printf("task%d send %s to pid=4\n", running->pid,line);
  }
}

int receiver()
{
  char line[128];
  int pid;
  while(1)          // use printf() to show actions
  {
    printf("task%d try to receive msg\n", running->pid);
    pid = recv(line);
    printf("task%d received: [%s] from task%d\n",
           running->pid, line, pid);
    //print messaage contents as sendPID, message
  }
}

int main()
{
  color = CYAN;
  row = col = 0;

  fbuf_init();
  kbd_init();

  // allow KBD interrupts
  *(VIC_BASE + VIC_INTENABLE) |= (1<<31); // allow VIC IRQ31
  *(SIC_BASE + SIC_INTENABLE) |= (1<<3);  // KBD int=3 on SIC

  kprintf("Welcome to WANIX in Arm\n");

  mesg_init();
  kernel_init();
  while(1)
  {
    if (readyQueue)
      tswitch();
  }
}

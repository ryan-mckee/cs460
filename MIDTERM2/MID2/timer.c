// timer.c file
/***** timer confiuguration values *****/
#define CTL_ENABLE          ( 0x00000080 )
#define CTL_MODE            ( 0x00000040 )
#define CTL_INTR            ( 0x00000020 )
#define CTL_PRESCALE_1      ( 0x00000008 )
#define CTL_PRESCALE_2      ( 0x00000004 )
#define CTL_CTRLEN          ( 0x00000002 )
#define CTL_ONESHOT         ( 0x00000001 )

// timer register offsets from base address
/**** byte offsets *******
#define TLOAD   0x00
#define TVALUE  0x04
#define TCNTL   0x08
#define TINTCLR 0x0C
#define TRIS    0x10
#define TMIS    0x14
#define TBGLOAD 0x18
*************************/
/** u32 * offsets *****/
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6

typedef volatile struct timer
{
  u32 *base;            // timer's base address; as u32 pointer
  int tick, hh, mm, ss; // per timer data area
  char clock[16];
} TIMER;

volatile TIMER timer[4];  // 4 timers; 2 timers per unit; at 0x00 and 0x20

extern int kpchar(char, int, int);
extern int unkpchar(char, int, int);
extern int kputc(char);

int k;

void timer_init()
{
  int i;
  TIMER *tp;
  kprintf("timer_init() ");

  for (i=0; i<4; i++)
  {
    tp = &timer[i];
    if (i==0) tp->base = (u32 *)0x101E2000;
    if (i==1) tp->base = (u32 *)0x101E2020;
    if (i==2) tp->base = (u32 *)0x101E3000;
    if (i==3) tp->base = (u32 *)0x101E3020;

    *(tp->base+TLOAD) = 0x0;   // reset
    *(tp->base+TVALUE)= 0x0;
    *(tp->base+TRIS)  = 0x0;
    *(tp->base+TMIS)  = 0x0;
    *(tp->base+TCNTL) = 0x62; //011-0000=|En|Pe|IntE|-|scal=00|32-bit|0=wrap|
    *(tp->base+TBGLOAD) = 0xE0000/60;

    tp->tick = tp->hh = tp->mm = tp->ss = 0;
    char cl[16] = "00:00:00";
    strcpy((char *)tp->clock, cl);
    //strcpy((char *)tp->clock, "00:00:00");
  }
}

void display_clock(TIMER *t)
{
  t->clock[0] = t->hh / 10 + '0';
  t->clock[1] = t->hh % 10 + '0';
  t->clock[2] = ':';
  t->clock[3] = t->mm / 10 + '0';
  t->clock[4] = t->mm % 10 + '0';
  t->clock[5] = ':';
  t->clock[6] = t->ss / 10 + '0';
  t->clock[7] = t->ss % 10 + '0';
  for (int i = 0; i < 8; ++i)
  {
    //unkpchar(' ', 0, 70 + i);
    unkpchar(127, 0, 70 + i);
    kpchar(t->clock[i], 0, 70 + i);
  }
}

void timer_handler(int n)
{
  TIMER *t = &timer[n];
  if (t->tick == 59 && sleepList)
  {
    for (PROC *temp = sleepList; temp; temp = temp->next)
    {
      if (temp->event)
        --temp->event;
    }
    if (sleepList)
    {
      printf("proc %drunning\n", running->pid);
      printsleepList(sleepList);
    }
    kwakeup(0);
  }
  int c1 = color;
  color = CYAN;

  if (t->tick == 30)
    clrcursor();

  t->tick++;
  if (t->tick == 60)
  {
    t->tick = 0;
    t->ss++;
    if (t->ss == 60)
    {
      t->ss = 0;
      t->mm++;
      if (t->mm == 60)
      {
        t->mm = 0;
        t->hh++;
      }
    }
    //kputs("timer interrupt\n");
    //kputs(t->clock);
    display_clock(t);
    putcursor();
    //kpchar('a', 5, 5);
  }

  timer_clearInterrupt(n);
  color = c1;
}

void timer_start(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  kprintf("timer_start\n");
  *(tp->base+TCNTL) |= 0x80;    // set enable bit 7
}

int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TCNTL) &= 0x7F;  // clear enable bit 7
}

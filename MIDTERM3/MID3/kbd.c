#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef volatile struct kbd
{
  char *base;
  char buf[128];
  int head, tail, data, room;
  struct semaphore kline;  // kline is a semaphore
} KBD;

volatile KBD kbd;

int release;       // key release flag
int shift;
int control;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10;    // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;       // ARM manual says clock=8
  kp->head = kp->tail = 0;       // circular buffer char buf[128]
  kp->data = 0;
  kp->room = 128;

  release = 0;

  shift = 0;
  control = 0;

  kp->kline.value = 0;
  kp->kline.queue = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  scode = *(kp->base + KDATA);   // get scan code of this interrpt

  if (scode == 0x12)
    shift = !release;

  if (scode == 0x14)
    control = !release;

  if (scode == 0xF0)   // it's key release
  {
    release = 1;      // set release flag
    return;
  }

  if (release == 1)    // scan code after 0xF0
  {
    release = 0;      // reset release flag
    return;
  }

  color = WHITE;

  // map scode to ASCII in lowercase
  c = ltab[scode];

  if (c != '\r')
  {
    if (scode != 0x12 && scode != 0x14)
    {
      if (shift)
        kputc(c - 32);
      else
        kputc(c);
    }
  }
  else
    printf("\n");

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++;
  kp->room--;

  if (c == '\r')
    V(&kp->kline);
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();                          // unmask IRQ in case it was masked out
  while(kp->data == 0);              // BUSY wait while kp->data is 0

  lock();                            // mask out IRQ
  c = kp->buf[kp->tail++];
  kp->tail %= 128;                 /*** Critical Region ***/
  kp->data--;
  kp->room++;
  unlock();                        // unmask IRQ
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r')
  {
    *s = c;
    s++;
  }
  *s = 0;
}


int kgetline(char line[ ])
{
  KBD *kp = &kbd;

  P(&kp->kline);       // wait for a line

  //get a line (replace \r with NULL)
  kgets(line);
}


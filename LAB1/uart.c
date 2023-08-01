/************** uart.c file *******************
Flag register (offset 0x18): status of UART port

   7    6    5    4    3   2   1   0
| TXFE RXFF TXFF RXFE BUSY -   -   - |
  
TXFE=Tx buffer empty
RXFF=Rx buffer full
TXFF=Tx buffer full
RXFE=Rx buffer empty
BUSY = device busy
**********************************************/
  
#define DR 0x00
#define FR 0x18

typedef struct uart{
  char *base;           // base address
  int  n;               // UART number 
}UART;

UART *up;

char *ctable = "0123456789ABCDEF";

// versatile_epb : UART0 at 0x101F1000

void uart_init()
{
  up->base = (UART *)(0x101F1000);
  up->n = 0;
}

void uputc(char c)
{
  while ( *(up->base + FR) & 0x20 ); // while FR.bit5 != 0 ;  (TX FULL)
  *(up->base + DR) = (int)c;         // write c to DR
}

int ugetc()
{
  while ( *(up->base + FR) & 0x10 ); // while FR.bit4 != 0 ;  (RX EMPTY)
  return *(up->base + DR);           // get data in DR
}

int uprints(char *s)
{
  while(*s)
    uputc(*s++);
}

int rpu(u32 x)
{
  char c;
  if (x)
  {
    c = ctable[x % 10];
    rpu(x / 10);
    uputc(c);
  }
}

int uprintu(u32 x)
{
  (x==0)? uputc('0') : rpu(x);
  //putchar(' ');
}

//recursive function for printing int in hex
int rpx(u32 x)
{
  char v = x % 16;
  if (x >> 4 != 0)
    rpx(x >> 4);
  v += (v > 9) ? 'a' - 10 : '0';
  uputc(v);
}

//prints an unsigned 32 bit integer in hex
int uprintx(u32 x)
{
  uputc('0');
  uputc('x');
  rpx(x);
}

//prints a signed 32 bit integer in decimal
int uprintd(int x)
{
  if (x >= 0)
    uprintu(x);
  else
  {
    uputc('-');
    uprintu(-x);
  }
}

int uprintf(char *fmt, ...)
{
  int *ip = (int *)(&fmt) + 1;
  for (char *cp = fmt; *cp != '\0'; ++cp)
  {
    if (*cp != '%')
    {
      uputc(*cp);
      if (*cp == '\n')
        uputc('\r');
    }
    else
    {
      ++cp;
      switch (*cp)
      {
      case 'c':
        uputc(*ip);
        break;
      case 's':
        uprints((char *)*ip);
        break;
      case 'u':
        uprintu(*ip);
        break;
      case 'x':
        uprintx(*ip);
        break;
      case 'd':
        uprintd(*ip);
        break;
      }
      ++ip;
    }
  }
}

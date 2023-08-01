#include "type.h"
#include "string.c"

#include "uart.c"
#include "sdc.c"
#include "boot.c"

int main()
{ 
   uart_init();      
   uprintf("Welcome to ARM EXT2 Booter\n");

   sdc_init();
   boot();

   uprintf("BACK FROM booter\n");
   ugetc();

   go();
}

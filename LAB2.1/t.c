/*********** t.c file **********/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#include "vid.c"
#include "uart.c"

extern char _binary_wsu_bmp_start;

int show_bmp(char *p, int startRow, int startCol)
{ 
   int h, w, pixel, r2, i, j; 
   unsigned char r, g, b;
   char *pp;
   
   int *q = (int *)(p+14); // skip over 14 bytes file header 
   q++;                    // skip 4 bytes in image header
   w = *q;                 // width in pixels 
   h = *(q + 1);           // height in pixels
   p += 54;                // p point at pixels now 

   r2 = 4*((3*w+3)/4);     // row size is a multiple of 4 bytes  
   p += (h-1)*r2;
   for (i=startRow; i < startRow + h; i++){
     pp = p;
     for (j=startCol; j<startCol+w; j++){
         r = *pp; g = *(pp+1); b = *(pp+2);
	 
         pixel = (r<<16) + (g<<8) + b;
         
         if (j < w / 2)
	   fb[i*640 + j] = pixel;
         else
	   fb[i*640 + j] = 0;
         pp += 3 * 2;    // advance pp to next pixel by 3 bytes times the number of pixels
     }
     p -= r2 * 2; //skip every other row
   }
   uprintf("\nBMP image height=%d width=%d\n", h, w);
}

int color;
UART *up;

int main()
{
   char *p;

   uart_init();
   up = &uart[0];

   fbuf_init();

   p = &_binary_wsu_bmp_start;
   show_bmp(p, 0, 0); 

   while(1);   // loop here  
}

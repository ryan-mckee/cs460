/************** more.c file ***************/

#include "ucode.c"

int print_line(fd)
{
  char c;
  int n = read(fd, &c, 1);
  while (c != '\n' && c != '\r')
  {
    putc(c);
    n = read(fd, &c, 1);
    //if file is ended
    if (!n)
      return 1;
  }
  //if file is ended
  if (!n)
    return 1;
  putc(c);
  return 0;
}

int print_screen(fd)
{
  for (int i = 0; i < 25; ++i)
  {
    //if file is ended
    if(print_line(fd))
      return 1;
  }
  return 0;
}

int main(int argc, char *argv[ ])
{
  int fd, end;
  char buf[1024];
  char c, c1;
  //redirect from stdin
  if (argc == 1)
  {
    fd = open("/dev/tty0", O_RDONLY);
    end = print_screen(0);
    //if the file is greater than 25 lines
    while (!end)
    {
      read(fd, &c1, 1);
      c1 = c1&0x7F;
      //if enter is pressed, print line
      if (c1 == '\n' || c1 == '\r' || c1 == 0)
        end = print_line(0);
      if (c1 == ' ')
        end = print_screen(0);
    }
  }
  else if (argc == 2)
  {
    printf("****************Ryan more****************\n");
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
      printf("open %s failed\n", argv[1]);
      exit(1);
    }
    end = print_screen(fd);
    //if the file is greater than 25 lines
    while (!end)
    {
      c1 = getc();
      //if enter is pressed, print line
      if (c1 == '\n' || c1 == '\r')
        end = print_line(fd);
      if (c1 == ' ')
        end = print_screen(fd);
    }
    close(fd);
  }
  else
    printf("incorrect number of arguments\n");
  exit(0);
}

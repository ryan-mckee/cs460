/************** cat.c file ***************/

#include "ucode.c"

int main(int argc, char *argv[ ])
{
  int fd, n;
  char buf[1024];
  //redirect from stdin
  if (argc == 1)
  {
    while(gets(buf))
    {
      printf("%s", buf);
    }
  }
  else
  {
    //printf("****************Ryan cat****************\n");
    //cat files in arguments
    for (int j = 1; j < argc; ++j)
    {
      fd = open(argv[j], O_RDONLY);
      if (fd < 0)
      {
        //printf("open %s failed\n", argv[j]);
        continue;
      }
      while (n = read(fd, buf, 1024))
        for(int i = 0; i < n; i++)
          putc(buf[i]);
      close(fd);
    }
  }
  exit(0);
}


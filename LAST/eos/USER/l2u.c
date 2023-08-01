/************** l2u.c file ***************/

#include "ucode.c"

int main(int argc, char *argv[ ])
{
  int fd1, fd2, n;
  char buf[1024];
  //redirect from stdin
  if (argc == 1)
  {
    while(gets(buf))
    {
      for(int i = 0; i < 1024; i++)
      {
        if (buf[i] >= 'a' && buf[i] <= 'z')
          buf[i] += 'A' - 'a';
      }
      printf("%s", buf);
    }
  }
  else if (argc == 2)
  {
    //print file
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0)
    {
      printf("open %s failed\n", argv[1]);
      exit(1);
    }
    while (n = read(fd1, buf, 1024))
      for(int i = 0; i < n; i++)
      {
        if (buf[i] >= 'a' && buf[i] <= 'z')
          buf[i] += 'A' - 'a';
        putc(buf[i]);
      }
    close(fd1);
  }
  else if (argc == 3)
  {
    printf("****************Ryan l2u****************\n");
    //to file
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0)
      printf("unable to perform l2u, src must exist\n");
    else
    {
      creat(argv[2]);
      fd2 = open(argv[2], O_WRONLY);
      while (n = read(fd1, buf, 1024))
      {
        for(int i = 0; i < n; i++)
        {
          if (buf[i] >= 'a' && buf[i] <= 'z')
            buf[i] += 'A' - 'a';
        }
        write(fd2, buf, n);
      }
      close(fd1);
      close(fd2);
    }
  }
  exit(0);
}


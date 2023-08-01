/************** grep.c file ***************/

#include "ucode.c"

int read_line(int fd, char *buf)
{
  int n = 1;
  for (int i = 0; i < 1024; ++i)
  {
    n = read(fd, buf + i, 1);
    if (!n)
    {
      buf[i] = 0;
      break;
    }
    if (buf[i] == '\n' || buf[i] == '\r')
    {
      buf[i + 1] = 0;
      break;
    }
  }
  buf[1023] = 0;
  return n;
}

int main(int argc, char *argv[ ])
{
  int fd, n;
  char buf[1024];
  int index = 0;
  //redirect from stdin
  if (argc == 1)
  {
    printf("Error, grep must have search pattern");
  }
  else if (argc == 2)
  {
    n = 1;
    while(n)
    {
      //read line into buffer
      n = read_line(0, buf);
      index = 0;
      for (int i = 0; i < 1024; ++i)
      {
        if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == 0)
          break;
        //if character matches
        if (buf[i] == argv[1][index])
          ++index;
        else
        {
          if (buf[i] == argv[1][0])
            index = 1;
          else
            index = 0;
        }
        //if all characters match, print the line
        if (index >= strlen(argv[1]))
        {
          printf("%s", buf);
          break;
        }
      }
    }
  }
  else
  {
    printf("****************Ryan grep****************\n");
    //cat files in arguments
    for (int j = 2; j < argc; ++j)
    {
      fd = open(argv[j], O_RDONLY);
      if (fd < 0)
      {
        //printf("open %s failed\n", argv[j]);
        continue;
      }
      n = 1;
      while (n)
      {
        //read line into buffer
        n = read_line(fd, buf);
        index = 0;
        for (int i = 0; i < 1024; ++i)
        {
          if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == 0)
            break;
          //if character matches
          if (buf[i] == argv[1][index])
            ++index;
          else
          {
            if (buf[i] == argv[1][0])
              index = 1;
            else
              index = 0;
          }
          //if all characters match, print the line
          if (index >= strlen(argv[1]))
          {
            printf("%s", buf);
            break;
          }
        }
      }
      close(fd);
    }
  }
  exit(0);
}


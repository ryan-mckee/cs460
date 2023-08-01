/************** cp.c file ***************/

#include "ucode.c"

int main(int argc, char *argv[ ])
{
  printf("****************Ryan cp****************\n");
  int fd1, fd2;
  int n;
  char buf[1024];
  if (argc != 3)
    printf("incorrect number of arguments, must be in format \"cp src dest\"\n");
  else
  {
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0)
      printf("unable to cp, src must exist\n");
    else
    {
      creat(argv[2]);
      fd2 = open(argv[2], O_WRONLY);
      while (n = read(fd1, buf, 1024))
        write(fd2, buf, n);
      close(fd1);
      close(fd2);
    }
  }
  exit(0);
}

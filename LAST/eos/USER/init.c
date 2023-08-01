/************** init.c file ***************/

#include "ucode.c"

int console, s0, s1;

int parent() // P1's code
{
  int pid, status;
  while(1)
  {
    printf("INIT : wait for ZOMBIE child\n");
    pid = wait(&status);
    if (pid==console)  // if console login process died
    {
      printf("INIT: forks a new console login\n");
      console = fork(); // fork another one
      if (console)
        continue;
      else
        exec("login /dev/tty0"); // new console login process
    }
    if (pid==s0)  // if console login process died
    {
      s0 = fork(); // fork another one
      if (s0)
        continue;
      else
        exec("login /dev/ttyS0"); // new console login process
    }
    if (pid==s1)  // if console login process died
    {
      s1 = fork(); // fork another one
      if (s1)
        continue;
      else
        exec("login /dev/ttyS1"); // new console login process
    }
    printf("INIT: I just buried an orphan child proc %d\n", pid);
  }
}

int main()
{
  int in, out; // file descriptors for terminal I/O
  in = open("/dev/tty0", O_RDONLY); // file descriptor 0
  out = open("/dev/tty0", O_WRONLY); // for display to console
  printf("****************Ryan init****************\n");
  printf("INIT : fork a login proc on console\n");
  console = fork();
  if (console) // parent
  {
    s0 = fork();
    if (s0)
    {
      s1 = fork();
      if (s1)
        parent();
      else // child: exec to login on ttyS1
        exec("login /dev/ttyS1");
    }
    else // child: exec to login on ttyS0
      exec("login /dev/ttyS0");
  }
  else // child: exec to login on tty0
    exec("login /dev/tty0");
}

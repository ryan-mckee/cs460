/********************* sh.c file ***********************/
#include "ucode.c" // user commands and syscall interface
int main(int argc, char *argv[ ])
{
  int pid, status;
  char cmdline[1024];
  char cmd[1024];
  while(1)
  {
    printf("sh %d$ ", getpid());
    //display executable commands in /bin directory
    //prompt for a command line cmdline = "cmd a1 a2 .... an"
    gets(cmdline);
    strcpy(cmd, cmdline);
    token(cmd);
    //if (!strcmp(cmdline,"exit") || !strcmp(cmdline,"logout"))
    if (!strcmp(cmd,"exit") || !strcmp(cmd,"logout"))
    //if (!strcmp(cmd, "exit"))
      exit(0);
    // fork a child process to execute the cmd line
    pid = fork();
    if (pid) // parent sh waits for child to die
      pid = wait(&status);
    else // child exec cmdline
    {
      exec(cmdline); // exec("cmd a1 a2 ... an");
      exit(0);
    }
  }
}

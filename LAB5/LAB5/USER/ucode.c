int umenu()
{
  uprintf("------------------------------------------------------------\n");
  uprintf("getpid getppid ps chname switch kfork sleep wakeup exit wait\n");
  uprintf("------------------------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}

int getPA()
{
  return syscall(92,0,0,0);
}

int argc;
char *argv[32];
 
int token(char *line)
{
  int i;
  char *cp;
  cp = line;
  argc = 0;
  
  while (*cp != 0){
       while (*cp == ' ') *cp++ = 0;        
       if (*cp != 0)
           argv[argc++] = cp;         
       while (*cp != ' ' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  argv[argc] = 0;
}

int ukfork()
{
  return syscall(5,0,0,0);
}

int usleep()
{
  return syscall(6,0,0,0);
}

int uwakeup()
{
  printf("input an event value to wakeup : ");
  int event = geti();
  return syscall(7,event,0,0);
}

int uexit()
{
  printf("input an exit value : ");
  int exitval = geti();
  return syscall(8,exitval,0,0);
}

int uwait()
{
  int status;
  return syscall(9,&status,0,0);
}

main0(char *s)
{
  uprintf("main0: s = %s\n", s);
  token(s);
  main(argc, argv);
}


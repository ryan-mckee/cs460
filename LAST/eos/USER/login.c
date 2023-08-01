/************** login.c file ***************/
// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX

#include "ucode.c"
#define PASSFILESIZE 151

int in, out, err;
char username[128], password[128];
char pass_buf[PASSFILESIZE];

int line_index;
//buffer of tokens: username:password:gid:uid:fullname:HOMEDIR:program
char tok_buf[7][128];

int tokenize(char *line)
{
  for (int j = 0; j < 7; ++j)
  {
    for (int i = 0; i < PASSFILESIZE; ++i, ++line_index)
    {
      char c = line[line_index];
      if (c == EOF || c == 0)
        return 1;
      if (c == ':' || c == '\n' || c == '\r')
      {
        tok_buf[j][i] = 0;
        ++line_index;
        break;
      }
      else
        tok_buf[j][i] = line[line_index];
    }
  }
  return 0;
}

int account_valid()
{
  return (strcmp(username, tok_buf[0]) == 0 && strcmp(password, tok_buf[1]) == 0);
}

int main(int argc, char *argv[])
{
  //printf("****************Ryan login****************\n");
  //(1). close file descriptors 0,1 inherited from INIT.
  close(0);
  close(1);
  //(2). open argv[1] 3 times as in(0), out(1), err(2).
  in = open(argv[1], O_RDONLY);
  out = open(argv[1], O_WRONLY);
  err = open(argv[1], O_RDWR);
  //(3).
  settty(argv[1]); // set tty name string in PROC.tty
  //(4). open /etc/passwd file for READ;
  int pass_file = open("/etc/passwd", O_RDONLY);
  read(pass_file, pass_buf, PASSFILESIZE);
  while(1)
  {
    //(5).
    printf("login:");
    gets(username);
    printf("password:");
    gets(password);
    line_index = 0;
    //for each line in /etc/passwd file do:
    //tokenize user account line;
    while(!tokenize(pass_buf))
    {
      //(6).
      if (account_valid())
      {
        //for (int i = 0; i < 7; ++i)
          //printf("%s|", tok_buf[i]);
        //(7). change uid, gid to user's uid, gid;
        chuid(atoi(tok_buf[2]), atoi(tok_buf[3]));
        //change cwd to user's home DIR
        chdir(tok_buf[5]);
        //close opened /etc/passwd file
        close(pass_file);
        //(8). exec to program in user account
        exec(tok_buf[6]);
      }
    }
    printf("login failed, try again\n");
  }
}

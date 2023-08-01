/************** ls.c file ***************/

#include "ucode.c"

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
struct stat mystat, *sp;

int ls_file(char *fname) // list a single file
{
  struct stat fstat, *sp = &fstat;
  int r, i;
  char sbuf[4096];
  r = stat(fname, sp); // lstat the file
  if (S_ISDIR(sp->st_mode))
    putc('d'); // print file type as d
  if (S_ISREG(sp->st_mode))
    putc('-'); // print file type as -
  if (S_ISLNK(sp->st_mode))
    putc('l'); // print file type as l
  for (i=8; i>=0; i--)
  {
    if (sp->st_mode & (1<<i))
      putc(t1[i]); // print permission bit as r w x
    else
      putc(t2[i]); // print permission bit as -
  }
  printf("\t%d\t", sp->st_nlink); // link count
  printf("%d\t", sp->st_uid); // uid
  printf("%d\t", sp->st_size); // file size
  printf("%s", fname);
  if (S_ISLNK(sp->st_mode))  // if symbolic link
  {
    r = readlink(fname, sbuf);
    printf(" -> %s", sbuf); // -> linked pathname
  }
  putc('\n');
}

int ls_dir(char *dname) // list a DIR
{
  char name[256]; // EXT2 filename: 1-255 chars
  int fd;
  DIR *dp;
  char *cp;
  char buf[1024];
  // open DIR to read names
  fd = open(dname, O_RDONLY);
  read(fd, buf, 1024);
  close(fd);
  dp = (DIR *)buf;
  cp = buf;
  while (cp < buf + 1024)  // readdir() syscall
  {
    strcpy(name, dp->name);
    name[dp->name_len] = 0;
    //if (!strcmp(name, ".") || !strcmp(name, ".."))
      //continue; // skip over . and .. entries
    //strcpy(name, dname);
    //strcat(name, "/");
    //strcat(name, dp->name);
    ls_file(name); // call list_file()
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
}

int main(int argc, char *argv[])
{
  printf("****************Ryan ls****************\n");
  struct stat mystat, *sp;
  int r;
  char *s;
  char filename[1024], cwd[1024];
  s = argv[1]; // ls [filename]
  if (argc == 1) // no parameter: ls CWD
    s = "./";
  sp = &mystat;
  if ((r = stat(s, sp)) < 0)  // stat() syscall
  {
    printf("Error - ls");
    exit(1);
  }
  strcpy(filename, s);
  if (s[0] != '/')  // filename is relative to CWD
  {
    getcwd(cwd); // get CWD path
    strcpy(filename, cwd);
    strcat(filename, "/");
    strcat(filename,s); // construct $CWD/filename
  }
  if (S_ISDIR(sp->st_mode))
    ls_dir(filename); // list DIR
  else
    ls_file(filename); // list single file
}

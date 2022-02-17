/***** LAB4 client base code *****/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>     // for dirname()/basename()
#include <time.h> 

#define MAX 256
#define PORT 1234
#define EOT "\\r\\n\\r\\n"

char line[MAX], ans[MAX];
int n;

struct sockaddr_in saddr; 
int sfd;

int cmd_index(char *command);
int lcat(char *filename);
int lls(char * pathname);
int ls_dir(char *pathname);
int ls_file(char *fname);
int lcd(char *pathname);
int check_end_trans(char * response);
int lpwd();
int lmkdir(char *pathname);
int lrmdir(char *pathname);
int lrm(char *pathname);

char *cmd[] = {"lcat", "lls", "lcd", "lpwd", "lmkdir", "lrmdir", "lrm"};

int (*fptr[])(char *) = {(int (*)())lcat, lls, lcd, lpwd, lmkdir, lrmdir, lrm};

int main(int argc, char *argv[], char *env[]) 
{ 
    int n; char how[64];
    int i;
    char command[16], arg[64];
    printf("1. create a socket\n");
    sfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sfd < 0) { 
        printf("socket creation failed\n"); 
        exit(0); 
    }
    
    printf("2. fill in server IP and port number\n");
    bzero(&saddr, sizeof(saddr)); 
    saddr.sin_family = AF_INET; 
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    saddr.sin_port = htons(PORT); 
  
    printf("3. connect to server\n");
    if (connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 

    printf("********  processing loop  *********\n");
    while (1){
      printf("input a line : ");
      bzero(line, MAX);                // zero out line[ ]
      fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

      line[strlen(line)-1] = 0;        // kill \n at end
      if (line[0]==0)                  // exit if NULL line
         exit(0);



      sscanf(line, "%s %s", command, arg);
      if(cmd_index(command) != -1)
      {
        printf("%s\n", command);

        int index = cmd_index(command);
        if (index != -1)
        {
          fptr[index](arg);
        }

        else
        {
          printf("invalid command %s\n", line);
        }
      }

      else
      {
        n = write(sfd, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        char command[16], arg[64];
        sscanf(line, "%s %s", command , arg);
        char buf[MAX];

        if(!strcmp(command , "get"))
        {
          int fd;
          int b = read(sfd, buf, 0);
          int fsize = atoi(buf);
          memset(buf, 0, sizeof(buf));

          fd = open(arg , O_WRONLY|O_CREAT, 0644);
          if(fd > 0)
          {
            int read_bytes = 0;
            int packet_size = 0;
            while (fsize > 0)
            {
              read(sfd,buf,MAX);
            }
          }
        }
      }

      
      // Send ENTIRE line to server
      

      // Read a line from sock and show it
      n = read(sfd, ans, MAX);
      printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
  }
}


/********************* YOU DO ***********************
    1. The assignment is the Project in 13.17.1 of Chapter 13

    2. Implement 2 sets of commands:

      ********************** menu ***********************
      * get  put  ls   cd   pwd   mkdir   rmdir   rm  *  // executed by server
      * lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *  // executed LOACLLY (this file)
      ***************************************************

    3. EXTRA Credits: make the server MULTI-threaded by processes

    Note: The client and server are in different folders on purpose.
          Get and put should work when cwd of client and cwd of server are different.
****************************************************/

int check_end_trans(char *response)
{
  if(!strcmp(response, EOT))
  {
    printf("End of transmission\n");

    return 1;
  }

  return 0;
}

int cmd_index(char *command)
{
  int i = 0;

  while(cmd[i])
  {
    if(!strcmp(command, cmd[i]))
    {
      printf("Found %s index %d\n", command , i);

      return i;
    }

    i++;
  }

  return -1;
}

int lmkdir(char *pathname)
{
  int r = mkdir(pathname,0755);

  return r;
}

int lrmdir(char *pathname)
{
  int r = rmdir(pathname);

  return r;
}

int lrm(char *pathname)
{
  int r = unlink(pathname);

  return r;
}

int lcd(char *pathname)
{
  int r = chdir(pathname);

  return r;
}

int lpwd()
{
  char buf[MAX];
  getcwd(buf,MAX);
  printf("%s\n", buf);
}


int ls_file(char *fname)
{
  char linkname[MAX];
  char *t1 = "xwrxwrxwr-------";
  char *t2 = "----------------";
  struct stat fstat, *sp;
  int r,i;
  char ftime[64];
  sp = &fstat;
  if((r=lstat(fname,&fstat))< 0)
  {
    printf("can't stat %s\n", fname);
    exit(1);
  }

  if((sp->st_mode & 0xF000) == 0x8000) ///if (S_ISREG())
  {
    printf("%c", '-');
  }

  if((sp->st_mode & 0xF000) == 0x4000) ///if (S_ISDIR())
  {
    printf("%c", 'd');
  }

  if((sp->st_mode & 0xF000) == 0xA000) ///if (S_ISLINK())
  {
    printf("%c", 'l');
  }

  for(i=8;i>0;i--)
  {
    if(sp->st_mode & (1 << i))
    {
      printf("%c", t1[i]);
    }

    else
    {
      printf("%c", t2[i]);
    }
  }

  printf("%4d", sp->st_nlink);
  printf("%4d", sp->st_gid);
  printf("%4d", sp->st_uid);
  printf("%8d", sp->st_size);
  strcpy(ftime,ctime(&sp->st_ctime));
  ftime[strlen(ftime-1)] = 0;
  printf("%s", ftime);
  printf("%s", basename(fname));

  if((sp->st_mode & 0xF000)== 0xA000)
  {
    readlink(fname,linkname, MAX);
    printf("-> %s", linkname);
  }

  printf("\n");
}

int ls_dir(char *pathname)
{
  struct dirent *dp;
  DIR *dir;
  char fpath[MAX *2];
  memset(fpath, '\0', sizeof(fpath));

  if((dir = opendir(fpath) == NULL)) 
  {
    perror("No pathname available for open.\n");
    return 1;
  }
  

  do
  {
    if((dp = readdir(dir)) != NULL)
    {
      memset(fpath, '\0', sizeof(fpath));
      strcpy(fpath,pathname);
      strcat(fpath, "/");
      strcat(fpath, dp->d_name);
      ls_file(fpath);
    }
  }while(dp != NULL);

  closedir(dir);
  return 0;
}

int lls(char *pathname)
{
  if(!strcmp(pathname,""))
  {
    ls_dir("./");
    return 1;
  }
  ls_dir(pathname);
  return 0;
}

int lcat(char *filename)
{
  char buf[512];
  FILE *fd = fopen(filename, "r");
  if (fd != NULL)
  {
    while(fgets(buf,512,fd) != NULL)
    {
      buf[strlen(buf) - 1] = '\0';
      puts(buf);
    }
  }

  else
  {
    printf("failed to fopen.\n");
    return 1;
  }

  return 0;
}
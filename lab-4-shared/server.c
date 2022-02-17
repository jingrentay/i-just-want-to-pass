/***** LAB4 server base code *****/ 

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
#include <libgen.h>
#include <time.h>

#define MAX 256
#define PORT 1234

int n;

char ans[MAX];
char line[MAX];
;
struct stat mystat, *sp

int ls_file(char *fname)
{
  struct stat fstat, *sp;
  int r, i;
  char ftime[64];
  sp = &fstat;
  if((r = lstat(fname, &stat)) < 0){
    printf("can't stat %s\n", fname);
    exit(1);
  }
  if((sp->st_mode & 0xF000) == 0x8000){
    printf("%c", '-');
  }
  if((sp->st_mode & 0xF000) == 0x4000){
    printf("%c", 'd');
  }
  if((sp->st_mode & 0xF000) == 0xA000){
    printf("%c", 'l');
  }
  for(i=8; i>=0; i--){
    if(sp->st_mode & (1 << i))
      printf("%c", t1[i]);
    else
      printf("%c", t2[i]);
  }
  printf("%4d ", sp->st_nlink); // link count
  printf("%4d ", sp->st_gid); // gid
  printf("%4d ", sp->st_uid); // uid
  printf("%4d ", sp->st_size); // file size
  // print time
  strcpy(ftime, ctime(&sp->st_ctime));
  ftime[strlen(ftime)-1];
  printf("%s ", ftime);
  // print time
  printf("%s", basename(fname));
  // print -> linkname if symbolic file
  if ((sp->st_mode & 0xF000) == 0xA000){
    // use readlink() to read linkname
    printf(" -> %s", linkname);
  }
  printf("\n");
}

int ls_dir(char *dname){
  // use opendir(), readdir(); call ls_file(name)
}

int server_mkdir(char *pathname)
{
  return mkdir(pathname, 0755);
}

int server_rmdir(char *pathname)
{
  return rmdir(pathname);
}

int server_rm(char *pathname)
{
  return unlink(pathname);
}

int server_cd(char *pathname)
{
  return chdir(pathname);
}

int server_pwd()
{
  char buf[MAX];
  getcwd(buf, MAX);
  printf("%s\n", buf);
}

int main() 
{ 
    int sfd, cfd, len; 
    struct sockaddr_in saddr, caddr; 
    int i, length;
    
    printf("1. create a socket\n");
    sfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sfd < 0) { 
        printf("socket creation failed\n"); 
        exit(0); 
    }
    
    printf("2. fill in server IP and port number\n");
    bzero(&saddr, sizeof(saddr)); 
    saddr.sin_family = AF_INET; 
    //saddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    saddr.sin_port = htons(PORT);
    
    printf("3. bind socket to server\n");
    if ((bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr))) != 0) { 
        printf("socket bind failed\n"); 
        exit(0); 
    }
      
    // Now server is ready to listen and verification 
    if ((listen(sfd, 5)) != 0) { 
        printf("Listen failed\n"); 
        exit(0); 
    }
    while(1){
       // Try to accept a client connection as descriptor newsock
       length = sizeof(caddr);
       cfd = accept(sfd, (struct sockaddr *)&caddr, &length);
       if (cfd < 0){
          printf("server: accept error\n");
          exit(1);
       }

       printf("server: accepted a client connection from\n");
       printf("-----------------------------------------------\n");
       printf("    IP=%s  port=%d\n", "127.0.0.1", ntohs(caddr.sin_port));
       printf("-----------------------------------------------\n");

       // Processing loop
       while(1){
         printf("server ready for next request ....\n");
         n = read(cfd, line, MAX);
         if (n==0){
           printf("server: client died, server loops\n");
           close(cfd);
           break;
         }

         // show the line string
         printf("server: read  n=%d bytes; line=[%s]\n", n, line);

         strcat(line, " ECHO");

         // send the echo line to client 
         n = write(cfd, line, MAX);

         printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
         printf("server: ready for next request\n");
       }
    }
}

/********************* YOU DO ***********************
    1. The assignment is the Project in 13.17.1 of Chapter 13

    2. Implement 2 sets of commands:

      ********************** menu ***********************
      * get  put  ls   cd   pwd   mkdir   rmdir   rm  *  // executed by server (this file)
      * lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *  // executed LOACLLY
      ***************************************************

    3. EXTRA Credits: make the server MULTI-threaded by processes
    
    Note: The client and server are in different folders on purpose.
          Get and put should work when cwd of client and cwd of server are different.
****************************************************/


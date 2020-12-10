////WELCOME TO THE LINUX SHELL PROJECT///////////
/*
Behnam Sobhani Nadri and Michele Catalano Wrote this Project on December 2020
*/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAXLINE 200
#define MAXARG 20

   /////// Global Vriables //////
   char s[MAXLINE];
   char* token;
   char rest;
   char* cmd[MAXARG];

   int main(void) {
   pid_t pid;
   int status;
   printf("WELCOME TO MY NEO MODERN UNIX SHELL:\n");
   ////open loop is needed to receive commands every cycle
   while(1){
      printf(">>>>>>>>>>>> ");
      char *rest=fgets(s,MAXLINE,stdin);
      int i=0;
      while((token=strtok_r(rest," \n",&rest))){
         cmd[i]=token;
         i++;
         //printf("cmd[%d] is %s\n\n",i, cmd[i]);
         }
         if(strcmp(cmd[0],"exit") == 0 ) break;          
         pid = fork();
   
     if (pid == 0) {
        //code for child process only
        char* fp;
        sprintf(fp,"%s_%d.log",cmd[0],getpid());
        printf("%s\n\n",fp);
        int fd = open(fp,O_WRONLY|O_CREAT,0777);
        dup2(fd,1);
        //execlp(cmd[0], cmd[0], (char*) NULL); 
        execvp(cmd[0], cmd);       
        close(fd);
        exit(0);
     } else if (pid == -1) {
        printf("Failed to create child process\n");
     } else {
        waitpid(pid, &status, 0);
     }
  }      
return 0;
}

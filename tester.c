#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define DEBUG 0 
void debugp(char* msg) {if (DEBUG) fprintf(stderr, "%s\n", msg); return;}

// first argument is how many procs to fork
// second argument is the program name to fork
// and then the arguments for given program
int main(int argc, char* argv[]) {

  int i=0;
  int forkcount=0; 
  
  char err[1024];
  if (argc < 3) {
    fprintf(stderr, "usage: ./tester <# of forks> <program name> <program arguments*>\n");
    exit(0);
  }

  forkcount = atoi(argv[1]);
  if (forkcount < 1) {
    fprintf(stderr, "<# of forks> must be greater than 0");
    exit(0);
  }
  int status[forkcount];  
  pid_t cpid[forkcount];
  sprintf(err, "trying to fork %d procs", forkcount);
  debugp(err);
  
  for (i=0; i < forkcount; ++i) {
    debugp("trying to fork");    
    cpid[i] = fork();
    if (cpid[i] == -1) {
       fprintf(stderr, "error forking: %s\n", strerror(errno));
       exit(-1);
    }
    if (cpid[i] == 0) {
    execvp(argv[2], argv+2);
    exit(errno);
    }
  }

  debugp("\nentering main block\n");
  for (i=0; i < forkcount; ++i) {
    if (waitpid(cpid[i], &status[i], 0) != cpid[i]) {
      fprintf(stderr, "waitpid failed!\n");
    }
  }

  for (i=0; i < forkcount; ++i) {
    printf("\nproc %d returned with status %d\n", cpid[i], WEXITSTATUS(status[i]));
  }
   
  return 0;
}

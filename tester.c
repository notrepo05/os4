#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG 1 
void debugp(char* msg) {if (DEBUG) fprintf(stderr, "%s\n", msg); return;}

// first argument is how many procs to fork
// second argument is the program name to fork
// and then the arguments for given program
int main(int argc, char* argv[]) {

  pid_t cpid[argc];
  int i=0;
  int forkcount=0; 
  
  char err[102500];
  if (argc < 3) {
    fprintf(stderr, "usage: ./tester <# of forks> <program name> <program arguments*>\n");
    exit(-1);
  }

  forkcount = atoi(argv[1]);
  int status[forkcount];  
  sprintf(err, "trying to fork %d procs", forkcount);
  debugp(err);
  
  for (i=0; i < forkcount; ++i) {
    debugp("trying to fork");    
    cpid[i] = fork();
    if (cpid[i] == 0) break; // child proc. we don't want n! behavior
  }
  if (cpid[i] == 0) {
    //sprintf(err, "\nI'm a child proc trying to execvp %s.\n", argv[2]);
    //debugp(err);
    execvp(argv[2], argv+3);
    return 0;
  }
  else {
  debugp("\nentering main block\n");
  for (i=0; i < forkcount; ++i) {
    if (waitpid(cpid[i], &status[i], 0) != cpid[i]) {
      fprintf(stderr, "waitpid failed!\n");
    }
  }

  for (i=0; i < forkcount; ++i) {
    printf("\nproc %d returned with status %d\n", cpid[i], status[i]);
  }
   
  return 0;
  }

}

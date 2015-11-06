#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sched.h>

#define USAGE "for help type ./tester help.\n usage: ./tester <scheduler> <# of forks> <program name> <program arguments*>"
#define SCHED_USAGE "<scheduler> must be an integer of value 1, 2, or 3.\n Reference: SCHED_FIFO(1), SCHED_RR(2), SCHED_NORM(3)"
#define DEBUG 0

// first argument is how many procs to fork
// second argument is the program name to fork
// and then the arguments for given program
int main(int argc, char* argv[]) {

  int i=0;
  int forkcount=0; 
  struct sched_param sparam; 
 
  if ((argc == 2) && (strcmp(argv[1], "help") == 0)) {
    printf("%s\n%s\n", USAGE, SCHED_USAGE);
    exit(0);
  }
  if (argc < 4) {
    fprintf(stderr, "%s\n", USAGE);
    exit(0);
  }

  int sched = atoi(argv[1]);
  if (sched < 1 || sched > 3) {
    fprintf(stderr, "%s\n", "scheduler must be between 1 and 3. see ./tester help for help");
    exit(0);
  }
  forkcount = atoi(argv[2]);
  if (forkcount < 1) {
    fprintf(stderr, "<# of forks> must be greater than 0");
    exit(0);
  }
  int status[forkcount];  
  pid_t cpid[forkcount];
  if (DEBUG) printf("trying to fork %d procs on scheduler %d", forkcount, sched);
  

  // set the scheduling policy
      int sched_res;
      switch (sched) {
        case 1:
          sparam.sched_priority = sched_get_priority_max(SCHED_FIFO); 
          if ((sched_res = sched_setscheduler(getpid(), SCHED_FIFO, &sparam)) == -1) {
            fprintf(stderr, "%s ret: %d\n", "couldn't change scheduling policy to SCHED_FIFO!", sched_res);
            exit(-1);
          }
          break;
        case 2: 
          sparam.sched_priority = sched_get_priority_max(SCHED_RR);
          if ((sched_res = sched_setscheduler(getpid(), SCHED_RR, &sparam)) == -1) {
            fprintf(stderr, "%s ret %d\n", "couldn't change scheduling policy to SCHED_RR!", sched_res);
            exit(-1);
          }
          break;
        case 3: 
          break;
        default: 
          fprintf(stderr, "internal error!");
          exit(-1);
      }


  for (i=0; i < forkcount; ++i) {
    if (DEBUG) printf("%s\n", "trying to fork");    
    cpid[i] = fork();
    if (cpid[i] == -1) {
       fprintf(stderr, "error forking: %s\n", strerror(errno));
       exit(-1);
    }
    if (cpid[i] == 0) {
      if (DEBUG) printf("\nwas running on scheduler %d\n", sched_getscheduler(cpid[i]));
      execvp(argv[3], argv+3);
      exit(errno);
    }
  }

  if (DEBUG) printf("%s\n", "entering main block");
  for (i=0; i < forkcount; ++i) {
    if (waitpid(cpid[i], &status[i], 0) != cpid[i]) {
      fprintf(stderr, "waitpid failed!\n");
    }
  }

  for (i=0; i < forkcount; ++i) {
    if (DEBUG) printf("\nproc %d returned with status %d\n", cpid[i], WEXITSTATUS(status[i]));
  }
   
  return 0;
}

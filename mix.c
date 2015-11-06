/** Author: Nick Rohn
 * this file is intended to randomly compute interleaved
 * io and cpu bound processes. 
 */

#include <stdlib.h>
#include <stdio.h>

// horrible fib

long long hfib(long long a) {
  a = a - 1;
  if (a <= 1) return 1;
  return hfib(a-1) + hfib(a-2);
}

int main(int argc, char* argv[]) {

  if (argc < 4) {
    printf("%s\n", "Usage: ./mix <count> <io-to-cpu ratio max> <hfib max>");
    exit(-1);
  }

  int count = atoi(argv[1]); 
  int iocpur = atoi(argv[2]);
  int hfibc = atoi(argv[3]);

  if (count < 1 || iocpur < 1 || hfibc < 1) {
    fprintf(stderr, "error: %s\n", "<count> must be greater than 0");
    exit(-1);
  }
  
  const char* read_path = "/dev/urandom";
  const char* write_path = "/dev/null";

  FILE *p = fopen(read_path, "r");
  if (p == NULL) {
    fprintf(stderr, "failed to open %s\n", read_path);
    exit(-1);
  }

  FILE *pr = fopen(write_path, "w");
  if (pr == NULL) {
    fprintf(stderr, "failed to open %s\n",write_path);
    fclose(p);
    exit(-1);
  }

  // read and write for no reason
  size_t read;
  char *buff;
  int j;
  while(count-- > 0) {
    // do io bound process
    for (j=0; j < rand() % iocpur; ++j) {
      getline(&buff, &read, p);
      fprintf(pr, "%s", buff);
    }
    hfib(rand() % hfibc);
    //printf("from %s: %s\n", read_path, buff);
  }
  free(buff);
  fclose(pr);
  fclose(p);
  return 0;
}

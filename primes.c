#include "types.h"
#include "stat.h"
#include "user.h"

int isPrime(int n) {
  if (n < 2) return 0;
  if (n == 2) return 1;
  if (n % 2 == 0) return 0;

  int i;
  for(i=3; i*i <= n; i += 2) {
    if (n % i == 0) return 0;
  }
  return 1;
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    printf(2, "Usage: n\n");
    exit();
  }

  int begin = 2;
  int end = atoi(argv[1]);
  int i;
  
  for(i = begin; i <= end; i++) {
    if (isPrime(i) == 1) {
      printf(1, "%d\n", i);
    }
  }

  exit();
}

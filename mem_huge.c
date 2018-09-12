/*
  Author:   Jeff Kinne

  Purpose:  How much memory can we allocate with malloc before it fails?
            And how does the system behave while this happens?
 */
#include "types.h"
#include "stat.h"
#include "user.h"


int
main(int argc, char *argv[])
{
  // if there is an argument, then it is the # of bytes
  // to get up to.  if not argument, then -1 to mean no limit
  int maxBytes = -1;
  if (argc > 1) {
    maxBytes = atoi(argv[1]);
  }

  // start off allocating 1 byte
  int memSize = 1;
  char * ptr = 0;

  // and double the amount in this loop repeatedly until it 
  // fails or we reach maxBytes
  while (memSize < maxBytes || maxBytes < 0) {
    printf(1, "Attempting to allocate %d bytes of memory.\n", memSize);
    ptr = malloc(memSize);
    if (ptr != 0) 
      printf(1," -- success, starts at 0x%p\n", ptr);
    else {
      printf(1," ** FAILED\n");
      break;
    }
    free(ptr);
    memSize *= 2;
  }
  
  exit();
}

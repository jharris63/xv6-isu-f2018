/*
  Author:  Jeff Kinne
  Purpose: Make the program fault, see what happens.  The OS should get control and kill the process
 */
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  // standard checking of argc and usage statement
  if (argc < 2) {
    printf(1, "Usage: ./fault type\n");
    printf(1, " 0 - none\n");
    printf(1, " 1 - seg fault\n");
    printf(2, " 2 - divide by 0\n");
    exit();
  }
  // standard grabbing command line argumen ts
  int which = atoi(argv[1]);

  // used below
  int zero = 0;
  int * bad_p =(int *) 0x80000000; 

  // so I can see on the screen how far we got
  printf(1,"Faulting, based on argument...\n");

  if (which == 0)
    ;
  else if (which == 1) {
    which = *bad_p; // supposed to be bad memory access, because in xv6, kernel code starts at virtual memory address bad_p
    printf(1, "which = %d", which); // shouldn't see this print out
  }
  else if (which == 2) { 
    which = which / zero; // had which / 0, but the compiler caught that
    printf(1, "which = %d", which); // shouldn't see this print out
  }

  // note: there are other problems we could try to make happen

  // if there was a fault, shouldn't see this either
  printf(1,"Note - got here without dying...\n");
  
  exit();
}

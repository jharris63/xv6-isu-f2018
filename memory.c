/*
  Author:   Jeff Kinne
  Purpose:  Seeing what memory addresses get printed for different types of things.
 */
#include "types.h"
#include "stat.h"
#include "user.h"

// a global variable, in the "text" or "data" section of memory
int globalVar = 42;


int
main(int argc, char *argv[])
{
  printf(1, "Memory addresses...\n");
  printf(1, "address of main:  %p\n", main); // memory address where the main function is, should be in text or data
  printf(1, "data, &globalVar: %p\n", &globalVar);
  printf(1, "stack, &argc:     %p\n", &argc); // a local/argument, should be on "the stack"
  char *s = (char *) malloc(10 * sizeof(char)); // should be on "the heap"
  strcpy(s,"abcdefg");
  printf(1, "heap, s:          %p\n", s);

  if (argc > 1) {
    char *p = (char *) atoi(argv[1]);
    printf(1, "&%p = %x\n", p, *p);
  }

  // sleep forever so that we can have this program running in the background if so desired
  printf(1, "now sleeping forever 1000000 ticks at a time\n");
  while (1) sleep(1000000);
  
  exit();
}

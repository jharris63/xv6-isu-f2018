/*
  Author:   Jeff Kinne

  Purpose:  Use a system call that we created, to get usage information about this
            process
 */
#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"


int
main(int argc, char *argv[])
{
  struct proc_usage u;
  printf(1, "Running my system call...\n");
  usage(&u);
  printf(1, " memory_size = %d\n", u.memory_size);
  
  exit();
}

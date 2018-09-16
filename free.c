/*
  Author:   Joseph Harris

  Purpose:  Use system_load syscall that I created to output system information
  	    like:
	    	$ free [-h]
		 #procs: 3, size(uvm) 40000, ncpu 2
            -h will output size(uvm) in human-readable form like "40KB"

	    system call "system_load" code is in sysproc.c
 */
#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"

char *abbr[] = {"B", "KB", "MB", "GB"};

int convert(int num) {
  int bytes = 0;
  if(num > 2000 && num < 2000000)
    bytes = 1;
  else if(num > 2000000 && num < 2000000000)
    bytes = 2;
  else if(num > 2000000000)
    bytes = 3;
    
  for(int i = 0; i < bytes; i++)
    num /= 1000;

  return num;
}

int get_suff(int num) {
  int in = 0;
  if(num > 2000 && num < 2000000) in = 1;
  else if(num > 2000000 && num < 2000000000) in = 2;
  else if(num > 2000000000) in = 3;

  return in;
}

int
main(int argc, char *argv[])
{
  struct system_info u;
  printf(1, "Running my system call ...\n");
  system_load(&u); 			//pass u to my syscall that loads system information into it
  
  if (strcmp(argv[1], "-h") == 0) {	//human-readable
    int index = get_suff(u.uvm_used);
    printf(1, " #procs: %d, size(uvm) %d%s, ncpu %d\n", u.num_procs, convert(u.uvm_used), abbr[index], u.num_cpus);
  } 
  else printf(1, " #procs: %d, size(uvm) %d, ncpu %d\n", u.num_procs, u.uvm_used, u.num_cpus);

  exit();
}

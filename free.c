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

// a global variable
char g = 0xf0;

int convert(int num, int bytes) {
    
  for(int i = 0; i < bytes; i++)
    num /= 1000;

  return num;
}

int
main(int argc, char *argv[])
{
  printf(1, " Note: run ./free -usage for usage statement\n");
  int human = 0;
  int flags = 0;
  int local=0, global=0, heap=0, code=0;
  struct system_info u;
  //printf(1, "Running my system call ...\n");
  system_load(&u); 			//pass u to my syscall that loads system information into it
  
  int i = 0;
  for(;i < argc; i++) {
    if (strcmp(argv[i], "--") == 0 || strcmp(argv[i], "-usage") == 0) {
      printf(1, 
	     "Usage: ./free [flags]\n"
	     " -usage or -- -> usage statements\n"
	     " -h -> human friendly sizes\n"
	     " -p -> print trace of pointer through virtual memory\n"
	     " -f -> print info about list of free pages\n"
	     " -t -> print info about page directories and page tables\n"
	     " -global -> pointer should be global variable\n"
	     " -local -> pointer should be local variable\n"
	     " -heap -> pointer should be heap variable\n"
	     " -code -> pointer should be in code\n"
	);
    }
    else if (strcmp(argv[i], "-h") == 0) human = 1;
    else if (strcmp(argv[i], "-p") == 0) flags |= PAGE_DIR_DUMP_POINTER;
    else if (strcmp(argv[i], "-f") == 0) flags |= PAGE_DIR_DUMP_FREE;
    else if (strcmp(argv[i], "-t") == 0) flags |= PAGE_DIR_DUMP_TABLES;
    else if (strcmp(argv[i], "-global") == 0) global = 1;
    else if (strcmp(argv[i], "-local") == 0) local = 1;
    else if (strcmp(argv[i], "-heap") == 0) heap = 1;
    else if (strcmp(argv[i], "-code") == 0) code = 1;
  }

  if (human) {	  //human-readable
    
    char *abbr[] = {"B", "KB", "MB", "GB"};
    int index=0, r = u.uvm_used;
    if(r > 2000 && r < 2000000) {
      index = 1;
      r = convert(r, index);
    } else if(r > 2000000 && r < 2000000000) {
      index = 2;
      r = convert(r, index);
    } else if(r > 2000000000) {
      index = 3;
      r = convert(r, index);
    }

    printf(1, "#procs: %d, size(uvm) %d%s, ncpu %d\n", u.num_procs, r, abbr[index], u.num_cpus);
  } 
  else printf(1, "#procs: %d, size(uvm) %d, ncpu %d\n", u.num_procs, u.uvm_used, u.num_cpus);

  //a local variable
  char c = 0x5a;

  //a variable on the heap
  char *h = (char *) malloc(sizeof(char));
  *h = 0x76;

  //depending on options, set pointer p to be pointing
  //to local, global, heap, or code
  char *p;
  if (local) p = &c;
  else if (global) p = &g;
  else if (heap) p = h;
  else if (code) p = (char *) main;
  else p = &c; // note: default to doing local if none of those are set

  //if doing the pointer trace, print the value going in. at the end
  //of things in the page_dir_dump system call, should also have the same value.
  if (flags & PAGE_DIR_DUMP_POINTER)
    printf(1, "Note - passing in value 0x%x, address 0x%p to page_dir_dump...\n", *p, p);
  page_dir_dump(p, flags);

  free(h);
  exit();
}

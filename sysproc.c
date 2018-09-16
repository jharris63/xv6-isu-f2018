#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
// ISU-f2018, added so can print inode information for debugging
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// ISU-f2018
void
mem_dump(void)
{
  //struct proc *p = myproc();
  // here!
  cprintf(" -- mem dump\n");
  
  cprintf(" mem dump --\n");
}

// ISU-f2018
void
proc_dump(void)
{
  struct proc *p = myproc();
  cprintf(" -- struct proc dump\n");
  cprintf(" sz\t%d\n",p->sz);
  cprintf(" pgdir\t%p\n",p->pgdir);
  cprintf(" kstack\t%p\n",p->kstack);
  cprintf(" state\t%d\n",p->state);
  cprintf(" pid\t%d\n",p->pid);
  if (p->parent != 0)
    cprintf(" parent\t%p, %s\n",p->parent, p->parent->name);
  else
    cprintf(" parent\t0\n");
  cprintf(" tf\t%p\n",p->tf);
  cprintf(" context\t%p\n",p->context);
  cprintf(" chan\t%p\n",p->chan);
  cprintf(" killed\t%d\n",p->killed);
  cprintf(" ofile\t%p\n",p->ofile);
  if (p->cwd != 0)
    cprintf(" cwd\t%p - dev %d, inum %d\n",p->cwd,p->cwd->dev,p->cwd->inum);
  else
    cprintf(" cwd\t0\n");
  cprintf(" name\t%s\n",p->name);
  cprintf(" struct proc dump --\n");
}


// ISU-f2018
int
sys_usage(void)
{
  // cprintf("Hello World.\n");
  struct proc_usage *u;
  if (argptr(0, (char **) &u, sizeof(struct proc_usage)) < 0)
    return -1;

  u->memory_size = myproc()->sz;

  // debug print proc state...
  if (1) {
    proc_dump();
    mem_dump();
  }
  
  return 0;
}

struct { //copied from proc.c for system_load
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

//Joseph Harris ISU-f2018
//load system information
//access the process table and find used processes, memory used by the processes
//and the number of cpus running on the system
int sys_system_load(void)
{
  //cprintf("Hello, world!\n");
  struct system_info *si;
  struct proc *p;

  if (argptr(0, (char**) &si, sizeof(struct system_info)) < 0) 
    return -1;

  //basically the first quarter of allocproc()
  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) //go through the process
    if(p->state != UNUSED) {			     //table.
      si->num_procs++;				     //as long as proc isn't UNUSED, keep track of it
      si->uvm_used+=p->sz;			     //add the processes size (sz) to the running total in si->uvm_used
    }

  si->num_cpus = ncpu;				     //ncpu is a global declared in proc.h and calculated in mp.c
  release(&ptable.lock);

  return 0;
}

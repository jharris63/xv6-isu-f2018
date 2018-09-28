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

//Joseph Harris ISU-f2018
//system call to dump page directory information
char *sys_page_dir_dump(char *s, int f)
{
  argstr(0, &s);
  argint(1, &f);
  struct proc *p = myproc();			//start a process that won't get rescheduled or interrupted
  pde_t *pd = p->pgdir; 			//get that procs page directory
  pde_t *pde = &pd[PDX(s)];			//just like pd[25] but it is the address of the pointer
  pde_t *pgtadd = P2V((pte_t*)PTE_ADDR(*pde));	//page table virtual address
  pde_t pgphys = (pde_t)V2P(pgtadd[PTX(s)]);	//page physical address...just like pgtadd[20] but with the table index of s
  pte_t pg = (PTE_ADDR(pgphys)+(pgphys&0xfff))-0x80000000;//page address...adding the page table entry of pgphys and the offset bits.
  							  //subtract 0x80000000 because v2p was not being reliable...


  if(f==PAGE_DIR_DUMP_POINTER) {//-p
    uint array[9] = {PTE_P, PTE_W, PTE_U, PTE_PWT, PTE_PCD, PTE_A, PTE_D, PTE_PS, PTE_MBZ}; //all of the offset flags for a page address
    char *flags[9] = {"present", "writable", "user", "write-through", "cache-disable", "accessed", "dirty", "page size", "bits must be zero"};
    						//arrays make testing flags and printing them much easier
    
    cprintf("\n\n-- tracing lookup of pointer --\n");
    cprintf("virtual address:\t0x%x\n..\n",s+PTX(s));
    
    cprintf("page directory index:\t0x%x\n"
	    "page table index:\t0x%x\n"
	    "offset in page:\t\t0x%x\n..\n",
    	    PDX(s), PTX(s), ((uint)s&0xfff)+PTX(s)
	    );//just printing a bunch of things...
    
    cprintf("page directory:\t\t0x%x\n"
	    "pde flags:\t\t0x%x\n", *pde, *pde&0xfff);
    cprintf("  pde flags: ");
    for (int i = 0; i<9; i++) {//loop though flags and test their values
      if (*pde&array[i]) cprintf("%s, ", flags[i]);
    }
    cprintf("\npage table address:\t0x%x\n..\n",
	    PTE_ADDR(*pde));

    cprintf("page table entry:\t0x%x\n"
	    "pte flags:\t\t0x%x\n"
	    "  pte flags: ",
	    pg, pg&0xfff);
    for (int i = 0; i<9; i++) {
      if (pg&array[i]) cprintf("%s, ", flags[i]);
    }
    cprintf("\npage address:\t\t0x%x\n..\n", PTE_ADDR(pg));
    
    pde_t loc = (pde_t)P2V(((PTE_ADDR(pg)|((uint)s&0xfff))+PTX(s))); //the math way of finding the address
  
    cprintf("physical location:\t0x%x\n"
	    "value at location:\t0x%x\n\n",
	    V2P_WO(loc), *s
	   );
    
  } else if (f == PAGE_DIR_DUMP_TABLES){ //-t
    int count = 0, ptcount = 0, bcount = 0, kerncount = 0, memcount = 0, physcount = 0, devcount = 0, total = 0;
    cprintf("PDE's for virtual addresses below KERNBASE -\n");
    for (uint i=0x0; pde[i]; i++) { //existing pde's
      if((pde[i]&PTE_P) && pde[i]<KERNBASE) {
	pde_t vpg = PTE_ADDR(pde[i])+0x80000000;
	cprintf(" PDE: row 0x%x (so virtual address 0x%x), pde 0x%x, page at 0x%x\n", i, i<<12, pde[i], vpg);
        for (uint j=0x0; pgtadd[j]; j++) {//existing pte's
	  if(pgtadd[j]&PTE_P) {
	    pde_t pagead = PTE_ADDR(pgtadd[j]);
	    cprintf("  PTE: row 0x%x (so virtual address 0x%x), pte 0x%x, physical page at 0x%x\n", j, j<<12, (pagead|(pgtadd[j]&0xfff)), pagead);
	    count++;
	  }
	}
      }
      ptcount++;
      cprintf("  - %d pages present in that PT\n\n", count);
    }
    //count existing entries depending on placement and possible amount of entries in a range.
    for(int i=0; i<1024; i++){
       if(pde[i]) {
         if(i < 512 && i >= 0) {
	   bcount++;
	   total++;
	 }
	 else if(i == 512) {
   	   kerncount++;
	   total++;
	 }
	 else if(i > 512 && i < 568) {
	   memcount++;
	   total++;
	 }
	 else if(i > 567 && i < 1016) {
	   physcount++;
	   total++;
	 }
	 else if(i > 1015 && i < 1025) {
	   devcount++;
	   total++;
	 }
       }
    }
    cprintf("Virtual memory summary ...\n"
	    "%d page table present, %d pages total.\n",
	    total, count);
    cprintf("0x0 (0x00000000) - 0x80000000 (KERNBASE), #pdes = %d (out of %d possible)\n", bcount, (2147483648/(4096*1024)));//decimal value divided by space of a page on a 4096 boundary
    //PAGE 29 IN THE xv6 BOOK
    cprintf("0x80000000 (KERNBASE) - 0x80100000 (KERNBASE+EXTMEM), #pdes = %d (out of 0 possible)\n", kerncount);
    cprintf("0x80100000 (KERNBASE+EXTMEM) - 0x8e000000 (KERNBASE+PHYSTOP), #pdes = %d (out of %d possible)\n", memcount, (233832448/(4096*1024)));
    cprintf("0x8e000000 (KERNBASE+PHYSTOP) - 0xfe000000 (DEVSPACE), #pdes = %d (out of %d possible)\n", physcount, (1879048192/(4096*1024)));
    cprintf("0xfe000000 (DEVSPACE) - 0xffffffff (0xFFFFFFFF), #pdes = %d (out of %d possible)\n", devcount, (33554431/(4096*1024)));
    cprintf("\n\nBottom of kernel stack for process:\t%x\n\n", pde[1024]);//couldn't get the bottom of kernel stack. this was my best guess
  }

  return s;
}

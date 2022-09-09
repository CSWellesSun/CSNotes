#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 base;
  int len;
  uint64 mask; 
  uint64 buffer = 0;
  pte_t *pte;

  if (argaddr(0, &base) < 0) 
    return -1; 
  if (argint(1, &len) < 0)
    return -1;
  if (len > 64) {
    printf("Too many pages to be scanned!\n");
  }
  if (argaddr(2, &mask) < 0)
    return -1;

  for (int i = 0; i < len; i++) {
    if (base >= MAXVA)
      break;

    pagetable_t pagetable = myproc()->pagetable;
    int level;
    for(level = 2; level > 0; level--) {
      pte = &pagetable[PX(level, base)];
      if(*pte & PTE_V) {
        pagetable = (pagetable_t)PTE2PA(*pte);
      } else {
        pte = 0;
        break;
      }
    }
    if (level == 0)
      pte = &pagetable[PX(0, base)];
    if (pte == 0)
      continue;
    if ((*pte & PTE_V) == 0)
      continue;
    if ((*pte & PTE_A)) {
      buffer = buffer | (1L << i);
      *pte = *pte & (~PTE_A); // clear PTE_A
    }
    base += PGSIZE;
  }
  copyout(myproc()->pagetable, mask, (char*)&buffer, sizeof(uint64));
  
  return 0;  
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

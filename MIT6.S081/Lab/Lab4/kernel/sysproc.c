#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
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
  backtrace();
  return 0;
}

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

uint64
sys_sigalarm(void) {
  int n;
  uint64 fn;
  struct proc *p = myproc();

  if (argint(0, &n) < 0) {
    return -1;
  }
  if (argaddr(1, &fn) < 0) {
    return -1;
  }
  if (n == 0 && fn == 0) {
    p->ticks = -1;
    p->curticks = -1;
    p->handler = 0;
  } else {
    p->ticks = n;
    p->curticks = n;
    p->handler = fn;
  }
  return 0;
}

uint64
sys_sigreturn(void) {
  struct proc *p = myproc();
  p->trapframe->epc = p->his_epc; 
  p->trapframe->ra = p->his_ra; 
  p->trapframe->sp = p->his_sp; 
  p->trapframe->gp = p->his_gp; 
  p->trapframe->tp = p->his_tp; 
  p->trapframe->a0 = p->his_a0; 
  p->trapframe->a1 = p->his_a1; 
  p->trapframe->a2 = p->his_a2; 
  p->trapframe->a3 = p->his_a3; 
  p->trapframe->a4 = p->his_a4; 
  p->trapframe->a5 = p->his_a5; 
  p->trapframe->a6 = p->his_a6; 
  p->trapframe->a7 = p->his_a7; 
  p->trapframe->t0 = p->his_t0; 
  p->trapframe->t1 = p->his_t1; 
  p->trapframe->t2 = p->his_t2; 
  p->trapframe->t3 = p->his_t3; 
  p->trapframe->t4 = p->his_t4; 
  p->trapframe->t5 = p->his_t5; 
  p->trapframe->t6 = p->his_t6;
  p->trapframe->s0 = p->his_s0;
  p->trapframe->s1 = p->his_s1;
  p->trapframe->s2 = p->his_s2;
  p->trapframe->s3 = p->his_s3;
  p->trapframe->s4 = p->his_s4;
  p->trapframe->s5 = p->his_s5;
  p->trapframe->s6 = p->his_s6;
  p->trapframe->s7 = p->his_s7;
  p->trapframe->s8 = p->his_s8;
  p->trapframe->s9 = p->his_s9;
  p->trapframe->s10 = p->his_s10;
  p->trapframe->s11 = p->his_s11;

  p->iscalling = 0;
  return 0;
}
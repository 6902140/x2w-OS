#include "sched.h"
#include "mm.h"
#include "kernel/memory.h"
#include "irq.h"
#define DIV_CEILING(X, STEP) ((X + STEP - 1) / (STEP))
/**
 * @brief create_user_vaddr_bitmap用于给user_prog指向的用户进程创建虚拟内存位图
 * 
 * @param user_prog 需要创先虚拟内存位图的用户进程
 */
void create_user_vaddr_bitmap(struct task_struct *user_prog){
    raw_local_irq_disable();
   
    user_prog->userprog_vaddr.vaddr_start = K_HEAP_START;
    // 计算存储描述用户程序所占用的内存需要的bitmap占用的页数。用户程序中给出的地址虽然是虚拟地址, 但是每个虚拟地址都是和一个物理地址一一对应的
    // 因此, 也需要对虚拟地址进行管理, 依旧是使用位图来管理。
    // 由于位图要在物理内存中存储，因此这里先计算描述用户程序所占用内存的位图需要的位图占用页数
    uint32_t bitmap_pg_cnt = DIV_CEILING(TOTAL_MEMORY / PAGE_SIZE / 8, PAGE_SIZE);
    user_prog->userprog_vaddr.vaddr_bitmap=&(user_prog->virtual_bitmap);
    user_prog->userprog_vaddr.vaddr_bitmap->bits = (byte*)malloc_pages(bitmap_pg_cnt,1);
    
    kprintf("bitmap start from 0x%x\n",user_prog->userprog_vaddr.vaddr_bitmap->bits);
    
    bitmap_init(user_prog->userprog_vaddr.vaddr_bitmap,TOTAL_MEMORY / PAGE_SIZE / 8);
    
    //memset(user_prog->userprog_vaddr.vaddr_bitmap->bits,0,user_prog->userprog_vaddr.vaddr_bitmap->size);
    raw_local_irq_enable();
}


/*

// Reset tf->a0 to 0 so that fork returns 0 in the child.
// 这里将tf->a0设置为0，以便fork在子进程中返回0。
static void
usertrapret(void)
{
  struct proc *p = myproc();

  // 从当前进程的trapframe中恢复用户态寄存器状态
  memmove(p->tf, p->tf->trapframe, sizeof(struct trapframe));
  p->tf->epc += 4;  // 在epc中加上4，以绕过触发陷阱的指令

  // 切换页表为当前进程的页表
  lcr3(V2P(p->pagetable));

  // 切换到用户态
  p->state = RUNNING;
  release(&p->lock);

  // 跳转到用户态代码
  asm volatile("sret");
}


*/

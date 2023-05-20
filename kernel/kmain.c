/**
 * @file main.c
 * @author Shihong Wang (jack4shihong@gmail.com)
 * @brief `main.c`是内核的主函数
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright Shihong Wang (c) 2023 with GNU Public License V3.0
 */

#include "uart.h"
#include "types.h"
#include "test/test.h"
#include "kernel/kstdio.h"
#include "kernel/kmain.h"
#include "kernel/kinit.h"
#include "kernel/memory.h"
#include "sched.h"
#include "asm/irq.h"
#include "kernel/locks.h"
#include "irq.h"
extern struct task_struct* oncpu;
extern struct ready_queue_base g_rq;
extern int TASK_READY;



struct _test_lock_t{
	int public_variable;
	spinlock_t mylock;
};
struct _test_lock_t test_lock_t;

void test_lock_add(struct _test_lock_t* t){
	spinlock_acquire(&(t->mylock));
	t->public_variable+=1000;
	delay(10000);
	t->public_variable-=999;
	spinlock_release(&(t->mylock));
}

void delay(uint64_t k){
	while(k){
		k--;
	}
}



void kernel_stage2(void){
	kprintf("welcome to kernel stage .2\n");
	
	while(1){
		delay(5000);
		test_lock_add(&test_lock_t);
		kprintf("now public variable with lock protection is <%d>\n",test_lock_t.public_variable);
		kprintf("pid[%d] %d is on running 12345;\n",oncpu->pid,oncpu->counter);
	};
	
}

void kernel_stage3(void){
	kprintf("welcome to kernel stage 3\n");
	

	while(1){
		delay(5000);
		test_lock_add(&test_lock_t);
		kprintf("now public variable with lock protection is <%d>\n",test_lock_t.public_variable);
		kprintf("pid[%d] is on running abcde;\n",oncpu->pid);
	};
	
}

void kernel_stage4(void){
	kprintf("welcome to kernel stage 3\n");

	while(1){
		delay(5000);
		test_lock_add(&test_lock_t);
		kprintf("now public variable with lock protection is <%d>\n",test_lock_t.public_variable);
		kprintf("pid[%d] is on running !@#$^;\n",oncpu->pid);
	};
	
}


void kernel_main(void){
    kprintf(DELIMITER);
    uart_puts("In kernel!\n");
    kprintf("Kernel init!\n");
	oncpu=0;TASK_READY=1;
    kinit_all();

    kprintf("Start testing!\n");
    // test_all();


	print_kmem();
    kprintf("Kernel Hanging Here!\n");
	oncpu=g_task[0];
	oncpu->counter=20;
	oncpu->priority=5;
	oncpu->preempt_count=1;
	unsigned long val = read_csr(sstatus);
	write_csr(sstatus,val|SR_SPP);
	val = read_csr(sstatus);
	kprintf("kernel main sstatus:0x%x\n",val);
	//raw_local_irq_disable();
	int pid_main = do_fork(PF_KTHREAD, (unsigned long)&kernel_stage2, 0);
	int pid_main2 = do_fork(PF_KTHREAD, (unsigned long)&kernel_stage3, 0);
	int pid_main3 = do_fork(PF_KTHREAD, (unsigned long)&kernel_stage4, 0);
	
	move_to_user_space((unsigned long)&kernel_stage2,g_task[1]);
	
	move_to_user_space((unsigned long)&kernel_stage3,g_task[2]);
	
	move_to_user_space((unsigned long)&kernel_stage4,g_task[3]);
	kprintf("PID [1][2][3] move to user space success!!!\n");
	for(int i=0;i<4;i++){
		kprintf("pid[%d]'s pgdir vaddr=0x%x\n",i,g_task[i]->private_pgdir);
	}
	raw_local_irq_disable();
	oncpu->preempt_count=0;
	raw_local_irq_enable();
	//raw_local_irq_enable();
	// for(int i=0;i<3;i++)
	// 	create_user_vaddr_bitmap(g_task[i]);
	//move_to_user_space((unsigned long)&kernel_stage2,g_task[1]);
	kprintf("00ci0w-acvjd\n");
	test_lock_t.public_variable=0;
	spinlock_init(&(test_lock_t.mylock),"for test");
	//kprintf("pid:%d,%d,%d\n",pid_main,pid_main2,pid_main3);

	
	wake_up_process(oncpu);
	
    while (1){
		delay(10000);
		kprintf("pid[0],In kernel main thread now\n");
	};
}

// TODO: 需要完成printf函数更多的feature
void print_kmem(void){
    kprintf(DELIMITER);
    kprintf("X2W-OS Image Layout:\n");
    kprintf("  kernel: 0x%08lx - 0x%08lx (%6ld B)\n",
			(addr_t)_s_kernel, (addr_t)_e_kernel,
			(addr_t)(_e_kernel - _s_kernel));
    kprintf("  .text.boot: 0x%08lx - 0x%08lx (%6ld B)\n",
			(addr_t)_s_text_boot, (addr_t)_e_text_boot,
			(addr_t)(_e_text_boot - _s_text_boot));
	kprintf("       .text: 0x%08lx - 0x%08lx (%6ld B)\n",
			(addr_t)_s_text, (addr_t)_e_text,
			(addr_t)(_e_text - _s_text));
	kprintf("     .rodata: 0x%08lx - 0x%08lx (%6ld B)\n",
			(addr_t)_s_rodata, (addr_t)_e_rodata,
			(addr_t)(_e_rodata - _s_rodata));
	kprintf("       .data: 0x%08lx - 0x%08lx (%6ld B)\n",
			(addr_t)_s_data, (addr_t)_e_data,
			(addr_t)(_e_data - _s_data));
	kprintf("        .bss: 0x%08lx - 0x%08lx (%6ld B)\n",
			(addr_t)_s_bss, (addr_t)_e_bss,
			(addr_t)(_e_bss - _s_bss));
    kprintf(DELIMITER);
}
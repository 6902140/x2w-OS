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

extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[];
extern char _rodata[], _erodata[];
extern char _data[], _edata[];
extern char _bss[], _ebss[];



void kernel_main(void){
	mem_init((unsigned long)_ebss, DDR_END);
	paging_init();
	//初始化内存和分页制度
    kprintf(DELIMITER);
    uart_puts("In kernel!\n");
    kprintf("Kernel init!\n");
    kinit_all();

    kprintf("Start testing!\n");
    test_all();

	print_kmem();
    kprintf("Kernel Hanging Here!\n");
    while (1);
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
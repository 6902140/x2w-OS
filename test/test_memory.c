#include "test/test_memory.h"
#include "kernel/memory.h"
#include "pgable.h"
#include "mm.h"
// 声明外部符号, 避免include
extern size_t kprintf(const char* format, ...);

extern char idmap_pg_dir[];//定义一个外部指针指向内核进程的PGD基地址

extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[],_data[];




int test_memory(void){
    //test_record.num=0;
    kprintf("\n\n***********===>start test memory and paging->*****************\n");
    kprintf("\tthe base addr of PGD is %x\n",idmap_pg_dir);
    dump_pgtable();
    unsigned long vad[16];
    for(int i=0;i<16;i++){
        vad[i]=(unsigned long)malloc_a_page();
        kprintf("\ttest memory malloc page-> %d: virtual address=0x%x\n",i+1,vad);
    }
    for(int i=0;i<16;i++){
        kprintf("\t test free 0x%x page\n",vad[i]);
        free_a_page((void*)vad[i]);
    }
    kprintf("***************Congratulations! Paging successfully***********\n");
    return 0;
}
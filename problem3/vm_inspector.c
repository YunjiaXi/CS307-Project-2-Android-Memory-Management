/*
The program dumps the page table entries of a process in given range.
The application should run on AVD.
usage： command “./vm_inspector pid beginVA endVA”.  
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#define __NR_expose_syscall 379


int main(int argc, char *argv[])
{
    unsigned long begin_vaddr, end_vaddr, current_vaddr, fake_pgd, page_table_addr;
    unsigned long pgd_index, pte_index, offset, pfn;
    unsigned long *pte;
    //check and get arguments
    if(argc != 4)
    {
        printf("Wrong arguments!\nCorrect usage:./vm_inspector  pid  begin_addresss end_address \n");
        return -1;
    }
	pid_t pid = atoi(argv[1]);
	begin_vaddr = strtoul(argv[2], NULL, 0);
    end_vaddr = strtoul(argv[3], NULL, 0);

     // allocate user-space memory
    fake_pgd = (unsigned long)malloc(2 * PAGE_SIZE);
    if (!fake_pgd)
    {
        printf("fail to allocate space to fake_pgd\n");
    	return -1;
    }
    page_table_addr = (unsigned long)mmap(NULL, 1 << 22, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (!page_table_addr)
    {
        printf("fail to allocate space to page_table_addr\n");
    	return -1;
    }

    // call syscall "expose_page_table"
    if(syscall(__NR_expose_syscall, pid, fake_pgd, page_table_addr, begin_vaddr, end_vaddr))
    {
        printf("Expose page table error!");
        return -1;
    }

    current_vaddr = begin_vaddr;
    printf("\n-------virtual address translation-------\n");
    while(current_vaddr <= end_vaddr)
    {
        // segment the virtual address
        pgd_index = current_vaddr >> 21;
    	pte_index = (current_vaddr >> 12) & 0x1FF;
    	offset = current_vaddr & 0xFFF;
    	// get page frame number
    	pte = (unsigned long*)((unsigned long*)fake_pgd)[pgd_index];
    	pfn = pte[pte_index] & 0xFFFFF000;
    	//  only print pages that are present
		if (pfn)
             printf("  virtual address:0x%08lx  ====>  physical address: 0x%08lx\n\n", current_vaddr, pfn + offset);
    	current_vaddr += (1<<12);
    }

	// free allocated space
	free((void*)fake_pgd);
    munmap((void*)page_table_addr, 1 << 22);
	return 0;
}

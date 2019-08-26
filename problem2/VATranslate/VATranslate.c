/*
This program is to test the system calls:get_pagetable_layout and expose_page_table.
 The application should run on AVD.
You can get the physical address via command “./VATranslate pid #VA”.  
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#define __NR_layout 378
#define __NR_expose 379

#define PGD_SIZE ((1 << 11) * sizeof(unsigned long))
#define PTE_SIZE ((1 << 20) * sizeof(unsigned long))

struct pagetable_layout_info{
    uint32_t pgdir_shift;
    uint32_t page_shift;
};


void show_expose_pagetable(pid_t pid, unsigned long begin_vaddr, unsigned long end_vaddr,struct pagetable_layout_info pg_layout)
{
    unsigned long fake_pgd, page_table_addr, pgd_index, pte_index, offset, phy_addr,pfn;
    unsigned long *pte;

    // allocate user-space memory
    fake_pgd = (unsigned long)malloc(2 * PAGE_SIZE);
    if (!fake_pgd)
    {
        printf("fail to allocate space to fake_pgd\n");
    	return;
    }
    page_table_addr = (unsigned long)mmap(NULL, 1 << 22, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (!page_table_addr)
    {
        printf("fail to allocate space to page_table_addr\n");
    	return;
    }

     // invoke system call expose_page_table()
	if (syscall(__NR_expose, pid, fake_pgd, page_table_addr, begin_vaddr, end_vaddr))
    {
    	printf("Failed to call syscall expose pagetable layout()\n");
    	return;
    }

    // segment the virtual address
    pgd_index = (begin_vaddr >> pg_layout.pgdir_shift) & 0x7FF;
    pte_index = (begin_vaddr >> pg_layout.page_shift) & 0x1FF;
    offset = begin_vaddr & 0xFFF;

    // get physical address
    pte = (unsigned long*)((unsigned long*)fake_pgd)[pgd_index];
    if (pte)
    {
        pfn = pte[pte_index] & 0xFFFFF000;
        phy_addr = pfn + offset;
        // print result
        if (pfn)
        {
            printf("-----------Virtual Address Translation------------\n");
            printf("  pgd_index: 0x%03lx\tpte_index: 0x%03lx\toffset: 0x%03lx\n", pgd_index, pte_index, offset);
            printf("  pte_base = pgd_base[0x%03lx] = 0x%08lx\n  pfn = pte_base[0x%03lx] = 0x%08lx\n", pgd_index, (unsigned long)pte, pte_index, pfn);
            printf("  virtual address:0x%08lx ====> physical address: 0x%08lx\n\n", begin_vaddr, phy_addr);
        }
        else
            printf("The virtual memory is not in the memeory\n");
    }
    else
        printf("The virtual memory is not in the memeory\n");
    

    //free user-space memory
    free((void*)fake_pgd);
    munmap((void*)page_table_addr, 1 << 22);
    return;
}

int main(int argc, char **argv)
{
    pid_t pid;
	char *ptr;
	unsigned long begin_vaddr, end_vaddr;
    struct pagetable_layout_info pg_layout;

    //check and handle arguments
    if (argc != 3)
    {
        printf("Wrong format!\n The correct format should be: ./VATranslate #PID #VA\n");
        return -1;
    }
    pid = atoi(argv[1]);
	begin_vaddr = strtoul(argv[2], &ptr, 16);
	end_vaddr = begin_vaddr + 1;

    
    //invoke system call to get pagetable layout
    if (syscall(__NR_layout, &pg_layout, sizeof(struct pagetable_layout_info)))
    {
        printf("fail to call syscall 356\n");
        return -1;
    }

    //print layout we get
    printf("------------Android pagetable layout-----------\n");
    printf("  pgdir_shift = %u\n", pg_layout.pgdir_shift);
    printf("  page_shift  = %u\n\n", pg_layout.page_shift);

    
    show_expose_pagetable(pid, begin_vaddr, end_vaddr, pg_layout);

    return 0;
}

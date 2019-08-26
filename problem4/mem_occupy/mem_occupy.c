/*
 * This prpgram tries to occupy up to 1GB memory, 
 * which will always wake up the kawapd process
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/mman.h>


int main()
{
    unsigned long **p;
    unsigned long i, j;
	printf("Occupy 1GB memory--start\n");
    p = (unsigned long**)malloc(1 << 15);
    if (!p)
    {
    	printf("Memory allocation failed.\n");
        return -1;
    }
    {
        for (i = 0; i < (1 << 13); ++i)
        {
        	p[i] = (unsigned long*)malloc(1 << 17);
            if (!p[i])
            {
                printf("Memory allocation failed.\n");
                return -1;
            }
        	for (j = 0; j < 32; ++j)
        		p[i][j << 10] = j;
        }
        printf("Allocate done!\nReleasing after 1s.\n");
        sleep(1);
        for (i = 0; i < (1 << 13); ++i)
        	if(p[i])
        		free(p[i]);
    }
    if (p)
    	free(p);
	return 0;
}
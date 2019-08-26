/* 
 * This program investigates the file /proc/meminfo and prints out the 5th and 6th line in it.
 * The two specific lines contains the current size of active and inactive memory.
 * It prints every 1 second.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    char mesg[256];
    int i,j;
    for (i = 0; i <= 50; i++)
    {
        printf("After %d second(s):\n", i);
        freopen("/proc/meminfo", "r", stdin);
    	for(j=0; j<6;j++)
    	fgets(mesg, 256, stdin);
    	printf("\t%s", mesg);
    	fgets(mesg, 256, stdin);
    	printf("\t%s", mesg);
        sleep(1);
    }
    return 0;
}

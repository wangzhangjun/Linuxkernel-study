#include <stdio.h>
int main()
{
	int i = 10,j=5;
	printf("i = %d,j = %d\n",i,j);
//__asm__("asm statements" : outputs : inputs : registers-modified);
	__asm__ __volatile__("movl %1,%%eax;"
	"movl %%eax,%0;"
	:"=r"(j) // %0
	:"r"(i)  // %1
	:"%eax");
	
	printf("i = %d,j = %d\n",i,j);
}

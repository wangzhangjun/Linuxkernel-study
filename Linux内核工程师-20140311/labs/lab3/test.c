#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	char *p = NULL;
	p = malloc(10);
	printf("address = 0x%x\n",p);
	while(1);

	return 0;
}

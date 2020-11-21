#include<stdio.h>
#include <stdlib.h>
int  main()
{

	char *p=NULL;
	printf("Hello world\n");
	p = malloc(100);
	free(p);
	return 0;
}

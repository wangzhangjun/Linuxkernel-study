#include <stdlib.h>   
#include <stdio.h>   
#include <pthread.h>   
#include <unistd.h> 

#define barrier()  __asm__ __volatile__("": : :"memory")

volatile int ready;
int buf[100];
void test(int i) {
 	buf[i] = 50;
	barrier();
 	ready = 1;
}


int main()
{
	int m = 10;
	printf("before buf[%d] = %d\n",m,buf[m]);
	test(m);
	printf("after buf[%d] = %d\n",m,buf[m]);
	

}



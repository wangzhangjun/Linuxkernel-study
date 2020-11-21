#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(void) 
{ 
	pid_t pid; 
	int static count = 5;
	printf("process id = %d,old count = %d,address count = 0x%x\n",getpid(),count,&count);
        pid=fork(); 

        if(pid < 0) 
        	printf("error in fork!\n"); 
        else if (pid == 0) 
		{
        		printf("i am the child process, my process id is %d , pid is %d\n",getpid(),pid); 
			sleep(15);
			count++;
			count++;
			printf("child process id = %d ,count = %d,address count = 0x%x\n",getpid(),count,&count);
		}
  	else 
		{
        		printf("i am the parent process, my process id is %d,pid is %d\n",getpid(),pid); 
			sleep(5);	
			count++;
			printf("parent process id = %d,count = %d,address count = 0x%x\n",getpid(),count,&count);
 		}
	return 0;
} 


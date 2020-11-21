#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>

int main(void)
{

	int fd;
 	ssize_t len;
	fd = open("/root/workspace/labs/lab49/test.img",O_RDWR);

   	len = readahead(fd,20,40000000);
	printf("fd = %d,len = %d\n",fd,len);
	close(fd);
	



}

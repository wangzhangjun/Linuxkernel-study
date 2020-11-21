#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>

#define DEVICE_FILENAME "/dev/calldev"

int main ()
{
	int dev;
	char buff[128];
	int ret;
	printf("1)device file open\n");

	dev = open ( DEVICE_FILENAME,O_RDWR | O_NDELAY);
	if(dev>=0)
	{
		printf("2) seek function call\n");
		ret = lseek(dev,0x20,SEEK_SET);
		printf("ret = %08X\n",ret);
		printf("3)read function call\n");
 		ret = read(dev,0x30,0x31);
		printf("ret = %08X\n",ret);
		printf("4)write function call\n");
		ret = write(dev,0x40,0x41);
		printf("ret=%08X\n",ret);
		printf("5)ioctl function call\n");
		ret = ioctl(dev,0x51,0x52);
		printf("ret=%08X\n",ret);
		printf("6)device file close\n");
		ret=close(dev);
		printf("ret = %08X\n",ret);

		

	}
}

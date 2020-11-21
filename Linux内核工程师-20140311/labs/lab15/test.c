/*
 * Using ioctl() for Anything. (test program)
 *
 * As we commented, one reason for resistance to adding dynamic system
 * calls it that the ioctl() system call can already be used for
 * almost any purpose.
 *
 * One minor drawback is that you have to have a device node and write
 * a user application to access the node and pass ioctl() commands to
 * it.
 *
 * For those of you who know how to do a character driver, write a
 * very very brief character driver that has only an ioctl() entry
 * point.  (You don't even need open() or release()).
 *
 * Write a companion program that shows how you can use it.
 *
 * Note you'll have to create a device node, for instance by
 * doing mknod /dev/mycdrv c 700 0.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main (int argc, char *argv[])
{
    int fd, j;
    char *devname = "/dev/mycdrv";
    if (argc > 1)
        devname = argv[1];
    fd = open (devname, O_RDWR);
    for (j = 10; j < 20; j++)
	{
	sleep(1);
        
	printf ("ioctl(%d) = %d\n", j, ioctl (fd, j, NULL));
	}    
exit (0);
}

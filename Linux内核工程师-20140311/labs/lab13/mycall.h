#include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#define _MYCALL_  341

long mysyscall(int i)
{
	return syscall(_MYCALL_ , i);

}

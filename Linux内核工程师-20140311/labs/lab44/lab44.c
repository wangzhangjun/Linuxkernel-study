#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/types.h>
#include <linux/fcntl.h>

#define CALL_DEV_NAME  "calldev"
#define CALL_DEV_MAJOR 240

int call_open(struct inode *inode, struct file *filp)
{
	int num = MINOR (inode ->i_rdev);
	printk("call open->minor:%d\n",num);
	return 0;
}

loff_t call_llseek (struct file *filp, loff_t off, int whence)
{
	printk("call llseek ->off:%08X,whenec:%08X\n",off,whence);
	return 0x23;
}

ssize_t call_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	printk ("call read->buf:%08X,count:%08X\n",buf,count);
	return 0x33;
}

ssize_t call_write(struct file *filp, const char *buf, size_t count,loff_t *f_pos)
{
	printk("call write ->buf:%08X,count:%08X\n",buf,count);
	return 0x43;
}

int call_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,unsigned long arg)
{
	printk ("call ioctl ->cmd:%08X,arg:%08X\n",cmd,arg);
	return 0x53;
}

int call_release(struct inode *inode,struct file *filp)
{
	printk ("call release \n");
	return 0;
}

struct file_operations call_fops =
{
	.owner = THIS_MODULE,
	.llseek = call_llseek,
	.read = call_read,
	.write = call_write,
	.ioctl = call_ioctl,
	.open = call_open,
	.release = call_release,
};

static int call_init(void)
{
	int result;

	printk ("call call_init\n");

	result = register_chrdev (CALL_DEV_MAJOR,CALL_DEV_NAME,&call_fops);
	if(result < 0)return result;
	return 0;
}
static void call_exit(void)
{
	printk("call call_exit\n");
	unregister_chrdev(CALL_DEV_MAJOR,CALL_DEV_NAME);
}

module_init(call_init);
module_exit(call_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Character Devices kernel module");
MODULE_LICENSE("Dual BSD/GPL");

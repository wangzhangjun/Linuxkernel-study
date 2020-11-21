#include <linux/module.h>       /* for modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/init.h>         /* module_init, module_exit */
#include <linux/cdev.h>         /* cdev utilities */

#define MYDEV_NAME "mycdrv"

static dev_t first;
static unsigned int count = 1;
//#define MAJORDEVICE 700
static int my_major = 700, my_minor = 2;
static struct cdev *my_cdev;

static inline long
mycdrv_unlocked_ioctl (struct file *fp, unsigned int cmd, unsigned long arg)
{
    	printk (" I entered the ioctl with cmd=%d, do what you want!\n", cmd);
    	return 0;
}

static const struct file_operations mycdrv_fops = {
    	.owner = THIS_MODULE,
	.unlocked_ioctl = mycdrv_unlocked_ioctl,
};
static int __init my_init (void)
{
	int result = 0;
	first = MKDEV (my_major, my_minor);
	if(my_major)
    		result = register_chrdev_region (first, count, MYDEV_NAME);
	else
		result = result= alloc_chrdev_region(&first, 0, count, MYDEV_NAME);

	if(result < 0)
		printk("can'tget major devno:%d\n", MAJOR(first));
	printk("dev major is %d,minor = %d\n",MAJOR(first),MINOR(first));

    	my_cdev = cdev_alloc ();
    	cdev_init (my_cdev, &mycdrv_fops);
    	cdev_add (my_cdev, first, count);
    	printk ("\nSucceeded in registering character device %s\n", MYDEV_NAME);
    	return 0;
}
static void __exit my_exit (void)
{
    	cdev_del (my_cdev);
    	unregister_chrdev_region (first, count);
    	printk ("\ndevice unregistered\n");
}

module_init (my_init);
module_exit (my_exit);

MODULE_LICENSE ("GPL v2");
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("ioctrl kernel module");

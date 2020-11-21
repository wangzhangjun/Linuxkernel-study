#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#define MAJOR_NUM 200
#define DEVICE_NAME "hello"

// Store and Show functions.....
static ssize_t enable_store(struct class *cls, struct class_attribute *attr, const int *buf, size_t count);
static ssize_t enable_show(struct class *cls, struct class_attribute *attr, char *buf);

static struct file_operations fops;

static ssize_t enable_show(struct class *cls, struct class_attribute *attr, char *buf)
{
        char *x[] = { "Enabled", "Disabled" };    
        printk("In enable_show function\n");  
        return sprintf(buf, "%s\n", x[0]);
}

static ssize_t enable_store(struct class *cls, struct class_attribute *attr, const int *buf, size_t count)
{ 
       printk("In enable_store function\n");   
       
       return 1;
}
static struct class_attribute class_attr[] ={ __ATTR(var_switch, 0644, enable_show, enable_store), __ATTR_NULL };

static struct class hello_drv ={
	.name = "Dev_Hello", 
	.owner = THIS_MODULE,
	.class_attrs = (struct class_attribute *) &class_attr, 
};

// module init function.......
static int hello_init(void){
        int status;
        status = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops); 
        if (status < 0)
                 printk("Registering char driver failed with status = %d\n", status);        
	else   
	         printk("Hello World\n");    
        status = class_register(&hello_drv);       
        if (status < 0)        
        	printk("Registering Class Failed\n");      
	return 0;
}

// module exit function .......
static void hello_exit(void)
{        
	class_unregister(&hello_drv);
        unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
        printk(" GoodBye, world\n");
}

module_init( hello_init);
module_exit( hello_exit);
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION("sysfs test");


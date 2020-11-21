/*
#include  <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/page.h>
*/

#include <linux/capability.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/bitops.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/highmem.h>
#include <linux/firmware.h>
#include <linux/slab.h>


MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION("High-Memory Page Mapping");
static struct page *pg = NULL;

unsigned char *pagemem;
unsigned char *kmallocmem;
unsigned char *vmallocmem;

void *page_data	;
void *page_data2;
static int __init init(void)
{
	printk(KERN_INFO " init \n");
	pg = alloc_page(GFP_KERNEL | __GFP_HIGHMEM);
	//pg = alloc_page(GFP_KERNEL);
	if(NULL == pg)
		return -ENOMEM;
	page_data = kmap(pg);
	printk(KERN_INFO "permanent kernel mapping page_data = 0x%x\n",page_data);
	memcpy(page_data,"Hello world!",13);
	printk(KERN_INFO "content of page_data is %s\n",page_data);



//	page_data2 = kmap_atomic(pg,KM_USER0);
//	printk(KERN_INFO "temporary  kernel mapping page_data2 = 0x%x\n",page_data2);

/*
        if ((pagemem = (unsigned char*)get_free_page(0))==NULL)
 	{
  		printk(KERN_INFO "get_free_page Error!\n");
  		return -ENOMEM;
 	}
 	printk(KERN_INFO  "pagemem addr=0x%08X\n", pagemem);
*/


        if ((kmallocmem = (unsigned char*)kmalloc(100, 0))==NULL)
 	{
  		printk(KERN_INFO "kmalloc Error!\n");
  		return -ENOMEM;
 	}	
        printk(KERN_INFO "kmalloc mem addr=0x%08X\n", kmallocmem);
 
	if ((vmallocmem = (unsigned char*)vmalloc(1000000))==NULL)
 	{
  		printk(KERN_INFO "vmalloc Error!\n");
  		return -ENOMEM;
 	}
 	printk(KERN_INFO "vmallocmem addr=0x%08X\n", vmallocmem);



	return 0;
}



static void __exit finish(void)
{
	vfree(vmallocmem);
	kfree(kmallocmem);
	kunmap(page_data);
//	kunmap_atomic(page_data2,KM_USER0);
	__free_page(pg);
	printk(KERN_INFO " finish \n");
}
module_init(init);
module_exit(finish);

MODULE_LICENSE("GPL");

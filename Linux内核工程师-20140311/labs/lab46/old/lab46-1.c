#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
  
#define   VHDD_MAX_DEVICE           2
#define   VHDD_DEVICE_SIZE          (4*1024*1024)
  
char *vdisk[VHDD_MAX_DEVICE] = {NULL,};
 
static int vdisk_init(void)
{
        int result;
  
        vdisk[0] = vmalloc(VHDD_DEVICE_SIZE);
        vdisk[1] = vmalloc(VHDD_DEVICE_SIZE);
	memset(vdisk[0], 0, VHDD_DEVICE_SIZE);
        memset(vdisk[1], 0, VHDD_DEVICE_SIZE);
        return 0;
}
  
static void vdisk_exit(void)
{
        vfree(vdisk[0]);
        vfree(vdisk[1]);
}

EXPORT_SYMBOL(vdisk);
  
module_init(vdisk_init);
module_exit(vdisk_exit);
 
MODULE_LICENSE("Dual BSD/GPL");

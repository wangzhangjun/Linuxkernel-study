#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>          
#include <linux/errno.h>       
#include <linux/types.h>       
#include <linux/fcntl.h>       
#include <linux/vmalloc.h>
#include <linux/hdreg.h>  
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <asm/uaccess.h>
#include <linux/bio.h>
#define   VRD_DEV_NAME            "vrd"
#define   VRD_DEV_MAJOR            240
  
#define   VRD_MAX_DEVICE           1
  
#define   VRD_SECTOR_SIZE         512
#define   VRD_SIZE                (8*1024*1024)
#define   VRD_SECTOR_TOTAL        (VRD_SIZE)/(VRD_SECTOR_SIZE)

typedef struct 
{
	unsigned char        *data;
        struct request_queue *queue;
        struct gendisk       *gd;
} vrd_device;

static char                 *vdisk[VRD_MAX_DEVICE];
static vrd_device            device[VRD_MAX_DEVICE];
  
static int vrd_make_request(struct request_queue_t *q, struct bio *bio)
{
	printk("vrd_make_request\n");
        vrd_device     *pdevice;
        char           *pVHDDData;
        char           *pBuffer; 
        struct bio_vec *bvec;
        int             i;

        if(((bio->bi_sector*VRD_SECTOR_SIZE) + bio-> bi_size) > VRD_SIZE)
		goto fail;  
      
        pdevice = (vrd_device *) bio->bi_bdev->bd_disk-> private_data;
  
        pVHDDData = pdevice->data + (bio-> bi_sector*VRD_SECTOR_SIZE);

        bio_for_each_segment(bvec, bio, i)
        {
        	pBuffer = kmap(bvec->bv_page) + bvec-> bv_offset;
          	switch(bio_data_dir(bio)) 
          	{
          		case READA : 
			case READ  : 
				memcpy(pBuffer, pVHDDData, bvec-> bv_len); 
	                	break; 
          		case WRITE : memcpy(pVHDDData, pBuffer, bvec-> bv_len); 
                       		break;
          		default    : kunmap(bvec->bv_page);
                      		goto fail;
          	}
          	kunmap(bvec->bv_page);
          	pVHDDData += bvec->bv_len;
      	}
      
        bio_endio(bio,  0);
        return 0;
fail:
	bio_io_error(bio);
        return 0;
}

int vrd_open(struct inode *inode, struct file *filp)
{
	printk("vrd_open\n");
        return 0;
}

  
int vrd_release (struct inode *inode, struct file *filp)
{
	printk("vrd_release\n");
        return 0;
}
int vrd_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,unsigned long arg)
{
        return -ENOTTY;
}  

static struct block_device_operations vrd_fops =
{
        .owner   = THIS_MODULE,    
        .open    = vrd_open,
        .release = vrd_release,  
        .ioctl   = vrd_ioctl,
};

static int vrd_init(void)
{
        int lp;
      
        vdisk[0] = vmalloc(VRD_SIZE);
        vdisk[1] = vmalloc(VRD_SIZE);
  
        register_blkdev(VRD_DEV_MAJOR, VRD_DEV_NAME);
        for(lp = 0; lp < VRD_MAX_DEVICE; lp++)
        {
  
        	device[lp].data  = vdisk[lp];
                device[lp].gd    = alloc_disk(1);
                device[lp].queue = blk_alloc_queue(GFP_KERNEL);
                blk_queue_make_request(device[lp].queue, &vrd_make_request);
  
                device[lp].gd->major         = VRD_DEV_MAJOR;
                device[lp].gd->first_minor   = lp;
                device[lp].gd->fops          = &vrd_fops;
                device[lp].gd->queue = device[lp].queue;
                device[lp].gd->private_data  = &device[lp];
                sprintf(device[lp].gd->disk_name,  "vrd%c" , 'a'+lp);
                set_capacity(device[lp].gd,VRD_SECTOR_TOTAL);
          
                add_disk(device[lp].gd);
          
       }
       printk("vrd_init ok\n");
       return 0;
}


static void vrd_exit(void)
{
	int lp;
 
        for(lp = 0; lp < VRD_MAX_DEVICE; lp++)
        {
        	del_gendisk(device[lp].gd);
                put_disk(device[lp].gd);
        }
        unregister_blkdev(VRD_DEV_MAJOR, VRD_DEV_NAME);
  
        vfree(vdisk[0]);
        vfree(vdisk[1]);
	printk("vrd_exit ok\n");
}
 
module_init(vrd_init);
module_exit(vrd_exit);
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Virtual Random Block Devices kernel module");
MODULE_LICENSE("Dual BSD/GPL");

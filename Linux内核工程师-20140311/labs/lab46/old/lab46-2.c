#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>      
  
#include <linux/fs.h>          
#include <linux/errno.h>       
#include <linux/bio.h>
#include <linux/types.h>       
#include <linux/fcntl.h>       
#include <linux/vmalloc.h>
#include <linux/hdreg.h>  
#include <linux/blkdev.h>
#include <linux/blkpg.h>
//#include <linux/blk.h>
#include <asm/uaccess.h>
 
#define   VHDD_DEV_NAME            "vhdd"
#define   VHDD_DEV_MAJOR            240
#define   VHDD_MAX_DEVICE           2
#define   VHDD_MAX_PARTITONS        16
  
#define   VHDD_SECTOR_SIZE         512
#define   VHDD_SECTORS             16
#define   VHDD_HEADS               2
#define   VHDD_CYLINDERS           256
  
#define   VHDD_SECTOR_TOTAL (VHDD_SECTORS*VHDD_HEADS*VHDD_CYLINDERS)
#define   VHDD_SIZE           (VHDD_SECTOR_SIZE*VHDD_SECTOR_ TOTAL)
  
typedef struct 
{
     unsigned char        *data;
      struct request_queue *queue;
      spinlock_t            lock;
      struct gendisk       *gd;
}vhdd_device;


extern char                 *vdisk[VHDD_MAX_DEVICE];
  
static vhdd_device           device[VHDD_MAX_DEVICE];
static struct timer_list     vhdd_timer; 
static int                   vhdd_busy = 0;
  
static void vhdd_request(struct request_queue_t * q)
{
      vhdd_device     *pdevce;
          
      struct request  *vhdd_req;    
      char            *pData;
      int              size;
      
      if(vhdd_busy) return;
      
      while(1) 
      {
        vhdd_req = blk_fetch_request(q);
	if(!vhdd_req) return;
   
        pdevce = (vhdd_device *) vhdd_req->rq_disk-> private_data;

      
        pData = pdevce->data + (blk_rq_pos(vhdd_req))*VHDD_SECTOR_SIZE;
  
         if (blk_rq_pos(vhdd_req)+ blk_rq_cur_sectors(vhdd_req) >
get_capacity(vhdd_req->rq_disk))
         {
              blk_end_request_all(vhdd_req, 0);

  continue;
          }
          
        size = blk_rq_cur_sectors(vhdd_req) * VHDD_SECTOR_SIZE;
  
         switch(rq_data_dir(vhdd_req)) 
         {
         case READ  : memcpy(vhdd_req->buffer, pData, size); break; 
         case WRITE : memcpy(pData, vhdd_req->buffer, size); break;
         default    : blk_end_request_all(vhdd_req,0);  continue;
         }
  
          vhdd_timer.expires = jiffies + 2;
          vhdd_timer.data    = (unsigned long) pdevce;
         add_timer(&vhdd_timer);
         vhdd_busy = 1;

     return;
      }
  }

void vhdd_interrupt(unsigned long data)
  {
      vhdd_device     *pdevce;
      struct request  *vhdd_req;    
     
      pdevce   = (vhdd_device *) data;
      vhdd_req = blk_fetch_request(pdevce->queue);
  
      blk_end_request_all(vhdd_req, 1);
      vhdd_busy = 0;
      vhdd_request(pdevce->queue);
  }


int vhdd_open(struct inode *inode, struct file *filp)
  {
      return 0;
  }
  
  int vhdd_release (struct inode *inode, struct file * filp)
  {
      return 0;
  }

  int vhdd_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
  {
      int err;
      struct hd_geometry geo;
  
     switch(cmd) 
 {
  case HDIO_GETGEO:
       err = ! access_ok(VERIFY_WRITE, arg, sizeof(geo));
       if (err) return -EFAULT;
      geo.cylinders = VHDD_CYLINDERS;
      geo.heads     = VHDD_HEADS;
      geo.sectors   = VHDD_SECTORS;
      geo.start = get_start_sect(inode->i_bdev);
      if (copy_to_user((void *) arg, &geo, sizeof (geo)))
	 return -EFAULT;
      return 0;
 }
 return -ENOTTY;
}

static struct block_device_operations vhdd_fops =
  {
      .owner   = THIS_MODULE,
      .open    = vhdd_open,
      .release = vhdd_release,  
     .ioctl   = vhdd_ioctl,
 };
  
  int vhdd_init(void)
  {
      int lp;
  
      init_timer(&(vhdd_timer));
     vhdd_timer.function = vhdd_interrupt;
  
      register_blkdev(VHDD_DEV_MAJOR, VHDD_DEV_NAME);
      
      for(lp = 0; lp < VHDD_MAX_DEVICE; lp++)
      {
  
          device[lp].data = vdisk[lp];

device[lp].gd   = alloc_disk(VHDD_MAX_PARTITONS);
          spin_lock_init(&device[lp].lock);
         device[lp].queue = blk_init_queue(vhdd_request,&device
[lp].lock);
  
          device[lp].gd->major         = VHDD_DEV_MAJOR;
          device[lp].gd->first_minor   =lp*VHDD_MAX_PARTITONS;
          device[lp].gd->fops          = &vhdd_fops;
          device[lp].gd->queue         = device[lp]. queue;
          device[lp].gd->private_data  = &device[lp];
 sprintf(device[lp].gd->disk_name,  "vhdd%c" , 'a'+lp);
          set_capacity(device[lp].gd, VHDD_SECTOR_TOTAL);

 add_disk(device[lp].gd);
          
      }
      return 0;
  }
  
  void vhdd_exit(void)
  {
      int lp;
 
     for(lp = 0; lp < VHDD_MAX_DEVICE; lp++)
      {
del_gendisk(device[lp].gd);
           put_disk(device[lp].gd);
      
           blk_cleanup_queue(device[lp].queue);
      }
      unregister_blkdev(VHDD_DEV_MAJOR, VHDD_DEV_NAME);
  }
  
  module_init(vhdd_init);
  module_exit(vhdd_exit);
  
  MODULE_LICENSE("Dual BSD/GPL");


#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION("slab test module");

static struct kmem_cache  *test_cachep = NULL;
struct slab_test
{
        int val;
};
void fun_ctor(struct slab_test *object , struct kmem_cache  *cachep , unsigned long flags )
{
        printk(KERN_INFO "ctor fuction ...\n");
        object->val = 1;
}
void fun_dtor(struct slab_test *object , struct kmem_cache *cachep , unsigned long flags)
{
        printk(KERN_INFO " dtor fuction ...\n");
        object -> val = 0;
} 
static int __init slab_init(void)
{
	
        struct slab_test *object = NULL;
	printk(KERN_INFO "slab_init\n");
        //test_cachep = kmem_cache_create("test_cachep",sizeof(struct slab_test)*3,0,(SLAB_HWCACHE_ALIGN | SLAB_RECLAIM_ACCOUNT|SLAB_MEM_SPREAD),fun_ctor);
        test_cachep = kmem_cache_create("test_cachep",2,0,(SLAB_HWCACHE_ALIGN | SLAB_RECLAIM_ACCOUNT|SLAB_MEM_SPREAD),fun_ctor);
        if(NULL == test_cachep) 
                return  -ENOMEM ;
	//allloc an object form cache ,fun_ctor  will be call many times.
	printk(KERN_INFO "Cache name is %s\n",kmem_cache_name(test_cachep));
	printk(KERN_INFO "Cache object size  is %d\n",kmem_cache_size(test_cachep));
 	object = kmem_cache_alloc(test_cachep,GFP_KERNEL);
        if(object)
        {
                printk(KERN_INFO "alloc one val = %d\n",object->val);
                kmem_cache_free( test_cachep, object );
                printk(KERN_INFO "alloc three val = %d\n",object->val);
                object = NULL;
        }else
                return -ENOMEM;

        object = kmem_cache_alloc( test_cachep, GFP_KERNEL );
        if(object)
        {
                printk(KERN_INFO "alloc two val = %d\n",object->val);
                kmem_cache_free(test_cachep, object );
                object = NULL;
        }else
                return -ENOMEM;  
	return 0;
}

static void  __exit slab_clean(void)
{
	printk(KERN_INFO "slab_clean\n");
	if(test_cachep)
                kmem_cache_destroy(test_cachep);

}


module_init(slab_init);
module_exit(slab_clean);
MODULE_LICENSE("GPL");

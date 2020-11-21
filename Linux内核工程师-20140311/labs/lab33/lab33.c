#include <linux/rwsem.h> 
#include <linux/init.h> 
#include <linux/module.h> 

static int __init down_write_trylock_init(void); 
static void __exit down_write_trylock_exit(void);

#define EXEC_DOWN_READ 0
struct rw_semaphore rwsem ;
int __init down_write_trylock_init(void) 

{     

        int ret;
	init_rwsem( &rwsem );             
	printk("<0>after init_rwsem, count: %ld\n",rwsem.count); 

       	if( EXEC_DOWN_READ )
              down_read( &rwsem );          
	ret = down_write_trylock( &rwsem );
 	if( ret )
       	{     

              printk("<0>after down_write_trylock, count: 0x%0lx\n",rwsem.count);
              up_write( &rwsem );           
 	      printk("<0>after up_write, count: %ld\n",rwsem.count);
       }
       else
	{
              printk("<0>down_write_trylock failed!\n");
              printk("<0>after down_write_trylock, count: 0x%0lx\n",rwsem.count);
       	}
	return 0;
}

void __exit down_write_trylock_exit(void) 
{ 
       printk("<0>exit!\n");
}

module_init(down_write_trylock_init); 
module_exit(down_write_trylock_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Reader-Writer Semaphores test");
MODULE_LICENSE("GPL"); 

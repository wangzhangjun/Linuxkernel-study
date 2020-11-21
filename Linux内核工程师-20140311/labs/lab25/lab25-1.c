#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>


char my_tasklet_data[]="my_tasklet_function was called";

/* Bottom Half Function */
void my_tasklet_function( unsigned long data )
{
  printk( "%s\n", (char *)data );
}

DECLARE_TASKLET( my_tasklet, my_tasklet_function,(unsigned long) &my_tasklet_data );

int init_module( void )
{
  	/* Schedule the Bottom Half */
	printk("diable my_tasklet\n");
	   tasklet_disable(&my_tasklet);
//	printk("enable my_tasklet\n");
//	   tasklet_enable(&my_tasklet);
	  tasklet_schedule( &my_tasklet );

  return 0;
}

void cleanup_module( void )
{
  /* Stop the tasklet before we exit */
  tasklet_kill( &my_tasklet );

  return;
}

MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION ("tasklet of buttom half test2");
MODULE_LICENSE ("GPL v2");
 


#include <linux/module.h>
#include <linux/timer.h>
#include <asm/msr.h>            /* needed for Time Stamp Counter functions */
#include <linux/init.h>
#include <linux/jiffies.h>

static unsigned  long  speed;     /* MHZ of CPU */
//static   float  speed;     /* MHZ of CPU */

static struct timer_list timer_a, timer_b, my_timer;
static struct kt_data
{
    unsigned long period;
    unsigned long start_time;   /* jiffies value when we first started the timer */
    u64 tsc_start;              /* TSC when timer was queued */
    u64 tsc_end;                /* TSC when timer is executed */
} *data_a, *data_b;

void my_timer_callback( unsigned long data )
{
  printk( "my_timer_callback called (%ld).\n", jiffies );
}

static void ktfun_a (unsigned long var)
{
    u64 ticks;
    int msecs;
    struct kt_data *tdata = (struct kt_data *)var;

    rdtscll (tdata->tsc_end);
    ticks = tdata->tsc_end - tdata->tsc_start;
    msecs = (unsigned long)ticks / (speed * 1000);
    printk ("A: period = %ld  elapsed = %ld TSC ticks: %lld msecs = %d\n",
            tdata->period, jiffies - tdata->start_time, ticks, msecs);

    /* read the TSC for start time for the next cycle and resubmit */
    rdtscll (tdata->tsc_start);
    mod_timer (&timer_a, tdata->period + jiffies);
}
static void ktfun_b (unsigned long var)
{
    struct kt_data *tdata = (struct kt_data *)var;

    printk ("   B: period = %ld  elapsed = %ld \n", tdata->period,
            jiffies - tdata->start_time);

    /* resubmit */
    mod_timer (&timer_b, tdata->period + jiffies);
}
static int __init my_init (void)
{
    int ret;	
    long  temp;
    temp = 1.0;
    speed = (cpu_khz ) / 1000;
    temp = cpu_khz /1000;
    printk ("CPU MHZ is found to be: %d \n", temp);

    init_timer (&timer_a);
    init_timer (&timer_b);

    timer_a.function = ktfun_a;
    timer_b.function = ktfun_b;

    data_a = kmalloc (sizeof (*data_a), GFP_KERNEL);
    data_b = kmalloc (sizeof (*data_b), GFP_KERNEL);

    timer_a.data = (unsigned long)data_a;
    timer_b.data = (unsigned long)data_b;

    data_a->period = 1 * HZ;    /* short period, 1 second  */
    data_b->period = 10 * HZ;   /* longer period, 10 seconds */

    data_a->start_time = jiffies;
    data_b->start_time = jiffies;

    timer_a.expires = jiffies + data_a->period;
    timer_b.expires = jiffies + data_b->period;

    rdtscll (data_a->tsc_start);

    add_timer (&timer_a);
    add_timer (&timer_b);

    setup_timer( &my_timer, my_timer_callback, 0 );

    printk( "Starting timer to fire in 200ms (%ld)\n", jiffies );
    ret = mod_timer( &my_timer, jiffies + msecs_to_jiffies(200) );
    if (ret) printk("Error in mod_timer\n");
    return 0;
}
static void __exit my_exit (void)
{
    /* delete any running timers */
    printk ("Deleted timer A: rc = %d\n", del_timer_sync (&timer_a));
    printk ("Deleted timer B: rc = %d\n", del_timer_sync (&timer_b));
    kfree (data_a);
    kfree (data_b);
    printk ("Module successfully unloaded \n");
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Low-Resolution Timers Test");
MODULE_LICENSE ("GPL v2");

#include <linux/module.h>
#include <linux/timer.h>
#include <asm/msr.h>            /* needed for Time Stamp Counter functions */
#include <linux/init.h>
#include <linux/version.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21)
typedef int fun_type;
#else
typedef enum hrtimer_restart fun_type;
#define HRTIMER_REL HRTIMER_MODE_REL
#define HRTIMER_ABS HRTIMER_MODE_ABS    /* not actually needed */
#endif

static unsigned long speed;     /* MHZ of CPU */

static struct kt_data
{
    struct hrtimer timer;
    ktime_t period;
    unsigned long start_time; /* jiffies value when we first started the timer */
    u64 tsc_start;            /* TSC when timer was queued */
    u64 tsc_end;              /* TSC when timer is executed */
} *data_a, *data_b;

static void resubmit_it (struct hrtimer *var, struct kt_data *data)
{
    /* resubmit function must be different for modules or built-in */
    /* hrtimer_forward() un-exported!, not need for RHEL kernels */
    ktime_t now = var->base->get_time ();
    data->start_time = jiffies;
    hrtimer_forward (var, now, data->period);
}

static fun_type ktfun_a (struct hrtimer *var)
{
    u64 ticks;
    int msecs;
    rdtscll (data_a->tsc_end);
    ticks = data_a->tsc_end - data_a->tsc_start;
    msecs = (unsigned long)ticks / (speed * 1000);
    /* read the TSC for start time for the next cycle and resubmit */
    rdtscll (data_a->tsc_start);
    printk ("A: period = %lld  elapsed = %ld TSC ticks: %lld msecs = %d\n",
            ktime_to_ns (data_a->period), jiffies - data_a->start_time, ticks,
            msecs);
    resubmit_it (var, data_a);
    //return HRTIMER_NORESTART;
    return HRTIMER_RESTART;
}

static fun_type ktfun_b (struct hrtimer *var)
{
    printk ("   B: period = %lld  elapsed = %ld \n",
            ktime_to_ns (data_b->period), jiffies - data_b->start_time);
    /* resubmit */
    resubmit_it (var, data_b);
    //return HRTIMER_NORESTART;
    return HRTIMER_RESTART;
}

static int __init my_init (void)
{

    data_a = kmalloc (sizeof (*data_a), GFP_KERNEL);
    data_b = kmalloc (sizeof (*data_b), GFP_KERNEL);

    speed = cpu_khz / 1000;
    printk ("CPU MHZ is found to be: %ld \n", speed);

    data_a->period = ktime_set (1, 0);  /* short period, 1 second  */
    data_b->period = ktime_set (10, 0); /* longer period, 10 seconds */

    hrtimer_init (&data_a->timer, CLOCK_REALTIME, HRTIMER_REL);
    hrtimer_init (&data_b->timer, CLOCK_REALTIME, HRTIMER_REL);

    data_a->timer.function = ktfun_a;
    data_b->timer.function = ktfun_b;

    /* initial timing info */
    data_a->start_time = jiffies;
    data_b->start_time = jiffies;
    rdtscll (data_a->tsc_start);

    hrtimer_start (&data_a->timer, data_a->period, HRTIMER_REL);
    hrtimer_start (&data_b->timer, data_b->period, HRTIMER_REL);

    return 0;
}
static void __exit my_exit (void)
{
    /* delete any running timers */
    printk ("Deleted timer A: rc = %d\n", hrtimer_cancel (&data_a->timer));
    printk ("Deleted timer B: rc = %d\n", hrtimer_cancel (&data_b->timer));
    kfree (data_a);
    kfree (data_b);
    printk ("Module successfully unloaded \n");
}

module_init (my_init);
module_exit (my_exit);
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("High-Resolution Timers Test");
MODULE_LICENSE ("GPL v2");

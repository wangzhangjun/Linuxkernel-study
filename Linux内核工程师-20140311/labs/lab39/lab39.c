#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21)
typedef int fun_type;
#else
typedef enum hrtimer_restart fun_type;
#define HRTIMER_REL HRTIMER_MODE_REL
#define HRTIMER_ABS HRTIMER_MODE_ABS
#endif

static struct kt_data
{
    struct hrtimer timer;
    ktime_t period;
} *data;
ktime_t tmp ,tmp_new,tmp2;
static count ;
static fun_type ktfun (struct hrtimer *var)
{
    ktime_t now = var->base->get_time ();
    printk ("timer running at jiffies=%ld\n", jiffies);
    if(count == 10)
	{
		data->period = tmp2;
        	hrtimer_forward (var, now, data->period);
	}
    else
        hrtimer_forward (var, now, data->period);

    count++;
    return HRTIMER_NORESTART;
  //  return HRTIMER_RESTART;
}
static int __init my_init (void)
{
    data = kmalloc (sizeof (*data), GFP_KERNEL);
    //tmp = kmalloc(sizeof(*tmp),GFP_KERNEL);
    tmp = ktime_set(2,0);
    data->period = ktime_set (1, 0);    /* short period, 1 second  */
    hrtimer_init (&data->timer, CLOCK_REALTIME, HRTIMER_REL);
    data->timer.function = ktfun;

    tmp_new = ktime_add(data->period,tmp); 
    data->period = tmp_new;
    tmp2 = ktime_sub(data->period,tmp);	

    hrtimer_start (&data->timer, data->period, HRTIMER_REL);

    return 0;
}
static void __exit my_exit (void)
{
    hrtimer_cancel (&data->timer);
    kfree (data);
}
module_init (my_init);
module_exit (my_exit);
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("Generic Time Subsystem Test");
MODULE_LICENSE ("GPL v2");

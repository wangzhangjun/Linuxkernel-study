/*
 * Per Cpu Variables
 *
 * Write a trivial module that creates per-cpu variables, and modifies
 * and prints out their values.
 *
 * Try using both methods; a fixed allocation and a dynamical
 * allocation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/percpu.h>

static DEFINE_PER_CPU (long, cpuvar) = 10;
static long *cpualloc;

static int __init my_init (void)
{
    	int cpu;
    	printk ("cpuvar=%ld\n", get_cpu_var (cpuvar)++);
    	cpu = smp_processor_id ();
	printk("current cpu = %d\n",cpu);
    	put_cpu_var (cpuvar);
    	cpualloc = alloc_percpu (long);
    	*per_cpu_ptr (cpualloc, cpu) = 1000;
    	printk ("Hello: module loaded at 0x%p\n", my_init);
    	return 0;
}
static void __exit my_exit (void)
{
    	int cpu;
    	printk ("cpuvar=%ld\n", get_cpu_var (cpuvar));
    	cpu = smp_processor_id ();
	printk("current cpu = %d\n",cpu);
    	put_cpu_var (cpuvar);
    	printk ("per_cpu_ptr = %ld\n", *per_cpu_ptr (cpualloc, cpu));
    	free_percpu (cpualloc);
    	printk ("Bye: module unloaded from 0x%p\n", my_exit);
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("get binded cpu id by cpuvar");
MODULE_LICENSE ("GPL v2");

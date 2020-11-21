
/*
 * Examining Virtual Memory Areas
 *
 * Write a simple module which examines the address space for a
 * process.
 *
 * For each virtual memory area it finds, print out the address of the
 * vm_area_struct describing it, and the start, end, and length of the
 * address region.
 *
 * For simplicity you can use the current process (which will be
 * insmod during loading), or you can pass a process ID to the module.
 *
 * Examining the results can you identify some of the regions?  Look
 * at the maps entry in /proc for any process (such as /proc/self).
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>        /* not needed before 2.6.22 */
#include <linux/version.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26)
#define find_task_by_pid find_task_by_vpid
#endif

static int pid = 0;
module_param (pid, int, S_IRUGO);

static void printit (struct task_struct *tsk)
{
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    int j = 0;
    unsigned long start, end, length;

    mm = tsk->mm;
    printk ("mm = %p\n", mm);
    vma = mm->mmap;

    /* protect from simultaneous modification */

    down_read (&mm->mmap_sem);
    printk
        ("vmas:                vma        start          end          length\n");

    while (vma) {
        j++;
        start = vma->vm_start;
        end = vma->vm_end;
        length = end - start;
        printk ("%6d: %16p %12lx %12lx   %8lx=%8ld\n",
                j, vma, start, end, length, length);
        vma = vma->vm_next;
    }
    up_read (&mm->mmap_sem);
}
static int __init my_init (void)
{
    struct task_struct *tsk;
    if (pid == 0) {
        tsk = current;
        pid = current->pid;
    } else {
//        tsk = find_task_by_pid (pid);
		tsk = pid_task(find_vpid(pid),PIDTYPE_PID);
    }
    if (!tsk)
        return -1;
    printk (" Examining vma's for pid=%d, command=%s\n", pid, tsk->comm);
    printit (tsk);
    return 0;
}
static void __exit my_exit (void)
{
    printk ("Module Unloading\n");
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("vma test");
MODULE_LICENSE ("GPL v2");

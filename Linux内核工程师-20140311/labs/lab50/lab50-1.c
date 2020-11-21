/*
 * Launching User Mode Processes: (wall example)
 *
 * Write a kernel module that launches a user mode process.
 *
 * To do this you'll have to use the  call_usermodehelper() function.
 *
 * To begin with you can do something as simple as send a message
 * to everyone logged in with
 *
 *        wall message
 *
 * Once you get this working, try something fancier, such as copying a
 * file.
 *
 * Is it possible to redirect standard input/output while doing this?
*/

#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/init.h>

static char *str;
module_param (str, charp, S_IRUGO);

static int __init my_init (void)
{
    int rc, j = 0;
    char *argv[] = { "wall", "This is a message from the Kernel", NULL };
    static char *envp[] = { NULL };
    if (str)
        argv[1] = str;

    printk ("Trying to execute:");
    while (argv[j])
        printk (" %s ", argv[j++]);
    printk ("\n");

    rc = call_usermodehelper ("/usr/bin/wall", argv, envp, 0);
    if (rc) {
        printk ("Failed to execute %s %s\n", argv[0], argv[1]);
        return rc;
    }
    return 0;
}

static void __exit my_exit (void)
{
    printk ("Unloading Module: %s \n", __stringify (KBUILD_BASENAME));
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("call_usermodehelper fuction test");
MODULE_LICENSE ("GPL v2");

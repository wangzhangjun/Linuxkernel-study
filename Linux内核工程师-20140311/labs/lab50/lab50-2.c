/*
 * Launching User Mode Processes: (cp example)
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
#include <linux/init.h>

static int __init my_init (void)
{
    static int j = 0;
    char *argv[] = { "cp", "/var/log/dmesg", "/tmp/dmesg_copy", NULL };

    printk ("Trying to execute:");
    while (argv[j])
        printk (" %s ", argv[j++]);
    printk ("\n");

    if (call_usermodehelper ("/bin/cp", argv, NULL, 0))
        printk ("Failed to make run the program\n");
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

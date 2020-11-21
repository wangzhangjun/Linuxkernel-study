#include <linux/init.h>
#include <linux/utsname.h>
#include <linux/version.h>
#include <linux/module.h>

static int __init myingben_init(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
    struct new_utsname *utsname = &system_utsname;
#else
    struct new_utsname *utsname = &init_uts_ns.name;
#endif
    printk
        ("sysname    = %s\n"
         "nodename   = %s\n"
         "release    = %s\n"
         "version    = %s\n"
         "machine    = %s\n"
         "domainname = %s\n",
         utsname->sysname, utsname->nodename, utsname->release,
         utsname->version, utsname->machine, utsname->domainname);

    return 0;
}
static void __exit myingben_exit(void)
{
    printk ("Module Unloading\n");
}

module_init(myingben_init);
module_exit(myingben_exit);
MODULE_AUTHOR("support@ingben.com");
MODULE_DESCRIPTION("Get System information");
MODULE_LICENSE ("GPL v2");

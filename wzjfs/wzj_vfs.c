#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/parser.h>
#include <linux/smp_lock.h>
#include <linux/buffer_head.h>
#include <linux/exportfs.h>
#include <linux/vfs.h>
#include <linux/random.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/quotaops.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

//mount -t wzjfs /root/t1/ /root/t1/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wzj");
#define wzjfs_MAGIC 0x19980122
static DEFINE_RWLOCK(file_systems_lock);

static struct inode *wzjfs_make_inode(struct super_block *sb, int mode)
{
        struct inode *ret = new_inode(sb);
        if (ret) {
                ret->i_mode = mode;
                ret->i_uid = ret->i_gid = 0;
                ret->i_blocks = 0;
                ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
        }
        return ret;
}
static int wzjfs_open(struct inode *inode, struct file *filp)
{
        filp->private_data = inode->i_private;
        return 0;
}
#define TMPSIZE 20
static ssize_t wzjfs_read_file(struct file *filp, char *buf,
                size_t count, loff_t *offset)
{
        atomic_t *counter = (atomic_t *) filp->private_data;
        int v, len;
        char tmp[TMPSIZE];
        v = atomic_read(counter);
        if (*offset > 0)
                v -= 1;  
        else
                atomic_inc(counter);
        len = snprintf(tmp, TMPSIZE, "%d\n", v);
        if (*offset > len)
                return 0;
        if (count > len - *offset)
                count = len - *offset;
        if (copy_to_user(buf, tmp + *offset, count))
                return -EFAULT;
        *offset += count;
        return count;
}
static ssize_t wzjfs_write_file(struct file *filp, const char *buf,
                size_t count, loff_t *offset)
{
        atomic_t *counter = (atomic_t *) filp->private_data;
        char tmp[TMPSIZE];

        if (*offset != 0)
                return -EINVAL;

        if (count >= TMPSIZE)
                return -EINVAL;
        memset(tmp, 0, TMPSIZE);
        if (copy_from_user(tmp, buf, count))
                return -EFAULT;

        atomic_set(counter, simple_strtol(tmp, NULL, 10));
        return count;
}
static struct file_operations wzjfs_file_ops = {
        .open        = wzjfs_open,
        .read         = wzjfs_read_file,
        .write  = wzjfs_write_file,
};

static struct dentry *wzjfs_create_file (struct super_block *sb,
                struct dentry *dir, const char *name,
                atomic_t *counter)
{
        struct dentry *dentry;
        struct inode *inode;
        struct qstr qname;

        qname.name = name;
        qname.len = strlen (name);
        qname.hash = full_name_hash(name, qname.len);

        dentry = d_alloc(dir, &qname);
        if (! dentry)
                goto out;
        inode = wzjfs_make_inode(sb, S_IFREG | 0644);
        if (! inode)
                goto out_dput;
        inode->i_fop = &wzjfs_file_ops;
        inode->i_private = counter;
        d_add(dentry, inode);
        return dentry;
  out_dput:
        dput(dentry);
  out:
        return 0;
}
static struct dentry *wzjfs_create_dir (struct super_block *sb,
                struct dentry *parent, const char *name)
{
        struct dentry *dentry;
        struct inode *inode;
        struct qstr qname;
        qname.name = name;
        qname.len = strlen (name);
        qname.hash = full_name_hash(name, qname.len);
		//dentry的主要作用是建立文件名和inode之间的关联。
		/*所以该结构体包括两个最主要的字段，d_inode和d_name。
		其中，d_name为文件名。qstr是内核对字符串的封装（可以理解为带有散列值的char*）。
			d_inode是与该文件名对应的inode。*/
        dentry = d_alloc(parent, &qname);
        if (! dentry)
                goto out;
        inode = wzjfs_make_inode(sb, S_IFDIR | 0644);
        if (! inode)
                goto out_dput;
        inode->i_op = &simple_dir_inode_operations;
        inode->i_fop = &simple_dir_operations;
        d_add(dentry, inode);
        return dentry;
  out_dput:
        dput(dentry);
  out:
        return 0;
}
static atomic_t counter, subcounter;
static void wzjfs_create_files (struct super_block *sb, struct dentry *root)
{
        struct dentry *subdir;

        atomic_set(&counter, 0);
        wzjfs_create_file(sb, root, "counter", &counter);

        atomic_set(&subcounter, 0);
        subdir = wzjfs_create_dir(sb, root, "subdir");
        if (subdir)
                wzjfs_create_file(sb, subdir, "subcounter", &subcounter);
}
static struct super_operations wzjfs_s_ops = {
        .statfs                = simple_statfs,
        .drop_inode        = generic_delete_inode,
};
static int wzjfs_fill_super (struct super_block *sb, void *data, int silent)
{
        struct inode *root;
        struct dentry *root_dentry;
		
        sb->s_blocksize = PAGE_CACHE_SIZE;
        sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
        sb->s_magic = wzjfs_MAGIC;
        sb->s_op = &wzjfs_s_ops;

	printk(KERN_INFO "wzjfs_fill_super is here\n");
        root = wzjfs_make_inode (sb, S_IFDIR | 0755);
        if (! root)
                goto out;
        root->i_op = &simple_dir_inode_operations;
        root->i_fop = &simple_dir_operations;

        root_dentry = d_alloc_root(root);
        if (! root_dentry)
                goto out_iput;
        sb->s_root = root_dentry;

        wzjfs_create_files (sb, root_dentry);
        return 0;
        
  out_iput:
        iput(root);
  out:
        return -ENOMEM;
}
static int wzjfs_get_super(struct file_system_type *fst,int flags, const char *devname, void *data,struct vfsmount *mount)
{
	printk(KERN_INFO "mount from user\n");
    return get_sb_single(fst, flags, data, wzjfs_fill_super,mount);
}

static struct file_system_type wzjfs_type = {
        .owner                 = THIS_MODULE,
        .name                = "wzjfs",
        .get_sb                = wzjfs_get_super,
        .kill_sb        = kill_litter_super,
};

static int __init wzjfs_init(void)
{
	struct file_system_type * tmp;  
	printk("wzjfs_init ok\n");
    return register_filesystem(&wzjfs_type);
}
static void __exit wzjfs_exit(void)
{
    unregister_filesystem(&wzjfs_type);
	printk("wzjfs_exit ok\n");
}
module_init(wzjfs_init);
module_exit(wzjfs_exit);

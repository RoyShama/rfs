#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>


static struct dentry* rfs_mount(struct file_system_type *type, int flags,
                 char const *dev, void *data)
{
    struct dentry *const entry = mount_bdev(type, flags, dev,
                         data, &rfs_fill_sb);
    if (IS_ERR(entry))
        printk("rfs mounting failed\n");
    else
        printk("rfs mounted\n");
    return entry;
}


static struct file_system_type rfs_type = {
    .owner = THIS_MODULE,
    .name = FS_NAME,
    .mount = &rfs_mount,
    .kill_sb = kill_block_super,
    .fs_flags = FS_REQUIRES_DEV,
};


static int __init rfs_init(void)
{
    int ret = register_filesystem(&rfs_type);
    if (ret != 0)
    {
        printk("rfs cannot register filesystem\n");
        return ret;
    }
    printk("rfs module loaded\n");
    return 0;
}


static void __exit rfs_exit(void)
{
    int ret = unregister_filesystem(&rfs_type);
    if (ret != 0)
        printk("rfs cannot unregister filesystem\n");
    printk("rfs module unloaded\n");
}


module_init(rfs_init);
module_exit(rfs_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roy Shama");


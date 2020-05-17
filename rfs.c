#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define FS_NAME "rfs" // roy file system


static struct file_system_type rfs_type = {
	.owner = THIS_MODULE,
	.name = FS_NAME,
	.mount = rfs_mount,
	.kill_sb = kill_block_super
	.fs_flags = FS_REQUIRES_DEV,
};


static struct dentry* rfs_mount(struct file_system_type *type, int flags,
				 char const *dev, void *data)
{
	struct dentry *const entry = mount_bdev(type, flags, dev,
						 data, rfs_fill_sb);
	if (IS_ERR(entry))
		pr_err("rfs mounting failed\n");
	else
		pr_debug("rfs mounted\n");
	return entry;
}


static int rfs_fill_sb(struct super_block *sb, void *data, int silent)
{
	struct inode *root = NULL;

	sb->s_magic = RFS_MAGIC_NUMBER;
	sb->s_op = &rfs_super_ops;

	root = new_inode(sb);
	if(!root)
	{ 
		pr_err("inode allocation failed\n");
		return -ENOMEM;
	}

	root->i_ino = 0;
	root->i_sb = sb;
	root->i_atime = root->imtime = root->i_crime = CURRENT_TIME;
	inode_init_owner(root, NULL, S_IFDIR);

	sb->s_root= d_make_root(root);
	if (!sb->s_root)
	{
		pr_err("root creation failed\n");
		return -ENOMEM;
	}

	return 0;
}


static void aufs_put_super(struct super_block *sb)
{
	pr_debug("rfs super block destroyed\n");
}


static struct super_operations const rfs_super_ops = {
	.put_super = rfs_put_super,
};


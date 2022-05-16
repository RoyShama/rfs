#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include "rfs.h"


static void rfs_put_super(struct super_block *sb)
{
	printk("rfs super block destroyed\n");
}


static struct super_operations const rfs_super_ops = {
	.put_super = rfs_put_super,
};


static int rfs_fill_sb(struct super_block *sb, void *data, int silent)
{
	struct inode *root = NULL;
	child_arr* new_arr=(child_arr*)kmalloc(sizeof(child_arr),GFP_KERNEL);
	sb->s_magic = RFS_MAGIC_NUMBER;
	sb->s_op = &rfs_super_ops;

	root = new_inode(sb);
	if(!root)
	{ 
		printk("rfs inode allocation failed\n");
		return -ENOMEM;
	}

	root->i_ino = top_id++;
	root->i_sb = sb;
	root->i_atime = root->i_mtime = root->i_ctime = current_time(root);
	root->i_op = &rfs_inode_ops;
	inode_init_owner(root, NULL, S_IFDIR);
	root->i_private = new_arr;
	new_arr->len = 0;
	sb->s_root= d_make_root(root);
	if (!sb->s_root)
	{
		printk("rfs root creation failed\n");
		return -ENOMEM;
	}
	printk("rfs root\n");
	return 0;
}


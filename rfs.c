#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#define FS_NAME "rfs" // roy file system
static const unsigned long RFS_MAGIC_NUMBER = 0x13131313;
static int top_id = 0;

typedef struct child{
	const char* name;
	struct inode* child_inode;
}child;


typedef struct child_arr{
	int len;
	child* arr;
}child_arr;	


static void rfs_put_super(struct super_block *sb)
{
	printk("rfs super block destroyed\n");
}


static struct super_operations const rfs_super_ops = {
	.put_super = rfs_put_super,
};


static int rfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl)
{
	struct inode* new_file =NULL;
	child new_child;
	child_arr* new_arr = (child_arr*)kmalloc(sizeof(child_arr),GFP_KERNEL);
	child_arr* old_arr = (child_arr*)dir->i_private;
	child_arr* new_dir_arr;
	new_file = new_inode(dir->i_sb);
	if(!new_arr)
	{ 
		printk("rfs inode allocation failed\n");
		return -ENOMEM;
	}

	new_file->i_ino = top_id++;
	new_file->i_sb = dir->i_sb;
	new_file->i_atime = new_file->i_mtime = new_file->i_ctime = current_time(new_file);
	inode_init_owner(new_file, dir, mode);
	
	new_child.name = dentry->d_name.name;
	new_child.child_inode = new_file;
	new_arr->arr = (child*)kmalloc(sizeof(child)*++old_arr->len,GFP_KERNEL);
	new_arr->len = old_arr->len;
	memcpy(new_arr->arr,old_arr->arr,sizeof(child*)*(new_arr->len-1));
	new_arr->arr[new_arr->len-1] = new_child;
	dir->i_private = new_arr;
	kfree(old_arr);
	
	if(mode == S_IFDIR){
		new_dir_arr=(child_arr*)kmalloc(sizeof(child_arr),GFP_KERNEL);
		new_file->i_private = new_dir_arr;
		new_dir_arr->len = 0;
	}
	
	return 0;
}

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

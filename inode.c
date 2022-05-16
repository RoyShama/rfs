#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include "rfs.h"


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
	new_file->i_op = &rfs_inode_ops;
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


static int rfs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
    return rfs_create(dir, dentry, mode | S_IFDIR, 0);
}


static struct dentry* rfs_lookup(struct inode *dir, struct dentry *dentry, unsigned int flags)
{
	child_arr* dir_arr = (child_arr*)dir->i_private;
	int i;
	for(i=0; i<dir_arr->len; ++i){
		 if(strcmp(dir_arr->arr[i].name, dentry->d_name.name))
		 	goto file_found;
	}
	
	dentry = NULL;
	return ERR_PTR(-EIO);
	
	file_found:
		d_add(dentry, dir_arr->arr[i].child_inode);
		return NULL;
}


static const struct inode_operations rfs_inode_ops = {
    .lookup = rfs_lookup,
    .create = rfs_create,
    .mkdir = rfs_mkdir,
};

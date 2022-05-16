#ifndef RFS_H
#define RFS_H


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

static const struct inode_operations rfs_inode_ops;


#endif /* SIMPLEFS_H */

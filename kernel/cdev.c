#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/anon_inodes.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uio.h>

MODULE_AUTHOR("acshai");
MODULE_LICENSE("GPL");
MODULE_INFO(supported, "external");
MODULE_DESCRIPTION(" to experiment cdev fs");

int nr_nodes=0;
char filedata[PAGE_SIZE] = {'\0'};
int nr_bytes = 0;

dev_t d;
struct cdev c_dev;
struct class *class_c;
struct device *device_d;

struct data_set {
	int mem_used;
	char data[4096];
};

ssize_t cdev_write(struct file *fp, const char __user *ubuffer, size_t count, loff_t *foffset) 
{
	struct data_set *ds;

	ds = fp->private_data;
	printk("In %s\n ", __func__);
	
        if (copy_from_user(ds->data, ubuffer, count))
        {
		printk("In %s\n ", __func__);
                return -EFAULT;
        }

	ds->mem_used = count;

	return count;
} 


ssize_t cdev_read(struct file *fp, char __user *ubuffer, size_t count, loff_t *foffset)
{
	struct data_set *ds;
	int len;

	ds = fp->private_data;

	if (*foffset >= ds->mem_used )
        {
                printk("%s: invalid foffset %d mem_used %d \n", __func__,(int) *foffset, ds->mem_used);
                return 0;
        }

        len = ((int)*foffset + count > ds->mem_used) ? ds->mem_used : count;

	if(copy_to_user(ubuffer, ds->data + *foffset, count))
	{
		printk("%s: copy_to_user failed\n", __func__);
		return -EFAULT;
	}
	*foffset += count;

	printk("In %s \n", __func__);

	return count;
}

int cdev_release(struct inode *inode, struct file *fp)
{

	struct data_set *data;
        data = fp->private_data;

	if(data)
        {
                kfree(data);
                printk("freed data struct");
        }

	return 0;
}

void cdev_showfdinfo(struct seq_file *m, struct file *f)
{
	seq_printf(m,"test fd info fp->f_pos=%d \n", (int)f->f_pos);
}

/*loff_t (*llseek) (struct file *, loff_t, int);*/
loff_t cdev_seek(struct file *fp, loff_t foffset, int whence)
{	
	struct data_set *data;
        loff_t offset = 0;
        data = fp->private_data;
	
	switch(whence)
	{
		case SEEK_SET:
			offset = foffset;
			break; 
		case SEEK_CUR:
			offset = fp->f_pos + foffset; 
			break;
		case SEEK_END:
			offset = nr_bytes +(int) foffset;
			break;
	}
//	printk("In %s new_offset=%d mem_used=%d\n",__func__,(int)offset, data->mem_used);
	fp->f_pos = offset;
	
	return offset;
}

//int (*getattr) (struct vfsmount *mnt, struct dentry *, struct kstat *);
int cdev_getattr(struct vfsmount *mnt, struct dentry *d_entry, struct kstat *k_stat) {


	printk("In %s \n", __func__);
	generic_fillattr(d_entry->d_inode, k_stat);

	return 0;
}

int cdev_open(struct inode *i, struct file *fp)
{
	struct data_set *ds;
	
	printk("In %s\n", __func__);

	ds = kmalloc(sizeof(struct data_set), GFP_KERNEL);
        if(ds == NULL) {
                return -ENOMEM;
        }

	ds->mem_used =0;
	fp->private_data = ds;

	return 0;
}

//ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
static ssize_t cdev_read_iter(struct kiocb *k_cb, struct iov_iter *i_iter)
{
	struct data_set *ds;
	size_t count, ret;

	ds = k_cb->ki_filp->private_data;
	count = iov_iter_count(i_iter);
	ret = copy_to_iter(ds->data,count, i_iter);

	return ret;
}

//ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
ssize_t cdev_write_iter(struct kiocb *k_cb, struct iov_iter *i_iter)
{
	struct data_set *ds;
	size_t count, ret;

	ds = k_cb->ki_filp->private_data;
	count = iov_iter_count(i_iter);
	ret = copy_from_iter(ds->data,count, i_iter);
	
	ds->mem_used = count;
	return ret;
}

struct file_operations cdev_file_ops = {
	.owner = THIS_MODULE,
	.open = cdev_open,
	.read = cdev_read,
	.write = cdev_write,
	.release = cdev_release,
	.show_fdinfo = cdev_showfdinfo,
	.llseek = &cdev_seek,
	.write_iter = &cdev_write_iter,
	.read_iter = &cdev_read_iter,
};

struct inode_operations cdev_mdata = {
	.getattr=&cdev_getattr,
};


int c_dev_init(void)
{

	if ( 0 != alloc_chrdev_region(&d, 0, 6, "cdev_new")){
		printk("Failed to create cdev entry \n");
		return -1;
	}
	
	cdev_init(&c_dev, &cdev_file_ops);
	cdev_add(&c_dev, d, 1);
	class_c = class_create(THIS_MODULE, "temp_class");
	device_d = device_create(class_c, NULL, d, NULL, "temp_device");
	printk("%s\n", __func__);

	return 0;
}

void c_dev_exit(void)
{
	printk("%s\n", __func__);

	device_destroy(class_c, d);
	class_destroy(class_c);
	cdev_del(&c_dev);
	unregister_chrdev_region(d , 6);
}

module_init(c_dev_init);
module_exit(c_dev_exit);

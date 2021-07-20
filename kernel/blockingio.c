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
#include<linux/seq_file.h>
#include<linux/list.h>
#include<linux/delay.h>

MODULE_AUTHOR("acshai");
MODULE_LICENSE("GPL");
MODULE_INFO(supported, "external");
MODULE_DESCRIPTION("To experiment blocking_io");


struct data_set  {
	struct task_struct *tsk_struct;
	struct list_head list;
};

int nr_nodes=0;

static LIST_HEAD(linked_list);

char module_name[MODULE_NAME_LEN] = {'\0'};
char filedata[PAGE_SIZE] = {'\0'};
int nr_bytes = 0;

ssize_t blockingio_write(struct file *fp, const char __user *ubuffer, size_t count, loff_t *foffset) 
{

	 printk("In %s\n ", __func__);
        if (copy_from_user(filedata, ubuffer, count))
        {
		printk("In %s\n ", __func__);
                return -EFAULT;
        }

	nr_bytes = count;

	if(!list_empty(&linked_list))
	{
		struct data_set *tmp_data_set;
		struct list_head *p, *n;

		list_for_each_safe(p, n, &linked_list)
		{
			printk("In list for each safe \n");
			tmp_data_set = list_entry(p, struct data_set, list);
			list_del(&tmp_data_set->list);
			wake_up_process(tmp_data_set->tsk_struct);
			printk("Waked up process's\n");
		}
	}

	return count;
} 
extern struct file_operations blockingio_ops;

int get_module_name(char *buffer, const struct kernel_param *kp)
{
	int fd =0, len=0;
	struct file *file;

	printk("In %s \n",__func__);
	fd = anon_inode_getfd("temp", &blockingio_ops, NULL, O_RDWR);
	
	file = fget(fd);
	file->f_mode |= FMODE_LSEEK;

	fput(file);
	len = sprintf(buffer, "%d", fd);

	return len;	

}

ssize_t blockingio_read(struct file *fp, char __user *ubuffer, size_t count, loff_t *foffset)
{
	if(nr_bytes == 0) 
	{
		struct data_set newnode;
	//	msleep(10000);
		newnode.tsk_struct = current,
		list_add_tail(&newnode.list, &linked_list);
		nr_nodes += 1;

		set_current_state( TASK_KILLABLE );
		schedule();
	}
/*	if (*foffset >= data->mem_used )
	{
		printk("%s: invalid foffset %d mem_used %d \n", __func__,(int) *foffset, data->mem_used);
		return 0;
	} 
*/

//	len = ((int)*foffset + count > data->mem_used) ? data->mem_used : count;
	
	if(copy_to_user(ubuffer,filedata  + *foffset, count))
	{
		printk("%s: copy_to_user failed\n", __func__);
		return -EFAULT;
	}
	*foffset += count;

	printk("In %s \n", __func__);

	return count;
}

int blockingio_release(struct inode *inode, struct file *fp)
{

	return 0;
}

void blockingio_showfdinfo(struct seq_file *m, struct file *f)
{
	seq_printf(m,"test fd info fp->f_pos=%d \n", (int)f->f_pos);
}

/*loff_t (*llseek) (struct file *, loff_t, int);*/
loff_t blockingio_seek(struct file *fp, loff_t foffset, int whence)
{	

	loff_t offset = 0;

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

struct file_operations blockingio_ops = {
        .owner = THIS_MODULE,
        .read = blockingio_read,
        .write = blockingio_write,
        .release =blockingio_release,
	.show_fdinfo = blockingio_showfdinfo,
	.llseek = &blockingio_seek
};

struct kernel_param_ops module_name_ops = {
	.get=&get_module_name,
};


module_param_cb(module_param, &module_name_ops, module_name, 0600);

int __init blockingio_init(void)
{
	
	printk("%s\n", __func__);
	return 0;
}

void blockingio_exit(void)
{
	printk("%s\n", __func__);
}

module_init(blockingio_init);
module_exit(blockingio_exit);

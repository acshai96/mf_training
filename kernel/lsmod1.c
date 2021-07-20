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

MODULE_AUTHOR("acshai");
MODULE_LICENSE("GPL");
MODULE_INFO(supported, "external");
MODULE_DESCRIPTION("Param with module name");

struct dataset {
	long long mem_used;
	int nr_pages;
	char *module_list;
};

char module_name[MODULE_NAME_LEN] = {'\0'};

ssize_t lsmod_write(struct file *fp, const char __user *ubuffer, size_t count, loff_t *foffset) 
{
	struct module *tmp_module =NULL;
	struct module *mod = NULL;
        char tmp_buffer[200];
	char buffer1[count];
        int modrec_len=0;
	struct dataset *data;
	int mem_available=0;

	memset(buffer1,0,count);
	if (copy_from_user(buffer1, ubuffer, count))
    	{
		return -EFAULT;
		printk("in %s", __func__);
    	}

	printk("%s\n",buffer1);	
	mutex_lock(&module_mutex);
	mod = find_module(buffer1);
	
	if(mod)
	{
		memset(module_name, 0, MODULE_NAME_LEN);
		strncpy(module_name,(char *) buffer1, strlen(buffer1));
		data = fp->private_data;
		if(data == NULL){
			printk("In %s data is null\n", __func__);
			return -EFAULT;
		}
		data->nr_pages=data->mem_used=0;
		if(data->module_list)
		{
			kfree(data->module_list);
			data->module_list = NULL;
		}

		list_for_each_entry(tmp_module, &mod->list, list) 
		{
			if( tmp_module->name[0] == '\0'){
				continue;
			}
                        modrec_len = sprintf(tmp_buffer, " Module name: %s \n", tmp_module->name);
                        if(modrec_len > mem_available) {
                                char *realloc_buffer = (char *)krealloc(data->module_list, (data->nr_pages + 1) * PAGE_SIZE, GFP_KERNEL);
                                if (realloc_buffer == NULL)
                                {
                                       if(data->module_list){
						kfree(data->module_list);
					}
                                        mutex_unlock(&module_mutex);
                                        return -ENOMEM;
                                }
                                data->nr_pages += 1;
                                mem_available += PAGE_SIZE;
                                data->module_list = realloc_buffer;
                        }
                        memcpy(data->module_list + data->mem_used, tmp_buffer, modrec_len);
                        mem_available-= modrec_len;
                        data->mem_used += modrec_len;
                }

		mutex_unlock(&module_mutex);
		return count;
	}
	else
	{
		/* Entered module is not loaded / not found */
		printk("in else part");
		mutex_unlock(&module_mutex);
		return 0;
	}
} 
extern struct file_operations lsmod_ops;

int get_module_name(char *buffer, const struct kernel_param *kp)
{
	int fd =0, len=0;
	struct dataset *data;
	struct file *file;

	data=kmalloc(sizeof(struct dataset), GFP_KERNEL);
	if(data == NULL) {
		return -ENOMEM;
	}

	data->mem_used=data->nr_pages=0;
	data->module_list=NULL;

	printk("In %s \n",__func__);
	fd = anon_inode_getfd("temp", &lsmod_ops, data, O_RDWR);
	
	file = fget(fd);
	file->f_mode |= FMODE_LSEEK;

	fput(file);
	len = sprintf(buffer, "%d", fd);

	return len;	

}

ssize_t lsmod_read(struct file *fp, char __user *ubuffer, size_t count, loff_t *foffset)
{
	struct dataset *data;
	int len;
	
	data = fp->private_data;

	if (*foffset >= data->mem_used )
	{
		printk("%s: invalid foffset %d mem_used %d \n", __func__,(int) *foffset, data->mem_used);
		return 0;
	} 

	len = ((int)*foffset + count > data->mem_used) ? data->mem_used : count;
	
	if(copy_to_user(ubuffer, data->module_list + *foffset, len))
        {
            	printk("%s: copy_to_user failed\n", __func__);
            	return -EFAULT;
        }
        *foffset += len;

	printk("In %s \n", __func__);

	return len;
}

int lsmod_release(struct inode *inode, struct file *fp)
{
	struct dataset *data;
	data = fp->private_data;
	if(data)
	{
		if(data->module_list) 
		{
			kfree(data->module_list);
			printk("Freed module_list");
		}
		kfree(data);
		printk("freed data struct");
	}
	printk("In %s \n", __func__);

	return 0;
}

void lsmod_showfdinfo(struct seq_file *m, struct file *f)
{
	seq_printf(m,"test fd info fp->f_pos=%d \n", (int)f->f_pos);
}

/*loff_t (*llseek) (struct file *, loff_t, int);*/
loff_t lsmod_seek(struct file *fp, loff_t foffset, int whence)
{	
	struct dataset *data;

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
			offset = data->mem_used +(int) foffset;
			break;
	}
//	printk("In %s new_offset=%d mem_used=%d\n",__func__,(int)offset, data->mem_used);
	fp->f_pos = offset;
	return offset;
}
struct file_operations lsmod_ops = {
        .owner = THIS_MODULE,
        .read = lsmod_read,
        .write = lsmod_write,
        .release =lsmod_release,
	.show_fdinfo = lsmod_showfdinfo,
	.llseek = &lsmod_seek
};

struct kernel_param_ops module_name_ops = {
	.get=&get_module_name,
};


module_param_cb(module_param, &module_name_ops, module_name, 0600);

int module_list_init(void)
{
	printk("%s\n", __func__);
	return 0;
}

void module_list_exit(void)
{
	printk("%s\n", __func__);
}


module_init(module_list_init);
module_exit(module_list_exit);

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>

MODULE_AUTHOR("acshai");
MODULE_LICENSE("GPL");
MODULE_INFO(supported, "external");
MODULE_DESCRIPTION("Param with module name");

char module_name[MODULE_NAME_LEN] = {'\0'};
struct module *mod = NULL;
char *module_list = NULL;
int mod_list_pages=0;
int offset, mem_available, mem_used =0;

int set_module_name(const char *val, const struct kernel_param *kp)
{
	struct module *tmp_module;
        char tmp_buffer[200];
        int modrec_len=0;

	mutex_lock(&module_mutex);
	mod = find_module(val);
	if(mod){

		memset(module_name, 0, MODULE_NAME_LEN);
		strncpy(module_name,(char *) val, strlen(val));
		offset=mem_used=mod_list_pages=mem_available=0;
		
		if(module_list)
		{
			kfree(module_list);
			module_list = NULL;
		}

		list_for_each_entry(tmp_module, &mod->list, list) {
			if( tmp_module->name[0] == '\0'){
				continue;
			}
                        modrec_len = sprintf(tmp_buffer, " ==============================Module name: %s ============================\n", tmp_module->name);
                        if(modrec_len > mem_available) {
                                char *realloc_buffer = (char *)krealloc(module_list, (mod_list_pages + 1) * PAGE_SIZE, GFP_KERNEL);
                                if (realloc_buffer == NULL)
                                {
                                       if(module_list){
						kfree(module_list);
					}
                                        mutex_unlock(&module_mutex);
                                        return -ENOMEM;
                                }
                                mod_list_pages += 1;
                                mem_available += PAGE_SIZE;
                                module_list = realloc_buffer;
                        }
                        memcpy(module_list + mem_used, tmp_buffer, modrec_len);
                        mem_available-= modrec_len;
                        mem_used += modrec_len;
                }
		
		mutex_unlock(&module_mutex);
		return 0;

	}
	else
	{
		/* Entered module is not loaded / not found */
		mutex_unlock(&module_mutex);
		return -1;
	}
} 

int get_module_name(char *buffer, const struct kernel_param *kp)
{
	
	int start_offset, count= 0;

	dump_stack();

	start_offset = offset;

	if( (offset + 4094 ) > mem_used) {
		count = mem_used % 4094;
		offset = 0;
        }

        else {
		count = 4094;
                offset += 4094;
        }
	
	printk("In %s count=%d offset=%d start_offset=%d mem_used=%d \n", __func__, count, offset, start_offset, mem_used);
	memcpy(buffer, module_list + start_offset, count);

	return 4094;
}

struct kernel_param_ops module_name_ops = {
	.set=&set_module_name,
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
	if ( module_list != NULL ) {
		kfree(module_list);
	}
}


module_init(module_list_init);
module_exit(module_list_exit);

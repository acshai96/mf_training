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

int set_module_name(const char *val, const struct kernel_param *kp)
{
	struct module *mod;
	mod = find_module(val);
	if(mod){

		memset(module_name, 0, MODULE_NAME_LEN);
		strncpy(module_name,(char *) val, strlen(val));
		return 0;

	}
	else
	{
		/* Entered module is not loaded / not found */
		return -1;
	}
} 

int get_module_name(char *buffer, const struct kernel_param *kp)
{
	strncpy((char *)buffer, module_name, strlen(module_name));
	return strlen(module_name);
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
}


module_init(module_list_init);
module_exit(module_list_exit);

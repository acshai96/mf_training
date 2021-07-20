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
MODULE_DESCRIPTION("Dumps the module struct");

char module_name[MODULE_NAME_LEN] = {'\0'};

/* Returns length written or -errno.  Buffer is 4k (ie. be short!) */
//int (*get)(char *buffer, const struct kernel_param *kp);

int module_list_get(char *buffer, const struct kernel_param *kp)
{
	struct module *mod;

	if ( module_name[0] == '\0' || ( !strcasecmp(THIS_MODULE->name, module_name) ))
	{
		mod = THIS_MODULE;
		sprintf(buffer, "Module name:%s num_kp=%u\n", mod->name, mod->num_kp);
	}
	else if ( module_name[0] != '\0')
	{
		mutex_lock(&module_mutex);
		mod = find_module(module_name);
		if ( !mod )
		{
			sprintf(buffer, "Module \"%s\" not found\n", module_name);
		}
		else
		{
			sprintf(buffer, "Module name:%s num_kp=%u \n", mod->name, mod->num_kp);
			
		}
		mutex_unlock(&module_mutex);
	}
	
	module_name[0] = '\0';

	return strlen(buffer);
}

/* Returns 0, or -errno.  arg is in kp->arg. */
//int (*set)(const char *val, const struct kernel_param *kp);

int module_list_set(const char *val, const struct kernel_param *kp)
{
	strncpy(module_name, val, MODULE_NAME_LEN);

	return 0;
}

struct kernel_param_ops  module_info_ops =
{
    .set = &module_list_set,
    .get = &module_list_get,
};

module_param_cb(module_name, &module_info_ops, &module_name , 0600);
MODULE_PARM_DESC(module_name, "module info for a given module name");


char *myecho = NULL;

int myecho_get(char *buffer, const struct kernel_param *kp)
{
	if ( !myecho ) return 0;

	return strlen(strcpy(buffer, myecho));
}

int myecho_set(const char *val, const struct kernel_param *kp)
{
	if ( myecho )
	{
TryAgain:
		strcpy(myecho, val);
		return 0;
	}
	myecho = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if ( myecho )
	{
		printk("%s<%i> mycho=%p\n", __func__, __LINE__, myecho );
	 	goto TryAgain;
	}

	return -ENOMEM;
}

static struct kernel_param_ops myecho_ops = {.get = myecho_get, .set = myecho_set };

module_param_cb(echo, &myecho_ops, &myecho, 0600);
MODULE_PARM_DESC(echo, "ECHOS the users data");

int module_list_init(void)
{
	printk("%s\n", __func__);

	return 0;
}

void module_list_exit(void)
{
	printk("%s\n", __func__);
	if ( myecho ) kfree( myecho );
}


module_init(module_list_init);
module_exit(module_list_exit);

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/kernel.h>

MODULE_AUTHOR("acshai");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is a sample km to experiment cb_readonly ");

char moduleparam_string[100];

int cb_readonly_get(char *buffer, const struct kernel_param *kp) 
{	
	
	strncpy( buffer, moduleparam_string, strlen(moduleparam_string));

	return strlen(moduleparam_string);
}

int cb_readonly_init (void)
{
	printk("Sampleko : Loading ... \n");

	return 0;
}

void cb_readonly_exit (void)
{

	printk("Sampleko: Unloading... \n");	

}

struct kernel_param_ops  cb_readonly_ops =
{
    .get = &cb_readonly_get,
};

module_param_cb(samplecontent, &cb_readonly_ops,&moduleparam_string , 0600);
MODULE_PARM_DESC(samplecontent, "parameter with a callback");

module_init(cb_readonly_init);
module_exit(cb_readonly_exit);

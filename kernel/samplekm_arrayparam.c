#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/kernel.h>

MODULE_AUTHOR("acshai");
MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("Sample kernel module to demonstrate array paramaters"); 

//int  moduleparam_int[10];
int count;
//static char *moduleparam_string = "my param initial data";

//module_param_array(moduleparam_int, int , NULL , 0600);

int kernel_params_set(const char *val, const struct kernel_param *kp) {
	printk("In %s val=%s \n", __func__, val);
//	strncpy(moduleparam_string, (char *)val, 99);
	return 0;
//	return param_set_charp(val, kp);
}

int kernel_params_get(char *buffer, const struct kernel_param *kp) {	
	printk("In %s val=%d \n", __func__, count);
//	strncpy( buffer, moduleparam_string, strlen(moduleparam_string));
//	return param_get_charp(buffer, kp);
	return 0;
}

int hello_init (void)
{
	printk("Sampleko1 : Loading ... \n");
	return 0;
}

void hello_exit (void)
{
	printk("Sampleko1: Unloading... \n");	
}

struct kernel_param_ops  hello_proc_file_ops =
{
    .set = &kernel_params_set,
    .get = &kernel_params_get,
};

module_param_cb(samplecontent, &hello_proc_file_ops,&count , 0600);
MODULE_PARM_DESC(samplecontent, "cb with paramater 4byte int");

module_init(hello_init);
module_exit(hello_exit);

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/kernel.h>

MODULE_AUTHOR("acshai");
MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("To experiment pcpu variaables");

struct data {
	int len;
	char pcpu_data[100];
};

DEFINE_PER_CPU(struct data, pcpu_str);
char module_param[4096];

int pcpu_set(const char *val, const struct kernel_param *kp) {
	
//	printk("In %s val=%s moduleparam_str=%s \n", __func__, val, moduleparam_string);
	get_cpu_var(pcpu_str);
//	memcpy((void *)&pcpu_str, val, sizeof(struct data));
	put_cpu_var(pcpu_str);	
	return 0;
}

int pcpu_get(char *buffer, const struct kernel_param *kp) {	
//	printk("In %s val=%s \n", __func__, buffer);

	get_cpu_var(pcpu_str);
	memcpy( buffer, &pcpu_str, sizeof(struct data));
//	return param_get_charp(buffer, kp);
	put_cpu_var(pcpu_str);

	return sizeof(struct data);
}

int pcpu_init (void)
{
	printk(" %s: Loading ... \n", __func__);
	return 0;
}

void pcpu_exit (void)
{
	printk(" %s: Unloading... \n", __func__);	
}

struct kernel_param_ops  hello_proc_file_ops =
{
    .set = &pcpu_set,
    .get = &pcpu_get,
};

module_param_cb(samplecontent, &hello_proc_file_ops, module_param, 0600);
MODULE_PARM_DESC(samplecontent, "pcpu variable");

module_init(pcpu_init);
module_exit(pcpu_exit);

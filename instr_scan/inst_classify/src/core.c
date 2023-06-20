 #include <linux/irqflags.h>
 #include <linux/sched.h>
 #include <linux/module.h>
 #include <linux/version.h>
 #include <linux/kernel.h>
 #include <linux/init.h>
 #include <linux/slab.h>
 #include <linux/uaccess.h>
 #include <linux/kallsyms.h>


 int load_test(void)
 {
	// read system registers.

	// uint64_t target = 0;
	// printk("Insert kernel module\n");
	// asm volatile("mrs x0,SCTLR_EL1\n\t"
	// 	"mov %[result], x0 \n\t"
	// 	: [result]"=r"(target)
	// 	::
	// 	);
	// printk("SCTLR_EL1: %llx\n", target);
	// printk("test\n");

	asm volatile(
		"HVC 0\n\t"
		: 
		::	
	);

	printk("success\n");

	return 0;
 }	

 void unload_test(void)
 {
     printk(KERN_EMERG "Unloading test module...\n");
 }

 module_init(load_test);
 module_exit(unload_test);

 /*Kernel module Comments*/
 MODULE_AUTHOR("xmy");
 MODULE_DESCRIPTION("test");
 MODULE_LICENSE("GPL");



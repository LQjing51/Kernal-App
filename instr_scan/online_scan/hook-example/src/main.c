#include <linux/init.h>		/* to initiate a module */
#include <linux/module.h>	/* to recognise this module as a mmodule */

#include "common.h"
#include "hook.h"

int __init example_init(void) {
	/* entry point of the module */
	INFO("Hello cruel world\n");

    resolve_syms();
	install_hooks();
	return 0;
}

void __exit example_exit(void) {
	/* exit point of the module */
	remove_hooks();	

	INFO("GoodBye kernel\n");
}

module_init(example_init); 
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hook Example Module");
MODULE_AUTHOR("xjl");

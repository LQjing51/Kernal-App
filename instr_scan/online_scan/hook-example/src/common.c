#include "common.h"

#ifdef KPROBE_LOOKUP
typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
kallsyms_lookup_name_t this_kln = 0;
unsigned long kln(const char *name) {
 	if (!this_kln){	
		static struct kprobe kp = {
			.symbol_name = "kallsyms_lookup_name"
		};

		register_kprobe(&kp);
		this_kln = (kallsyms_lookup_name_t) kp.addr;//kallsyms_lookup_name() funcion pointer
		unregister_kprobe(&kp);
	}
	return this_kln(name);//return "name" function pointer
} 
#endif

//define a function pointer name orig_##_get_mm_exe_file
struct file *(*ORIG(get_mm_exe_file))(struct mm_struct *mm);

//point orig_##_get_mm_exe_file to kenel function "get_mm_exe_file"
void resolve_syms(void) {
#pragma GCC diagnostic ignored "-Wint-conversion"
    
    ORIG(get_mm_exe_file)          = kln("get_mm_exe_file");

#pragma GCC diagnostic warning  "-Wint-conversion"
}


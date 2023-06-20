#ifndef COMMON_H
#define COMMON_H

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#include <linux/kprobes.h>
#define KPROBE_LOOKUP 1
unsigned long kln(const char *name);
#else
#define kln kallsyms_lookup_name
#endif

#define INFO(format, ...)     do{ printk(KERN_INFO "info: "format, ##__VA_ARGS__); }while(0)
#define WARNNING(format, ...) do{ printk(KERN_ERR "warn: "format, ##__VA_ARGS__); }while(0)
#define ERR(format, ...)      do{ printk(KERN_ERR "error: "format, ##__VA_ARGS__); }while(0)
#define DEBUG(format, ...)    do{ printk(KERN_DEBUG "debug: "format, ##__VA_ARGS__); }while(0)
#define PRINTK(format, ...)   do{ printk(KERN_DEFAULT "print: "format, ##__VA_ARGS__); }while(0)

#define ORIG(sym) orig_##sym
#define STR_ORIG(sym) "orig_"sym

extern struct file *(*ORIG(get_mm_exe_file))(struct mm_struct *mm);

void resolve_syms(void);

#endif // !COMMON_H

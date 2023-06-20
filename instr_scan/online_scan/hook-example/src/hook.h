#ifndef __HOOK_H__
#define __HOOK_H__

extern int install_hooks(void);
extern void remove_hooks(void);

#define LOW32 ((1ul << 32) - 1)
#define LOW16 ((1 << 16) - 1)
#define LOW12 ((1 << 12) - 1)
#define LOW10 ((1 << 10) - 1)
#define LOW7  ((1 << 7) - 1)
#define LOW6  ((1 << 6) - 1)
#define LOW5  ((1 << 5) - 1)
#define LOW3  7
#define LOW2  3

#endif // !__HOOK_H__


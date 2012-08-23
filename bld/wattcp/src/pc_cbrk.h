#ifndef __PC_CBRK_H
#define __PC_CBRK_H

extern WORD         wathndlcbrk;
extern volatile int watcbroke;

extern int set_cbreak (int want_brk);
extern int tcp_cbreak (int mode);

#endif


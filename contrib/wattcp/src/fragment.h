#ifndef __FRAGMENT_H
#define __FRAGMENT_H

extern int _ip_frag_reasm;

extern in_Header *ip_defragment (const in_Header *ip, DWORD ofs, WORD flg);

extern int  free_fragment       (const in_Header *ip);
extern void chk_timeout_frags   (void);

#endif


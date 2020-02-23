#ifndef __UDP_NDS_H
#define __UDP_NDS_H

extern DWORD aton     (const char *text);
extern int   isaddr   (const char *text);
extern void  psocket  (const sock_type *sk);
extern int   priv_addr(DWORD ip);

extern DWORD aton_dotless   (const char *str);
extern int   isaddr_dotless (const char *text, DWORD *ip);

#endif

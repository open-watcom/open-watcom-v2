#ifndef __PCHOOKS_H
#define __PCHOOKS_H

extern int   (*_raw_ip_hook) (const in_Header*);
extern int   (*_tcp_syn_hook) (tcp_Socket**);
extern Socket *(*_tcp_find_hook) (const tcp_Socket*);

#endif

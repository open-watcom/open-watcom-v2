#ifndef __PCHOOKS_H
#define __PCHOOKS_H

extern int   (*_raw_ip_hook) (const in_Header*);
extern int   (*_tcp_syn_hook) (sock_type **skp);
extern Socket *(*_tcp_find_hook) (const sock_type *sk);

#endif

#ifndef __TCP_FSM_H
#define __TCP_FSM_H

extern int _tcp_fsm (sock_type **skp, const in_Header *ip);

extern int _tcp_reset (sock_type *sk, const in_Header *ip, tcp_Header *tcp_hdr,
                       const char *file, unsigned line);

#define TCP_RESET(sk, ip, tcp_hdr) \
       _tcp_reset(sk, ip, tcp_hdr, __FILE__, __LINE__)

#endif

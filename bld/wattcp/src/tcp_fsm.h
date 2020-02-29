#ifndef __TCP_FSM_H
#define __TCP_FSM_H

extern int _tcp_fsm (tcp_Socket **tcp_skp, const in_Header *ip);

extern int _tcp_reset (tcp_Socket *tcp_sk, const in_Header *ip, tcp_Header *tcp_hdr,
                       const char *file, unsigned line);

#define TCP_RESET(tcp_sk, ip, tcp_hdr) \
       _tcp_reset(tcp_sk, ip, tcp_hdr, __FILE__, __LINE__)

#endif

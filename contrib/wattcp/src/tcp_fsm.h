#ifndef __TCP_FSM_H
#define __TCP_FSM_H

extern int _tcp_fsm (tcp_Socket **s, const in_Header *ip);

extern int _tcp_reset (tcp_Socket *s, const in_Header *ip, tcp_Header *tcp,
                       const char *file, unsigned line);

#define TCP_RESET(s, ip, tcp) \
       _tcp_reset(s, ip, tcp, __FILE__, __LINE__)

#endif

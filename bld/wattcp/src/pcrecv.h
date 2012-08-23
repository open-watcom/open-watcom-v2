#ifndef __PCRECV_H
#define __PCRECV_H

#define RECV_USED    0xF7E3D2B1L
#define RECV_UNUSED  0xE6D2C1AFL

typedef struct recv_data {
        DWORD  recv_sig;
        BYTE  *recv_bufs;
        WORD   recv_bufnum;
      } recv_data;

typedef struct recv_buf {
        DWORD  buf_sig;
        DWORD  buf_hisip;
        WORD   buf_hisport;
        short  buf_len;
        BYTE   buf_data [ETH_MAX_DATA];
      } recv_buf;

extern int sock_recv_init (udp_Socket *s, void *space, int len);
extern int sock_recv_used (udp_Socket *s);
extern int sock_recv      (udp_Socket *s, char  *buffer, int len);
extern int sock_recv_from (udp_Socket *s, DWORD *hisip, WORD *hisport,
                           char *buffer, int len, int peek);

#endif

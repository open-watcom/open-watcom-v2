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

extern int sock_recv_init (sock_type *sk, char *buffer, int len);
extern int sock_recv_used (sock_type *sk);
extern int sock_recv      (sock_type *sk, char *buffer, int len);
extern int sock_recv_from (sock_type *sk, DWORD *hisip, WORD *hisport,
                                char *buffer, int len, int peek);

#endif

#ifndef __PCBUF_H
#define __PCBUF_H

#define VALID_UDP  1
#define VALID_TCP  2
#define VALID_IP   3

extern int sock_rbsize  (const sock_type *s);
extern int sock_rbused  (const sock_type *s);
extern int sock_rbleft  (const sock_type *s);
extern int sock_tbsize  (const sock_type *s);
extern int sock_tbused  (const sock_type *s);
extern int sock_tbleft  (const sock_type *s);

extern int sock_setbuf  (sock_type *s, BYTE *buf, unsigned len);
extern int sock_preread (const sock_type *s, BYTE *buf, unsigned len);

extern const char  *sockerr   (const tcp_Socket *s);
extern const char  *sockstate (const tcp_Socket *s);
extern int         _chk_socket(const sock_type *s);

#endif


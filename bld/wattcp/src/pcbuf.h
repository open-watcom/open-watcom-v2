#ifndef __PCBUF_H
#define __PCBUF_H

#define VALID_UDP  1
#define VALID_TCP  2
#define VALID_IP   3

extern int          sock_rbsize  (const sock_type *sk);
extern int          sock_rbused  (const sock_type *sk);
extern int          sock_rbleft  (const sock_type *sk);
extern int          sock_tbsize  (const sock_type *sk);
extern int          sock_tbused  (const sock_type *sk);
extern int          sock_tbleft  (const sock_type *sk);

extern const char  *sockerr   (const sock_type *sk);
extern const char  *sockstate (const sock_type *sk);
extern int         _chk_socket(const sock_type *sk);

#endif


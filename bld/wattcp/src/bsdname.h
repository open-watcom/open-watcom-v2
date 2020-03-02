#ifndef __BSDNAME_H
#define __BSDNAME_H

extern int _getpeername   (const sock_type *sk, void *dest, socklen_t *len);
extern int _getsockname   (const sock_type *sk, void *dest, socklen_t *len);

#endif

#ifndef __BSDNAME_H
#define __BSDNAME_H

extern int _getpeername   (const sock_type *sk, void *dest, int *len);
extern int _getsockname   (const sock_type *sk, void *dest, int *len);

extern int  sethostname   (const char *name, int len);
extern int  setdomainname (const char *name, int len);

extern int  getdomainname (char *name, int len);
extern int  gethostname   (char *name, int len);

extern int _get_machine_name (char *buf, int size);

#endif

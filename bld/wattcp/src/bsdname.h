#ifndef __BSDNAME_H
#define __BSDNAME_H

extern int _getpeername   (const tcp_Socket *s, void *dest, int *len);
extern int _getsockname   (const tcp_Socket *s, void *dest, int *len);

extern int  sethostname   (const char *name, int len);
extern int  setdomainname (const char *name, int len);

extern int  getdomainname (char *name, int len);
extern int  gethostname   (char *name, int len);

extern int _get_machine_name (char *buf, int size);

#endif

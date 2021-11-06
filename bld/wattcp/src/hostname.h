#ifndef __HOSTNAME_H
#define __HOSTNAME_H

extern int  sethostname   (const char *name, size_t len);
extern int  setdomainname (const char *name, size_t len);

extern int  getdomainname (char *name, size_t len);
extern int  gethostname   (char *name, size_t len);

extern int _get_machine_name (char *buf, size_t size);

#endif

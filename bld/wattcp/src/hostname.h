#ifndef __HOSTNAME_H
#define __HOSTNAME_H

extern int  sethostname   (const char *name, int len);
extern int  setdomainname (const char *name, int len);

extern int  getdomainname (char *name, int len);
extern int  gethostname   (char *name, int len);

extern int _get_machine_name (char *buf, int size);

#endif

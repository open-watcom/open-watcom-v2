#if !defined(__LOOPBACK_H) && defined(USE_LOOPBACK)
#define __LOOPBACK_H

#ifndef IPPORT_ECHO
#define IPPORT_ECHO     7
#endif

#ifndef IPPORT_DISCARD
#define IPPORT_DISCARD  9
#endif

extern int   loopback_enable;
extern int (*loopback_handler)(in_Header *);
extern int   loopback_device  (in_Header *);

#endif

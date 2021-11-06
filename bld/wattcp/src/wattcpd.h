#ifndef __WATTCPD_H
#define __WATTCPD_H

extern void (*wattcpd) (void);

extern int addwattcpd (void (*p)(void));
extern int delwattcpd (void (*p)(void));

#endif

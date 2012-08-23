#ifndef __SYSLOG2_H
#define __SYSLOG2_H

extern char *syslog_fileName;
extern char *syslog_hostName;
extern WORD  syslog_port;
extern int   syslog_mask;

extern void syslog_init (void);

#endif

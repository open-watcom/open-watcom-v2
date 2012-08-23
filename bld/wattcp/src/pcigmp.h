#ifndef __PCIGMP_H
#define __PCIGMP_H

extern void igmp_handler (const in_Header *ip, BOOL brdcast);
extern int  igmp_report  (DWORD ip);

#endif

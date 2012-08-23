#ifndef __PCMULTI_H
#define __PCMULTI_H

extern int _multicast_on;

#if defined(USE_MULTICAST)

  extern  multicast _ipmulti [IPMULTI_SIZE];

  extern int  join_mcast_group    (DWORD);
  extern int  leave_mcast_group   (DWORD);
  extern void multi_to_eth        (DWORD, BYTE *);
  extern void check_mcast_reports (void);

#endif
#endif


#ifndef __SOCK_INI_H
#define __SOCK_INI_H

#define WERR_ILL_DOSX    1  /* Watcom/HighC:  illegal dos-extender */
#define WERR_NO_DOSMEM   2  /* Watcom+DOS4GW: No memory for DOS buffers */
#define WERR_NO_DRIVER   3  /* All: No network driver found (pktdrvr) */
#define WERR_BOOTP_FAIL  4  /* All: BOOTP protocol failed */
#define WERR_DHCP_FAIL   5  /* All: DHCP protocol failed */
#define WERR_RARP_FAIL   6  /* All: RARP protocol failed */
#define WERR_NO_IPADDR   7  /* All: Failed to find an IP-address */
#define WERR_IP_RRESOLVE 8  /* All: Failed to reverse resolve IP-address */
#define WERR_PPPOE_DISC  9  /* All: PPPoE discovery failed (timeout) */

extern int   _bootpon;      /* boot-up through BOOTP and/or DHCP */
extern int   _dhcpon;
extern int   _rarpon;
extern int   _domask_req;
extern int   _watt_do_exit;
extern int   _watt_is_init;
extern int   _watt_no_config;
extern int   _watt_fatal_error;
extern DWORD _watt_start_time;
extern DWORD _watt_start_day;

#define survivebootp  NAMESPACE(survivebootp)
#define survivedhcp   NAMESPACE(survivedhcp)
#define surviverarp   NAMESPACE(surviverarp)

extern int survivebootp, survivedhcp, surviverarp;

extern void (*_watt_post_hook) (void);

extern int  sock_init (void);
extern void sock_exit (void);

#endif

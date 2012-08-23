/*
 *
 *   BOOTP - Boot Protocol (RFC 854/951/1048)
 *
 *   These extensions get called if _bootphost is set to an IP address or
 *   to 0xffffffff.
 *
 *   Version
 *
 *   0.3 : Feb  1, 1992 : J. Dent - patched up various things
 *   0.2 : May 22, 1991 : E.J. Sutcliffe - added RFC_1048 vendor fields
 *   0.1 : May  9, 1991 : E. Engelke - made part of the library
 *   0.0 : May  3, 1991 : E. Engelke - original program as an application
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "udp_dom.h"
#include "syslog2.h"
#include "misc.h"
#include "pctcp.h"
#include "pcsed.h"
#include "pcarp.h"
#include "pcbsd.h"
#include "pcconfig.h"
#include "pcqueue.h"
#include "pcpkt.h"
#include "pcdhcp.h"
#include "pcbootp.h"

DWORD _bootphost = 0xFFFFFFFFL;

#if defined(USE_DHCP)
  int _bootptimeout = 15;  /* give DHCP a chance also (sock_delay=30) */
#else
  int _bootptimeout = 30;
#endif

#if defined(USE_BOOTP)

/*
 * _dobootpc - Checks global variables _bootptimeout, _bootphost.
 *             If no host specified, use the broadcast address.
 *             Returns 1 on success and sets ip address
 */
int _dobootp (void)
{
  struct     bootp bootp_out;   /* outgoing data */
  struct     bootp bootp_in;    /* incoming data */
  DWORD      bootptimeout;
  WORD       magictimeout;
  DWORD      xid;
  BYTE      *p;
  int        status;
  udp_Socket sock;
  sock_type *bsock = (sock_type*)&sock;

  outs (_LANG("Configuring through BOOTP.."));

  /* We must get Waterloo TCP to use IP address 0 for sending
   */
  xid = my_ip_addr;
  my_ip_addr = 0UL;

  if (!udp_open(&sock,IPPORT_BOOTPC,_bootphost,IPPORT_BOOTPS,NULL))
  {
    outsnl (_LANG("\r\nUnable to resolve bootp server."));
    return (0);
  }

  bootptimeout = set_timeout (1000 * _bootptimeout);
  magictimeout = ((WORD)xid & 7) + 7;  /* between 7 and 14 seconds */

  memset (&bootp_out, 0, sizeof(bootp_out));

  bootp_out.bp_op = BOOTREQUEST;
  *(DWORD*) &bootp_out.bp_vend = intel (VM_RFC1048);  /* Magic Number */

  _eth_get_hwtype (&bootp_out.bp_htype, &bootp_out.bp_hlen);

  bootp_out.bp_xid  = xid;
  bootp_out.bp_secs = intel16 (1);

  memcpy (&bootp_out.bp_chaddr, &_eth_addr, sizeof(eth_address));

  while (1)
  {
    DWORD sendtimeout;

    sock_fastwrite (bsock, (BYTE*)&bootp_out, sizeof(bootp_out));
    bootp_out.bp_secs = intel16 (intel16(bootp_out.bp_secs) + magictimeout);
    magictimeout += (WORD) (xid >> 5) & 7;
    sendtimeout   = set_timeout (1000 * magictimeout);

    while (!chk_timeout(sendtimeout))
    {
      WORD len;

      if (chk_timeout(bootptimeout))
         goto give_up;

      kbhit();
      sock_tick (bsock, &status);

      if (!sock_dataready(bsock))
         continue;

      /* got a response, lets consider it
       */
      len = sock_fastread (bsock, (BYTE*)&bootp_in, sizeof(bootp_in));
      if (len < sizeof(bootp_in))
      {
        /* too small, not a BOOTP packet
         */
        memset (&bootp_in, 0, sizeof(bootp_in));
        continue;
      }

      /* we must see if this is for us
       */
      if (bootp_in.bp_xid != bootp_out.bp_xid)
      {
        memset (&bootp_in, 0, sizeof(bootp_in));
        continue;
      }

      /* we must have found it
       */
      my_ip_addr = intel (bootp_in.bp_yiaddr);

      if (intel(*(DWORD*)&bootp_in.bp_vend) == VM_RFC1048)
      {
        /* RFC1048 complient BOOTP vendor field
         * Based heavily on NCSA Telnet BOOTP
         */
        p = &bootp_in.bp_vend[4]; /* Point just after vendor field */

        /*@-infloops@*/

        while (*p != END_OPT && p <= &bootp_in.bp_vend[63])
        {
          DWORD ip;
          int   i;

          switch (*p)
          {
            case PAD_OPT:
                 p++;
                 break;

            case SUBNET_MASK:
                 sin_mask = intel (*(DWORD*)(p+2));
                 p += *(p+1) + 2;
                 break;

            case ROUTERS_ON_SNET:
                 /* only add first */
                 ip = intel (*(DWORD*)(p+2));
                 _arp_add_gateway (NULL, ip);
                 p += *(p+1) + 2;
                 break;

            case DNS_SRV:
                 for (i = 0; i < *(p+1); i += sizeof(ip))
                 {
                   ip = intel (*(DWORD*)(p+2+i));
                   _add_server (&last_nameserver, MAX_NAMESERVERS,
                                def_nameservers, ip);
                 }
                 p += *(p+1) + 2;
                 break;

            case COOKIE_SRV:
                 for (i = 0; i < *(p+1) ; i += sizeof(ip))
                 {
                   ip = intel (*(DWORD*)(p+2+i));
                   _add_server (&last_cookie, MAX_COOKIES, cookies, ip);
                 }
                 p += *(p+1) + 2;
                 break;

#if defined(USE_BSD_FUNC)
            case LOG_SRV:
                 ip = intel (*(DWORD*)(p+2)); /* select 1st host */
                 if (!syslog_hostName &&      /* not in config-file */
                     p[1] % 4 == 0)           /* length = n * 4 */
                 {
                   char buf[20];
                   syslog_hostName = strdup (_inet_ntoa(buf,ip));
                 }
                 p += *(p+1) + 2;
                 break;
#endif

            case HOST_NAME:
                 len = min (p[1], sizeof(hostname));
                 memcpy (&hostname[0], p+2, len);
                 hostname[len] = 0;
                 p += *(p+1) + 2;
                 break;

            case END_OPT:
                 break;

            case NAME_SRV:    /* IEN=116 name server */
            case LPR_SRV:
            case IMPRESS_SRV:
            case RES_LOCATION_SRV:

            case TIME_SRV:
            case TIME_OFFSET:
                 /* FALL THROUGH */

            default:
                 p += *(p+1) + 2;
                 break;
          }                  /* end of switch  */
        }                    /* end of while   */
      }                      /* if (intel()..  */
      goto give_up;
    }
  }

give_up:
  sock_close (bsock);
  return (my_ip_addr != 0);

sock_err:                    /* major network error if UDP fails */
  sock_close (bsock);
  return (0);
}

#endif /* USE_BOOTP */


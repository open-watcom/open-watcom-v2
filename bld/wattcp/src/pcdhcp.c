/*
 *
 *   DHCP - Dynamic Host Configuration Protocol (RFC 1541/2131/2132)
 *
 *   These extensions gets called if "MYIP" is set to "DHCP"
 *
 *   Version
 *
 *   0.5 : Oct 28, 1996 : G. Vanem - implemented from RFC1541 with
 *                                   help from pcbootp.c
 *
 *   0.6 : May 18, 1997 : G. Vanem - added RFC2131 DHCPINFORM message
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "wattcpd.h"
#include "strings.h"
#include "language.h"
#include "misc.h"
#include "udp_dom.h"
#include "bsdname.h"
#include "ip_out.h"
#include "syslog2.h"
#include "pctcp.h"
#include "pcbsd.h"
#include "pcsed.h"
#include "pcarp.h"
#include "pcqueue.h"
#include "pcpkt.h"
#include "pcconfig.h"
#include "pcbootp.h"
#include "pcdhcp.h"

#if defined(USE_DHCP)

#if defined(USE_TFTP)
#include "tftp.h"
#endif

/*@-usedef*/

#define BROADCAST_FLAG  1

static DWORD exchange_id;
static DWORD gateway;
static DWORD dhcp_server   = 0;
static DWORD suggestLease  = 0;
static DWORD dhcp_renewal  = 0;
static DWORD dhcp_rebind   = 0;
static DWORD dhcp_iplease  = 0;
static DWORD check_timer   = 0;
static long  renewal_count = 0;
static long  rebind_count  = 0;

static int   bcast_on      = 1;
static int   bcast_flag    = 0;
static int   renewing      = 0;
static int   got_offer     = 0;
static int   configured    = 0;
static int   do_inform     = 0;
static int   arp_check_ip  = 0;

static int   dhcp_timeout  = 10;
static int   max_retries   = 3;

static time_t     start_time;
static sock_type *sock;

static BYTE default_request_list[] = {
            SUBNET_MASK,
            ROUTERS_ON_SNET,
            DNS_SRV,
            COOKIE_SRV,
            LPR_SRV,
            HOST_NAME,
            DOMAIN_NAME,
            IP_DEFAULT_TTL,
            IF_MTU,
            ARP_CACHE_TIMEOUT,
            ETHERNET_ENCAPSULATION,
            TCP_DEFAULT_TTL,
#if defined(USE_BSD_FUNC)
            LOG_SRV,         /* links in syslog2 */
            NBIOS_NAME_SRV,  /* Samba needs this */
#endif
#if defined(USE_TFTP)
            DHCP_TFTP_SERVER,
            DHCP_BOOT_FILENAME
#endif
          };

static struct DHCP_list request_list = {
                        &default_request_list[0],
                        sizeof (default_request_list)
                      };

static struct DHCP_list extra_options = { NULL, 0 };

static void  dhcp_option_add  (BYTE *opt, int max);
static void  dhcp_options_free(void);
static void  dhcp_set_timers  (void);
static void  dhcp_daemon      (void);
static BYTE *put_request_list (BYTE *opt, int filled);

static int  trace_on = 0;

#if defined(USE_DEBUG)
  static char traceBuf [20];
  #define TRACE(fmt,arg)   do { if(trace_on)(*_printf)(fmt,arg); } while(0)
  #define INET_NTOA(x)     _inet_ntoa (traceBuf, x)
#else
  #define TRACE(fmt,arg)   ((void)0)
  #define INET_NTOA(x)     ((void)0)
#endif


/*-------------------------------------------------------------------*/

static void DHCP_bootHeader (struct dhcp *out, BYTE op)
{
  DWORD myip = intel (my_ip_addr);

  memset (out, 0, sizeof(*out));

  _eth_get_hwtype (&out->dh_htype, &out->dh_hlen);

  out->dh_op     = op;
  out->dh_xid    = exchange_id;
  out->dh_secs   = intel16 ((WORD)(time(NULL) - start_time));
  out->dh_flags  = (bcast_on && bcast_flag) ? BROADCAST_FLAG : 0;
  out->dh_yiaddr = myip;
  out->dh_ciaddr = myip;
  out->dh_giaddr = gateway;
  memcpy (&out->dh_chaddr, &_eth_addr, sizeof(eth_address));
  *(DWORD*) &out->dh_opt[0] = DHCP_MAGIC_COOKIE;
}

/*-------------------------------------------------------------------*/

static int DHCP_send (struct dhcp *out, BYTE *end, BOOL full_size)
{
  int len;

  if (full_size)
       len = sizeof (*out);
  else len = end - (BYTE*)out;

  return sock_fastwrite (sock, (BYTE*)out, len);
}

/*-------------------------------------------------------------------*/

static int DHCP_discover (struct dhcp *out)
{
  BYTE *opt   = (BYTE*) &out->dh_opt[4];
  BYTE *start = opt;

  exchange_id = set_timeout (0);       /* random exchange ID */
  DHCP_bootHeader (out, BOOTREQUEST);
  *opt++ = DHCP_MSG_TYPE;
  *opt++ = 1;
  *opt++ = DHCP_DISCOVER;

  *opt++ = DHCP_CLIENT_ID;             /* Client ID option */
  *opt++ = sizeof (_eth_addr) + 1;
  *opt++ = _eth_get_hwtype (NULL, NULL);
  memcpy (opt, &_eth_addr, sizeof(mac_address));
  opt += sizeof (mac_address);

  *opt++ = DHCP_MAX_MSG_SIZE;   /* Maximum DHCP message size */
  *opt++ = 2;
  *(WORD*)opt = intel16 (sizeof(struct dhcp));
  opt += 2;

  if (suggestLease)
  {
    *opt++ = DHCP_IP_ADDR_LEASE_TIME;
    *opt++ = sizeof (suggestLease);
    *(DWORD*)opt = intel (suggestLease);
    opt += sizeof (suggestLease);
  }
  opt = put_request_list (opt, opt-start);
  *opt++ = END_OPT;

  return DHCP_send (out, opt, TRUE);
}

/*-------------------------------------------------------------------*/

static int DHCP_inform (struct dhcp *out)
{
  BYTE *opt   = (BYTE*) &out->dh_opt[4];
  BYTE *start = opt;

  exchange_id = set_timeout (0);     /* random exchange ID */
  DHCP_bootHeader (out, BOOTREQUEST);
  *opt++ = DHCP_MSG_TYPE;
  *opt++ = 1;
  *opt++ = DHCP_INFORM;

  *opt++ = DHCP_CLIENT_ID;           /* Client ID option */
  *opt++ = sizeof (_eth_addr) + 1;
  *opt++ = _eth_get_hwtype (NULL, NULL);
  memcpy (opt, &_eth_addr, sizeof(mac_address));
  opt += sizeof (eth_address);

  opt = put_request_list (opt, opt-start);
  *opt++ = END_OPT;

  return DHCP_send (out, opt, FALSE);
}

/*-------------------------------------------------------------------*/

static int DHCP_request (struct dhcp *out, BOOL renew, DWORD ciaddr)
{
  BYTE *opt = (BYTE*) &out->dh_opt[4];

  DHCP_bootHeader (out, BOOTREQUEST);

  *opt++ = DHCP_MSG_TYPE;
  *opt++ = 1;
  *opt++ = DHCP_REQUEST;

  if (!renew)
  {
    *opt++ = DHCP_SRV_IDENTIFIER;
    *opt++ = sizeof (dhcp_server);
    *(DWORD*)opt = intel (dhcp_server);
    opt += sizeof (dhcp_server);
    *opt++ = DHCP_REQUESTED_IP_ADDR;
    *opt++ = sizeof (ciaddr);
    *(DWORD*)opt = ciaddr;
    opt += sizeof (ciaddr);
  }

  if (dhcp_iplease)
  {
    *opt++ = DHCP_IP_ADDR_LEASE_TIME;
    *opt++ = sizeof (dhcp_iplease);
    *(DWORD*)opt = intel (dhcp_iplease);
    opt += sizeof (dhcp_iplease);
  }
  *opt++ = END_OPT;

  return DHCP_send (out, opt, TRUE);
}

/*-------------------------------------------------------------------*/

static int DHCP_release_decline (struct dhcp *out, int msg_type, const char *msg)
{
  BYTE *opt = (BYTE*) &out->dh_opt[4];

  exchange_id = set_timeout (0);     /* new exchange ID */
  DHCP_bootHeader (out, BOOTREQUEST);
  out->dh_secs = 0;

  *opt++ = DHCP_MSG_TYPE;
  *opt++ = 1;
  *opt++ = msg_type;
  *opt++ = DHCP_SRV_IDENTIFIER;
  *opt++ = sizeof (dhcp_server);
  *(DWORD*)opt = intel (dhcp_server);
  opt += sizeof (dhcp_server);

  if (msg)
  {
    int len = strlen (msg);
    *opt++ = DHCP_MSG;
    *opt++ = len;
    strcpy ((char*)opt, msg);
    opt += len;
  }
  *opt++ = END_OPT;

  return DHCP_send (out, opt, FALSE);
}

/*-------------------------------------------------------------------*/

static int DHCP_process_ack (struct dhcp *out)
{
  DWORD bcast_ip = my_ip_addr | ~sin_mask;

  TRACE ("Got DHCP ack\r\n", 0);

  /* ARP broadcast to announce our new IP
   */
  _arp_reply (NULL, intel(bcast_ip), intel(my_ip_addr));

  if (arp_check_ip)       /* check if IP is used by anybody else */
  {
    TRACE ("Checking ARP..", 0);
    if (_arp_check_own_ip())
    {
      outsnl ("\7Someone has the same IP! Declining..");
      my_ip_addr = 0;     /* decline from 0.0.0.0 */

      DHCP_release_decline (out, DHCP_DECLINE, _LANG("IP is not free"));
      RandomWait (4000, 6000);
      return (0);
    }
    TRACE ("\r\n", 0);
  }
  configured = 1;    /* we are (re)configured */
  return (1);
}

/*-------------------------------------------------------------------*/

static int DHCP_offer (struct dhcp *in)
{
  int    len;
  DWORD  gw, ip;
  BYTE  *opt = (BYTE*) &in->dh_opt[4];

  while (opt < in->dh_opt + sizeof(in->dh_opt))
  {
    switch (*opt)
    {
      case PAD_OPT:
           opt++;
           continue;

      case SUBNET_MASK:
           sin_mask = intel (*(DWORD*)(opt+2));
           TRACE ("Net-mask: %s\r\n", INET_NTOA(sin_mask));
           break;

      case TIME_OFFSET:
#if defined(USE_DEBUG)
           {
             long tofs = *(long*)(opt+2);
             TRACE ("Time-ofs: %.2fh\r\n", (double)tofs/3600.0);
           }
#endif
           break;

      case ROUTERS_ON_SNET:
           {
             static int gw_added = 0;

             if (!gw_added)
                arp_last_gateway = 0; /* delete gateways from cfg-file */
             gw_added = 1;
             gw = intel (*(DWORD*)(opt+2));
             _arp_add_gateway (NULL, gw);
             TRACE ("Gateway:  %s\r\n", INET_NTOA(gw));
           }
           break;

      case DNS_SRV:
           {
             static int dns_added = 0;

             if (!dns_added)
                last_nameserver = 0;  /* delete nameserver from cfg-file */

             for (len = 0; len < *(opt+1); len += sizeof(DWORD))
             {
               ip = intel (*(DWORD*)(opt+2+len));
               /*
                * !!fix-me: Add only first Name-server cause resolve() doesn't
                * handle multiple servers (in different nets) very well
                */
               if (len == 0)
                  _add_server (&last_nameserver,
                               MAX_NAMESERVERS, def_nameservers, ip);
               dns_added = 1;
               TRACE ("DNS:      %s\r\n", INET_NTOA(ip));
             }
           }
           break;

#if defined(USE_BSD_FUNC)
      case LOG_SRV:
           {
             static int srv_added = 0;

             if (!srv_added)        /* only use first log-server */
                break;
             
             ip = intel (*(DWORD*)(opt+2)); /* select 1st host */
             TRACE ("LOG:      %s\r\n", INET_NTOA(ip));

             if (!syslog_hostName &&  /* not in config-file */
                 opt[1] % 4 == 0)     /* length = n * 4 */
             {
               char buf[20];
               syslog_hostName = strdup (_inet_ntoa(buf,ip));
               srv_added = 1;
             }
           }
           break;

      case NBIOS_NAME_SRV:
           ip = intel (*(DWORD*)(opt+2));
           TRACE ("WNS:      %s\r\n", INET_NTOA(ip));
           /* !! to-do: make a hook for Samba */
           break;
#endif

      case HOST_NAME:
          /* Don't use sethostname() because '*(opt+2)' is not a FQDN.
           */
           len = min (opt[1], sizeof(hostname));
           memcpy (hostname, opt+2, len);
           hostname[len] = 0;
           TRACE ("Host name: `%s'\r\n", hostname);
           break;

      case DOMAIN_NAME:
           len = min (opt[1], sizeof(defaultdomain)-1);
           setdomainname ((const char*)(opt+2), len);
           TRACE ("Domain:  `%s'\r\n", def_domain);
           break;

      case IP_DEFAULT_TTL:
      case TCP_DEFAULT_TTL:
           _default_ttl = opt[2];
           break;

      case DHCP_MSG_TYPE:
           if (opt[2] == DHCP_OFFER)
              got_offer = 1;
           break;

      case DHCP_MSG:
           outsn ((const char*)(opt+2), *(opt+1));
           break;

      case DHCP_SRV_IDENTIFIER:
           dhcp_server = intel (*(DWORD*)(opt+2));
           TRACE ("Server:   %s\r\n", INET_NTOA(dhcp_server));
           break;

      case DHCP_IP_ADDR_LEASE_TIME:
           dhcp_iplease = intel (*(DWORD*)(opt+2));
           TRACE ("IP lease: %lu hrs\r\n", dhcp_iplease/3600L);
           break;

      case DHCP_T1_VALUE:
           dhcp_renewal = intel (*(DWORD*)(opt+2));
           TRACE ("Renewal:  %lu hrs\r\n", dhcp_renewal/3600L);
           break;

      case DHCP_T2_VALUE:
           dhcp_rebind = intel (*(DWORD*)(opt+2));
           TRACE ("Rebind:   %lu hrs\r\n", dhcp_rebind/3600L);
           break;

      case TCP_KEEPALIVE_INTERVAL:
           tcp_keepalive = intel (*(DWORD*)(opt+2));
           break;

      case DHCP_OPT_OVERLOAD:
           switch (opt[2])
           {
             case 1:
                  TRACE ("Overload: `dh_file' options\r\n", 0);
                  dhcp_option_add (in->dh_file, sizeof(in->dh_file));
                  break;
             case 2:
                  TRACE ("Overload: `dh_sname' options\r\n", 0);
                  dhcp_option_add (in->dh_sname, sizeof(in->dh_sname));
                  break;
             case 3:
                  TRACE ("Overload: `dh_file/dh_sname' options\r\n", 0);
                  dhcp_option_add (in->dh_file, sizeof(in->dh_file));
                  dhcp_option_add (in->dh_sname, sizeof(in->dh_sname));
                  break;
           }
           break;

#if defined(USE_TFTP)
      case DHCP_TFTP_SERVER:
           {
             char *p = tftp_set_server ((const char*)(opt+2), opt[1]);
             TRACE ("TFTP-serv: `%s'\r\n", p);
           }
           break;

      case DHCP_BOOT_FILENAME:
           {
             char *p = tftp_set_boot_fname ((const char*)(opt+2), opt[1]);
             TRACE ("BOOT-file: `%s'\r\n", p);
           }
           break;
#endif

      case END_OPT:
           TRACE ("got end-option\r\n", 0);
           return (got_offer);

      default:
           TRACE ("Ignoring option %d\r\n", *opt);
           break;
    }
    opt += *(opt+1) + 2;
  }

  if (extra_options.data)
  {
    struct dhcp ext;

    len = min (extra_options.size, sizeof(ext.dh_opt));
    extra_options.data [len] = END_OPT;
    memcpy (ext.dh_opt, extra_options.data, len);
    DHCP_offer (&ext);
  }
  return (got_offer);
}

/*-------------------------------------------------------------------*/

static int DHCP_ack (struct dhcp *in)
{
  BYTE *opt = (BYTE*) &in->dh_opt[4];

  return (opt[0] == DHCP_MSG_TYPE && opt[1] == 1 && opt[2] == DHCP_ACK);
}

/*-------------------------------------------------------------------*/

static int DHCP_nack (struct dhcp *in)
{
  BYTE *opt = (BYTE*) &in->dh_opt[4];

  return (opt[0] == DHCP_MSG_TYPE && opt[1] == 1 && opt[2] == DHCP_NAK);
}

/*-------------------------------------------------------------------*/

void DHCP_release (void)
{
  struct udp_Socket socket;
  struct dhcp       dhcp_out;

  if (!configured)
     return;

#if 0 /* !!to-do: if DHCP-config is saved on disk, use that next time */
  if (DHCP_remaining_lease() > 10)
     return;

  set_config ("dhcp_iplease", 0);
#endif

  TRACE ("Sending DHCP release\r\n", 0);  /* release is unicast */

  udp_open (&socket, IPPORT_BOOTPC, dhcp_server, IPPORT_BOOTPS, NULL);
  sock = (sock_type*) &socket;

  DHCP_release_decline (&dhcp_out, DHCP_RELEASE, NULL);
  sock_close (sock);
}

/*-------------------------------------------------------------------*/

static sock_type *dhcp_open (int reconf)
{
  static udp_Socket socket;
  DWORD  host;

  outs (reconf ? _LANG("Reconfiguring through DHCP..")
               : _LANG("Configuring through DHCP..."));

  if (bcast_on)
       host = (DWORD)-1;
  else host = dhcp_server;

  udp_open (&socket, IPPORT_BOOTPC, host, IPPORT_BOOTPS, NULL);
  return (sock_type*)&socket;
}

/*-------------------------------------------------------------------*/

static void arp_add_server (sock_type *sock)
{
  if ((_pktdevclass == PD_ETHER || _pktdevclass == PD_TOKEN) &&
      memcmp(&sock->udp.hisethaddr, &_eth_brdcast, sizeof(_eth_brdcast)))
    _arp_add_cache (dhcp_server, &sock->udp.hisethaddr, TRUE);
}

/*
 * _dodhcp() - our first time (booting) DHCP handler.
 * Doesn't hurt that it's blocking, but should be rewitten together
 * with dhcp_renew() to make a proper state machine.
 */
int _dodhcp (void)
{
  struct dhcp dhcp_out;
  struct dhcp dhcp_in;
  int    status, loop, len;
  int    save_mtu = mtu;

  mtu        = ETH_MAX_DATA;
  sock       = dhcp_open (0);
  start_time = time (NULL);

  for (loop = 0; loop < max_retries; loop++)
  {
    DWORD sendtimeout;

    TRACE ("Sending DHCP discover (%d)\r\n", loop);

    if (!DHCP_discover (&dhcp_out))
       goto sock_exit;

    sendtimeout = set_timeout (dhcp_timeout * 1000);

    while (!chk_timeout(sendtimeout))
    { 
      sock_tick (sock, &status);
      if (!sock_dataready(sock))
         continue;

      len = sock_fastread (sock, (BYTE*)&dhcp_in, sizeof(dhcp_in));

      if (len < DHCP_MIN_SIZE ||              /* too short packet */
          dhcp_in.dh_op  != BOOTREPLY ||      /* not a BOOT reply */
          dhcp_in.dh_xid != dhcp_out.dh_xid)  /* not our exchange ID */
         continue;
     
      gateway = dhcp_in.dh_giaddr;

      if (!got_offer && DHCP_offer(&dhcp_in))
      {
        TRACE ("Got DHCP offer\r\n", 0);
        RandomWait (1000, 2000);

        if (dhcp_iplease == 0L)                  /* cannot happen ? */
            dhcp_iplease = 3600L;
        if (dhcp_renewal == 0L)
            dhcp_renewal = dhcp_iplease / 2;     /* default T1 time */
        if (dhcp_rebind == 0)
            dhcp_rebind = dhcp_iplease * 7 / 8;  /* default T2 time */

        TRACE ("Sending DHCP request\r\n", 0);
        DHCP_request (&dhcp_out, renewing, dhcp_in.dh_yiaddr);

        /* Remember my_ip_addr from OFFER because WinNT server
         * doesn't include it in ACK message.
         */
        my_ip_addr = ((udp_Socket*)sock)->myaddr = intel (dhcp_in.dh_yiaddr);
        TRACE ("my_ip_addr = %s\r\n", INET_NTOA(my_ip_addr));

        sendtimeout = set_timeout (10000);
        continue;
      }

      if (DHCP_ack(&dhcp_in))
      {
        if (!DHCP_process_ack(&dhcp_out))
           continue;

        dhcp_set_timers();
        addwattcpd (dhcp_daemon);
#if 0
       /* !!to-do: save DHCP-config to disk (temp-file) for use next
        *          time we boot. Hence no need to do all this each time
        */
#endif
        goto sock_exit;    /* OK return */
      }
      if (DHCP_nack(&dhcp_in))
      {
        TRACE ("Got DHCP nack\r\n", 0);
        my_ip_addr = 0;
        break;          /* retry */
      }

      memset (&dhcp_in, 0, sizeof(dhcp_in));
      continue;
    }
  }

sock_exit:
sock_err:

  if (dhcp_server)
     arp_add_server (sock);

  sock_close (sock);
  dhcp_options_free();

  mtu = save_mtu;
  return (my_ip_addr != 0);
}

/*
 * dhcp_renew() - enter renewing or rebinding state
 * !! to-do: make this a non-blocking state-machine.
 */
static int dhcp_renew (void)  
{
  struct dhcp dhcp_out;
  struct dhcp dhcp_in;
  int    status, loop, len;

  sock       = dhcp_open (1);
  start_time = time (NULL);

  for (loop = 0; loop < max_retries; loop++)
  {
    DWORD sendtimeout;

    TRACE ("Sending DHCP request\r\n", 0);
    if (!DHCP_request (&dhcp_out, renewing, intel(my_ip_addr)))
       goto sock_exit;

    sendtimeout = set_timeout (dhcp_timeout * 1000);

    while (!chk_timeout(sendtimeout))
    {
      sock_tick (sock, &status);
      if (!sock_dataready(sock))
         continue;

      len = sock_fastread (sock, (BYTE*)&dhcp_in, sizeof(dhcp_in));

      if (len < DHCP_MIN_SIZE ||              /* too short packet */
          dhcp_in.dh_op  != BOOTREPLY ||      /* not a BOOT reply */
          dhcp_in.dh_xid != dhcp_out.dh_xid)  /* not our exchange ID */
         continue;
     
      gateway = dhcp_in.dh_giaddr;
      if (DHCP_ack(&dhcp_in))
      {
        if (!DHCP_process_ack(&dhcp_out))
           continue;

        dhcp_set_timers();
        addwattcpd (dhcp_daemon);
        goto sock_exit;           /* OK return */
      }
      if (DHCP_nack(&dhcp_in))
      {
        TRACE ("Got DHCP nack\r\n", 0);
        my_ip_addr = 0;
        break;          /* retry */
      }
      memset (&dhcp_in, 0, sizeof(dhcp_in));
      continue;
    }
  }

sock_exit:
sock_err:
  sock_close (sock);
  return (my_ip_addr != 0);
}

/*-------------------------------------------------------------------*/

static void dhcp_option_add (BYTE *opt, int max)
{
  int   len = 0;
  char *add;

  extra_options.data = realloc (extra_options.data, max);
  if (!extra_options.data)
     return;

  add = (char*)&extra_options.data + extra_options.size;

  /* Loop over `opt' and append to `add'. Strip away END_OPT
   * and PAD_OPT options.
   */
  while (opt < opt + max)
  {
    if (*opt == PAD_OPT)
       continue;
    if (*opt == END_OPT)
       return;

    len = opt[1];
    memcpy (add, opt, len+2);
    add += len + 2;
    opt += len + 2;
    extra_options.size += len + 2;
  }
}

/*-------------------------------------------------------------------*/

static void dhcp_options_free (void)
{
  if (extra_options.data)
  {
    free (extra_options.data);
    extra_options.data = NULL;
    extra_options.size = 0;
  }
}

/*-------------------------------------------------------------------*/

void _dodhcp_inform (void)
{
  struct dhcp dhcp_out;
  struct dhcp dhcp_in;
  udp_Socket  socket;
  int         status, len;
  DWORD       sendtimeout;

  if (!do_inform || !dhcp_server)
     return;
 
  udp_open (&socket, IPPORT_BOOTPC, dhcp_server, IPPORT_BOOTPS, NULL);
  sock       = (sock_type*) &socket;
  start_time = time (NULL);

  if (trace_on || debug_on)
     outs (_LANG("Sending DHCP inform.."));

  DHCP_inform (&dhcp_out);
  sendtimeout = set_timeout (dhcp_timeout * 1000);

  while (!chk_timeout(sendtimeout))
  {
    sock_tick (sock, &status);
    if (!sock_dataready(sock))
       continue;

    len = sock_fastread (sock, (BYTE*)&dhcp_in, sizeof(dhcp_in));

    if (len < DHCP_MIN_SIZE ||              /* too short packet */
        dhcp_in.dh_op  != BOOTREPLY ||      /* not a BOOT reply */
        dhcp_in.dh_xid != dhcp_out.dh_xid)  /* not our exchange ID */
       continue;

    if (DHCP_ack(&dhcp_in))
       break;
  }

sock_err:
  sock_close (sock);
  if (chk_timeout(sendtimeout))
       TRACE ("Got no DHCP ack\r\n", 0);
  else TRACE ("Got DHCP ack\r\n", 0);
}

/*-------------------------------------------------------------------*/

static void dhcp_set_timers (void)
{
  check_timer   = set_timeout (1000); /* count down each sec */
  renewal_count = (long)dhcp_renewal;
  rebind_count  = (long)dhcp_rebind;
  if (rebind_count == renewal_count)
      rebind_count += 10;             /* add 10 seconds */
}

/*
 * dhcp_daemon() - called from tcp_tick()
 */
static void dhcp_daemon (void) 
{
  BOOL t1, t2;

  if (!chk_timeout(check_timer))
     return;

  check_timer = set_timeout (1000);
  t1 = (--renewal_count <= 0);
  t2 = (--rebind_count  <= 0);

  if (!t1 && !t2)
     return;

  renewing = 1;
  bcast_on = 0;              /* can Unicast now       */
  if (t1) gateway  = 0;      /* don't use relay agent */
  if (t2) bcast_on = 1;      /* must use Broadcast    */

  TRACE ("DHCP daemon timeout: %s\r\n", t1 ? "RENEWAL" : "REBIND");

  suggestLease = dhcp_iplease;
  delwattcpd (dhcp_daemon);  /* don't run daemon now */
  dhcp_renew();
}

/*
 * Parse a list of DHCP-request options from config-file.
 * e.g DHCP_REQ_LIST = 1,23,24,28,36
 */
static int set_request_list (char *options)
{
  static int init = 0;
  int    num      = 0;
  int    maxreq   = 312 - 27; /* sizeof(dh_opt) - min size of rest */
  BYTE  *list, *start, *tok, *end;

  if (init || (list = calloc(maxreq,1)) == NULL)
     return (0);

  init  = 1;
  start = list;
  end   = start + maxreq - 1;
  tok   = (BYTE*) strtok (options, ", \t");

  while (tok && list < end)
  {
    *list = atoi ((const char*)tok);
    tok   = (BYTE*) strtok (NULL, ", \t");

    /* If request list start with Pad option ("DHCP.REQ_LIST=0"),
     * disable options all-together.
     */
    if (num == 0 && *list == '0')
       break;
    num++;
    list++;
  }

  request_list.data = start;
  request_list.size = num;

#if 0  /* test */
  {
    int i;
    for (i = 0; i < request_list.size; i++)
        printf ("%2d, ", request_list.data[i]);
    puts ("");
  }
#endif
  return (1);
}

static BYTE *put_request_list (BYTE *opt, int filled)
{
  struct dhcp dhcp;
  int    size = min (request_list.size, sizeof(dhcp.dh_opt)-filled-1);
                                               /* room for END_OPT ^ */
  if (size > 0 && request_list.data)
  {
    *opt++ = DHCP_PARAM_REQUEST;
    *opt++ = request_list.size;
    memcpy (opt, request_list.data, size);
    opt += size;
  }
  return (opt);
}

/*-------------------------------------------------------------------*/

static void (*prev_hook) (const char*, const char*) = NULL;

static void DHCP_config (const char *name, const char *value)
{
  static struct config_table dhcp_cfg[] = {
                { "REQ_LIST", ARG_FUNC,   (void*)set_request_list },
                { "TRACE",    ARG_ATOI,   (void*)&trace_on        },
                { "BCAST",    ARG_ATOI,   (void*)&bcast_flag      },
                { "INFORM",   ARG_ATOI,   (void*)&do_inform       },
                { "TIMEOUT",  ARG_ATOI,   (void*)&dhcp_timeout    },
                { "RETRIES",  ARG_ATOI,   (void*)&max_retries     },
                { "ARPCHECK", ARG_ATOI,   (void*)&arp_check_ip    },
                { "HOST",     ARG_RESOLVE,(void*)&dhcp_server     },
                { NULL }
              };

  if (!parse_config_table(&dhcp_cfg[0], "DHCP.", name, value) && prev_hook)
     (*prev_hook) (name, value);
}

void DHCP_init (void)
{
  prev_hook = usr_init;
  usr_init  = DHCP_config;
}

#endif /* USE_DHCP */

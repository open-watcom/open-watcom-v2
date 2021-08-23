#ifndef __PCDHCP_H
#define __PCDHCP_H

#define DHCP_MAGIC_COOKIE  0x63538263   /* magic cookie */


enum { DHCP_DISCOVER = 1,
       DHCP_OFFER,
       DHCP_REQUEST,
       DHCP_DECLINE,
       DHCP_ACK,
       DHCP_NAK ,
       DHCP_RELEASE,
       DHCP_INFORM
     };

enum { DHCP_RENEWAL_TO = 1,
       DHCP_REBIND_TO
     };

/*
 * DHCP options and values (ref. RFC1533)
 * Most are also BOOTP options.
 */
enum { PAD_OPT                  =  0,
       END_OPT                  = 255,
       SUBNET_MASK              =  1,
       TIME_OFFSET              =  2,
       ROUTERS_ON_SNET          =  3,
       TIME_SRV                 =  4,
       NAME_SRV                 =  5,
       DNS_SRV                  =  6,
       LOG_SRV                  =  7,
       COOKIE_SRV               =  8,
       LPR_SRV                  =  9,
       IMPRESS_SRV              = 10,
       RES_LOCATION_SRV         = 11,
       HOST_NAME                = 12,
       BOOT_FSIZE               = 13,
       MERIT_DUMPFILE           = 14,
       DOMAIN_NAME              = 15,
       SWAP_SRV                 = 16,
       ROOT_PATH                = 17,
       EXTENTIONS_PATH          = 18,
       IP_FORWARDING            = 19,
       NON_LOCAL_SRC_ROUTE      = 20,
       POLICY_FILTER            = 21,
       MAX_DGRAM_REASM_SIZE     = 22,
       IP_DEFAULT_TTL           = 23,
       PATH_MTU_AGING_TIMEOUT   = 24,
       PATH_MTU_PLATEAU_TABLE   = 25,
       IF_MTU                   = 26,
       ALL_SUBNETS_LOCAL        = 27,
       BROADCAST_ADDR           = 28,
       PERFORM_MASK_DISCOVERY   = 29,
       MASK_SUPPLIER            = 30,
       PERFORM_ROUTER_DISCOVERY = 31,
       ROUTER_SOLICITATION_ADDR = 32,
       STATIC_ROUTE             = 33,
       TRAILER_ENCAPSULATION    = 34,
       ARP_CACHE_TIMEOUT        = 35,
       ETHERNET_ENCAPSULATION   = 36,
       TCP_DEFAULT_TTL          = 37,
       TCP_KEEPALIVE_INTERVAL   = 38,
       TCP_KEEPALIVE_GARBAGE    = 39,
       NIS_DOMAIN_NAME          = 40,
       NIS_SRVS                 = 41,
       NTP_SRVS                 = 42,
       VENDOR_SPECIFIC_INFO     = 43,
       NBIOS_NAME_SRV           = 44,
       NBIOS_DGRAM_DIST_SRV     = 45,
       NBIOS_NODE_TYPE          = 46,
       NBIOS_SCOPE              = 47,
       XFONT_SRV                = 48,
       XDISPLAY_MANAGER         = 49,
       DHCP_REQUESTED_IP_ADDR   = 50,
       DHCP_IP_ADDR_LEASE_TIME  = 51,
       DHCP_OPT_OVERLOAD        = 52,
       DHCP_MSG_TYPE            = 53,
       DHCP_SRV_IDENTIFIER      = 54,
       DHCP_PARAM_REQUEST       = 55,
       DHCP_MSG                 = 56,
       DHCP_MAX_MSG_SIZE        = 57,
       DHCP_T1_VALUE            = 58,
       DHCP_T2_VALUE            = 59,
       DHCP_CLASS_ID            = 60,
       DHCP_CLIENT_ID           = 61,
       DHCP_NIS_DOMAIN_OPT      = 64,
       DHCP_NIS_SRV_OPT         = 65,
       DHCP_TFTP_SERVER         = 66,
       DHCP_BOOT_FILENAME       = 67,
       DHCP_MOBIP_HOME_AGENTS   = 68,
       DHCP_SMTP_SRVS           = 69,
       DHCP_POP3_SRVS           = 70,
       DHCP_NNTP_SRVS           = 71,
       DHCP_WWW_SRVS            = 72,
       DHCP_FINGER_SRVS         = 73,
       DHCP_IRC_SRVS            = 74,
       DHCP_STREET_TALK_SRVS    = 75,
       DHCP_STDA_SRVS           = 76
     };

#include <sys/packon.h>

struct dhcp  {
       BYTE  dh_op;         /* packet op code / message type.            */
       BYTE  dh_htype;      /* hardware address type, 1 = 10 mb ethernet */
       BYTE  dh_hlen;       /* hardware address len, eg '6' for 10mb eth */
       BYTE  dh_hops;       /* client sets to zero, optionally used by   */
                            /* gateways in cross-gateway booting.        */
       DWORD dh_xid;        /* transaction ID, a random number           */
       WORD  dh_secs;       /* filled in by client, seconds elapsed      */
                            /* since client started trying to boot.      */
       WORD  dh_flags;
       DWORD dh_ciaddr;     /* client IP address filled in by client if known*/
       DWORD dh_yiaddr;     /* 'your' (client) IP address                */
                            /* filled by server if client doesn't know   */
       DWORD dh_siaddr;     /* server IP address returned in bootreply   */
       DWORD dh_giaddr;     /* gateway IP address,                       */
                            /* used in optional cross-gateway booting.   */
       BYTE  dh_chaddr[16]; /* client hardware address, filled by client */
       BYTE  dh_sname[64];  /* optional server host name, null terminated*/

       BYTE  dh_file[128];  /* boot file name, null terminated string    */
                            /* 'generic' name or null in bootrequest,    */
                            /* fully qualified directory-path            */
                            /* name in bootreply.                        */
       BYTE  dh_opt[312];   /* DHCP options area (minimum 308 bytes)     */
     };

#define DHCP_MIN_SIZE (sizeof(struct dhcp) - 64 - 128 - 312)

#include <sys/packoff.h>


struct DHCP_list {
       BYTE *data;
       int   size;
     };

extern int  _dodhcp (void);
extern void _dodhcp_inform (void);
extern void DHCP_init (void);
extern void DHCP_release (void);

#endif

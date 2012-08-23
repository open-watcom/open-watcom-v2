#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "chksum.h"
#include "misc.h"
#include "pctcp.h"
#include "pcsed.h"
#include "pcarp.h"
#include "pcbsd.h"
#include "pcstat.h"
#include "pcdbug.h"
#include "pcconfig.h"
#include "pcqueue.h"
#include "pcpkt.h"
#include "sock_ini.h"
#include "ip_out.h"
#include "pcicmp.h"

/*
 * ICMP - RFC 792
 */

const char *icmp_type_str [ICMP_MAXTYPE+1] = {
      __LANG("Echo Reply"),  "1", "2",
      __LANG("Destination Unreachable"),
      __LANG("Source Quench"),
      __LANG("Redirect"),    "6", "7",
      __LANG("Echo Request"),
      __LANG("Router Advert"),
      __LANG("Router Solic"),
      __LANG("TTL exceeded"),
      __LANG("Param Problem"),
      __LANG("Timestamp Message"),
      __LANG("Timestamp Reply"),
      __LANG("Info Request"),
      __LANG("Info Reply"),
      __LANG("Mask Request"),
      __LANG("Mask Reply")
    };

const char *icmp_unreach_str [16] = {
      __LANG("Network Unreachable"),
      __LANG("Host Unreachable"),
      __LANG("Protocol Unreachable"),
      __LANG("Port Unreachable"),
      __LANG("Fragmentation needed and DF set"),
      __LANG("Source Route Failed"),
      __LANG("Network unknown"),
      __LANG("Host unknown"),
      __LANG("Source host isolated"),
      __LANG("Net-access denied"),
      __LANG("Host-access denied"),
      __LANG("Bad TOS for net"),
      __LANG("Bad TOS for host"),
      __LANG("Admin prohibited"),
      __LANG("Host precedence violation"),
      __LANG("Precedence cutoff")
    };

const char *icmp_redirect_str [4] = {
      __LANG("Redirect for Network"),
      __LANG("Redirect for Host"),
      __LANG("Redirect for TOS and Network"),
      __LANG("Redirect for TOS and Host")
    };

const char *icmp_exceed_str [2] = {
      __LANG("TTL exceeded in transit"),
      __LANG("Frag ReAsm time exceeded")
    };

#include <sys/packon.h>

struct _pkt {
       in_Header  in;
       ICMP_PKT   icmp;
       in_Header  data;
     };

#include <sys/packoff.h>

static struct {
       char  icmp;
       char  igmp;
       char  udp;
       char  tcp;
     } do_redirect = { 1,1,1,1 };

static DWORD ping_hcache = 0;   /* host */
static DWORD ping_tcache = 0;   /* time */
static DWORD ping_number = 0;


DWORD _chk_ping (DWORD host, DWORD *ping_num)
{
  if (ping_hcache == host)
  {
    ping_hcache = 0xFFFFFFFFL;
    *ping_num = ping_number;
    return (ping_tcache);
  }
  return (0xFFFFFFFFL);
}


static void icmp_print (int dbg_lvl, const char *msg, DWORD src)
{
  if (debug_on < dbg_lvl)
     return;

  outs ("\r\nICMP: ");
  if (src)
  {
    char adr[20];
    outs ("(");
    outs (_inet_ntoa(adr,intel(src)));
    outs ("): ");
  }
  outsnl (_LANG(msg));
}

/*
 * print info about bogus and possibly dangerous ICMP messages
 * e.g. "ICMP: (144.133.122.111) Bogus Redirect; GW = 111.110.109.108"
 */
static void icmp_bogus (const in_Header *ip, int type, const char *msg)
{
  char buf[100];

  strcpy (buf, _LANG("Bogus "));
  strcat (buf, icmp_type_str[type]);
  if (msg)
     strcat (buf, msg);
  icmp_print (1, buf, ip->source);
}

#ifdef NOT_USED
/*
 */
void *icmp_Format (DWORD host)
{
  mac_address dest;

  /* We use ARP rather than supplied hardware address.
   * After first ping this will still be in cache.
   */
  if (!_arp_resolve(host,&dest,0))  /* unable to find address */
     return (NULL);     

  return _eth_formatpacket (&dest, IP_TYPE);
}
#endif


/*
 * Check if ip-source is a (directed) broadcast address.
 * Some hacker may try to create a broadcast storm.
 * Also check for null source address (0.0.0.0).
 * Broadcast destination is already filtered out by icmp_handler().
 */
static int icmp_chk_src (const in_Header *ip, int type)
{
  BOOL bcast = (~intel(ip->source) & ~sin_mask) == 0;

  if (bcast)
  {
    icmp_bogus (ip, type, _LANG(" (broadcast)"));
    return (0);
  }
  if (ip->source == 0UL)
  {
    icmp_bogus (ip, type, _LANG(" (network)"));
    return (0);
  }
  return (1);
}

/*
 * icmp_send - format and send a ICMP packet
 *           - note that src and dest are network order.
 */
static int icmp_send (struct _pkt *pkt, DWORD src, DWORD dest, int length)
{
  in_Header *ip   = &pkt->in;
  ICMP_PKT  *icmp = &pkt->icmp;

  icmp->unused.checksum = 0;
  icmp->unused.checksum = ~checksum (icmp, length);

  return IP_OUTPUT (ip, src, dest, ICMP_PROTO, 0, 0, 0, length, NULL);
}

/*
 *  Send an ICMP destination/protocol unreachable back to ip->source
 */
void icmp_unreach (const in_Header *ip, int code)
{
  struct _pkt    *pkt;
  union icmp_pkt *unr;
  int             len;

  if (!icmp_chk_src(ip,ICMP_UNREACH))
     return;

  pkt = (struct _pkt*) _eth_formatpacket (MAC_SRC(ip), IP_TYPE);
  unr = &pkt->icmp;
  len = intel16 (ip->length) - in_GetHdrLen (ip);
  len = min (len, sizeof(*ip)+sizeof(unr->unused.spares));

  icmp_print (1, icmp_unreach_str[code], ip->destination);
  memcpy (&unr->unused.ip, ip, len);
  unr->unused.type = ICMP_UNREACH;
  unr->unused.code = code;

  icmp_send (pkt, ip->destination, ip->source, sizeof(unr->unused));
}

/*
 *  Send an "ICMP Time Exceeded" (reassebly timeout) back to 'ip->source'
 */
void icmp_timexceed (const in_Header *ip, const void *mac_dest)
{
  struct _pkt    *pkt;
  union icmp_pkt *tim;
  int             len;

  if (!icmp_chk_src(ip,ICMP_TIMXCEED))
     return;

  pkt = (struct _pkt*) _eth_formatpacket (mac_dest, IP_TYPE);
  tim = &pkt->icmp;
  len = intel16 (ip->length) - in_GetHdrLen (ip);
  len = min (len, sizeof(*ip) + sizeof(tim->unused.spares));

  icmp_print (1, icmp_exceed_str[1], ip->destination);
  memcpy (&tim->unused.ip, ip, len);
  tim->unused.type = ICMP_TIMXCEED;
  tim->unused.code = 1;

  icmp_send (pkt, ip->destination, ip->source, sizeof(tim->unused));
}

/*
 *  Send an ICMP Address Mask Request as link-layer + IP broadcast
 *  Even if we know our address, we send with src = 0.0.0.0.
 */
static WORD addr_mask_id  = 0;
static WORD addr_mask_seq = 0;

void icmp_mask_req (void)
{
  mac_address    *dst = (_pktserial ? NULL : &_eth_brdcast);
  struct _pkt    *pkt = (struct _pkt*) _eth_formatpacket (dst, IP_TYPE);
  union icmp_pkt *req = &pkt->icmp;

  addr_mask_id = (WORD) set_timeout (0);   /* get a random ID */
  req->mask.type       = ICMP_MASKREQ;
  req->mask.code       = 0;
  req->mask.identifier = addr_mask_id;
  req->mask.sequence   = addr_mask_seq++;
  req->mask.mask       = 0;
  icmp_send (pkt, 0, (DWORD)INADDR_BROADCAST, sizeof(req->mask));
}

/*
 *  Handler for incoming ICMP packets
 */
void icmp_handler (const in_Header *ip, BOOL broadcast)
{
  union icmp_pkt *icmp;
  in_Header      *orig_ip;
  int             len, type, code;
  BOOL            for_me, i_orig;  /* is it for me, did I originate it */
  const char     *msg;

  DEBUG_RX (NULL, ip);

  if (block_icmp)   /* application is handling ICMP; not needed */
     return;

  len    = in_GetHdrLen (ip);
  icmp   = (union icmp_pkt*) ((BYTE*)ip + len);
  len    = intel16 (ip->length) - len;
  for_me = (DWORD) (intel(ip->destination) - my_ip_addr) <= multihomes;

  if (!for_me || broadcast)  /* drop broadcast pings.. */
     return;

  if (len < sizeof(icmp->info))
  {
    STAT (icmpstats.icps_tooshort++);
    return;
  }

  if (checksum(icmp,len) != 0xFFFF)
  {
    STAT (icmpstats.icps_checksum++);
    icmp_print (1, _LANG("bad checksum"), ip->source);
    return;
  }

  type    = icmp->unused.type;
  code    = icmp->unused.code;
  orig_ip = &icmp->ip.ip;
  i_orig  = is_local_addr (intel(orig_ip->source));

  if (type == ICMP_MASKREPLY)
  {
    if (!_domask_req)
       return;
    i_orig = TRUE;
  }

  /* !! this needs work
   */
  if (!i_orig &&
      (type != ICMP_ECHOREPLY && type != ICMP_ECHO &&
       type != ICMP_IREQREPLY && type != ICMP_TSTAMP))
  {
    icmp_bogus (ip, type, NULL);
    return;
  }

  switch (type)
  {
    case ICMP_ECHOREPLY:  /* check if we were waiting for it */
         STAT (icmpstats.icps_inhist[ICMP_ECHOREPLY]++);
         ping_hcache = intel (ip->source);
         ping_tcache = set_timeout (1000) - *(DWORD*)&icmp->echo.identifier;
         if (ping_tcache > 0x7FFFFFFFL)
             ping_tcache += 0x1800B0L;
         ping_number = *(DWORD*)(((BYTE*)&icmp->echo.identifier) + 4);
         return;

    case ICMP_UNREACH:
         STAT (icmpstats.icps_inhist[ICMP_UNREACH]++);
         if (code < DIM(icmp_unreach_str))
         {
           icmp_print (1, msg = icmp_unreach_str[code], ip->source);
#if !defined(USE_UDP_ONLY)
           if (orig_ip->proto == TCP_PROTO)
              _tcp_cancel (orig_ip, type, msg, 0);
           else
#endif
           if (orig_ip->proto == UDP_PROTO)
              _udp_cancel (orig_ip, type, msg, 0);
         }
         else
           STAT (icmpstats.icps_badcode++);
         return;

    case ICMP_SOURCEQUENCH:
         STAT (icmpstats.icps_inhist[ICMP_SOURCEQUENCH]++);
#if !defined(USE_UDP_ONLY)
         if (orig_ip->proto == TCP_PROTO)
         {
           icmp_print (1, _LANG("Source Quench"), ip->source);
           _tcp_cancel (orig_ip, type, NULL, 0);
         }
#endif
         return;

    case ICMP_REDIRECT:
         STAT (icmpstats.icps_inhist[ICMP_REDIRECT]++);
         if (code < 4)
         {
           DWORD new_gw = intel (icmp->ip.ipaddr);

           /* Check if new gateway is on our subnet
            */
           if ((new_gw ^ my_ip_addr) & sin_mask)
           {
             char buf[100], adr[20];
             strcpy (buf, ", GW = ");
             strcat (buf, _inet_ntoa(adr,new_gw));
             icmp_bogus (ip, type, buf);
             return;
           }
           icmp_print (1, msg = icmp_redirect_str[code], ip->source);

           switch (orig_ip->proto)
           {
#if !defined(USE_UDP_ONLY)
             case TCP_PROTO:
                  if (do_redirect.tcp)  /* do it to some socket */
                     _tcp_cancel (orig_ip, type, msg, new_gw);
                  break;
#endif
             case UDP_PROTO:
                  if (do_redirect.udp)
                     _udp_cancel (orig_ip, type, msg, new_gw);
                  break;

             case ICMP_PROTO:
                  if (do_redirect.icmp)
                  {
                    _ip_recursion = 1;
                    _arp_register (new_gw, intel(orig_ip->destination), 0);
                    _ip_recursion = 0;
                  }
                  break;

             case IGMP_PROTO:
                  if (do_redirect.igmp)
                  {
                    _ip_recursion = 1;
                    _arp_register (new_gw, intel(orig_ip->destination), 0);
                    _ip_recursion = 0;
                  }
                  break;
           }
         }
         else
           STAT (icmpstats.icps_badcode++);
         return;

    case ICMP_ECHO:
         STAT (icmpstats.icps_inhist[ICMP_ECHO]++);
         icmp_print (2, _LANG("PING requested of us"), ip->source);
         {
           /* Extract eth-address and create Echo reply packet.
            */
           struct _pkt     *pkt;
           union  icmp_pkt *newicmp;

           if (!icmp_chk_src(ip,ICMP_ECHO))
              return;

           pkt     = (struct _pkt*) _eth_formatpacket (MAC_SRC(ip), IP_TYPE);
           newicmp = &pkt->icmp;

           /* Don't let a huge reassembled ICMP-packet kill us.
            */
           len = min (len, mtu - sizeof(*ip));
           memcpy (newicmp, icmp, len);
           newicmp->echo.type = ICMP_ECHOREPLY;
           newicmp->echo.code = code;

           /* Use supplied ip values in case we ever multi-home.
            * Note that ip values are still in network order.
            */
           icmp_send (pkt, ip->destination, ip->source, len);
           icmp_print (2, _LANG("PING reply sent"), 0);
         }
         return;

    case ICMP_TIMXCEED:
         if (code >= DIM(icmp_exceed_str))
         {
           STAT (icmpstats.icps_badcode++);
           return;
         }
         STAT (icmpstats.icps_inhist[ICMP_TIMXCEED]++);

         if (code != 1)
            switch (orig_ip->proto)
            {
#if !defined(USE_UDP_ONLY)
              case TCP_PROTO:
                   icmp_print (1, icmp_exceed_str[code], ip->source);
                   _tcp_cancel (orig_ip, ICMP_TIMXCEED, NULL, 0);
                   break;
#endif
              case UDP_PROTO:
                   icmp_print (1, icmp_exceed_str[code], ip->source);
                   _udp_cancel (orig_ip, ICMP_TIMXCEED, NULL, 0);
                   break;
            }
         return;

    case ICMP_PARAMPROB:
         STAT (icmpstats.icps_inhist[ICMP_PARAMPROB]++);
         switch (orig_ip->proto)
         {
#if !defined(USE_UDP_ONLY)
           case TCP_PROTO:
                icmp_print (0, _LANG(icmp_type_str[type]), ip->source);
                _tcp_cancel (orig_ip, type, NULL, 0);
                break;
#endif
           case UDP_PROTO:
                icmp_print (0, _LANG(icmp_type_str[type]), ip->source);
                _udp_cancel (orig_ip, type, NULL, 0);
                break;
         }
         return;

    case ICMP_ROUTERADVERT:  /* todo !! */
         STAT (icmpstats.icps_inhist[ICMP_ROUTERADVERT]++);
         icmp_print (1, _LANG(icmp_type_str[type]), ip->source);
         return;

    case ICMP_ROUTERSOLICIT: /* todo !! */
         STAT (icmpstats.icps_inhist[ICMP_ROUTERSOLICIT]++);
         icmp_print (1, _LANG(icmp_type_str[type]), ip->source);
         return;

    case ICMP_TSTAMP:
         STAT (icmpstats.icps_inhist[ICMP_TSTAMP]++);
         icmp_print (1, _LANG(icmp_type_str[type]), ip->source);
         /* todo!!, send reply? */
         return;

    case ICMP_TSTAMPREPLY:
         STAT (icmpstats.icps_inhist[ICMP_TSTAMPREPLY]++);
         icmp_print (1, _LANG(icmp_type_str[type]), ip->source);
         /* todo!!, should store */
         return;

    case ICMP_IREQ:
         STAT (icmpstats.icps_inhist[ICMP_IREQ]++);
         icmp_print (1, _LANG(icmp_type_str[type]), ip->source);
         /* todo!!, send reply */
         return;

    case ICMP_IREQREPLY:
         STAT (icmpstats.icps_inhist[ICMP_IREQREPLY]++);
         icmp_print (1, _LANG(icmp_type_str[type]), ip->source);
         /* todo!!, send reply upwards */
         return;

    case ICMP_MASKREQ:
         STAT (icmpstats.icps_inhist[ICMP_MASKREQ]++);
         break;

    case ICMP_MASKREPLY:
         STAT (icmpstats.icps_inhist[ICMP_MASKREPLY]++);
         icmp_print (0, _LANG(icmp_type_str[type]), ip->source);
         if ((icmp->mask.identifier == addr_mask_id)    &&
             (icmp->mask.sequence   == addr_mask_seq-1) &&
             sin_mask != intel(icmp->mask.mask))
            outsnl ("Conflicting net-mask from \"ICMP Addr Mask Reply\"\7");
         addr_mask_id = 0;
         return;
  }
}

/*
 *  Determine which protocols we shall act upon when
 *  ICMP redirect is received
 */
void icmp_redirect (const char *value)
{
  char *val = strdup (value);

  if (val)
  {
    strupr (val);
    do_redirect.icmp = (strstr(val,"ICMP") != NULL);
    do_redirect.igmp = (strstr(val,"IGMP") != NULL);
    do_redirect.udp  = (strstr(val,"UDP")  != NULL);
    do_redirect.tcp  = (strstr(val,"TCP")  != NULL);
    free (val);
  }
}

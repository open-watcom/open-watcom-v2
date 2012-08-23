/*
 *  A simple loopback device.
 *  Handles ICMP Echo-request and UDP sink/discard packets.
 *
 *  Add protocol handlers to `loopback_handler' pointer as required.
 *
 *  The modified (src/dst IP swapped) ip packet is enqueued to the IP
 *  receive queue after loopback_device() returns.
 *
 *  The return value is:
 *    -1  if IP-packet should not be enqueued to IP input queue
 *    >0  length of (modified) IP-packet
 *
 *  Gisle Vanem  10 Dec. 1997
 */

#include <stdio.h>
#include <stdlib.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "chksum.h"
#include "ip_out.h"
#include "pcstat.h"
#include "pcicmp.h"
#include "loopback.h"

#if defined(USE_LOOPBACK)

#if 0

  A sketch of calls involved in sending/receiving an IP-packet


  Sending an IP-packet            |             Receiving an IP-packet
  --------------------------------|-----------------------------------
      |                           |                     |
      |                           |               tcp_tick()
      |                           |                     |
   _eth_formatpacket() ->buffer X |               _eth_arrived()
      |                           |                     |
   _ip_output()                   |               poll_recv_queues()
      |                           |                     |
      |                           |                     |
   _eth_send()                    enqueue copy of       |
      |                           X to IP-queue         |
      *----->loopback_device()--*-->>>>>>-<-------poll_ip_queue()
      |                         | |   |            |    |
      |                         | |   |            |    |
   _pkt_send()    drop buffer X \ |   |          none ip_defragment()
      |                           |   |            |    |
      |                           |   |            \  _ip_handler()
      |                           |   |                 |
      |                           |   |               protocol handlers
      |                           |   |                 |
      |                           |   |               _eth_free()
      v                           |   |                 |
  packet-driver                   |   `-----------<-- pkt_free_pkt()
      |                           |                     |
  Ethernet/PPP                                          \

  From the above it evident the current design cannot easily send
  IP-fragments. _ip_output() must be rewritten to handle max. 64kB
  packets, split them in make_fragments(), format a link-layer packet
  using _eth_formatpacket() and repetively call _eth_send().

#endif


int loopback_enable = 1; 

int (*loopback_handler) (in_Header*) = NULL;

static int icmp_loopback (ICMP_PKT   *icmp, unsigned icmp_len);
static int udp_loopback  (udp_Header *udp,  unsigned udp_len);

/*
 * We have been called with an ip-packet located on stack in
 * _eth_send_loopback(). Hence, it's safe to call _eth_send() again
 * here. MAC-header is in front of 'ip'.
 */
int loopback_device (in_Header *ip)
{
  int   ip_hlen, ip_len;
  DWORD ip_dst;
  DWORD ip_ofs;
  WORD  ip_flg;

  if (!loopback_enable || !_chk_ip_header(ip)) /* silently discard */
     return (-1);

  ip_hlen = in_GetHdrLen (ip);    /* length of IP-header (w/options) */
  ip_len  = intel16 (ip->length); /* total length of IP-packet */
  ip_dst  = ip->destination;

  ip->destination = ip->source;   /* swap source and destination */
  ip->source      = ip_dst;
  ip->checksum    = 0;
  ip->checksum    = ~checksum (ip, ip_hlen);   /* redo check-sum */

  ip_ofs = intel16 (ip->frag_ofs);
  ip_flg = ip_ofs & ~IP_OFFMASK;
  ip_ofs = (ip_ofs & IP_OFFMASK) << 3; /* 0 <= ip_ofs <= 65536-8 */
   
  if (ip_ofs || (ip_flg & IP_MF)) /* fragment; let _ip_fragment() */
     return (ip_len);             /* handle it on next poll */

  if (ip->proto == ICMP_PROTO)
  {
    ICMP_PKT *icmp = (ICMP_PKT*) ((BYTE*)ip + ip_hlen);
    int       len  = icmp_loopback (icmp, ip_len - ip_hlen);

    if (len > 0)
       return (ip_hlen+len);
  }
  else if (ip->proto == UDP_PROTO)
  {
    udp_Header *udp = (udp_Header*) ((BYTE*)ip + ip_hlen);
    int         len = udp_loopback (udp, ip_len-ip_hlen);

    if (len > 0)
       return (ip_hlen+len);
  }

  if (loopback_handler)
     ip_len = (*loopback_handler) (ip);
  return (ip_len);
}

static int icmp_loopback (ICMP_PKT *icmp, unsigned icmp_len)
{
  if (icmp_len >= sizeof(icmp->echo)    &&
      checksum(icmp,icmp_len) == 0xFFFF &&
      icmp->echo.type == ICMP_ECHO)
  {
    static WORD echo_seq_num = 0;

    icmp->echo.type     = ICMP_ECHOREPLY;
    icmp->echo.sequence = echo_seq_num++;
    icmp->echo.checksum = 0;
    icmp->echo.checksum = ~checksum (icmp, icmp_len);
    STAT (icmpstats.icps_reflect++);
    STAT (icmpstats.icps_outhist[ICMP_ECHOREPLY]++);
    return (icmp_len);
  }
  /* !!to-do: fall-through to another ICMP loopback handler */

  return (0);
}

static int udp_loopback (udp_Header *udp, unsigned udp_len)
{
  if (intel16(udp->dstPort) == IPPORT_ECHO)
  {
    /* !!to-do */
  }
  else if (intel16(udp->dstPort) == IPPORT_DISCARD)
  {
    /* !!to-do */
  }
  ARGSUSED (udp_len);
  return (0);
}

/*
 *  An example loopback handler for RPC Portmapper (udp port 111)
 *  (Maybe an contradiction in terms to do RPC locally :-)
 *
 *  int (*old_loopback) (in_Header*) = NULL;
 *
 *  static int pmap_loopback (in_Header *ip)
 *  {
 *    WORD        hlen = in_GetHdrLen (ip);
 *    udp_Header *udp  = (udp_Header*) ((BYTE*)ip + hlen);
 *
 *    if (ip->proto == UDP_PROTO && intel16(udp->dstPort) == 111)
 *       return do_portmapper (udp); (return length of IP reply packet)
 *
 *    if (old_loopback)
 *       return (*old_loopback) (ip);
 *    return (length);
 *  }
 *
 *  void init_pmap_loopback (void)
 *  {
 *    old_loopback = loopback_handler;
 *    loopback_handler = pmap_loopback;
 *  }
 */

#endif /* USE_LOOPBACK */

/*
 * Compute internet checksum on data buffer.
 * 1's complement (~) is done by caller.
 */

#include <stdio.h>
#include <stdlib.h>

#include "wattcp.h"
#include "pcicmp.h"
#include "pcconfig.h"
#include "strings.h"
#include "chksum.h"

/*
 * This checksum routine is only used by 16-bit targets.
 * 32-bit targets use inchksum_fast() in chksum0.s + chksum0.asm.
 * The function is here for speed comparision only.
 */
WORD inchksum (const void *ptr, int len)
{
  register long  sum = 0;
  register const WORD *wrd = (const WORD*) ptr;

  while (len > 1)
  {
    sum += *wrd++;
    len -= 2;
  }
  if (len > 0)
     sum += *(const BYTE*)wrd;

  /*@-shiftsigned@*/
  while (sum >> 16)
      sum = (sum & 0xFFFF) + (sum >> 16);

  return (WORD)sum;
}


#ifdef NOT_USED

#define CKSUM_CARRY(x) (x = (x >> 16) + (x & 0xffff), \
                        (~(x + (x >> 16)) & 0xffff) )

#ifndef IP_PROTO
#define IP_PROTO 0  /* dummy for IP */
#endif

/*
 * Set header checksum on outgoing packets.
 * Dug Song came up with this very cool checksuming implementation
 * eliminating the need for explicit psuedoheader use. Check it out.
 *
 * Ripped from libnet 1.0.1 
 */
int do_checksum (BYTE *buf, int protocol, int len)
{
  struct in_Header   *ip = (struct in_Header*) buf;
  struct tcp_Header  *tcp;
  struct udp_Header  *udp;
  struct IGMP_packet *igmp;
  union  icmp_pkt    *icmp;
  int    ip_hlen, sum = 0;

  ip_hlen = in_GetHdrLen (ip);

  switch (protocol)
  {
    case TCP_PROTO:
         tcp = (struct tcp_Header*) (buf + ip_hlen);
         tcp->checksum = 0;
         sum = checksum (&ip->source, 8);
         sum += intel16 (TCP_PROTO + len);
         sum += checksum (tcp, len);
         tcp->checksum = CKSUM_CARRY (sum);
         break;

    case UDP_PROTO:
         udp = (struct udp_Header*) (buf + ip_hlen);
         udp->checksum = 0;
         sum = checksum (&ip->source, 8);
         sum += intel16 (UDP_PROTO + len);
         sum += checksum (udp, len);
         udp->checksum = CKSUM_CARRY (sum);
         break;

    case ICMP_PROTO:
         icmp = (union icmp_pkt*) (buf + ip_hlen);
         icmp->unused.checksum = 0;
         sum = checksum (icmp, len);
         icmp->unused.checksum = CKSUM_CARRY (sum);
         break;

    case IGMP_PROTO:
         igmp = (struct IGMP_packet*) (buf + ip_hlen);
         igmp->checksum = 0;
         sum += checksum (igmp, len);
         igmp->checksum = CKSUM_CARRY (sum);
         break;

    case IP_PROTO:
         ip->checksum = 0;
         sum = checksum (ip, len);
         ip->checksum = CKSUM_CARRY (sum);
         break;

    default:
#if defined(USE_DEBUG)
        if (debug_on > 1)
          (*_printf) ("do_checksum: unknown protocol %d\r\n", protocol);
#endif
        return (-1);
  }
  return (1);
}

#endif

#include <stdio.h>
#include <stdlib.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "chksum.h"
#include "strings.h"
#include "language.h"
#include "misc.h"
#include "ip_out.h"
#include "pcsed.h"
#include "pcarp.h"
#include "pcconfig.h"
#include "pctcp.h"
#include "pcicmp.h"
#include "pcping.h"

#include <sys/packon.h>

 /*
  * to-do!! make all "struct _pkt" into a union in wattcp.h
  */
struct ping_pkt {
       in_Header        in;
       struct icmp_echo icmp;
    /* BYTE             data[]; */
     };

#include <sys/packoff.h>


int _ping (DWORD host, DWORD countnum, const BYTE *pattern, int len)
{
  struct ping_pkt  *pkt;
  struct icmp_echo *icmp;
  struct in_Header *ip;
  eth_address       dest;

  if (!(~host & ~sin_mask))
  {
    outsnl (_LANG("Cannot ping a network!"));
    return (0);
  }
  if (!_arp_resolve(host,&dest,0))
  {
    outsnl (_LANG("Cannot resolve host's hardware address"));
    return (0);
  }

  if (debug_on)
  {
    outs (_LANG("\n\rDEBUG: destination hardware :"));
    outhexes ((char*)&dest, sizeof(dest));
    outs ("\n");
  }

  /* make ether IP head
   */
  pkt  = (struct ping_pkt*) _eth_formatpacket (&dest, IP_TYPE);
  ip   = &pkt->in;
  icmp = &pkt->icmp;

  if (pattern && len > 0)
  {
    len = min (mtu-sizeof(struct ping_pkt), len);
    memcpy (pkt+1, pattern, len);      /* copy to pkt->data[] */
    len += sizeof (*icmp);
  }
  else
    len = sizeof (*icmp);

  icmp->type       = ICMP_ECHO;
  icmp->code       = 0;
  icmp->index      = countnum;
  icmp->identifier = (WORD) set_timeout (1000);  /* "random" id */
  icmp->sequence   = 0;
  icmp->checksum   = 0;
  icmp->checksum   = ~checksum (icmp, len);

  return IP_OUTPUT (ip, 0, intel(host), ICMP_PROTO,
                    0, (BYTE)_default_tos, 0, len, NULL);
}


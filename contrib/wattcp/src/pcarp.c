/*
 * Address Resolution Protocol
 *
 *  Externals:
 *  _arp_handler (pb) - returns 1 on handled correctly, 0 on problems
 *  _arp_resolve - return 1 on success, 0 on fail
 *               - does not return hardware address if passed NULL for buffer
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "udp_nds.h"
#include "misc.h"
#include "pc_cbrk.h"
#include "pcdbug.h"
#include "pctcp.h"
#include "pcsed.h"
#include "pcconfig.h"
#include "pcqueue.h"
#include "pcpkt.h"
#include "pcarp.h"

/*
 * arp resolution cache and list of gateways.
 */
struct arp_table  arp_cache_data[MAX_ARP_DATA];
struct gate_table arp_gate_list [MAX_GATE_DATA];


WORD arp_last_gateway;
int  arp_timeout = 2;     /* 2 seconds ARP timeout    */
int  arp_alive   = 300;   /* 5 min ARP cache lifespan */

/*
 * Compare function used by quick-sort
 */
static int _arp_compare (const void *b, const void *a)
{
  const struct gate_table *_a = (const struct gate_table*)a;
  const struct gate_table *_b = (const struct gate_table*)b;

  if (_a->mask == _b->mask)
     return (_a->metric - _b->metric);
  return (_a->mask < _b->mask ? -1 : +1);
}


/*
 * _arp_add_gateway - add a gateway to the routing table
 */
void _arp_add_gateway (const char *value, DWORD ip)
{
  DWORD subnet, mask;

  subnet = mask = 0;
  if (value)
  {
    char *subnetp, *maskp;
    char *gateway = strdup (value);
  
    if (!gateway)
       return;

    maskp = NULL;
    subnetp = strchr (gateway, ',');
    if (subnetp)
    {
      *subnetp++ = 0;
      maskp = strchr (subnetp, ',');
      if (maskp)
      {
        *maskp++ = 0;
        mask   = aton (maskp);
        subnet = aton (subnetp);
      }
      else
      {
        subnet = aton (subnetp);
        switch (subnet >> 30)
        {
          case 0:
          case 1: mask = 0xFF000000L;
                  break;
          case 2: mask = 0xFFFFFE00L;   /* minimal class B */
                  break;
          case 3: mask = 0xFFFFFF00L;
                  break;
        }
      }
    }
    ip = aton (gateway);
    free (gateway);
  }

  if (arp_last_gateway < DIM(arp_gate_list)-1)
  {
    /* Sort the gateway list on accending netmask and metric
     */
#if 1     /* new enhancement */
    struct gate_table *new_gw = arp_gate_list + arp_last_gateway;

    new_gw->gate_ip = ip;   /* append to list */
    new_gw->subnet  = subnet;
    new_gw->mask    = mask;
    new_gw->metric  = 0;
    qsort ((void*)&arp_gate_list, arp_last_gateway+1,
           sizeof(*new_gw), _arp_compare);
#else
    int i;

    for (i = 0; i < arp_last_gateway; i++)
    {
      if (arp_gate_list[i].mask < mask)
      {
        movmem (&arp_gate_list[1], &arp_gate_list[i+1],
                (arp_last_gateway-i) * sizeof(struct gate_table));
        break;
      }
    }
    arp_gate_list[i].gate_ip = ip;
    arp_gate_list[i].mask    = mask;
    arp_gate_list[i].subnet  = subnet;
#endif
    arp_last_gateway++;
  }
}


static struct arp_table * _arp_new_entry (void)
{
  static WORD arp_index = 0;        /* static rotates round-robin */
  int    i;

  for (i = 0; i < DIM(arp_cache_data); i++)
  {
    struct arp_table *arp = &arp_cache_data[i];

    /* pick an empty or non-fixed expired cache slot.
     */
    if (!arp->ip ||
        (arp->flags != ARP_FLAG_FIXED &&
         chk_timeout(arp->expiry+MAX_ARP_GRACE)))
       return (arp);
  }
  /* update the index for next slot.
   * !!Fix-me: this may overwrite a fixed entry.
   */
  arp_index++;
  arp_index %= DIM(arp_cache_data);
  return (&arp_cache_data [arp_index]);
}


static int _arp_send (const arp_Header *arp, unsigned line)
{
#if defined(USE_DEBUG)
  if (_dbugxmit)
    (*_dbugxmit) (NULL, arp, __FILE__, line);
#else
  ARGSUSED (line);
  ARGSUSED (arp);
#endif
  return _eth_send (sizeof(*arp));
}


/*
 *  _arp_request - send broadcast ARP request
 */
static int _arp_request (DWORD ip)
{
  arp_Header *arp = (arp_Header*) _eth_formatpacket (&_eth_brdcast[0],
                                                     ARP_TYPE);
  arp->hwType       = intel16 (_eth_get_hwtype(NULL,NULL));
  arp->protType     = IP_TYPE;

  arp->hwAddrLen    = sizeof (eth_address);
  arp->protoAddrLen = sizeof (ip);
  arp->opcode       = ARP_REQUEST;

  memcpy (arp->srcEthAddr, &_eth_addr, sizeof(arp->srcEthAddr));
  arp->srcIPAddr = intel (my_ip_addr);

  memset (arp->dstEthAddr, 0, sizeof(arp->dstEthAddr));
  arp->dstIPAddr = intel (ip);

  return _arp_send (arp, __LINE__);
}


struct arp_table *_arp_search (DWORD ip, int create)
{
  int i;

  for (i = 0; i < DIM(arp_cache_data); i++)
  {
    struct arp_table *arp = &arp_cache_data[i];

    if (ip == arp->ip)
       return (arp);

    if (arp->flags != ARP_FLAG_FIXED &&
        (!arp->expiry || chk_timeout(arp->expiry+MAX_ARP_GRACE)))
       return (arp);  /* !! was !chk_timeout() */
  }

  if (create)
     return _arp_new_entry();
  return (NULL);
}

/*
 * Used to resolve a MAC address when ICMP "redirect for host"
 * is received. Pretend the new MAC address is for the old IP address.
 */
void _arp_register (DWORD use_this, DWORD instead_of, int nowait)
{
  struct arp_table *arp = _arp_search (instead_of, 0);

  if (!arp)
     arp = _arp_search (use_this, 1);  /* not found, create a new one */

  arp->flags = 0;
  if (_arp_resolve(use_this,&arp->hardware,nowait))
  {
    arp->expiry = set_timeout (1000 * arp_alive);
    arp->ip     = instead_of;
  }
}

/*
 * Add given IP/Ether address to ARP-cache.
 * 'ip' is on host order.
 */
void _arp_add_cache (DWORD ip, eth_address *eth, BOOL expire)
{
  struct arp_table *arp = _arp_search (ip, 0);

  if (!arp)
     arp = _arp_search (ip, 1);  /* not found, create a new one */

  memcpy (&arp->hardware, eth, sizeof(*eth));
  arp->ip = ip;
  if (expire)
  {
    arp->expiry = set_timeout (1000 * arp_alive);
    arp->flags  = ARP_FLAG_FOUND;
  }
  else
  {
    arp->expiry = 0;
    arp->flags  = ARP_FLAG_FIXED;
  }
}

/*
 *  _arp_reply - send unicast/broadcast ARP reply
 */
int _arp_reply (const eth_address *mac_dst, DWORD src_ip, DWORD dst_ip)
{
  arp_Header *arp;

  if (!mac_dst)
     mac_dst = (const eth_address*) &_eth_brdcast;

  arp = (arp_Header*) _eth_formatpacket (mac_dst, ARP_TYPE);
  arp->hwType       = intel16 (_eth_get_hwtype(NULL,NULL));
  arp->protType     = IP_TYPE;
  arp->hwAddrLen    = sizeof (mac_address);
  arp->protoAddrLen = sizeof (dst_ip);
  arp->opcode       = ARP_REPLY;
  arp->dstIPAddr    = src_ip;
  arp->srcIPAddr    = dst_ip;

  memcpy (arp->srcEthAddr, &_eth_addr,sizeof(mac_address));
  memcpy (arp->dstEthAddr, mac_dst,   sizeof(mac_address));
  return _arp_send (arp, __LINE__);
}

/*
 * _arp_handler - handle incomming ARP packets
 */
int _arp_handler (const arp_Header *ah)
{
  struct arp_table *arp;
  DWORD  ip;
  WORD   hw_needed = intel16 (_eth_get_hwtype(NULL,NULL));

  DEBUG_RX (NULL, ah);

  if (ah->hwType   != hw_needed ||   /* wrong hardware type, */
      ah->protType != IP_TYPE)       /* or not IP-protocol   */
     return (0);

#if 0
  if (ah->hwAddrLen    != sizeof(mac_address) ||
      ah->protoAddrLen != sizeof(ip))
     return (0);
#endif

  /* continuously accept data - but only for people we talk to
   */
  ip  = intel (ah->srcIPAddr);
  arp = _arp_search (ip, 0);

  if (arp && arp->ip == ip)
  {
    arp->expiry = set_timeout (1000 * arp_alive);
    memcpy (arp->hardware, ah->srcEthAddr, sizeof(eth_address));
    arp->flags = ARP_FLAG_FOUND;
  }
#if 0   /* !!to-do */
  else
  {
    /* collect into ARP cache */
  }
#endif

  /* does someone else want our Ethernet address?
   */
  if (ah->opcode == ARP_REQUEST)
  {
    ip = intel (ah->dstIPAddr);
    if (is_local_addr(ip) && !is_multicast(ip) && (ip >> 24) != 127)
      _arp_reply (&ah->srcEthAddr, ah->srcIPAddr, ah->dstIPAddr);
  }
  return (1);
}


/*
 * _arp_resolve - resolve IP address to hardware address
 *                NB! this function may be called recursively
 */
int _arp_resolve (DWORD ip, eth_address *ethap, int nowait)
{
  static struct arp_table *arp;   /* must be static !! */
  int    i, oldhndlcbrk;
  DWORD  timeout;

  if (_pktserial)  /* Serial-drivers does not use MAC addresses */
  {
    if (ethap)
       memset (ethap, 0, sizeof(*ethap));
    return (1);
  }

  if (is_local_addr(ip)) /* !!fix-me: messes up arp_check_own_ip() */
  {
    if (ethap)
       memcpy (ethap, _eth_addr, sizeof(*ethap));
    return (1);
  }

  /* Look in ARP cache first.
   */
  arp = _arp_search (ip, 0);
  if (arp && arp->flags >= ARP_FLAG_FOUND)  /* has been resolved */
  {
    if (ethap)
       memcpy (ethap, arp->hardware, sizeof(*ethap));
    return (1);
  }

  /* make a new one if necessary
   */
  if (!arp)
     arp = _arp_search (ip, 1);

  /* we must look elsewhere - but is it on our subnet???
   */
  if ((ip ^ my_ip_addr) & sin_mask)
  {
    /* not of this network
     */
    for (i = 0; i < arp_last_gateway; i++)
    {
      struct gate_table *gw = arp_gate_list + i;

      /* is the gateway on our subnet ?
       * or if mask is FF...FF, we assume any gateway must succeed
       * because we are on `no' network
       */
      if ((((gw->gate_ip ^ my_ip_addr) & sin_mask) == 0) ||
          sin_mask == 0xFFFFFFFFL)
      {
        /* compare the various subnet bits.
         */
        if (((gw->mask & ip) == gw->subnet) &&
            _arp_resolve(gw->gate_ip,ethap,nowait))
          return (1);
      }
    }
    if (i == 0)
       outs (_LANG("No gateways defined. "));
    return (0);
  }

  if (!ip)       /* return if no host, or no gateway */
     return (0);

  /* is on our subnet, we must resolve
   */
  timeout     = set_timeout (1000 * arp_timeout);
  oldhndlcbrk = wathndlcbrk;
  wathndlcbrk = 1;
  watcbroke   = 0;

  while (!chk_timeout(timeout))
  {
    DWORD resend;

    if (!_arp_request (arp->ip = ip))  /* do the request */
       goto fail;

    resend = set_timeout (250);

    while (!chk_timeout(resend))
    {
      kbhit();
      if (watcbroke)
         goto fail;

      tcp_tick (NULL);
      if (arp->flags >= ARP_FLAG_FOUND)
      {
        if (ethap)
           memcpy (ethap, arp->hardware, sizeof(*ethap));
        arp->expiry = set_timeout (1000 * arp_alive);
        watcbroke   = 0;
        wathndlcbrk = oldhndlcbrk;
        return (1);
      }
    }
    if (nowait)
       break;
  }
fail:
  watcbroke   = 0;
  wathndlcbrk = oldhndlcbrk;
  return (0);
}


#if defined(USE_DHCP)  /* only needed in pcdhcp.c */
/*
 *  Do an ARP resolve on our own IP address to check if someone
 *  else is replying. Return non-zero if someone replied.
 */
int _arp_check_own_ip (void)
{
  DWORD       save = my_ip_addr;
  eth_address eth  = { 0 };

  my_ip_addr = 0;
  _arp_resolve (save, &eth, 0);
  my_ip_addr = save;

  if (*(DWORD*)eth)
     return (1);
  return (0);
}
#endif


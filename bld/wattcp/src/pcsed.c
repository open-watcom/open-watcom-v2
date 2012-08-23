/*
 * Ethernet Driver Routines
 *
 *  The TCP code uses Ethernet constants for protocol numbers and 48 bits
 *  for address.  Also, 0xFFFFFFFFFFFF is assumed to be a broadcast.
 *
 *  If you need to write a new driver, implement it at this level and use
 *  the above mentioned constants as this program's constants, not device
 *  dependant constants.
 *
 *  The packet driver code lies below this module.
 *
 *  _eth_addr    - MAC address of this host.
 *  _eth_brdcast - MAC broadcast address.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

#ifdef __HIGHC__
#include <init.h>  /* _mwenv, PL_ENV */
#endif

#include "copyrigh.h"
#include "wattcp.h"
#include "wdpmi.h"
#include "strings.h"
#include "language.h"
#include "sock_ini.h"
#include "loopback.h"
#include "misc.h"
#include "fragment.h"
#include "pcmulti.h"
#include "pcqueue.h"
#include "pcconfig.h"
#include "pcstat.h"
#include "pcdbug.h"
#include "pctcp.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pppoe.h"

mac_address _eth_addr;                /* Local link-layer source address  */
mac_address _eth_brdcast;             /* Link-layer broadcast address     */
mac_address _eth_loop_addr;           /* Link-layer loopback address      */
BOOL        _eth_is_init  = 0;        /* we are initialised               */
BOOL        _ip_recursion = 0;        /* avoid recursion in arp_resolve() */

/*
 * Pointer to functions that when set bypasses the normal poll/send
 * functions _eth_arrived() and _eth_send()
 */
void *(*_eth_recv_hook) (WORD *type);
int   (*_eth_xmit_hook) (void *buf, unsigned len);

/*
 * Pointer to functions that does the filling of correct MAC-header
 * and sends the link-layer packet. We store 'proto' between calls.
 */
static BYTE *(*mac_format)   (void *mac_buf, const void *mac_dest, WORD type);
static int   (*mac_transmit) (void *mac_buf, WORD len);

static WORD  proto;   /* protocol set in _eth_formatpacket() */
static BYTE *nw_pkt;  /* where network protocol packet starts */


/*
 *  _eth_format_packet() places the next packet to be transmitted into
 *  this link-layer output packet. We return address of higher-level
 *  protocol (IP/RARP/RARP) header.
 *
 *  Note, I only maintain a single output buffer, and it gets used quickly
 *  then released.  The benefits of non-blocking systems are immense.
 */
static union link_Packet outbuf;


BYTE *_eth_formatpacket (const void *mac_dest, WORD eth_type)
{
  nw_pkt = (*mac_format) (&outbuf, mac_dest, eth_type);
  memset (nw_pkt, 0, sizeof(in_Header));  /* clear only IP-header */
  return (nw_pkt);
}


/*
 *  _eth_send() does the actual transmission once we are complete with
 *  filling the buffer.  Do any last minute patches here, like fix the
 *  size. Send to "loopback" device if it's IP and destination matches
 *  loopback network (127.x.x.x.).
 *
 *  Return length of network-layer packet (not length of link-layer
 *  packet).
 */
int _eth_send (WORD len)
{
  void *tx_buf = &outbuf;

  update_out_stat (nw_pkt, proto);

  if (proto == IP_TYPE)
  {
    /* Sending to loopback device (only ip accepted)
     */
    if (is_local_addr(intel(((in_Header*)nw_pkt)->destination)))
    {
#if defined(USE_LOOPBACK)
      return _eth_send_loopback (outbuf);
#else
      /* packet dropped (null-device)
       */
      STAT (ipstats.ips_odropped++);
      return (len);
#endif
    }

#if defined(USE_PPPOE)
    if (pppoe_is_up())
       /* tx_buf = ppp_output (&outbuf, len) */ ;
#endif
  }

  /* Do the MAC-dependant transmit. 'len' on return is total length
   * of link-layer packet sent. 'len' is 0 on failure. The xmit-hook
   * is used by e.g. libpcap/libnet
   */
  if (_eth_xmit_hook)
       len = (*_eth_xmit_hook) (tx_buf, len + _pkt_ip_ofs);
  else len = (*mac_transmit) (tx_buf, len + _pkt_ip_ofs);

  if (len > _pkt_ip_ofs)
     return (len - _pkt_ip_ofs);

  if (debug_on)
     outs ("Tx failed.");
  return (0);
}

/*
 *  Functions for formatting MAC-headers for Ethernet, Token-Ring
 *  and FDDI. null_mac_format() is for protocols without MAC-headers.
 */
static BYTE *eth_mac_format (void *mac_buf, const void *mac_dest, WORD type)
{
  union link_Packet *buf = (union link_Packet*) mac_buf;

  memcpy (&buf->eth.head.destination, mac_dest, sizeof(mac_address));
  memcpy (&buf->eth.head.source,    &_eth_addr, sizeof(mac_address));
  buf->eth.head.type = type;
  proto = type;             /* remember protocol for _eth_send() */
  return (&buf->eth.data[0]);
}

static BYTE *tok_mac_format (void *mac_buf, const void *mac_dest, WORD type)
{
  union link_Packet *buf = (union link_Packet*) mac_buf;

  memcpy (&buf->tok.head.destination, mac_dest, sizeof(mac_address));
  memcpy (&buf->tok.head.source,    &_eth_addr, sizeof(mac_address));
  buf->tok.head.accessCtrl = TR_AC;
  buf->tok.head.frameCtrl  = TR_FC;
  buf->tok.head.DSAP       = TR_DSAP;
  buf->tok.head.SSAP       = TR_SSAP;
  buf->tok.head.ctrl       = TR_CTRL;
  buf->tok.head.org[0]     = TR_ORG;
  buf->tok.head.org[1]     = TR_ORG;
  buf->tok.head.org[2]     = TR_ORG;
  buf->tok.head.type       = type;
  proto = type;
  return (&buf->tok.data[0]);
}

static BYTE *fddi_mac_format (void *mac_buf, const void *mac_dest, WORD type)
{
  union link_Packet *buf = (union link_Packet*) mac_buf;

  memcpy (&buf->fddi.head.destination, mac_dest, sizeof(mac_address));
  memcpy (&buf->fddi.head.source,    &_eth_addr, sizeof(mac_address));
  buf->fddi.head.frameCtrl = FDDI_FC;
  buf->fddi.head.DSAP      = FDDI_DSAP;
  buf->fddi.head.SSAP      = FDDI_SSAP;
  buf->fddi.head.ctrl      = FDDI_CTRL;
  buf->fddi.head.org[0]    = FDDI_ORG;
  buf->fddi.head.org[1]    = FDDI_ORG;
  buf->fddi.head.org[2]    = FDDI_ORG;
  buf->fddi.head.type      = type;
  proto = type;
  return (&buf->fddi.data[0]);
}

static BYTE *null_mac_format (void *mac_buf, const void *mac_dest, WORD type)
{
  union link_Packet *buf = (union link_Packet*) mac_buf;

  ARGSUSED (mac_dest);
  ARGSUSED (type);
  proto = IP_TYPE;
  return (BYTE*) (&buf->ip.head);
}

/*
 *  Function called via 'mac_transmit' to actually send the data.
 */
static int eth_mac_xmit (void *buf, WORD len)
{
  if (len < ETH_MIN)
  {
    memset ((void*)((BYTE*)buf + len), 0, ETH_MIN - len);
    len = ETH_MIN;
  }
  else if (len > ETH_MAX)
    len = ETH_MAX;

  return pkt_send (buf, len);
}

static int tok_mac_xmit (void *buf, WORD len)
{
  if (len > TOK_MAX)  /* Token-Ring has no min. length */
      len = TOK_MAX;
  return pkt_send (buf, len);
}

static int fddi_mac_xmit (void *buf, WORD len)
{
  if (len < FDDI_MIN)
  {
    memset ((void*)((BYTE*)buf + len), 0, FDDI_MIN - len);
    len = FDDI_MIN;
  }
  else if (len > FDDI_MAX)
    len = FDDI_MAX;
  return pkt_send (buf, len);
}

static int null_mac_xmit (void *buf, WORD len)
{
  return pkt_send (buf, len);
}


/*
 *  Initialize the (DOS-extender API and) network driver interface.
 *  Return error-code or 0 if okay.
 */
int _eth_init (void)
{
  if (_eth_is_init)
     return (0);

#if defined(__HIGHC__)
  if (_mwenv != PL_ENV)
  {
    outsnl (_LANG("Only Pharlap DOS extender supported"));
    return (WERR_ILL_DOSX);
  }

#elif defined(__WATCOM386__)   /* Watcom386 + DOS4GW style or Pharlap */
  if (dpmi_init() < 0)
     return (WERR_ILL_DOSX);

#elif (DOSX & WDOSX)           /* 32-bit Borland/MSVC + WDOSX extender */
  if (dpmi_init() < 0)
     return (WERR_ILL_DOSX);
#endif

  if (!pkt_eth_init(&_eth_addr))
     return (WERR_NO_DRIVER);  /* error message already printed */

  switch (_pktdevclass)
  {
    case PD_ETHER:
         mac_format   = eth_mac_format;
         mac_transmit = eth_mac_xmit;
         break;
    case PD_TOKEN:
         mac_format   = tok_mac_format;
         mac_transmit = tok_mac_xmit;
         break;
    case PD_FDDI:
         mac_format   = fddi_mac_format;
         mac_transmit = fddi_mac_xmit;
         break;
    case PD_SLIP:
    case PD_PPP:
         mac_format   = null_mac_format;
         mac_transmit = null_mac_xmit;
         break;
    default:     /* already handled in pkt_drvr_info() */
         break;
  }
  memset (&outbuf, 0, sizeof(outbuf));
  memset (&_eth_brdcast, 0xFF, sizeof(_eth_brdcast));
  _eth_loop_addr[0] = 0xCF;
  pkt_buf_wipe();
  _eth_is_init = 1;

  return (0);  /* everything okay */
}

/*
 *  _eth_release() - release the hardware driver
 */
void _eth_release (void)
{
  if (_eth_is_init)
  {
    _eth_is_init = 0;
    pkt_release();
  }
}

/*
 *  Sets a new MAC address for our interface
 */
int _eth_set_addr (mac_address *addr)
{
  if (_pktserial || pkt_set_addr(addr))
  {
    memcpy (&_eth_addr, addr, sizeof(_eth_addr));
    return (1);
  }
  return (0);
}


/*
 *  Fill in hardware address type/length for BOOTP/DHCP packets.
 *  Also used for ARP/RARP packets. Results must be converted to
 *  proper byte-order by caller.
 */
BYTE _eth_get_hwtype (BYTE *hwtype, BYTE *hwlen)
{
  if (_pktdevclass == PD_ETHER ||
      _pktdevclass == PD_FDDI)    /* according to RFC-1390 */
  {
    if (hwlen)
       *hwlen = sizeof (mac_address);
    if (hwtype)
       *hwtype = HW_TYPE_ETHER;
    return (HW_TYPE_ETHER);
  }
  if (_pktdevclass == PD_TOKEN)
  {
    if (hwlen)
       *hwlen = sizeof (mac_address);
    if (hwtype)
       *hwtype = HW_TYPE_TOKEN;
    return (HW_TYPE_TOKEN);
  }
  return (0);
}


/*
 *  _eth_free() - free an input buffer once it is no longer needed.
 *  If it's a IP fragment free the fragment buffers, but don't update
 *  the '_pkt_inf->ip_queue' queue. If 'pkt' is NULL, restart the queues.
 */
void _eth_free (const void *pkt, WORD type)
{
  if (_eth_recv_hook) /* hook-function should free it's own packet */
     return;

  if (!pkt)
  {
    pkt_buf_wipe();   /* restart the queues */
    return;
  }

  if (type == IP_TYPE)
  {
#if defined(USE_FRAGMENTS)
    if (!free_fragment((const in_Header*)pkt))
#endif
       pkt_free_pkt (pkt, TRUE);
  }
  else if (type == ARP_TYPE || type == RARP_TYPE)
       pkt_free_pkt (pkt, FALSE);
  else
  {
#if defined(USE_DEBUG)
    (*_printf) ("%s: freeing illegal buffer, type %04Xh.\r\n",
                __FILE__, type);
#endif
    pkt_buf_wipe();   /* restart the queues */
  }
}


/*
 * Check a Token-Ring raw packet for RIF/RCF. Remove RCF if
 * found. '*trp' on output is corrected for dropped RCF.
 * These macros are taken from `tcpdump'.
 */
#define RCF DSAP   /* Routing Control where DSAP is when no routing */

#define TR_IS_SROUTED(th)   ((th)->source[0] & 0x80)
#define TR_IS_BROADCAST(th) (((intel16((th)->RCF) & 0xE000) >> 13) >= 4)
#define TR_RIF_LENGTH(th)   ((intel16((th)->RCF) & 0x1F00) >> 8)

#define TR_MAC_SIZE         (2+2+2*sizeof(mac_address)) /* AC,FC,src/dst */

static void fix_tok_head (tok_Header **trp)
{
  tok_Header *tr = *trp;

#if defined(USE_DEBUG)
  if (dbug_handle() > 0)
  {
    BYTE *raw = (BYTE*)tr;
    char  buf[202], *p = buf;
    int   i;

    for (i = 0; i < 50; i++)
        p += sprintf (p, "%02X ", raw[i]);
    *p++ = '\r';
    *p   = '\n';
    dbug_write_raw (buf);
  }
#endif

  if (TR_IS_SROUTED(tr))     /* Source routed */
  {
    int rlen = TR_RIF_LENGTH (tr);

    tr->source[0] &= 0x7F;   /* clear RII bit */

    /* Set our notion of link-layer broadcast
     */
    if (TR_IS_BROADCAST(tr))
       tr->destination[0] |= 1;

    /* copy MAC-header rlen bytes upwards
     */
    movmem (tr, (BYTE*)tr + rlen, TR_MAC_SIZE);
    *trp = (tok_Header*) ((BYTE*)tr + rlen);
  }
}

#ifdef NOT_YET
static void fix_llc_head (void **mac)
{
}
#endif

/*
 *  Poll the IP-queue. If fragmented, give to fragment handler.
 */
static union link_Packet *poll_ip_queue (WORD *type)
{
  struct pkt_ringbuf *q = &_pkt_inf->ip_queue;

  while (pktq_queued(q))
  {
    WORD         ip_flg;
    DWORD        ip_ofs;
    in_Header   *ip;
    link_Packet *pkt = (link_Packet*) pktq_out_buf (q);

    if (_pktserial)
    {
      ip = &pkt->ip.head;       /* PD_SLIP / PD_PPP */
      *type = IP_TYPE;
    }

    else if (_pktdevclass == PD_TOKEN)
    {
      tok_Header *tr = &pkt->tok.head;

      fix_tok_head (&tr);
      *type = tr->type;

      /* A non-IP packet in the IP-queue?
       * I guess this could happen with a buggy driver.
       */
      if (*type != IP_TYPE)
         return (union link_Packet*) tr;

      ip = (in_Header*) (tr+1);
    }

    else if (_pktdevclass == PD_FDDI)
    {
      fddi_Packet *fddi = &pkt->fddi;

      *type = fddi->head.type;
      if (*type != IP_TYPE)
         return (union link_Packet*) fddi;

      ip = (in_Header*) &fddi->data[0];
    }

    else              /* must be PD_ETHER */
    {
      *type = pkt->eth.head.type;
      if (*type != IP_TYPE)
         return (union link_Packet*) pkt;

      ip = (in_Header*) &pkt->eth.data[0];
    }

    /* We have determined it's an IP-packet.
     */  
    ip_ofs = intel16 (ip->frag_ofs);
    ip_flg = (WORD) (ip_ofs & ~IP_OFFMASK);
    ip_ofs = (ip_ofs & IP_OFFMASK) << 3;  /* 0 <= ip_ofs <= 65536-8 */
   
    /* It's a non-fragmented IP-packet.
     *
     * fix-me!!: This packet might be a last fragment (ofs 0, MF=0)
     *           This would be normal for Linux.
     * to-do!!: look in 'fraglist' for a match.
     */
    if (ip_ofs == 0 && (ip_flg & IP_MF) == 0)
       return (union link_Packet*) pkt;

#if defined(USE_FRAGMENTS)
    /* IP-packet is part of a fragment chain. Match with previous
     * fragments and return reassembled IP-packet.
     */
    ip = ip_defragment (ip, ip_ofs, ip_flg);

    pktq_inc_out (q);  /* drop the queued fragment */
    if (!ip)           /* not reassembled yet, or error */
       continue;       /* loop and check for more */

    /*
     * Return the reassembled segment (icmp, udp or tcp).
     * We assume MAC-header is the same on all fragments.
     * We return a packet with the MAC-header of the first
     * fragment received.
     */
    return (union link_Packet*) ((BYTE*)ip - _pkt_ip_ofs);

#else
    pktq_inc_out (q);  /* drop the fragment */
    continue;          /* poll for next */
#endif
  }
  return (NULL);
}


/*
 *  Poll the non-IP (ARP/RARP) queue.
 *  Serial drivers should always return NULL here.
 */
static union link_Packet *poll_arp_queue (WORD *type)
{
  struct pkt_ringbuf *q   = &_pkt_inf->arp_queue;
  union  link_Packet *ret = NULL;

  while (pktq_queued(q))
  {
    char *tail = pktq_out_buf (q);

    if (_pktserial)
    {
      pktq_inc_out (q);
#if defined(USE_DEBUG)
      outsnl (_LANG("Non-IP from serial-type driver !?"));
#endif
      STAT (macstats.num_err_type++);
      continue;
    }

    if (_pktdevclass == PD_TOKEN)
    {
      tok_Header *tr = (tok_Header*) tail;

      fix_tok_head (&tr);
      *type = tr->type;
      ret   = (union link_Packet*) tr;
    }
    else if (_pktdevclass == PD_FDDI)
    {
      *type = ((fddi_Header*) tail)->type;
      ret   = (union link_Packet*) tail;
    }
    else            /* must be PD_ETHER */
    {
      *type = ((eth_Header*) tail)->type;
      ret   = (union link_Packet*) tail;
    }

    if (*type != ARP_TYPE
#if defined(USE_RARP)
       && *type != RARP_TYPE)
#endif
    )
    {
      STAT (macstats.num_err_type++);
      pktq_inc_out (q);    /* drop the packet */
      continue;
    }
    return (ret);
  }
  return (NULL);
}

/*
 *  Poll IP or ARP/RARP queues.
 *  Alternate between ARP/IP on each poll. Or poll other queue if
 *  this queue is empty. Serial drivers should only get an IP-packet.
 *  Return pointer to start of MAC-header or NULL if no packets are
 *  queued.
 *
 *  Not used when e.g. libpcap has set the `_eth_recv_hook' to
 *  do it's own packet-polling.
 */
static union link_Packet *poll_recv_queues (WORD *type)
{
  static int which = 1;   /* start polling IP-queue */
  int    loop;

  ASSERT_PKT_INF (NULL);

#if defined(USE_DEBUG)
  if (!pktq_check(&_pkt_inf->ip_queue))
  {
    fprintf (stderr, "%s: IP-queue destroyed!\n", __FILE__);
    exit (-1);
  }
  if (!pktq_check(&_pkt_inf->arp_queue))
  {
    fprintf (stderr, "%s: ARP-queue destroyed!\n", __FILE__);
    exit (-1);
  }
#endif
                        
  for (loop = 0; loop < 2; loop++)
  {
    union link_Packet *pkt = which ? poll_ip_queue (type)
                                   : poll_arp_queue (type);
    which ^= 1;
    if (pkt)
       return (pkt);
  }
  return (NULL);
}  


/*
 *  _eth_arrived() - poll for new packet (IP/ARP/RARP/PPPoE protocol)
 *  arrival.  Sets protocol-type of packet received.
 *
 *  For Ethernet/TokenRing-type drivers:
 *    The return value points past the MAC-header to the IP/ARP/RARP
 *    protocol header. Also check for link-layer broadcast.
 *
 *  For PPP/SLIP-type drivers (MAC-less links):
 *    The return value points to the IP-packet itself.
 *    IP-protocol is assumed. Can never be link-layer broadcast.
 */
void *_eth_arrived (WORD *type, BOOL *brdcast)
{
  union link_Packet *pkt;
  BOOL  bcast = FALSE;
  void *ret   = NULL;

  if (_eth_recv_hook)
       pkt = (union link_Packet*) (*_eth_recv_hook) (type);
  else pkt = poll_recv_queues (type);

  if (!pkt)
     return (NULL);

  /* If ip_handler() can't be reentered, only accept
   * non-IP packets
   */ 
  if (_ip_recursion && *type == IP_TYPE)
     return (NULL);

  if (_pktserial)
  {
    bcast = FALSE;
    ret   = (void*) &pkt->ip;
  }
  else if (_pktdevclass == PD_TOKEN)
  {
    bcast = (pkt->tok.head.destination[0] & 1);
    ret   = (void*) &pkt->tok.data;
  }
  else if (_pktdevclass == PD_FDDI)
  {
    bcast = (pkt->fddi.head.destination[0] & 1);
    ret   = (void*) &pkt->fddi.data;
  }
  else
  {
    bcast = (pkt->eth.head.destination[0] & 1);
    ret   = (void*) &pkt->eth.data;
  }

  if (intel16(*type) < ETH_MAX)  /* LLC field */
  {
    STAT (macstats.num_llc_frames++);
#if 1
    _eth_free (ret, *type);
    return (NULL);
#else
    /* !!to-do: handle IEEE 802.3 encapsulation also
     */
    fix_llc_head (&ret);
#endif
  }

  update_in_stat (ret, *type);

  if (brdcast)
     *brdcast = bcast;

  return (ret);
}


#if defined(USE_DEBUG)
/*
 * Return pointer to MAC header start address of an IP packet.
 */
void *_eth_mac_hdr (const in_Header *ip)
{
  if (!_pktserial)
     return (void*) ((BYTE*)ip - _pkt_ip_ofs);

  (*_printf) ("Illegal use of `_eth_mac_hdr()' for class %d\r\n",
              _pktdevclass);
  exit (-1);
  /*@-unreachable@*/
  return (NULL);
}

/*
 * Return pointer to MAC source address of an IP packet.
 */
void *_eth_mac_src (const in_Header *ip)
{
  const union link_Packet *pkt;

  pkt = (const union link_Packet*) ((BYTE*)ip - _pkt_ip_ofs);

  if (_pktdevclass == PD_ETHER)
     return (void*) &pkt->eth.head.source;

  if (_pktdevclass == PD_TOKEN)
     return (void*) &pkt->tok.head.source;

  if (_pktdevclass == PD_FDDI)
     return (void*) &pkt->fddi.head.source;

  (*_printf) ("Illegal use of `_eth_mac_src()' for class %d\r\n",
              _pktdevclass);
  exit (-1);
  /*@-unreachable@*/
  return (NULL);
}

/*
 * Return value of protocol-type given an IP packet.
 */
WORD _eth_mac_typ (const in_Header *ip)
{
  const union link_Packet *pkt;

  pkt = (const union link_Packet*) ((BYTE*)ip - _pkt_ip_ofs);

  if (_pktdevclass == PD_ETHER)
     return (pkt->eth.head.type);

  if (_pktdevclass == PD_TOKEN)
     return (pkt->tok.head.type);

  if (_pktdevclass == PD_FDDI)
     return (pkt->fddi.head.type);

  (*_printf) ("Illegal use of `_eth_mac_typ()' for class %d\r\n",
              _pktdevclass);
  exit (-1);
  /*@-unreachable@*/
  return (0);
}
#endif /* USE_DEBUG */


#if defined(USE_LOOPBACK)
/*
 *  _eth_send_loopback() - enqueue a link-layer frame (ip only)
 *                         to the loopback device.
 *
 *  Note: this function uses call-by-value. Thus 'pkt' buffer can
 *  be modified by loopback_device() and loopback handler may send
 *  using _eth_send().
 *
 *  Note: loopback device cannot send to itself (potential recursion)
 */
int _eth_send_loopback (link_Packet pkt)
{
  struct pkt_ringbuf *q;
  const  in_Header   *ip;
  int    ip_len;

  if (!_pkt_inf)
  {
    STAT (ipstats.ips_odropped++); /* maybe this should be an input counter */
    return (0);
  }

  /* Call loopback handler with IP-packet
   */
  ip     = (in_Header*) ((BYTE*)&pkt + _pkt_ip_ofs);
  ip_len = loopback_device ((in_Header*)ip);

  q = &_pkt_inf->ip_queue;

  if (!q || ip_len > ETH_MAX_DATA) /* !!should be MTU of current driver */
  {
    STAT (ipstats.ips_odropped++); /* maybe this should be an input counter */
    return (0);
  }

  if (ip_len > 0)
  {
    union link_Packet *head;

    /* Don't let pkt_receiver() modify the queue while testing/copying.
     */
    DISABLE();

    if (pktq_in_index(q) == q->out_index)  /* queue is full, drop it */
    {
      STAT (ipstats.ips_odropped++);
      q->num_drop++;
      ENABLE();
      return (0);
    }

    head = (union link_Packet*) pktq_in_buf (q);

    /* Enqueue packet to head of input IP-queue.
     */
    if (!_pktserial)
    {
      BYTE *data = (*mac_format) (head, &_eth_addr, IP_TYPE);
      memcpy (MAC_SRC(data), &_eth_loop_addr, sizeof(mac_address));
      memcpy (data, ip, ip_len);
    }
    else
      memcpy (head, ip, ip_len);

    /* Update queue head index
     */
    q->in_index = pktq_in_index (q);
    ENABLE();
  }
  return (ip_len + _pkt_ip_ofs);
}
#endif  /* USE_LOOPBACK */


#if defined(USE_MULTICAST)   /* Jim Martin's Multicast extensions */
/*
 * _eth_join_mcast_group - joins a multicast group (at the physical layer)
 *
 * int _eth_join_mcast_group (int entry)
 * Where:
 *    entry   the entry # in the _ipmulti table
 * Returns:
 *    1       if the group was joined successfully
 *    0       if attempt failed
 */
int _eth_join_mcast_group (int entry)
{
  eth_address list [IPMULTI_SIZE];
  int         i, len, mode;
  BYTE        nextentry = 0;

  /* initialize a few things
   */
  _ipmulti[entry].active    = 0;
  _ipmulti[entry].replytime = set_timeout (0);
  _ipmulti[entry].processes = 1;

  /* Fill in the hardware address
   */
  multi_to_eth (_ipmulti[entry].ina, (BYTE*)&_ipmulti[entry].ethaddr);

  mode = _pkt_get_ip_rcv_mode();
  if (mode == -1)               /* something seems to be broken */
     return (0);

  if (mode == RM_MULTICAST2)    /* we're already in all mcast mode */
  {
    _ipmulti[entry].active = 1;
    return (1);
  }

  len = _pkt_get_multicast_list (sizeof(list), list);
  if (len == -1)
     return (0);

  /* check to see if the address is already in the list
   */
  for (i = 0; i < len/sizeof(list[0]); i++)
  {
    if (!memcmp(list[i], _ipmulti[entry].ethaddr, sizeof(list[0])))
    {
      _ipmulti[entry].active = 1;
      return (1);
    }
  }

  if (len)
     nextentry = len / sizeof(list[0]);

  memcpy (&list[nextentry], &_ipmulti[entry].ethaddr, sizeof(eth_address));
  len += sizeof (list[0]);

  if (_pkt_set_multicast_list(len,list) == -1)
  {
    if (_pkterror == NO_SPACE)     /* out of space, switch mode */
    {
      if (_pkt_set_ip_rcv_mode(RM_MULTICAST2) == -1)
         return (0);
    }
    else
      return (0);
  }

  _ipmulti[entry].active = 1;
  return (1);
}


/*
 * _eth_leave_mcast_group - leaves a multicast group (at the physical layer)
 *
 * int _eth_leave_mcast_group (int entry)
 * Where:
 *    entry   the entry # in the _ipmulti table
 * Returns:
 *    1       if the group was left successfully
 *    0       if attempt failed
 */
int _eth_leave_mcast_group (int entry)
{
  eth_address list[IPMULTI_SIZE];
  unsigned    i;
  int  len, mode;
  int  ethindex = -1;

  /* NOTE: This should be expanded to include switching back to mode 4
   * if the list of multicast addresses has shrunk sufficiently.
   *
   * First check to see if we're in mode 5. if so, mark
   * the entry as inactive and return.
   */

  mode = _pkt_get_ip_rcv_mode();
  if (mode == -1)
     return (0);

  if (mode == RM_MULTICAST2)
  {
    _ipmulti[entry].active = 0;
    return (1);
  }

  /* get the list of current multicast addresses
   */
  len = _pkt_get_multicast_list (sizeof(list), list);
  if (len < 0)
     return (0);

  /* find the apropriate entry
   */
  for (i = 0; i < len/sizeof(list[0]); i++)
      if (!memcmp(list[i],_ipmulti[entry].ethaddr,sizeof(list[0])))
         ethindex = i;

  /* if it's not in the list, just set the entry inactive and return
   */
  if (ethindex == -1)
  {
    _ipmulti[entry].active = 0;
    return (1);
  }

  /* ahh, but it _is_ in the list. So shorten the list
   * and send it back to the PD
   */
  if (ethindex+1 < (int)(len/sizeof(list[0])))
     movmem (&list[ethindex+1], &list[ethindex],
             len-(ethindex+1)*sizeof(list[0]));

  len -= sizeof (list[0]);

  if (_pkt_set_multicast_list(len,&list[0]) == -1)
  {
    if (_pkterror == NO_SPACE)     /* out of space, switch mode */
    {
      if (_pkt_set_ip_rcv_mode(RM_MULTICAST2) == -1)
         return (0);
    }
    else
      return (0);
  }

  _ipmulti[entry].active = 0;
  return (1);
}
#endif /* USE_MULTICAST */

/*
 * Turn off stack-checking because eth_release() might be called from
 * exception handler.
 */
#if defined(__HIGHC__) || defined(__WATCOMC__)
#pragma Off(check_stack)
#endif

#if defined(__BORLANDC__)
#pragma option -N-
#endif



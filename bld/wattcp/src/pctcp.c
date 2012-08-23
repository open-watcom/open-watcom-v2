
/*
 *  PCTCP - the true worker of Waterloo TCP
 *        - contains all opens, closes, major read/write routines and
 *          basic IP handler for incomming packets
 *        - NOTE: much of the TCP/UDP/IP layering is done at the data
 *          structure level, not in separate routines or tasks
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <dos.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "wattcpd.h"
#include "chksum.h"
#include "strings.h"
#include "language.h"
#include "udp_dom.h"
#include "bsdname.h"
#include "pcstat.h"
#include "pcconfig.h"
#include "pcqueue.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pcicmp.h"
#include "pcigmp.h"
#include "pcmulti.h"
#include "pcdbug.h"
#include "pcdhcp.h"
#include "pcbsd.h"
#include "pcarp.h"
#include "ip_out.h"
#include "misc.h"
#include "fragment.h"
#include "pppoe.h"
#include "tcp_fsm.h"
#include "pctcp.h"

#if defined(USE_BSD_FUNC)
#include "socket.h"
#endif

#ifndef __inline  /* normally in <sys/cdefs.h> */
#define __inline   
#endif

/*
 * These are hooks to prevent the BSD-socket API being linked in
 * by default. These function pointers are only set from the BSD
 * functions when needed; `_raw_ip_hook' is set to filter SOCK_RAW
 * packets, `_tcp_syn_hook' is set to filter incoming SYN packets
 * for SOCK_STREAM packets used in `accept()'. And '_tcp_find_hook'
 * is set to `sock_find_tcp()' when allocating SOCK_STREAM sockets.
 */
int   (*_raw_ip_hook)  (const in_Header *)  = NULL;
int   (*_tcp_syn_hook) (tcp_Socket **)      = NULL;
void *(*_tcp_find_hook) (const tcp_Socket*) = NULL;

char   hostname[MAX_HOSTLEN+1] = "random-pc";

int    mss          = ETH_MAX_DATA - sizeof(tcp_Header) - sizeof(in_Header);
int    mtu          = ETH_MAX_DATA;
int    mtu_discover = 0;   /* to-do */

int    block_tcp    = 0;   /* when application handles tcp itself  */
int    block_udp    = 0;   /*                          udp itself  */
int    block_icmp   = 0;   /*                          icmp itself */
int    block_ip     = 0;   /*                          ip itself   */

DWORD  my_ip_addr   = 0L;          /* our IP address */
DWORD  sin_mask     = 0xFFFFFF00L; /* our net-mask, 255.255.255.0 */

DebugProc _dbugxmit = NULL;
DebugProc _dbugrecv = NULL;

udp_Socket *_udp_allsocs = NULL;   /* list of udp-sockets */


/*
 * Prototypes and local data
 */

#if !defined(USE_UDP_ONLY)   /* not used for UDP/IP */

  /* TCP timer values
   */
  int tcp_OPEN_TO     = DEF_OPEN_TO;
  int tcp_CLOSE_TO    = DEF_CLOSE_TO;
  int tcp_RTO_ADD     = DEF_RTO_ADD;
  int tcp_RTO_BASE    = DEF_RTO_BASE;
  int tcp_RST_TIME    = DEF_RST_TIME;
  int tcp_RETRAN_TIME = DEF_RETRAN_TIME;

  /* TCP option config flags.
   */
  int tcp_opt_timstmp = 0;
  int tcp_opt_wscale  = 0;
  int tcp_opt_sackok  = 0;

  /* Misc TCP values
   */
  int tcp_nagle     = 1;
  int tcp_keepalive = 30;

  tcp_Socket *_tcp_allsocs  = NULL; /* list of tcp-sockets */

  static tcp_Socket *tcp_findseq (const in_Header *ip, const tcp_Header *tcp);

  static void tcp_sockreset(tcp_Socket *s, int proxy);
  static void tcp_rtt_wind (tcp_Socket *s);
  static void tcp_upd_wind (tcp_Socket *s, unsigned line);
  static int  tcp_chksum   (const in_Header *ip, const tcp_Header *tcp, int len);
  
  static void tcp_rtt_add  (tcp_Socket *s, UINT rto);
  static void tcp_rtt_clr  (tcp_Socket *s);
  static UINT tcp_rtt_get  (tcp_Socket *s);
#endif

static void udp_close   (udp_Socket *s);
static void (*system_yield)(void) = NULL;


/*
 * Passive open: listen for a connection on a particular port
 */
int udp_listen (udp_Socket *s, WORD lport, DWORD ina, WORD port, ProtoHandler handler)
{
  udp_close (s);
  watt_largecheck (s, sizeof(*s), __FILE__, __LINE__);
  memset (s, 0, sizeof(*s));

  s->rdata        = &s->rddata[0];
  s->maxrdatalen  = udp_MaxBufSize;
  s->ip_type      = UDP_PROTO;
  s->myport       = findfreeport (lport, 0); /* get a nonzero port val */
  s->hisport      = port;
  s->hisaddr      = ina;
  s->ttl          = _default_ttl;
  s->protoHandler = handler;
  s->usr_yield    = system_yield;
  s->safetysig    = SAFETYUDP;               /* insert into chain */
  s->next         = _udp_allsocs;
  _udp_allsocs    = s;

  return (1);
}

/*
 * Active open: open a connection on a particular port
 */
int udp_open (udp_Socket *s, WORD lport, DWORD ip, WORD port, ProtoHandler handler)
{
  BOOL bcast = (ip == (DWORD)-1) ||
               (~ip & ~sin_mask) == 0;

  udp_close (s);
  watt_largecheck (s, sizeof(*s), __FILE__, __LINE__);
  memset (s, 0, sizeof(*s));

  if (ip - my_ip_addr <= multihomes)
     return (0);

  s->rdata       = &s->rddata[0];
  s->maxrdatalen = udp_MaxBufSize;
  s->ip_type     = UDP_PROTO;
  s->myport      = findfreeport (lport, 0);
  s->myaddr      = my_ip_addr;
  s->ttl         = _default_ttl;

  if (bcast || !ip)      /* check for broadcast */
  {
    memset (s->hisethaddr, 0xFF, sizeof(eth_address));
    if (!ip)
       ip = (DWORD)-1;   /* make s->hisaddr = 255.255.255.255 */
  }

#if defined(USE_MULTICAST)
  else if (is_multicast(ip))   /* check for multicast */
  {
    multi_to_eth (ip, (BYTE*)&s->hisethaddr[0]);
    s->ttl = 1;     /* so we don't send worldwide as default */
  }
#endif
  else if (!_arp_resolve(ip,&s->hisethaddr,0))
          return (0);
 
  s->hisaddr      = ip;
  s->hisport      = port;
  s->protoHandler = handler;
  s->usr_yield    = system_yield;
  s->safetysig    = SAFETYUDP;
  s->next         = _udp_allsocs;
  _udp_allsocs    = s;
  return (1);
}

/*
 *  Since UDP is stateless, simply reclaim the local-port and
 *  unthread the socket from the list.
 */
static void udp_close (udp_Socket *ds)
{
  udp_Socket *s, *prev;

  for (s = prev = _udp_allsocs; s; prev = s, s = s->next)
  {
    if (ds != s)
       continue;

    (void) reuse_localport (s->myport);

    if (s == _udp_allsocs)
         _udp_allsocs = s->next;
    else prev->next   = s->next;
    if (s->err_msg == NULL)
        s->err_msg = _LANG("UDP Close called");
  }
}

/*
 * Set the TTL on an outgoing UDP datagram.
 */
void udp_SetTTL (udp_Socket *s, BYTE ttl)
{
  s->ttl = ttl;
}


#if !defined(USE_UDP_ONLY)

/*
 * Actively open a TCP connection to a particular destination.
 *  - 0 on error
 *
 * 'lport' is local port to associate with the connection.
 * 'rport' is remote port for same connection
 */
int tcp_open (tcp_Socket *s, WORD lport, DWORD ina, WORD rport, ProtoHandler handler)
{
  UINT rtt;

  watt_largecheck (s, sizeof(*s), __FILE__, __LINE__);
  (void) _tcp_unthread (s);       /* just in case not totally closed */
  memset (s, 0, sizeof(*s));

  if ((ina - my_ip_addr <= multihomes) || is_multicast(ina))
     return (0);

  if (!_arp_resolve(ina,&s->hisethaddr,0))
     return (0);

  s->rdata        = &s->rddata[0];
  s->maxrdatalen  = tcp_MaxBufSize;
  s->ip_type      = TCP_PROTO;
  s->max_seg      = mss;        /* to-do !!: use mss from setsockopt() */
  s->state        = tcp_StateSYNSENT;
  s->timeout      = set_timeout (tcp_LONGTIMEOUT);

  /* to-do !!: use TCP_NODELAY set in setsockopt()
   */
  s->sockmode     = tcp_nagle ? TCP_MODE_NAGLE : TCP_MODE_NONAGLE;
  s->cwindow      = 1;
  s->wwindow      = 0;                      /* slow start VJ algorithm */
  s->vj_sa        = INIT_VJSA;
  s->rto          = tcp_OPEN_TO;            /* added 14-Dec 1999, GV   */
  s->myaddr       = my_ip_addr;
  s->myport       = findfreeport (lport,1); /* get a nonzero port val  */
  s->locflags     = LF_LINGER;              /* close via TIMEWT state  */
  if (tcp_opt_timstmp)
     s->locflags |= LF_REQ_TSTMP;           /* use timestamp option */

  s->ttl          = _default_ttl;
  s->hisaddr      = ina;
  s->hisport      = rport;
  s->seqnum       = INIT_SEQ();
  s->flags        = tcp_FlagSYN;
  s->unhappy      = TRUE;
  s->protoHandler = handler;
  s->usr_yield    = system_yield;

  s->safetysig    = SAFETYTCP;              /* marker signatures */
  s->safetytcp    = SAFETYTCP;
  s->next         = _tcp_allsocs;           /* insert into chain */
  _tcp_allsocs    = s;

  (void) TCP_SEND (s);                      /* send opening SYN */
 
  /* find previous RTT replacing RTT set in tcp_send() above
   */
  if ((rtt = tcp_rtt_get(s)) > 0)
       s->rtt_time = set_timeout (rtt);
  else s->rtt_time = set_timeout (tcp_OPEN_TO);
  return (1);
}

/*
 * Passive open: listen for a connection on a particular port
 */
int tcp_listen (tcp_Socket *s, WORD lport, DWORD ina, WORD port, ProtoHandler handler, WORD timeout)
{
  watt_largecheck (s, sizeof(*s), __FILE__, __LINE__);
  (void) _tcp_unthread (s);    /* just in case not totally closed */
  memset (s, 0, sizeof(*s));

  if (is_multicast(ina))
     return (0);

  s->rdata        = &s->rddata[0];
  s->maxrdatalen  = tcp_MaxBufSize;
  s->ip_type      = TCP_PROTO;
  s->max_seg      = mss;        /* to-do !!: use mss from setsockopt() */
  s->cwindow      = 1;
  s->wwindow      = 0;               /* slow start VJ algorithm */
  s->vj_sa        = INIT_VJSA;
  s->state        = tcp_StateLISTEN;
  s->locflags     = LF_LINGER;

  s->myport       = findfreeport (lport, 0);
  s->hisport      = port;
  s->hisaddr      = ina;
  s->seqnum       = INIT_SEQ();
  s->unhappy      = FALSE;
  s->ttl          = _default_ttl;
  s->protoHandler = handler;
  s->usr_yield    = system_yield;
  s->safetysig    = SAFETYTCP;      /* marker signatures */
  s->safetytcp    = SAFETYTCP;
  s->next         = _tcp_allsocs;   /* insert into chain */
  _tcp_allsocs    = s;

  if (timeout != 0)
     s->timeout = set_timeout (1000 * timeout);
  return (1);
}

/*
 *  Send a FIN on a particular port -- only works if it is open.
 *  Must still allow receives
 */
void _tcp_close (tcp_Socket *s)
{
  if (s->ip_type != TCP_PROTO)
     return;

  if (s->state == tcp_StateESTAB ||
      s->state == tcp_StateESTCL ||
      s->state == tcp_StateSYNREC)
  {
    if (s->datalen)      /* must first flush all Tx data */
    {
      s->flags |= (tcp_FlagPUSH | tcp_FlagACK);
      if (s->state < tcp_StateESTCL)
      {
        s->state = tcp_StateESTCL;
        TCP_SENDSOON (s);
      }
    }
    else  /* really closing */
    {
      s->flags = (tcp_FlagACK | tcp_FlagFIN);
      if (s->err_msg == NULL)
          s->err_msg = _LANG("Connection closed normally");

      s->state    = tcp_StateFINWT1;
      s->timeout  = set_timeout (tcp_TIMEOUT);
      s->rtt_time = 0UL;   /* stop RTT timer */
      (void) TCP_SEND (s);
    }
    s->unhappy = TRUE;
  }
  else if (s->state == tcp_StateCLOSWT)
  {
   /* need to ACK the FIN and get on with it
    */
    s->timeout = set_timeout (tcp_LASTACK_TIME); /* Added AGW 6 Jan 2001 */
    s->state   = tcp_StateLASTACK;
    s->flags  |= tcp_FlagFIN;
    (void) TCP_SEND (s);
    s->unhappy = TRUE;
  }
  else if (s->state == tcp_StateSYNSENT)   /* unlink failed connection */
  {
    s->state = tcp_StateCLOSED;
    maybe_reuse_lport (s);
    (void) _tcp_unthread (s);
  }
}

/*
 * Abort a tcp connection
 */
void tcp_abort (tcp_Socket *s)
{
  if (s->err_msg == NULL)
      s->err_msg = _LANG("TCP Abort");

  if (s->state != tcp_StateLISTEN && s->state != tcp_StateCLOSED)
  {
    s->flags   = (tcp_FlagRST | tcp_FlagACK);
    s->unhappy = TRUE;
    if (s->state <= tcp_StateSYNREC)
    {
      s->rtt_time = 0UL; /* Stop RTT timer */
      tcp_rtt_clr (s);   /* Clear cached RTT */
    }
    (void) TCP_SEND (s);
  }
  s->unhappy = FALSE;
  s->datalen = 0;        /* discard Tx buffer, but not Rx buffer */
  s->ip_type = 0;

  maybe_reuse_lport (s);
  (void) _tcp_unthread (s);
}

/*
 * _tcp_sendsoon - schedule a transmission pretty soon.
 *  - this one has an imperfection at midnight, but it
 *    is not significant to the connection performance.
 *
 *    gv: Added - 5 May 2000: Relax retransmission period to
 *        tcp_CLOSE_TO when CLOSEWT state is entered.
 *        Relax retransmission period to tcp_OPEN_TO in
 *        SYNSENT state.
 */
int _tcp_sendsoon (tcp_Socket *s, char *file, unsigned line)
{
  DWORD timeout;

  if (s->ip_type != TCP_PROTO)
     return (0);

  if (s->state >= tcp_StateCLOSWT)
       timeout = set_timeout (tcp_CLOSE_TO);
  else timeout = set_timeout (tcp_RTO_BASE);

  if (s->rto <= tcp_RTO_BASE && s->recent == 0 &&
      cmp_timers(s->rtt_time,timeout) <= 0)
  {                         /* !! was == */
    int rc;

    s->karn_count = 0;
    rc = _tcp_send (s, file, line);
    s->recent = 1;
    return (rc);
  } 

  if ((s->unhappy || s->datalen > 0 || s->karn_count == 1) &&
      (s->rtt_time && cmp_timers(s->rtt_time,timeout) < 0))
     return (0);

  if (s->state == tcp_StateSYNSENT)  /* relaxed in SYNSENT state */
       s->rtt_time = set_timeout (tcp_OPEN_TO);
  else s->rtt_time = set_timeout (tcp_RTO_BASE + (s->rto >> 4));

  s->karn_count = 1;

  return (0);
}

/*
 * Unthread a socket from the tcp socket list, if it's there
 */
tcp_Socket *_tcp_unthread (tcp_Socket *ds)
{
  tcp_Socket *s, *prev;
  tcp_Socket *next = NULL;

  for (s = prev = _tcp_allsocs; s; prev = s, s = s->next)
  {
    if (ds != s)
       continue;

    if (s == _tcp_allsocs)
         _tcp_allsocs = s->next;
    else prev->next   = s->next;
    next = s->next;
  }

  if (ds->rdatalen == 0 || (ds->state > tcp_StateESTCL))
      ds->ip_type = 0;             /* fail further I/O */
  ds->state = tcp_StateCLOSED;     /* tcp_tick needs this */

  return (next);
}

/*
 * Returns 1 if connection is established
 */
int tcp_established (tcp_Socket *s)
{
  return (s->state >= tcp_StateESTAB);
}

/*
 *  tcp_handler - All tcp input processing is done from here.
 */
static tcp_Socket *tcp_handler (const in_Header *ip, BOOL broadcast)
{
  tcp_Header *tcp;
  tcp_Socket *s;
  int        len;
  BYTE       flags;
  DWORD      source = intel (ip->source);
  DWORD      destin = intel (ip->destination);
  WORD       dstPort, srcPort;

  if (broadcast || block_tcp ||
      !is_local_addr(destin) || is_multicast(source))
  {
    DEBUG_RX (NULL, ip);
    if (!block_tcp)
       STAT (tcpstats.tcps_drops++);
    return (NULL);
  }

  len   = in_GetHdrLen (ip);                /* len of IP header  */
  tcp   = (tcp_Header*) ((BYTE*)ip + len);  /* tcp frame pointer */
  len   = intel16 (ip->length) - len;       /* len of tcp+data   */
  flags = tcp->flags & tcp_FlagMASK;        /* get TCP flags     */

  if (!tcp_chksum(ip,tcp,len))
  {
    DEBUG_RX (NULL, ip);
    return (NULL);
  }

  dstPort = intel16 (tcp->dstPort);
  srcPort = intel16 (tcp->srcPort);

  /* demux to active sockets
   */
  for (s = _tcp_allsocs; s; s = s->next)
  {
    if (s->safetysig != SAFETYTCP || s->safetytcp != SAFETYTCP)
    {
      outsnl (_LANG("tcp-socket error in tcp_handler()"));
      DEBUG_RX (s, ip);
      return (NULL);
    }

    if (s->hisport            &&   /* not a listening socket */
        destin  == s->myaddr  &&   /* addressed to my IP */
        source  == s->hisaddr &&   /* and from my peer address */
        dstPort == s->myport  &&   /* addressed to my local port */
        srcPort == s->hisport)     /* and from correct remote port */
      break;
  }

  if (!s && (flags & tcp_FlagSYN))
  {
    /* demux to passive (listening) sockets, must be a new session
     */
    for (s = _tcp_allsocs; s; s = s->next)
        if (s->hisport == 0 &&        /* =0, listening socket */
            dstPort    == s->myport)  /* addressed to my local port */
        {
          s->hisport = srcPort;  /* remember his IP-address */
          s->hisaddr = source;   /*   and src-port */
          s->myaddr  = destin;   /* socket is now active */
          break;
        }
  }

  DEBUG_RX (s, ip);

  if (!s)
  {
    if (!(flags & tcp_FlagRST))              /* don't answer RST */
       TCP_RESET (NULL, ip, tcp);

    else if ((flags & tcp_FlagACK) &&           /* got ACK,RST   */
             (s = tcp_findseq(ip,tcp)) != NULL) /* ACK = SEQ + 1 */
            tcp_sockreset (s, 1);  /* e.g. a firewall is sending */
    STAT (tcpstats.tcps_drops++);  /* RST for server on inside   */
    return (NULL);
  }

  /* Restart idle-timer
   */
  if (sock_inactive && !(s->locflags & LF_RCVTIMEO))
     s->inactive_to = set_timeout (1000 * sock_inactive);

  if (flags & tcp_FlagRST)         /* got reset code */
  {
    tcp_sockreset (s, 0);
    return (NULL);
  }

  tcp_rtt_wind (s);       /* update retrans timer, windows etc. */

  if (_tcp_fsm(&s,ip) &&  /* do input tcp state-machine */
      s->unhappy)         /* if "unhappy", retransmit soon */
     TCP_SENDSOON (s);

  return (s);
}
#endif  /* !USE_UDP_ONLY */


/*
 * Handler for incoming udp packets.
 */
static udp_Socket *udp_handler (const in_Header *ip, BOOL broadcast)
{
  udp_Socket      *s;
  udp_Header      *udp;
  tcp_PseudoHeader ph;

  WORD  len, dstPort, srcPort;
  DWORD destin   = intel (ip->destination);
  BOOL  ip_bcast = broadcast ||        /* link-layer broadcast */
                   is_ip_brdcast(ip);  /* (directed) ip-broadcast */

#if !defined(USE_MULTICAST)

  /* dst = ip number
   *     or 255.255.255.255
   *     or sin_mask.255.255
   * This is the only really gross hack in the multicasting stuff.
   * I'll fix it as soon as I can figure out what I want to do here.
   * -JRM 8/1/93
   */
  if (!ip_bcast &&                        /* not a broadcast packet */
      destin - my_ip_addr > multihomes && /* not my address         */
      my_ip_addr)                         /* and I know my address  */
  {
    DEBUG_RX (NULL, ip);
    return (NULL);
  }
#endif

  len = in_GetHdrLen (ip);
  udp = (udp_Header*) ((BYTE*)ip + len);   /* udp segment pointer */
  len = intel16 (udp->length);

  if (len < sizeof(*udp))
  {
    DEBUG_RX (NULL, ip);
    STAT (udpstats.udps_hdrops++);
    return (NULL);
  }

  srcPort = intel16 (udp->srcPort);
  dstPort = intel16 (udp->dstPort);

  /* demux to active sockets
   */
  for (s = _udp_allsocs; s; s = s->next)
  {
    if (s->safetysig != SAFETYUDP)
    {
      outsnl (_LANG("udp-socket error in udp_handler()"));
      DEBUG_RX (s, ip);
      return (NULL);
    }
    if (!ip_bcast               &&
        (s->hisport != 0)       &&
        (dstPort == s->myport)  &&
        (srcPort == s->hisport) &&
        ((destin & sin_mask) == (s->myaddr & sin_mask)) &&
        (intel(ip->source)   == s->hisaddr))
       break;
  }

  if (!s)
  {
    /* demux to passive sockets
     */
    for (s = _udp_allsocs; s; s = s->next)
    {
      if ((s->hisaddr == 0 || s->hisaddr == 0xFFFFFFFFUL) &&
          dstPort == s->myport)
      {
        if (s->hisaddr == 0)
        {
          s->hisaddr = intel (ip->source);
          s->hisport = srcPort;
          SET_PEER_MAC_ADDR (s, ip);

          /* take on value of expected destination
           * unless it is broadcast
           */
          if (!ip_bcast)
             s->myaddr = destin;
        }
        break;
      }
    }
  }

  DEBUG_RX (s, ip);

#if defined(USE_MULTICAST)
  if (!s)
  {
    /* demux to multicast sockets
     */
    for (s = _udp_allsocs; s; s = s->next)
    {
      if (s->hisport != 0         &&
          s->hisaddr == destin    &&
          dstPort    == s->myport &&
          is_multicast(destin))
        break;
    }
  }
#endif

  if (!s)
  {
    /* Demux to broadcast sockets.
     */
    for (s = _udp_allsocs; s; s = s->next)
    {
      if (s->hisaddr == (DWORD)-1 && dstPort == s->myport)
         break;
    }
  }

  if (!s)  /* no demultiplexer found anything */
  {
    if (debug_on)
       outs (_LANG("discarding..."));

    if ((destin - my_ip_addr <= multihomes) && my_ip_addr)
    {
      if (!ip_bcast &&              /* broadcast? */
          srcPort != DOM_DST_PORT)  /* from a nameserver?  */
         icmp_unreach (ip, 3);      /* send port unreachable */

      if (ip_bcast)
           STAT (udpstats.udps_noportbcast++);
      else STAT (udpstats.udps_noport++);
    }
    return (NULL);
  }

  /* these parameters are used for things other than just checksums
   */
  memset (&ph, 0, sizeof(ph));
  ph.src      = ip->source;      /* already network order */
  ph.dst      = ip->destination;
  ph.protocol = UDP_PROTO;
  ph.length   = udp->length;
  ph.checksum = checksum (udp, len);

  if (udp->checksum && (s->sockmode & UDP_MODE_NOCHK) == 0)
  {
    if (checksum(&ph,sizeof(ph)) != 0xFFFF)
    {
      if (debug_on)
         outsnl (_LANG("bad udp checksum"));
      STAT (udpstats.udps_badsum++);
      return (s);
    }
  }

  /* Process user data. 0-byte probe is legal for s->protoHandler.
   */
  {
    BYTE *data = (BYTE*)(udp+1);

    len -= sizeof(*udp);
    if (s->protoHandler)
      (*s->protoHandler) (s, data, len, &ph, udp);
    /* save first received packet rather than latest */
    else if (len > 0 && s->rdatalen == 0)
    {
      if (len > s->maxrdatalen)   /* truncate data :-( */
      {
        len = s->maxrdatalen;
        STAT (udpstats.udps_fullsock++);
      }
      /* Might overwrite previous data! But hey, this is UDP..
       */
      memcpy (s->rdata, data, len);
      s->rdatalen = len;
    }
  }
  return (s);
}


/*
 * tcp_Retransmit() - called periodically to perform retransmissions.
 *                  - if 'force == 1' do it now.
 */
void tcp_Retransmitter (int force)
{
#if defined(USE_UDP_ONLY)
  ARGSUSED (force);
#else
  tcp_Socket *s, *next;

  static DWORD timeout = 0UL;

  /* do this once per tcp_RETRAN_TIME
   */
  if (!force && timeout && !chk_timeout(timeout))
     return;

  timeout = set_timeout (tcp_RETRAN_TIME);

  for (s = _tcp_allsocs; s; s = next)
  {
    next = s->next;

    /* possible to be closed but still queued
     */
    if (s->state == tcp_StateCLOSED)
    {
      if (s->rdatalen == 0)
      {
        maybe_reuse_lport (s);
        next = _tcp_unthread (s);
      }
      continue;
    }

    if (s->datalen > 0 || s->unhappy || s->karn_count == 1)
    {
      if (chk_timeout(s->rtt_time))  /* retransmission timeout */
      {
        s->rtt_time = 0UL;           /* stop RTT timer */

#if defined(USE_DEBUG)
        if (debug_on > 1)
          (*_printf) ("regular retran TO set unacked back to 0 from %u\r\n",
                      s->unacked);
#endif
        /* strategy handles closed windows.  JD + EE
         */
        if (s->window == 0 && s->karn_count == 2)
            s->window = 1;

        if (s->karn_count == 0)
        {
          /* Simple "Slow start" algorithm:
           * Use the backed off RTO - implied, no code necessary.
           * Reduce the congestion window by 25%
           */
          unsigned cwindow = ((unsigned)(s->cwindow + 1) * 3) >> 2;

          s->cwindow = cwindow;
          if (s->cwindow == 0)
              s->cwindow = 1;

          s->wwindow = 0;       /* dup ACK counter ? */
       /* s->snd_ssthresh = s->cwindow * s->max_seg; */ /* !!to-do */

          /* if really did timeout
           */
          s->karn_count = 2;
          s->unacked    = 0;
        }
        if (s->datalen > 0)
            s->flags |= (tcp_FlagPUSH | tcp_FlagACK);

        if (s->unhappy)
           STAT (tcpstats.tcps_rexmttimeo++);

        else if (s->flags & tcp_FlagACK)
           STAT (tcpstats.tcps_delack++);

        (void) TCP_SEND (s);
      }

      /* handle inactive tcp timeouts (not sending data)
       */
      if (chk_timeout(s->datatimer))  /* EE 99.08.23 */
      {
        s->err_msg = _LANG("Connection timed out - no data sent");
        tcp_abort (s);
      }
    }  /* end of retransmission strategy */


    /* handle inactive tcp timeouts (not received anything)
     */
    if (chk_timeout(s->inactive_to))
    {
      /* this baby has timed out. Don't do this again.
       */
      s->inactive_to = 0UL;
      s->err_msg = _LANG("Connection timed out - no activity");
      sock_close ((sock_type*)s);
    }
    else if (chk_timeout(s->timeout))
    {
      if (s->state == tcp_StateTIMEWT)
      {
        s->state = tcp_StateCLOSED;
        break;
      }
      else if (s->state != tcp_StateESTAB && s->state != tcp_StateESTCL)
      {
        s->err_msg = _LANG("Timeout, aborting");
        tcp_abort (s);
        break;
      }
    }
  }
#endif /* !USE_UDP_ONLY */
}


/*
 *  ip_handler - do a simple check on IP header
 *             - Demultiplex packet to correct protocol handler
 */
int _ip_handler (in_Header *ip, BOOL broadcast)
{
  sock_type *s = NULL;

  if (block_ip || !_chk_ip_header(ip))
     return (0);

#if 0
  /* to-do: check for LSRR option and replace ip->source
   */       with actual source-address burried in option
   *        (ref. RFC1122)
#endif

#if defined(USE_BSD_FUNC)
  /*
   * Match 'ip' against all SOCK_RAW sockets before doing normal
   * protocol multiplexing below.
   *
   * Note: _raw_ip_hook is only set if we have allocated at least
   *       one SOCK_RAW socket. Don't waste time searching otherwise.
   *
   * Fix-me: a raw socket may gobble up a packet we're awaiting in
   *         e.g. resolve().
   */
  if (_raw_ip_hook && (*_raw_ip_hook)(ip))
     return (1);
#endif

  switch (ip->proto)
  {
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
         s = (sock_type*) tcp_handler (ip, broadcast);
         break;
#endif

    case UDP_PROTO:
         s = (sock_type*) udp_handler (ip, broadcast);
         break;

    case ICMP_PROTO:
         icmp_handler (ip, broadcast);
         break;

#if defined(USE_MULTICAST)
    case IGMP_PROTO:
         igmp_handler (ip, broadcast);
         break;
#endif
    default:
         if (!broadcast)
         {
           if (is_local_addr (intel(ip->destination)))
              icmp_unreach (ip, 2);  /* protocol unreachable */
           DEBUG_RX (NULL, ip);
           STAT (ipstats.ips_noproto++);
         }
         return (0);
  }

  if (s)    /* Check if peer allows IP-fragments */
  {
    if (intel16(ip->frag_ofs) & IP_DF)
         s->tcp.locflags |=  LF_NOFRAGMENT;
    else s->tcp.locflags &= ~LF_NOFRAGMENT;
  }

  STAT (ipstats.ips_delivered++);
  return (1);
}


/*
 * tcp_tick - called periodically by user application
 *          - called with socket parameter or NULL
 *          - returns 1 when our socket closes
 */
WORD tcp_tick (sock_type *s)
{
  static DWORD daemon_timer = 0UL;

#if !defined(USE_UDP_ONLY)
  /* finish off dead sockets
   */
  if (s)
  {
    if ((s->tcp.ip_type  == TCP_PROTO)       &&
        (s->tcp.state    == tcp_StateCLOSED) &&
        (s->tcp.rdatalen == 0))
    {
      (void) _tcp_unthread (&s->tcp);
      s->tcp.ip_type = 0;   /* fail further I/O */
    }
  }
#endif

  while (1)
  {
    WORD eth_type = 0;
    BOOL brdcast  = FALSE;
    void *packet  = _eth_arrived (&eth_type, &brdcast);

    if (!packet)  /* packet points to network layer protocol */
       break;

    switch (eth_type)
    {
      case IP_TYPE:
           _ip_handler ((in_Header*)packet, brdcast);
           break;

      case ARP_TYPE:
           _arp_handler ((arp_Header*)packet);
           break;

#if defined(USE_PPPOE)
      case PPPOE_DISC_TYPE:
      case PPPOE_SESS_TYPE:
           pppoe_handler ((struct pppoe_Packet*)packet);
           break;
#endif
      /* RARP is only used during boot. Not needed here */
    }
    _eth_free (packet, eth_type);
  }

#if defined(USE_MULTICAST)
  if (_multicast_on)
     check_mcast_reports();
#endif

#if !defined(USE_UDP_ONLY)
  tcp_Retransmitter (0); /* check for our outstanding packets */
#endif

  if ((daemon_timer == 0UL || chk_timeout(daemon_timer)) && wattcpd)
  {
    (*wattcpd)();        /* do our various daemons */
    daemon_timer = set_timeout (DAEMON_RUN_TIME);
  }

  return (s ? s->tcp.ip_type : 0);
}

/*
 * udp_write()
 */
static int udp_write (udp_Socket *s, const BYTE *data, int len)
{
  #include <sys/packon.h>
  struct udp_pkt {
         in_Header  in;
         udp_Header udp;
      /* BYTE       data[]; */
       } *pkt;
  #include <sys/packoff.h>

  tcp_PseudoHeader ph;
  in_Header       *ip;
  udp_Header      *udp;
  mac_address     *dst;

  /* build link-layer header
   */
  dst = (_pktserial ? NULL : &s->hisethaddr);
  pkt = (struct udp_pkt*) _eth_formatpacket (dst, IP_TYPE);
  ip  = &pkt->in;
  udp = &pkt->udp;

  /* build udp header
   */
  udp->srcPort  = intel16 (s->myport);
  udp->dstPort  = intel16 (s->hisport);
  udp->checksum = 0;
  udp->length   = intel16 (sizeof(*udp)+len);

  memcpy (pkt+1, data, len);   /* copy 'data' to 'pkt->data[]' */
  memset (&ph, 0, sizeof(ph));
  ph.src = intel (s->myaddr);
  ph.dst = intel (s->hisaddr);

  if (!(s->sockmode & UDP_MODE_NOCHK))
  {
    ph.protocol = UDP_PROTO;
    ph.length   = udp->length;
    ph.checksum = checksum (udp, sizeof(*udp)+len);
    udp->checksum = ~checksum (&ph, sizeof(ph));
  }
  if (!IP_OUTPUT(ip, ph.src, ph.dst, UDP_PROTO, s->ttl,
                 (BYTE)_default_tos, 0, sizeof(*udp)+len, s))
     return (-1);
  return (len);
}

/*
 * udp_read - read socket data to 'buf', does large buffering
 */
static int udp_read (udp_Socket *s, BYTE *buf, int maxlen)
{
  int len = s->rdatalen;

  if (maxlen < 0)
      maxlen = INT_MAX;

  if (len > 0)
  {
    if (len > maxlen)
        len = maxlen;
    if (len > 0)
    {
      if (buf)
         memcpy (buf, s->rdata, len);
      s->rdatalen -= len;
      if (s->rdatalen)
         movmem (s->rdata+len, s->rdata, s->rdatalen);
    }
  }
  return (len);
}

void _udp_cancel (const in_Header *ip, int type, const char *msg, DWORD gateway)
{
  WORD        srcPort, dstPort;
  int         len     = in_GetHdrLen (ip);
  BOOL        passive = FALSE;
  udp_Header *udp     = (udp_Header*) ((BYTE*)ip + len);
  udp_Socket *s;

  srcPort = intel16 (udp->srcPort);
  dstPort = intel16 (udp->dstPort);

  for (s = _udp_allsocs; s; s = s->next)  /* demux to active sockets */
  {
    if (s->hisport && dstPort == s->hisport && srcPort == s->myport &&
        intel(ip->destination) == s->hisaddr)
       break;
  }

  if (!s)       /* check passive sockets */
  {
    passive = TRUE;
    for (s = _udp_allsocs; s; s = s->next)
        if (s->hisport == 0 && dstPort == s->myport)
           break;
  }

  if (s)  
  {
    if (s->err_msg == NULL && msg)
        s->err_msg = msg;

    if (s->sol_callb)            /* tell the socket layer about it */
      (*s->sol_callb) ((void*)s, type);

    if (type == ICMP_REDIRECT && /* handle redirect on active sockets */
        !passive)
    {
      _ip_recursion = 1;
      _arp_resolve (gateway, &s->hisethaddr, 1);
      _ip_recursion = 0;
    }
    else if (type != ICMP_TIMXCEED)
    {
      s->rdatalen = 0;  /* will it be unthreaded ? */
      s->ip_type  = 0;
      s->err_msg  = _LANG("Port unreachable");
    }
  }
  else
  {
    /* tell the INADDR_ANY sockets about it
     */
    for (s = _udp_allsocs; s; s = s->next)
    {
      if (s->sol_callb)
        (*s->sol_callb) ((void*)s, type);
    }
  }
}

#if !defined(USE_UDP_ONLY)
void _tcp_cancel (const in_Header *ip, int type, const char *msg, DWORD gateway)
{
  tcp_Header *tcp = (tcp_Header*) ((BYTE*)ip + in_GetHdrLen (ip));
  tcp_Socket *s;

  WORD srcPort = intel16 (tcp->srcPort);
  WORD dstPort = intel16 (tcp->dstPort);

  /* demux to active sockets
   */
  for (s = _tcp_allsocs; s; s = s->next)
  {
    if (srcPort == s->myport && dstPort == s->hisport &&
        intel(ip->destination) == s->hisaddr)
    {
      switch (type)
      {
        case ICMP_TIMXCEED:
             if (s->ttl < 255)
                 s->ttl++;
             /* FALLTROUGH */

        case ICMP_UNREACH:
             if (s->stress++ > s->rigid && s->rigid < 100)  /* halt it */
             {
               s->err_msg  = msg ? msg : _LANG("ICMP closed connection");
               s->rdatalen = 0;
               s->datalen  = 0;
               s->unhappy  = FALSE;
               tcp_abort (s);
               break;
             }
             /* FALLTROUGH */

        case ICMP_SOURCEQUENCH:    
             s->cwindow = 1;       /* slow-down tx-rate */
             s->wwindow = 1;
             s->vj_sa <<= 2;
             s->vj_sd <<= 2;
             s->rto   <<= 2;
             tcp_rtt_add (s, s->rto);
             break;

        case ICMP_REDIRECT:
             /* don't bother handling redirect if we're closing
              */
             if (s->state >= tcp_StateFINWT1)
             {
               if (s->err_msg == NULL && msg)
                   s->err_msg = msg;
               break;
             }
             _ip_recursion = 1;
             _arp_resolve (gateway, &s->hisethaddr, 1);
             _ip_recursion = 0;
             break;

        case ICMP_PARAMPROB:
             tcp_abort (s);
             break;
      }
      if (s->sol_callb)
        (*s->sol_callb) ((void*)s, type);
    }
  }
}

/*
 * tcp_read - read socket data to 'buf', does large buffering
 */
static int tcp_read (tcp_Socket *s, BYTE *buf, int maxlen)
{
  int len = s->rdatalen;

  if (maxlen < 0)
      maxlen = INT_MAX;

  if (len > 0)
  {
    if (len > maxlen)
        len = maxlen;
    if (len > 0)
    {
      if (buf)
         memcpy (buf, s->rdata, len);
      s->rdatalen -= len;
      if (s->missed_seg[0] || s->rdatalen > 0)
      {
        int diff = 0;
        if (s->missed_seg[0] != 0)
        {
          long ldiff = s->missed_seg[1] - s->acknum;
          diff = abs ((int)ldiff);
        }
        movmem (s->rdata + len, s->rdata, s->rdatalen + diff);
        TCP_SENDSOON (s);     /* update the window soon */
      }
      else
        tcp_upd_wind (s, __LINE__);
    }
  }
  else if (s->state == tcp_StateCLOSWT)
          _tcp_close (s);
  return (len);
}

/*
 * Write data to a connection.
 * Returns number of bytes written, == 0 when no room in socket-buffer
 */
static int tcp_write (tcp_Socket *s, const BYTE *data, UINT len)
{
  UINT room;

  if (s->state != tcp_StateESTAB)
     return (0);

  room = tcp_MaxTxBufSize - s->datalen;
  if (len > room)
      len = room;
  if (len > 0)
  {
    int rc = 0;

    memcpy (s->data + s->datalen, data, len);
    s->datalen  += len;
    s->unhappy   = TRUE;    /* redundant because we have outstanding data */
    s->datatimer = set_timeout (1000 * sock_data_timeout); /* EE 99.08.23 */

    if (s->sockmode & TCP_LOCAL)    /* queue up data, flush on next write */
    {
      s->sockmode &= ~TCP_LOCAL;
      return (len);
    }

    if (s->sockmode & TCP_MODE_NONAGLE)
       rc = TCP_SEND (s);
    else
    {
      /* transmit if first data or reached MTU.
       * not true MTU, but better than nothing
       */
      if (s->datalen == len || s->datalen > s->max_seg/2)
           rc = TCP_SEND (s);
      else rc = TCP_SENDSOON (s);
    }
    if (rc < 0)
       return (-1);
  }        
  return (len);
}



/*
 *  Find the socket with the tripplet:
 *  DESTADDR=MYADDR,DESTPORT=MYPORT and ACKNUM=SEQNUM+1
 */
static tcp_Socket *tcp_findseq (const in_Header *ip, const tcp_Header *tcp)
{
  tcp_Socket *s;
  DWORD      dstHost = intel (ip->destination);
  DWORD      ackNum  = intel (tcp->acknum);
  WORD       dstPort = intel16 (tcp->dstPort);

  for (s = _tcp_allsocs; s; s = s->next)
  {     
    if (s->hisport != 0      &&
        dstHost == s->myaddr &&
        dstPort == s->myport &&
        ackNum  == (s->seqnum+1))
      break;
  }
  return (s);
}

static void tcp_sockreset (tcp_Socket *s, int proxy)
{
  char *str = proxy ? "Proxy reset connection"
                    : "Remote reset connection";
  if (debug_on)
     outsnl (_LANG(str));

  s->datalen = 0;  /* Flush Tx buffer */

  if (s->state != tcp_StateCLOSED && s->state != tcp_StateLASTACK)
      s->rdatalen = 0;
  s->err_msg = _LANG (str);
  s->state   = tcp_StateCLOSED;
  s->ip_type = 0;   /* 2001.1.18 - make it fail tcp_tick() */

#if defined(USE_BSD_FUNC)
  if (_tcp_find_hook)
  {
    Socket *sock = (Socket*) (*_tcp_find_hook) (s);

    if (sock)  /* do a "read-wakeup" on the SOCK_STREAM socket */
    {
      sock->so_state |= SS_CONN_REFUSED;
      if (sock->so_error == 0)
          sock->so_error = ECONNRESET;
    }
  }
#endif
  (void) _tcp_unthread (s);
}

/*
 *  tcp_chksum - Check tcp header checksum
 */
static int tcp_chksum (const in_Header *ip, const tcp_Header *tcp, int len)
{
  tcp_PseudoHeader ph;

  memset (&ph, 0, sizeof(ph));
  ph.src      = ip->source;
  ph.dst      = ip->destination;
  ph.protocol = TCP_PROTO;
  ph.length   = intel16 (len);
  ph.checksum = checksum (tcp, len);

  if (checksum(&ph,sizeof(ph)) == 0xFFFF)
     return (1);

  STAT (tcpstats.tcps_rcvbadsum++);
  if (debug_on)
     outsnl (_LANG("bad tcp checksum"));
  return (0);
}

/*
 *  tcp_rtt_wind - Update retransmission timer, VJ algorithm
 *  and tcp windows.
 */
static void tcp_rtt_wind (tcp_Socket *s)
{
  DWORD timeout;

  /* update our retransmission stuff (Karn algorithm)
   */
  if (s->karn_count == 2)    /* Wake up from slow-start */
  {
#if defined(USE_DEBUG)
    if (debug_on > 1)
       (*_printf)("finally got it safely zapped from %u to ????\r\n",
                  s->unacked);
#endif
  }
  else if (s->vj_last)  /* We expect an immediate response */
  {
    long  dT;      /* time since last (re)transmission */
    DWORD now;

    chk_timeout (0UL);          /* update date/date_ms */
    now = (long) set_timeout (0);
    dT  = (long) get_timediff (now, s->vj_last);

    if (dT >= 0)        /* !!shouldn't this be '> 0' ? */
    {
      dT -= (DWORD)(s->vj_sa >> 3);
      s->vj_sa += (int)dT;

      if (dT < 0)
          dT = -dT;

      dT -= (s->vj_sd >> 2);
      s->vj_sd += (int)dT;      /* vj_sd = RTTVAR, rtt variance */

      if (s->vj_sa > MAX_VJSA)  /* vj_sa = SRTT, smoothed rtt */
          s->vj_sa = MAX_VJSA;
      if (s->vj_sd > MAX_VJSD)
          s->vj_sd = MAX_VJSD;
    }

#if 0 /* !!to-do: use TimeStamp option values */
    if (s->ts_echo && s->ts_echo == s->ts_sent)
    {
      dT = get_timediff (now, s->ts_echo);
    }
    else
#else
    /* only recompute RTT hence RTO after success
     */
    s->rto = tcp_RTO_BASE + (((s->vj_sa >> 2) + (s->vj_sd)) >> 1);
#endif

    tcp_rtt_add (s, s->rto);


#if defined(USE_DEBUG)
    if (debug_on > 1)
       (*_printf)("rto %u  sa %u  sd %u  cwindow %u"
                  "  wwindow %u  unacked %u\r\n",
                  s->rto, s->vj_sa, s->vj_sd, s->cwindow,
                  s->wwindow, s->unacked);
#endif
  }

  s->karn_count = 0;
  if (s->wwindow != 255)
  {
    /* A. Iljasov (iljasov@oduurl.ru) suggested this pre-increment
     */
    if (++s->wwindow >= s->cwindow)
    {
      if (s->cwindow != 255)
          s->cwindow++;
      s->wwindow = 0;    /* mdurkin -- added 95.05.02 */
    }
  }


  /* Restart RTT timer or postpone retransmission
   * based on calculated RTO. Make sure date/date_ms variables
   * are updated close to midnight.
   */
  chk_timeout (0UL);
  timeout = set_timeout (s->rto + tcp_RTO_ADD);

  if (s->rtt_time == 0UL || cmp_timers(s->rtt_time,timeout) < 0)
      s->rtt_time = timeout;

  s->datatimer = 0UL; /* resetting tx-timer, EE 99.08.23 */
}

/*
 *  tcp_upd_wind - Check if receive window needs an update.
 */
static void tcp_upd_wind (tcp_Socket *s, unsigned line)
{
  UINT winfree = s->maxrdatalen - s->rdatalen;

  if (winfree < s->max_seg/2)
     _tcp_send (s, __FILE__, line);  /* update window now */
}

/*
 * TCP option routines.
 * Note: Each of these MUST add multiple of 4 byte of options.
 *
 * Insert MSS option.
 */
static __inline int tcp_opt_maxsegment (tcp_Socket *s, BYTE *opt)
{
  *opt++ = TCPOPT_MAXSEG;    /* option: MAXSEG,length,mss */
  *opt++ = 4;
  *(WORD*) opt = intel16 (s->max_seg);
  return (4);
}

/*
 * Insert TimeStamp option.
 */
static __inline int tcp_opt_timestamp (tcp_Socket *s, BYTE *opt,
                                       DWORD ts_val, DWORD ts_echo)
{
  *opt++ = TCPOPT_NOP;     /* NOP,NOP,TIMESTAMP,length,TSval,TSecho */
  *opt++ = TCPOPT_NOP;
  *opt++ = TCPOPT_TIMESTAMP;   
  *opt++ = 10;
  *(DWORD*) opt = intel (ts_val);  opt += sizeof(ts_val);
  *(DWORD*) opt = intel (ts_echo); opt += sizeof(ts_echo);
  s->ts_sent = ts_val;       /* remember ts_sent */
  return (12);
}

#ifdef NOT_USED
/*
 * Pad options to multiple of 4 bytes.
 */
static __inline int tcp_opt_padding (BYTE *opt, int len)
{
  int i, pad = len % 4;

  for (i = 0; i < pad; i++)
      *opt++ = TCPOPT_NOP;
  return (i);
}

static __inline int tcp_opt_winscale (tcp_Socket *s, BYTE *opt)
{
  *opt++ = TCPOPT_WINDOW;    /* option: WINDOW,length,wscale */
  *opt++ = 4;
  *(WORD*) opt = intel16 (s->send_wscale);
  return (4);
}

static __inline int tcp_opt_sack_ok (tcp_Socket *s, BYTE *opt)
{
  *opt++ = TCPOPT_SACKOK;
  *opt++ = 2;
  *opt++ = TCPOPT_NOP;
  *opt++ = TCPOPT_NOP;
  return (4);
}

static __inline int tcp_opt_sack (tcp_Socket *s, BYTE *opt,
                                  struct SACK_list *sack)
{
  int i, len = 2 + 8 * sack->num_blk;

  *opt++ = TCPOPT_SACK;       /* option: SACK,length,left,right,.. */
  *opt++ = len;
  for (i = 0; i < sack->num_blk; i++)
  {
    *(DWORD*) opt = intel (sack->list[i].left_edge);
    opt += sizeof(DWORD);
    *(DWORD*) opt = intel (sack->list[i].right_edge);
    opt += sizeof(DWORD);
  }
  for (i = 0; i < len % 4; i++)
     *opt++ = TCPOPT_NOP;
  return (len + i);
}
#endif

/*
 * tcp_do_options - Add TCP options to output segment
 */
static __inline int tcp_do_options (tcp_Socket *s, BYTE *opt, BOOL is_syn)
{
  DWORD now = set_timeout (0);
  int   len = 0;

  if (is_syn && !(s->locflags & LF_NOOPT))
  {
    len += tcp_opt_maxsegment (s, opt);

    if (s->locflags & LF_REQ_TSTMP)
       len += tcp_opt_timestamp (s, opt+len, now, 0UL);
#if 0
    if (s->locflags & LF_REQ_SCALE)
       len += tcp_opt_winscale (s, opt+len);

    if (tcp_opt_sackok)
       len += tcp_opt_sack_ok (s, opt+len);
#endif
  }
  else if (!is_syn)
  {
    /* We got a TS option in a previous SYN-ACK or SYN.
     * Send it back unless we send a RST or disallow options.
     * A Win98 host will have 0 in 's->ts_recent'
     */
    if ((s->flags & tcp_FlagRST) == 0 && tcp_opt_timstmp &&
        (s->locflags & (LF_RCVD_TSTMP|LF_NOOPT)) == LF_RCVD_TSTMP)
    {
      len += tcp_opt_timestamp (s, opt, now, s->ts_recent);
      s->locflags &= ~LF_RCVD_TSTMP;  /* don't echo this again */
    }
  }

#ifdef USE_DEBUG
  assert (len == 0 || len % 4 == 0);
#endif
  return (len);
}

/*
 * _tcp_send - Format and send an outgoing segment.
 *             Several packets may be sent depending on peer's window.
 */
int _tcp_send (tcp_Socket *s, char *file, unsigned line)
{
  #include <sys/packon.h>
  struct tcp_pkt {
         in_Header  in;
         tcp_Header tcp;
       } *pkt;
  #include <sys/packoff.h>

  tcp_PseudoHeader ph;

  BOOL         tx_ok;
  BYTE        *data;            /* where to copy user's data */
  mac_address *dst;
  in_Header   *ip;
  tcp_Header  *tcp;
  int          sendtotlen = 0;  /* count of data length we've sent */
  int          senddatalen;     /* how much data in this segment */
  int          startdata;       /* where data starts in tx-buffer */
  int          sendtotdata;     /* total amount of data to send */
  int          tcp_len;         /* total length of TCP segment */
  int          opt_len;         /* total length of TCP options */
  int          pkt_num;         /* 0..s->cwindow-1 */
  int          rtt;

  s->recent = 0;

  dst  = (_pktserial ? NULL : &s->hisethaddr);
  pkt  = (struct tcp_pkt*) _eth_formatpacket (dst, IP_TYPE);
  ip   = &pkt->in;
  tcp  = &pkt->tcp;
  data = (BYTE*) (tcp+1);

  if (s->karn_count == 2)   /* doing slow-start */
  {
    sendtotdata = min (s->datalen, s->window);
    startdata = 0;
  }
  else
  {
    /* Morten Terstrup <MorTer@dk-online.dk> found this signed bug
     */
    int size = min (s->datalen, s->window);
    sendtotdata = size - s->unacked;
    if (sendtotdata < 0)
        sendtotdata = 0;
    startdata = s->unacked;
  }

  /* step through our packets
   */
  for (pkt_num = 0; pkt_num < s->cwindow; pkt_num++)
  {
    /* make tcp header
     */
    tcp->srcPort  = intel16 (s->myport);
    tcp->dstPort  = intel16 (s->hisport);
    tcp->seqnum   = intel (s->seqnum + startdata); /* unacked - no longer sendtotlen */
    tcp->acknum   = intel (s->acknum);

    tcp->window   = intel16 (s->maxrdatalen - s->rdatalen);
    tcp->flags    = s->flags;
    tcp->unused   = 0;
    tcp->checksum = 0;
    tcp->urgent   = 0;

    /* Insert any TCP options after header
     */
    if (pkt_num == 0 && (s->flags & (tcp_FlagSYN|tcp_FlagACK)) == tcp_FlagSYN)
    {
      opt_len = tcp_do_options (s, data, TRUE);
      senddatalen = 0;   /* no data, only options */
    }
    else
    {
      int data_free;

      opt_len = tcp_do_options (s, data, FALSE);
      if ((data_free = s->max_seg - opt_len) < 0)
         data_free = 0;
      senddatalen = min (sendtotdata, data_free);
    }

    tcp_len = sizeof(*tcp) + opt_len;
    data   += opt_len;
    tcp->offset = tcp_len/4;

    if (senddatalen > 0)         /* non-SYN packets with data */
    {
      tcp_len += senddatalen;
      if (s->queuelen)
           memcpy (data, s->queue+startdata, senddatalen);
      else memcpy (data, s->data +startdata, senddatalen);
    }

    if (s->locflags & LF_NOPUSH)
       tcp->flags &= ~tcp_FlagPUSH;

    /* make tcp header check-sum
     */
    memset (&ph, 0, sizeof(ph));
    ph.src      = intel (s->myaddr);
    ph.dst      = intel (s->hisaddr);
    ph.protocol = TCP_PROTO;
    ph.length   = intel16 (tcp_len);
    ph.checksum = checksum (tcp, tcp_len);
    tcp->checksum = ~checksum (&ph, sizeof(ph));

    tx_ok = _ip_output (ip, ph.src, ph.dst, TCP_PROTO,
                        s->ttl, s->tos, 0, tcp_len, s, file, line) != 0;
    if (!tx_ok)
    {
      TCP_SENDSOON (s);
      return (-1);
    }

    /* do next packet
     */
    if (senddatalen > 0)
    {
      sendtotlen  += senddatalen;
      startdata   += senddatalen;
      sendtotdata -= senddatalen;
    }
    if (sendtotdata <= 0)
       break;
  }

  s->unacked = startdata;

#if defined(USE_DEBUG)
  if (debug_on > 1)
     (*_printf)(" Sent %u (win %u) bytes in %u (cwin %u) packets with "
                "(%u) unacked  SEQ %lu  line %u\r\n",
                sendtotlen, s->window, pkt_num, s->cwindow,
                s->unacked, s->seqnum, line);
#endif

  s->vj_last = 0UL;
  if (s->karn_count == 2)
  {
    if (s->rto)
         s->rto = (s->rto * 3) / 2;  /* increase by 50% */
    else s->rto = 2*tcp_RTO_ADD;     /* !!was 4 tick */
  }
  else
  {
    /* vj_last nonzero if we expect an immediate response
     */
    if (s->unhappy || s->datalen)
        s->vj_last = set_timeout (0);
    s->karn_count = 0;
  }

  rtt = s->rto + tcp_RTO_ADD;

  s->rtt_time = set_timeout (rtt);

  if (sendtotlen > 0)
     s->rtt_lasttran = s->rtt_time;

  return (sendtotlen);
}


/*
 * Force the peer to send us a segment by sending a keep-alive packet:
 *   <SEQ=SND.UNA-1><ACK=RCV.NXT><CTL=ACK>
 */
int sock_keepalive (sock_type *s)
{
  tcp_Socket *tcp;
  DWORD       ack, seq;
  BYTE        kc;
  int         datalen;

  if (s->tcp.ip_type != TCP_PROTO)
     return (0);

  tcp     = &s->tcp;
  ack     = tcp->acknum;
  seq     = tcp->seqnum;
  kc      = tcp->karn_count;
  datalen = tcp->datalen;

  tcp->acknum     = tcp->seqnum;
  tcp->seqnum     = tcp->unacked - 1;
  tcp->flags      = tcp_FlagACK;
  tcp->karn_count = 2;
  tcp->datalen    = 0;
  (void) TCP_SEND (tcp);

  tcp->acknum     = ack;
  tcp->seqnum     = seq;
  tcp->karn_count = kc;
  tcp->datalen    = datalen;

  STAT (tcpstats.tcps_keepprobe++);
  STAT (tcpstats.tcps_keeptimeo++);
  return (1);
}
#endif  /* !USE_UDP_ONLY */


/*
 * sock_mode - set binary or ascii - affects sock_gets, sock_dataready
 *           - set udp checksums
 */
WORD sock_mode (sock_type *s, WORD mode)
{
  if (s->tcp.ip_type == TCP_PROTO || s->tcp.ip_type == UDP_PROTO)
  {
    s->tcp.sockmode = (s->tcp.sockmode & 0xFFFC) | mode;
    return (s->tcp.sockmode);
  }
  return (0);
}

/*
 * sock_yield - enable user defined yield function
 */
int sock_yield (tcp_Socket *s, void (*fn)())
{
  if (s)
       s->usr_yield = fn;
  else system_yield = fn;
  return (0);
}


void sock_abort (sock_type *s)
{
  switch (s->tcp.ip_type)
  {
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
         tcp_abort (&s->tcp);
         break;
#endif
    case UDP_PROTO:
         udp_close (&s->udp);
         break;
    case IP_TYPE:
         s->raw.ip_type = 0;
         s->raw.used    = 0;
         break;
  }
}

#if defined(USE_BSD_FUNC)
/*
 * Read data from a raw-socket. Don't copy IP-header to buf.
 */
static int raw_read (raw_Socket *raw, BYTE *buf, int maxlen)
{
  int len = 0;

  if (raw->used)
  {
    int   hlen = in_GetHdrLen (&raw->ip);
    BYTE *data = (BYTE*)&raw->ip + hlen;

    len = intel16 (raw->ip.length) - hlen;
    len = min (len, maxlen);
    memcpy (buf, data, len);
    raw->used = 0;
  }
  return (len);
}
#endif

/*
 * sock_read - read a socket with maximum 'maxlen' bytes
 *           - busywaits until 'buf' is full (and call 's->usr_yield')
 *           - returns count ( <= maxlen) also when connection gets closed
 */
int sock_read (sock_type *s, BYTE *buf, int maxlen)
{
  int count = 0;

  do
  {
    int len = 0;
    int raw = 0;

    switch (s->udp.ip_type)
    {
#if !defined(USE_UDP_ONLY)
      case TCP_PROTO:
           len = tcp_read (&s->tcp, buf, maxlen);
           break;
#endif
      case UDP_PROTO:
           len = udp_read (&s->udp, buf, maxlen);
           break;

#if defined(USE_BSD_FUNC)
      case IP_TYPE:
           raw = TRUE;
           len = raw_read (&s->raw, buf, maxlen);
           break;
#endif
    }

    if (len < 1)
    {
      if (!tcp_tick(s))
         return (count);
    }
    else
    {
      count  += len;
      buf    += len;
      maxlen -= len;
    }
    if (maxlen > 0 && !raw && s->tcp.usr_yield) /* yield only when room */
      (*s->tcp.usr_yield)();                    /* 99.07.01 EE */
  }
  while (maxlen);
  return (count);
}

/*
 * sock_fastread - read a socket with maximum 'len' bytes
 *               - does not busywait until buffer is full
 */
int sock_fastread (sock_type *s, BYTE *buf, int len)
{
  if (s->udp.ip_type == UDP_PROTO)
     return udp_read (&s->udp, buf, len);

#if !defined(USE_UDP_ONLY)
  if (s->tcp.ip_type == TCP_PROTO || s->tcp.rdatalen > 0)
     return tcp_read (&s->tcp, buf, len);
#endif

#if defined(USE_BSD_FUNC)
  if (s->raw.ip_type == IP_TYPE)
     return raw_read (&s->raw, buf, len);
#endif

  return (-1);
}


/*
 * sock_write - writes data and returns length written
 *            - send with PUSH-bit (flush data)
 *            - repeatedly calls s->usr_yield
 */
int sock_write (sock_type *s, const BYTE *data, int len)
{
  int chunk   = len;
  int written = 0;

  while (chunk > 0)
  {
    switch (s->udp.ip_type)
    {
#if !defined(USE_UDP_ONLY)
      case TCP_PROTO:
           s->tcp.flags |= tcp_FlagPUSH;
           written = tcp_write (&s->tcp, data, chunk);
           break;
#endif           
      case UDP_PROTO:
           chunk = min (mtu - sizeof(in_Header) - sizeof(udp_Header), chunk);
           written = udp_write (&s->udp, data, chunk);
           break;

#if defined(USE_BSD_FUNC)
      case IP_TYPE:
           return (0);   /* not supported yet */
#endif
      default:           /* EE 99.06.14 */
           return (0);
    }

    if (written < 0)
    {
      s->udp.err_msg = _LANG("Tx Error");
      return (0);
    }
    data  += written;
    chunk -= written;

    if (s->udp.usr_yield)
      (*s->udp.usr_yield)();

    if (!tcp_tick(s))
       return (0);
  }
  return (len);
}

/*
 * sock_fastwrite
 *
 * NOTE: for UDP, assumes data fits in one datagram, else only the first
 *     fragment will be sent!  Because MTU is used for splits,
 *     by default the max data size is MTU-(20+sizeof(udp_Header)) = 548
 *     for a non-fragged datagram.
 */
int sock_fastwrite (sock_type *s, const BYTE *data, int len)
{
  switch (s->udp.ip_type)
  {
    case UDP_PROTO:
         len = min (mtu - sizeof(in_Header) - sizeof(udp_Header), len);
         len = udp_write (&s->udp, data, len);
         return (len < 0 ? 0 : len);

#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
         len = tcp_write (&s->tcp, data, len);
         return (len < 0 ? 0 : len);
#endif
  }
  return (0);
}


int sock_enqueue (sock_type *s, const BYTE *data, int len)
{
  if (len <= 0)
     return (0);

  if (s->udp.ip_type == UDP_PROTO)
  {
    int written = 0;
    int total   = 0;
    do
    {
      len = min (mtu - sizeof(in_Header) - sizeof(udp_Header), len);
      written = udp_write (&s->udp, data, len);
      if (written < 0)
      {
        s->udp.err_msg = _LANG("Tx Error");
        break;
      }
      data  += written;
      len   -= written;
      total += written;
    }
    while (len > 0);
    return (total);
  }

#if !defined(USE_UDP_ONLY)
  if (s->tcp.ip_type == TCP_PROTO)
  {
    s->tcp.queue    = data;
    s->tcp.queuelen = len;
    s->tcp.datalen  = len;
    return TCP_SEND (&s->tcp);
  }
#endif
  return (0);
}

#if !defined(USE_UDP_ONLY)
void sock_noflush (sock_type *s)
{
  if (s->tcp.ip_type == TCP_PROTO)
  {
    s->tcp.flags &= ~tcp_FlagPUSH;
    s->tcp.sockmode |= TCP_LOCAL;
  }
}

/*
 * sock_flush - Send pending TCP data
 */
void sock_flush (sock_type *s)
{
  if (s->tcp.ip_type == TCP_PROTO)
  {
    tcp_Socket *tcp = &s->tcp;

    tcp->sockmode &= ~TCP_LOCAL;
    if (tcp->datalen > 0)
    {
      tcp->flags |= tcp_FlagPUSH;
      if (s->tcp.unacked == 0)  /* !! S. Lawson - only if data not moving */
         (void) TCP_SEND (tcp);
    }
  }
}

/*
 * sock_flushnext - cause next transmission to have a flush
 */
void sock_flushnext (sock_type *s)
{
  if (s->tcp.ip_type == TCP_PROTO)
  {
    s->tcp.flags |= tcp_FlagPUSH;
    s->tcp.sockmode &= ~TCP_LOCAL;
  }
}
#endif  /* !USE_UDP_ONLY */


int sock_close (sock_type *s)
{
  switch (s->tcp.ip_type)
  {
    case UDP_PROTO:
         udp_close (&s->udp);
         break;

#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
         _tcp_close (&s->tcp);
         (void) tcp_tick (s);
         break;
#endif
  }
  return (0);
}

#if !defined(USE_UDP_ONLY)

/*
 * Round trip timing cache routines.
 * These functions implement a very simple system for keeping track of
 * network performance for future use in new connections.
 * The emphasis here is on speed of update (rather than optimum cache
 * hit ratio) since tcp_rtt_add() is called every time a TCP connection
 * updates its round trip estimate. Note: 'rto' is either in ticks or
 * milli-sec depending on if PC has an 8254 Time chip.
 *
 * These routines are modified versions from KA9Q by Phil Karn.
 */
static struct tcp_rtt rtt_cache [RTTCACHE];

static void tcp_rtt_add (tcp_Socket *s, UINT rto)
{
  struct tcp_rtt *rtt;
  DWORD  addr = s->hisaddr;

  if (~addr & ~sin_mask)  /* 0.0.0.0 or broadcast addresses? */
     return;

  rtt = &rtt_cache [(WORD)addr % RTTCACHE];

  /* Cache-slot is vacant or we're updating previous RTO
   * for same peer
   */
  if (!rtt->ip || rtt->ip == addr)
  {
    rtt->ip  = addr;
    rtt->rto = rto;
    STAT (tcpstats.tcps_cachedrtt++);
  }
}

static UINT tcp_rtt_get (tcp_Socket *s)
{
  struct tcp_rtt *rtt = &rtt_cache [(WORD)s->hisaddr % RTTCACHE];

  if (s->hisaddr && rtt->ip == s->hisaddr && rtt->rto > 0)
  {
#if defined(USE_DEBUG) && !defined(_MSC_VER) /* MSC6 crashes below */
    char buf[20];
    dbug_write_raw ("\r\nRTT-cache: host ");
    dbug_write_raw (_inet_ntoa(buf, rtt->ip));
    dbug_write_raw (": ");
    dbug_write_raw (time_str(rtt->rto));
    dbug_write_raw ("s\r\n\r\n");
#endif

    STAT (tcpstats.tcps_usedrtt++);
    return (rtt->rto);
  }
  return (0);
}

static void tcp_rtt_clr (tcp_Socket *s)
{
  struct tcp_rtt *rtt = &rtt_cache [(WORD)s->hisaddr % RTTCACHE];

  if (s->hisaddr && rtt->ip == s->hisaddr)
  {
    rtt->rto = 0;
    rtt->ip  = 0;
  }
}
#endif /* !USE_UDP_ONLY */

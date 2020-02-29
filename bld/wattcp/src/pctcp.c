
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
#include "pchooks.h"
#endif

#ifndef __inline  /* normally in <sys/cdefs.h> */
#define __inline
#endif

#if defined(USE_BSD_FUNC)
/*
 * These are hooks to prevent the BSD-socket API being linked in
 * by default. These function pointers are only set from the BSD
 * functions when needed; `_raw_ip_hook' is set to filter SOCK_RAW
 * packets, `_tcp_syn_hook' is set to filter incoming SYN packets
 * for SOCK_STREAM packets used in `accept()'. And '_tcp_find_hook'
 * is set to `socket_find_tcp()' when allocating SOCK_STREAM sockets.
 */
int   (*_raw_ip_hook)  (const in_Header *) = NULL;
int   (*_tcp_syn_hook) (tcp_Socket **tcp_skp) = NULL;
Socket *(*_tcp_find_hook) (const tcp_Socket *tcp_sk) = NULL;
#endif

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

static tcp_Socket *tcp_findseq (const in_Header *ip, const tcp_Header *tcp_hdr);

static void tcp_sockreset(tcp_Socket *tcp_sk, int proxy);
static void tcp_rtt_wind (tcp_Socket *tcp_sk);
static void tcp_upd_wind (tcp_Socket *tcp_sk, unsigned line);
static int  tcp_chksum   (const in_Header *ip, const tcp_Header *tcp_hdr, int len);

static void tcp_rtt_add  (tcp_Socket *tcp_sk, UINT rto);
static void tcp_rtt_clr  (tcp_Socket *tcp_sk);
static UINT tcp_rtt_get  (tcp_Socket *tcp_sk);

#endif

static void udp_close   (udp_Socket *udp_sk);
static void (*system_yield)(void) = NULL;


/*
 * Passive open: listen for a connection on a particular port
 */
int udp_listen (udp_Socket *udp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler)
{
    udp_close (udp_sk);
    watt_largecheck (udp_sk, sizeof(*udp_sk), __FILE__, __LINE__);
    memset (udp_sk, 0, sizeof(*udp_sk));

    udp_sk->rdata        = &udp_sk->rddata[0];
    udp_sk->maxrdatalen  = udp_MaxBufSize;
    udp_sk->ip_type      = UDP_PROTO;
    udp_sk->myport       = findfreeport (lport, 0); /* get a nonzero port val */
    udp_sk->hisport      = port;
    udp_sk->hisaddr      = ina;
    udp_sk->ttl          = _default_ttl;
    udp_sk->protoHandler = handler;
    udp_sk->usr_yield    = system_yield;
    udp_sk->safetysig    = SAFETYUDP;               /* insert into chain */
    udp_sk->next         = _udp_allsocs;
    _udp_allsocs    = udp_sk;

    return (1);
}

/*
 * Active open: open a connection on a particular port
 */
int udp_open (udp_Socket *udp_sk, WORD lport, DWORD ip, WORD port, ProtoHandler handler)
{
    BOOL bcast = (ip == (DWORD)-1) ||
               (~ip & ~sin_mask) == 0;

    udp_close (udp_sk);
    watt_largecheck (udp_sk, sizeof(*udp_sk), __FILE__, __LINE__);
    memset (udp_sk, 0, sizeof(*udp_sk));

    if (ip - my_ip_addr <= multihomes)
        return (0);

    udp_sk->rdata       = &udp_sk->rddata[0];
    udp_sk->maxrdatalen = udp_MaxBufSize;
    udp_sk->ip_type     = UDP_PROTO;
    udp_sk->myport      = findfreeport (lport, 0);
    udp_sk->myaddr      = my_ip_addr;
    udp_sk->ttl         = _default_ttl;

    if (bcast || !ip) {    /* check for broadcast */
        memset (udp_sk->hisethaddr, 0xFF, sizeof(eth_address));
        if (!ip) {
            ip = (DWORD)-1;   /* make udp_sk->hisaddr = 255.255.255.255 */
        }
#if defined(USE_MULTICAST)
    } else if (is_multicast(ip)) { /* check for multicast */
        multi_to_eth (ip, (BYTE*)&udp_sk->hisethaddr[0]);
        udp_sk->ttl = 1;     /* so we don't send worldwide as default */
#endif
    } else if (!_arp_resolve(ip, &udp_sk->hisethaddr, 0)) {
        return (0);
    }

    udp_sk->hisaddr      = ip;
    udp_sk->hisport      = port;
    udp_sk->protoHandler = handler;
    udp_sk->usr_yield    = system_yield;
    udp_sk->safetysig    = SAFETYUDP;
    udp_sk->next         = _udp_allsocs;
    _udp_allsocs    = udp_sk;
    return (1);
}

/*
 *  Since UDP is stateless, simply reclaim the local-port and
 *  unthread the socket from the list.
 */
static void udp_close (udp_Socket *udp_sk)
{
    udp_Socket *_udp_sk, *prev;

    for (_udp_sk = prev = _udp_allsocs; _udp_sk != NULL; prev = _udp_sk, _udp_sk = _udp_sk->next) {
        if (_udp_sk != udp_sk)
            continue;

        (void) reuse_localport (_udp_sk->myport);

        if (_udp_sk == _udp_allsocs) {
            _udp_allsocs = _udp_sk->next;
        } else {
            prev->next   = _udp_sk->next;
        }
        if (_udp_sk->err_msg == NULL) {
            _udp_sk->err_msg = _LANG("UDP Close called");
        }
    }
}

/*
 * Set the TTL on an outgoing UDP datagram.
 */
void udp_SetTTL (udp_Socket *udp_sk, BYTE ttl)
{
    udp_sk->ttl = ttl;
}


#if !defined(USE_UDP_ONLY)

/*
 * Actively open a TCP connection to a particular destination.
 *  - 0 on error
 *
 * 'lport' is local port to associate with the connection.
 * 'rport' is remote port for same connection
 */
int tcp_open (tcp_Socket *tcp_sk, WORD lport, DWORD ina, WORD rport, ProtoHandler handler)
{
    UINT rtt;

    watt_largecheck (tcp_sk, sizeof(*tcp_sk), __FILE__, __LINE__);
    (void) _tcp_unthread (tcp_sk);       /* just in case not totally closed */
    memset (tcp_sk, 0, sizeof(*tcp_sk));

    if ((ina - my_ip_addr <= multihomes) || is_multicast(ina))
        return (0);

    if (!_arp_resolve(ina, &tcp_sk->hisethaddr, 0))
        return (0);

    tcp_sk->rdata        = &tcp_sk->rddata[0];
    tcp_sk->maxrdatalen  = tcp_MaxBufSize;
    tcp_sk->ip_type      = TCP_PROTO;
    tcp_sk->max_seg      = mss;        /* to-do !!: use mss from setsockopt() */
    tcp_sk->state        = tcp_StateSYNSENT;
    tcp_sk->timeout      = set_timeout (tcp_LONGTIMEOUT);

    /* to-do !!: use TCP_NODELAY set in setsockopt()
     */
    if (!tcp_nagle)
        SETON_SOCKMODE(*tcp_sk, TCP_MODE_NONAGLE);

    tcp_sk->cwindow      = 1;
    tcp_sk->wwindow      = 0;                       /* slow start VJ algorithm */
    tcp_sk->vj_sa        = INIT_VJSA;
    tcp_sk->rto          = tcp_OPEN_TO;             /* added 14-Dec 1999, GV   */
    tcp_sk->myaddr       = my_ip_addr;
    tcp_sk->myport       = findfreeport (lport, 1); /* get a nonzero port val  */
    tcp_sk->locflags     = LF_LINGER;               /* close via TIMEWT state  */
    if (tcp_opt_timstmp)
        tcp_sk->locflags |= LF_REQ_TSTMP;           /* use timestamp option */

    tcp_sk->ttl          = _default_ttl;
    tcp_sk->hisaddr      = ina;
    tcp_sk->hisport      = rport;
    tcp_sk->seqnum       = INIT_SEQ();
    tcp_sk->flags        = tcp_FlagSYN;
    tcp_sk->unhappy      = TRUE;
    tcp_sk->protoHandler = handler;
    tcp_sk->usr_yield    = system_yield;

    tcp_sk->safetysig    = SAFETYTCP;               /* marker signatures */
    tcp_sk->safetytcp    = SAFETYTCP;
    tcp_sk->next         = _tcp_allsocs;            /* insert into chain */
    _tcp_allsocs    = tcp_sk;

    (void) TCP_SEND (tcp_sk);                       /* send opening SYN */

    /* find previous RTT replacing RTT set in tcp_send() above
     */
    if ((rtt = tcp_rtt_get(tcp_sk)) > 0) {
        tcp_sk->rtt_time = set_timeout (rtt);
    } else {
        tcp_sk->rtt_time = set_timeout (tcp_OPEN_TO);
    }
    return (1);
}

/*
 * Passive open: listen for a connection on a particular port
 */
int tcp_listen (tcp_Socket *tcp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler, WORD timeout)
{
    watt_largecheck (tcp_sk, sizeof(*tcp_sk), __FILE__, __LINE__);
    (void) _tcp_unthread (tcp_sk);    /* just in case not totally closed */
    memset (tcp_sk, 0, sizeof(*tcp_sk));

    if (is_multicast(ina))
        return (0);

    tcp_sk->rdata        = &tcp_sk->rddata[0];
    tcp_sk->maxrdatalen  = tcp_MaxBufSize;
    tcp_sk->ip_type      = TCP_PROTO;
    tcp_sk->max_seg      = mss;        /* to-do !!: use mss from setsockopt() */
    tcp_sk->cwindow      = 1;
    tcp_sk->wwindow      = 0;               /* slow start VJ algorithm */
    tcp_sk->vj_sa        = INIT_VJSA;
    tcp_sk->state        = tcp_StateLISTEN;
    tcp_sk->locflags     = LF_LINGER;

    tcp_sk->myport       = findfreeport (lport, 0);
    tcp_sk->hisport      = port;
    tcp_sk->hisaddr      = ina;
    tcp_sk->seqnum       = INIT_SEQ();
    tcp_sk->unhappy      = FALSE;
    tcp_sk->ttl          = _default_ttl;
    tcp_sk->protoHandler = handler;
    tcp_sk->usr_yield    = system_yield;
    tcp_sk->safetysig    = SAFETYTCP;      /* marker signatures */
    tcp_sk->safetytcp    = SAFETYTCP;
    tcp_sk->next         = _tcp_allsocs;   /* insert into chain */
    _tcp_allsocs    = tcp_sk;

    if (timeout != 0)
        tcp_sk->timeout = set_timeout (1000 * timeout);
    return (1);
}

/*
 * Reuse local port now if not owned by a STREAM-socket.
 * Otherwise let socket daemon free local port when linger period
 * expires. We don't care about rapid reuse of local ports connected
 * to DGRAM-sockets.
 */
static void maybe_reuse_lport (tcp_Socket *tcp_sk)
{
#if defined(USE_BSD_FUNC)
    if (_tcp_find_hook == NULL || (*_tcp_find_hook)(tcp_sk) == NULL) {
#endif
        reuse_localport (tcp_sk->myport);
#if defined(USE_BSD_FUNC)
    }
#endif
}

/*
 *  Send a FIN on a particular port -- only works if it is open.
 *  Must still allow receives
 */
void _tcp_close (tcp_Socket *tcp_sk)
{
    if (tcp_sk->ip_type != TCP_PROTO)
        return;

    if (tcp_sk->state == tcp_StateESTAB ||
        tcp_sk->state == tcp_StateESTCL ||
        tcp_sk->state == tcp_StateSYNREC)
    {
        if (tcp_sk->datalen) {    /* must first flush all Tx data */
            tcp_sk->flags |= (tcp_FlagPUSH | tcp_FlagACK);
            if (tcp_sk->state < tcp_StateESTCL) {
                tcp_sk->state = tcp_StateESTCL;
                TCP_SENDSOON (tcp_sk);
            }
        } else { /* really closing */
            tcp_sk->flags = (tcp_FlagACK | tcp_FlagFIN);
            if (tcp_sk->err_msg == NULL)
                tcp_sk->err_msg = _LANG("Connection closed normally");

            tcp_sk->state    = tcp_StateFINWT1;
            tcp_sk->timeout  = set_timeout (tcp_TIMEOUT);
            tcp_sk->rtt_time = 0UL;   /* stop RTT timer */
            (void) TCP_SEND (tcp_sk);
        }
        tcp_sk->unhappy = TRUE;
    } else if (tcp_sk->state == tcp_StateCLOSWT) {
        /* need to ACK the FIN and get on with it
         */
        tcp_sk->timeout = set_timeout (tcp_LASTACK_TIME); /* Added AGW 6 Jan 2001 */
        tcp_sk->state   = tcp_StateLASTACK;
        tcp_sk->flags  |= tcp_FlagFIN;
        (void) TCP_SEND (tcp_sk);
        tcp_sk->unhappy = TRUE;
    } else if (tcp_sk->state == tcp_StateSYNSENT) { /* unlink failed connection */
        tcp_sk->state = tcp_StateCLOSED;
        maybe_reuse_lport (tcp_sk);
        (void) _tcp_unthread (tcp_sk);
    }
}

/*
 * Abort a tcp connection
 */
void tcp_abort (tcp_Socket *tcp_sk)
{
    if (tcp_sk->err_msg == NULL)
        tcp_sk->err_msg = _LANG("TCP Abort");

    if (tcp_sk->state != tcp_StateLISTEN && tcp_sk->state != tcp_StateCLOSED) {
        tcp_sk->flags   = (tcp_FlagRST | tcp_FlagACK);
        tcp_sk->unhappy = TRUE;
        if (tcp_sk->state <= tcp_StateSYNREC) {
            tcp_sk->rtt_time = 0UL; /* Stop RTT timer */
            tcp_rtt_clr (tcp_sk);   /* Clear cached RTT */
        }
        (void) TCP_SEND (tcp_sk);
    }
    tcp_sk->unhappy = FALSE;
    tcp_sk->datalen = 0;        /* discard Tx buffer, but not Rx buffer */
    tcp_sk->ip_type = 0;

    maybe_reuse_lport (tcp_sk);
    (void) _tcp_unthread (tcp_sk);
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
int _tcp_sendsoon (tcp_Socket *tcp_sk, char *file, unsigned line)
{
    DWORD timeout;

    if (tcp_sk->ip_type != TCP_PROTO)
        return (0);

    if (tcp_sk->state >= tcp_StateCLOSWT) {
        timeout = set_timeout (tcp_CLOSE_TO);
    } else {
        timeout = set_timeout (tcp_RTO_BASE);
    }

    if (tcp_sk->rto <= tcp_RTO_BASE && tcp_sk->recent == 0 &&
      cmp_timers(tcp_sk->rtt_time, timeout) <= 0)
    {                         /* !! was == */
        int rc;

        tcp_sk->karn_count = 0;
        rc = _tcp_send (tcp_sk, file, line);
        tcp_sk->recent = 1;
        return (rc);
    }

    if ((tcp_sk->unhappy || tcp_sk->datalen > 0 || tcp_sk->karn_count == 1) &&
      (tcp_sk->rtt_time && cmp_timers(tcp_sk->rtt_time, timeout) < 0))
        return (0);

    if (tcp_sk->state == tcp_StateSYNSENT) { /* relaxed in SYNSENT state */
        tcp_sk->rtt_time = set_timeout (tcp_OPEN_TO);
    } else {
        tcp_sk->rtt_time = set_timeout (tcp_RTO_BASE + (tcp_sk->rto >> 4));
    }

    tcp_sk->karn_count = 1;

    return (0);
}

/*
 * Unthread a socket from the tcp socket list, if it's there
 */
tcp_Socket *_tcp_unthread (tcp_Socket *tcp_sk)
{
    tcp_Socket *_tcp_sk, *prev;
    tcp_Socket *next = NULL;

    for (_tcp_sk = prev = _tcp_allsocs; _tcp_sk != NULL; prev = _tcp_sk, _tcp_sk = _tcp_sk->next) {
        if (_tcp_sk != tcp_sk)
            continue;

        if (_tcp_sk == _tcp_allsocs) {
            _tcp_allsocs = _tcp_sk->next;
        } else {
            prev->next   = _tcp_sk->next;
        }
        next = _tcp_sk->next;
    }

    if (tcp_sk->rdatalen == 0 || (tcp_sk->state > tcp_StateESTCL))
        tcp_sk->ip_type = 0;             /* fail further I/O */
    tcp_sk->state = tcp_StateCLOSED;     /* tcp_tick needs this */

    return (next);
}

/*
 * Returns 1 if connection is established
 */
int tcp_established (tcp_Socket *tcp_sk)
{
    return (tcp_sk->state >= tcp_StateESTAB);
}

/*
 *  tcp_handler - All tcp input processing is done from here.
 */
static tcp_Socket *tcp_handler (const in_Header *ip, BOOL broadcast)
{
    tcp_Header *tcp_hdr;
    tcp_Socket *_tcp_sk;
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

    len   = in_GetHdrLen (ip);                  /* len of IP header  */
    tcp_hdr = (tcp_Header*) ((BYTE*)ip + len);  /* tcp frame pointer */
    len   = intel16 (ip->length) - len;         /* len of tcp+data   */
    flags = tcp_hdr->flags & tcp_FlagMASK;      /* get TCP flags     */

    if (!tcp_chksum(ip, tcp_hdr, len)) {
        DEBUG_RX (NULL, ip);
        return (NULL);
    }

    dstPort = intel16 (tcp_hdr->dstPort);
    srcPort = intel16 (tcp_hdr->srcPort);

    /* demux to active sockets
     */
    for (_tcp_sk = _tcp_allsocs; _tcp_sk != NULL; _tcp_sk = _tcp_sk->next) {
        if (_tcp_sk->safetysig != SAFETYTCP || _tcp_sk->safetytcp != SAFETYTCP) {
            outsnl (_LANG("tcp-socket error in tcp_handler()"));
            DEBUG_RX ((sock_type *)_tcp_sk, ip);
            return (NULL);
        }

        if (_tcp_sk->hisport            &&   /* not a listening socket */
            destin  == _tcp_sk->myaddr  &&   /* addressed to my IP */
            source  == _tcp_sk->hisaddr &&   /* and from my peer address */
            dstPort == _tcp_sk->myport  &&   /* addressed to my local port */
            srcPort == _tcp_sk->hisport)     /* and from correct remote port */
        break;
    }

    if (_tcp_sk == NULL && (flags & tcp_FlagSYN)) {
        /* demux to passive (listening) sockets, must be a new session
         */
        for (_tcp_sk = _tcp_allsocs; _tcp_sk != NULL; _tcp_sk = _tcp_sk->next) {
            if (_tcp_sk->hisport == 0 &&        /* =0, listening socket */
                dstPort    == _tcp_sk->myport)  /* addressed to my local port */
            {
                _tcp_sk->hisport = srcPort;     /* remember his IP-address */
                _tcp_sk->hisaddr = source;      /*   and src-port */
                _tcp_sk->myaddr  = destin;      /* socket is now active */
                break;
            }
        }
    }

    DEBUG_RX ((sock_type *)_tcp_sk, ip);

    if (_tcp_sk == NULL) {
        if (!(flags & tcp_FlagRST)) {           /* don't answer RST */
            TCP_RESET (NULL, ip, tcp_hdr);
        } else if ((flags & tcp_FlagACK) &&     /* got ACK,RST   */
            (_tcp_sk = tcp_findseq(ip, tcp_hdr)) != NULL) { /* ACK = SEQ + 1 */
            tcp_sockreset (_tcp_sk, 1);         /* e.g. a firewall is sending */
        }
        STAT (tcpstats.tcps_drops++);           /* RST for server on inside   */
        return (NULL);
    }

    /* Restart idle-timer
     */
    if (sock_inactive && !(_tcp_sk->locflags & LF_RCVTIMEO))
        _tcp_sk->inactive_to = set_timeout (1000 * sock_inactive);

    if (flags & tcp_FlagRST) {       /* got reset code */
        tcp_sockreset (_tcp_sk, 0);
        return (NULL);
    }

    tcp_rtt_wind (_tcp_sk);         /* update retrans timer, windows etc. */

    if (_tcp_fsm(&_tcp_sk, ip) &&   /* do input tcp state-machine */
        _tcp_sk->unhappy)           /* if "unhappy", retransmit soon */
        TCP_SENDSOON (_tcp_sk);

    return (_tcp_sk);
}
#endif  /* !USE_UDP_ONLY */


/*
 * Handler for incoming udp packets.
 */
static udp_Socket *udp_handler (const in_Header *ip, BOOL broadcast)
{
    udp_Socket        *_udp_sk;
    udp_Header        *udp_hdr;
    tcp_PseudoHeader  tcp_phdr;

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
    udp_hdr = (udp_Header*) ((BYTE*)ip + len);   /* udp segment pointer */
    len = intel16 (udp_hdr->length);

    if (len < sizeof(*udp_hdr)) {
        DEBUG_RX (NULL, ip);
        STAT (udpstats.udps_hdrops++);
        return (NULL);
    }

    srcPort = intel16 (udp_hdr->srcPort);
    dstPort = intel16 (udp_hdr->dstPort);

    /* demux to active sockets
     */
    for (_udp_sk = _udp_allsocs; _udp_sk != NULL; _udp_sk = _udp_sk->next) {
        if (_udp_sk->safetysig != SAFETYUDP) {
            outsnl (_LANG("udp-socket error in udp_handler()"));
            DEBUG_RX ((sock_type *)_udp_sk, ip);
            return (NULL);
        }
        if (!ip_bcast               &&
            (_udp_sk->hisport != 0)       &&
            (dstPort == _udp_sk->myport)  &&
            (srcPort == _udp_sk->hisport) &&
            ((destin & sin_mask) == (_udp_sk->myaddr & sin_mask)) &&
            (intel(ip->source)   == _udp_sk->hisaddr)) {
            break;
        }
    }

    if (_udp_sk == NULL) {
        /* demux to passive sockets
         */
        for (_udp_sk = _udp_allsocs; _udp_sk != NULL; _udp_sk = _udp_sk->next) {
            if ((_udp_sk->hisaddr == 0 || _udp_sk->hisaddr == 0xFFFFFFFFUL) &&
                dstPort == _udp_sk->myport)
            {
                if (_udp_sk->hisaddr == 0) {
                    _udp_sk->hisaddr = intel (ip->source);
                    _udp_sk->hisport = srcPort;
                    SET_PEER_MAC_ADDR (_udp_sk, ip);

                    /* take on value of expected destination
                     * unless it is broadcast
                     */
                    if (!ip_bcast) {
                        _udp_sk->myaddr = destin;
                    }
                }
                break;
            }
        }
    }

    DEBUG_RX ((sock_type *)_udp_sk, ip);

#if defined(USE_MULTICAST)
    if (_udp_sk == NULL) {
        /* demux to multicast sockets
         */
        for (_udp_sk = _udp_allsocs; _udp_sk != NULL; _udp_sk = _udp_sk->next) {
            if (_udp_sk->hisport != 0      &&
                _udp_sk->hisaddr == destin &&
                dstPort == _udp_sk->myport &&
                is_multicast(destin)) {
                break;
            }
        }
    }
#endif

    if (_udp_sk == NULL) {
        /* Demux to broadcast sockets.
         */
        for (_udp_sk = _udp_allsocs; _udp_sk != NULL; _udp_sk = _udp_sk->next) {
            if (_udp_sk->hisaddr == (DWORD)-1 && dstPort == _udp_sk->myport) {
                break;
            }
        }
    }

    if (_udp_sk == NULL) {
        /* no demultiplexer found anything */
        if (debug_on)
            outs (_LANG("discarding..."));

        if ((destin - my_ip_addr <= multihomes) && my_ip_addr) {
            if (!ip_bcast &&              /* broadcast? */
                srcPort != DOM_DST_PORT) { /* from a nameserver?  */
                icmp_unreach (ip, 3);      /* send port unreachable */
            }

            if (ip_bcast) {
                STAT (udpstats.udps_noportbcast++);
            } else {
                STAT (udpstats.udps_noport++);
            }
        }
        return (NULL);
    }

    /* these parameters are used for things other than just checksums
     */
    memset (&tcp_phdr, 0, sizeof(tcp_phdr));
    tcp_phdr.src      = ip->source;      /* already network order */
    tcp_phdr.dst      = ip->destination;
    tcp_phdr.protocol = UDP_PROTO;
    tcp_phdr.length   = udp_hdr->length;
    tcp_phdr.checksum = checksum (udp_hdr, len);

    if (udp_hdr->checksum && ISOFF_SOCKMODE(*_udp_sk, UDP_MODE_NOCHK)) {
        if (checksum(&tcp_phdr, sizeof(tcp_phdr)) != 0xFFFF) {
            if (debug_on)
                outsnl (_LANG("bad udp checksum"));
            STAT (udpstats.udps_badsum++);
            return (_udp_sk);
        }
    }

    /* Process user data. 0-byte probe is legal for s->protoHandler.
     */
    {
        BYTE *data = (BYTE*)(udp_hdr+1);

        len -= sizeof(*udp_hdr);
        if (_udp_sk->protoHandler != NULL) {
            (*_udp_sk->protoHandler) ((sock_type *)_udp_sk, data, len, &tcp_phdr, udp_hdr);
        /* save first received packet rather than latest */
        } else if (len > 0 && _udp_sk->rdatalen == 0) {
            if (len > _udp_sk->maxrdatalen) { /* truncate data :-( */
                len = _udp_sk->maxrdatalen;
                STAT (udpstats.udps_fullsock++);
            }
            /* Might overwrite previous data! But hey, this is UDP..
             */
            memcpy (_udp_sk->rdata, data, len);
            _udp_sk->rdatalen = len;
        }
    }
    return (_udp_sk);
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
    tcp_Socket *_tcp_sk, *next;

    static DWORD timeout = 0UL;

    /* do this once per tcp_RETRAN_TIME
     */
    if (!force && timeout && !chk_timeout(timeout))
        return;

    timeout = set_timeout (tcp_RETRAN_TIME);

    for (_tcp_sk = _tcp_allsocs; _tcp_sk != NULL; _tcp_sk = next) {
        next = _tcp_sk->next;

        /* possible to be closed but still queued
         */
        if (_tcp_sk->state == tcp_StateCLOSED) {
            if (_tcp_sk->rdatalen == 0) {
                maybe_reuse_lport (_tcp_sk);
                next = _tcp_unthread (_tcp_sk);
            }
            continue;
        }

        if (_tcp_sk->datalen > 0 || _tcp_sk->unhappy || _tcp_sk->karn_count == 1) {
            if (chk_timeout(_tcp_sk->rtt_time)) { /* retransmission timeout */
                _tcp_sk->rtt_time = 0UL;           /* stop RTT timer */

#if defined(USE_DEBUG)
                if (debug_on > 1) {
                    (*_printf) ("regular retran TO set unacked back to 0 from %u\r\n",
                      _tcp_sk->unacked);
                }
#endif
                /* strategy handles closed windows.  JD + EE
                 */
                if (_tcp_sk->window == 0 && _tcp_sk->karn_count == 2)
                    _tcp_sk->window = 1;

                if (_tcp_sk->karn_count == 0) {
                    /* Simple "Slow start" algorithm:
                     * Use the backed off RTO - implied, no code necessary.
                     * Reduce the congestion window by 25%
                     */
                    unsigned cwindow = ((unsigned)(_tcp_sk->cwindow + 1) * 3) >> 2;

                    _tcp_sk->cwindow = cwindow;
                    if (_tcp_sk->cwindow == 0)
                        _tcp_sk->cwindow = 1;

                    _tcp_sk->wwindow = 0;       /* dup ACK counter ? */
                    /* _tcp_sk->snd_ssthresh = _tcp_sk->cwindow * _tcp_sk->max_seg; */ /* !!to-do */

                    /* if really did timeout
                     */
                    _tcp_sk->karn_count = 2;
                    _tcp_sk->unacked    = 0;
                }
                if (_tcp_sk->datalen > 0)
                    _tcp_sk->flags |= (tcp_FlagPUSH | tcp_FlagACK);

                if (_tcp_sk->unhappy) {
                    STAT (tcpstats.tcps_rexmttimeo++);
                } else if (_tcp_sk->flags & tcp_FlagACK) {
                    STAT (tcpstats.tcps_delack++);
                }

                (void) TCP_SEND (_tcp_sk);
            }

            /* handle inactive tcp timeouts (not sending data)
             */
            if (chk_timeout(_tcp_sk->datatimer)) { /* EE 99.08.23 */
                _tcp_sk->err_msg = _LANG("Connection timed out - no data sent");
                tcp_abort (_tcp_sk);
            }
        }  /* end of retransmission strategy */


        /* handle inactive tcp timeouts (not received anything)
         */
        if (chk_timeout(_tcp_sk->inactive_to)) {
            /* this baby has timed out. Don't do this again.
             */
            _tcp_sk->inactive_to = 0UL;
            _tcp_sk->err_msg = _LANG("Connection timed out - no activity");
            sock_close ((sock_type*)_tcp_sk);
        } else if (chk_timeout(_tcp_sk->timeout)) {
            if (_tcp_sk->state == tcp_StateTIMEWT) {
                _tcp_sk->state = tcp_StateCLOSED;
                break;
            } else if (_tcp_sk->state != tcp_StateESTAB && _tcp_sk->state != tcp_StateESTCL) {
                _tcp_sk->err_msg = _LANG("Timeout, aborting");
                tcp_abort (_tcp_sk);
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
    sock_type   *sk;

    if (block_ip || !_chk_ip_header(ip))
        return (0);

#if 0
    /* to-do: check for LSRR option and replace ip->source
     *        with actual source-address burried in option
     *        (ref. RFC1122)
     */
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
    if (_raw_ip_hook != NULL && (*_raw_ip_hook)(ip))
        return (1);
#endif

    switch (ip->proto) {
    case UDP_PROTO:
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        if (ip->proto == TCP_PROTO) {
            sk = (sock_type *)tcp_handler (ip, broadcast);
        } else {
#endif
            sk = (sock_type *)udp_handler (ip, broadcast);
#if !defined(USE_UDP_ONLY)
        }
#endif
        if (sk != NULL) {    /* Check if peer allows IP-fragments */
            if (intel16(ip->frag_ofs) & IP_DF) {
                sk->u.locflags |=  LF_NOFRAGMENT;
            } else {
                sk->u.locflags &= ~LF_NOFRAGMENT;
            }
        }
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
        if (!broadcast) {
            if (is_local_addr (intel(ip->destination)))
                icmp_unreach (ip, 2);  /* protocol unreachable */
            DEBUG_RX (NULL, ip);
            STAT (ipstats.ips_noproto++);
        }
        return (0);
    }
    STAT (ipstats.ips_delivered++);
    return (1);
}


/*
 * tcp_tick - called periodically by user application
 *          - called with socket parameter or NULL
 *          - returns 1 when our socket closes
 */
WORD tcp_tick (sock_type *sk)
{
    static DWORD daemon_timer = 0UL;

#if !defined(USE_UDP_ONLY)
    /* finish off dead sockets
     */
    if (sk != NULL && (sk->u.ip_type == TCP_PROTO)) {
        if ((sk->tcp.state == tcp_StateCLOSED) && (sk->tcp.rdatalen == 0)) {
            (void) _tcp_unthread (&sk->tcp);
            sk->u.ip_type = 0;   /* fail further I/O */
        }
    }
#endif
    for ( ;; ) {
        WORD eth_type = 0;
        BOOL brdcast  = FALSE;
        void *packet  = _eth_arrived (&eth_type, &brdcast);

        if (!packet)  /* packet points to network layer protocol */
            break;

        switch (eth_type) {
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

    if ((daemon_timer == 0UL || chk_timeout(daemon_timer)) && wattcpd) {
        (*wattcpd)();        /* do our various daemons */
        daemon_timer = set_timeout (DAEMON_RUN_TIME);
    }

    return ((sk != NULL) ? sk->u.ip_type : 0);
}

/*
 * udp_write()
 */
static int udp_write (udp_Socket *udp_sk, const BYTE *data, int len)
{
    #include <sys/packon.h>
    struct udp_pkt {
         in_Header  in;
         udp_Header udp_hdr;
      /* BYTE       data[]; */
       } *pkt;
    #include <sys/packoff.h>

    tcp_PseudoHeader  tcp_phdr;
    in_Header         *ip;
    udp_Header        *udp_hdr;
    mac_address       *dst;

    /* build link-layer header
     */
    dst = (_pktserial ? NULL : &udp_sk->hisethaddr);
    pkt = (struct udp_pkt*) _eth_formatpacket (dst, IP_TYPE);
    ip  = &pkt->in;
    udp_hdr = &pkt->udp_hdr;

    /* build udp header
     */
    udp_hdr->srcPort  = intel16 (udp_sk->myport);
    udp_hdr->dstPort  = intel16 (udp_sk->hisport);
    udp_hdr->checksum = 0;
    udp_hdr->length   = intel16 (sizeof(*udp_hdr)+len);

    memcpy (pkt+1, data, len);   /* copy 'data' to 'pkt->data[]' */
    memset (&tcp_phdr, 0, sizeof(tcp_phdr));
    tcp_phdr.src = intel (udp_sk->myaddr);
    tcp_phdr.dst = intel (udp_sk->hisaddr);

    if (ISOFF_SOCKMODE(*udp_sk, UDP_MODE_NOCHK)) {
        tcp_phdr.protocol = UDP_PROTO;
        tcp_phdr.length   = udp_hdr->length;
        tcp_phdr.checksum = checksum (udp_hdr, sizeof(*udp_hdr)+len);
        udp_hdr->checksum = ~checksum (&tcp_phdr, sizeof(tcp_phdr));
    }
    if (!IP_OUTPUT(ip, tcp_phdr.src, tcp_phdr.dst, UDP_PROTO, udp_sk->ttl,
                 (BYTE)_default_tos, 0, sizeof(*udp_hdr)+len, (sock_type *)udp_sk))
        return (-1);
    return (len);
}

/*
 * udp_read - read socket data to 'buf', does large buffering
 */
static int udp_read (udp_Socket *udp_sk, BYTE *buf, int maxlen)
{
    int len = udp_sk->rdatalen;

    if (maxlen < 0)
        maxlen = INT_MAX;

    if (len > 0) {
        if (len > maxlen)
            len = maxlen;
        if (len > 0) {
            if (buf)
                memcpy (buf, udp_sk->rdata, len);
            udp_sk->rdatalen -= len;
            if (udp_sk->rdatalen) {
                movmem (udp_sk->rdata+len, udp_sk->rdata, udp_sk->rdatalen);
            }
        }
    }
    return (len);
}

void _udp_cancel (const in_Header *ip, int type, const char *msg, DWORD gateway)
{
    WORD        srcPort, dstPort;
    int         len     = in_GetHdrLen (ip);
    BOOL        passive = FALSE;
    udp_Header *udp_hdr = (udp_Header*) ((BYTE*)ip + len);
    udp_Socket *_udp_sk;

    srcPort = intel16 (udp_hdr->srcPort);
    dstPort = intel16 (udp_hdr->dstPort);

    for (_udp_sk = _udp_allsocs; _udp_sk != NULL; _udp_sk = _udp_sk->next) { /* demux to active sockets */
        if (_udp_sk->hisport && dstPort == _udp_sk->hisport && srcPort == _udp_sk->myport &&
            intel(ip->destination) == _udp_sk->hisaddr) {
            break;
        }
    }

    if (_udp_sk == NULL) {     /* check passive sockets */
        passive = TRUE;
        for (_udp_sk = _udp_allsocs; _udp_sk != NULL; _udp_sk = _udp_sk->next) {
            if (_udp_sk->hisport == 0 && dstPort == _udp_sk->myport) {
                break;
            }
        }
    }

    if (_udp_sk != NULL) {
        if (_udp_sk->err_msg == NULL)
            _udp_sk->err_msg = msg;

        if (_udp_sk->sol_callb != NULL)     /* tell the socket layer about it */
            (*_udp_sk->sol_callb) ((sock_type *)_udp_sk, type);

        if (type == ICMP_REDIRECT && /* handle redirect on active sockets */
            !passive)
        {
            _ip_recursion = 1;
            _arp_resolve (gateway, &_udp_sk->hisethaddr, 1);
            _ip_recursion = 0;
        } else if (type != ICMP_TIMXCEED) {
            _udp_sk->rdatalen = 0;  /* will it be unthreaded ? */
            _udp_sk->ip_type  = 0;
            _udp_sk->err_msg  = _LANG("Port unreachable");
        }
    } else {
        /* tell the INADDR_ANY sockets about it
         */
        for (_udp_sk = _udp_allsocs; _udp_sk != NULL; _udp_sk = _udp_sk->next) {
            if (_udp_sk->sol_callb != NULL) {
                (*_udp_sk->sol_callb) ((sock_type *)_udp_sk, type);
            }
        }
    }
}

#if !defined(USE_UDP_ONLY)
void _tcp_cancel (const in_Header *ip, int type, const char *msg, DWORD gateway)
{
    tcp_Header *tcp_hdr = (tcp_Header*) ((BYTE*)ip + in_GetHdrLen (ip));
    tcp_Socket *_tcp_sk;

    WORD srcPort = intel16 (tcp_hdr->srcPort);
    WORD dstPort = intel16 (tcp_hdr->dstPort);

    /* demux to active sockets
     */
    for (_tcp_sk = _tcp_allsocs; _tcp_sk != NULL; _tcp_sk = _tcp_sk->next) {
        if (srcPort == _tcp_sk->myport && dstPort == _tcp_sk->hisport &&
            intel(ip->destination) == _tcp_sk->hisaddr)
        {
            switch (type) {
            case ICMP_TIMXCEED:
                if (_tcp_sk->ttl < 255)
                    _tcp_sk->ttl++;
                /* FALLTROUGH */
            case ICMP_UNREACH:
                if (_tcp_sk->stress++ > _tcp_sk->rigid && _tcp_sk->rigid < 100)  /* halt it */
                {
                    _tcp_sk->err_msg  = msg != NULL ? msg : _LANG("ICMP closed connection");
                    _tcp_sk->rdatalen = 0;
                    _tcp_sk->datalen  = 0;
                    _tcp_sk->unhappy  = FALSE;
                    tcp_abort (_tcp_sk);
                    break;
                }
                /* FALLTROUGH */
            case ICMP_SOURCEQUENCH:
                _tcp_sk->cwindow = 1;       /* slow-down tx-rate */
                _tcp_sk->wwindow = 1;
                _tcp_sk->vj_sa <<= 2;
                _tcp_sk->vj_sd <<= 2;
                _tcp_sk->rto   <<= 2;
                tcp_rtt_add (_tcp_sk, _tcp_sk->rto);
                break;

            case ICMP_REDIRECT:
                /* don't bother handling redirect if we're closing
                 */
                if (_tcp_sk->state >= tcp_StateFINWT1) {
                    if (_tcp_sk->err_msg == NULL) {
                        _tcp_sk->err_msg = msg;
                    }
                    break;
                }
                _ip_recursion = 1;
                _arp_resolve (gateway, &_tcp_sk->hisethaddr, 1);
                _ip_recursion = 0;
                break;
            case ICMP_PARAMPROB:
                tcp_abort (_tcp_sk);
                break;
            }
            if (_tcp_sk->sol_callb != NULL) {
                (*_tcp_sk->sol_callb) ((sock_type *)_tcp_sk, type);
            }
        }
    }
}

/*
 * tcp_read - read socket data to 'buf', does large buffering
 */
static int tcp_read (tcp_Socket *tcp_sk, BYTE *buf, int maxlen)
{
    int len = tcp_sk->rdatalen;

    if (maxlen < 0)
        maxlen = INT_MAX;

    if (len > 0) {
        if (len > maxlen)
            len = maxlen;
        if (len > 0) {
            if (buf)
                memcpy (buf, tcp_sk->rdata, len);
            tcp_sk->rdatalen -= len;
            if (tcp_sk->missed_seg[0] || tcp_sk->rdatalen > 0) {
                int diff = 0;
                if (tcp_sk->missed_seg[0] != 0) {
                    long ldiff = tcp_sk->missed_seg[1] - tcp_sk->acknum;
                    diff = abs ((int)ldiff);
                }
                movmem (tcp_sk->rdata + len, tcp_sk->rdata, tcp_sk->rdatalen + diff);
                TCP_SENDSOON (tcp_sk);     /* update the window soon */
            } else {
                tcp_upd_wind (tcp_sk, __LINE__);
            }
        }
    } else if (tcp_sk->state == tcp_StateCLOSWT) {
        _tcp_close (tcp_sk);
    }
    return (len);
}

/*
 * Write data to a connection.
 * Returns number of bytes written, == 0 when no room in socket-buffer
 */
static int tcp_write (tcp_Socket *tcp_sk, const BYTE *data, UINT len)
{
    UINT room;

    if (tcp_sk->state != tcp_StateESTAB)
        return (0);

    room = tcp_MaxTxBufSize - tcp_sk->datalen;
    if (len > room)
        len = room;
    if (len > 0) {
        int rc = 0;

        memcpy (tcp_sk->data + tcp_sk->datalen, data, len);
        tcp_sk->datalen  += len;
        tcp_sk->unhappy   = TRUE;    /* redundant because we have outstanding data */
        tcp_sk->datatimer = set_timeout (1000 * sock_data_timeout); /* EE 99.08.23 */

        if (ISON_SOCKMODE(*tcp_sk, TCP_MODE_LOCAL)) {  /* queue up data, flush on next write */
            SETOFF_SOCKMODE(*tcp_sk, TCP_MODE_LOCAL);
            return (len);
        }

        if (ISON_SOCKMODE(*tcp_sk, TCP_MODE_NONAGLE)) {
            rc = TCP_SEND (tcp_sk);
        } else {
            /* transmit if first data or reached MTU.
             * not true MTU, but better than nothing
             */
            if (tcp_sk->datalen == len || tcp_sk->datalen > tcp_sk->max_seg/2) {
                rc = TCP_SEND (tcp_sk);
            } else {
                rc = TCP_SENDSOON (tcp_sk);
            }
        }
        if (rc < 0) {
            return (-1);
        }
    }
    return (len);
}



/*
 *  Find the socket with the tripplet:
 *  DESTADDR=MYADDR,DESTPORT=MYPORT and ACKNUM=SEQNUM+1
 */
static tcp_Socket *tcp_findseq (const in_Header *ip, const tcp_Header *tcp_hdr)
{
    tcp_Socket *_tcp_sk;
    DWORD      dstHost = intel (ip->destination);
    DWORD      ackNum  = intel (tcp_hdr->acknum);
    WORD       dstPort = intel16 (tcp_hdr->dstPort);

    for (_tcp_sk = _tcp_allsocs; _tcp_sk != NULL; _tcp_sk = _tcp_sk->next) {
        if (_tcp_sk->hisport != 0      &&
            dstHost == _tcp_sk->myaddr &&
            dstPort == _tcp_sk->myport &&
            ackNum  == (_tcp_sk->seqnum+1)) {
            break;
        }
    }
    return (_tcp_sk);
}

static void tcp_sockreset (tcp_Socket *tcp_sk, int proxy)
{
    char *str = proxy ? "Proxy reset connection"
                    : "Remote reset connection";
    if (debug_on)
        outsnl (_LANG(str));

    tcp_sk->datalen = 0;  /* Flush Tx buffer */

    if (tcp_sk->state != tcp_StateCLOSED && tcp_sk->state != tcp_StateLASTACK)
        tcp_sk->rdatalen = 0;
    tcp_sk->err_msg = _LANG (str);
    tcp_sk->state   = tcp_StateCLOSED;
    tcp_sk->ip_type = 0;   /* 2001.1.18 - make it fail tcp_tick() */

#if defined(USE_BSD_FUNC)
    if (_tcp_find_hook != NULL) {
        Socket *socket = (*_tcp_find_hook) (tcp_sk);

        if (socket != NULL) { /* do a "read-wakeup" on the SOCK_STREAM socket */
            socket->so_state |= SS_CONN_REFUSED;
            if (socket->so_error == 0) {
                socket->so_error = ECONNRESET;
            }
        }
    }
#endif
    (void) _tcp_unthread (tcp_sk);
}

/*
 *  tcp_chksum - Check tcp header checksum
 */
static int tcp_chksum (const in_Header *ip, const tcp_Header *tcp_hdr, int len)
{
    tcp_PseudoHeader  tcp_phdr;

    memset (&tcp_phdr, 0, sizeof(tcp_phdr));
    tcp_phdr.src      = ip->source;
    tcp_phdr.dst      = ip->destination;
    tcp_phdr.protocol = TCP_PROTO;
    tcp_phdr.length   = intel16 (len);
    tcp_phdr.checksum = checksum (tcp_hdr, len);

    if (checksum(&tcp_phdr, sizeof(tcp_phdr)) == 0xFFFF)
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
static void tcp_rtt_wind (tcp_Socket *tcp_sk)
{
    DWORD timeout;

    /* update our retransmission stuff (Karn algorithm)
     */
    if (tcp_sk->karn_count == 2) {  /* Wake up from slow-start */
#if defined(USE_DEBUG)
        if (debug_on > 1) {
            (*_printf)("finally got it safely zapped from %u to ????\r\n",
                  tcp_sk->unacked);
        }
#endif
    } else if (tcp_sk->vj_last) { /* We expect an immediate response */
        long  dT;      /* time since last (re)transmission */
        DWORD now;

        chk_timeout (0UL);          /* update date/date_ms */
        now = (long) set_timeout (0);
        dT  = (long) get_timediff (now, tcp_sk->vj_last);

        if (dT >= 0) {      /* !!shouldn't this be '> 0' ? */
            dT -= (DWORD)(tcp_sk->vj_sa >> 3);
            tcp_sk->vj_sa += (int)dT;

            if (dT < 0)
                dT = -dT;

            dT -= (tcp_sk->vj_sd >> 2);
            tcp_sk->vj_sd += (int)dT;      /* vj_sd = RTTVAR, rtt variance */

            if (tcp_sk->vj_sa > MAX_VJSA)  /* vj_sa = SRTT, smoothed rtt */
                tcp_sk->vj_sa = MAX_VJSA;
            if (tcp_sk->vj_sd > MAX_VJSD) {
                tcp_sk->vj_sd = MAX_VJSD;
            }
        }

#if 0 /* !!to-do: use TimeStamp option values */
        if (tcp_sk->ts_echo && tcp_sk->ts_echo == tcp_sk->ts_sent) {
            dT = get_timediff (now, tcp_sk->ts_echo);
        } else {
            tcp_rtt_add (tcp_sk, tcp_sk->rto);
        }
#else
        /* only recompute RTT hence RTO after success
         */
        tcp_sk->rto = tcp_RTO_BASE + (((tcp_sk->vj_sa >> 2) + (tcp_sk->vj_sd)) >> 1);
        tcp_rtt_add (tcp_sk, tcp_sk->rto);
#endif


#if defined(USE_DEBUG)
        if (debug_on > 1) {
            (*_printf)("rto %u  sa %u  sd %u  cwindow %u"
                  "  wwindow %u  unacked %u\r\n",
                  tcp_sk->rto, tcp_sk->vj_sa, tcp_sk->vj_sd, tcp_sk->cwindow,
                  tcp_sk->wwindow, tcp_sk->unacked);
        }
#endif
    }

    tcp_sk->karn_count = 0;
    if (tcp_sk->wwindow != 255) {
        /* A. Iljasov (iljasov@oduurl.ru) suggested this pre-increment
         */
        if (++tcp_sk->wwindow >= tcp_sk->cwindow) {
            if (tcp_sk->cwindow != 255)
                tcp_sk->cwindow++;
            tcp_sk->wwindow = 0;    /* mdurkin -- added 95.05.02 */
        }
    }


    /* Restart RTT timer or postpone retransmission
     * based on calculated RTO. Make sure date/date_ms variables
     * are updated close to midnight.
     */
    chk_timeout (0UL);
    timeout = set_timeout (tcp_sk->rto + tcp_RTO_ADD);

    if (tcp_sk->rtt_time == 0UL || cmp_timers(tcp_sk->rtt_time, timeout) < 0)
        tcp_sk->rtt_time = timeout;

    tcp_sk->datatimer = 0UL; /* resetting tx-timer, EE 99.08.23 */
}

/*
 *  tcp_upd_wind - Check if receive window needs an update.
 */
static void tcp_upd_wind (tcp_Socket *tcp_sk, unsigned line)
{
    UINT winfree = tcp_sk->maxrdatalen - tcp_sk->rdatalen;

    if (winfree < tcp_sk->max_seg/2) {
        _tcp_send (tcp_sk, __FILE__, line);  /* update window now */
    }
}

/*
 * TCP option routines.
 * Note: Each of these MUST add multiple of 4 byte of options.
 *
 * Insert MSS option.
 */
static __inline int tcp_opt_maxsegment (tcp_Socket *tcp_sk, BYTE *opt)
{
    *opt++ = TCPOPT_MAXSEG;    /* option: MAXSEG, length, mss */
    *opt++ = 4;
    *(WORD*) opt = intel16 (tcp_sk->max_seg);
    return (4);
}

/*
 * Insert TimeStamp option.
 */
static __inline int tcp_opt_timestamp (tcp_Socket *tcp_sk, BYTE *opt,
                                       DWORD ts_val, DWORD ts_echo)
{
    *opt++ = TCPOPT_NOP;     /* NOP,NOP,TIMESTAMP,length,TSval,TSecho */
    *opt++ = TCPOPT_NOP;
    *opt++ = TCPOPT_TIMESTAMP;
    *opt++ = 10;
    *(DWORD*) opt = intel (ts_val);  opt += sizeof(ts_val);
    *(DWORD*) opt = intel (ts_echo); opt += sizeof(ts_echo);
    tcp_sk->ts_sent = ts_val;       /* remember ts_sent */
    return (12);
}

#ifdef NOT_USED
/*
 * Pad options to multiple of 4 bytes.
 */
static __inline int tcp_opt_padding (BYTE *opt, int len)
{
    int i, pad;

    pad = len % 4;
    for (i = 0; i < pad; i++) {
        *opt++ = TCPOPT_NOP;
    }
    return (i);
}

static __inline int tcp_opt_winscale (tcp_Socket *tcp_sk, BYTE *opt)
{
    *opt++ = TCPOPT_WINDOW;    /* option: WINDOW,length,wscale */
    *opt++ = 4;
    *(WORD*) opt = intel16 (tcp_sk->send_wscale);
    return (4);
}

static __inline int tcp_opt_sack_ok (tcp_Socket *tcp_sk, BYTE *opt)
{
    *opt++ = TCPOPT_SACKOK;
    *opt++ = 2;
    *opt++ = TCPOPT_NOP;
    *opt++ = TCPOPT_NOP;
    return (4);
}

static __inline int tcp_opt_sack (tcp_Socket *tcp_sk, BYTE *opt,
                                  struct SACK_list *sack)
{
    int i, len = 2 + 8 * sack->num_blk;

    *opt++ = TCPOPT_SACK;       /* option: SACK,length,left,right,.. */
    *opt++ = len;
    for (i = 0; i < sack->num_blk; i++) {
        *(DWORD*) opt = intel (sack->list[i].left_edge);
        opt += sizeof(DWORD);
        *(DWORD*) opt = intel (sack->list[i].right_edge);
        opt += sizeof(DWORD);
    }
    for (i = 0; i < len % 4; i++) {
        *opt++ = TCPOPT_NOP;
    }
    return (len + i);
}
#endif

/*
 * tcp_do_options - Add TCP options to output segment
 */
static __inline int tcp_do_options (tcp_Socket *tcp_sk, BYTE *opt, BOOL is_syn)
{
    DWORD now = set_timeout (0);
    int   len = 0;

    if (is_syn && !(tcp_sk->locflags & LF_NOOPT)) {
        len += tcp_opt_maxsegment (tcp_sk, opt);

        if (tcp_sk->locflags & LF_REQ_TSTMP)
            len += tcp_opt_timestamp (tcp_sk, opt+len, now, 0UL);
#if 0
        if (tcp_sk->locflags & LF_REQ_SCALE)
            len += tcp_opt_winscale (tcp_sk, opt+len);

        if (tcp_opt_sackok) {
            len += tcp_opt_sack_ok (tcp_sk, opt+len);
        }
#endif
    } else if (!is_syn) {
        /* We got a TS option in a previous SYN-ACK or SYN.
         * Send it back unless we send a RST or disallow options.
         * A Win98 host will have 0 in 's->ts_recent'
         */
        if ((tcp_sk->flags & tcp_FlagRST) == 0 && tcp_opt_timstmp &&
            (tcp_sk->locflags & (LF_RCVD_TSTMP|LF_NOOPT)) == LF_RCVD_TSTMP)
        {
            len += tcp_opt_timestamp (tcp_sk, opt, now, tcp_sk->ts_recent);
            tcp_sk->locflags &= ~LF_RCVD_TSTMP;  /* don't echo this again */
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
int _tcp_send (tcp_Socket *tcp_sk, char *file, unsigned line)
{
    #include <sys/packon.h>
    struct tcp_pkt {
         in_Header  in;
         tcp_Header tcp_hdr;
       } *pkt;
    #include <sys/packoff.h>

    tcp_PseudoHeader  tcp_phdr;

    BOOL         tx_ok;
    BYTE        *data;            /* where to copy user's data */
    mac_address *dst;
    in_Header   *ip;
    tcp_Header  *tcp_hdr;
    int          sendtotlen = 0;  /* count of data length we've sent */
    int          senddatalen;     /* how much data in this segment */
    int          startdata;       /* where data starts in tx-buffer */
    int          sendtotdata;     /* total amount of data to send */
    int          tcp_len;         /* total length of TCP segment */
    int          opt_len;         /* total length of TCP options */
    int          pkt_num;         /* 0..s->cwindow-1 */
    int          rtt;

    tcp_sk->recent = 0;

    dst  = (_pktserial ? NULL : &tcp_sk->hisethaddr);
    pkt  = (struct tcp_pkt*) _eth_formatpacket (dst, IP_TYPE);
    ip   = &pkt->in;
    tcp_hdr = &pkt->tcp_hdr;
    data = (BYTE*) (tcp_hdr+1);

    if (tcp_sk->karn_count == 2) { /* doing slow-start */
        sendtotdata = min (tcp_sk->datalen, tcp_sk->window);
        startdata = 0;
    } else {
        /* Morten Terstrup <MorTer@dk-online.dk> found this signed bug
         */
        int size = min (tcp_sk->datalen, tcp_sk->window);
        sendtotdata = size - tcp_sk->unacked;
        if (sendtotdata < 0)
            sendtotdata = 0;
        startdata = tcp_sk->unacked;
    }

    /* step through our packets
     */
    for (pkt_num = 0; pkt_num < tcp_sk->cwindow; pkt_num++) {
        /* make tcp header
         */
        tcp_hdr->srcPort  = intel16 (tcp_sk->myport);
        tcp_hdr->dstPort  = intel16 (tcp_sk->hisport);
        tcp_hdr->seqnum   = intel (tcp_sk->seqnum + startdata); /* unacked - no longer sendtotlen */
        tcp_hdr->acknum   = intel (tcp_sk->acknum);

        tcp_hdr->window   = intel16 (tcp_sk->maxrdatalen - tcp_sk->rdatalen);
        tcp_hdr->flags    = tcp_sk->flags;
        tcp_hdr->unused   = 0;
        tcp_hdr->checksum = 0;
        tcp_hdr->urgent   = 0;

        /* Insert any TCP options after header
         */
        if (pkt_num == 0 && (tcp_sk->flags & (tcp_FlagSYN|tcp_FlagACK)) == tcp_FlagSYN) {
            opt_len = tcp_do_options (tcp_sk, data, TRUE);
            senddatalen = 0;   /* no data, only options */
        } else {
            int data_free;

            opt_len = tcp_do_options (tcp_sk, data, FALSE);
            if ((data_free = tcp_sk->max_seg - opt_len) < 0)
                data_free = 0;
            senddatalen = min (sendtotdata, data_free);
        }

        tcp_len = sizeof(*tcp_hdr) + opt_len;
        data   += opt_len;
        tcp_hdr->offset = tcp_len / 4;

        if (senddatalen > 0) {       /* non-SYN packets with data */
            tcp_len += senddatalen;
            if (tcp_sk->queuelen) {
                memcpy (data, tcp_sk->queue+startdata, senddatalen);
            } else {
                memcpy (data, tcp_sk->data + startdata, senddatalen);
            }
        }

        if (tcp_sk->locflags & LF_NOPUSH)
            tcp_hdr->flags &= ~tcp_FlagPUSH;

        /* make tcp header check-sum
         */
        memset (&tcp_phdr, 0, sizeof(tcp_phdr));
        tcp_phdr.src      = intel (tcp_sk->myaddr);
        tcp_phdr.dst      = intel (tcp_sk->hisaddr);
        tcp_phdr.protocol = TCP_PROTO;
        tcp_phdr.length   = intel16 (tcp_len);
        tcp_phdr.checksum = checksum (tcp_hdr, tcp_len);
        tcp_hdr->checksum = ~checksum (&tcp_phdr, sizeof(tcp_phdr));

        tx_ok = _ip_output (ip, tcp_phdr.src, tcp_phdr.dst, TCP_PROTO,
                        tcp_sk->ttl, tcp_sk->tos, 0, tcp_len, (sock_type *)tcp_sk, file, line) != 0;
        if (!tx_ok) {
            TCP_SENDSOON (tcp_sk);
            return (-1);
        }

        /* do next packet
         */
        if (senddatalen > 0) {
            sendtotlen  += senddatalen;
            startdata   += senddatalen;
            sendtotdata -= senddatalen;
        }
        if (sendtotdata <= 0) {
            break;
        }
    }

    tcp_sk->unacked = startdata;

#if defined(USE_DEBUG)
    if (debug_on > 1)
        (*_printf)(" Sent %u (win %u) bytes in %u (cwin %u) packets with "
                "(%u) unacked  SEQ %lu  line %u\r\n",
                sendtotlen, tcp_sk->window, pkt_num, tcp_sk->cwindow,
                tcp_sk->unacked, tcp_sk->seqnum, line);
#endif

    tcp_sk->vj_last = 0UL;
    if (tcp_sk->karn_count == 2) {
        if (tcp_sk->rto) {
            tcp_sk->rto = (tcp_sk->rto * 3) / 2;  /* increase by 50% */
        } else {
            tcp_sk->rto = 2*tcp_RTO_ADD;     /* !!was 4 tick */
        }
    } else {
        /* vj_last nonzero if we expect an immediate response
         */
        if (tcp_sk->unhappy || tcp_sk->datalen)
            tcp_sk->vj_last = set_timeout (0);
        tcp_sk->karn_count = 0;
    }

    rtt = tcp_sk->rto + tcp_RTO_ADD;

    tcp_sk->rtt_time = set_timeout (rtt);

    if (sendtotlen > 0)
        tcp_sk->rtt_lasttran = tcp_sk->rtt_time;

    return (sendtotlen);
}


/*
 * Force the peer to send us a segment by sending a keep-alive packet:
 *   <SEQ=SND.UNA-1><ACK=RCV.NXT><CTL=ACK>
 */
int sock_keepalive (sock_type *sk)
{
    DWORD       ack, seq;
    BYTE        kc;
    int         datalen;

    if (sk->u.ip_type != TCP_PROTO)
        return (0);

    ack     = sk->tcp.acknum;
    seq     = sk->tcp.seqnum;
    kc      = sk->tcp.karn_count;
    datalen = sk->tcp.datalen;

    sk->tcp.acknum     = sk->tcp.seqnum;
    sk->tcp.seqnum     = sk->tcp.unacked - 1;
    sk->tcp.flags      = tcp_FlagACK;
    sk->tcp.karn_count = 2;
    sk->tcp.datalen    = 0;
    (void) TCP_SEND (&sk->tcp);

    sk->tcp.acknum     = ack;
    sk->tcp.seqnum     = seq;
    sk->tcp.karn_count = kc;
    sk->tcp.datalen    = datalen;

    STAT (tcpstats.tcps_keepprobe++);
    STAT (tcpstats.tcps_keeptimeo++);
    return (1);
}
#endif  /* !USE_UDP_ONLY */


/*
 * sock_mode - set binary or ascii - affects sock_gets, sock_dataready
 *           - set udp checksums
 */
WORD sock_mode (sock_type *sk, WORD mode)
{
    if (sk->u.ip_type == TCP_PROTO || sk->u.ip_type == UDP_PROTO) {
        SETOFF_SOCKMODE(sk->u, SOCK_MODE_MASK);
        SETON_SOCKMODE(sk->u, mode & SOCK_MODE_MASK);
        return (sk->u.sockmode & SOCK_MODE_MASK);
    }
    return (0);
}

/*
 * sock_yield - enable user defined yield function
 */
int sock_yield (sock_type *sk, void (*fn)(void))
{
    if (sk != NULL) {
        switch (sk->u.ip_type) {
        case UDP_PROTO:
#if !defined(USE_UDP_ONLY)
        case TCP_PROTO:
#endif
            sk->u.usr_yield = fn;
            return (0);
        }
    }
    system_yield = fn;
    return (0);
}


void sock_abort (sock_type *sk)
{
    switch (sk->u.ip_type) {
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        tcp_abort (&sk->tcp);
        break;
#endif
    case UDP_PROTO:
        udp_close (&sk->udp);
        break;
    case IP_TYPE:
        sk->u.ip_type = 0;
        sk->raw.used = 0;
        break;
    }
}

#if defined(USE_BSD_FUNC)
/*
 * Read data from a raw-socket. Don't copy IP-header to buf.
 */
static int raw_read (raw_Socket *raw_sk, BYTE *buf, int maxlen)
{
    int len = 0;

    if (raw_sk->used) {
        int   hlen = in_GetHdrLen (&raw_sk->ip);
        BYTE *data = (BYTE*)&raw_sk->ip + hlen;

        len = intel16 (raw_sk->ip.length) - hlen;
        len = min (len, maxlen);
        memcpy (buf, data, len);
        raw_sk->used = 0;
    }
    return (len);
}
#endif

/*
 * sock_read - read a socket with maximum 'maxlen' bytes
 *           - busywaits until 'buf' is full (and call 's->usr_yield')
 *           - returns count ( <= maxlen) also when connection gets closed
 */
int sock_read (sock_type *sk, BYTE *buf, int maxlen)
{
    int count = 0;

    do {
        int len = 0;
        int raw = 0;

        switch (sk->u.ip_type) {
#if !defined(USE_UDP_ONLY)
        case TCP_PROTO:
            len = tcp_read (&sk->tcp, buf, maxlen);
            break;
#endif
        case UDP_PROTO:
            len = udp_read (&sk->udp, buf, maxlen);
            break;
#if defined(USE_BSD_FUNC)
        case IP_TYPE:
            raw = TRUE;
            len = raw_read (&sk->raw, buf, maxlen);
            break;
#endif
        }

        if (len < 1) {
            if (!tcp_tick(sk)) {
                return (count);
            }
        } else {
            count  += len;
            buf    += len;
            maxlen -= len;
        }
        if (maxlen > 0 && !raw && sk->tcp.usr_yield) { /* yield only when room */
            (*sk->tcp.usr_yield)();                    /* 99.07.01 EE */
        }
    } while (maxlen);
    return (count);
}

/*
 * sock_fastread - read a socket with maximum 'len' bytes
 *               - does not busywait until buffer is full
 */
int sock_fastread (sock_type *sk, BYTE *buf, int len)
{
    if (sk->u.ip_type == UDP_PROTO)
        return udp_read (&sk->udp, buf, len);

#if !defined(USE_UDP_ONLY)
    if (sk->u.ip_type == TCP_PROTO || sk->tcp.rdatalen > 0)
        return tcp_read (&sk->tcp, buf, len);
#endif

#if defined(USE_BSD_FUNC)
    if (sk->u.ip_type == IP_TYPE)
        return raw_read (&sk->raw, buf, len);
#endif

    return (-1);
}


/*
 * sock_write - writes data and returns length written
 *            - send with PUSH-bit (flush data)
 *            - repeatedly calls s->usr_yield
 */
int sock_write (sock_type *sk, const BYTE *data, int len)
{
    int chunk   = len;
    int written = 0;

    while (chunk > 0) {
        switch (sk->u.ip_type) {
#if !defined(USE_UDP_ONLY)
        case TCP_PROTO:
            sk->tcp.flags |= tcp_FlagPUSH;
            written = tcp_write (&sk->tcp, data, chunk);
            break;
#endif
        case UDP_PROTO:
            chunk = min (mtu - sizeof(in_Header) - sizeof(udp_Header), chunk);
            written = udp_write (&sk->udp, data, chunk);
            break;
#if defined(USE_BSD_FUNC)
        case IP_TYPE:
            return (0);     /* not supported yet */
#endif
        default:            /* EE 99.06.14 */
            return (0);
        }

        if (written < 0) {
            sk->udp.err_msg = _LANG("Tx Error");
            return (0);
        }
        data  += written;
        chunk -= written;

        if (sk->udp.usr_yield)
            (*sk->udp.usr_yield)();

        if (!tcp_tick(sk)) {
            return (0);
        }
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
int sock_fastwrite (sock_type *sk, const BYTE *data, int len)
{
    switch (sk->u.ip_type) {
    case UDP_PROTO:
        len = min (mtu - sizeof(in_Header) - sizeof(udp_Header), len);
        len = udp_write (&sk->udp, data, len);
        return (len < 0 ? 0 : len);
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        len = tcp_write (&sk->tcp, data, len);
        return (len < 0 ? 0 : len);
#endif
    }
    return (0);
}


int sock_enqueue (sock_type *sk, const BYTE *data, int len)
{
    int written;
    int total;

    if (len <= 0)
        return (0);

    switch(sk->u.ip_type) {
    case UDP_PROTO:
        written = 0;
        total   = 0;
        do {
            len = min (mtu - sizeof(in_Header) - sizeof(udp_Header), len);
            written = udp_write (&sk->udp, data, len);
            if (written < 0) {
                sk->udp.err_msg = _LANG("Tx Error");
                break;
            }
            data  += written;
            len   -= written;
            total += written;
        } while (len > 0);
        return (total);
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        sk->tcp.queue    = data;
        sk->tcp.queuelen = len;
        sk->tcp.datalen  = len;
        return TCP_SEND (&sk->tcp);
#endif
    }
    return (0);
}

#if !defined(USE_UDP_ONLY)
void sock_noflush (sock_type *sk)
{
    if (sk->u.ip_type == TCP_PROTO) {
        sk->tcp.flags &= ~tcp_FlagPUSH;
        SETON_SOCKMODE(sk->tcp, TCP_MODE_LOCAL);
    }
}

/*
 * sock_flush - Send pending TCP data
 */
void sock_flush (sock_type *sk)
{
    if (sk->u.ip_type == TCP_PROTO) {
        SETOFF_SOCKMODE(sk->tcp, TCP_MODE_LOCAL);
        if (sk->tcp.datalen > 0) {
            sk->tcp.flags |= tcp_FlagPUSH;
            if (sk->tcp.unacked == 0) { /* !! S. Lawson - only if data not moving */
                (void) TCP_SEND (&sk->tcp);
            }
        }
    }
}

/*
 * sock_flushnext - cause next transmission to have a flush
 */
void sock_flushnext (sock_type *sk)
{
    if (sk->u.ip_type == TCP_PROTO) {
        sk->tcp.flags |= tcp_FlagPUSH;
        SETOFF_SOCKMODE(sk->tcp, TCP_MODE_LOCAL);
    }
}
#endif  /* !USE_UDP_ONLY */


int sock_close (sock_type *sk)
{
    switch (sk->u.ip_type) {
    case UDP_PROTO:
        udp_close (&sk->udp);
        break;
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        _tcp_close (&sk->tcp);
        (void) tcp_tick (sk);
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

static void tcp_rtt_add (tcp_Socket *tcp_sk, UINT rto)
{
    struct tcp_rtt *rtt;
    DWORD  addr = tcp_sk->hisaddr;

    if (~addr & ~sin_mask)  /* 0.0.0.0 or broadcast addresses? */
        return;

    rtt = &rtt_cache [(WORD)addr % RTTCACHE];

    /* Cache-slot is vacant or we're updating previous RTO
     * for same peer
     */
    if (!rtt->ip || rtt->ip == addr) {
        rtt->ip  = addr;
        rtt->rto = rto;
        STAT (tcpstats.tcps_cachedrtt++);
    }
}

static UINT tcp_rtt_get (tcp_Socket *tcp_sk)
{
    struct tcp_rtt *rtt = &rtt_cache [(WORD)tcp_sk->hisaddr % RTTCACHE];

    if (tcp_sk->hisaddr && rtt->ip == tcp_sk->hisaddr && rtt->rto > 0) {
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

static void tcp_rtt_clr (tcp_Socket *tcp_sk)
{
    struct tcp_rtt *rtt = &rtt_cache [(WORD)tcp_sk->hisaddr % RTTCACHE];

    if (tcp_sk->hisaddr && rtt->ip == tcp_sk->hisaddr) {
        rtt->rto = 0;
        rtt->ip  = 0;
    }
}
#endif /* !USE_UDP_ONLY */

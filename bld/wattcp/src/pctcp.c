
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
#include "pcbuf.h"
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
int   (*_tcp_syn_hook) (sock_type **skp) = NULL;
Socket *(*_tcp_find_hook) (const sock_type *sk) = NULL;
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

sock_type *_udp_allsocs = NULL;   /* list of udp-sockets */


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

sock_type *_tcp_allsocs  = NULL; /* list of tcp-sockets */

#endif

static void (*system_yield)(void) = NULL;


/*
 *  Since UDP is stateless, simply reclaim the local-port and
 *  unthread the socket from the list.
 */
static void udp_close (sock_type *sk)
{
    sock_type *_sk, *prev;

    for (_sk = prev = _udp_allsocs; _sk != NULL; prev = _sk, _sk = _sk->next) {
        if (_sk != sk)
            continue;

        (void) reuse_localport (_sk->udp.myport);

        if (_sk == _udp_allsocs) {
            _udp_allsocs = _sk->next;
        } else {
            prev->next = _sk->next;
        }
        if (_sk->udp.err_msg == NULL) {
            _sk->udp.err_msg = _LANG("UDP Close called");
        }
    }
}

/*
 * Passive open: listen for a connection on a particular port
 */
int udp_listen (udp_Socket *udp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler)
{
    sock_type *sk = (sock_type *)udp_sk;

    udp_close (sk);
    watt_largecheck (sk, sizeof(*udp_sk), __FILE__, __LINE__);
    memset (sk, 0, sizeof(*udp_sk));

    sk->udp.rx_data       = sk->udp.rx_buf;
    sk->udp.rx_maxdatalen = udp_MaxBufSize;
    sk->udp.ip_type      = UDP_PROTO;
    sk->udp.myport       = findfreeport (lport, 0); /* get a nonzero port val */
    sk->udp.hisport      = port;
    sk->udp.hisaddr      = ina;
    sk->udp.ttl          = _default_ttl;
    sk->udp.protoHandler = handler;
    sk->udp.usr_yield    = system_yield;
    sk->udp.safetysig    = SAFETYUDP;               /* insert into chain */

    sk->next     = _udp_allsocs;
    _udp_allsocs = sk;

    return (1);
}

/*
 * Active open: open a connection on a particular port
 */
int udp_open (udp_Socket *udp_sk, WORD lport, DWORD ip, WORD port, ProtoHandler handler)
{
    BOOL bcast = (ip == (DWORD)-1) || (~ip & ~sin_mask) == 0;
    sock_type *sk = (sock_type *)udp_sk;

    udp_close (sk);
    watt_largecheck (sk, sizeof(*udp_sk), __FILE__, __LINE__);
    memset (sk, 0, sizeof(*udp_sk));

    if (ip - my_ip_addr <= multihomes)
        return (0);

    sk->udp.rx_data       = sk->udp.rx_buf;
    sk->udp.rx_maxdatalen = udp_MaxBufSize;
    sk->udp.ip_type      = UDP_PROTO;
    sk->udp.myport       = findfreeport (lport, 0);
    sk->udp.myaddr       = my_ip_addr;
    sk->udp.ttl          = _default_ttl;

    if (bcast || !ip) {    /* check for broadcast */
        memset (sk->udp.hisethaddr, 0xFF, sizeof(eth_address));
        if (!ip) {
            ip = (DWORD)-1;   /* make sk->udp.hisaddr = 255.255.255.255 */
        }
#if defined(USE_MULTICAST)
    } else if (is_multicast(ip)) { /* check for multicast */
        multi_to_eth (ip, (BYTE*)&sk->udp.hisethaddr[0]);
        sk->udp.ttl = 1;     /* so we don't send worldwide as default */
#endif
    } else if (!_arp_resolve(ip, &sk->udp.hisethaddr, 0)) {
        return (0);
    }

    sk->udp.hisaddr      = ip;
    sk->udp.hisport      = port;
    sk->udp.protoHandler = handler;
    sk->udp.usr_yield    = system_yield;
    sk->udp.safetysig    = SAFETYUDP;

    sk->next     = _udp_allsocs;
    _udp_allsocs = sk;
    return (1);
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

static void tcp_rtt_add (sock_type *sk, UINT rto)
{
    struct tcp_rtt *rtt;
    DWORD  addr = sk->tcp.hisaddr;

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

static UINT tcp_rtt_get (sock_type *sk)
{
    struct tcp_rtt *rtt = &rtt_cache [(WORD)sk->tcp.hisaddr % RTTCACHE];

    if (sk->tcp.hisaddr && rtt->ip == sk->tcp.hisaddr && rtt->rto > 0) {
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

static void tcp_rtt_clr (sock_type *sk)
{
    struct tcp_rtt *rtt = &rtt_cache [(WORD)sk->tcp.hisaddr % RTTCACHE];

    if (sk->tcp.hisaddr && rtt->ip == sk->tcp.hisaddr) {
        rtt->rto = 0;
        rtt->ip  = 0;
    }
}

/*
 *  tcp_upd_wind - Check if receive window needs an update.
 */
static void tcp_upd_wind (sock_type *sk, unsigned line)
{
    UINT winfree = sk->tcp.rx_maxdatalen - sk->tcp.rx_datalen;

    if (winfree < sk->tcp.max_seg/2) {
        _tcp_send (sk, __FILE__, line);  /* update window now */
    }
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
 *  Find the socket with the tripplet:
 *  DESTADDR=MYADDR,DESTPORT=MYPORT and ACKNUM=SEQNUM+1
 */
static sock_type *tcp_findseq (const in_Header *ip, const tcp_Header *tcp_hdr)
{
    sock_type  *sk;
    DWORD      dstHost = intel (ip->destination);
    DWORD      ackNum  = intel (tcp_hdr->acknum);
    WORD       dstPort = intel16 (tcp_hdr->dstPort);

    for (sk = _tcp_allsocs; sk != NULL; sk = sk->next) {
        if (sk->tcp.hisport != 0      &&
            dstHost == sk->tcp.myaddr &&
            dstPort == sk->tcp.myport &&
            ackNum == (sk->tcp.seqnum + 1)) {
            break;
        }
    }
    return (sk);
}

static void tcp_sockreset (sock_type *sk, int proxy)
{
    char *str = proxy ? "Proxy reset connection"
                    : "Remote reset connection";
    if (debug_on)
        outsnl (_LANG(str));

    sk->tcp.tx_datalen = 0;     /* Flush Tx buffer */

    if (sk->tcp.state != tcp_StateCLOSED && sk->tcp.state != tcp_StateLASTACK)
        sk->tcp.rx_datalen = 0;
    sk->tcp.err_msg = _LANG (str);
    sk->tcp.state   = tcp_StateCLOSED;
    sk->tcp.ip_type = 0;        /* 2001.1.18 - make it fail tcp_tick() */

#if defined(USE_BSD_FUNC)
    if (_tcp_find_hook != NULL) {
        Socket *socket = (*_tcp_find_hook) (sk);

        if (socket != NULL) {   /* do a "read-wakeup" on the SOCK_STREAM socket */
            socket->so_state |= SS_CONN_REFUSED;
            if (socket->so_error == 0) {
                socket->so_error = ECONNRESET;
            }
        }
    }
#endif
    (void) _tcp_unthread (sk);
}

/*
 *  tcp_rtt_wind - Update retransmission timer, VJ algorithm
 *  and tcp windows.
 */
static void tcp_rtt_wind (sock_type *sk)
{
    DWORD timeout;

    /* update our retransmission stuff (Karn algorithm)
     */
    if (sk->tcp.karn_count == 2) {  /* Wake up from slow-start */
#if defined(USE_DEBUG)
        if (debug_on > 1) {
            (*_printf)("finally got it safely zapped from %u to ????\r\n",
                  sk->tcp.unacked);
        }
#endif
    } else if (sk->tcp.vj_last) {   /* We expect an immediate response */
        long  dT;      /* time since last (re)transmission */
        DWORD now;

        chk_timeout (0UL);          /* update date/date_ms */
        now = (long) set_timeout (0);
        dT  = (long) get_timediff (now, sk->tcp.vj_last);

        if (dT >= 0) {      /* !!shouldn't this be '> 0' ? */
            dT -= (DWORD)(sk->tcp.vj_sa >> 3);
            sk->tcp.vj_sa += (int)dT;

            if (dT < 0)
                dT = -dT;

            dT -= (sk->tcp.vj_sd >> 2);
            sk->tcp.vj_sd += (int)dT;      /* vj_sd = RTTVAR, rtt variance */

            if (sk->tcp.vj_sa > MAX_VJSA)  /* vj_sa = SRTT, smoothed rtt */
                sk->tcp.vj_sa = MAX_VJSA;
            if (sk->tcp.vj_sd > MAX_VJSD) {
                sk->tcp.vj_sd = MAX_VJSD;
            }
        }

#if 0 /* !!to-do: use TimeStamp option values */
        if (sk->tcp.ts_echo && sk->tcp.ts_echo == sk->tcp.ts_sent) {
            dT = get_timediff (now, sk->tcp.ts_echo);
        } else {
            tcp_rtt_add (sk, sk->tcp.rto);
        }
#else
        /* only recompute RTT hence RTO after success
         */
        sk->tcp.rto = tcp_RTO_BASE + (((sk->tcp.vj_sa >> 2) + (sk->tcp.vj_sd)) >> 1);
        tcp_rtt_add (sk, sk->tcp.rto);
#endif


#if defined(USE_DEBUG)
        if (debug_on > 1) {
            (*_printf)("rto %u  sa %u  sd %u  cwindow %u"
                  "  wwindow %u  unacked %u\r\n",
                  sk->tcp.rto, sk->tcp.vj_sa, sk->tcp.vj_sd, sk->tcp.cwindow,
                  sk->tcp.wwindow, sk->tcp.unacked);
        }
#endif
    }

    sk->tcp.karn_count = 0;
    if (sk->tcp.wwindow != 255) {
        /* A. Iljasov (iljasov@oduurl.ru) suggested this pre-increment
         */
        if (++sk->tcp.wwindow >= sk->tcp.cwindow) {
            if (sk->tcp.cwindow != 255)
                sk->tcp.cwindow++;
            sk->tcp.wwindow = 0;    /* mdurkin -- added 95.05.02 */
        }
    }


    /* Restart RTT timer or postpone retransmission
     * based on calculated RTO. Make sure date/date_ms variables
     * are updated close to midnight.
     */
    chk_timeout (0UL);
    timeout = set_timeout (sk->tcp.rto + tcp_RTO_ADD);

    if (sk->tcp.rtt_time == 0UL || cmp_timers(sk->tcp.rtt_time, timeout) < 0)
        sk->tcp.rtt_time = timeout;

    sk->tcp.datatimer = 0UL; /* resetting tx-timer, EE 99.08.23 */
}

/*
 * Actively open a TCP connection to a particular destination.
 *  - 0 on error
 *
 * 'lport' is local port to associate with the connection.
 * 'rport' is remote port for same connection
 */
int tcp_open (tcp_Socket *tcp_sk, WORD lport, DWORD ina, WORD rport, ProtoHandler handler)
{
    sock_type *sk = (sock_type *)tcp_sk;
    UINT rtt;

    watt_largecheck (sk, sizeof(*tcp_sk), __FILE__, __LINE__);
    (void) _tcp_unthread (sk);       /* just in case not totally closed */
    memset (sk, 0, sizeof(*tcp_sk));

    if ((ina - my_ip_addr <= multihomes) || is_multicast(ina))
        return (0);

    if (!_arp_resolve(ina, &sk->tcp.hisethaddr, 0))
        return (0);

    sk->tcp.rx_data       = sk->tcp.rx_buf;
    sk->tcp.rx_maxdatalen = tcp_MaxBufSize;
    sk->tcp.tx_data       = sk->tcp.tx_buf;
    sk->tcp.tx_maxdatalen = tcp_MaxTxBufSize;
    sk->tcp.ip_type      = TCP_PROTO;
    sk->tcp.max_seg      = mss;        /* to-do !!: use mss from setsockopt() */
    sk->tcp.state        = tcp_StateSYNSENT;
    sk->tcp.timeout      = set_timeout (tcp_LONGTIMEOUT);

    /* to-do !!: use TCP_NODELAY set in setsockopt()
     */
    if (!tcp_nagle)
        SETON_SOCKMODE(sk->tcp, TCP_MODE_NONAGLE);

    sk->tcp.cwindow      = 1;
    sk->tcp.wwindow      = 0;                       /* slow start VJ algorithm */
    sk->tcp.vj_sa        = INIT_VJSA;
    sk->tcp.rto          = tcp_OPEN_TO;             /* added 14-Dec 1999, GV   */
    sk->tcp.myaddr       = my_ip_addr;
    sk->tcp.myport       = findfreeport (lport, 1); /* get a nonzero port val  */
    sk->tcp.locflags     = LF_LINGER;               /* close via TIMEWT state  */
    if (tcp_opt_timstmp)
        sk->tcp.locflags |= LF_REQ_TSTMP;           /* use timestamp option */

    sk->tcp.ttl          = _default_ttl;
    sk->tcp.hisaddr      = ina;
    sk->tcp.hisport      = rport;
    sk->tcp.seqnum       = INIT_SEQ();
    sk->tcp.flags        = tcp_FlagSYN;
    sk->tcp.unhappy      = TRUE;
    sk->tcp.protoHandler = handler;
    sk->tcp.usr_yield    = system_yield;

    sk->tcp.safetysig    = SAFETYTCP;               /* marker signatures */
    sk->tcp.safetytcp    = SAFETYTCP;

    sk->next     = _tcp_allsocs;                    /* insert into chain */
    _tcp_allsocs = sk;

    (void) TCP_SEND (sk);                           /* send opening SYN */

    /* find previous RTT replacing RTT set in tcp_send() above
     */
    if ((rtt = tcp_rtt_get(sk)) > 0) {
        sk->tcp.rtt_time = set_timeout (rtt);
    } else {
        sk->tcp.rtt_time = set_timeout (tcp_OPEN_TO);
    }
    return (1);
}

/*
 * Passive open: listen for a connection on a particular port
 */
int tcp_listen (tcp_Socket *tcp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler, WORD timeout)
{
    sock_type *sk = (sock_type *)tcp_sk;

    watt_largecheck (sk, sizeof(*tcp_sk), __FILE__, __LINE__);
    (void) _tcp_unthread (sk);    /* just in case not totally closed */
    memset (sk, 0, sizeof(*tcp_sk));

    if (is_multicast(ina))
        return (0);

    sk->tcp.rx_data       = sk->tcp.rx_buf;
    sk->tcp.rx_maxdatalen = tcp_MaxBufSize;
    sk->tcp.tx_data       = sk->tcp.tx_buf;
    sk->tcp.tx_maxdatalen = tcp_MaxTxBufSize;
    sk->tcp.ip_type      = TCP_PROTO;
    sk->tcp.max_seg      = mss;             /* to-do !!: use mss from setsockopt() */
    sk->tcp.cwindow      = 1;
    sk->tcp.wwindow      = 0;               /* slow start VJ algorithm */
    sk->tcp.vj_sa        = INIT_VJSA;
    sk->tcp.state        = tcp_StateLISTEN;
    sk->tcp.locflags     = LF_LINGER;

    sk->tcp.myport       = findfreeport (lport, 0);
    sk->tcp.hisport      = port;
    sk->tcp.hisaddr      = ina;
    sk->tcp.seqnum       = INIT_SEQ();
    sk->tcp.unhappy      = FALSE;
    sk->tcp.ttl          = _default_ttl;
    sk->tcp.protoHandler = handler;
    sk->tcp.usr_yield    = system_yield;
    sk->tcp.safetysig    = SAFETYTCP;       /* marker signatures */
    sk->tcp.safetytcp    = SAFETYTCP;

    sk->next     = _tcp_allsocs;            /* insert into chain */
    _tcp_allsocs = sk;

    if (timeout != 0)
        sk->tcp.timeout = set_timeout (1000 * timeout);
    return (1);
}

/*
 * Reuse local port now if not owned by a STREAM-socket.
 * Otherwise let socket daemon free local port when linger period
 * expires. We don't care about rapid reuse of local ports connected
 * to DGRAM-sockets.
 */
static void maybe_reuse_lport (sock_type *sk)
{
#if defined(USE_BSD_FUNC)
    if (_tcp_find_hook == NULL || (*_tcp_find_hook)(sk) == NULL) {
#endif
        reuse_localport (sk->tcp.myport);
#if defined(USE_BSD_FUNC)
    }
#endif
}

/*
 *  Send a FIN on a particular port -- only works if it is open.
 *  Must still allow receives
 */
void _tcp_close (sock_type *sk)
{
    if (sk->u.ip_type != TCP_PROTO)
        return;

    if (sk->tcp.state == tcp_StateESTAB ||
        sk->tcp.state == tcp_StateESTCL ||
        sk->tcp.state == tcp_StateSYNREC)
    {
        if (sk->tcp.tx_datalen) {   /* must first flush all Tx data */
            sk->tcp.flags |= (tcp_FlagPUSH | tcp_FlagACK);
            if (sk->tcp.state < tcp_StateESTCL) {
                sk->tcp.state = tcp_StateESTCL;
                TCP_SENDSOON (sk);
            }
        } else { /* really closing */
            sk->tcp.flags = (tcp_FlagACK | tcp_FlagFIN);
            if (sk->tcp.err_msg == NULL)
                sk->tcp.err_msg = _LANG("Connection closed normally");

            sk->tcp.state    = tcp_StateFINWT1;
            sk->tcp.timeout  = set_timeout (tcp_TIMEOUT);
            sk->tcp.rtt_time = 0UL;   /* stop RTT timer */
            (void) TCP_SEND (sk);
        }
        sk->tcp.unhappy = TRUE;
    } else if (sk->tcp.state == tcp_StateCLOSWT) {
        /* need to ACK the FIN and get on with it
         */
        sk->tcp.timeout = set_timeout (tcp_LASTACK_TIME); /* Added AGW 6 Jan 2001 */
        sk->tcp.state   = tcp_StateLASTACK;
        sk->tcp.flags  |= tcp_FlagFIN;
        (void) TCP_SEND (sk);
        sk->tcp.unhappy = TRUE;
    } else if (sk->tcp.state == tcp_StateSYNSENT) { /* unlink failed connection */
        sk->tcp.state = tcp_StateCLOSED;
        maybe_reuse_lport (sk);
        (void) _tcp_unthread (sk);
    }
}

/*
 * Abort a tcp connection
 */
void tcp_abort (tcp_Socket *tcp_sk)
{
    sock_type *sk = (sock_type *)tcp_sk;

    if (sk->tcp.err_msg == NULL)
        sk->tcp.err_msg = _LANG("TCP Abort");

    if (sk->tcp.state != tcp_StateLISTEN && sk->tcp.state != tcp_StateCLOSED) {
        sk->tcp.flags   = (tcp_FlagRST | tcp_FlagACK);
        sk->tcp.unhappy = TRUE;
        if (sk->tcp.state <= tcp_StateSYNREC) {
            sk->tcp.rtt_time = 0UL; /* Stop RTT timer */
            tcp_rtt_clr (sk);   /* Clear cached RTT */
        }
        (void) TCP_SEND (sk);
    }
    sk->tcp.unhappy = FALSE;
    sk->tcp.tx_datalen = 0;     /* discard Tx buffer, but not Rx buffer */
    sk->tcp.ip_type = 0;

    maybe_reuse_lport (sk);
    (void) _tcp_unthread (sk);
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
int _tcp_sendsoon (sock_type *sk, char *file, unsigned line)
{
    DWORD timeout;

    if (sk->u.ip_type != TCP_PROTO)
        return (0);

    if (sk->tcp.state >= tcp_StateCLOSWT) {
        timeout = set_timeout (tcp_CLOSE_TO);
    } else {
        timeout = set_timeout (tcp_RTO_BASE);
    }

    if (sk->tcp.rto <= tcp_RTO_BASE && sk->tcp.recent == 0 &&
      cmp_timers(sk->tcp.rtt_time, timeout) <= 0)
    {                         /* !! was == */
        int rc;

        sk->tcp.karn_count = 0;
        rc = _tcp_send (sk, file, line);
        sk->tcp.recent = 1;
        return (rc);
    }

    if ((sk->tcp.unhappy || sk->tcp.tx_datalen > 0 || sk->tcp.karn_count == 1) &&
      (sk->tcp.rtt_time && cmp_timers(sk->tcp.rtt_time, timeout) < 0))
        return (0);

    if (sk->tcp.state == tcp_StateSYNSENT) { /* relaxed in SYNSENT state */
        sk->tcp.rtt_time = set_timeout (tcp_OPEN_TO);
    } else {
        sk->tcp.rtt_time = set_timeout (tcp_RTO_BASE + (sk->tcp.rto >> 4));
    }

    sk->tcp.karn_count = 1;

    return (0);
}

/*
 * Unthread a socket from the tcp socket list, if it's there
 */
sock_type *_tcp_unthread (sock_type *sk)
{
    sock_type  *_sk, *prev;
    sock_type  *next = NULL;

    for (_sk = prev = _tcp_allsocs; _sk != NULL; prev = _sk, _sk = _sk->next) {
        if (_sk != sk)
            continue;

        if (_sk == _tcp_allsocs) {
            _tcp_allsocs = _sk->next;
        } else {
            prev->next = _sk->next;
        }
        next = _sk->next;
    }

    if (_sk->tcp.rx_datalen == 0 || (_sk->tcp.state > tcp_StateESTCL))
        _sk->u.ip_type = 0;              /* fail further I/O */
    _sk->tcp.state = tcp_StateCLOSED;    /* tcp_tick needs this */

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
static sock_type *tcp_handler (const in_Header *ip, BOOL broadcast)
{
    tcp_Header *tcp_hdr;
    sock_type  *sk;
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
    for (sk = _tcp_allsocs; sk != NULL; sk = sk->next) {
        if (sk->tcp.safetysig != SAFETYTCP || sk->tcp.safetytcp != SAFETYTCP) {
            outsnl (_LANG("tcp-socket error in tcp_handler()"));
            DEBUG_RX (sk, ip);
            return (NULL);
        }

        if (sk->tcp.hisport            &&   /* not a listening socket */
            destin  == sk->tcp.myaddr  &&   /* addressed to my IP */
            source  == sk->tcp.hisaddr &&   /* and from my peer address */
            dstPort == sk->tcp.myport  &&   /* addressed to my local port */
            srcPort == sk->tcp.hisport)     /* and from correct remote port */
        break;
    }

    if (sk == NULL && (flags & tcp_FlagSYN)) {
        /* demux to passive (listening) sockets, must be a new session
         */
        for (sk = _tcp_allsocs; sk != NULL; sk = sk->next) {
            if (sk->tcp.hisport == 0 &&        /* =0, listening socket */
                dstPort    == sk->tcp.myport)  /* addressed to my local port */
            {
                sk->tcp.hisport = srcPort;     /* remember his IP-address */
                sk->tcp.hisaddr = source;      /*   and src-port */
                sk->tcp.myaddr  = destin;      /* socket is now active */
                break;
            }
        }
    }

    DEBUG_RX (sk, ip);

    if (sk == NULL) {
        if (!(flags & tcp_FlagRST)) {           /* don't answer RST */
            TCP_RESET (NULL, ip, tcp_hdr);
        } else if ((flags & tcp_FlagACK) &&     /* got ACK,RST   */
            (sk = tcp_findseq(ip, tcp_hdr)) != NULL) { /* ACK = SEQ + 1 */
            tcp_sockreset (sk, 1);        /* e.g. a firewall is sending */
        }
        STAT (tcpstats.tcps_drops++);           /* RST for server on inside   */
        return (NULL);
    }

    /* Restart idle-timer
     */
    if (sock_inactive && !(sk->tcp.locflags & LF_RCVTIMEO))
        sk->tcp.inactive_to = set_timeout (1000 * sock_inactive);

    if (flags & tcp_FlagRST) {      /* got reset code */
        tcp_sockreset (sk, 0);
        return (NULL);
    }

    tcp_rtt_wind (sk);        /* update retrans timer, windows etc. */

    if (_tcp_fsm(&sk, ip) &&        /* do input tcp state-machine */
        sk->tcp.unhappy)            /* if "unhappy", retransmit soon */
        TCP_SENDSOON (sk);

    return (sk);
}
#endif  /* !USE_UDP_ONLY */


/*
 * Handler for incoming udp packets.
 */
static sock_type *udp_handler (const in_Header *ip, BOOL broadcast)
{
    sock_type         *sk;
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
    for (sk = _udp_allsocs; sk != NULL; sk = sk->next) {
        if (sk->udp.safetysig != SAFETYUDP) {
            outsnl (_LANG("udp-socket error in udp_handler()"));
            DEBUG_RX (sk, ip);
            return (NULL);
        }
        if (!ip_bcast                    &&
            (sk->udp.hisport != 0)       &&
            (dstPort == sk->udp.myport)  &&
            (srcPort == sk->udp.hisport) &&
            ((destin & sin_mask) == (sk->udp.myaddr & sin_mask)) &&
            (intel(ip->source) == sk->udp.hisaddr)) {
            break;
        }
    }

    if (sk == NULL) {
        /* demux to passive sockets
         */
        for (sk = _udp_allsocs; sk != NULL; sk = sk->next) {
            if ((sk->udp.hisaddr == 0 || sk->udp.hisaddr == 0xFFFFFFFFUL) &&
                dstPort == sk->udp.myport)
            {
                if (sk->udp.hisaddr == 0) {
                    sk->udp.hisaddr = intel (ip->source);
                    sk->udp.hisport = srcPort;
                    SET_PEER_MAC_ADDR (&sk->udp, ip);

                    /* take on value of expected destination
                     * unless it is broadcast
                     */
                    if (!ip_bcast) {
                        sk->udp.myaddr = destin;
                    }
                }
                break;
            }
        }
    }

    DEBUG_RX (sk, ip);

#if defined(USE_MULTICAST)
    if (sk == NULL) {
        /* demux to multicast sockets
         */
        for (sk = _udp_allsocs; sk != NULL; sk = sk->next) {
            if (sk->udp.hisport != 0      &&
                sk->udp.hisaddr == destin &&
                dstPort == sk->udp.myport &&
                is_multicast(destin)) {
                break;
            }
        }
    }
#endif

    if (sk == NULL) {
        /* Demux to broadcast sockets.
         */
        for (sk = _udp_allsocs; sk != NULL; sk = sk->next) {
            if (sk->udp.hisaddr == (DWORD)-1 && dstPort == sk->udp.myport) {
                break;
            }
        }
    }

    if (sk == NULL) {
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

    if (udp_hdr->checksum && ISOFF_SOCKMODE(sk->udp, UDP_MODE_NOCHK)) {
        if (checksum(&tcp_phdr, sizeof(tcp_phdr)) != 0xFFFF) {
            if (debug_on)
                outsnl (_LANG("bad udp checksum"));
            STAT (udpstats.udps_badsum++);
            return (sk);
        }
    }

    /* Process user data. 0-byte probe is legal for s->protoHandler.
     */
    {
        BYTE *data = (BYTE*)(udp_hdr+1);

        len -= sizeof(*udp_hdr);
        if (sk->udp.protoHandler != NULL) {
            (*sk->udp.protoHandler) (sk, data, len, &tcp_phdr, udp_hdr);
        /* save first received packet rather than latest */
        } else if (len > 0 && sk->udp.rx_datalen == 0) {
            if (len > sk->udp.rx_maxdatalen) {   /* truncate data :-( */
                len = sk->udp.rx_maxdatalen;
                STAT (udpstats.udps_fullsock++);
            }
            /* Might overwrite previous data! But hey, this is UDP..
             */
            memcpy (sk->udp.rx_data, data, len);
            sk->udp.rx_datalen = len;
        }
    }
    return (sk);
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
    sock_type *sk, *next;

    static DWORD timeout = 0UL;

    /* do this once per tcp_RETRAN_TIME
     */
    if (!force && timeout && !chk_timeout(timeout))
        return;

    timeout = set_timeout (tcp_RETRAN_TIME);

    for (sk = _tcp_allsocs; sk != NULL; sk = next) {
        next = sk->next;

        /* possible to be closed but still queued
         */
        if (sk->tcp.state == tcp_StateCLOSED) {
            if (sk->tcp.rx_datalen == 0) {
                maybe_reuse_lport (sk);
                next = _tcp_unthread (sk);
            }
            continue;
        }

        if (sk->tcp.tx_datalen > 0 || sk->tcp.unhappy || sk->tcp.karn_count == 1) {
            if (chk_timeout(sk->tcp.rtt_time)) { /* retransmission timeout */
                sk->tcp.rtt_time = 0UL;           /* stop RTT timer */

#if defined(USE_DEBUG)
                if (debug_on > 1) {
                    (*_printf) ("regular retran TO set unacked back to 0 from %u\r\n",
                      sk->tcp.unacked);
                }
#endif
                /* strategy handles closed windows.  JD + EE
                 */
                if (sk->tcp.window == 0 && sk->tcp.karn_count == 2)
                    sk->tcp.window = 1;

                if (sk->tcp.karn_count == 0) {
                    /* Simple "Slow start" algorithm:
                     * Use the backed off RTO - implied, no code necessary.
                     * Reduce the congestion window by 25%
                     */
                    unsigned cwindow = ((unsigned)(sk->tcp.cwindow + 1) * 3) >> 2;

                    sk->tcp.cwindow = cwindow;
                    if (sk->tcp.cwindow == 0)
                        sk->tcp.cwindow = 1;

                    sk->tcp.wwindow = 0;       /* dup ACK counter ? */
                    /* sk->tcp.snd_ssthresh = sk->tcp.cwindow * sk->tcp.max_seg; */ /* !!to-do */

                    /* if really did timeout
                     */
                    sk->tcp.karn_count = 2;
                    sk->tcp.unacked    = 0;
                }
                if (sk->tcp.tx_datalen > 0)
                    sk->tcp.flags |= (tcp_FlagPUSH | tcp_FlagACK);

                if (sk->tcp.unhappy) {
                    STAT (tcpstats.tcps_rexmttimeo++);
                } else if (sk->tcp.flags & tcp_FlagACK) {
                    STAT (tcpstats.tcps_delack++);
                }

                (void) TCP_SEND(sk);
            }

            /* handle inactive tcp timeouts (not sending data)
             */
            if (chk_timeout(sk->tcp.datatimer)) { /* EE 99.08.23 */
                sk->tcp.err_msg = _LANG("Connection timed out - no data sent");
                tcp_abort (&sk->tcp);
            }
        }  /* end of retransmission strategy */


        /* handle inactive tcp timeouts (not received anything)
         */
        if (chk_timeout(sk->tcp.inactive_to)) {
            /* this baby has timed out. Don't do this again.
             */
            sk->tcp.inactive_to = 0UL;
            sk->tcp.err_msg = _LANG("Connection timed out - no activity");
            sock_close (sk);
        } else if (chk_timeout(sk->tcp.timeout)) {
            if (sk->tcp.state == tcp_StateTIMEWT) {
                sk->tcp.state = tcp_StateCLOSED;
                break;
            } else if (sk->tcp.state != tcp_StateESTAB && sk->tcp.state != tcp_StateESTCL) {
                sk->tcp.err_msg = _LANG("Timeout, aborting");
                tcp_abort (&sk->tcp);
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
            sk = tcp_handler (ip, broadcast);
        } else {
#endif
            sk = udp_handler (ip, broadcast);
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
        if ((sk->tcp.state == tcp_StateCLOSED) && (sk->tcp.rx_datalen == 0)) {
            (void) _tcp_unthread (sk);
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
static int udp_write (sock_type *sk, const BYTE *data, int len)
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
    dst = (_pktserial ? NULL : &sk->udp.hisethaddr);
    pkt = (struct udp_pkt*) _eth_formatpacket (dst, IP_TYPE);
    ip  = &pkt->in;
    udp_hdr = &pkt->udp_hdr;

    /* build udp header
     */
    udp_hdr->srcPort  = intel16 (sk->udp.myport);
    udp_hdr->dstPort  = intel16 (sk->udp.hisport);
    udp_hdr->checksum = 0;
    udp_hdr->length   = intel16 (sizeof(*udp_hdr)+len);

    memcpy (pkt+1, data, len);   /* copy 'data' to 'pkt->data[]' */
    memset (&tcp_phdr, 0, sizeof(tcp_phdr));
    tcp_phdr.src = intel (sk->udp.myaddr);
    tcp_phdr.dst = intel (sk->udp.hisaddr);

    if (ISOFF_SOCKMODE(sk->udp, UDP_MODE_NOCHK)) {
        tcp_phdr.protocol = UDP_PROTO;
        tcp_phdr.length   = udp_hdr->length;
        tcp_phdr.checksum = checksum (udp_hdr, sizeof(*udp_hdr)+len);
        udp_hdr->checksum = ~checksum (&tcp_phdr, sizeof(tcp_phdr));
    }
    if (!IP_OUTPUT(ip, tcp_phdr.src, tcp_phdr.dst, UDP_PROTO, sk->udp.ttl,
                 (BYTE)_default_tos, 0, sizeof(*udp_hdr)+len, sk))
        return (-1);
    return (len);
}

/*
 * udp_read - read socket data to 'buf', does large buffering
 */
static int udp_read (sock_type *sk, BYTE *buf, int maxlen)
{
    int len = sk->udp.rx_datalen;

    if (maxlen < 0)
        maxlen = INT_MAX;

    if (len > 0) {
        if (len > maxlen)
            len = maxlen;
        if (len > 0) {
            if (buf)
                memcpy (buf, sk->udp.rx_data, len);
            sk->udp.rx_datalen -= len;
            if (sk->udp.rx_datalen) {
                movmem (sk->udp.rx_data + len, sk->udp.rx_data, sk->udp.rx_datalen);
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
    sock_type   *sk;

    srcPort = intel16 (udp_hdr->srcPort);
    dstPort = intel16 (udp_hdr->dstPort);

    for (sk = _udp_allsocs; sk != NULL; sk = sk->next) { /* demux to active sockets */
        if (sk->udp.hisport && dstPort == sk->udp.hisport && srcPort == sk->udp.myport &&
            intel(ip->destination) == sk->udp.hisaddr) {
            break;
        }
    }

    if (sk == NULL) {     /* check passive sockets */
        passive = TRUE;
        for (sk = _udp_allsocs; sk != NULL; sk = sk->next) {
            if (sk->udp.hisport == 0 && dstPort == sk->udp.myport) {
                break;
            }
        }
    }

    if (sk != NULL) {
        if (sk->udp.err_msg == NULL)
            sk->udp.err_msg = msg;

        if (sk->udp.sol_callb != NULL)     /* tell the socket layer about it */
            (*sk->udp.sol_callb) (sk, type);

        if (type == ICMP_REDIRECT && /* handle redirect on active sockets */
            !passive)
        {
            _ip_recursion = 1;
            _arp_resolve (gateway, &sk->udp.hisethaddr, 1);
            _ip_recursion = 0;
        } else if (type != ICMP_TIMXCEED) {
            sk->udp.rx_datalen = 0;  /* will it be unthreaded ? */
            sk->udp.ip_type = 0;
            sk->udp.err_msg = _LANG("Port unreachable");
        }
    } else {
        /* tell the INADDR_ANY sockets about it
         */
        for (sk = _udp_allsocs; sk != NULL; sk = sk->next) {
            if (sk->udp.sol_callb != NULL) {
                (*sk->udp.sol_callb) (sk, type);
            }
        }
    }
}

#if !defined(USE_UDP_ONLY)
void _tcp_cancel (const in_Header *ip, int type, const char *msg, DWORD gateway)
{
    tcp_Header *tcp_hdr = (tcp_Header*) ((BYTE*)ip + in_GetHdrLen (ip));
    sock_type  *sk;

    WORD srcPort = intel16 (tcp_hdr->srcPort);
    WORD dstPort = intel16 (tcp_hdr->dstPort);

    /* demux to active sockets
     */
    for (sk = _tcp_allsocs; sk != NULL; sk = sk->next) {
        if (srcPort == sk->tcp.myport && dstPort == sk->tcp.hisport &&
            intel(ip->destination) == sk->tcp.hisaddr)
        {
            switch (type) {
            case ICMP_TIMXCEED:
                if (sk->tcp.ttl < 255)
                    sk->tcp.ttl++;
                /* FALLTROUGH */
            case ICMP_UNREACH:
                if (sk->tcp.stress++ > sk->tcp.rigid && sk->tcp.rigid < 100)  /* halt it */
                {
                    sk->tcp.err_msg  = msg != NULL ? msg : _LANG("ICMP closed connection");
                    sk->tcp.rx_datalen = 0;
                    sk->tcp.tx_datalen = 0;
                    sk->tcp.unhappy = FALSE;
                    tcp_abort (&sk->tcp);
                    break;
                }
                /* FALLTROUGH */
            case ICMP_SOURCEQUENCH:
                sk->tcp.cwindow = 1;       /* slow-down tx-rate */
                sk->tcp.wwindow = 1;
                sk->tcp.vj_sa <<= 2;
                sk->tcp.vj_sd <<= 2;
                sk->tcp.rto   <<= 2;
                tcp_rtt_add (sk, sk->tcp.rto);
                break;

            case ICMP_REDIRECT:
                /* don't bother handling redirect if we're closing
                 */
                if (sk->tcp.state >= tcp_StateFINWT1) {
                    if (sk->tcp.err_msg == NULL) {
                        sk->tcp.err_msg = msg;
                    }
                    break;
                }
                _ip_recursion = 1;
                _arp_resolve (gateway, &sk->tcp.hisethaddr, 1);
                _ip_recursion = 0;
                break;
            case ICMP_PARAMPROB:
                tcp_abort (&sk->tcp);
                break;
            }
            if (sk->tcp.sol_callb != NULL) {
                (*sk->tcp.sol_callb) (sk, type);
            }
        }
    }
}

/*
 * tcp_read - read socket data to 'buf', does large buffering
 */
static int tcp_read (sock_type *sk, BYTE *buf, int maxlen)
{
    int len = sk->tcp.rx_datalen;

    if (maxlen < 0)
        maxlen = INT_MAX;

    if (len > 0) {
        if (len > maxlen)
            len = maxlen;
        if (len > 0) {
            if (buf)
                memcpy (buf, sk->tcp.rx_data, len);
            sk->tcp.rx_datalen -= len;
            if (sk->tcp.missed_seg[0] || sk->tcp.rx_datalen > 0) {
                int diff = 0;
                if (sk->tcp.missed_seg[0] != 0) {
                    long ldiff = sk->tcp.missed_seg[1] - sk->tcp.acknum;
                    diff = abs ((int)ldiff);
                }
                movmem (sk->tcp.rx_data + len, sk->tcp.rx_data, sk->tcp.rx_datalen + diff);
                TCP_SENDSOON (sk);     /* update the window soon */
            } else {
                tcp_upd_wind (sk, __LINE__);
            }
        }
    } else if (sk->tcp.state == tcp_StateCLOSWT) {
        _tcp_close (sk);
    }
    return (len);
}

/*
 * Write data to a connection.
 * Returns number of bytes written, == 0 when no room in socket-buffer
 */
static int tcp_write (sock_type *sk, const BYTE *data, UINT len)
{
    UINT room;

    if (sk->tcp.state != tcp_StateESTAB)
        return (0);

    room = sk->tcp.tx_maxdatalen - sk->tcp.tx_datalen;
    if (len > room)
        len = room;
    if (len > 0) {
        int rc = 0;

        memcpy (sk->tcp.tx_data + sk->tcp.tx_datalen, data, len);
        sk->tcp.tx_datalen += len;
        sk->tcp.unhappy   = TRUE;    /* redundant because we have outstanding data */
        sk->tcp.datatimer = set_timeout (1000 * sock_data_timeout); /* EE 99.08.23 */

        if (ISON_SOCKMODE(sk->tcp, TCP_MODE_LOCAL)) {  /* queue up data, flush on next write */
            SETOFF_SOCKMODE(sk->tcp, TCP_MODE_LOCAL);
            return (len);
        }

        if (ISON_SOCKMODE(sk->tcp, TCP_MODE_NONAGLE)) {
            rc = TCP_SEND (sk);
        } else {
            /* transmit if first data or reached MTU.
             * not true MTU, but better than nothing
             */
            if (sk->tcp.tx_datalen == len || sk->tcp.tx_datalen > sk->tcp.max_seg/2) {
                rc = TCP_SEND (sk);
            } else {
                rc = TCP_SENDSOON (sk);
            }
        }
        if (rc < 0) {
            return (-1);
        }
    }
    return (len);
}

/*
 * TCP option routines.
 * Note: Each of these MUST add multiple of 4 byte of options.
 *
 * Insert MSS option.
 */
static __inline int tcp_opt_maxsegment (sock_type *sk, BYTE *opt)
{
    *opt++ = TCPOPT_MAXSEG;    /* option: MAXSEG, length, mss */
    *opt++ = 4;
    *(WORD*) opt = intel16 (sk->tcp.max_seg);
    return (4);
}

/*
 * Insert TimeStamp option.
 */
static __inline int tcp_opt_timestamp (sock_type *sk, BYTE *opt,
                                       DWORD ts_val, DWORD ts_echo)
{
    *opt++ = TCPOPT_NOP;     /* NOP,NOP,TIMESTAMP,length,TSval,TSecho */
    *opt++ = TCPOPT_NOP;
    *opt++ = TCPOPT_TIMESTAMP;
    *opt++ = 10;
    *(DWORD*) opt = intel (ts_val);  opt += sizeof(ts_val);
    *(DWORD*) opt = intel (ts_echo); opt += sizeof(ts_echo);
    sk->tcp.ts_sent = ts_val;       /* remember ts_sent */
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

static __inline int tcp_opt_winscale (sock_type *sk, BYTE *opt)
{
    *opt++ = TCPOPT_WINDOW;    /* option: WINDOW,length,wscale */
    *opt++ = 3;
    *opt++ = sk->tcp.tx_wscale;
    *opt++ = TCPOPT_NOP;
    return (4);
}

static __inline int tcp_opt_sack_ok (sock_type *sk, BYTE *opt)
{
    *opt++ = TCPOPT_SACKOK;
    *opt++ = 2;
    *opt++ = TCPOPT_NOP;
    *opt++ = TCPOPT_NOP;
    return (4);
}

static __inline int tcp_opt_sack (sock_type *sk, BYTE *opt,
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
static __inline int tcp_do_options (sock_type *sk, BYTE *opt, BOOL is_syn)
{
    DWORD now = set_timeout (0);
    int   len = 0;

    if (is_syn && !(sk->tcp.locflags & LF_NOOPT)) {
        len += tcp_opt_maxsegment (sk, opt);

        if (sk->tcp.locflags & LF_REQ_TSTMP)
            len += tcp_opt_timestamp (sk, opt+len, now, 0UL);
#if 0
        if (sk->tcp.locflags & LF_REQ_SCALE)
            len += tcp_opt_winscale (sk, opt+len);

        if (tcp_opt_sackok) {
            len += tcp_opt_sack_ok (sk, opt+len);
        }
#endif
    } else if (!is_syn) {
        /* We got a TS option in a previous SYN-ACK or SYN.
         * Send it back unless we send a RST or disallow options.
         * A Win98 host will have 0 in 's->ts_recent'
         */
        if ((sk->tcp.flags & tcp_FlagRST) == 0 && tcp_opt_timstmp &&
            (sk->tcp.locflags & (LF_RCVD_TSTMP|LF_NOOPT)) == LF_RCVD_TSTMP)
        {
            len += tcp_opt_timestamp (sk, opt, now, sk->tcp.ts_recent);
            sk->tcp.locflags &= ~LF_RCVD_TSTMP;  /* don't echo this again */
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
int _tcp_send (sock_type *sk, char *file, unsigned line)
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

    sk->tcp.recent = 0;

    dst  = (_pktserial ? NULL : &sk->tcp.hisethaddr);
    pkt  = (struct tcp_pkt*) _eth_formatpacket (dst, IP_TYPE);
    ip   = &pkt->in;
    tcp_hdr = &pkt->tcp_hdr;
    data = (BYTE*) (tcp_hdr+1);

    if (sk->tcp.karn_count == 2) { /* doing slow-start */
        sendtotdata = min (sk->tcp.tx_datalen, sk->tcp.window);
        startdata = 0;
    } else {
        /* Morten Terstrup <MorTer@dk-online.dk> found this signed bug
         */
        int size = min (sk->tcp.tx_datalen, sk->tcp.window);
        sendtotdata = size - sk->tcp.unacked;
        if (sendtotdata < 0)
            sendtotdata = 0;
        startdata = sk->tcp.unacked;
    }

    /* step through our packets
     */
    for (pkt_num = 0; pkt_num < sk->tcp.cwindow; pkt_num++) {
        /* make tcp header
         */
        tcp_hdr->srcPort  = intel16 (sk->tcp.myport);
        tcp_hdr->dstPort  = intel16 (sk->tcp.hisport);
        tcp_hdr->seqnum   = intel (sk->tcp.seqnum + startdata); /* unacked - no longer sendtotlen */
        tcp_hdr->acknum   = intel (sk->tcp.acknum);

        tcp_hdr->window   = intel16 (sk->tcp.rx_maxdatalen - sk->tcp.rx_datalen);
        tcp_hdr->flags    = sk->tcp.flags;
        tcp_hdr->unused   = 0;
        tcp_hdr->checksum = 0;
        tcp_hdr->urgent   = 0;

        /* Insert any TCP options after header
         */
        if (pkt_num == 0 && (sk->tcp.flags & (tcp_FlagSYN|tcp_FlagACK)) == tcp_FlagSYN) {
            opt_len = tcp_do_options (sk, data, TRUE);
            senddatalen = 0;   /* no data, only options */
        } else {
            int data_free;

            opt_len = tcp_do_options (sk, data, FALSE);
            if ((data_free = sk->tcp.max_seg - opt_len) < 0)
                data_free = 0;
            senddatalen = min (sendtotdata, data_free);
        }

        tcp_len = sizeof(*tcp_hdr) + opt_len;
        data   += opt_len;
        tcp_hdr->offset = tcp_len / 4;

        if (senddatalen > 0) {       /* non-SYN packets with data */
            tcp_len += senddatalen;
            if (sk->tcp.queuelen) {
                memcpy (data, sk->tcp.queue + startdata, senddatalen);
            } else {
                memcpy (data, sk->tcp.tx_data + startdata, senddatalen);
            }
        }

        if (sk->tcp.locflags & LF_NOPUSH)
            tcp_hdr->flags &= ~tcp_FlagPUSH;

        /* make tcp header check-sum
         */
        memset (&tcp_phdr, 0, sizeof(tcp_phdr));
        tcp_phdr.src      = intel (sk->tcp.myaddr);
        tcp_phdr.dst      = intel (sk->tcp.hisaddr);
        tcp_phdr.protocol = TCP_PROTO;
        tcp_phdr.length   = intel16 (tcp_len);
        tcp_phdr.checksum = checksum (tcp_hdr, tcp_len);
        tcp_hdr->checksum = ~checksum (&tcp_phdr, sizeof(tcp_phdr));

        tx_ok = _ip_output (ip, tcp_phdr.src, tcp_phdr.dst, TCP_PROTO,
                        sk->tcp.ttl, sk->tcp.tos, 0, tcp_len, sk, file, line) != 0;
        if (!tx_ok) {
            TCP_SENDSOON (sk);
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

    sk->tcp.unacked = startdata;

#if defined(USE_DEBUG)
    if (debug_on > 1)
        (*_printf)(" Sent %u (win %u) bytes in %u (cwin %u) packets with "
                "(%u) unacked  SEQ %lu  line %u\r\n",
                sendtotlen, sk->tcp.window, pkt_num, sk->tcp.cwindow,
                sk->tcp.unacked, sk->tcp.seqnum, line);
#endif

    sk->tcp.vj_last = 0UL;
    if (sk->tcp.karn_count == 2) {
        if (sk->tcp.rto) {
            sk->tcp.rto = (sk->tcp.rto * 3) / 2;  /* increase by 50% */
        } else {
            sk->tcp.rto = 2*tcp_RTO_ADD;     /* !!was 4 tick */
        }
    } else {
        /* vj_last nonzero if we expect an immediate response
         */
        if (sk->tcp.unhappy || sk->tcp.tx_datalen)
            sk->tcp.vj_last = set_timeout (0);
        sk->tcp.karn_count = 0;
    }

    rtt = sk->tcp.rto + tcp_RTO_ADD;

    sk->tcp.rtt_time = set_timeout (rtt);

    if (sendtotlen > 0)
        sk->tcp.rtt_lasttran = sk->tcp.rtt_time;

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
    UINT        datalen;

    if (sk->u.ip_type != TCP_PROTO)
        return (0);

    ack     = sk->tcp.acknum;
    seq     = sk->tcp.seqnum;
    kc      = sk->tcp.karn_count;
    datalen = sk->tcp.tx_datalen;

    sk->tcp.acknum     = sk->tcp.seqnum;
    sk->tcp.seqnum     = sk->tcp.unacked - 1;
    sk->tcp.flags      = tcp_FlagACK;
    sk->tcp.karn_count = 2;
    sk->tcp.tx_datalen  = 0;
    (void) TCP_SEND (sk);

    sk->tcp.acknum     = ack;
    sk->tcp.seqnum     = seq;
    sk->tcp.karn_count = kc;
    sk->tcp.tx_datalen  = datalen;

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
        udp_close (sk);
        break;
    case IP_TYPE:
        sk->u.ip_type = 0;
        sk->raw.used = 0;
        break;
    }
}

int sock_preread (sock_type *sk, BYTE *buf, int len)
{
    int count;

    switch (_chk_socket(sk)) {
    case VALID_TCP:
    case VALID_UDP:
        count = sk->u.rx_datalen;
        if (count > 0) {
            if (count > len)
                count = len;
            if (buf != NULL) {
                memcpy (buf, sk->u.rx_data, count);
            }
        }
        return (count);
    }
    return (0);  /* Raw-sockets use fixed buffer */
}

#if defined(USE_BSD_FUNC)
/*
 * Read data from a raw-socket. Don't copy IP-header to buf.
 */
static int raw_read (sock_type *sk, BYTE *buf, int maxlen)
{
    int len = 0;

    if (sk->raw.used) {
        int   hlen = in_GetHdrLen (&sk->raw.ip);
        BYTE *data = (BYTE*)&sk->raw.ip + hlen;

        len = intel16 (sk->raw.ip.length) - hlen;
        len = min (len, maxlen);
        memcpy (buf, data, len);
        sk->raw.used = 0;
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
            len = tcp_read (sk, buf, maxlen);
            break;
#endif
        case UDP_PROTO:
            len = udp_read (sk, buf, maxlen);
            break;
#if defined(USE_BSD_FUNC)
        case IP_TYPE:
            raw = TRUE;
            len = raw_read (sk, buf, maxlen);
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
        if (maxlen > 0 && !raw && sk->tcp.usr_yield != NULL) {  /* yield only when room */
            (*sk->tcp.usr_yield)();                             /* 99.07.01 EE */
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
        return udp_read (sk, buf, len);

#if !defined(USE_UDP_ONLY)
    if (sk->u.ip_type == TCP_PROTO || sk->tcp.rx_datalen > 0)
        return tcp_read (sk, buf, len);
#endif

#if defined(USE_BSD_FUNC)
    if (sk->u.ip_type == IP_TYPE)
        return raw_read (sk, buf, len);
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
            written = tcp_write (sk, data, chunk);
            break;
#endif
        case UDP_PROTO:
            chunk = min (mtu - sizeof(in_Header) - sizeof(udp_Header), chunk);
            written = udp_write (sk, data, chunk);
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

        if (sk->udp.usr_yield != NULL)
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
        len = udp_write (sk, data, len);
        return (len < 0 ? 0 : len);
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        len = tcp_write (sk, data, len);
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
            written = udp_write (sk, data, len);
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
        sk->tcp.tx_datalen = len;
        return TCP_SEND (sk);
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
        if (sk->tcp.tx_datalen > 0) {
            sk->tcp.flags |= tcp_FlagPUSH;
            if (sk->tcp.unacked == 0) { /* !! S. Lawson - only if data not moving */
                (void) TCP_SEND (sk);
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
        udp_close (sk);
        break;
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        _tcp_close (sk);
        (void) tcp_tick (sk);
        break;
#endif
    }
    return (0);
}

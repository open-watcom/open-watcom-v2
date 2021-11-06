#ifndef __PCTCP_H
#define __PCTCP_H

/*
 * Timer definitions. All timers are in milli-seconds.
 */
#define tcp_TIMEOUT       13000UL            /* timeout during a connection */
#define tcp_LONGTIMEOUT  (1000UL*sock_delay) /* timeout for open */
#define tcp_LASTACK_TIME  10000     /* timeout in the LASTACK state added AGW 5th Jan 2001 */

#define DEF_OPEN_TO       1000UL    /* # of msec in tcp-open (<=3s; RFC1122) */
#define DEF_CLOSE_TO      1000UL    /* # of msec for CLOSEWT state       */
#define DEF_RTO_ADD       100UL     /* # of msec delay to add for RTT    */
#define DEF_RTO_BASE      10UL      /* # of msec in RTO recalculation    */
#define DEF_RST_TIME      100UL     /* # of msec before sending RST      */
#define DEF_RETRAN_TIME   50UL      /* do retrans logic every 50ms       */
#define DAEMON_RUN_TIME   55UL      /* run background daemons every 55ms */

/*
 * S. Lawson - define a short TIME_WAIT timeout. It should be from
 *  .5 to 4 minutes (2MSL) but it's not really practical for us.
 *  2 secs will hopefully handle the case where ACK must be retransmitted,
 *  but can't protect future connections on the same port from old packets.
 */
#define tcp_TIMEWT_TO 2000UL

/*
 * We use 32-bit from system-timer as initial sequence number
 * (ISN, network order). Maybe not the best choice (easy guessable).
 * The ISN should wrap only once a day.
 */
#define INIT_SEQ()  intel ((DWORD)set_timeout(1))

/*
 * Van Jacobson's Algorithm; max std. average and std. deviation
 */
#define MAX_VJSA    80000UL
#define MAX_VJSD    20000UL
#define INIT_VJSA   220

typedef void (*DebugProc) (const sock_type *, const void*, const char*, unsigned);

#define my_ip_addr      NAMESPACE (my_ip_addr)
#define sin_mask        NAMESPACE (sin_mask)
#define block_tcp       NAMESPACE (block_tcp)
#define block_udp       NAMESPACE (block_udp)
#define block_ip        NAMESPACE (block_ip)
#define block_icmp      NAMESPACE (block_icmp)

#define hostname        NAMESPACE (hostname)
#define mtu             NAMESPACE (mtu)
#define mss             NAMESPACE (mss)
#define mtu_discover    NAMESPACE (mtu_discover)
#define tcp_nagle       NAMESPACE (tcp_nagle)
#define tcp_keepalive   NAMESPACE (tcp_keepalive)
#define tcp_opt_timstmp NAMESPACE (tcp_opt_timstmp)
#define tcp_opt_sackok  NAMESPACE (tcp_opt_sackok)
#define tcp_opt_wscale  NAMESPACE (tcp_opt_wscale)

extern char      hostname[MAX_HOSTLEN+1];
extern int       mtu, mss;
extern int       mtu_discover;
extern DebugProc _dbugxmit;
extern DebugProc _dbugrecv;
extern int       block_tcp;
extern int       block_udp;
extern int       block_icmp;
extern int       block_ip;
extern int       tcp_keepalive;
extern int       tcp_nagle;
extern int       tcp_opt_timstmp;
extern int       tcp_opt_sackok;
extern int       tcp_opt_wscale;

extern int       tcp_OPEN_TO;
extern int       tcp_CLOSE_TO;
extern int       tcp_RTO_ADD;
extern int       tcp_RTO_BASE;
extern int       tcp_RST_TIME;
extern int       tcp_RETRAN_TIME;

extern DWORD     my_ip_addr;
extern DWORD     sin_mask;

extern sock_type *_tcp_allsocs;
extern sock_type *_udp_allsocs;

#define sock_wait_established(sk,seconds,fn,statusptr) \
        do {                                           \
           if (_ip_delay0 (sk,seconds,fn,statusptr))   \
              goto sock_err;                           \
        } while (0)

#define sock_wait_input(sk,seconds,fn,statusptr)       \
        do {                                           \
           if (_ip_delay1 (sk,seconds,fn,statusptr))   \
              goto sock_err;                           \
        } while (0)

#define sock_wait_closed(sk,seconds,fn,statusptr)      \
        do {                                           \
           if (_ip_delay2(sk,seconds,fn,statusptr))    \
              goto sock_err;                           \
        } while (0)

#define sock_tick(sk, statusptr)                       \
        do {                                           \
           if (!tcp_tick(sk)) {                        \
              if (statusptr)                           \
                 *statusptr = 1;                       \
              goto sock_err;                           \
           }                                           \
        } while (0)

extern int   udp_listen (udp_Socket *udp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler);
extern int   udp_open   (udp_Socket *udp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler);
extern void  udp_SetTTL (udp_Socket *udp_sk, BYTE ttl);
extern void _udp_cancel (const in_Header*, int, const char *, DWORD);
extern void _tcp_cancel (const in_Header*, int, const char *, DWORD);

extern void _tcp_close    (sock_type *sk);
extern void  tcp_abort    (tcp_Socket *tcp_sk);
extern int   tcp_open     (tcp_Socket *tcp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler);
extern int   tcp_listen   (tcp_Socket *tcp_sk, WORD lport, DWORD ina, WORD port, ProtoHandler handler, WORD timeout);
extern WORD  tcp_tick     (sock_type *sk);

extern int   tcp_established (tcp_Socket *tcp_sk);
extern int  _tcp_send        (sock_type *sk, char *file, unsigned line);
extern int  _tcp_sendsoon    (sock_type *sk, char *file, unsigned line);

extern sock_type *_tcp_unthread (sock_type *sk);
extern void tcp_Retransmitter (int force);
extern int  _ip_handler (in_Header *ip, BOOL broadcast);

#define TCP_SEND(sk)        _tcp_send    (sk, __FILE__, __LINE__)
#define TCP_SENDSOON(sk)    _tcp_sendsoon(sk, __FILE__, __LINE__)

extern void  sock_abort       (sock_type *sk);
extern int   sock_keepalive   (sock_type *sk);
extern int   sock_read        (sock_type *sk, BYTE *dp, int len);
extern int   sock_fastread    (sock_type *sk, BYTE *dp, int len);
extern int   sock_preread     (sock_type *sk, BYTE *dp, int len);
extern int   sock_write       (sock_type *sk, const BYTE *dp, int len);
extern int   sock_fastwrite   (sock_type *sk, const BYTE *dp, int len);
extern int   sock_enqueue     (sock_type *sk, const BYTE *dp, int len);
extern void  sock_noflush     (sock_type *sk);
extern void  sock_flush       (sock_type *sk);
extern void  sock_flushnext   (sock_type *sk);
extern BYTE  sock_putc        (sock_type *sk, BYTE c);
extern WORD  sock_getc        (sock_type *sk);
extern int   sock_puts        (sock_type *sk, const BYTE *dp);
extern int   sock_gets        (sock_type *sk, BYTE *dp, int n);
extern int   sock_printf      (sock_type *sk, const char *format, ...);
extern int   sock_scanf       (sock_type *sk, const char *format, ...);
extern WORD  sock_dataready   (sock_type *sk);
extern int   sock_close       (sock_type *sk);
extern int   sock_yield       (sock_type *sk, void (*fn)(void));
extern WORD  sock_mode        (sock_type *sk, WORD mode);
extern int   sock_sselect     (const sock_type *sk, int state);

/* In ports.c
 */
extern int   init_localport   (void);
extern WORD  findfreeport     (WORD oldport, BOOL sleep_msl);
extern int   grab_localport   (WORD port);
extern int   reuse_localport  (WORD port);

/* In sock_in.c
 */
extern void  ip_timer_init    (sock_type *sk, int);
extern int   ip_timer_expired (const sock_type *sk);
extern int  _ip_delay0        (sock_type *sk, int, UserHandler, int *);
extern int  _ip_delay1        (sock_type *sk, int, UserHandler, int *);
extern int  _ip_delay2        (sock_type *sk, int, UserHandler, int *);
extern int   sock_timeout     (sock_type *sk, int);
extern int   sock_established (sock_type *sk);

#endif

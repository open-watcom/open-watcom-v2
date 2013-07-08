/*
 * Waterloo TCP
 *
 * Copyright (c) 1990, 1991, 1992, 1993 Erick Engelke
 *
 * Portions copyright others, see copyright.h for details.
 *
 * This library is free software; you can use it or redistribute under
 * the terms of the license included in LICENSE.H.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * file LICENSE.H for more details.
 *
 */

#ifndef __WATT_TCP_H
#define __WATT_TCP_H

#define WATTCP_VER  0x0210   /* Kernel version (major.minor) */

#define WATTCP_MAJOR_VER  2
#define WATTCP_MINOR_VER  1
#define WATTCP_DEVEL_REL  5

#ifndef __WATTCP_H  /* <tcp.h> only included in ./src/version.c */

#include <stdio.h>       /* FILE, size_t */
#include <sys/wtypes.h>  /* fd_set */
#include <sys/wtime.h>   /* struct timeval */
#include <sys/swap.h>    /* intel(), intel16() */

#ifdef __cplusplus
extern "C" {
#endif

extern const char *wattcpCopyright;  /* "See COPYRIGHT.H for details" ? */

extern const char *wattcpVersion (void);      /* WatTCP target version/date */
extern const char *wattcpCapabilities (void); /* what's been compiled in */

/*
 * Typedefs and constants
 */
#ifndef BYTE
#define BYTE   unsigned char
#endif

#ifndef WORD
#define WORD   unsigned short
#endif

#ifndef DWORD
#define DWORD  unsigned long
#endif

#ifndef eth_address
typedef BYTE  eth_address[6];
#endif

typedef struct {
        BYTE   undoc [4308];
      } tcp_Socket;

typedef struct {
        BYTE   undoc [2136];
      } udp_Socket;

typedef int (*ProtoHandler) (void *sock, BYTE *data, int len,
                             void *tcp_phdr, void *udp_hdr);

typedef int (*UserHandler)  (void *sock);


#define MAX_COOKIES     10
#define MAX_NAMESERVERS 10

#define TCP_MODE_BINARY  0
#define TCP_MODE_ASCII   1
#define UDP_MODE_CHK     0       /* default to checksum */
#define UDP_MODE_NOCHK   2
#define TCP_MODE_NAGLE   0       /* Nagle algorithm */
#define TCP_MODE_NONAGLE 4

#define SOCKESTABLISHED  1       /* socket states for sock_sselect */
#define SOCKDATAREADY    2
#define SOCKCLOSED       4

/*
 * Hide "private" symbols by prefixing with "_w32_"
 */
#undef  W32_NAMESPACE
#define W32_NAMESPACE(x) _w32_ ## x

#define init_misc         W32_NAMESPACE (init_misc)
#define Random            W32_NAMESPACE (Random)
#define set_timeout       W32_NAMESPACE (set_timeout)
#define chk_timeout       W32_NAMESPACE (chk_timeout)
#define cmp_timeout       W32_NAMESPACE (cmp_timeout)
#define hires_timer       W32_NAMESPACE (hires_timer)
#define set_timediff      W32_NAMESPACE (set_timediff)
#define get_timediff      W32_NAMESPACE (get_timediff)
#define timeval_diff      W32_NAMESPACE (timeval_diff)

#define my_ip_addr        W32_NAMESPACE (my_ip_addr)
#define sin_mask          W32_NAMESPACE (sin_mask)
#define sock_delay        W32_NAMESPACE (sock_delay)
#define sock_inactive     W32_NAMESPACE (sock_inactive)
#define sock_data_timeout W32_NAMESPACE (sock_data_timeout)
#define multihomes        W32_NAMESPACE (multihomes)
#define block_tcp         W32_NAMESPACE (block_tcp)
#define block_udp         W32_NAMESPACE (block_udp)
#define block_ip          W32_NAMESPACE (block_ip)
#define block_icmp        W32_NAMESPACE (block_icmp)
#define last_cookie       W32_NAMESPACE (last_cookie)
#define cookies           W32_NAMESPACE (cookies)
#define survivebootp      W32_NAMESPACE (survivebootp)
#define survivedhcp       W32_NAMESPACE (survivedhcp)
#define surviverarp       W32_NAMESPACE (surviverarp)
#define loopback_handler  W32_NAMESPACE (loopback_handler)
#define usr_init          W32_NAMESPACE (usr_init)
#define inchksum          W32_NAMESPACE (inchksum)

extern int  sock_init (void);
extern void sock_exit (void);
extern void dbug_init (void);  /* effective if compiled with `USE_DEBUG' */
extern void dbug_exit (void);  /* effective if compiled with `USE_DEBUG' */
extern void init_misc (void);  /* might be called before sock_init() */


/*
 *  `s' is the pointer to a udp or tcp socket
 */
extern int  sock_read       (void *s, char *dp, int len);
extern int  sock_preread    (void *s, char *dp, int len);
extern int  sock_fastread   (void *s, char *dp, int len);
extern int  sock_write      (void *s, const char *dp, int len);
extern void sock_enqueue    (void *s, const char *dp, int len);
extern int  sock_fastwrite  (void *s, const char *dp, int len);
extern int  sock_setbuf     (void *s, char *buf, unsigned len);
extern int  sock_flush      (void *s);
extern int  sock_flushnext  (void *s);
extern int  sock_puts       (void *s, const char *dp);
extern WORD sock_gets       (void *s, char *dp, int n);
extern BYTE sock_putc       (void *s, char c);
extern BYTE sock_getc       (void *s);
extern WORD sock_dataready  (void *s);
extern int  sock_established(void *s);
extern int  sock_close      (void *s);
extern int  sock_abort      (void *s);
extern int  sock_keepalive  (void *s);
extern int  sock_printf     (void *s, const char *format, ...);
extern int  sock_scanf      (void *s, const char *format, ...);
extern int  sock_yield      (void *s, void (*fn)(void));
extern int  sock_mode       (void *s, WORD mode);        /* see TCP_MODE_... */
extern int  sock_sselect    (void *s, int waitstate);
extern int  sock_timeout    (void *s, int seconds);
extern int  sock_recv       (void *s, char *buf, int len);
extern int  sock_recv_init  (void *s, char *buf, int len);
extern int  sock_recv_from  (void *s, DWORD *ip, WORD *port, char *buf, int len, int peek);
extern int  sock_recv_used  (void *s);

/*
 * TCP or UDP specific stuff, must be used for open's and listens, but
 * sock stuff above is used for everything else
 */
extern int   udp_open   (void *s, WORD lport, DWORD ina, WORD port, ProtoHandler handler);
extern int   tcp_open   (void *s, WORD lport, DWORD ina, WORD port, ProtoHandler handler);
extern int   udp_listen (void *s, WORD lport, DWORD ina, WORD port, ProtoHandler handler);
extern int   tcp_listen (void *s, WORD lport, DWORD ina, WORD port, ProtoHandler handler, WORD timeout);
extern int   tcp_established (void *s);

extern char *rip        (char *s);
extern DWORD resolve    (const char *name);
extern int   resolve_ip (DWORD ip, char *name);
extern DWORD lookup_host(const char *host, char *ip_str);

/*
 * less general functions
 */
extern int   tcp_cbreak(WORD  mode);
extern DWORD aton      (const char *name);
extern int   isaddr    (const char *name);

#define tcp_cbrk(mode) tcp_cbreak(mode) /* old name */

/*
 * timers
 */
extern DWORD  set_timeout  (DWORD msec);
extern int    chk_timeout  (DWORD value);
extern int    cmp_timers   (DWORD t1, DWORD t2);
extern int    set_timediff (long msec);
extern DWORD  get_timediff (DWORD now, DWORD t);
extern void   hires_timer  (int on);
extern struct timeval timeval_diff (const struct timeval *a, const struct timeval *b);

extern void   ip_timer_init   (void *s , WORD delayseconds);
extern WORD   ip_timer_expired(void *s);

/*
 * TCP/IP system variables - do not change these since they
 *      are not necessarily the source variables, instead use
 *      ip_Init function
 */
extern DWORD my_ip_addr;
extern DWORD sin_mask;       /* eg.  0xfffffe00L */
extern WORD  sock_delay;
extern WORD  sock_inactive;
extern WORD  sock_data_timeout;
extern WORD  multihomes;

extern int   block_tcp;
extern int   block_udp;
extern int   block_icmp;
extern int   block_ip;
extern WORD  last_cookie;
extern DWORD cookies [MAX_COOKIES];

extern WORD  survivebootp;
extern WORD  survivedhcp;
extern WORD  surviverarp;
extern void (*loopback_handler)(void *ip);

/*
 * things you probably won't need to know about
 */

/*
 * sock_debugdump
 *      - dump some socket control block parameters
 * used for testing the kernal, not recommended
 */
extern void sock_debugdump (const void *s);

/*
 * tcp_config - read a configuration file
 *            - if special path desired, call after sock_init()
 *            - null reads path WATTCP.CFG env-var or from program's path
 * see sock_init();
 */
extern int tcp_config (const char *path);

/*
 * tcp_tick - must be called periodically by user application.
 *          - returns 1 when our socket closes
 */
extern WORD tcp_tick (void *s);

/*
 * tcp_set_debug_state - set to 1,2 or reset 0
 */
extern void tcp_set_debug_state (WORD x);


/*
 * name domain constants, etc.
 */
#define loc_domain       W32_NAMESPACE (loc_domain)
#define def_domain       W32_NAMESPACE (def_domain)
#define def_nameservers  W32_NAMESPACE (def_nameservers)
#define dns_timeout      W32_NAMESPACE (dns_timeout)
#define last_nameserver  W32_NAMESPACE (last_nameserver)
#define arp_last_gateway W32_NAMESPACE (arp_last_gateway)
#define arp_timeout      W32_NAMESPACE (arp_timeout)
#define arp_alive        W32_NAMESPACE (arp_alive)
#define arp_cache_data   W32_NAMESPACE (arp_cache_data)
#define arp_gate_list    W32_NAMESPACE (arp_gate_list)

#define mtu              W32_NAMESPACE (mtu)
#define mss              W32_NAMESPACE (mss)
#define ctrace_on        W32_NAMESPACE (ctrace_on)

extern char  *def_domain;
extern char  *loc_domain;
extern DWORD  def_nameservers [MAX_NAMESERVERS];
extern WORD   dns_timeout;
extern WORD   last_nameserver;
extern WORD   arp_last_gateway;
extern WORD   arp_timeout;
extern WORD   arp_alive;

extern struct {
       DWORD  gate_ip;
       DWORD  subnet;
       DWORD  mask;
       int    metric;
     } arp_gate_list[];

extern struct {
       DWORD       ip;
       eth_address hardware;
       BYTE        flags;
       BYTE        bits;
       DWORD       expiry;
     } arp_cache_data[];

extern WORD  wathndlcbrk;
extern WORD  watcbroke;
extern WORD  mtu, mss;
extern int   ctrace_on;


/*
 * sock_wait_... macros
 */

/*
 * sock_wait_established()
 *      - waits then aborts if timeout on s connection
 * sock_wait_input()
 *      - waits for received input on s
 *      - may not be valid input for sock_Gets...    check returned length
 * sock_tick()
 *      - do tick and jump on abort
 * sock_wait_closed();
 *      - discards all received data
 *
 * jump to sock_err with contents of *statusptr set to
 *       1 on closed
 *      -1 on timeout
 *
 */
extern int _ip_delay0 (void *s, int seconds, UserHandler fn, void *statusptr);
extern int _ip_delay1 (void *s, int seconds, UserHandler fn, void *statusptr);
extern int _ip_delay2 (void *s, int seconds, UserHandler fn, void *statusptr);


#define sock_wait_established(s,seconds,fn,statusptr) \
        do {                                          \
           if (_ip_delay0 (s,seconds,fn,statusptr))   \
              goto sock_err;                          \
        } while (0)

#define sock_wait_input(s,seconds,fn,statusptr)       \
        do {                                          \
           if (_ip_delay1 (s,seconds,fn,statusptr))   \
              goto sock_err;                          \
        } while (0)

#define sock_tick(s, statusptr)                       \
        do {                                          \
           if (!tcp_tick(s)) {                        \
              if (statusptr) *statusptr = 1;          \
              goto sock_err;                          \
           }                                          \
        } while (0)

#define sock_wait_closed(s,seconds,fn,statusptr)      \
        do {                                          \
           if (_ip_delay2(s,seconds,fn,statusptr))    \
              goto sock_err;                          \
        } while (0)

/*
 * User hook for WATTCP.CFG initialisation file.
 */
extern void (*usr_init) (const char *keyword, const char *value);

struct config_table {
       const char *keyword;
       int         type;
       void       *arg_func;
     };

#define ARG_ATOI    0   /* convert to int */
#define ARG_ATOB    1   /* convert to 8-bit byte */
#define ARG_ATOW    2   /* convert to 16-bit word */
#define ARG_ATON    3   /* convert to ip-address */
#define ARG_ATOX_B  4   /* convert to hex-byte */
#define ARG_ATOX_W  5   /* convert to hex-word */
#define ARG_STRDUP  6   /* duplicate string value */
#define ARG_RESOLVE 7   /* resolve host to ip-address */
#define ARG_FUNC    8   /* call convertion function */

extern int parse_config_table (struct config_table *tab,
                               const char *section,
                               const char *name,
                               const char *value);

/*
 * Run with no config file (embedded/diskless)
 */
extern int _watt_no_config; 


/*
 * Various function-pointer hooks etc.
 */
extern int  (*_printf) (const char *, ...); /* only if defined USE_DEBUG */
extern void (*_outch)  (char c);            /* or else user must define it */
extern int  (*_resolve_hook) (void);

extern int (*tftp_writer) (const void *buf, unsigned length);
extern int (*tftp_terminator) (void);

extern void  outs    (const char *s);
extern void  outsnl  (const char *s);
extern void  outsn   (const char *s, int n);
extern void  outhexes(const char *s, int n);
extern void  outhex  (char ch);

extern int   _ping     (DWORD host, DWORD num, const BYTE *pattern, int len);
extern DWORD _chk_ping (DWORD host, DWORD *ping_num);

extern void  _eth_init         (void);
extern void  _eth_release      (void);
extern BYTE *_eth_formatpacket (const void *eth_dest, WORD eth_type);
extern void  _eth_free         (const void *buf, WORD type);
extern BYTE *_eth_arrived      (WORD *type, int *brdcast);
extern void  _eth_send         (WORD len);
extern int   _eth_set_addr     (eth_address *addr);
extern void *_eth_hardware     (BYTE *p);
extern BYTE  _eth_get_hwtype   (BYTE *hwtype, BYTE *hwlen);

extern void *(*_eth_recv_hook) (WORD *type);
extern int   (*_eth_xmit_hook) (void *buf, unsigned len);

extern WORD  inchksum          (const void *ptr, int len);

/*
 * Turn off "C" scope because <stdio.h> may include
 * features specific to "C++". Nested scopes are bad.
 */
#ifdef __cplusplus 
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BSD-socket similarities
 */

struct watt_sockaddr {     /* for _getpeername, _getsockname */
       WORD   s_type;
       WORD   s_port;
       DWORD  s_ip;
       BYTE   s_spares[6]; /* unused */
     };

extern int sock_rbsize (const void *s);
extern int sock_rbused (const void *s);
extern int sock_rbleft (const void *s);
extern int sock_tbsize (const void *s);
extern int sock_tbused (const void *s);
extern int sock_tbleft (const void *s);

extern char *_inet_ntoa  (char *s, DWORD x);
extern DWORD _inet_addr  (const char *name);

extern int   _getsockname(const tcp_Socket *s, void *dest, int *len);
extern int   _getpeername(const tcp_Socket *s, void *dest, int *len);
extern DWORD _gethostid  (void);
extern DWORD _sethostid  (DWORD ip);
extern int  _chk_socket  (const tcp_Socket *s);
extern void  psocket     (tcp_Socket *s);

extern int   getdomainname (char *name, int len);
extern int   setdomainname (char *name, int len);
extern int   gethostname   (char *name, int len);
extern int   sethostname   (char *name, int len);
extern void _arp_register  (DWORD use, DWORD instead_of, int nowait);
extern int  _arp_resolve   (DWORD ina, void *eth, int nowait);

extern int  addwattcpd (void (*p)());
extern int  delwattcpd (void (*p)());

extern void _sock_debug_on  (void);
extern void _sock_debug_off (void);

extern const char *sockerr  (tcp_Socket *s);
extern const char *sockstate(tcp_Socket *s);

#ifndef iovec_defined
#define iovec_defined
struct iovec {
       void *iov_base;
       int   iov_len;
     };
#endif

extern int close_s  (int s);
extern int write_s  (int s, const char *buf, int nbyte);
extern int read_s   (int s,       char *buf, int nbyte);
extern int writev_s (int s, const struct iovec *vector, size_t count);
extern int select_s (int num_sockets,
                     fd_set *read_events,
                     fd_set *write_events,
                     fd_set *except_evenst,
                     struct timeval *timeout);

/*
 * Multicast stuff (if built with `USE_MULTICAST')
 */

extern int _multicast_on;

extern int join_mcast_group  (DWORD);
extern int leave_mcast_group (DWORD);
extern int is_multicast      (DWORD);
extern int multi_to_eth      (DWORD, void *);
extern int udp_SetTTL        (udp_Socket *s, BYTE ttl);


/*
 * Commandline parsing
 */

extern char *optarg;       /* argument of current option                    */
extern int   optind;       /* index of next argument; default=0: initialize */
extern int   opterr;       /* 0=disable error messages; default=1: enable   */
extern int   optopt;       /* option char returned from getopt()            */
extern char *optswchar;    /* characters introducing options; default="-"   */

extern enum _optmode {
       GETOPT_UNIX,        /* options at start of argument list (default)   */
       GETOPT_ANY,         /* move non-options to the end                   */
       GETOPT_KEEP,        /* return options in order                       */
     } optmode;

int getopt (int argc, char *const argv[], const char *opt_str);


/*
 * Statistics printing
 */

extern void print_eth_stats (void);
extern void print_ip_stats  (void);
extern void print_icmp_stats(void);
extern void print_igmp_stats(void);
extern void print_udp_stats (void);
extern void print_tcp_stats (void);
extern void print_all_stats (void);
extern void reset_stats     (void);

extern int  sock_stats (tcp_Socket *s, WORD *days, WORD *inactive,
                        WORD *cwindow, WORD *avg, WORD *sd);


/*
 * Controlling timer interrupt handler for background processing.
 * Not recommended, little tested
 */

extern void backgroundon  (void);
extern void backgroundoff (void);
extern void backgroundfn  (void (*func)(void));


/*
 *  Misc functions
 */

extern int ffs    (int mask);
extern int Random (unsigned a, unsigned b);

#if defined (__HIGHC__)
  extern int system (const char *cmd);
  pragma Alias (system, "_mw_watt_system");
#endif

#ifdef __cplusplus
};
#endif

#endif /* __WATTCP_H   */

#endif /* __WATT_TCP_H */

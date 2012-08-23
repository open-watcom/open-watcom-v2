#ifndef __WATTCP_H
#define __WATTCP_H    

/*
 * Our basic types
 */
typedef unsigned char   BYTE;           /*  8 bits   */
typedef unsigned short  WORD;           /* 16 bits   */
typedef unsigned long   DWORD;          /* 32 bits   */
typedef unsigned int    UINT;           /* 16/32 bit */
typedef BYTE            eth_address[6];
typedef BYTE            ax25_address[7];

#define mac_address eth_address /* our emphasis is no longer on Ethernet */

typedef int (*ProtoHandler) (void *sock, BYTE *data, int len,
                             void *tcp_phdr, void *udp_hdr);

typedef int (*UserHandler)  (void *sock);

typedef int (*sol_upcall)   (void *socket, int icmp_type);

#if defined(__HIGHC__) || defined(__GNUC__)
  typedef unsigned long long  uint64;
  #define HAVE_UINT64

#elif defined(__WATCOMC__) && defined(__WATCOM_INT64__)
  typedef unsigned __int64 uint64;
  #define HAVE_UINT64
#endif

/*
 * Until C compilers support C++ namespaces, we use this
 * prefix for our namespace.
 */
#define NAMESPACE(x)    _w32_ ## x     


#include "target.h"        /* portability macros & defines */
#include "config.h"        /* options & features to include */

#if defined(USE_FORTIFY)
#include "fortify.h"       /* use Fortify malloc library */
#endif

/*
 * Sizes and protocols we use
 */
#define ETH_MAX_DATA     1500
#define ETH_MIN          60
#define ETH_MAX          (ETH_MAX_DATA + sizeof(eth_Header))

#define TOK_MAX_DATA     ETH_MAX_DATA  /* could be much larger */
#define TOK_MAX          (TOK_MAX_DATA + sizeof(tok_Header))
#define TOK_MIN          sizeof(tok_Header)

#define FDDI_MAX_DATA    ETH_MAX_DATA  /* really is 4470 */
#define FDDI_MAX         (FDDI_MAX_DATA + sizeof(fddi_Header))
#define FDDI_MIN         (3 + sizeof(fddi_Header))

#define AX25_MAX_DATA    ETH_MAX_DATA
#define AX25_MAX         (AX25_MAX_DATA + sizeof(ax25_Header))
#define AX25_MIN         sizeof(ax25_Header)

#define PPPOE_MAX_DATA   (ETH_MAX_DATA-12)  // !!
#define PPPOE_MAX        (PPPOE_MAX_DATA + sizeof(pppoe_Header))
#define PPPOE_MIN        sizeof(pppoe_Header)

#define IP_TYPE          0x0008  /* these are in network order */
#define ARP_TYPE         0x0608
#define RARP_TYPE        0x3508
#define PPPOE_DISC_TYPE  0x8863
#define PPPOE_SESS_TYPE  0x8864


#if (DOSX)
#define MAX_FRAGMENTS    45UL
#else
#define MAX_FRAGMENTS    22UL
#endif

/* This should really be a function of current MAC-driver
 */
#define MAX_IP_DATA      (ETH_MAX_DATA - sizeof(in_Header))
#define MAX_FRAG_SIZE    (MAX_FRAGMENTS * MAX_IP_DATA)

#define MAX_COOKIES      10
#define MAX_STRING       50
#define MAX_PATHLEN      80
#define MAX_HOSTLEN      80           /* most strings are limited */
#define MAX_WINDOW       32767        /* max tcp window */

#define SAFETYTCP        0x538F25A3L  /* marker signatures */
#define SAFETYUDP        0x3E45E154L

#ifndef FALSE
#define FALSE            0
#endif

#ifndef TRUE
#define TRUE             (!FALSE)
#endif

/*
 * These are IP protocol numbers (see RFC-1700)
 */
#define UDP_PROTO        17
#define TCP_PROTO        6
#define ICMP_PROTO       1
#define IGMP_PROTO       2

#define TCP_MODE_BINARY  0       /* default mode */
#define TCP_MODE_ASCII   1
#define UDP_MODE_CHK     0       /* default to having checksums */
#define UDP_MODE_NOCHK   2       /* turn off checksums */
#define TCP_MODE_NAGLE   0       /* Nagle algorithm */
#define TCP_MODE_NONAGLE 4

/*
 * These are the IP options
 */
#define IPOPT_EOL        0       /* end-of-option list */
#define IPOPT_NOP        1       /* no-operation */
#define IPOPT_RR         7       /* record packet route */
#define IPOPT_TS         68      /* timestamp */
#define IPOPT_SECURITY   130     /* provide s,c,h,tcc */
#define IPOPT_LSRR       131     /* loose source route */
#define IPOPT_SATID      136     /* satnet id */
#define IPOPT_SSRR       137     /* strict source route */
#define IPOPT_RA         148     /* router alert */

/*
 * These are the TCP options
 */
#define TCPOPT_EOL       0       /* end-of-option list */
#define TCPOPT_NOP       1       /* no-operation */
#define TCPOPT_MAXSEG    2       /* maximum segment size */
#define TCPOPT_WINDOW    3       /* window scale factor (rfc1072) */
#define TCPOPT_SACKOK    4       /* selective ack ok (rfc1072) */
#define TCPOPT_SACK      5       /* selective ack (rfc1072) */
#define TCPOPT_ECHO      6       /* echo (rfc1072) */
#define TCPOPT_ECHOREPLY 7       /* echo (rfc1072) */
#define TCPOPT_TIMESTAMP 8       /* timestamps (rfc1323) */
#define TCPOPT_CC        11      /* T/TCP CC options (rfc1644) */
#define TCPOPT_CCNEW     12      /* T/TCP CC options (rfc1644) */
#define TCPOPT_CCECHO    13      /* T/TCP CC options (rfc1644) */

#define TCP_MAX_WINSHIFT 14      /* maximum window shift */

/*
 * These are the ICMP messages. Should be same as in <netinet/ip_icmp.h>
 */
#define ICMP_ECHOREPLY     0     /* echo reply */
#define ICMP_UNREACH       3     /* dest unreachable */
#define ICMP_SOURCEQUENCH  4     /* packet lost, slow down */
#define ICMP_REDIRECT      5     /* shorter route        */
#define ICMP_ECHO          8     /* echo service         */
#define ICMP_ROUTERADVERT  9     /* router advertisement */
#define ICMP_ROUTERSOLICIT 10    /* router solicitation  */
#define ICMP_TIMXCEED      11    /* time exceeded        */
#define ICMP_PARAMPROB     12    /* ip header bad        */
#define ICMP_TSTAMP        13    /* timestamp request    */
#define ICMP_TSTAMPREPLY   14    /* timestamp reply      */
#define ICMP_IREQ          15    /* information request  */
#define ICMP_IREQREPLY     16    /* information reply    */
#define ICMP_MASKREQ       17    /* address mask request */
#define ICMP_MASKREPLY     18    /* address mask reply   */
#define ICMP_MAXTYPE       18

/*
 * UDP/TCP socket local flags (locflags) bits.
 * Mostly used to support the BSD-socket API.
 */
#define LF_NODELAY      0x0001   /* don't use Nagle       */
#define LF_NOPUSH       0x0002   /* don't push on write   */
#define LF_NOOPT        0x0004   /* don't use tcp options */
#define LF_REUSEADDR    0x0008   /* to-do !! */
#define LF_KEEPALIVE    0x0010
#define LF_LINGER       0x0020
#define LF_xxxxxxxxxxx  0x0040
#define LF_NOFRAGMENT   0x0080
#define LF_OOBINLINE    0x0100
#define LF_SNDTIMEO     0x0200
#define LF_RCVTIMEO     0x0400
#define LF_GOT_FIN      0x0800
#define LF_REQ_TSTMP    0x1000   /* have/will request timestamps */
#define LF_RCVD_TSTMP   0x2000   /* a timestamp was received in SYN */
#define LF_RCVD_SCALE   0x4000   /* a win-scale was received in SYN */

#define loBYTE(w)       (BYTE)(w)
#define hiBYTE(w)       (BYTE)((WORD)(w) >> 8)
#define ARGSUSED(foo)   (void)foo
#define DIM(x)          (sizeof(x) / sizeof((x)[0]))

/*
 * Socket-states for sock_sselect().
 * Not used by BSD-socket API
 */
#define SOCKESTABLISHED  1
#define SOCKDATAREADY    2
#define SOCKCLOSED       4


#include <sys/packon.h>  /* align structs on byte boundaries */

/*
 * The Internet (ip) Header
 */
typedef struct in_Header {
        BYTE   hdrlen : 4;     /* Watcom requires BYTE here */
        BYTE   ver    : 4;
        BYTE   tos;
        WORD   length;
        WORD   identification;
        WORD   frag_ofs;
        BYTE   ttl;
        BYTE   proto;
        WORD   checksum;
        DWORD  source;
        DWORD  destination;
      } in_Header;

#define IP_CE      0x8000     /* `in_Header.frag_ofs' masks: */
#define IP_DF      0x4000     /*   Congestion Experienced */
#define IP_MF      0x2000     /*   Don't Fragment */
#define IP_OFFMASK 0x1fff     /*   More Fragments */

#define in_GetHdrLen(ip) ((ip)->hdrlen << 2)  /* # of bytes in IP-header */

/*
 * IP packet incuding header and data
 */
typedef struct ip_Packet {    
        in_Header head;
        BYTE      data [MAX_IP_DATA];
      } ip_Packet;


/*
 * The UDP/TCP headers
 */
typedef struct udp_Header {
        WORD   srcPort;
        WORD   dstPort;
        WORD   length;
        WORD   checksum;
      } udp_Header;

typedef struct tcp_Header {
        WORD   srcPort;
        WORD   dstPort;
        DWORD  seqnum;
        DWORD  acknum;
        BYTE   unused : 4;   /* Watcom requires BYTE here */
        BYTE   offset : 4;
        BYTE   flags;
        WORD   window;
        WORD   checksum;
        WORD   urgent;
      } tcp_Header;

#define tcp_FlagFIN   0x01
#define tcp_FlagSYN   0x02
#define tcp_FlagRST   0x04
#define tcp_FlagPUSH  0x08
#define tcp_FlagACK   0x10
#define tcp_FlagURG   0x20
#define tcp_FlagMASK  0x3F

/*
 * The TCP/UDP Pseudo Header
 */
typedef struct tcp_PseudoHeader {
        DWORD  src;
        DWORD  dst;
        BYTE   mbz;
        BYTE   protocol;
        WORD   length;
        WORD   checksum;
      } tcp_PseudoHeader;

/*
 * Internet Group Management Protocol Packet
 */
typedef struct IGMP_packet {
        int    type    : 4;
        int    version : 4;
        BYTE   mbz;
        WORD   checksum;
        DWORD  address;
      } IGMP_packet;

#define IGMP_VERSION  1
#define IGMP_QUERY    1
#define IGMP_REPORT   2


/*
 * ARP/RARP headers
 */
typedef struct arp_Header {
        WORD        hwType;
        WORD        protType;
        BYTE        hwAddrLen;     /* MAC addr. length (6) */
        BYTE        protoAddrLen;  /* IP addr. length  (4) */
        WORD        opcode;
        eth_address srcEthAddr;
        DWORD       srcIPAddr;
        eth_address dstEthAddr;
        DWORD       dstIPAddr;
      } arp_Header;

#define rarp_Header arp_Header

/*
 * ARP definitions
 */
#define ARP_REQUEST    0x0100      /* ARP/RARP op codes, Request   */
#define ARP_REPLY      0x0200      /*                    Reply     */
#define RARP_REQUEST   0x0300
#define RARP_REPLY     0x0400
#define ARP_MAX        (sizeof(eth_Header) + sizeof(arp_Header))

#include <sys/packoff.h>           /* restore default packing */


/*
 * TCP states, from tcp specification RFC-793.
 * Note: close-wait state is bypassed by automatically closing a connection
 *       when a FIN is received.  This is easy to undo.
 */
#define tcp_StateLISTEN   0      /* listening for connection */
#define tcp_StateSYNSENT  1      /* SYN sent, active open */
#define tcp_StateSYNREC   2      /* SYN received, ACK+SYN sent. */
#define tcp_StateESTAB    3      /* established */
#define tcp_StateESTCL    4      /* established, but will FIN */
#define tcp_StateFINWT1   5      /* sent FIN */
#define tcp_StateFINWT2   6      /* sent FIN, received FINACK */
#define tcp_StateCLOSWT   7      /* received FIN waiting for close */
#define tcp_StateCLOSING  8      /* sent FIN, received FIN (waiting for FINACK) */
#define tcp_StateLASTACK  9      /* FIN received, FINACK+FIN sent */
#define tcp_StateTIMEWT   10     /* dally after sending final FINACK */
#define tcp_StateCLOSED   11     /* FINACK received */

#define tcp_MaxBufSize    2048   /* maximum bytes to buffer on input */
#define udp_MaxBufSize    tcp_MaxBufSize
#define tcp_MaxTxBufSize  tcp_MaxBufSize        /* and on tcp output */

/*
 * Fields common to UDP & TCP socket definition.
 * Tries to keep members on natural boundaries (words on word-boundary,
 * dwords on dword boundary)
 */

#define UDP_TCP_COMMON                                                      \
        WORD         ip_type;          /* UDP_PROTO or TCPPROTO */          \
        BYTE         ttl;              /* Time To Live */                   \
        BYTE         fill_1;                                                \
        const char  *err_msg;          /* null when all is ok */            \
        void       (*usr_yield)(void); /* yield while waiting */            \
        sol_upcall   sol_callb;        /* socket-layer callback (icmp) */   \
        BYTE         rigid;                                                 \
        BYTE         stress;                                                \
        WORD         sockmode;         /* a logical OR of bits */           \
        DWORD        usertimer;        /* ip_timer_set, ip_timer_timeout */ \
        ProtoHandler protoHandler;     /* call with incoming data */        \
        eth_address  hisethaddr;       /* peer's ethernet address */        \
        WORD         fill_2;                                                \
                                                                            \
        DWORD        myaddr;           /* my ip-address */                  \
        DWORD        hisaddr;          /* peer's internet address */        \
        WORD         hisport;          /* peer's source port */             \
        WORD         myport;           /* my source port */                 \
        WORD         locflags;         /* local option flags */             \
        WORD         fill_3;                                                \
                                                                            \
        UINT         queuelen;         /* optional Tx queue length */       \
        const BYTE  *queue;                                                 \
                                                                            \
        int          rdatalen;         /* Rx length, must be signed */      \
        UINT         maxrdatalen;                                           \
        BYTE        *rdata;                    /* received data pointer */  \
        BYTE         rddata[tcp_MaxBufSize+1]; /* received data buffer */   \
        DWORD        safetysig                 /* magic marker */

/*
 * UDP socket definition
 */
typedef struct udp_Socket {
        struct udp_Socket *next;
        UDP_TCP_COMMON;
      } udp_Socket;

/*
 * TCP Socket definition (fields common to udp_Socket must come first)
 */
typedef struct tcp_Socket {
        struct  tcp_Socket *next;  /* link to next tcp-socket */
        UDP_TCP_COMMON;

        UINT   state;              /* tcp connection state */
        DWORD  acknum;             /* data ACK'ed */
        DWORD  seqnum;             /* sequence number */

#if defined(USE_DEBUG)
        DWORD  last_acknum[2];     /* for pcdbug.c; to follow SEQ/ACK */
        DWORD  last_seqnum[2];     /* increments */
#endif

        DWORD  timeout;            /* timeout, in milliseconds */
        BYTE   unhappy;            /* flag, indicates retransmitting segt's */
        BYTE   recent;             /* 1 if recently transmitted */
        WORD   flags;              /* tcp flags for last packet sent */

        UINT   window;             /* other guy's window */
        UINT   datalen;            /* number of bytes of data to send */
        int    unacked;            /* unacked data, must be signed */

        BYTE   cwindow;            /* Congestion window */
        BYTE   wwindow;            /* Van Jacobson's algorithm */
        WORD   fill_4;

        DWORD  vj_sa;              /* VJ's alg, standard average   (SRTT) */
        DWORD  vj_sd;              /* VJ's alg, standard deviation (RTTVAR) */
        DWORD  vj_last;            /* last transmit time */
        UINT   rto;                /* retransmission timeout */
        BYTE   karn_count;         /* count of packets */
        BYTE   tos;                /* priority */
        WORD   fill_5;

        DWORD  rtt_time;           /* Round Trip Time value */
        DWORD  rtt_lasttran;       /* RTT at last transmission */

        DWORD  ts_sent;            /* last TimeStamp value sent */
        DWORD  ts_recent;          /* last TimeStamp value received */
        DWORD  ts_echo;            /* last TimeStamp echo received */

        UINT   max_seg;
        DWORD  missed_seg[2];      /* S. Lawson - handle one dropped segment */
        DWORD  inactive_to;        /* for the inactive flag */
        DWORD  datatimer;          /* EE 99.08.23, note broken connections */
        int    sock_delay;

        BYTE   send_wscale;        /* to-do!!: window scales shifts, tx/rx */
        BYTE   recv_wscale;

        BYTE   data[tcp_MaxBufSize+1]; /* data for transmission */
        DWORD  safetytcp;              /* extra magic marker */
      } tcp_Socket;


/*
 * Raw IP socket definition. Only used in BSD-socket API.
 */
typedef struct raw_Socket {
        struct raw_Socket *next;
        WORD   ip_type;               /* same ofs as for udp/tcp Socket */
        BOOL   used;                  /* used flag; packet not read yet */
        struct in_Header ip;
        BYTE   data [MAX_FRAG_SIZE];  /* room for 1 jumbo IP packet */
      } raw_Socket;

/*
 * sock_type used for socket I/O
 */
typedef union sock_type {
        udp_Socket udp;
        tcp_Socket tcp;
        raw_Socket raw;
      } sock_type;

typedef struct watt_sockaddr {
        WORD   s_type;
        WORD   s_port;
        DWORD  s_ip;
        BYTE   s_spares[6];    /* unused in TCP realm */
      } watt_sockaddr;

/*
 * A simple RTT cache based on Phil Karn's KA9Q.
 * # of TCP round-trip-time cache entries
 */
#define RTTCACHE  16  

struct tcp_rtt {
       DWORD ip;
       UINT  rto;
     };

/*
 * Stuff for Multicast Support - JRM 6/7/93
 */
#define CLASS_D_MASK  0xE0000000UL  /* the mask that defines IP Class D  */
#define IPMULTI_MASK  0x007FFFFFUL  /* to get the low-order 23 bits      */
#define ETH_MULTI     0x01005EUL    /* high order bits of multi eth addr */
#define IPMULTI_SIZE  20            /* the size of the ipmulti table     */
#define ALL_SYSTEMS   0xE0000001UL  /* the default mcast addr 224.0.0.1  */

typedef struct multicast {
        DWORD       ina;            /* IP address of group               */
        eth_address ethaddr;        /* Ethernet address of group         */
        BYTE        processes;      /* number of interested processes    */
        DWORD       replytime;      /* IGMP query reply timer            */
        BYTE        active;         /* is this an active entry           */
      } multicast;

#endif /* __WATTCP_H */


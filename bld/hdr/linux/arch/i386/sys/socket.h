/* For setsockopt(2) */
#define SOL_SOCKET      1

#define SO_DEBUG        1
#define SO_REUSEADDR    2
#define SO_TYPE         3
#define SO_ERROR        4
#define SO_DONTROUTE    5
#define SO_BROADCAST    6
#define SO_SNDBUF       7
#define SO_RCVBUF       8
#define SO_KEEPALIVE    9
#define SO_OOBINLINE    10
#define SO_NO_CHECK     11
#define SO_PRIORITY     12
#define SO_LINGER       13
#define SO_BSDCOMPAT    14
/* #define SO_REUSEPORT    15 (future addition) */
#define SO_PASSCRED     16
#define SO_PEERCRED     17
#define SO_RCVLOWAT     18
#define SO_SNDLOWAT     19
#define SO_RCVTIMEO     20
#define SO_SNDTIMEO     21

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_RDM        4
#define SOCK_SEQPACKET  5
#define SOCK_PACKET     10
#define SOCK_MAX        (SOCK_PACKET+1)

/* For shutdown(2) */
#define SHUT_RD         0    /* further receives are disallowed */
#define SHUT_WR         1    /* further sends are disallowed */
#define SHUT_RDWR       2    /* further sends and receives are disallowed */

/* Flags for send(2) and recv(2) */
#define MSG_OOB         0x1
#define MSG_PEEK        0x2
#define MSG_DONTROUTE   0x4
#define MSG_TRYHARD     0x4
#define MSG_CTRUNC      0x8
#define MSG_PROBE       0x10
#define MSG_TRUNC       0x20
#define MSG_DONTWAIT    0x40
#define MSG_EOR         0x80
#define MSG_WAITALL     0x100
#define MSG_FIN         0x200
#define MSG_SYN         0x400
#define MSG_CONFIRM     0x800
#define MSG_RST         0x1000
#define MSG_ERRQUEUE    0x2000
#define MSG_NOSIGNAL    0x4000
#define MSG_MORE        0x8000
#define MSG_EOF         MSG_FIN

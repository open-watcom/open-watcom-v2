#ifndef __PCICMP_H
#define __PCICMP_H

#ifdef __NETINET_IP_ICMP_H
#error Cannot include both <netinet/ip_icmp.h> and pcicmp.h
#endif

#include <sys/packon.h>

struct icmp_unused {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       DWORD     unused;
       in_Header ip;
       BYTE      spares [8];
     };

struct icmp_pointer {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       BYTE      pointer;
       BYTE      unused [3];
       in_Header ip;
     };

struct icmp_ip {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       DWORD     ipaddr;         /* redirect gateway */
       in_Header ip;             /* original ip-header */
     };

struct icmp_echo {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       WORD      identifier;
       WORD      sequence;
       DWORD     index;
     };

struct icmp_timestamp {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       WORD      identifier;
       WORD      sequence;
       DWORD     original;       /* original timestamp */
       DWORD     receive;        /* receive timestamp  */
       DWORD     transmit;       /* transmit timestamp */
     };

struct icmp_info {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       WORD      identifier;
       WORD      sequence;
     };

struct icmp_addr_mask {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       WORD      identifier;
       WORD      sequence;
       DWORD     mask;
     };

struct icmp_traceroute {
       BYTE      type;
       BYTE      code;
       WORD      checksum;
       WORD      identifier;
       WORD      unused;
       WORD      outbound_hop;
       WORD      return_hop;
       DWORD     link_speed;
       DWORD     link_mtu;
     };
typedef union icmp_pkt {
        struct icmp_unused     unused;
        struct icmp_pointer    pointer;
        struct icmp_ip         ip;
        struct icmp_echo       echo;
        struct icmp_timestamp  timestamp;
        struct icmp_info       info;
        struct icmp_addr_mask  mask;
        struct icmp_traceroute tracert;
      } ICMP_PKT;

#include <sys/packoff.h>

extern const char *icmp_type_str [ICMP_MAXTYPE+1];
extern const char *icmp_unreach_str [16];
extern const char *icmp_redirect_str [4];
extern const char *icmp_exceed_str [2];

extern DWORD _chk_ping       (DWORD host, DWORD *ping_num);
extern void  *icmp_Format    (DWORD host);
extern void   icmp_handler   (const in_Header *ip, BOOL broadcast);
extern void   icmp_unreach   (const in_Header *ip, int code);
extern void   icmp_timexceed (const in_Header *ip, const void *mac_dest);
extern void   icmp_redirect  (const char *value);
extern void   icmp_mask_req  (void);

#endif

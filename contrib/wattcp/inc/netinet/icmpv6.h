#ifndef __NETINET_ICMPv6_H
#define __NETINET_ICMPv6_H

struct icmpv6hdr {
       u_char  type;
       u_char  code;
       u_short checksum;
       union {
         struct icmpv6_echo {
           u_short  identifier;
           u_short  sequence;
         } u_echo;

         u_long  pointer;
         u_long  mtu;
         u_long  unused;

         struct icmpv6_nd_advt {
           u_short reserved  : 5;
           u_short override  : 1;
           u_short solicited : 1;
           u_short router    : 1;
           u_short reserved2 : 8;
           u_short reserved3;
         } u_nd_advt;

         struct icmpv6_nd_ra {
           u_char  hop_limit;
           u_char  reserved : 6;
           u_char  other    : 1;
           u_char  managed  : 1;
           u_short rt_lifetime;
         } u_nd_ra;
       } u;
     };
#define icmp6_identifier        u.u_echo.identifier
#define icmp6_sequence		u.u_echo.sequence
#define icmp6_pointer		u.pointer
#define icmp6_mtu		u.mtu
#define icmp6_unused		u.unused
#define icmp6_router		u.u_nd_advt.router
#define icmp6_solicited		u.u_nd_advt.solicited
#define icmp6_override		u.u_nd_advt.override
#define icmp6_ndiscreserved	u.u_nd_advt.reserved
#define icmp6_hop_limit		u.u_nd_ra.hop_limit
#define icmp6_addrconf_managed	u.u_nd_ra.managed
#define icmp6_addrconf_other	u.u_nd_ra.other
#define icmp6_rt_lifetime	u.u_nd_ra.rt_lifetime


#define ICMPV6_DEST_UNREACH         1
#define ICMPV6_PKT_TOOBIG           2
#define ICMPV6_TIME_EXCEEDED        3
#define ICMPV6_PARAMETER_PROB       4

#define ICMPV6_ECHO_REQUEST         128
#define ICMPV6_ECHO_REPLY           129
#define ICMPV6_MEMBERSHIP_QUERY     130
#define ICMPV6_MEMBERSHIP_REPORT    131
#define ICMPV6_MEMBERSHIP_REDUCTION 132

/* 
 *    Codes for Destination Unreachable
 */
#define ICMPV6_NOROUTE           0
#define ICMPV6_ADM_PROHIBITED    1
#define ICMPV6_NOT_NEIGHBOUR     2
#define ICMPV6_ADDR_UNREACH      3
#define ICMPV6_PORT_UNREACH      4

/* 
 *    Codes for Time Exceeded
 */
#define ICMPV6_EXC_HOPLIMIT      0
#define ICMPV6_EXC_FRAGTIME      1

/* 
 *    Codes for Parameter Problem
 */
#define ICMPV6_HDR_FIELD         0
#define ICMPV6_UNK_NEXTHDR       1
#define ICMPV6_UNK_OPTION        2

/* 
 *    constants for (set|get)sockopt
 */
#define ICMPV6_FILTER            1

/* 
 *    ICMPV6 filter
 */
#define ICMPV6_FILTER_BLOCK       1
#define ICMPV6_FILTER_PASS        2
#define ICMPV6_FILTER_BLOCKOTHERS 3
#define ICMPV6_FILTER_PASSONLY    4

struct icmp6_filter {
       u_long data[8];
     };

#endif

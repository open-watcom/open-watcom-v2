/*
 *	Linux INET6 implementation
 *
 *	Authors:
 *	Pedro Roque		<roque@di.fc.ul.pt>
 *
 *	$Id: ipv6.h,v 1.6 1997/04/01 02:22:58 davem Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#ifndef _NET_IPV6_H
#define _NET_IPV6_H

#ifndef __NETINET_IN_H
#include <netinet/in.h>  /* in6_addr */
#endif

/*
 *  Advanced API
 *  source interface/address selection, source routing, etc...
 *  *under construction*
 */

#include <sys/packon.h>

/*
 *  IPv6 fixed header
 */
struct ipv6hdr {
       unsigned char   ipv6_priority:4;
       unsigned char   ipv6_version:4;
       unsigned char   ipv6_flow_lbl[3];
       unsigned short  ipv6_len;
       unsigned char   ipv6_nextheader;
       unsigned char   ipv6_hoplimit;
       struct in6_addr ipv6_src;
       struct in6_addr ipv6_dst;
     };



/*
 *  The length of this struct cannot be greater than the length of
 *  the proto_priv field in a sk_buff which is currently
 *  defined to be 16 bytes.
 *  Pointers take upto 8 bytes (sizeof(void *) is 8 on the alpha).
 */
struct ipv6_options  {
       /* length of extension headers */
       unsigned short opt_flen;       /* after fragment hdr */
       unsigned short opt_nflen;      /* before fragment hdr */
      /*
       * protocol options
       * usualy carried in IPv6 extension headers
       */
       struct ipv6_rt_hdr *srcrt; /* Routing Header */
      };

struct in6_pktinfo {
       struct in6_addr ipi6_addr;
       int             ipi6_ifindex;
     };


struct in6_ifreq {
       struct in6_addr ifr6_addr;
       unsigned long   ifr6_prefixlen;
       unsigned long   ifr6_ifindex;
     };

#define IPV6_SRCRT_STRICT       0x01    /* this hop must be a neighbor  */
#define IPV6_SRCRT_TYPE_0       0       /* IPv6 type 0 Routing Header   */

/*
 *      routing header
 */
struct ipv6_rt_hdr {
       unsigned char  nexthdr;
       unsigned char  hdrlen;
       unsigned char  type;
       unsigned char  segments_left;
       /*
        * type specific data
        * variable length field
        */
     };

/*
 * routing header type 0 (used in cmsghdr struct)
 */
struct ipv6_rt0_hdr {
       struct ipv6_rt_hdr rt_hdr;
       unsigned long      bitmap;         /* strict/loose bit map */
       struct in6_addr    addr[0];
#define rt0_type rt_hdr.type;
     };

/*
 *  NextHeader field of IPv6 header
 */

#define NEXTHDR_HOP		0	/* Hop-by-hop option header. */
#define NEXTHDR_TCP		6	/* TCP segment. */
#define NEXTHDR_UDP		17	/* UDP message. */
#define NEXTHDR_IPV6		41	/* IPv6 in IPv6 */
#define NEXTHDR_ROUTING		43	/* Routing header. */
#define NEXTHDR_FRAGMENT	44	/* Fragmentation/reassembly header. */
#define NEXTHDR_ESP		50	/* Encapsulating security payload. */
#define NEXTHDR_AUTH		51	/* Authentication header. */
#define NEXTHDR_ICMP		58	/* ICMP for IPv6. */
#define NEXTHDR_NONE		59	/* No next header */
#define NEXTHDR_DEST		60	/* Destination options header. */

#define NEXTHDR_MAX		255


#define IPV6_DEFAULT_HOPLIMIT   64
#define IPV6_DEFAULT_MCASTHOPS	1

/*
 *	Addr type
 *	
 *	type	-	unicast | multicast | anycast
 *	scope	-	local	| site	    | global
 *	v4	-	compat
 *	v4mapped
 *	any
 *	loopback
 */

#define IPV6_ADDR_ANY		0x0000U

#define IPV6_ADDR_UNICAST      	0x0001U	
#define IPV6_ADDR_MULTICAST    	0x0002U	
#define IPV6_ADDR_ANYCAST	0x0004U

#define IPV6_ADDR_LOOPBACK	0x0010U
#define IPV6_ADDR_LINKLOCAL	0x0020U
#define IPV6_ADDR_SITELOCAL	0x0040U

#define IPV6_ADDR_COMPATv4	0x0080U

#define IPV6_ADDR_SCOPE_MASK	0x00f0U

#define IPV6_ADDR_MAPPED	0x1000U
#define IPV6_ADDR_RESERVED	0x2000U	/* reserved address space */

/*
 *	fragmentation header
 */

struct ipv6_fraghdr {
       unsigned char   nexthdr;
       unsigned char   reserved;
       unsigned short  frag_off;
       unsigned long   identification;
     };
#define fraghdr ipv6_fraghdr

#include <sys/packoff.h>

#endif


/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by WIDE Project and
 *    its contributors.
 * 4. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _NETINET6_ND6_H_
#define _NETINET6_ND6_H_

#include <sys/queue.h>

struct  llinfo_nd6 {
        struct  llinfo_nd6 *ln_next;
        struct  llinfo_nd6 *ln_prev;
        struct  rtentry *ln_rt;
        struct  mbuf *ln_hold;  /* last packet until resolved/timeout */
        long    ln_asked;       /* last time we QUERIED for this addr */
        short   ln_state;
        short   ln_flags;       /* 2^0: ND6 router bit */
};

#define ND6_LLINFO_INCOMPLETE   0
#define ND6_LLINFO_REACHABLE    1
#define ND6_LLINFO_STALE        2
#define ND6_LLINFO_DELAY        3
#define ND6_LLINFO_PROBE        4

struct  nd_defrouter {
        LIST_ENTRY(nd_defrouter) dr_entry;
#define dr_next dr_entry.le_next
        struct  in6_addr rtaddr;
        u_char  flags;
        u_short rtlifetime;
        u_long  expire;
        struct  ifnet *ifp;
};

struct nd_prefix {
        LIST_ENTRY(nd_prefix) pr_entry;
#define pr_next pr_entry.le_next
        struct  in6_addr prefix;
        struct  in6_addr addr;
        struct  ifnet *ifp;
        u_char  flags;
        u_long  vltime;
        u_long  pltime;
        u_long  expire;
};

#define DRLSTSIZ 10
#define PRLSTSIZ 10
struct  in6_drlist {
        char ifname[IFNAMSIZ];
        struct nd_defrouter defrouter[DRLSTSIZ];
};
struct  in6_prlist {
        char ifname[IFNAMSIZ];
        struct nd_prefix prefix[PRLSTSIZ];
};

/* protocol constants */
#define MAX_RTR_SOLICITATION_DELAY      1       /*1sec*/
#define RTR_SOLICITATION_INTERVAL       4       /*4sec*/
#define MAX_RTR_SOLICITATIONS           3

#endif /* _NETINET6_ND6_H_ */


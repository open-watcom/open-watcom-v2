/*
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)tcp_debug.h	8.1 (Berkeley) 6/10/93
 * $Id: tcp_debug.h,v 1.5 1996/04/13 12:45:57 bde Exp $
 */

#ifndef __NETINET_TCP_DEBUG_H
#define __NETINET_TCP_DEBUG_H

struct tcp_debug {
       n_time           td_time;
       short            td_act;
       short            td_ostate;
       caddr_t          td_tcb;
       struct  tcpiphdr td_ti;
       short            td_req;
       struct  tcpcb    td_cb;
     };

#define	TA_INPUT 	0
#define	TA_OUTPUT	1
#define	TA_USER		2
#define	TA_RESPOND	3
#define	TA_DROP		4

#ifdef TANAMES
static char *tanames[] = {
            "input", "output", "user", "respond", "drop"
          };
#endif

#define	TCP_NDEBUG 100

#ifdef TCP_AUTO_DEBUG
#include <sys/wtime.h>

/* The following table is used as a circular buffer in which TCP
 * autotuning statistics can be kept.
 * It is intended that these statistics will be periodically read
 * by a debugging application using kvm.
 */

#define TAD_ENTRIES 1024            /* number of entries in monitor_table */

struct tad_entry {
       u_long         seq_no;            /* tad entry number */
       struct timeval time;              /* time of entry */
       u_long         snd_cwnd;          /* congestion window */
       u_long         sb_hiwat;          /* send socket buffer hi water mark */
       u_long         sb_target_hiwat;   /*      target for same */
       u_long         sb_cc;             /* space used in send socket buf */
       u_long         m_clused;          /* m_clusters - m_clfree */
       u_short        lport;             /* local port number */
       u_short        rport;             /* remote port number */
       u_long         debug;             /* used for debugging */
       tcp_seq        snd_max;           /* highest sequence number sent */
     } tad_table[TAD_ENTRIES];

extern struct tad_entry *tad_index;     /* insert point */
extern u_long tad_seq;
extern u_long tad_debug;

/* increment the index into the table
 */
#define TAD_INDEX_INCR tad_index = (struct tad_entry*)((u_long)tad_index + \
                                    sizeof(struct tad_entry)); \
		       if ((u_long) tad_index >= \
			   (u_long) &tad_table[TAD_ENTRIES]) \
                          tad_index = tad_table;

/* add a log entry to the table
 */
#define TAD_SNAPSHOT(tp, so)  tad_index->seq_no = tad_seq++; \
                   microtime(&(tad_index->time)); \
		   tad_index->snd_cwnd = tp->snd_cwnd; \
		   tad_index->sb_hiwat = so->so_snd.sb_hiwat; \
		   tad_index->sb_target_hiwat = so->so_snd.sb_net_target; \
		   tad_index->sb_cc = so->so_snd.sb_cc; \
		   tad_index->m_clused = mbstat.m_clusters - mbstat.m_clfree;\
		   tad_index->lport = tp->t_inpcb->inp_lport; \
		   tad_index->rport = tp->t_inpcb->inp_fport; \
		   tad_index->debug = tad_debug; \
		   tad_index->snd_max = tp->snd_max; \
		   TAD_INDEX_INCR;

#endif  /* TCP_AUTO_DEBUG */

#endif  /* __NETINET_TCP_DEBUG_H */

/*
 * Copyright (c) 1997, Pittsburgh Supercomputing Center, 
 * Jamshid Mahdavi, Matt Mathis, Jeffrey Semke
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 *
 * This is experimental software under active development and may
 * potentially contain bugs.   Use at your own risk.
 *
 */

#ifndef __NETINET_TCP_SCOREBOARD_H
#define __NETINET_TCP_SCOREBOARD_H

/**********************************************************************
 *
 *  Scoreboard module headers: 
 *
 **********************************************************************/

/* Initialize the scoreboard
 */
#define scrb_init(tp) { LIST_INIT(&((tp)->scrb.head));     \
                        (tp)->scrb.last_ack=(tp)->snd_una; \
                        (tp)->snd_retran_data = 0; }

/*  
 *  Check to see if the scoreboard is empty
 *  scrb_isempty(struct tcpcp *tp)
 */
#define scrb_isempty(tp)     (! ((tp)->scrb.scrb_head))

/* This macro quickly takes care of the common case of an empty
 *  scoreboard. Otherwise it called scrb_getnextretran_func to hunt
 *  through the scoreboard and return the next block of data to be
 *  retransmitted.  The start and end of the block are filled in to
 *  start_ptr and end_ptr, and the length of the block is returned.  A
 *  zero return value indicates that there is no data to be
 *  retransmitted at this time.  Note that end_ptr actually points to
 *  the first byte of data which is NOT to be retransmitted (or the
 *  first byte following the data to be retransmitted) similar in
 *  fashion to the rest of this code.
 *
 *  scrb_getnextretran(struct tcpcp *tp, tcp_seq *start, tcp_seq *end) 
 */

#define scrb_getnextretran(tp,start,end)   \
           (scrb_isempty(tp) ? \
             (int)((tcp_seq*)*start = (tcp_seq*)*end = \
             (tcp_seq*)0) \
           : scrb_getnextretran_func(tp,start,end))
                             

/* sender side -- tracks packets sent that WERE selectively acknowledged
 * by the other end.
 * Each sb_entry represents a hole (missing data) followed by
 * consecutive received data.
 */
struct scrb_entry {
      LIST_ENTRY(scrb_entry) ptrs;    /*  Next/Prev structure pointers */
      tcp_seq start;                  /*  Start of received data block */
      tcp_seq end;                    /*  End of received data block      */
      tcp_seq retran;                 /*  End of subsequent data 
					    retransmitted  */
      tcp_seq snd_max;                /*  Value of snd_max at the time of 
					    retransmission */
      int     sack_cnt;               /*  FACK ONLY:  Number of reports for 
					  this hole */
};

#define scrb_next ptrs.le_next       /* next element */
#define scrb_prev ptrs.le_prev       /* previous element */


/* sender side -- tracks packets sent that were selectively
 * acknowledged by the other end
 */
struct scoreboard {
      tcp_seq last_ack;               /* This value replicates snd_una, 
					 but is needed for internal 
					 scoreboard state.  */
      LIST_HEAD(scrb_head_internal, scrb_entry) head; /* Scoreboard list */
};

#define scrb_head head.lh_first       /* first element of scoreboard */

/* return codes from routines that might have to clear the scoreboard
 */
#define E_SCRB_CLEAR          -1
#define E_SCRB_NOERR           0

/* reason parameters for scrb_clear
 */
#define SCRB_INIT              0
#define SCRB_RENEGE            1
#define SCRB_NOMEM             2
#define SCRB_TIMEOUT           3

#endif /* __NETINET_TCP_SCOREBOARD_H */


/*
 *  TCP_FSM - the state machine for TCP processing.
 *            previously this was in pctcp.c and tcp_handler()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "chksum.h"
#include "strings.h"
#include "misc.h"
#include "language.h"
#include "pcconfig.h"
#include "pcqueue.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "ip_out.h"
#include "pcdbug.h"
#include "pcstat.h"
#include "pctcp.h"
#include "tcp_fsm.h"

#if !defined(USE_UDP_ONLY)

#define flag_SYNACK (tcp_FlagSYN | tcp_FlagACK)

typedef int (*tcp_StateProc)  (tcp_Socket**, const in_Header*, tcp_Header*, int);

static int  tcp_listen_state  (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_synsent_state (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_synrec_state  (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_estab_state   (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_estcl_state   (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_finwt1_state  (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_finwt2_state  (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_closewt_state (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_closing_state (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_lastack_state (tcp_Socket**, const in_Header*, tcp_Header*, int);
static int  tcp_timewt_state  (tcp_Socket**, const in_Header*, tcp_Header*, int);

static void tcp_ProcessData (tcp_Socket *s, tcp_Header *tcp, int len, int flags);
static int  tcp_ProcessAck  (tcp_Socket *s, tcp_Header *tcp, long *unack);

static tcp_StateProc tcp_state_tab [] = {
  tcp_listen_state,   /* tcp_StateLISTEN  : listening for connection */
  tcp_synsent_state,  /* tcp_StateSYNSENT : SYN sent, active open */
  tcp_synrec_state,   /* tcp_StateSYNREC  : SYN received, SYN+ACK sent. */
  tcp_estab_state,    /* tcp_StateESTAB   : established */
  tcp_estcl_state,    /* tcp_StateESTCL   : established, but will FIN */
  tcp_finwt1_state,   /* tcp_StateFINWT1  : sent FIN */
  tcp_finwt2_state,   /* tcp_StateFINWT2  : sent FIN, received FIN+ACK */
  tcp_closewt_state,  /* tcp_StateCLOSWT  : received FIN waiting for close */
  tcp_closing_state,  /* tcp_StateCLOSING : sent FIN, received FIN (waiting for FIN+ACK) */
  tcp_lastack_state,  /* tcp_StateLASTACK : FIN received, ACK+FIN sent */
  tcp_timewt_state,   /* tcp_StateTIMEWT  : dally after sending final FIN+ACK */
};


/*
 * _tcp_fsm - Our TCP-input state-machine.
 *
 *   Returns 1 to tcp_handler() if a retransmission is required
 *   immediately or when RTO expires.
 */
int _tcp_fsm (tcp_Socket **sp, const in_Header *ip)
{
  tcp_Header *tcp;
  tcp_Socket *s = *sp;
  WORD       len;
  BYTE       flags;

  if ((signed)s->state < tcp_StateLISTEN || s->state >= tcp_StateCLOSED)
     return (0);   /* cannot happen! */

  len   = in_GetHdrLen (ip);                /* len of IP header   */
  tcp   = (tcp_Header*) ((BYTE*)ip + len);  /* tcp frame pointer  */
  flags = tcp->flags & tcp_FlagMASK;        /* get TCP-head flags */

  return (*tcp_state_tab[s->state]) (sp, ip, tcp, flags);
}
                            
/*
 * LISTEN state
 */
static int tcp_listen_state (tcp_Socket **sp, const in_Header *ip,
                             tcp_Header *tcp, int flags)
{
  tcp_Socket *s = *sp;

  SET_PEER_MAC_ADDR (s, ip);  /* save his ethernet address */

  if (flags & tcp_FlagSYN)
  {
#if defined(USE_BSD_FUNC)
    /*
     * Append the TCB `s' to the listen-queue. The new TCB on output
     * from `_tcp_syn_hook' (_sock_append) is the clone of `s' on
     * input unless the listen-queue is full.
     */
    if (_tcp_syn_hook && (*_tcp_syn_hook)(&s) < 0)
    {
      /* Append failed due to queue full or (temporary) memory shortage.
       * Silently discard SYN. TCB `s' is unchanged.
       */
      CLR_PEER_MAC_ADDR (s);
      return (0);
    }
#endif

    if (ip->tos > s->tos)
       s->tos = ip->tos;
   /*
    * if (ip->tos < s->tos):
    * RFC 793 says we should close connection.
    * We best not do that since SunOS ignores TOS
    */

    s->acknum  = intel (tcp->seqnum) + 1;
    s->flags   = flag_SYNACK;
    s->state   = tcp_StateSYNREC;
    s->unhappy = TRUE;
    TCP_SEND (s);            /* respond immediately */

    s->timeout = set_timeout (tcp_TIMEOUT);
    STAT (tcpstats.tcps_connattempt++);
  }
  else
  {
    if (!(flags & tcp_FlagRST))  /* don't answer RST */
       TCP_RESET (s, ip, tcp);   /* send a reset */
    STAT (tcpstats.tcps_conndrops++);
    CLR_PEER_MAC_ADDR (s);
  }
  return (0);
}

/*
 * SYNSENT state
 */
static int tcp_synsent_state (tcp_Socket **sp, const in_Header *ip,
                              tcp_Header *tcp, int flags)
{
  tcp_Socket *s = *sp;

  if (flags & tcp_FlagSYN)
  {
    if (ip->tos > s->tos)
       s->tos = ip->tos;

    s->flags   = tcp_FlagACK;
    s->timeout = set_timeout (tcp_TIMEOUT);

    /* FlagACK means connection established, else SYNREC
     */
    if (flags & tcp_FlagACK)
    {
      /* but is it for the correct session ?
       */
      if (tcp->acknum == intel(s->seqnum + 1))
      {
        int len = intel16 (ip->length) - in_GetHdrLen (ip);

        s->state = tcp_StateESTAB;
        s->seqnum++;                          /* good increment */
        s->acknum = intel (tcp->seqnum) + 1;
        tcp_ProcessData (s, tcp, len, flags); /* someone may try it */
        s->unhappy = TRUE;                    /* rely on their attempts */
        TCP_SEND (s); /* !! maybe use TCP_SENDSOON() to merge application
                         data into ACK */
      }
      else
      {
        /* wrong ack, force a RST and resend SYN now
         */
        s->flags   = tcp_FlagRST;
        s->unhappy = TRUE;
        TCP_SEND (s);
        s->seqnum = INIT_SEQ();  /* !! should we set a new seq-num? */
        s->flags  = tcp_FlagSYN;
#if defined(USE_DEBUG)
        s->last_seqnum[0] = s->last_seqnum[1] = 0;
#endif
        TCP_SENDSOON (s);     /* !! was TCP_SEND() */
      }

      /* !!to-do: recalculate RTT-timer
       */
    }
    else
    {
      s->acknum++;
      s->state = tcp_StateSYNREC;
      return (0);
    }
  }
  else
  {
    TCP_RESET (s, ip, tcp);
    return (0);              /* Added 18-Aug 1999, GV */
  }
  return (1);
}

/*
 * SYNREC state
 */
static int tcp_synrec_state (tcp_Socket **sp, const in_Header *ip,
                             tcp_Header *tcp, int flags)
{
  tcp_Socket *s = *sp;

  if (flags & tcp_FlagSYN)  /* retransmitted SYN */
  {
    s->flags   = flag_SYNACK;
    s->unhappy = TRUE;
    TCP_SEND (s);
    s->timeout = set_timeout (tcp_TIMEOUT);
    return (0);
  }

  if ((flags & tcp_FlagACK) && (intel(tcp->acknum) == (s->seqnum + 1)))
  {
    s->window = intel16 (tcp->window);
    if (s->window > MAX_WINDOW)
        s->window = MAX_WINDOW;
    s->seqnum++;
    s->flags   = tcp_FlagACK;
    s->state   = tcp_StateESTAB;
    s->timeout = 0UL;            /* never timeout */
    s->unhappy = FALSE;
    return (0);
  }
  ARGSUSED (ip);
  return (1);
}

/*
 * ESTABLISHED state
 */
static int tcp_estab_state (tcp_Socket **sp, const in_Header *ip,
                            tcp_Header *tcp, int flags)
{
  tcp_Socket *s = *sp;
  int   len;
  long  ldiff;
  DWORD ack;

  /* handle lost SYN
   */
  if ((flags & tcp_FlagSYN) && (flags & tcp_FlagACK))
  {
    TCP_SEND (s);
    return (0);
  }

  if (!(flags & tcp_FlagACK))   /* must ack something */
     return (0);

  s->timeout = 0UL;             /* we do not timeout at this point */

  if (!tcp_ProcessAck(s,tcp,&ldiff))
  {
#if defined(USE_DEBUG)
    if (debug_on > 1)
       (*_printf)("tcphandler confused so set unacked "
                  "back to 0 from %u\r\n", s->unacked);
#endif
    STAT (tcpstats.tcps_persistdrop++); /* !! a better counter? */
    s->unacked = 0;
  }
  if (s->unacked < 0)
      s->unacked = 0;

  s->flags = tcp_FlagACK;

  len = intel16 (ip->length) - in_GetHdrLen (ip);
  ack = s->acknum;
  tcp_ProcessData (s, tcp, len, flags);

  if (s->state != tcp_StateCLOSWT &&
      (flags & tcp_FlagFIN)       &&
      s->missed_seg[0] == 0L      &&
      ack == intel(tcp->seqnum))
  {
    s->acknum++;
    if (s->err_msg == NULL)
        s->err_msg = _LANG("Connection closed");

    TCP_SEND (s);

    s->locflags |= LF_GOT_FIN;
    s->flags    |= tcp_FlagFIN;    /* for the tcp_Retransmitter() */
    s->unhappy   = TRUE;
    s->timeout   = set_timeout (tcp_LASTACK_TIME); /* Added AGW 6 Jan 2001 */
    s->state     = tcp_StateLASTACK;
  }

  /*
   * Eliminate the spurious ACK messages bug.
   * For the window update, the length should be the
   * data length only, so exclude the TCP header size
   *  -- Joe <jdhagen@itis.com>
   */
  len -= (tcp->offset << 2);
  if ((ldiff > 0 && s->datalen > 0) || len > 0)
  {
   /* Need to ACK and update window, but how urgent ??
    * Added new window update -GV 21-Nov-1998
    */
    UINT winfree   = s->maxrdatalen - s->rdatalen;
    UINT treshhold = max (s->max_seg/2, winfree);

    if (debug_on > 1)
       (*_printf) ("fastACK: ldiff %ld, th %u\r\n", ldiff, treshhold);

    /* Need a better criteria for doing Fast-ACK
     */
    if (s->missed_seg[0] || ldiff > 0 || len > treshhold)
    {
      s->karn_count = 0;
      s->flags |= tcp_FlagPUSH;  /* !! added */
      TCP_SEND (s);
    }
    else
      TCP_SENDSOON (s);          /* delayed ACK */
  }
  return (0);
}

/*
 * ESTAB_CLOSE state
 */
static int tcp_estcl_state (tcp_Socket **sp, const in_Header *ip,
                            tcp_Header *tcp, int flags)
{
  tcp_estab_state (sp, ip, tcp, flags);
  _tcp_close (*sp);
  return (0);
}

/*
 * CLOSE_WAIT state
 */
static int tcp_closewt_state (tcp_Socket **sp, const in_Header *ip,
                              tcp_Header *tcp, int flags)
{
  return tcp_estab_state (sp, ip, tcp, flags);
}

/*
 * FIN_WAIT1 state -
 */
static int tcp_finwt1_state (tcp_Socket **sp, const in_Header *ip,
                             tcp_Header *tcp, int flags)
{
  long  ldiff;
  int   len = intel16 (ip->length) - in_GetHdrLen (ip);
  DWORD ack, seq;
  tcp_Socket *s = *sp;


 /* They have not necessarily read all the data
  * yet, we must still supply it as requested
  */
  if (tcp_ProcessAck(s,tcp,&ldiff))
  {
    if (ldiff == 0 || s->unacked < 0)
       s->unacked = 0;
  }

  /* they may still be transmitting data, we must read it
   */
  tcp_ProcessData (s, tcp, len, flags);

  ack = intel (tcp->acknum);
  seq = intel (tcp->seqnum);
 

  /* check if other tcp has ACK'ed all sent data and
   * is ready to change states.
   */
  if (s->missed_seg[0] == 0L && (flags & flag_SYNACK) == flag_SYNACK)
  {                         
    if (seq == s->acknum)
    {
      s->acknum++;               /* we must ACK their FIN! */

      if (ack >= s->seqnum + 1)
      {
        /* Not simultaneous close (they've ACKed our FIN)
         * We need to ACK their FIN and move to TIME_WAIT
         */
        s->seqnum++;
        s->timeout = set_timeout (tcp_TIMEWT_TO);
        s->state = tcp_StateTIMEWT;
      }
      else
      {
        /* Simultaneous close (haven't ACKed our FIN yet)
         * We need to ACK their FIN and move to CLOSING
         */
        s->timeout = set_timeout (tcp_TIMEOUT); /* !! S. Lawson, added 12.Nov 1999 */
        s->state   = tcp_StateCLOSING;
      }
      s->flags   = tcp_FlagACK;
      s->unhappy = FALSE;
      TCP_SEND (s);
    }
  }
  else if (flags & tcp_FlagACK)
  {
    /* other side is legitimately acking our FIN
     */
    if ((ack == s->seqnum + 1) &&
        (seq == s->acknum)     &&
        (s->datalen == 0))
    {
      if (!(s->locflags & LF_LINGER))
      {
        _tcp_unthread (s);  /* enters tcp_StateCLOSED */
        return (0);
      }
      s->seqnum++;
      s->state   = tcp_StateFINWT2;
      s->unhappy = FALSE;             /* we don't send anything */
      s->timeout = set_timeout (tcp_TIMEOUT);
    }
    else if ((ack == s->seqnum + 1) && (seq == s->acknum + 1))
    {
     /* !! added 30-Aug 1999 GV
      * Try to stop that annoying retransmission bug/feature(?)
      * from FreeBSD 4.x which increments both SEQ and ACK.
      */
      s->seqnum++;
      s->acknum++;
      s->flags      = tcp_FlagRST;
      s->unhappy    = FALSE;
      s->karn_count = 0;
      s->datalen    = 0;
      TCP_SEND (s);
      _tcp_unthread (s);
      return (0);
    }
  }
  return (1);
}

/*
 * FIN_WAIT2 state
 */
static int tcp_finwt2_state (tcp_Socket **sp, const in_Header *ip,
                             tcp_Header *tcp, int flags)
{
  tcp_Socket *s   = *sp;
  int         len = intel16 (ip->length) - in_GetHdrLen (ip);
  DWORD       ack, seq;

  /* They may still be transmitting data, we must read it
   */
  ack = intel (tcp->acknum);
  seq = intel (tcp->seqnum);

  tcp_ProcessData (s, tcp, len, flags);

  if (s->missed_seg[0])  /* must retransmit to get all data */
     return (1);

  if (flags & tcp_FlagFIN)
     s->locflags |= LF_GOT_FIN;

  if ((flags & flag_SYNACK) == flag_SYNACK &&
      ack == s->seqnum && seq == s->acknum)
  {
    s->acknum++;
    s->flags   = tcp_FlagACK;
    s->unhappy = FALSE;          /* don't send anything */
    s->state   = tcp_StateTIMEWT;
    s->timeout = set_timeout (tcp_TIMEWT_TO);
    TCP_SEND (s);
    return (0);
  }
  return (1);
}


/*
 * CLOSING state
 */
static int tcp_closing_state (tcp_Socket **sp, const in_Header *ip,
                              tcp_Header *tcp, int flags)
{
  tcp_Socket *s = *sp;

  if ((flags & flag_SYNACK) == tcp_FlagACK)  /* ACK, no FIN */
  {
    /* Per FINWT1 above, tcp->acknum should be s->seqnum+1,
     * which should cause us to bump s->seqnum to match
     */
// !! if ((tcp->acknum >= intel(s->seqnum) + 1) &&
      if (((long)(intel(tcp->acknum) - s->seqnum) > 0) && /* AGW - moved intel() so +1 OK 6th Jan 2001 */
          (tcp->seqnum == intel(s->acknum)))
      {
        s->seqnum++;
        s->state   = tcp_StateTIMEWT;
        s->unhappy = FALSE;
        s->timeout = set_timeout (tcp_TIMEWT_TO);
      }
  }
  ARGSUSED (ip);
  return (1);
}

/*
 * LASTACK state
 */
static int tcp_lastack_state (tcp_Socket **sp, const in_Header *ip,
                              tcp_Header *tcp, int flags)
{
  tcp_Socket *s = *sp;

  if (flags & tcp_FlagFIN)
  {
    /* they lost our two packets, back up
     */
    s->locflags |= LF_GOT_FIN;
    s->flags     = flag_SYNACK;
    TCP_SEND (s);
    s->unhappy = TRUE;
    return (0);
  }

//!! if ((intel(tcp->acknum) == (s->seqnum + 1)) &&
  if (((long)(intel(tcp->acknum) - s->seqnum) > 0) && /* AGW allow for any later acks 6th Jan 2001 */

      (intel(tcp->seqnum) == s->acknum))
  {
    s->state   = tcp_StateCLOSED;   /* no 2msl necessary */
    s->unhappy = FALSE;             /* we're done        */
    return (0);
  }
  ARGSUSED (ip);
  return (1);
}

/*
 * TIMEWAIT state
 */
static int tcp_timewt_state (tcp_Socket **sp, const in_Header *ip,
                             tcp_Header *tcp, int flags)
{
  tcp_Socket *s = *sp;

  if ((flags & flag_SYNACK) == flag_SYNACK)
  {
    /* our peer needs an ack
     */
    s->flags   = tcp_FlagACK;
    s->unhappy = FALSE;
    s->state   = tcp_StateCLOSED;  /* support 2 msl in RST code */
    TCP_SEND (s);
  }
  ARGSUSED (ip);
  ARGSUSED (tcp);
  return (1);
}


/*
 * Process the data in an incoming packet.
 * Called from all states where incoming data can be received:
 * SYNSENT, ESTAB, ESTCL, CLOSWT, FIN-WAIT-1 and FIN-WAIT-2
 */
static void tcp_ProcessData (tcp_Socket *s, tcp_Header *tcp, int len, int flags)
{
  long  ldiff, tmpldiff;
  int   diff,  tmpdiff, data_ofs;
  BYTE *data;

  if (s->stress > 0)
      s->stress--;

  s->window = intel16 (tcp->window);
  if (s->window > MAX_WINDOW)
      s->window = MAX_WINDOW;

  ldiff = s->acknum - intel (tcp->seqnum);

  if (flags & tcp_FlagSYN)
     ldiff--;                         /* back up to 0 */

  diff = abs ((int)ldiff);

  /* find the data portion
   */
  data_ofs = tcp->offset << 2;        /* dword to byte offset */
  data     = (BYTE*) tcp + data_ofs;

  if (data_ofs - sizeof(*tcp) > 0)    /* process TCP options */
  {
    BYTE *opt = (BYTE*)(tcp+1);

    while (opt < data)
    {
      switch (*opt)
      {
        case TCPOPT_EOL:
             opt = data;
             break;

        case TCPOPT_NOP:
             opt++;
             break;

        case TCPOPT_MAXSEG:   /* we are very liberal on MSS stuff */
             if (flags & tcp_FlagSYN)
             {
               WORD max_seg = intel16 (*(WORD*)(opt+2));
               if (max_seg < s->max_seg)
                   s->max_seg = max_seg;
             }
             opt += 4;
             break;

        case TCPOPT_TIMESTAMP:
             if ((flags & tcp_FlagSYN) ||
                 ((flags & tcp_FlagACK) && ldiff >= 0)) /* only if SEQ-num ACKs new data */
             {
               s->ts_recent = intel (*(DWORD*)(opt+2));
               s->ts_echo   = intel (*(DWORD*)(opt+6));
               s->locflags |= LF_RCVD_TSTMP;
             }
             opt += 10;
             break;

        case TCPOPT_WINDOW:
             if (flags & tcp_FlagSYN)
             {
               s->recv_wscale = min (TCP_MAX_WINSHIFT, *(opt+2));
               s->locflags |= LF_RCVD_SCALE;
             }
             opt += 4;
             break;
#if 0
        case TCPOPT_SACK_OK:
             if (flags & tcp_FlagSYN)
                s->locflags |= LF_SACK_PERMIT;
             opt += 2;
             break;
#endif

        default:              /* unknown options; type,length,... */
             opt += *(opt+1);
      }
    }
  }


  len -= data_ofs;     /* remove the header length */

#if 0  /* !!to-do */
  /*
   * Handle Out-of-Order data. Raise SIGURG.
   */
  if ((flags & tcp_FlagURG) &&
      tcp->urgent && intel16(tcp->urgent) < len)
  {
    intel16 (tcp->urgent) + intel16 (tcp->seqnum);
    STAT (tcpstats.tcps_rcvoopack++);
    STAT (tcpstats.tcps_rcvoobyte += len);
  }
#endif

  /*
   * SYN/RST segments shouldn't carry any data.
   */
  if ((flags & tcp_FlagSYN) == tcp_FlagSYN ||
      (flags & tcp_FlagRST) == tcp_FlagRST)
     return;


  /* Handle any new data that increments the ACK number
   */
  if (ldiff >= 0)
  {
    data += diff;
    len  -= diff;

    if (s->protoHandler)
        s->acknum += (*s->protoHandler) (s, data, len, NULL, NULL);
    else
    {
      /* no handler, just dump to buffer, should be indexed,
       * handles goofs limit receive size to our window
       */
      if (s->rdatalen >= 0)
      {
        int room = s->maxrdatalen - s->rdatalen;
        if (len > room)
            len = room;

        if (s->missed_seg[0])   /* don't write into missed segment */
        {
          tmpldiff = s->missed_seg[0] - s->acknum;
          tmpdiff  = abs ((int)tmpldiff);
          if (tmpldiff >= 0 && len > tmpdiff)
             len = tmpdiff;
        }
        if (len > 0)
        {
          s->acknum += len;   /* our new ack begins at end of data */
          memcpy (s->rdata + s->rdatalen, data, len);
          s->rdatalen += len;
        }

        if (s->missed_seg[0] && s->missed_seg[0] == s->acknum)
        {
          tmpldiff = s->missed_seg[1] - s->acknum;
          tmpdiff  = abs ((int)tmpldiff);
          if (tmpldiff > 0)
          {
            s->rdatalen += tmpdiff;
            s->acknum = s->missed_seg[1];
          }
          s->missed_seg[0] = 0L;
          s->missed_seg[1] = 0L;
        }
      }
    }

    s->unhappy = s->datalen ? TRUE : FALSE;

    if (ldiff == 0 && s->unacked && chk_timeout(s->rtt_lasttran))
    {
#if defined(USE_DEBUG)
      if (debug_on > 1)
         (*_printf)("data process timeout so set unacked "
                    "back to 0 from %u\r\n", s->unacked);
#endif
      s->unacked = 0;
      STAT (tcpstats.tcps_persistdrop++);  /* !! a better counter? */
    }
  }
  else    /* handle one out-of-segment packet */
  {
    DWORD seqnum = intel (tcp->seqnum);

#if 0
    /* S. Lawson - no out-of-sequence processing of FIN flag
     */
    *flags &= ~tcp_FlagFIN;
#endif

    if (s->missed_seg[0] == 0L)  /* just dropped a segment */
    {                         
      len = min (s->maxrdatalen - s->rdatalen - diff, len);
      if (len > 0)
      {
        memcpy (s->rdata + s->rdatalen + diff, data, len);
        s->missed_seg[0]  = s->missed_seg[1] = seqnum;
        s->missed_seg[1] += len;
      }
    }
    else  /* haven't seen missing segment yet */
    {     
      tmpldiff = s->missed_seg[0] - seqnum;
      tmpdiff  = abs ((int)tmpldiff);

      if (tmpldiff > 0 && len >= tmpdiff)  /* prepend bytes to fragment */
      {   
        memcpy (s->rdata + s->rdatalen + diff, data, tmpdiff);
        s->missed_seg[0] -= tmpdiff;
      }
      tmpldiff = seqnum + len - s->missed_seg[1];
      tmpdiff  = abs ((int)tmpldiff);

      /* append bytes touching fragment
       */
      if (tmpldiff > 0)
      {          
        tmpldiff = s->missed_seg[1] - seqnum;
        if (tmpldiff >= 0)
        {
          /* packet source offset
           */
          int dst, src = abs ((int)tmpldiff);
          tmpldiff = s->missed_seg[1] - s->acknum;

          /* buffer destination offset
           */
          dst = abs ((int)tmpldiff);

          /* length to move
           */
          tmpdiff = min (s->maxrdatalen - s->rdatalen - dst, tmpdiff);
          if (tmpdiff > 0)
          {
            memcpy (s->rdata + s->rdatalen + dst, data + src, tmpdiff);
            s->missed_seg[1] += tmpdiff;
          }
        }
      }
    }
  }
}


/*
 * Process the ACK value in received packet, but only if it falls within
 * current window. Discard queued data that have been acknowledged.
 */
static int tcp_ProcessAck (tcp_Socket *s, tcp_Header *tcp, long *unacked)
{
  long ldiff = (long) (intel(tcp->acknum) - s->seqnum);
  int  diff  = (int) ldiff;

  if (unacked)
     *unacked = ldiff;

  if (ldiff >= 0 && diff <= s->datalen)
  {
    if (s->queuelen)
    {
      s->queue    += diff;
      s->queuelen -= diff;
    }
    else if (diff < s->datalen)
      movmem (s->data+diff, s->data, s->datalen-diff);

    s->datalen -= diff;
    s->unacked -= diff;
    s->seqnum  += ldiff;
    return (1);
  }
  return (0);
}


/*
 * Format and send a reset tcp packet
 */
int _tcp_reset (tcp_Socket *s, const in_Header *his_ip, tcp_Header *old_tcp,
                const char *file, unsigned line)
{
  #include <sys/packon.h>
  struct packet {
         in_Header  ip;
         tcp_Header tcp;
         WORD       options[2];
       } *pkt;
  #include <sys/packoff.h>

  static DWORD next_RST_time = 0UL;

  tcp_PseudoHeader ph;
  BYTE         oldflags;
  in_Header   *ip;
  tcp_Header  *tcp;
  int          temp;

  /* see RFC 793 page 65 for details
   */
  if (next_RST_time && !chk_timeout(next_RST_time))
     return (-1);

  next_RST_time = set_timeout (tcp_RST_TIME);

  oldflags = old_tcp->flags;
  if (oldflags & tcp_FlagRST)
     return (-1);

  if (oldflags & tcp_FlagACK)                        /* ACK */
  {
    old_tcp->seqnum = old_tcp->acknum;
    old_tcp->acknum = 0;
    oldflags        = tcp_FlagRST;
  }
  else if ((oldflags & flag_SYNACK) == tcp_FlagSYN)  /* SYN, no ACK */
  {
    old_tcp->acknum = intel (intel(old_tcp->seqnum) + 1);
    old_tcp->seqnum = 0;
    oldflags        = tcp_FlagACK | tcp_FlagRST;
  }
  else
  {
    temp = intel16 (his_ip->length) - in_GetHdrLen (his_ip);
    old_tcp->acknum = intel (intel(old_tcp->seqnum) + temp);
    old_tcp->seqnum = 0;
    oldflags        = tcp_FlagRST;
  }

  if (s && (s->locflags & LF_NOPUSH))
     oldflags &= ~tcp_FlagPUSH;

  pkt = (struct packet*) _eth_formatpacket (MAC_SRC(his_ip), IP_TYPE);
  ip  = &pkt->ip;
  tcp = &pkt->tcp;

  /* tcp header
   */
  tcp->srcPort  = old_tcp->dstPort;
  tcp->dstPort  = old_tcp->srcPort;
  tcp->seqnum   = old_tcp->seqnum;
  tcp->acknum   = old_tcp->acknum;
  tcp->window   = 0;
  tcp->flags    = (oldflags | tcp_FlagRST);
  tcp->unused   = 0;
  tcp->offset   = sizeof(*tcp) / 4;
  tcp->checksum = 0;
  tcp->urgent   = 0;

  memset (&ph, 0, sizeof(ph));
  ph.src      = his_ip->destination;   /* already network order */
  ph.dst      = his_ip->source;
  ph.protocol = TCP_PROTO;
  ph.length   = intel16 (sizeof(*tcp));
  ph.checksum = checksum (tcp, sizeof(*tcp));

  tcp->checksum = ~checksum (&ph, sizeof(ph));

  return _ip_output (ip, ph.src, ph.dst, TCP_PROTO,
                     s ? s->ttl : _default_ttl,
                     his_ip->tos, 0, sizeof(*tcp), s, file, line);
}

#endif /* !USE_UDP_ONLY */

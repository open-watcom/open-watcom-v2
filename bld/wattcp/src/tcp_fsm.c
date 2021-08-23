
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

#if defined(USE_BSD_FUNC)
    #include "socket.h"
    #include "pchooks.h"
#endif

#if !defined(USE_UDP_ONLY)

#define flag_SYNACK (tcp_FlagSYN | tcp_FlagACK)

typedef int (*tcp_StateProc)  (sock_type **skp, const in_Header*, tcp_Header *tcp_hdr, int);

static int  tcp_listen_state  (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_synsent_state (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_synrec_state  (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_estab_state   (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_estcl_state   (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_finwt1_state  (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_finwt2_state  (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_closewt_state (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_closing_state (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_lastack_state (sock_type **skp, const in_Header*, tcp_Header*, int);
static int  tcp_timewt_state  (sock_type **skp, const in_Header*, tcp_Header*, int);

static void tcp_ProcessData (sock_type *sk, tcp_Header *tcp_hdr, int len, int flags);
static int  tcp_ProcessAck  (sock_type *sk, tcp_Header *tcp_hdr, long *unack);

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
int _tcp_fsm (sock_type **skp, const in_Header *ip)
{
    tcp_Header *tcp_hdr;
    sock_type  *sk = *skp;
    WORD       len;
    BYTE       flags;

    if ((signed)sk->tcp.state < tcp_StateLISTEN || sk->tcp.state >= tcp_StateCLOSED)
        return (0);   /* cannot happen! */

    len = in_GetHdrLen (ip);                    /* len of IP header   */
    tcp_hdr = (tcp_Header*) ((BYTE*)ip + len);  /* tcp frame pointer  */
    flags = tcp_hdr->flags & tcp_FlagMASK;      /* get TCP-head flags */

    return (*tcp_state_tab[sk->tcp.state]) (skp, ip, tcp_hdr, flags);
}

/*
 * LISTEN state
 */
static int tcp_listen_state (sock_type **skp, const in_Header *ip,
                             tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;

    SET_PEER_MAC_ADDR (&sk->tcp, ip);  /* save his ethernet address */

    if (flags & tcp_FlagSYN) {
#if defined(USE_BSD_FUNC)
        /*
         * Append the TCB `s' to the listen-queue. The new TCB on output
         * from `_tcp_syn_hook' (_sock_append) is the clone of `s' on
         * input unless the listen-queue is full.
         */
        if (_tcp_syn_hook != NULL && (*_tcp_syn_hook)(&sk) < 0) {
            /* Append failed due to queue full or (temporary) memory shortage.
             * Silently discard SYN. TCB `s' is unchanged.
             */
            CLR_PEER_MAC_ADDR (&sk->tcp);
            return (0);
        }
#endif

        if (ip->tos > sk->tcp.tos)
            sk->tcp.tos = ip->tos;
        /*
         * if (ip->tos < sk->tcp.tos):
         * RFC 793 says we should close connection.
         * We best not do that since SunOS ignores TOS
         */

        sk->tcp.acknum  = intel (tcp_hdr->seqnum) + 1;
        sk->tcp.flags   = flag_SYNACK;
        sk->tcp.state   = tcp_StateSYNREC;
        sk->tcp.unhappy = TRUE;
        TCP_SEND (sk);                  /* respond immediately */

        sk->tcp.timeout = set_timeout (tcp_TIMEOUT);
        STAT (tcpstats.tcps_connattempt++);
    } else {
        if (!(flags & tcp_FlagRST))     /* don't answer RST */
            TCP_RESET (sk, ip, tcp_hdr);  /* send a reset */
        STAT (tcpstats.tcps_conndrops++);
        CLR_PEER_MAC_ADDR (&sk->tcp);
    }
    return (0);
}

/*
 * SYNSENT state
 */
static int tcp_synsent_state (sock_type **skp, const in_Header *ip,
                              tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;

    if (flags & tcp_FlagSYN) {
        if (ip->tos > sk->tcp.tos)
            sk->tcp.tos = ip->tos;

        sk->tcp.flags   = tcp_FlagACK;
        sk->tcp.timeout = set_timeout (tcp_TIMEOUT);

        /* FlagACK means connection established, else SYNREC
         */
        if (flags & tcp_FlagACK) {
            /* but is it for the correct session ?
             */
            if (tcp_hdr->acknum == intel(sk->tcp.seqnum + 1)) {
                int len = intel16 (ip->length) - in_GetHdrLen (ip);

                sk->tcp.state = tcp_StateESTAB;
                sk->tcp.seqnum++;                          /* good increment */
                sk->tcp.acknum = intel (tcp_hdr->seqnum) + 1;
                tcp_ProcessData (sk, tcp_hdr, len, flags); /* someone may try it */
                sk->tcp.unhappy = TRUE;                    /* rely on their attempts */
                TCP_SEND (sk); /* !! maybe use TCP_SENDSOON() to merge application data into ACK */
            } else {
                /* wrong ack, force a RST and resend SYN now
                 */
                sk->tcp.flags   = tcp_FlagRST;
                sk->tcp.unhappy = TRUE;
                TCP_SEND (sk);
                sk->tcp.seqnum = INIT_SEQ();  /* !! should we set a new seq-num? */
                sk->tcp.flags  = tcp_FlagSYN;
#if defined(USE_DEBUG)
                sk->tcp.last_seqnum[0] = sk->tcp.last_seqnum[1] = 0;
#endif
                TCP_SENDSOON (sk);     /* !! was TCP_SEND() */
            }

            /* !!to-do: recalculate RTT-timer
             */
        } else {
            sk->tcp.acknum++;
            sk->tcp.state = tcp_StateSYNREC;
            return (0);
        }
    } else {
        TCP_RESET (sk, ip, tcp_hdr);
        return (0);              /* Added 18-Aug 1999, GV */
    }
    return (1);
}

/*
 * SYNREC state
 */
static int tcp_synrec_state (sock_type **skp, const in_Header *ip,
                             tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;

    if (flags & tcp_FlagSYN) { /* retransmitted SYN */
        sk->tcp.flags   = flag_SYNACK;
        sk->tcp.unhappy = TRUE;
        TCP_SEND (sk);
        sk->tcp.timeout = set_timeout (tcp_TIMEOUT);
        return (0);
    }

    if ((flags & tcp_FlagACK) && (intel(tcp_hdr->acknum) == (sk->tcp.seqnum + 1))) {
        sk->tcp.window = intel16 (tcp_hdr->window);
        if (sk->tcp.window > MAX_WINDOW)
            sk->tcp.window = MAX_WINDOW;
        sk->tcp.seqnum++;
        sk->tcp.flags   = tcp_FlagACK;
        sk->tcp.state   = tcp_StateESTAB;
        sk->tcp.timeout = 0UL;            /* never timeout */
        sk->tcp.unhappy = FALSE;
        return (0);
    }
    ARGSUSED (ip);
    return (1);
}

/*
 * ESTABLISHED state
 */
static int tcp_estab_state (sock_type **skp, const in_Header *ip,
                            tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;
    int   len;
    long  ldiff;
    DWORD ack;

    /* handle lost SYN
     */
    if ((flags & tcp_FlagSYN) && (flags & tcp_FlagACK)) {
        TCP_SEND (sk);
        return (0);
    }

    if (!(flags & tcp_FlagACK))   /* must ack something */
        return (0);

    sk->tcp.timeout = 0UL;             /* we do not timeout at this point */

    if (!tcp_ProcessAck(sk, tcp_hdr, &ldiff)) {
#if defined(USE_DEBUG)
        if (debug_on > 1) {
            (*_printf)("tcphandler confused so set unacked "
                  "back to 0 from %u\r\n", sk->tcp.unacked);
        }
#endif
        STAT (tcpstats.tcps_persistdrop++); /* !! a better counter? */
        sk->tcp.unacked = 0;
    }
    if (sk->tcp.unacked < 0)
        sk->tcp.unacked = 0;

    sk->tcp.flags = tcp_FlagACK;

    len = intel16 (ip->length) - in_GetHdrLen (ip);
    ack = sk->tcp.acknum;
    tcp_ProcessData (sk, tcp_hdr, len, flags);

    if (sk->tcp.state != tcp_StateCLOSWT &&
      (flags & tcp_FlagFIN)       &&
      sk->tcp.missed_seg[0] == 0L      &&
      ack == intel(tcp_hdr->seqnum))
    {
        sk->tcp.acknum++;
        if (sk->tcp.err_msg == NULL)
            sk->tcp.err_msg = _LANG("Connection closed");

        TCP_SEND (sk);

        sk->tcp.locflags |= LF_GOT_FIN;
        sk->tcp.flags    |= tcp_FlagFIN;    /* for the tcp_Retransmitter() */
        sk->tcp.unhappy   = TRUE;
        sk->tcp.timeout   = set_timeout (tcp_LASTACK_TIME); /* Added AGW 6 Jan 2001 */
        sk->tcp.state     = tcp_StateLASTACK;
    }

    /*
     * Eliminate the spurious ACK messages bug.
     * For the window update, the length should be the
     * data length only, so exclude the TCP header size
     *  -- Joe <jdhagen@itis.com>
     */
    len -= (tcp_hdr->offset << 2);
    if ((ldiff > 0 && sk->tcp.tx_datalen > 0) || len > 0) {
        /* Need to ACK and update window, but how urgent ??
         * Added new window update -GV 21-Nov-1998
         */
        UINT winfree   = sk->tcp.rx_maxdatalen - sk->tcp.rx_datalen;
        UINT treshhold = max (sk->tcp.max_seg/2, winfree);

        if (debug_on > 1)
            (*_printf) ("fastACK: ldiff %ld, th %u\r\n", ldiff, treshhold);

        /* Need a better criteria for doing Fast-ACK
         */
        if (sk->tcp.missed_seg[0] || ldiff > 0 || len > treshhold) {
            sk->tcp.karn_count = 0;
            sk->tcp.flags |= tcp_FlagPUSH;  /* !! added */
            TCP_SEND (sk);
        } else {
            TCP_SENDSOON (sk);          /* delayed ACK */
        }
    }
    return (0);
}

/*
 * ESTAB_CLOSE state
 */
static int tcp_estcl_state (sock_type **skp, const in_Header *ip,
                            tcp_Header *tcp_hdr, int flags)
{
    tcp_estab_state (skp, ip, tcp_hdr, flags);
    _tcp_close (*skp);
    return (0);
}

/*
 * CLOSE_WAIT state
 */
static int tcp_closewt_state (sock_type **skp, const in_Header *ip,
                              tcp_Header *tcp_hdr, int flags)
{
    return tcp_estab_state (skp, ip, tcp_hdr, flags);
}

/*
 * FIN_WAIT1 state -
 */
static int tcp_finwt1_state (sock_type **skp, const in_Header *ip,
                             tcp_Header *tcp_hdr, int flags)
{
    long  ldiff;
    int   len = intel16 (ip->length) - in_GetHdrLen (ip);
    DWORD ack, seq;
    sock_type *sk = *skp;


    /* They have not necessarily read all the data
     * yet, we must still supply it as requested
     */
    if (tcp_ProcessAck(sk, tcp_hdr, &ldiff)) {
        if (ldiff == 0 || sk->tcp.unacked < 0) {
            sk->tcp.unacked = 0;
        }
    }

    /* they may still be transmitting data, we must read it
     */
    tcp_ProcessData (sk, tcp_hdr, len, flags);

    ack = intel (tcp_hdr->acknum);
    seq = intel (tcp_hdr->seqnum);


    /* check if other tcp has ACK'ed all sent data and
     * is ready to change states.
     */
    if (sk->tcp.missed_seg[0] == 0L && (flags & flag_SYNACK) == flag_SYNACK) {
        if (seq == sk->tcp.acknum) {
            sk->tcp.acknum++;               /* we must ACK their FIN! */

            if (ack >= sk->tcp.seqnum + 1) {
                /* Not simultaneous close (they've ACKed our FIN)
                 * We need to ACK their FIN and move to TIME_WAIT
                 */
                sk->tcp.seqnum++;
                sk->tcp.timeout = set_timeout (tcp_TIMEWT_TO);
                sk->tcp.state = tcp_StateTIMEWT;
            } else {
                /* Simultaneous close (haven't ACKed our FIN yet)
                 * We need to ACK their FIN and move to CLOSING
                 */
                sk->tcp.timeout = set_timeout (tcp_TIMEOUT); /* !! S. Lawson, added 12.Nov 1999 */
                sk->tcp.state   = tcp_StateCLOSING;
            }
            sk->tcp.flags   = tcp_FlagACK;
            sk->tcp.unhappy = FALSE;
            TCP_SEND (sk);
        }
    } else if (flags & tcp_FlagACK) {
        /* other side is legitimately acking our FIN
         */
        if ((ack == sk->tcp.seqnum + 1) &&
            (seq == sk->tcp.acknum)     &&
            (sk->tcp.tx_datalen == 0))
        {
            if (!(sk->tcp.locflags & LF_LINGER)) {
                _tcp_unthread (sk);  /* enters tcp_StateCLOSED */
                return (0);
            }
            sk->tcp.seqnum++;
            sk->tcp.state   = tcp_StateFINWT2;
            sk->tcp.unhappy = FALSE;             /* we don't send anything */
            sk->tcp.timeout = set_timeout (tcp_TIMEOUT);
        } else if ((ack == sk->tcp.seqnum + 1) && (seq == sk->tcp.acknum + 1)) {
            /* !! added 30-Aug 1999 GV
             * Try to stop that annoying retransmission bug/feature(?)
             * from FreeBSD 4.x which increments both SEQ and ACK.
             */
            sk->tcp.seqnum++;
            sk->tcp.acknum++;
            sk->tcp.flags      = tcp_FlagRST;
            sk->tcp.unhappy    = FALSE;
            sk->tcp.karn_count = 0;
            sk->tcp.tx_datalen  = 0;
            TCP_SEND (sk);
            _tcp_unthread (sk);
            return (0);
        }
    }
    return (1);
}

/*
 * FIN_WAIT2 state
 */
static int tcp_finwt2_state (sock_type **skp, const in_Header *ip,
                             tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;
    int         len = intel16 (ip->length) - in_GetHdrLen (ip);
    DWORD       ack, seq;

    /* They may still be transmitting data, we must read it
    */
    ack = intel (tcp_hdr->acknum);
    seq = intel (tcp_hdr->seqnum);

    tcp_ProcessData (sk, tcp_hdr, len, flags);

    if (sk->tcp.missed_seg[0])  /* must retransmit to get all data */
        return (1);

    if (flags & tcp_FlagFIN)
        sk->tcp.locflags |= LF_GOT_FIN;

    if ((flags & flag_SYNACK) == flag_SYNACK &&
        ack == sk->tcp.seqnum && seq == sk->tcp.acknum)
    {
        sk->tcp.acknum++;
        sk->tcp.flags   = tcp_FlagACK;
        sk->tcp.unhappy = FALSE;          /* don't send anything */
        sk->tcp.state   = tcp_StateTIMEWT;
        sk->tcp.timeout = set_timeout (tcp_TIMEWT_TO);
        TCP_SEND (sk);
        return (0);
    }
    return (1);
}


/*
 * CLOSING state
 */
static int tcp_closing_state (sock_type **skp, const in_Header *ip,
                              tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;

    if ((flags & flag_SYNACK) == tcp_FlagACK) { /* ACK, no FIN */
        /* Per FINWT1 above, tcp_hdr->acknum should be sk->tcp.seqnum+1,
         * which should cause us to bump sk->tcp.seqnum to match
         */
//!!        if ((tcp_hdr->acknum >= intel(sk->tcp.seqnum) + 1) &&
        if (((long)(intel(tcp_hdr->acknum) - sk->tcp.seqnum) > 0) && /* AGW - moved intel() so +1 OK 6th Jan 2001 */
            (tcp_hdr->seqnum == intel(sk->tcp.acknum)))
        {
            sk->tcp.seqnum++;
            sk->tcp.state   = tcp_StateTIMEWT;
            sk->tcp.unhappy = FALSE;
            sk->tcp.timeout = set_timeout (tcp_TIMEWT_TO);
        }
    }
    ARGSUSED (ip);
    return (1);
}

/*
 * LASTACK state
 */
static int tcp_lastack_state (sock_type **skp, const in_Header *ip,
                              tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;

    if (flags & tcp_FlagFIN) {
        /* they lost our two packets, back up
         */
        sk->tcp.locflags |= LF_GOT_FIN;
        sk->tcp.flags     = flag_SYNACK;
        TCP_SEND (sk);
        sk->tcp.unhappy = TRUE;
        return (0);
    }

//!!    if ((intel(tcp_hdr->acknum) == (sk->tcp.seqnum + 1)) &&
    if (((long)(intel(tcp_hdr->acknum) - sk->tcp.seqnum) > 0) && /* AGW allow for any later acks 6th Jan 2001 */
      (intel(tcp_hdr->seqnum) == sk->tcp.acknum))
    {
        sk->tcp.state   = tcp_StateCLOSED;   /* no 2msl necessary */
        sk->tcp.unhappy = FALSE;             /* we're done        */
        return (0);
    }
    ARGSUSED (ip);
    return (1);
}

/*
 * TIMEWAIT state
 */
static int tcp_timewt_state (sock_type **skp, const in_Header *ip,
                             tcp_Header *tcp_hdr, int flags)
{
    sock_type *sk = *skp;

    if ((flags & flag_SYNACK) == flag_SYNACK) {
        /* our peer needs an ack
         */
        sk->tcp.flags   = tcp_FlagACK;
        sk->tcp.unhappy = FALSE;
        sk->tcp.state   = tcp_StateCLOSED;  /* support 2 msl in RST code */
        TCP_SEND (sk);
    }
    ARGSUSED (ip);
    ARGSUSED (tcp_hdr);
    return (1);
}


/*
 * Process the data in an incoming packet.
 * Called from all states where incoming data can be received:
 * SYNSENT, ESTAB, ESTCL, CLOSWT, FIN-WAIT-1 and FIN-WAIT-2
 */
static void tcp_ProcessData (sock_type *sk, tcp_Header *tcp_hdr, int len, int flags)
{
    long  ldiff, tmpldiff;
    int   diff,  tmpdiff, data_ofs;
    BYTE *data;

    if (sk->tcp.stress > 0)
        sk->tcp.stress--;

    sk->tcp.window = intel16 (tcp_hdr->window);
    if (sk->tcp.window > MAX_WINDOW)
        sk->tcp.window = MAX_WINDOW;

    ldiff = sk->tcp.acknum - intel (tcp_hdr->seqnum);

    if (flags & tcp_FlagSYN)
        ldiff--;                         /* back up to 0 */

    diff = abs ((int)ldiff);

    /* find the data portion
     */
    data_ofs = tcp_hdr->offset << 2;        /* dword to byte offset */
    data     = (BYTE*) tcp_hdr + data_ofs;

    if (data_ofs - sizeof(*tcp_hdr) > 0) {  /* process TCP options */
        BYTE *opt = (BYTE*)(tcp_hdr+1);

        while (opt < data) {
            switch (*opt) {
            case TCPOPT_EOL:
                opt = data;
                break;
            case TCPOPT_NOP:
                opt++;
                break;
            case TCPOPT_MAXSEG:   /* we are very liberal on MSS stuff */
                if (flags & tcp_FlagSYN) {
                    WORD max_seg = intel16 (*(WORD*)(opt+2));
                    if (max_seg < sk->tcp.max_seg)
                        sk->tcp.max_seg = max_seg;
                }
                opt += 4;
                break;
            case TCPOPT_TIMESTAMP:
                if ((flags & tcp_FlagSYN) ||
                    ((flags & tcp_FlagACK) && ldiff >= 0)) /* only if SEQ-num ACKs new data */
                {
                    sk->tcp.ts_recent = intel (*(DWORD*)(opt+2));
                    sk->tcp.ts_echo   = intel (*(DWORD*)(opt+6));
                    sk->tcp.locflags |= LF_RCVD_TSTMP;
                }
                opt += 10;
                break;
            case TCPOPT_WINDOW:
                if (flags & tcp_FlagSYN) {
                    sk->tcp.rx_wscale = min (TCP_MAX_WINSHIFT, *(opt+2));
                    sk->tcp.locflags |= LF_RCVD_SCALE;
                }
                opt += 3;
                break;
#if 0
            case TCPOPT_SACK_OK:
                if (flags & tcp_FlagSYN)
                    sk->tcp.locflags |= LF_SACK_PERMIT;
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
        tcp_hdr->urgent && intel16(tcp_hdr->urgent) < len)
    {
        intel16 (tcp_hdr->urgent) + intel16 (tcp_hdr->seqnum);
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
    if (ldiff >= 0) {
        data += diff;
        len  -= diff;

        if (sk->tcp.protoHandler != NULL) {
            sk->tcp.acknum += (*sk->tcp.protoHandler) (sk, data, len, NULL, NULL);
        } else {
            /* no handler, just dump to buffer, should be indexed,
             * handles goofs limit receive size to our window
             */
            if (sk->tcp.rx_datalen >= 0) {
                int room = sk->tcp.rx_maxdatalen - sk->tcp.rx_datalen;
                if (len > room)
                    len = room;

                if (sk->tcp.missed_seg[0]) { /* don't write into missed segment */
                    tmpldiff = sk->tcp.missed_seg[0] - sk->tcp.acknum;
                    tmpdiff  = abs ((int)tmpldiff);
                    if (tmpldiff >= 0 && len > tmpdiff) {
                        len = tmpdiff;
                    }
                }
                if (len > 0) {
                    sk->tcp.acknum += len;   /* our new ack begins at end of data */
                    memcpy (sk->tcp.rx_data + sk->tcp.rx_datalen, data, len);
                    sk->tcp.rx_datalen += len;
                }

                if (sk->tcp.missed_seg[0] && sk->tcp.missed_seg[0] == sk->tcp.acknum) {
                    tmpldiff = sk->tcp.missed_seg[1] - sk->tcp.acknum;
                    tmpdiff  = abs ((int)tmpldiff);
                    if (tmpldiff > 0) {
                        sk->tcp.rx_datalen += tmpdiff;
                        sk->tcp.acknum = sk->tcp.missed_seg[1];
                    }
                    sk->tcp.missed_seg[0] = 0L;
                    sk->tcp.missed_seg[1] = 0L;
                }
            }
        }

        sk->tcp.unhappy = sk->tcp.tx_datalen ? TRUE : FALSE;

        if (ldiff == 0 && sk->tcp.unacked && chk_timeout(sk->tcp.rtt_lasttran)) {
#if defined(USE_DEBUG)
            if (debug_on > 1)
                (*_printf)("data process timeout so set unacked "
                    "back to 0 from %u\r\n", sk->tcp.unacked);
#endif
            sk->tcp.unacked = 0;
            STAT (tcpstats.tcps_persistdrop++);  /* !! a better counter? */
        }
    } else {  /* handle one out-of-segment packet */
        DWORD seqnum = intel (tcp_hdr->seqnum);

#if 0
        /* S. Lawson - no out-of-sequence processing of FIN flag
         */
        *flags &= ~tcp_FlagFIN;
#endif

        if (sk->tcp.missed_seg[0] == 0L) { /* just dropped a segment */
            len = min (sk->tcp.rx_maxdatalen - sk->tcp.rx_datalen - diff, len);
            if (len > 0) {
                memcpy (sk->tcp.rx_data + sk->tcp.rx_datalen + diff, data, len);
                sk->tcp.missed_seg[0]  = sk->tcp.missed_seg[1] = seqnum;
                sk->tcp.missed_seg[1] += len;
            }
        } else { /* haven't seen missing segment yet */
            tmpldiff = sk->tcp.missed_seg[0] - seqnum;
            tmpdiff  = abs ((int)tmpldiff);

            if (tmpldiff > 0 && len >= tmpdiff) { /* prepend bytes to fragment */
                memcpy (sk->tcp.rx_data + sk->tcp.rx_datalen + diff, data, tmpdiff);
                sk->tcp.missed_seg[0] -= tmpdiff;
            }
            tmpldiff = seqnum + len - sk->tcp.missed_seg[1];
            tmpdiff  = abs ((int)tmpldiff);

            /* append bytes touching fragment
             */
            if (tmpldiff > 0) {
                tmpldiff = sk->tcp.missed_seg[1] - seqnum;
                if (tmpldiff >= 0) {
                    /* packet source offset
                     */
                    int dst, src = abs ((int)tmpldiff);
                    tmpldiff = sk->tcp.missed_seg[1] - sk->tcp.acknum;

                    /* buffer destination offset
                     */
                    dst = abs ((int)tmpldiff);

                    /* length to move
                     */
                    tmpdiff = min (sk->tcp.rx_maxdatalen - sk->tcp.rx_datalen - dst, tmpdiff);
                    if (tmpdiff > 0) {
                        memcpy (sk->tcp.rx_data + sk->tcp.rx_datalen + dst, data + src, tmpdiff);
                        sk->tcp.missed_seg[1] += tmpdiff;
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
static int tcp_ProcessAck (sock_type *sk, tcp_Header *tcp_hdr, long *unacked)
{
    long ldiff = (long) (intel(tcp_hdr->acknum) - sk->tcp.seqnum);
    int  diff  = (int) ldiff;

    if (unacked)
        *unacked = ldiff;

    if (ldiff >= 0 && diff <= sk->tcp.tx_datalen) {
        if (sk->tcp.queuelen) {
            sk->tcp.queue    += diff;
            sk->tcp.queuelen -= diff;
        } else if (diff < sk->tcp.tx_datalen) {
            movmem (sk->tcp.tx_data + diff, sk->tcp.tx_data, sk->tcp.tx_datalen - diff);
        }

        sk->tcp.tx_datalen -= diff;
        sk->tcp.unacked -= diff;
        sk->tcp.seqnum  += ldiff;
        return (1);
    }
    return (0);
}


/*
 * Format and send a reset tcp packet
 */
int _tcp_reset (sock_type *sk, const in_Header *his_ip, tcp_Header *old_tcp_hdr,
                const char *file, unsigned line)
{
    #include <sys/packon.h>
    struct packet {
         in_Header  ip;
         tcp_Header tcp_hdr;
         WORD       options[2];
       } *pkt;
    #include <sys/packoff.h>

    static DWORD next_RST_time = 0UL;

    tcp_PseudoHeader  tcp_phdr;
    BYTE              oldflags;
    in_Header         *ip;
    tcp_Header        *tcp_hdr;
    int               temp;

    /* see RFC 793 page 65 for details
     */
    if (next_RST_time && !chk_timeout(next_RST_time))
        return (-1);

    next_RST_time = set_timeout (tcp_RST_TIME);

    oldflags = old_tcp_hdr->flags;
    if (oldflags & tcp_FlagRST)
        return (-1);

    if (oldflags & tcp_FlagACK) {                      /* ACK */
        old_tcp_hdr->seqnum = old_tcp_hdr->acknum;
        old_tcp_hdr->acknum = 0;
        oldflags        = tcp_FlagRST;
    } else if ((oldflags & flag_SYNACK) == tcp_FlagSYN) { /* SYN, no ACK */
        old_tcp_hdr->acknum = intel (intel(old_tcp_hdr->seqnum) + 1);
        old_tcp_hdr->seqnum = 0;
        oldflags        = tcp_FlagACK | tcp_FlagRST;
    } else {
        temp = intel16 (his_ip->length) - in_GetHdrLen (his_ip);
        old_tcp_hdr->acknum = intel (intel(old_tcp_hdr->seqnum) + temp);
        old_tcp_hdr->seqnum = 0;
        oldflags        = tcp_FlagRST;
    }

    if (sk != NULL && (sk->tcp.locflags & LF_NOPUSH))
        oldflags &= ~tcp_FlagPUSH;

    pkt = (struct packet*) _eth_formatpacket (MAC_SRC(his_ip), IP_TYPE);
    ip  = &pkt->ip;
    tcp_hdr = &pkt->tcp_hdr;

    /* tcp header
     */
    tcp_hdr->srcPort  = old_tcp_hdr->dstPort;
    tcp_hdr->dstPort  = old_tcp_hdr->srcPort;
    tcp_hdr->seqnum   = old_tcp_hdr->seqnum;
    tcp_hdr->acknum   = old_tcp_hdr->acknum;
    tcp_hdr->window   = 0;
    tcp_hdr->flags    = (oldflags | tcp_FlagRST);
    tcp_hdr->unused   = 0;
    tcp_hdr->offset   = sizeof(*tcp_hdr) / 4;
    tcp_hdr->checksum = 0;
    tcp_hdr->urgent   = 0;

    memset (&tcp_phdr, 0, sizeof(tcp_phdr));
    tcp_phdr.src      = his_ip->destination;   /* already network order */
    tcp_phdr.dst      = his_ip->source;
    tcp_phdr.protocol = TCP_PROTO;
    tcp_phdr.length   = intel16 (sizeof(*tcp_hdr));
    tcp_phdr.checksum = checksum (tcp_hdr, sizeof(*tcp_hdr));

    tcp_hdr->checksum = ~checksum (&tcp_phdr, sizeof(tcp_phdr));

    return _ip_output (ip, tcp_phdr.src, tcp_phdr.dst, TCP_PROTO,
                     sk != NULL ? sk->tcp.ttl : _default_ttl,
                     his_ip->tos, 0, sizeof(*tcp_hdr), sk, file, line);
}

#endif /* !USE_UDP_ONLY */

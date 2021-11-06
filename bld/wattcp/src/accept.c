/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 *   0.6 : Sep 16, 1999 : fixes by Claus Oberste-Brandenburg
 *   0.7 : Nov 22, 1999 : G. Vanem - simplified the socket 'dup' action.
 *                        Simply allocate a new 'remote_addr'.
 *                        Poll backlogged listen-queue in accept-loop.
 *   0.8 : Dec 15, 1999 : Listen-queue filled in _sock_append() and TCB
 *                        is cloned when SYN is received in tcp_listen_state().
 *   0.9 : Mar 09, 2000 : Plugged a memory leak in dup_bind() where 'tcp_sock'
 *                        memory from socket() wasn't free'ed. Thanks to
 *                        Francisco Pastor <fpastor.etra-id@etra.es> for
 *                        finding this.
 *   0.91: Jun 01, 2000 : Rearranged acccept-loop for EWOULDBLOCK with no
 *                        listen-queue element (dropped 'continue' construct).
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

#include "pcbufsiz.h"
#include "pchooks.h"

static int dup_bind   (Socket *socket, Socket **clone, int idx);
static int alloc_addr (Socket *socket, Socket  *clone);
static int listen_free(Socket *socket, int idx);


int accept (int s, struct sockaddr *addr, socklen_t *addrlen)
{
    Socket  *clone, *socket;
    volatile DWORD   timeout;
    volatile int     newsock = -1;
    volatile int     que_idx;
    volatile int     maxconn;

    socket = _socklist_find (s);

    SOCK_PROLOGUE (socket, "\naccept:%d", s);

    if (!socket->local_addr) {
        SOCK_DEBUGF ((socket, ", not bound"));
        SOCK_ERR (ENOTCONN);
        return (-1);
    }

    if (socket->so_type != SOCK_STREAM) {
        SOCK_DEBUGF ((socket, ", EOPNOTSUPP"));
        SOCK_ERR (EOPNOTSUPP);
        return (-1);
    }

    if (!(socket->so_options & SO_ACCEPTCONN)) { /* listen() not called */
        SOCK_DEBUGF ((socket, ", not SO_ACCEPTCONN"));
        SOCK_ERR (EINVAL);
        return (-1);
    }

    if (!(socket->so_state & (SS_ISLISTENING | SS_ISCONNECTING))) {
        SOCK_DEBUGF ((socket, ", not listening"));
        SOCK_ERR (ENOTCONN);
        return (-1);
    }

    if (addr && addrlen) {
        if (*addrlen < sizeof(*addr)) {
            SOCK_DEBUGF ((socket, ", EFAULT"));
            SOCK_ERR (EFAULT);
            return (-1);
        }
        VERIFY_RW (addr, *addrlen);
    }

    /* Get max possible TCBs on listen-queue.
     * Some (or all) may be NULL until a SYN comes in.
     */
    maxconn = socket->backlog;
    if (maxconn < 1 || maxconn > SOMAXCONN) {
#if defined(USE_BSD_FATAL)
        SOCK_FATAL (("%s(%d): Illegal socket backlog %d",
                __FILE__, __LINE__, maxconn));
#else
        SOCK_ERR (EINVAL);
        return (-1);
#endif
    }

    if (socket->timeout) {
        timeout = set_timeout (1000 * socket->timeout);
    } else {
        timeout = 0UL;
    }


    if (_sock_sig_setup() < 0) {
        SOCK_ERR (EINTR);
        goto accept_fail;
    }

    /* Loop over all queue-slots and accept first connected TCB
     */
    for (que_idx = 0; ; que_idx = (++que_idx % maxconn)) {
        sock_type *tcb_sk = socket->listen_queue[que_idx];

        tcp_tick (NULL);

        SOCK_YIELD();

        /* No SYNs received yet. This shouldn't happen if we called 'accept()'
         * after 'select_s()' said that socket was readable. (At least one
         * connection on the listen-queue).
         */
        if (tcb_sk != NULL) {
            /* This could happen if 'accept()' was called too long after connection
             * was established and then closed by peer. This could also happen if
             * someone did a portscan on us. I.e. he sent 'SYN', we replied with
             * 'SYN+ACK' and he never sent an 'ACK'. Thus we timeout in
             * 'tcp_Retransmitter()' and abort the TCB.
             *
             * Queue slot is in any case ready for another 'SYN' to come and be
             * handled by '_sock_append()'.
             */
            if (tcb_sk->u.ip_type == 0 && tcb_sk->tcp.state >= tcp_StateLASTACK) {
                SOCK_DEBUGF ((socket, ", aborted TCB (idx %d)", que_idx));
                listen_free (socket, que_idx);
                continue;
            }

            /* !!to-do: Should maybe loop over all maxconn TCBs and accept the
             *          one with oldest 'syn_timestamp'.
             */
            if (tcp_established(&tcb_sk->tcp)) {
                SOCK_DEBUGF ((socket, ", connected! (idx %d)", que_idx));
                break;
            }
        }

        /* We've polled all listen-queue slots and none are connected.
         * Return fail if socket is non-blocking.
         */
        if (que_idx == maxconn-1 && (socket->so_state & SS_NBIO)) {
            SOCK_DEBUGF ((socket, ", would block"));
            SOCK_ERR (EWOULDBLOCK);
            goto accept_fail;
        }

        if (chk_timeout(timeout)) {
            SOCK_DEBUGF ((socket, ", ETIMEDOUT"));
            SOCK_ERR (ETIMEDOUT);
            goto accept_fail;
        }
    }

    /* We're here only when above 'tcp_established()' succeeded.
     * Now duplicate 'socket' into a new listening socket 'clone'
     * with handle 'newsock'.
     */
    _sock_enter_scope();
    newsock = dup_bind (socket, &clone, que_idx);
    if (newsock < 0)
        goto accept_fail;

    if (alloc_addr(socket, clone) < 0) {
        SOCK_DEL_FD (newsock);
        goto accept_fail;
    }

    /* Clone is connected, but *not* listening/accepting.
     * Note: other 'so_state' bits from parent is unchanged.
     *       e.g. clone may be non-blocking.
     */
    clone->so_state   |=  SS_ISCONNECTED;
    clone->so_state   &= ~(SS_ISLISTENING | SS_ISCONNECTING);
    clone->so_options &= ~SO_ACCEPTCONN;

    /* Prevent a PUSH on first segment sent.
     */
    sock_noflush (clone->proto_sock);

    SOCK_DEBUGF ((clone, "\nremote %s (%d)",
                inet_ntoa (clone->remote_addr->sin_addr),
                ntohs (clone->remote_addr->sin_port)));

    if (addr && addrlen) {
        struct sockaddr_in *sa = (struct sockaddr_in*)addr;

        sa->sin_family = AF_INET;
        sa->sin_port   = clone->remote_addr->sin_port;
        sa->sin_addr   = clone->remote_addr->sin_addr;
        memset (sa->sin_zero, 0, sizeof(sa->sin_zero));
        *addrlen = sizeof(*sa   );
    }

    _sock_leave_scope();
    _sock_sig_restore();
    return (newsock);

accept_fail:
    _sock_leave_scope();
    _sock_sig_restore();
    return (-1);
}


/*
 *  Duplicate a SOCK_STREAM 'socket' to 'newconn'. Doesn't set
 *  local/remote addresses. Transfer TCB from listen-queue[idx] of
 *  'socket' to TCB of 'clone'.
 */
static int dup_bind (Socket *_socket, Socket **newconn, int idx)
{
    int fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fd >= 0) {
        Socket *clone = _socklist_find (fd);

        /* child gets state from parent
         */
        clone->timeout    = _socket->timeout;
        clone->close_time = _socket->close_time;
        clone->keepalive  = _socket->keepalive;
        clone->ip_tos     = _socket->ip_tos;
        clone->ip_ttl     = _socket->ip_ttl;
        clone->so_state   = _socket->so_state;
        clone->so_options = _socket->so_options;

        /* TCB for clone is from listen-queue[idx]; free proto_sock from
         * socket(). Reuse listen-queue slot for another SYN.
         */
        free (clone->proto_sock);
        clone->proto_sock = _socket->listen_queue[idx];
        _socket->listen_queue[idx] = NULL;
        _socket->syn_timestamp[idx] = 0;
        *newconn = clone;
    }
    return (fd);
}

/*
 *  Allocate and fill local/remote addresses for 'clone'.
 *  Take local address from 'socket', and remote address from
 *  TCB of clone.
 */
static int alloc_addr (Socket *socket, Socket *clone)
{
    struct in_addr peer;
    sock_type *sk;

    clone->local_addr  = SOCK_CALLOC (sizeof(*clone->local_addr));
    clone->remote_addr = SOCK_CALLOC (sizeof(*clone->remote_addr));

    if (!clone->local_addr || !clone->remote_addr) {
        SOCK_DEBUGF ((socket, ", ENOMEM"));
        SOCK_ERR (ENOMEM);
        return (-1);
    }
    sk = clone->proto_sock;
    peer.s_addr = htonl (sk->tcp.hisaddr);
    clone->local_addr->sin_family  = AF_INET;
    clone->local_addr->sin_port    = socket->local_addr->sin_port;
    clone->local_addr->sin_addr    = socket->local_addr->sin_addr;

    clone->remote_addr->sin_family = AF_INET;
    clone->remote_addr->sin_port   = htons (sk->tcp.hisport);
    clone->remote_addr->sin_addr   = peer;
    return (0);
}

/*
 * Release a listen-queue slot and associated memory.
 */
static int listen_free (Socket *socket, int idx)
{
    sock_type *tcb_sk = socket->listen_queue[idx];

    _tcp_unthread (tcb_sk);

    if (tcb_sk->tcp.rx_data != tcb_sk->tcp.rx_buf)   /* free large Rx buffer? */
        free (tcb_sk->tcp.rx_data);
    if (tcb_sk->tcp.tx_data != tcb_sk->tcp.tx_buf)   /* free large Tx buffer? */
        free (tcb_sk->tcp.tx_data);
    free (tcb_sk);
    socket->listen_queue[idx] = NULL;
    return (0);
}

/*
 *  Called from tcp_fsm.c / tcp_listen_state() (via _tcp_syn_hook) to
 *  append a new connection to the listen-queue of socket 'sock'.
 *
 *  TCB on input ('orig_sk') has received a SYN. Replace TCB on output
 *  with a cloned TCB that we append to the listen-queue and eventually
 *  is used by accept() to create a new socket.
 *
 *  TCB on input ('orig_sk') must still be listening for further connections
 *  on the same port as specified in call to _TCP_listen().
 */
int _sock_append (sock_type **skp)
{
    sock_type   *clone_sk;
    sock_type   *orig_sk = *skp;
    Socket      *socket = NULL;   /* associated socket for '*skp' */
    int         i;

    /* Lookup BSD-socket for Wattcp TCB
     */
    if (_tcp_find_hook == NULL || (socket = (*_tcp_find_hook)(orig_sk)) == NULL) {
        SOCK_DEBUGF ((NULL, "\n  sock_append: not found!?"));
        return (0);  /* i.e. could be a native Wattcp socket */
    }

    SOCK_DEBUGF ((socket, "\n  sock_append:%d", socket->fd));

    if (!(socket->so_options & SO_ACCEPTCONN)) {
        SOCK_DEBUGF ((socket, ", not SO_ACCEPTCONN"));
        return (-1);  /* How could this happen? */
    }

    /* Find the first vacant slot for this clone
     */
    for (i = 0; i < socket->backlog; i++) {
        if (socket->listen_queue[i] == NULL) {
            break;
        }
    }

    if (i >= socket->backlog || i >= SOMAXCONN) {
        /* !!to-do: drop the oldest (or a random) slot in the listen-queue.
         */
        SOCK_DEBUGF ((socket, ", queue full (idx %d)", i));
        return (-1);
    }

    SOCK_DEBUGF ((socket, ", idx %d", i));

    clone_sk = SOCK_CALLOC (sizeof(tcp_Socket));
    if (clone_sk == NULL) {
        SOCK_DEBUGF ((socket, ", ENOMEM"));
        return (-1);
    }

    /* Link in the semi-connected socket (SYN received, ACK will be sent)
     */
    socket->listen_queue[i] = clone_sk;
    socket->syn_timestamp[i] = set_timeout (0);

    /* Copy the TCB to clone
    */
    memcpy (clone_sk, orig_sk, sizeof(tcp_Socket));

    /* Reset clone TX-buffer
    */
    clone_sk->tcp.tx_datalen = 0;
    clone_sk->tcp.tx_maxdatalen = tcp_MaxTxBufSize;
    clone_sk->tcp.tx_data = clone_sk->tcp.tx_buf;
    clone_sk->tcp.safetytcp = SAFETYTCP;

    /* Increase the clone RX-buffer/window (to 16kB)
     */
    sock_recv_buf (clone_sk, DEFAULT_RECV_WIN);

    /* Undo what tcp_handler() and tcp_listen_state() did to
     * this listening socket.
     */
    orig_sk->tcp.hisport = 0;
    orig_sk->tcp.hisaddr = 0;
    orig_sk->tcp.myaddr  = 0;

    orig_sk->tcp.seqnum  = INIT_SEQ();   /* set new ISS */
    orig_sk->tcp.unhappy = FALSE;
    CLR_PEER_MAC_ADDR (&orig_sk->tcp);

#if defined(USE_DEBUG)          /* !!needs some work */
    orig_sk->tcp.last_acknum[0] = orig_sk->tcp.last_acknum[1] = 0;
    orig_sk->tcp.last_seqnum[0] = orig_sk->tcp.last_seqnum[1] = 0;
#endif

    clone_sk->next = _tcp_allsocs;
    _tcp_allsocs = clone_sk;            /* prepend clone to TCB-list */
    *skp = _tcp_allsocs;                /* clone is now the new TCB */
    return (0);
}

/*
  A handy note from:
    http://help.netscape.com/kb/corporate/960513-73.html

Solaris 2.3, 2.4, and 2.5 have a listen backlog queue for incoming
TCP/IP connections; its maximum length is 5 by default.  If you leave
this set to the default, then you will frequently see connections to
your web server time out under moderate loads, even though there are
enough idle web server listener processes available to handle the
connections and your Solaris system shows no other signs of resource
saturation.

The listen backlog queue holds connections that are "half-open" (in
the process of being opened), as well as connections that have been
fully opened but have not yet been accepted by any local processes.
This has no effect on the total number of open TCP/IP connections that
your Solaris system can deal with at once; it only means that your
system can't juggle more than five loose connections at a time, and
any other connections that come in while Solaris is busy with five
loose connections will be dropped and will time out on the client end.

On Solaris 2.3 and 2.4, you can bring the maximum queue length from 5
up to 32 by using the "ndd" command, which must be run as root:

    /usr/sbin/ndd -set /dev/tcp tcp_conn_req_max 32

    It is theoretically possible to increase this number beyond 32,
    although this is not recommended.  If increasing the maximum length
    from 5 to 32 solved the problem temporarily for you but your web
    server's traffic has now increased to a point where the symptoms
    appear with the maximum queue length set to 32, then you should
    contact Sun for further help with this.

    The Netscape servers on any Unix system will request a listen backlog
    queue length of 128 when they run; the operating system then reduces
    that to something it can handle.

    Solaris 2.5 allows a maximum listen backlog queue length as high as
    1024 (and you can raise it in the same way), but it still defaults to
    a maximum queue length of 5.

*/

#endif /* USE_BSD_FUNC */

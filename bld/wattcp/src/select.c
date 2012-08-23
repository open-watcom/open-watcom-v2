/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 *   0.6 : Nov 05, 1999 : G. Vanem - several changes;
 *                        Protect select-loop as critical region.
 *                        Changed criteria for read/writeability.
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

#ifdef __HIGHC__     /* set warning for stack-usage */
#pragma stack_size_warn (9200)    /* ~3*MAX_SOCKETS */
#endif

/*
 * Setup for read/write/except_select()
 */
static __inline Socket *setup_select (int s)
{
  Socket *socket = _socklist_find (s);

  if (!socket)
  {
    if (_sock_dos_fd(s))
    {
      SOCK_DEBUGF ((NULL, ", ENOTSOCK (%d)", s));
      SOCK_ERR (ENOTSOCK);
    }
    else
    {
      SOCK_DEBUGF ((NULL, ", EBADF (%d)", s));
      SOCK_ERR (EBADF);
    }
  }
  return (socket);
}


int select_s (int nfds, fd_set *readfds, fd_set *writefds,
              fd_set *exceptfds, struct timeval *timeout)
{
  fd_set oread  [NUM_SOCK_FDSETS];
  fd_set owrite [NUM_SOCK_FDSETS];
  fd_set oexcept[NUM_SOCK_FDSETS];
  struct timeval starttime, expiry, now;

  volatile int num_fd    = nfds;
  volatile int ret_count = 0;
  volatile int expired   = 0;
  int      s;

#if defined(USE_DEBUG)
  unsigned total_rd = 0;
  unsigned total_wr = 0;
  unsigned total_ex = 0;
#endif

  if (num_fd > MAX_SOCKETS)
      num_fd = MAX_SOCKETS;

  SOCK_DEBUGF ((NULL, "\nselect: n=0-%d, %c%c%c", num_fd-1,
                readfds   ? 'r' : '-',
                writefds  ? 'w' : '-',
                exceptfds ? 'x' : '-'));


  /* num_fd == 0 is permitted. Often used to perform delays:
   *   select (0,NULL,NULL,NULL,&tv);
   */
  if (num_fd < 0)
  {
    SOCK_DEBUGF ((NULL, ", (EINVAL), num_fd %d", num_fd));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  if (timeout)
  {
    if ((long)timeout->tv_sec < 0 || timeout->tv_usec < 0)
    {
      SOCK_DEBUGF ((NULL, ", negative timeout (EINVAL)"));
      SOCK_ERR (EINVAL);
      return (-1);
    }

    gettimeofday2 (&starttime, NULL); /* initialize start time */

    expiry.tv_sec  = starttime.tv_sec  + timeout->tv_sec;
    expiry.tv_usec = starttime.tv_usec + timeout->tv_usec;
    while (expiry.tv_usec >= 1000000UL)
    {
      expiry.tv_usec -= 1000000UL;
      expiry.tv_sec++;
    }

    SOCK_DEBUGF ((NULL, ", timeout %u.%06lds",
                  timeout->tv_sec, timeout->tv_usec));
  }
  else
    SOCK_DEBUGF ((NULL, ", timeout undef"));


  /* Clear our "working" fd_sets
   */
  FD_ZERO (&oread[0]);
  FD_ZERO (&owrite[0]);
  FD_ZERO (&oexcept[0]);

  /* If application catches same signals we do, we must exit
   * gracefully from the do-while and for loops below.
   */
  if (_sock_sig_setup() < 0)
  {
    SOCK_ERR (EINTR);
    goto select_fail;
  }

  /*
   * Loop until specified timeout expires or event(s) satisfied.
   */
  do
  {

    for (s = 0; s < num_fd; s++)
    {
      /* read/write/except counters for socket 's'
       */
      int     do_read  = 0, do_write  = 0, do_exc  = 0;
      int     read_cnt = 0, write_cnt = 0, exc_cnt = 0;
      Socket *socket = NULL;

#if (SK_FIRST > 0)
      /* We ignore stdin/stdout/stderr handles for now
       */
      if (s < SK_FIRST)
         continue;
#endif

      /* Not safe to run sock_daemon() (or other "tasks") now
       */
      _sock_start_crit();


      if (readfds && FD_ISSET(s,readfds))
         do_read = 1;

      if (writefds && FD_ISSET(s,writefds))
         do_write = 1;

      if (exceptfds && FD_ISSET(s,exceptfds))
         do_exc = 1;

      if (do_read || do_write || do_exc)
      {
        /* lookup 'socket' only once for each 's'
         */
        socket = setup_select (s);
        if (!socket)
           goto select_fail;

        tcp_tick (NULL);
      }

      /* Check this socket for readability ?
       */
      if (do_read)
      {
        read_cnt = _sock_read_select (socket);
        if (read_cnt > 0)
           FD_SET (s, &oread[0]);
      }

      /* Check this socket for writeability ?
       */
      if (do_write)
      {
        write_cnt = _sock_write_select (socket);
        if (write_cnt > 0)
           FD_SET (s, &owrite[0]);
      }

      /* Check this socket for exception ?
       */
      if (do_exc)
      {
        exc_cnt = _sock_exc_select (socket);
        if (exc_cnt > 0)
           FD_SET (s, &oexcept[0]);
      }

      /* Increment the return and total counters (may incr. by 0)
       */
      ret_count += (read_cnt + write_cnt + exc_cnt);

#if defined(USE_DEBUG)
      total_rd  += read_cnt;
      total_wr  += write_cnt;
      total_ex  += exc_cnt;
#endif

      /* Safe to run other "tasks" now.
       */
      _sock_stop_crit();
      _sock_dbgflush();

      SOCK_YIELD();

    } /* end of for loop; all sockets checked at least once */


    if (timeout)
    {
      gettimeofday2 (&now, NULL);
      if (now.tv_sec > expiry.tv_sec ||
          (now.tv_sec == expiry.tv_sec && now.tv_usec > expiry.tv_usec))
        expired = TRUE;
    }

    /* At least 1 event is set.
     */
    if (ret_count > 0)
    {
      SOCK_DEBUGF ((NULL, ", cnt=%d (%dr/%dw/%dx)",
                   ret_count, total_rd, total_wr, total_ex));


      /* Copy our "working" fd_sets to output fd_sets
       */
      for (s = 0; s < num_fd; s++)
      {
        if (readfds)
        {
          if (FD_ISSET(s, &oread[0]))
               FD_SET (s, readfds);
          else FD_CLR (s, readfds);
        }
        if (writefds)
        {
          if (FD_ISSET(s, &owrite[0]))
               FD_SET (s, writefds);
          else FD_CLR (s, writefds);
        }
        if (exceptfds)
        {
          if (FD_ISSET(s, &oexcept[0]))
               FD_SET (s, exceptfds);
          else FD_CLR (s, exceptfds);
        }
      }

      /* Do as Linux and return the time left of the period.
       * NB! The tv_sec might be negative if select_s() took too long.
       */
      if (timeout)
      {
        *timeout = timeval_diff (&now, &expiry);
      }
      goto select_ok;
    }

    if (expired)
    {
      expiry = timeval_diff(&now, &starttime);
      SOCK_DEBUGF ((NULL, ", timeout!: %u.%06lus",
                   expiry.tv_secs, expiry.tv_usecs ));

      for (s = 0; s < num_fd; s++)
      {
        if (readfds)   FD_CLR (s, readfds);
        if (writefds)  FD_CLR (s, writefds);
        if (exceptfds) FD_CLR (s, exceptfds);
      }
      ret_count = 0;   /* should already be 0 */
      goto select_ok;
    }
  }
  while (1);


select_fail:
  ret_count = -1;
  _sock_stop_crit();

select_ok:
  _sock_sig_restore();
  return (ret_count);
}

/*
 * Check listen-queue for first connected TCB.
 * Only called for is listening (accepting) sockets.
 */
static __inline int listen_queued (Socket *socket)
{
  int i;

  for (i = 0; i < socket->backlog && i < DIM(socket->listen_queue); i++)
  {
    tcp_Socket *tcb = socket->listen_queue[i];

    if (!tcb)
       continue;

    /* Socket has reached Established state or receive data above
     * low water mark. This means, socket may have reached Closed,
     * but this still counts as a readable event.
     */
    if (tcb->state == tcp_StateESTAB ||
        sock_rbused((sock_type*)tcb) > socket->recv_lowat)
       return (1);
  }
  return (0);
}

/*
 * Return TRUE if socket has a "real" error and not a "pending" error.
 * I.e. EALREADY is not a real error, but a pending condition until a
 * non-blocking socket is actually connected. Or if connection fails,
 * in which case the error is ECONNREFUSED.
 *
 * This signalled read/write state is assumed to persist for the
 * remaining life of the socket.
 */
#define READ_STATE_MASK   (SS_CANTRCVMORE)  /* set in recv() or shutdown() */
#define WRITE_STATE_MASK  (SS_CANTSENDMORE|SS_ISCONNECTED)

static __inline int sock_signalled (Socket *socket, int mask)
{
  if (socket->so_state & mask)
     return (1);

  /* A normal blocking socket. 'so_error' is set in
   * connect(), a "ICMP Unreachable" callback or when RST received
   * in pctcp.c. Otherwise 'so_error' is zero.
   */
  if (!(socket->so_state & SS_NBIO))
     return (socket->so_error);

#if 0
  if (socket->so_options & SO_ACCEPTCONN) /* non-blocking listen sock */
     return (0);

  if (socket->so_error == EALREADY)       /* temporary non-blocking error */
     return (0);
  return (1);
#else
  return (0);
#endif
}


/*
 * Check if socket can be read from.
 */
int _sock_read_select (Socket *socket)
{
  int len;

#if defined(USE_LIBPCAP)
  if (socket->so_type == SOCK_PACKET)
  {
    /* !!fix-me: need to push-back this packet, else it's lost
     *           when read_s()/recv() is called.
     */
    if (pcap_peek(_pcap_w32))
       return (1);
    return (0);
  }
#endif

  if (socket->so_type == SOCK_RAW)
     return (socket->raw_sock && socket->raw_sock->used);

  if (socket->so_type == SOCK_DGRAM)
  {
    if (socket->so_state & SS_PRIV)
         len = sock_recv_used (socket->udp_sock);
    else len = sock_rbused ((sock_type*)socket->udp_sock);

    if (len > socket->recv_lowat ||
        sock_signalled(socket,READ_STATE_MASK))
       return (1);
    return (0);
  }

  if (socket->so_type == SOCK_STREAM)
  {
    sock_type *sk = (sock_type*) socket->tcp_sock;

    if (sock_signalled(socket,READ_STATE_MASK) || /* signalled for read_s() */
        sk->tcp.state >= tcp_StateLASTACK      || /* got FIN from peer */
        sock_rbused(sk) > socket->recv_lowat)     /* Rx-data above low-limit */
       return (1);

    if ((socket->so_options & SO_ACCEPTCONN) &&   /* connection pending */
        listen_queued(socket))
       return (1);
    return (0);
  }
  return (0);
}

/*
 * Check if socket can be written to.
 */
int _sock_write_select (Socket *socket)
{
  /* SOCK_PACKET, SOCK_RAW and SOCK_DGRAM sockets are always writeable
   */
#if defined(USE_LIBPCAP)
  if (socket->so_type == SOCK_PACKET)
     return (1);
#endif

  if (socket->so_type == SOCK_RAW ||
      socket->so_type == SOCK_DGRAM)
     return (1);

  if (socket->so_type == SOCK_STREAM)
  {
    sock_type *sk = (sock_type*) socket->tcp_sock;

    if (sock_tbleft(sk) > socket->send_lowat ||  /* Tx room above low-limit */
        sock_signalled(socket,WRITE_STATE_MASK)) /* signalled for write */
      return (1);
    return (0);
  }
  return (0);  /* shouldn't happen */
}

/*
 * Check socket for exception or faulty condition.
 */
int _sock_exc_select (Socket *socket)
{
#if defined(USE_LIBPCAP)
  if (socket->so_type == SOCK_PACKET &&
      pcap_geterr(_pcap_w32))
     return (1);
#else
  ARGSUSED (socket);
#endif

  /* !!to-do: Only arrival of OOB-data should count here
   */
#if 0
  if (sock_signalled(socket, READ_STATE_MASK|WRITE_STATE_MASK))
     return (1);
#endif

  return (0);
}
#endif /* USE_BSD_FUNC */


/*
 * A small test program, for djgpp only
 */
#if defined(TEST_PROG)

#include <unistd.h>
#include "pcdbug.h"

int main (int argc, char **argv)
{
  DWORD usec;

  if (argc < 2)
  {
    fprintf (stderr, "Usage: %s micro-sec\n", argv[0]);
    return (-1);
  }

  usec = atol (argv[1]);
  dbug_init();
  sock_init();

  printf ("has_8254 %d\n", has_8254);

  while (!kbhit())
  {
    struct timeval tv;
    uclock_t start, diff;

    tv.tv_sec  = usec / 1000000UL;
    tv.tv_usec = usec % 1000000UL;
    start = uclock();
    select_s (1, NULL, NULL, NULL, &tv);  /* select in the "for loop" */

    diff = uclock() - start;

    SOCK_DEBUGF ((NULL, ", diff: %.3fs", (double)diff/(double)UCLOCKS_PER_SEC));

    fputc ('.', stderr);
    usleep (usec + 10000UL);    /* >= 10 msec */
  }
  return (0);
}
#endif

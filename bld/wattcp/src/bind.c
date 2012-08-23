/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

#ifndef IPPORT_ANY
#define IPPORT_ANY 0
#endif

/*
 * Bind:
 *  The purpose of bind is to fill in the information for the local IP
 *  address and local port (local_addr). In this respect, it is the
 *  opposite of connect(). connect() fills in the destination IP address
 *  and destination port (remote_addr).
 *  In most situations, one does not need to call bind().
 *
 *  Calling bind() on a SOCK_DGRAM socket and for multiple addresses
 *  ('myaddr->sin_addr.s_addr == INADDR_ANY') requires special attention:
 *  Wattcp's 'udp_handler()' doesn't store address information for
 *  broadcast.
 *
 *  We therefore install a 's->dataHandler' pointing to '_recvdaemon()'
 *  which queues up SOCK_DGRAM messages. This queue is polled (by
 *  'sock_recv_from') when we call 'receive()' on such a socket.
 *  Thus the '*from' address in 'receive()' will be correctly set to
 *  source address/port of peer.
 */

int bind (int s, const struct sockaddr *myaddr, int namelen)
{
  Socket             *socket = _socklist_find (s);
  struct sockaddr_in *addr   = (struct sockaddr_in*) myaddr;
  BOOL   bind_any;
  WORD   local_port;

  SOCK_PROLOGUE (socket, "\nbind:%d", s);

  if (!addr || namelen < sizeof(*addr))
  {
    SOCK_DEBUGF ((socket, ", EINVAL"));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  if (addr->sin_family != AF_INET)
  {
    SOCK_DEBUGF ((socket, ", EAFNOSUPPORT"));
    SOCK_ERR (EAFNOSUPPORT);
    return (-1);
  }

  VERIFY_RW (addr, sizeof(*addr));

  if (socket->so_type == SOCK_STREAM &&
      IN_MULTICAST(ntohl(addr->sin_addr.s_addr)))
  {
    SOCK_DEBUGF ((socket, ", EINVAL (mc)"));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  /* binding to anyone (including local machine?)
   */
  bind_any = (addr->sin_addr.s_addr == INADDR_ANY);

  if (socket->local_addr)
  {
    u_short port = socket->local_addr->sin_port;
    u_long  ip   = socket->local_addr->sin_addr.s_addr;

    if ((addr->sin_port != IPPORT_ANY && addr->sin_port == port) ||
        (!bind_any && ip == addr->sin_addr.s_addr))
    {
      SOCK_DEBUGF ((socket, ", EADDRINUSE"));
      SOCK_ERR (EADDRINUSE);
      return (-1);
    }
    /* !!to-do: check for "sleeping" ports (lport_inuse in pctcp.c)
     *          also and give EADDRINUSE if local port not free.
     */
  }
  else
  {
    socket->local_addr = SOCK_CALLOC (sizeof(*socket->local_addr));
    if (!socket->local_addr)
    {
      SOCK_DEBUGF ((socket, ", ENOMEM"));
      SOCK_ERR (ENOMEM);
      return (-1);
    }
  }

  if (addr->sin_port == IPPORT_ANY)
  {
    local_port = findfreeport (0, TRUE);
    addr->sin_port = htons (local_port);
  }
  else  /* check if requested port is vacant */
  {
    local_port = ntohs (addr->sin_port); 
    if (grab_localport (local_port) > 0)
    {
      SOCK_DEBUGF ((socket, ", EADDRINUSE"));
      SOCK_ERR (EADDRINUSE);
      return (-1);
    }
  }

  socket->local_addr->sin_port   = addr->sin_port;
  socket->local_addr->sin_addr   = addr->sin_addr;
  socket->local_addr->sin_family = AF_INET;

  SOCK_DEBUGF ((socket, ", %s, (%d)",
                bind_any ? "INADDR_ANY" : inet_ntoa(addr->sin_addr),
                ntohs(addr->sin_port)));


  /* Since SOCK_DGRAM sockets are connectionless, the application need
   * not use connect() or accept(). Hence we need to use _UDP_listen()
   */
  if (socket->so_type == SOCK_DGRAM)
  {
    if (bind_any)
       socket->so_state |= SS_PRIV; /* privileged for broadcast reception */

    if (_UDP_listen (socket, addr->sin_addr, addr->sin_port) < 0)
       return (-1);
  }
  return (0);
}


/*
 * A small test djgpp program
 */
#if defined(TEST_PROG)

#define MY_PORT_ID  6060
#undef  close

#include <unistd.h>
#include <conio.h>
#include "pcdbug.h"

int main (int argc, char **argv)
{
  struct sockaddr_in addr;
  int    sock, quit;

  dbug_init();

  sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0)
  {
    perror ("socket");
    return (-1);
  }

  memset (&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;

  if (argc >= 2 && !strcmp(argv[1],"-frag")) /* test Tx of large datagrams */
  {
    #define CHUNK_SIZE 500
    char data [3*CHUNK_SIZE];
#if 1
    int i;
    for (i = 0; i < sizeof(data);  i++)
        data[i] = i;
#else
    memset (&data[0], '1', CHUNK_SIZE);
    memset (&data[CHUNK_SIZE], '2', CHUNK_SIZE);
    memset (&data[2*CHUNK_SIZE], '3', CHUNK_SIZE);
#endif

    addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
    addr.sin_port        = IPPORT_ANY;

    if (sendto (sock, &data, sizeof(data), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0)
       perror ("sendto");

    close (sock);
    return (-1);
  }

  /* INADDR_ANY will take all the address of the system
   */
  addr.sin_addr.s_addr = htonl (INADDR_ANY);
  addr.sin_port        = htons (MY_PORT_ID);

  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
  {
    perror ("bind");
    close (sock);
    return (-1);
  }

#if 0
  if (listen (sock, 5) < 0)
  {
    perror ("listen");
    close (sock);
    return (-1);
  }
#endif

  quit = 0;

  while (!quit)
  {
    struct timeval  tv;
    struct sockaddr from;
    fd_set fd_read, fd_write, fd_exc;
    int    num, from_len = sizeof(from);

    FD_ZERO (&fd_read);
    FD_ZERO (&fd_write);
    FD_ZERO (&fd_exc);
    FD_SET (STDIN_FILENO, &fd_read);
    FD_SET (sock, &fd_read);
    FD_SET (sock, &fd_write);
    FD_SET (sock, &fd_exc);
    tv.tv_usec = 0;
    tv.tv_sec  = 1;

    num = select (sock+1, &fd_read, &fd_write, &fd_exc, &tv);

    if (FD_ISSET(sock, &fd_read))  fputc ('r', stderr);
    if (FD_ISSET(sock, &fd_write)) fputc ('w', stderr);
    if (FD_ISSET(sock, &fd_exc))   fputc ('x', stderr);

    if (FD_ISSET(STDIN_FILENO, &fd_read))
    {
      int ch = getch();
      quit = (ch == 27);
      fputc (ch, stderr);
    }

    if (FD_ISSET(sock, &fd_read) &&
        accept (sock, &from, &from_len) < 0)
    {
      perror ("accept");
      break;
    }
    if (num < 0)
    {
      perror ("select");
      break;
    }
    fputc ('.', stderr);
    usleep (300000);   /* 300ms */
  }

  close (sock);
  return (0);
}
#endif  /* TEST_PROG */

#endif /* (USE_BSD_FUNC) */


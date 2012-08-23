/*
 * Handling of local ports used by udp/tcp.
 * Moved from pctcp.c
 */

#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "pcbsd.h"
#include "pctcp.h"

#if defined(USE_BSD_FUNC)
#include "socket.h"
#endif

static fd_set lport_inuse[6]; /* at least 1536 bits */

#define NUM_PORTS (8*sizeof(lport_inuse))

/*
 *  Clear the "lport_inuse" bit-array.
 *  !!to-do: allocate fd_set from heap and make empherical
 *           port range configurable.
 */
int init_localport (void)
{
  memset (&lport_inuse[0], 0, sizeof(lport_inuse));
  return (NUM_PORTS);
}

/*
 * findfreeport - return unused local port
 *              - port = 0 -> normal port,
 *                port = 1 -> special port (513-1023)
 *              - we need not be this picky, but it doesn't hurt
 *                (yes it does!)
 *
 * `linger' is set if port shall be matched against "sleeping" ports.
 * Local tcp-ports should not be reused 60sec (2*MSL?) after they have
 * been closed. Note this is NOT related to sock_delay.
 */
WORD findfreeport (WORD port, BOOL linger)
{
  WORD p;
  WORD lo_port, hi_port;

  if (port > 0 && port < USHRT_MAX)    /* return port as-is */
     return (port);

  if (port == 0)
       lo_port = 1025;
  else lo_port = 513;
  hi_port = lo_port + 510;  /* our range of empherical ports */

  assert (hi_port < NUM_PORTS);

  /* This is too wasteful; O(2^n)
   * !!to-do: replace these loops with FD_* macros
   */
  for (p = lo_port; p < hi_port; p++)
  {
    if (_udp_allsocs)
    {
      udp_Socket *udp = _udp_allsocs;

      while (udp->next && udp->myport != p)
             udp = udp->next;
      if (udp->myport == p)
         continue;
    }
#if !defined(USE_UDP_ONLY)
    if (_tcp_allsocs)
    {
      tcp_Socket *tcp = _tcp_allsocs;

      while (tcp->next && tcp->myport != p)
             tcp = tcp->next;
      if (tcp->myport == p)
         continue;
    }
#endif
    if (linger && FD_ISSET(p,&lport_inuse[0])) /* inuse, try next 'p' */
       continue;
    break;
  }
  FD_SET (p, &lport_inuse[0]);
  return (p);
}

#if defined(USE_BSD_FUNC)
/*
 *  Set the "lport_inuse" bit for a local port.
 *  'port' is in host order.
 *  Returns original state or -1 if port above our range.
 */
int grab_localport (WORD port)
{
  if (port < NUM_PORTS)
  {
    int rc = FD_ISSET (port, &lport_inuse[0]);
    FD_SET (port, &lport_inuse[0]);
    return (rc);
  }
  return (-1);
}
#endif

/*
 *  Clear the "lport_inuse" bit for a local port.
 *  'port' is in host order.
 *  Returns original state or -1 if port above our range.
 */
int reuse_localport (WORD port)
{
  if (port < NUM_PORTS)
  {
    int rc = FD_ISSET (port, &lport_inuse[0]);
    FD_CLR (port, &lport_inuse[0]);
    return (rc);
  }
  return (-1);
}


#if !defined(USE_UDP_ONLY)
/*
 * Reuse local port now if not owned by a STREAM-socket.
 * Otherwise let socket daemon free local port when linger period
 * expires. We don't care about rapid reuse of local ports connected
 * to DGRAM-sockets.
 */
void maybe_reuse_lport (tcp_Socket *s)
{
  if (!_tcp_find_hook || !(*_tcp_find_hook)(s))
     reuse_localport (s->myport);
}
#endif /* USE_UDP_ONLY */

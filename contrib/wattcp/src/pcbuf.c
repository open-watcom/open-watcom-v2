#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "language.h"
#include "misc.h"
#include "pctcp.h"
#include "pcdbug.h"
#include "pcbuf.h"


int sock_rbsize (const sock_type *s)
{
  switch (_chk_socket(s))
  {
    case VALID_IP:
         return (sizeof(s->raw.ip) + sizeof(s->raw.data));
    case VALID_UDP:
         return (s->udp.maxrdatalen);
    case VALID_TCP:
         return (s->tcp.maxrdatalen);
  }
  return (0);
}

int sock_rbused (const sock_type *s)
{
  switch (_chk_socket(s))
  {
    case VALID_IP:
         return (s->raw.used ? intel16 (s->raw.ip.length) : 0);
    case VALID_UDP:
         return (s->udp.rdatalen);
    case VALID_TCP:
         return (s->tcp.rdatalen);
  }
  return (0);
}

int sock_rbleft (const sock_type *s)
{
  switch (_chk_socket(s))
  {
    case VALID_IP:
         return (s->raw.used ? 0 : sizeof(s->raw.ip) + sizeof(s->raw.data));
    case VALID_UDP:
         return (s->tcp.maxrdatalen - s->udp.rdatalen);
    case VALID_TCP:
         return (s->tcp.maxrdatalen - s->tcp.rdatalen);
  }
  return (0);
}

int sock_tbsize (const sock_type *s)
{
  switch (_chk_socket(s))
  {
    case VALID_IP:
         return (mtu);
    case VALID_TCP:
         return (tcp_MaxTxBufSize);
    case VALID_UDP:
         return (mtu - sizeof(in_Header) - sizeof(udp_Header));
  }
  return (0);
}

int sock_tbleft (const sock_type *s)
{
  switch (_chk_socket(s))
  {
    case VALID_IP:
         return (mtu);
    case VALID_TCP:
         return (tcp_MaxTxBufSize - s->tcp.datalen);
    case VALID_UDP:
         return (mtu - sizeof(in_Header) - sizeof(udp_Header));
  }
  return (0);
}

int sock_tbused (const sock_type *s)
{
  if (_chk_socket(s) == VALID_TCP)
     return (s->tcp.datalen);
  return (0);
}

/*
 *  Sets new buffer for Rx-data (for udp/tcp).
 *  Should be used only when current buffer is empty,
 *  i.e. sock_rbused() returns 0
 */
int sock_setbuf (sock_type *s, BYTE *rx_buf, unsigned rx_len)
{
  int type = _chk_socket (s);

  if (!type || type == VALID_IP)  /* Raw-sockets use fixed buffer */
     return (0);

  if (!rx_len || !rx_buf)
  {
    s->tcp.rdata       = s->tcp.rddata;
    s->tcp.maxrdatalen = tcp_MaxBufSize;
  }
  else
  {
    s->tcp.rdata       = rx_buf;
    s->tcp.maxrdatalen = min (rx_len, USHRT_MAX);
    memset (rx_buf, 0, s->tcp.maxrdatalen);
  }
  return (s->tcp.maxrdatalen);
}

int sock_preread (const sock_type *s, BYTE *buf, unsigned len)
{
  int count, type = _chk_socket (s);

  if (!type || type == VALID_IP)  /* Raw-sockets use fixed buffer */
     return (0);

  count = s->tcp.rdatalen;
  if (count < 1)
     return (count);

  if (count > len)
      count = len;
  if (buf)
     memcpy (buf, s->tcp.rdata, count);
  return (count);
}

/*
 * chk_socket - determine whether a real socket or not
 */
int _chk_socket (const sock_type *s)
{
  if (!s)
     return (0);

  if (s->tcp.ip_type == TCP_PROTO && s->tcp.state <= tcp_StateCLOSED)
     return (VALID_TCP);

  if (s->udp.ip_type == UDP_PROTO)
     return (VALID_UDP);

  if (s->raw.ip_type == IP_TYPE)
     return (VALID_IP);

  return (0);
}

const char *sockerr (const tcp_Socket *s)
{
  if (s && s->err_msg && s->err_msg[0])
     return (s->err_msg);
  return (NULL);
}

const char *sockstate (const tcp_Socket *s)
{
  switch (_chk_socket((const sock_type*)s))
  {
    case VALID_IP:
         return (_LANG("Raw IP Socket"));

    case VALID_UDP:
         return (_LANG("UDP Socket"));

    case VALID_TCP:
         return (_LANG(tcpState[s->state]));

    default:
         return (_LANG("Not an active socket"));
  }
}


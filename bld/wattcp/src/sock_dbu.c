#include <stdio.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "pcdbug.h"
#include "sock_dbu.h"

void sock_debugdump (const sock_type *s)
{
#if defined(USE_DEBUG)
  const tcp_Socket *tcp = &s->tcp;

  if (s->raw.ip_type == IP_TYPE)
     return;

#if defined(__SMALL__)
  (*_printf) ("next       %04X\r\n",      s->tcp.next);
#elif defined(__LARGE__)
  (*_printf) ("next       %04X:%04X\r\n", FP_SEG(s->tcp.next), FP_OFF(s->tcp.next));
#else
  (*_printf) ("next       %08lX\r\n",     (DWORD)s->tcp.next);
#endif

  (*_printf) ("type       %d\r\n", s->tcp.ip_type);

  (*_printf) ("stat/error %s\r\n", s->tcp.err_msg ? s->tcp.err_msg : "(none)");
  (*_printf) ("usr-timer  %08lX (%sexpired)\r\n",
              s->tcp.usertimer, chk_timeout(s->tcp.usertimer) ? "" : "not ");

  switch (s->tcp.ip_type)
  {
    case UDP_PROTO:
         (*_printf) ("udp rdata  %u `%.*s'\r\n",
                     s->tcp.rdatalen, s->tcp.rdatalen, s->tcp.rdata);
         break;
    case TCP_PROTO:
         (*_printf) ("tcp rdata  %u `%.*s'\r\n",
                     tcp->rdatalen, tcp->rdatalen, tcp->rdata);
         (*_printf) ("tcp state  %u (%s)\r\n",
                     tcp->state, tcpState[tcp->state]);
         break;
  }
#else
  ARGSUSED (s);
#endif
}

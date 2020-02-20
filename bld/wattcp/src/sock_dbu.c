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
  if (s->u.ip_type == IP_TYPE)
     return;

#if defined(__SMALL__)
  (*_printf) ("next       %04X\r\n",      s->u.next);
#elif defined(__LARGE__)
  (*_printf) ("next       %04X:%04X\r\n", FP_SEG(s->u.next), FP_OFF(s->u.next));
#else
  (*_printf) ("next       %08lX\r\n",     (DWORD)s->u.next);
#endif

  (*_printf) ("type       %d\r\n", s->u.ip_type);

  (*_printf) ("stat/error %s\r\n", s->u.err_msg ? s->u.err_msg : "(none)");
  (*_printf) ("usr-timer  %08lX (%sexpired)\r\n",
              s->u.usertimer, chk_timeout(s->u.usertimer) ? "" : "not ");

  switch (s->u.ip_type)
  {
    case UDP_PROTO:
         (*_printf) ("udp rdata  %u `%.*s'\r\n",
                     s->udp.rdatalen, s->udp.rdatalen, s->udp.rdata);
         break;
    case TCP_PROTO:
         (*_printf) ("tcp rdata  %u `%.*s'\r\n",
                     s->tcp.rdatalen, s->tcp.rdatalen, s->tcp.rdata);
         (*_printf) ("tcp state  %u (%s)\r\n",
                     s->tcp.state, tcpState[s->tcp.state]);
         break;
  }
#else
  ARGSUSED (s);
#endif
}

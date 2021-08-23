#include <stdio.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "pcdbug.h"
#include "sock_dbu.h"

void sock_debugdump (const sock_type *sk)
{
#if defined(USE_DEBUG)
    if (sk->u.ip_type == IP_TYPE)
        return;

#if (DOSX)
    (*_printf) ("next       %08lX\r\n",     (DWORD)sk->next);
#elif defined(__LARGE__)
    (*_printf) ("next       %04X:%04X\r\n", FP_SEG(sk->next), FP_OFF(sk->next));
#else
    (*_printf) ("next       %04X\r\n",      sk->next);
#endif

    (*_printf) ("type       %d\r\n", sk->u.ip_type);

    (*_printf) ("stat/error %s\r\n", sk->u.err_msg ? sk->u.err_msg : "(none)");
    (*_printf) ("usr-timer  %08lX (%sexpired)\r\n",
              sk->u.usertimer, chk_timeout(sk->u.usertimer) ? "" : "not ");

    switch (sk->u.ip_type) {
    case UDP_PROTO:
        (*_printf) ("udp rxdata  %u `%.*s'\r\n",
                     sk->udp.rx_datalen, sk->udp.rx_datalen, sk->udp.rx_data);
        break;
    case TCP_PROTO:
        (*_printf) ("tcp rxdata  %u `%.*s'\r\n",
                     sk->tcp.rx_datalen, sk->tcp.rx_datalen, sk->tcp.rx_data);
        (*_printf) ("tcp state  %u (%s)\r\n",
                     sk->tcp.state, tcpState[sk->tcp.state]);
        break;
    }
#else
    ARGSUSED (sk);
#endif
}

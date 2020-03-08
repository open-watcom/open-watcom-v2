/*
 * SOCK_IN: User input functions moved from pctcp.c;
 *
 *  Functions used in these macros:
 *    sock_wait_established()
 *    sock_wait_input()
 *    sock_wait_closed();
 */

#include <stdio.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "language.h"
#include "misc.h"
#include "pctcp.h"

/*
 * ip user level timer stuff
 *   void ip_timer_init (sock_type *sk, int delayseconds)
 *   int  ip_timer_expired (sock_type *sk)
 *  - 0 if not expired
 */
void ip_timer_init (sock_type *sk, int seconds)
{
    switch (sk->u.ip_type) {
    case UDP_PROTO:
    case TCP_PROTO:
        if (seconds) {
            sk->u.usertimer = set_timeout (1000 * seconds);
        } else {
            sk->u.usertimer = 0;
        }
        break;
    }
}

int ip_timer_expired (const sock_type *sk)
{
    switch (sk->u.ip_type) {
    case UDP_PROTO:
    case TCP_PROTO:
        return chk_timeout (sk->u.usertimer);
    }
    return (0);
}

/*
 * _ip_delay0 called by macro sock_wait_established()
 * _ip_delay1 called by macro sock_wait_input()
 * _ip_delay2 called by macro sock_wait_closed();
 *
 */
int _ip_delay0 (sock_type *sk, int timeoutseconds, UserHandler fn, int *statusptr)
{
    int status = -1;

    ip_timer_init (sk, timeoutseconds);
    for ( ;; ) {
#if !defined(USE_UDP_ONLY)
        if (sk->u.ip_type == TCP_PROTO) {
            if (tcp_established(&sk->tcp)) {
                status = 0;
                break;
            }
        }
#endif

        kbhit();        /* permit ^C */

        if (!tcp_tick(sk)) {
            if (sk->tcp.err_msg == NULL)
                sk->tcp.err_msg = _LANG("Host refused connection");
            status = -1;       /* get an early reset */
            break;
        }
        if (ip_timer_expired(sk)) {
            if (sk->tcp.err_msg == NULL)
                sk->tcp.err_msg = _LANG("Open timed out");
            sock_close (sk);
            status = -1;
            break;
        }
        if (fn != NULL && (status = (*fn)(sk)) != 0)
            break;

        if (sk->tcp.usr_yield != NULL)
            (*sk->tcp.usr_yield)();

        if (sk->u.ip_type == UDP_PROTO) {
            status = 0;
            break;
        }
    }

    if (statusptr)
        *statusptr = status;
    return (status);
}

int _ip_delay1 (sock_type *sk, int timeoutseconds, UserHandler fn, int *statusptr)
{
    int status = -1;

    ip_timer_init (sk, timeoutseconds);

#if !defined(USE_UDP_ONLY)
    sock_flush (sk);    /* new enhancement */
#endif

    for ( ;; ) {
        if (sock_dataready(sk)) {
            status = 0;
            break;
        }
        kbhit();         /* permit ^C */

        if (!tcp_tick(sk)) {
            status = 1;
            break;
        }
        if ((sk->tcp.locflags & LF_GOT_FIN) && !sock_dataready(sk)) {
            status = 1;
            break;
        }

        if (ip_timer_expired(sk)) {
            if (sk->tcp.err_msg == NULL)
                sk->tcp.err_msg = _LANG("Connection timed out");
            sock_close (sk);
            status = -1;
            break;
        }
        if (fn != NULL && (status = (*fn)(sk)) != 0)
            break;

        if (sk->tcp.usr_yield != NULL) {
            (*sk->tcp.usr_yield)();
        }
    }

    if (statusptr)
        *statusptr = status;
    return (status);
}

int _ip_delay2 (sock_type *sk, int timeoutseconds, UserHandler fn, int *statusptr)
{
    int status = -1;

    if (sk->u.ip_type != TCP_PROTO) {
        if (statusptr)
            *statusptr = 1;
        return (1);
    }

#if !defined(USE_UDP_ONLY)
    ip_timer_init (sk, timeoutseconds);

    for ( ;; ) {
        /* in this situation we know user is not planning to read rxdata
         */
        sk->tcp.rx_datalen = 0;
        kbhit();              /* permit ^C */
        if (!tcp_tick(sk)) {
            status = 1;
            break;
        }
        if (ip_timer_expired(sk)) {
            if (sk->tcp.err_msg == NULL)
                sk->tcp.err_msg = _LANG("Connection timed out");
            sock_abort (sk);
            status = -1;
            break;
        }
        if (fn != NULL && (status = (*fn)(sk)) != 0)
            break;

        if (sk->tcp.usr_yield != NULL) {
            (*sk->tcp.usr_yield)();
        }
    }

    if (statusptr)
        *statusptr = status;

#else
    ARGSUSED (fn);
    ARGSUSED (timeoutseconds);
#endif
    return (status);
}

int sock_timeout (sock_type *sk, int sec)
{
    if (sk->u.ip_type != TCP_PROTO)
        return (1);

#if !defined(USE_UDP_ONLY)
    if (sk->tcp.state != tcp_StateESTAB)
        return (2);

    sk->tcp.timeout = set_timeout (1000 * sec);
#else
    ARGSUSED (sec);
#endif
    return (0);
}

int sock_established (sock_type *sk)
{
    switch (sk->u.ip_type) {
    case UDP_PROTO:
        return (1);
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        return (sk->tcp.state == tcp_StateESTAB ||
                 sk->tcp.state == tcp_StateESTCL ||
                 sk->tcp.state == tcp_StateCLOSWT);
#endif
    default:
        return (0);
    }
}

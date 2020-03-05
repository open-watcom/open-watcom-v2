#include <errno.h>
#include <sys/werrno.h>

#include "socket.h"

#if defined(USE_BSD_FUNC)

#include "bsdname.h"

int _getpeername (const sock_type *sk, void *dest, socklen_t *len)
{
    struct watt_sockaddr temp;
    int    ltemp;

    memset (&temp, 0, sizeof(temp));
    temp.s_ip   = sk->u.hisaddr;
    temp.s_port = sk->u.hisport;

    if (sk->u.hisaddr == 0 || sk->u.hisport == 0 || _chk_socket(sk) == 0) {
        if (len != NULL)
            *len = '\0';
        return (-1);
    }

    /* how much do we move
     */
    ltemp = ((len != NULL) ? *len : sizeof (struct watt_sockaddr));
    if (ltemp > sizeof (struct watt_sockaddr))
        ltemp = sizeof (struct watt_sockaddr);
    memcpy (dest, &temp, ltemp);

    if (len != NULL)
        *len = ltemp;
    return (0);
}

int _getsockname (const sock_type *sk, void *dest, socklen_t *len)
{
    struct watt_sockaddr temp;
    int    ltemp;

    memset (&temp, 0, sizeof(temp));
    temp.s_ip   = sk->u.myaddr;
    temp.s_port = sk->u.myport;

    if (sk->u.myaddr == 0 || sk->u.myport == 0 || _chk_socket(sk) == 0) {
        if (len != NULL)
            *len = '\0';
        return (-1);
    }

    /* how much do we move
     */
    ltemp = ((len != NULL) ? *len : sizeof (struct watt_sockaddr));
    if (ltemp > sizeof (struct watt_sockaddr))
        ltemp = sizeof (struct watt_sockaddr);
    memcpy (dest, &temp, ltemp);

    if (len != NULL)
        *len = ltemp;
    return (0);
}

#endif /* defined(USE_BSD_FUNC) */

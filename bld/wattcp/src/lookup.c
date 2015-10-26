#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "strings.h"
#include "udp_nds.h"
#include "udp_dom.h"
#include "pcconfig.h"
#include "pcbsd.h"
#include "pctcp.h"

DWORD lookup_host (const char *host, char *ip_str)
{
  DWORD ip;

  if (isaddr(host))
     ip = aton (host);
  else
  {
#if defined(USE_DEBUG)
    char buf[20];

    (*_printf) ("Resolving `%s'...", host);
    fflush (stdout);
    ip = resolve (host);
    if (ip)
       (*_printf) ("[%s]\r\n", _inet_ntoa(buf,ip));
#else
    ip = resolve (host);
#endif
  }

  /* Don't use is_local_addr(); an loopback address is valid here.
   */
  if (ip - my_ip_addr <= multihomes)
  {
#if defined(USE_DEBUG)
    (*_printf) ("Can not connect to ourself\r\n");
#endif
    exit (1);
  }

  if (ip_str)
     _inet_ntoa (ip_str, ip);
  return (ip);
}

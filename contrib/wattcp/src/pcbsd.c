/*
 * PCBSD - provide some typical BSD UNIX functionality
 * Erick Engelke, Feb 22, 1991
 *
 * Prefixed with `_' to support real BSD-functions elsewhere
 * G.Vanem 1996
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "misc.h"
#include "udp_nds.h"
#include "pctcp.h"
#include "pcbsd.h"

char *_inet_ntoa (char *s, DWORD x)
{
  itoa ((int)(x >> 24), s, 10);
  strcat (s, ".");
  itoa ((int)(x >> 16) & 0xFF, strchr(s,0), 10);
  strcat (s, ".");
  itoa ((int)(x >> 8) & 0xFF, strchr(s,0), 10);
  strcat (s, ".");
  itoa ((int)(x & 0xFF), strchr(s,0), 10);
  return (s);
}

DWORD _inet_addr (const char *s)
{
  DWORD addr = 0;

  if (isaddr(s))
     return aton (s);
  if (isaddr_dotless(s,&addr))
     return (addr);
  return (0);
}

DWORD _gethostid (void)
{
  return (my_ip_addr);
}

DWORD _sethostid (DWORD ip)
{
  return (my_ip_addr = ip);
}


#ifdef NOT_USED
/*
 * Warn about calling 'getXbyY()' functions before calling
 * 'sock_init()'. Many other functions will fail if we're not
 * initialised, but 'getXbyY()' are often used during application
 * startup.
 */
void uninit_warn (const char *func)
{
  outs ("Warning: function \"");
  outs ((char*)func);
  outsnl ("()\" called before sock_init() called.");
}
#endif

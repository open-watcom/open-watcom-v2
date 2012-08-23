/*
 *  NETADDR - provide some more BSD address functions
 *  Gisle Vanem, Oct 12, 1995
 *
 *  inet_network() is Copyright (c) 1983, 1993
 *  The Regents of the University of California.  All rights reserved.
 *  All rights reserved.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "pcbsd.h"

#if defined(USE_BSD_FUNC)

char * inet_ntoa (struct in_addr adr)
{
  static char buf[4][20];   /* use max 4 at a time */
  static int  idx = 0;
  char       *rc  = buf[idx];

  _inet_ntoa (rc, ntohl(adr.s_addr));
  idx = (idx+1) & 3;
  return (rc);
}

u_long inet_aton (const char *name, struct in_addr *adr)
{
  DWORD ip = htonl (_inet_addr((char*)name));

  if (adr)
     adr->s_addr = ip;
  return (ip);
}

u_long inet_addr (const char *adr)
{
  DWORD ip = htonl (_inet_addr((char*)adr));

  if (ip)
     return (ip);
  return (INADDR_NONE);
}

u_long inet_network (const char *name)
{
  u_long val, base, n;
  u_long parts[4], *pp = parts;
  int    i;
  char   c;

again:
  /*
   * Collect number up to ``.''.
   * Values are specified as for C:
   * 0x=hex, 0=octal, other=decimal.
   */
  val  = 0;
  base = 10;
  /*
   * The 4.4BSD version of this file also accepts 'x__' as a hexa
   * number.  I don't think this is correct.  -- Uli
   */
  if (*name == '0')
  {
    if (*++name == 'x' || *name == 'X')
         base = 16, name++;
    else base = 8;
  }
  while ((c = *name) != 0)
  {
    if (isdigit(c))
    {
      val = (val * base) + (c - '0');
      name++;
      continue;
    }
    if (base == 16 && isxdigit(c))
    {
      val = (val << 4) + (c + 10 - (islower(c) ? 'a' : 'A'));
      name++;
      continue;
    }
    break;
  }
  if (*name == '.')
  {
    if (pp >= parts + 4)
       return (INADDR_NONE);

    *pp++ = val;
    name++;
    goto again;
  }
  if (*name && !isspace(*name))
     return (INADDR_NONE);

  *pp++ = val;
  n = pp - parts;
  if (n > 4)
     return (INADDR_NONE);

  for (val = 0, i = 0; i < n; i++)
  {
    val <<= 8;
    val |= parts[i] & 0xff;
  }
  return (val);
}

/*
 * Return the network number from an internet
 * address; handles class A/B/C network #'s.
 */
u_int32_t inet_netof (struct in_addr adr)
{
  u_long a = ntohl (adr.s_addr);

  if (IN_CLASSA(a))
     return ((a & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT);

  if (IN_CLASSB(a))
     return ((a & IN_CLASSB_NET) >> IN_CLASSB_NSHIFT);

  return ((a & IN_CLASSC_NET) >> IN_CLASSC_NSHIFT);
}

/*
 * Return the local network address portion of an
 * internet address; handles class A/B/C network
 * number formats only.
 * NB! return value is host-order
 */
u_long inet_lnaof (struct in_addr adr)
{
  u_long a = ntohl (adr.s_addr);

  if (IN_CLASSA(a))
     return (a & IN_CLASSA_HOST);

  if (IN_CLASSB(a))
     return (a & IN_CLASSB_HOST);

  return (a & IN_CLASSC_HOST);
}

/*
 * Formulate an Internet address from network + host (with subnet address).
 * NB! net is host-order
 */
struct in_addr inet_makeaddr (u_long net, u_long host)
{
  u_long addr;

  if (net < 128)
       addr = (net << IN_CLASSA_NSHIFT) | (host & IN_CLASSA_HOST);

  else if (net < 65536)
       addr = (net << IN_CLASSB_NSHIFT) | (host & IN_CLASSB_HOST);

  else if (net < 16777216L)
       addr = (net << IN_CLASSC_NSHIFT) | (host & IN_CLASSC_HOST);

  else addr = (net | host);

  addr = htonl (addr);
  return (*(struct in_addr*) &addr);
}

#endif  /* USE_BSD_FUNC */

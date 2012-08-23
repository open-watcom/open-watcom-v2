/*
 * Domain Name Service - address convertions
 *
 * v 0.0 : Jan 11, 1991 : E. Engelke
 * v 0.2 : Apr 10, 1995 : G. Vanem, function priv_addr()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "misc.h"
#include "strings.h"
#include "pcbsd.h"
#include "udp_nds.h"


/*
 * aton()
 *      - converts [a.b.c.d] or a.b.c.d to 32 bit long (host order)
 *      - returns 0 on error (safer than -1)
 */
DWORD aton (const char *str)
{
  int   i;
  DWORD ip = 0;

  if (*str == '[')
     ++str;

  for (i = 24; i >= 0; i -= 8)
  {
    int cur = atoi (str);

    ip |= (DWORD)(cur & 0xFF) << i;
    if (!i)
       return (ip);

    str = strchr (str, '.');
    if (!str)
       return (0);      /* return 0 on error */
    ++str;
  }
  return (0);           /* cannot happen ??  */
}

/*
 * isaddr()
 *      - returns nonzero if 'str' is simply an ip address
 *      - Should it accept dotless ip-addresses?
 */
int isaddr (const char *str)
{
  char ch;

  while ((ch = *str++) != 0)
  {
    if (isdigit(ch))
       continue;
    if (ch == '.' || ch == ' ' || ch == '[' || ch == ']')
       continue;
    return (0);
  }
  return (1);
}

/*
 * aton_dotless()
 *      - converts `[dotless]' or `dotless' to 32 bit long (host order)
 *      - returns 0 on error (safer than -1)
 */
DWORD aton_dotless (const char *str)
{
  DWORD ip = 0UL;

  isaddr_dotless (str, &ip);
  return (ip);
}

/*
 * isaddr_dotless()
 *      - returns nonzero if 'str' is an dotless ip address
 *      - accept "[ 0-9\[\]]+". Doesn't accept octal base.
 */
int isaddr_dotless (const char *str, DWORD *ip)
{
  char  ch, buf[10] = { 0 };
  int   i = 0;
  DWORD adr;

  while ((ch = *str++) != 0)
  {
    if (ch == '.' || i == sizeof(buf))
       return (0);

    if (isdigit(ch))
    {
      buf[i++] = ch;
      continue;
    }
    if (ch == ' ' || ch == '[' || ch == ']')
       continue;
    return (0);
  }

  buf[i] = '\0';
  adr = atol (buf);
  if (adr == 0)
     return (0);

  if ((adr % 256) == 0)         /* LSB must be non-zero */
     return (0);

  if (((adr >> 24) % 256) == 0) /* MSB must be non-zero */
     return (0);

  if (ip)
     *ip = adr;
  return (1);
}

void psocket (const tcp_Socket *s)
{
  char buffer[20];

  (*_outch) ('[');
  outs (_inet_ntoa(buffer, s->hisaddr));
  (*_outch) (':');
  itoa (s->hisport,buffer,10);
  outs (buffer);
  (*_outch) (']');
}

#ifdef NOT_USED
int priv_addr (DWORD ip)
{
  /*
   * private addresses in the "black range":
   *
   * 10.0.0.0    - 10.255.255.255
   * 172.16.0.0  - 172.31.255.255
   * 192.168.0.0 - 192.168.255.255
   */
  if (ip >= aton("10.0.0.0") && ip <= aton("10.255.255.255"))
     return (1);

  if (ip >= aton("172.16.0.0") && ip <= aton("172.31.255.255"))
     return (1);

  if (ip >= aton("192.168.0.0") && ip <= aton("192.168.255.255"))
     return (1);

  return (0);
}
#endif

/*
 *  BSD/XOpen-like functions:
 *    Reentrant (MT-safe) version of some
 *    getXbyY function from <netdb.h>
 *
 *  G. Vanem  <giva@bgnett.no>
 *
 *  18.aug 1998 (GV)  - Created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>

#include "wattcp.h"

#if defined(USE_BSD_FUNC)

/*
 * NB! This file isn't part of Watt-32 yet as I don't have
 *     proper documentation on these `_r' functions.
 *     Anybody got a clue?
 */

/*
 * gethostbyY_r..
 */
int gethostbyaddr_r (const char *addr, int addr_len, int addr_type,
                     struct hostent *result,
                     struct hostent *buffer,
                     int buffer_len, int *p_errno)
{   
  struct hostent *he;

  if (!result || !buffer)
  {
    *p_errno = EINVAL;
    return (-1);
  }

  he = gethostbyaddr (addr, addr_len, addr_type);
  *p_errno = h_errno;
  if (!he)
     return (-1);

  memcpy (result, he, sizeof(*result));
  return (0);
}

int gethostbyname_r (const char *name,
                     struct hostent *result,
                     struct hostent *buffer,
                     int buffer_len, int *p_errno)
{
}

int gethostent_r (struct hostent *result,
                  struct hostent *buffer)
{
}

int sethostent_r (int stayopen, struct hostent *buffer)
{
}

int endhostent_r (struct hostent *buffer)
{
}


/*
 * getprotobyY_r..
 */
int getprotobynumber_r (int proto,
                        struct protoent *result,
                        struct protoent *buffer)
{
}

int getprotobyname_r (const char *name,
                      struct protoent *result,
                      struct protoent *buffer)
{
}

int getprotoent_r (struct protoent *result,
                   struct protoent *buffer)
{
}

int setprotoent_r (int stayopen, struct protoent *buffer)
{
}

int endprotoent_r (struct protoent *buffer)
{
}

/*
 * getservbyY_r..
 */
int getservbyname_r (const char *name, const char *proto,
                     struct servent *result,
                     struct servent *buffer)
{
}

int getservbyport_r (int port, const char *proto,
                     struct servent *result,
                     struct servent *buffer)
{
}

#endif /* USE_BSD_FUNC */

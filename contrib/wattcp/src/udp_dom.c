/*
 * Domain Name Server protocol
 *
 * This portion of the code needs some major work. I ported it (read STOLE IT)
 * from NCSA and lost about half the code somewhere in the process.
 *
 *  0.3 : Jun 16, 1997 - calling usr_yield (i.e. system_yield) while resolving
 *  0.2 : Apr 24, 1991 - use substring portions of domain
 *  0.1 : Mar 18, 1991 - improved the trailing domain list
 *  0.0 : Feb 19, 1991 - pirated by Erick Engelke
 * -1.0 :              - NCSA code
 */


#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "language.h"
#include "pcconfig.h"
#include "pc_cbrk.h"
#include "pcbsd.h"
#include "pctcp.h"
#include "udp_nds.h"
#include "udp_dom.h"

/* a hack to make gethostbyname() and resolve() cooperate --gv
 */
int called_from_resolve = 0;
int called_from_ghbn    = 0;

/* These next 'constants' are loaded from WATTCP.CFG file
 */
char  defaultdomain [MAX_HOSTLEN+1] = "your.domain.com";
char *def_domain = defaultdomain;
char *loc_domain;    /* current subname to be used by the domain system */

int (*_resolve_hook)(void);      /* user hook for terminating resolve() */
int   _resolve_exit;             /* user hook interrupted */
int   _resolve_timeout;          /* (reverse) lookup timeout */

DWORD def_nameservers [MAX_NAMESERVERS];
WORD  last_nameserver = 0;
UINT  dns_timeout     = 0;
int   dns_recurse     = 1;

static DWORD resolve_timeout;

static udp_Socket   *dom_sock;
static struct useek *question;
static const char   *no_serv = __LANG ("No nameserver defined!");

static void qinit (void)
{
  question->h.flags   = intel16 (DRD);
  question->h.qdcount = intel16 (1);
  question->h.ancount = 0;
  question->h.nscount = 0;
  question->h.arcount = 0;
}


/*
 * packdom()
 *   pack a regular text string into a packed domain name, suitable
 *   for the name server.
 *
 *   returns length
 */
static int packdom (char *dst, const char *src)
{
  char *p, *q, *savedst;
  int   i, dotflag, defflag;

  p = (char*) src;
  dotflag = defflag = 0;
  savedst = dst;

  do                          /* copy whole string */
  {
    *dst = 0;
    q = dst + 1;
    while (*p && *p != '.')
       *q++ = *p++;

    i = p - (char*)src;
    if (i > 0x3F)
       return (-1);
    *dst = i;
    *q = 0;

    if (*p)                   /* update pointers */
    {
      dotflag = 1;
      src = ++p;
      dst = q;
    }
    else if (!dotflag && !defflag && loc_domain)
    {
      p = loc_domain;         /* continue packing with default */
      defflag = 1;
      src = p;
      dst = q;
    }
  }
  while (*p);

  q++;
  return (q - savedst);       /* length of packed string */
}

/*
 * unpackdom()
 *  Unpack a compressed domain name that we have received from another
 *  host.  Handles pointers to continuation domain names -- buf is used
 *  as the base for the offset of any pointer which is present.
 *  returns the number of bytes at src which should be skipped over.
 *  Includes the NULL terminator in its length count.
 */
static int unpackdom (BYTE *dst, const BYTE *src, const BYTE *buf)
{
  int   retval  = 0;
  const BYTE *p = src;

  while (*src)
  {
    int i,j = *src;

    while ((j & 0xC0) == 0xC0)
    {
      if (!retval)
         retval = src - p + 2;
      src++;
      src = &buf[(j & 0x3F)*256 + *src];  /* pointer dereference */
      j = *src;
    }

    src++;
    for (i = 0; i < (j & 0x3F); i++)
        *dst++ = *src++;

    *dst++ = '.';
  }

  *(--dst) = 0;              /* add terminator */
  src++;                     /* account for terminator on src */

  if (!retval)
     retval = src - p;

  return (retval);
}


/*
 * send_dom()
 *   put together a domain lookup packet and send it.
 *   uses port 53.
 */
static int send_dom (const char *name, DWORD towho)
{
  WORD  ulen;
  BYTE *start = (BYTE*)&question->x;
  WORD  i     = packdom ((char*)start, name);
  BYTE *p     = &question->x[i];

  *p++ = 0;               /* high byte of qtype */
  *p++ = DTYPEA;          /* number is < 256, so we know high byte=0 */
  *p++ = 0;               /* high byte of qclass */
  *p++ = DIN;             /* qtype is < 256 */

  question->h.ident = Random (1, USHRT_MAX);

  if (!udp_open(dom_sock,997,towho,DOM_DST_PORT,NULL))
  {
    outsnl (_LANG("Nameserver ARP failed"));
    return (0);
  }
  ulen = sizeof (struct dhead) + (p - start);
  sock_write ((sock_type*)dom_sock, (BYTE*)question, ulen);
  return (ulen);
}

static __inline int countpaths (const char *pathstring)
{
  int   count = 0;
  const char *p;

  for (p = pathstring; *p || *(p+1); p++)
  {
    if (*p == 0)
       count++;
  }
  return (++count);
}

static __inline const char *getpath (
                const char *pathstring,  /* the path list to search        */
                int         whichone)    /* which path to get, starts at 1 */
{
  const char *rc;

  if (whichone > countpaths(pathstring))
     return (NULL);

  whichone--;
  for (rc = pathstring; whichone; rc++)
  {
    if (*rc == 0)
       whichone--;
  }
  return (rc);
}


/*
 * ddextract()
 *   extract the ip number from a response message.
 *   returns the appropriate status code and if the ip number is available,
 *   copies it into mip
 */
static int ddextract (const struct useek *qp, DWORD *mip)
{
  WORD nans  = intel16 (qp->h.ancount);
  BYTE rcode = DRCODE & intel16(qp->h.flags);

  if (rcode > 0)
     return (int)rcode;

  if (nans > 0 &&                              /* at least one answer   */
      (intel16(qp->h.flags) & DQR))            /* response flag is set  */
  {
    BYTE  space[260];
    const BYTE *p = (const BYTE*) &qp->x;
    int   i = unpackdom (space, p, (const BYTE*)qp);

   /* spec defines name then QTYPE + QCLASS = 4 bytes
    */
    p += i + 4;

   /* at this point, there may be several answers.  We will take the first
    * one which has an IP number.  There may be other types of answers that
    * we want to support later.
    */
    while (nans-- > 0)                     /* look at each answer   */
    {
      struct rrpart *rrp;
      int    j;

      i   = unpackdom (space, p, (const BYTE*)qp);
      p  += i;                             /* account for string    */
      rrp = (struct rrpart*) p;            /* resource record here  */

      if (!*p && *(p+1) == DTYPEA &&       /* correct type and class */
          !*(p+2) && *(p+3) == DIN)
      {
        *mip = *(DWORD*)&rrp->rdata;       /* save IP #         */
        return (0);                        /* successful return */
      }
      j  = *(WORD*) &rrp->rdlength;
      p += 10 + intel16 (j);               /* length of rest of RR */
    }
  }
  return (-1);                             /* generic failed to parse */
}


/*
 * parse_domain()
 *   Look at the results to see if our DOMAIN request is ready.
 *   It may be a timeout, which requires another query.
 */
static DWORD parse_domain (void)
{
  DWORD ip = 0UL;

  sock_fastread ((sock_type*)dom_sock, (BYTE*)question, sizeof(*question));

 /* check to see if the necessary information was in the UDP response
  */
  switch (ddextract (question, &ip))
  {
    case  3: return (0);         /* name does not exist                */
    case  0: return intel (ip);  /* we found the IP number             */
    case -1: return (0);         /* strange return code from ddextract */
    default: return (0);         /* dunno                              */
  }
}


/*
 * lookup_domain()
 *   DOMAIN based name lookup
 *   query a domain name server to get an IP number
 *   Returns the machine number of the machine record for future reference.
 *   Events generated will have this number tagged with them.
 *   Returns various negative numbers on error conditions.
 *
 *   if add_dom is nonzero, add default domain
 */
static DWORD lookup_domain (const char *mname, int  add_dom,
                            DWORD nameserver,  BYTE *timedout)
{
  char  namebuff [512];
  int   sec;
  DWORD response;

  response  = 0;
  *timedout = 1;     /* assume we will timeout */

  if (!nameserver)   /* no nameserver, give up now */
  {
    outsnl (_LANG(no_serv));
    _resolve_exit = 1;
    return (0);
  }

  while (*mname && *mname <= ' ')   /* kill leading spaces */
     mname++;

  if (*mname == 0)
     return (0L);

  qinit();                     /* initialize some flag fields */
  strcpy (namebuff, mname);

  if (add_dom)
  {
    int dot = strlen (namebuff) - 1;
    if (namebuff[dot] != '.')    /* if no trailing dot */
    {
      if (loc_domain)            /* there is a search list */
      {
        strcat (namebuff, ".");
        strcat (namebuff, getpath(loc_domain,1));
      }
    }
    else
      namebuff [dot] = 0;        /* kill trailing dot */
  }

  /*
   * This is not terribly good, but it attempts to use a binary
   * exponentially increasing delays.
   */

  for (sec = 2; sec < dns_timeout-1 && !_resolve_exit; sec *= 2)
  {
    if (!send_dom(namebuff,nameserver))
    {
      _resolve_timeout = 1; /* Sort of..ARP failed */
      return (0);
    }

    ip_timer_init (dom_sock,sec);
    do
    {
      tcp_tick ((sock_type*)dom_sock);

      if (ip_timer_expired(dom_sock) || chk_timeout(resolve_timeout))
         break;

      kbhit();
      if (watcbroke || (_resolve_hook && (*_resolve_hook)() == 0))
      {
        _resolve_exit = 1;
        break;
      }
      if (dom_sock->usr_yield)    /* Added, 16-Jun-97 GV */
        (*dom_sock->usr_yield)();

      if (sock_dataready((sock_type*)dom_sock))
         *timedout = 0;
    }
    while (*timedout);

    if (!*timedout)           /* got an answer */
       break;      
  }

  if (*timedout)
       _resolve_timeout = 1;
  else response = parse_domain();

  sock_close ((sock_type*)dom_sock);
  return (response);
}

/*
 * nextdomain - given domain and count = 0,1,2,..., return next larger
 * domain or NULL when no more are available
 */
static const char *nextdomain (const char *domain, int count)
{
  const char *p = domain;
  int   i;

  for (i = 0; i < count; i++)
  {
    if ((p = strchr(p,'.')) == NULL)
       return (NULL);
    p++;
  }
  return (p);
}


/*
 * resolve()
 *   convert domain name -> address resolution.
 *   returns 0 if name is unresolvable right now
 *   return value is host-order
 */
DWORD resolve (const char *name)
{
  DWORD        ip_address = 0L;
  int          count, len;
  char         namebuf [MAX_HOSTLEN];
  BYTE         timeout [MAX_NAMESERVERS];
  WORD         oldhndlcbrk;
  struct useek qp;       /* temp buffer */
  udp_Socket   ds;       /* DNS socket  */

  if (!name || *name == 0)
     return (0);

  len = strlen (name);
  if (len >= sizeof(namebuf)-1)
     return (0);

  memcpy (namebuf, name, len+1); /* make a copy of 'name'      */
  rip (namebuf);                 /* strip off '\r' and/or '\n' */

  if (isaddr(namebuf))
     return aton (namebuf);

#if defined(USE_BSD_FUNC)
  if (!called_from_ghbn)         /* This hack avoids reentrancy */
  {                              /* from gethostbyname()        */
    struct hostent *h;

    called_from_resolve = 1;     /* ditto hack ! (vice versa)   */
    h = gethostbyname (namebuf);
    called_from_resolve = 0;
    if (h)                       /* IP from host file or cache  */
    {
      /*
       * NOTE: gethostbyname() returns network order
       *       We assume IPv4 (32-bit)
       */
      DWORD ip = *(DWORD*)h->h_addr_list[0];
      return intel (ip);
    }

    /* not found in /etc/hosts file, ask the DNS server(s)
     */
  }
#endif

  if (last_nameserver == 0)    /* no nameserver, give up now */
  {
    outsnl (_LANG(no_serv));
    return (0);
  }

  if (my_ip_addr == 0)         /* not configured, give up now */
  {
    outsnl (_LANG("Cannot resolve without IP"));
    return (0);
  }

  if (dns_timeout == 0)
      dns_timeout = (UINT)sock_delay << 2;
  resolve_timeout = set_timeout (1000 * dns_timeout);

  count = 0;
  memset (&timeout, 0, sizeof(timeout));

  question    = &qp;
  dom_sock    = &ds;
  oldhndlcbrk = wathndlcbrk;
  wathndlcbrk = 1;        /* enable special interrupt mode */
  watcbroke   = 0;

  _resolve_exit = _resolve_timeout = 0;

  do
  {
    int i;

    if (strchr(namebuf,'.'))
    {
      loc_domain = NULL;
      count = -1;
    }
    else if (dns_recurse == 0 && count == 0)
    {
      loc_domain = NULL;
      count = -1;
    }
    else
    {
      loc_domain = (char*) nextdomain (def_domain, count);
      if (!loc_domain)
         count = -1;     /* use default name */
    }

    for (i = 0; i < last_nameserver; i++)
    {
      if (!timeout[i])
      {
        ip_address = lookup_domain (namebuf, count != -1,
                                    def_nameservers[i], timeout+i);
        if (ip_address)
           break;           /* got name, bail out of for loop */
        if (_resolve_exit)
           break;           /* an error occured, return to caller */
      }

      /*
       * Should we really try the other nameservers if the first
       * says the host doesn't exist? Maybe a trusting mechanism
       * is needed
       */
    }
    if (count == -1)
       break;
    count++;
  }
  while (!ip_address && !_resolve_exit);

  watcbroke   = 0;        /* always clean up */
  wathndlcbrk = oldhndlcbrk;

  return (ip_address);
}


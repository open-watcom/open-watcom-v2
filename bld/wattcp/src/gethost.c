/*
 *  BSD-like host-entry functions
 *
 *  G. Vanem  <giva@bgnett.no>
 *
 *  18.aug 1996 (GV)  - Created
 *  02.dec 1997 (GV)  - Integrated with resolve()
 *  05.jan 1998 (GV)  - Added host cache functionality
 *  18.may 1999 (GV)  - Added timeout of cached values
 *  02.may 2000 (GV)  - Added function reverse_lookup_myip()
 *
 *  todo: support real host aliases as they come from the name server
 *  todo: accept "rooted FQDN" strings as normal FQDN strings.
 *        Note: "domain_name.com" and "domain_name.com." are equivalent
 *        (both are valid forms of fully qualified domain names (FQDNs);
 *        with the period, it is referred to as a rooted FQDN). Both forms
 *        should work with all mail clients and servers.  However, using the
 *        trailing "." is rarely used (except in DNS maintenance).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <netdb.h>

#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "language.h"
#include "pcconfig.h"
#include "pc_cbrk.h"
#include "pcbsd.h"
#include "pctcp.h"
#include "bsdname.h"
#include "udp_dom.h"
#include "gethost.h"

typedef struct {
        struct dhead h;
        char   x[50];
      } question_t;

typedef struct {
        struct dhead  h;
        char   x[500];
      } answer_t;


#if defined(USE_BSD_FUNC)

typedef struct _hostent {            /* internal hostent structure  */
        char            *h_name;     /* official name of host       */
        char           **h_aliases;  /* hostname alias list         */
        u_long           h_address;  /* IP address (network order)  */
        time_t           h_timeout;  /* cached entry expiry time    */
        struct _hostent *h_next;     /* ptr to next entry (or NULL) */
      } _hostent;


#define MAX_HOST_ALIASES  5
#define MAX_CACHE_LIFE    (15*60)

int h_errno = 0;

int netdbCacheLife = MAX_CACHE_LIFE;

static char *hostFname = NULL;
static FILE *hostFile  = NULL;
static BOOL  hostClose = 0;
static _hostent *host0 = NULL;

/*
 *
 */
static __inline struct hostent *FillHostEnt (const _hostent *h)
{
  static struct in_addr adr;
  static struct hostent ret;
  static char  *list[2];

  if (!h->h_name)
     return (NULL);

  adr.s_addr      = h->h_address;
  list[0]         = (char*) &adr;
  list[1]         = NULL;
  ret.h_name      = h->h_name;
  ret.h_aliases   = h->h_aliases;
  ret.h_addrtype  = AF_INET;
  ret.h_length    = sizeof (adr.s_addr);
  ret.h_addr_list = list;
  h_errno         = NETDB_SUCCESS;
  return (&ret);
}                          

/*
 * Modify an expired cached entry or create a new node to
 * the linked list.
 */
static __inline void AddHostEnt (struct _hostent *h,
                                 const char *name,
                                 u_long      addr,
                                 BOOL        expires)
{
  if (h)         /* reuse expired entry */
  {
    if (h->h_name)
       free (h->h_name);
  }
  else           /* create a new node */
  {
    h = calloc (sizeof(*h),1);
    if (h)
    {
      h->h_next = host0;
      host0 = h;
    }
  }

  if (h)
  {
    h->h_name    = strdup (name);
    h->h_address = addr;
    if (netdbCacheLife && expires)
         h->h_timeout = time (NULL) + netdbCacheLife;
    else h->h_timeout = 0;
  }

#ifdef TEST_PROG  /* test updated cache */
  if (h)
       printf ("new entry: h->h_address = %-17.17s h->h_name = %s\n",
               inet_ntoa(*(struct in_addr*)&h->h_address), h->h_name);
  else printf ("new entry: no mem\n");
#endif
}

/*------------------------------------------------------------------*/

void ReadHostFile (const char *fname)
{
  static int been_here = 0;

  if (!fname || !*fname)
     return;

  if (been_here)  /* loading multiple hosts files */
  {
    free (hostFname);
    fclose (hostFile);
    hostFile = NULL;
  }
  been_here = 1;

  hostFname = strdup (fname);
  if (!hostFname)
     return;

  sethostent (1);
  if (!hostFile)
     return;

  while (1)
  {
    struct  hostent *h = gethostent();
    struct _hostent *h2;

    if (!h)
       break;

    h2 = calloc (sizeof(*h2),1);
    if (!h2)
    {
      outsnl (_LANG("Hostfile too big!\7"));
      break;
    }
    h2->h_name    = h->h_name;
    h2->h_aliases = h->h_aliases;
    h2->h_address = *(u_long*) h->h_addr_list[0];
    h2->h_timeout = 0;      /* don't expire */
    h2->h_next = host0;
    host0      = h2;
  }
  rewind (hostFile);
  atexit (endhostent);

}

const char *GetHostsFile (void)
{
  return (hostFname);
}

/*
 * To prevent running out of file-handles, one should close the
 * 'hosts' file before spawning a new shell.
 */
void CloseHostFile (void)
{
  fclose (hostFile);
  hostFile = NULL;
}

void ReopenHostFile (void)
{
  ReadHostFile (hostFname);
}

/*
 * Return the next (non-commented) line from the host-file
 * Format is:
 *  ip-address [=] host-name [alias..] {\n | # ..}
 */
struct hostent * gethostent (void)
{
  static _hostent h;
  char   *ip, *name, *alias;
  char    buf[100];

  if (!netdb_init() || !hostFile)
     return (NULL);

  do
  {
    if (!fgets(buf,sizeof(buf)-1,hostFile))
       return (NULL);
  }
  while (buf[0] == '#' || buf[0] == ';' || buf[0] == '\n');

  if (hostClose)
     endhostent();

  ip   = strtok (buf, " \t");
  name = strtok (NULL, " \t\n");

  if (!strcmp(ip,"0.0.0.0"))  /* inet_addr() maps 0 -> INADDR_NONE */
       h.h_address = 0;
  else h.h_address = inet_addr (ip);

  h.h_name = strdup (name);
  if (!h.h_name)
     return (NULL);

  h.h_aliases = NULL;
  alias       = strtok (NULL, " \t\n");

  if (alias && *alias != '#' && *alias != ';')
  {
    char **alist = calloc ((1+MAX_HOST_ALIASES) * sizeof(char*), 1);
    int  i = 0;
    do
    {
      if (*alias == '#' || *alias == ';')
         break;
      if (!alist || (i == MAX_HOST_ALIASES) ||
          (alist[i++] = strdup(alias)) == NULL)
         break;
      alias = strtok (NULL, " \t\n");
    }
    while (alias);
    h.h_aliases = alist;
  }
  return FillHostEnt (&h);
}

/*------------------------------------------------------------------*/

struct hostent *gethostbyname (const char *name)
{
  struct _hostent *h, ret;
  struct  in_addr  addr;
  u_long  ip;
  static  char fqdn[MAX_HOSTLEN];

  h_errno = HOST_NOT_FOUND;

  if (!netdb_init())
     return (NULL);

  if (inet_aton(name,&addr))
  {
    ret.h_name    = (char*) name; /* !!to-do: should be canonical name */
    ret.h_aliases = NULL;
    ret.h_address = addr.s_addr;
    return FillHostEnt (&ret);
  }

  for (h = host0; h; h = h->h_next)
  {
    char **alias;

    if (h->h_name && !stricmp(h->h_name,name))
    {
      /* if cached entry expired, do DNS lookup
       */
      if (h->h_timeout && time(NULL) >= h->h_timeout)
         goto expired;

      if (h->h_address)
         return FillHostEnt (h);
      return (NULL);
    }
    for (alias = h->h_aliases; alias && *alias; alias++)
        if (!stricmp(name,*alias))
        {
          if (h->h_timeout && time(NULL) >= h->h_timeout)
             goto expired;

          if (h->h_address)
             return FillHostEnt (h);
          return (NULL);
        }
  }

  if (!h)
  {
    /* Not found in linked list (hosts file or cache). Check name
     * against our own host-name (short-name or FQDN)
     */
    if (hostname[0] && !stricmp(name,hostname))
    {
      ret.h_address = gethostid();
      ret.h_name    = hostname;
      return FillHostEnt (&ret);
    }

    if (!gethostname(fqdn,sizeof(fqdn)) && !stricmp(name,fqdn))
    {
      ret.h_address = gethostid();
      ret.h_name    = fqdn;
      return FillHostEnt (&ret);
    }
  }

expired:

  if (called_from_resolve) /* prevent recursion */
     return (NULL);

 /* Not our own host-name; do a full DNS lookup
  */

  called_from_ghbn = 1;
  ip = htonl (resolve((char*)name)); /* do a normal lookup */
  called_from_ghbn = 0;

 /* Add the IP to the list even if DNS failed (but not interrupted by
  * _resolve_hook() or timedout). Thus the next call to gethostbyxx()
  * will return immediately.
  */
  if (_resolve_exit ||   /* interrupted by _resolve_hook() */
      _resolve_timeout)  /* timed out resolving */
     return (NULL);

  if (ip)                /* successfully resolved */
  {
    AddHostEnt (h, name, ip, TRUE);
    ret.h_address = ip;
    ret.h_name    = (char*) name;  /* !!to-do: should be canonical name */
    ret.h_aliases = h ? h->h_aliases : NULL;
    return FillHostEnt (&ret);
  }

  AddHostEnt (h, name, INADDR_ANY, TRUE);  /* or INADDR_NONE ? */
  return (NULL);
}

/*------------------------------------------------------------------*/

struct hostent *gethostbyaddr (const char *adr_name, int len, int type)
{
  _hostent *h, ret;
  u_long   addr;
  static   char name [MAX_HOSTLEN];
  int      rc;

  h_errno = HOST_NOT_FOUND;

  if (!netdb_init())
     return (NULL);

  if (type != AF_INET || len < sizeof(addr))
     return (NULL);

  addr = *(u_long*) adr_name;

  if ((addr == INADDR_ANY ||           /* 0.0.0.0 -> my_ip_addr */
       addr == gethostid()) &&
      !gethostname(name,sizeof(name)))
  {
    ret.h_address = gethostid();
    ret.h_name    = name;
    return FillHostEnt (&ret);
  }

  if (addr == INADDR_BROADCAST ||      /* 255.255.255.255    */
      (~ntohl(addr) & ~sin_mask) == 0) /* directed broadcast */
  {
    ret.h_address = addr;
    ret.h_name    = "broadcast";
    return FillHostEnt (&ret);
  }

  for (h = host0; h; h = h->h_next)
      if (h->h_address == addr)
      {
       /* if cached entry expired, do a new reverse lookup
        */
        if (h->h_timeout && time(NULL) >= h->h_timeout)
           break;
        return FillHostEnt (h);
      }


  rc = resolve_ip (addr, name);  /* do a reverse ip lookup */

  /* interrupted or timedout
   */
  if (!rc && (_resolve_exit || _resolve_timeout))
     return (NULL);

  if (rc)                        /* successfully resolved */
  {
    AddHostEnt (h, name, addr, TRUE);
    ret.h_address = addr;
    ret.h_name    = name;
    ret.h_aliases = h ? h->h_aliases : NULL;
    return FillHostEnt (&ret);
  }

 /* Add the IP to the list even if reverse lookup failed and not
  * interrupted by _resolve_hook(). Thus the next call to gethostbyxx()
  * will return immediately.
  */

  AddHostEnt (h, "*unknown*", addr, FALSE);
  return (NULL);
}

/*------------------------------------------------------------------*/

void sethostent (int stayopen)
{
  hostClose = (stayopen == 0);
  if (!netdb_init() || !hostFname)
     return;

  if (!hostFile)
       hostFile = fopen (hostFname, "rt");
  else rewind (hostFile);
}

/*------------------------------------------------------------------*/

static __inline void free_aliases (const struct _hostent *h)
{
  if (h->h_aliases)
  {
    int i;
    for (i = 0; i < MAX_HOST_ALIASES; i++)
        if (h->h_aliases[i])
           free (h->h_aliases[i]);
    free (h->h_aliases);
  }
}

/*------------------------------------------------------------------*/

void endhostent (void)
{
  struct _hostent *h, *next = NULL;

  if (!netdb_init() || !hostFile)
     return;   

  free (hostFname);
  fclose (hostFile);
  hostFname = NULL;
  hostFile  = NULL;

  for (h = host0; h; h = next)
  {
    free_aliases (h);
    next = h->h_next;
    free (h->h_name);
    free (h);
  }
  host0 = NULL;
  hostClose = 1;
}                          

#endif /* USE_BSD_FUNC */

/*------------------------------------------------------------------*/

u_long gethostid (void)
{
  if (!netdb_init())
     return (INADDR_NONE);
  return htonl (my_ip_addr);
}

u_long sethostid (DWORD ip)
{
  return (my_ip_addr = ntohl(ip));
}

/*
 * Fill in the reverse lookup question packet
 */
static __inline void qinit (question_t *q, DWORD ip)
{
  char *c;
  BYTE  i;

  q->h.ident   = set_timeout (0);  /* Random ID */
  q->h.flags   = intel16 (DRD);    /* recursion desired */
  q->h.qdcount = intel16 (1);
  q->h.ancount = 0;
  q->h.nscount = 0;
  q->h.arcount = 0;

  c  = q->x;
  ip = ntohl (ip);
  for (i = 0; i < 4; ++i)
  {
    BYTE x = (BYTE) ip;
    ip >>= 8;
    *c = (x < 10) ? 1 : (x < 100) ? 2 : 3;
    itoa (x,c+1,10);
    c += *c + 1;
  }
  strcpy (c, "\7in-addr\4arpa");
  c += 14;
  *(short*) c = intel16 (DTYPEPTR);
  c += sizeof(short);
  *(short*) c = intel16 (DIN);
}

/*
 * getresult() - read answer and extract host name
 *               return 0 on error, 1 on success
 */
static __inline int getresult (sock_type *s, char *name)
{
  answer_t a;
  struct   rrpart *rr;
  char    *c;    
  int      len = sock_fastread (s, (BYTE*)&a, sizeof(a));

  if (len < sizeof(struct dhead) ||
      a.h.qdcount != intel16(1)  ||
      a.h.ancount == 0)
     return (0);

  /* Skip question */
  c = a.x;
  while (*c)
        ++c;
  c += 5;

  /* Skip name */
  while (*c)
  {
    if ((*c & 0xC0) == 0xC0)
    {
      c += 2;
      break;
    }
    else
      ++c;
  }
  rr = (struct rrpart*) c;
  if (rr->rtype != intel16(DTYPEPTR))
     return (0);

  c = (char*) &rr->rdata;
  while (*c)
  {
    if ((*c & 0xC0) == 0xC0)
        c = a.x + (*(int*)c & 0x3FFF);
    strncpy (name,c+1,*c);
    name += *c;
    c += *c + 1;
    if (*c)
        *name++ = '.';
  }
  *name = 0;
  return (1);
}

/*
 * reverse_lookup() - Translate an IP into a host name.
 * Returns 1 on success, 0 on error or timeout
 */
static DWORD resolve_timeout = 0UL;

static int reverse_lookup (question_t *q, char *name, DWORD nameserver)
{
  int        sec, ret;
  int        ready = 0;
  int        quit  = 0;
  udp_Socket dom_sock;
  sock_type *s = (sock_type*)&dom_sock;

  if (!nameserver ||         /* no nameserver, give up */
      dns_timeout == 0)
     return (0);

  if (!udp_open(&s->udp, 997, nameserver, 53, NULL))
     return (0);

  for (sec = 2; sec < dns_timeout-1 && !quit && !_resolve_exit; sec *= 2)
  {
    sock_write (s, (BYTE*)q, sizeof(*q));
    ip_timer_init (&s->udp, sec);
    do
    {
      kbhit();
      tcp_tick (s);

      if (watcbroke || (_resolve_hook && (*_resolve_hook)() == 0))
      {
        _resolve_exit = 1;
        quit  = 1;
        ready = 0;
        break;
      }
      if (sock_dataready(s))
      {
        quit  = 1;
        ready = 1;
      }
      if (ip_timer_expired(&s->udp) || chk_timeout(resolve_timeout))
      {
        ready = 0;
        resolve_timeout = 1;
        break;  /* retry */
      }
    }
    while (!quit);
  }

  if (ready)
       ret = getresult (s, name);
  else ret = 0;

  sock_close (s);
  return (ret);
}

/*
 * Do a reverse lookup on `my_ip_addr'. If successfull, replace
 * `hostname' and `def_domain' with returned result.
 */
int reverse_lookup_myip (void)
{
  char myname [MAX_HOSTLEN];

  if (!resolve_ip(htonl(my_ip_addr),myname))
     return (0);

  if (debug_on >= 1)
  {
    outs (_LANG("My FQDN: "));
    outsnl (myname);
  }
  if (sethostname(myname,sizeof(myname)) < 0)
     return (0);
  return (1);
}

/*------------------------------------------------------------------*/

int resolve_ip (DWORD ip, char *result)
{
  question_t q;
  int        i;
  WORD oldhndlcbrk;

  if (dns_timeout == 0)
      dns_timeout = (UINT)sock_delay << 2;

  resolve_timeout = set_timeout (1000 * dns_timeout);
  oldhndlcbrk = wathndlcbrk;
  wathndlcbrk = 1;        /* enable special interrupt mode */
  watcbroke   = 0;
  *result     = 0;

  _resolve_exit = _resolve_timeout = 0;

  qinit (&q, ip);

  for (i = 0; i < last_nameserver; ++i)
      if (reverse_lookup(&q,result,def_nameservers[i]))
         break;

  watcbroke   = 0;          /* always clean up */
  wathndlcbrk = oldhndlcbrk;
  return (*result != 0);
}

/*------------------------------------------------------------------*/

#ifdef TEST_PROG

#include <conio.h>
#include <unistd.h>

#include "pcdbug.h"
#include "sock_ini.h"

/*
 *  Print list of hosts unsorted.
 */
static void print_hosts (void)
{
  struct _hostent *h;

  for (h = host0; h; h = h->h_next)
  {
    int i;

    printf ("h->h_address = %-17.17s h->h_name = %s; ",
            inet_ntoa(*(struct in_addr*)&h->h_address), h->h_name);

    for (i = 0; h->h_aliases && h->h_aliases[i]; i++)
    {
      if (i == 0)
        printf ("Aliases: ");
      printf ("%s,", h->h_aliases[i]);
    }
    puts ("");
  }
}

void Sleep (int time)
{
  while (time)
  {
    if (kbhit() && getch() == 27)
    {
      fputc ('\n', stderr);
      return;
    }
    fprintf (stderr, "%4d\b\b\b\b", time--);
    sleep (1);
  }
}

int main (void)
{
  struct hostent *h;
  int    wait_time;
  char  *host_name = "test-address";

  dbug_init();
  sock_init();
  print_hosts();

  wait_time = netdbCacheLife + 1;

  AddHostEnt (NULL, host_name, inet_aton("11.22.33.44",NULL), TRUE);
  h = gethostbyname (host_name);
  if (!h)
  {
    fprintf (stderr, "gethostbyname() failed!. h_errno = %d\n", h_errno);
    return (1);
  }
  fprintf (stderr, "Waiting for cache-entry to timeout..");
  Sleep (wait_time);

  fprintf (stderr, "gethostbyname() should do a DNS lookup now.\n");
  h = gethostbyname (host_name);
  if (h)
  {
    fprintf (stderr, "entry didn't timeout!.\n");
    return (1);
  }

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  Fortify_OutputStatistics();
#endif
  return (0);
}
#endif /* TEST_PROG */


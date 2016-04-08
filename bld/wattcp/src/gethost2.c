/*
 *  BSD-like host-entry functions
 *
 *  G. Vanem  <giva@bgnett.no>
 *
 *  18.aug 1996 (GV)  - Created
 *  02.dec 1997 (GV)  - Integrated with resolve()
 *  05.jan 1998 (GV)  - Added host cache functionality
 *  21.jan 1999 (GV)  - Use binary-tree data structure
 *
 *  todo: support real host aliases as they come from the DNS
 */

/*
 * NB! This file isn't used yet (btree not finished).
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
#include "btree.h"

#if defined(USE_BSD_FUNC)

typedef struct _hostent {    /* internal hostent structure */
        char   *h_name;      /* official name of host      */
        char  **h_aliases;   /* hostname alias list        */
        DWORD   h_address;   /* IP address (network order) */
      } _hostent;

typedef struct {
        struct dhead h;
        char   x[50];
      } question_t;

typedef struct {
        struct dhead  h;
        char   x[500];
      } answer_t;

#define MAX_HOST_ALIASES  5
#define MAX_CACHE_LIFE    (15*60)

int h_errno = 0;

int netdbCacheLife = MAX_CACHE_LIFE;

static char *hostFname = NULL;
static FILE *hostFile  = NULL;
static BOOL  hostClose = 0;

static TreeNode *host_root = NULL;

static int _gethostent   (struct _hostent *h);
static int host_cmp_name (struct _hostent *h1, struct _hostent *h2);
static int host_cmp_addr (struct _hostent *h1, struct _hostent *h2);

/*------------------------------------------------------------------*/

void ReadHostFile (const char *fname)
{
  if (!fname || !*fname)
     return;

  hostFname = strdup (fname);
  if (!hostFname)
     return;

  sethostent (1);
  if (!hostFile)
     return;

  while (1)
  {
    struct _hostent h;

    if (!_gethostent(&h))
       break;

    if (!tree_insert(&host_root, (void*)&h, sizeof(h), (CmpFunc)host_cmp_name))
    {
      outsnl (_LANG("Hostfile too big!\7"));
      break;
    }
  }
  atexit (endhostent);

}


/*------------------------------------------------------------------*/

static __inline struct hostent *FillHostEnt (struct _hostent *h)
{
  static char          *list[2];
  static struct in_addr adr;
  static struct hostent ret;

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
 * return the next (non-commented) line from the host-file
 * Format is:
 *  ip-address [=] host-name [alias..] {\n | # ..}
 */
static int _gethostent (struct _hostent *h)
{
  char *ip, *name, *alias;
  char  buf[100];

  if (!hostFile)
     return (0);

  do
  {
    if (!fgets(buf,sizeof(buf)-1,hostFile))
       return (0);
  }
  while (buf[0] == '#' || buf[0] == ';' || buf[0] == '\n');

  if (hostClose)
     endhostent();

  ip   = strtok (buf," \t");
  name = strtok (NULL,"= \t\n");

  h->h_address = inet_addr (ip);
  h->h_name    = strdup (name);
  if (!h->h_name)
     return (NULL);

  h->h_aliases = NULL;
  alias        = strtok (NULL," \t\n");

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
      alias = strtok (NULL," \t\n");
    }
    while (alias);
    h->h_aliases = alist;
  }
  return (1);
}

struct hostent *gethostent (void)
{
  struct _hostent h;

  if (_gethostent(&h))
     return FillHostEnt (&h);
  return (NULL);
}

/*------------------------------------------------------------------*/

struct hostent *gethostbyname (const char *name)
{
  struct _hostent *h, ret;
  struct  hostent *h2;
  struct  in_addr  adr;
  DWORD   ip;

  h_errno = HOST_NOT_FOUND;

  if (inet_aton(name,&adr))
  {
    ret.h_name    = (char*) name;
    ret.h_aliases = NULL;
    ret.h_address = adr.s_addr;
    return FillHostEnt (&ret);
  }

  ret.h_name = (char*) name;
  h = (struct _hostent*) tree_find (host_root, (void*)&ret,
                                    (CmpFunc)host_cmp_name);
  if (h)
  {
#if 1
    printf ("ghbn: address = %-17.17s name = %s\n",
            inet_ntoa (*(struct in_addr*)&h->h_address), h->h_name);
#endif

    if (h->h_address)
       return FillHostEnt (h);
    return (NULL);
  }

  if (called_from_resolve) /* prevent recursion */
     return (NULL);

 /* allocate name before calling resolve().
  */
  ret.h_name = strdup (name);
  if (!ret.h_name)
  {
    h_errno = NETDB_INTERNAL;
    errno = ENOMEM;
    return (NULL);
  }

 /* not found in hostfile or cache; do a full DNS lookup
  */

  called_from_ghbn = 1;
  ip = htonl (resolve((char*)name));     /* do a normal lookup */
  called_from_ghbn = 0;

  /* interrupted by _resolve_hook()
   */
  if (_resolve_exit)
  {
    free (ret.h_name);
    h_errno = NETDB_INTERNAL;
    errno = EINTR;
    return (NULL);
  }

 /* Add the IP to the list even if DNS failed and not interrupted by
  * _resolve_hook(). Thus the next call to gethostbyxx() will return
  * immediately
  */

  ret.h_address = ip ? ip : INADDR_ANY;  /* or INADDR_NONE ? */
  ret.h_aliases = NULL;

  if (tree_insert (&host_root, (void*)&ret, sizeof(ret), (CmpFunc)host_cmp_name))
       h2 = FillHostEnt (&ret);
  else h2 = NULL;

#ifdef TEST_PROG  /* test updated cache */
  if (h2)
       printf ("new entry: h->h_address = %-17.17s h->h_name = %s\n",
               inet_ntoa (*(struct in_addr*)h2->h_addr), h2->h_name);
  else printf ("new entry: no mem\n");
#endif

  return (ip ? h2 : NULL);
}

/*------------------------------------------------------------------*/

struct hostent *gethostbyaddr (const char *adr_name, int len, int type)
{
  struct _hostent *h, ret;
  struct  hostent *h2;
  DWORD   addr;
  char    name [MAX_HOSTLEN];

  h_errno = HOST_NOT_FOUND;

  if (type != AF_INET || len != sizeof(addr))
     return (NULL);

  addr = *(DWORD*) adr_name;

  ret.h_address = addr;
  h = (_hostent*) tree_find (host_root, (void*)&ret, (CmpFunc)host_cmp_addr);
  if (h)
     return FillHostEnt (h);

  if ((ret.h_name = strdup(name)) == NULL)
  {
    h_errno = NETDB_INTERNAL;
    errno = ENOMEM;
    return (NULL);
  }

  if (resolve_ip(addr,name))   /* do a reverse ip lookup */
     h2 = FillHostEnt (&ret);
  else
  {
    ret.h_name = "";
    h2 = NULL;
  }
  tree_insert (&host_root, (void*)&ret, sizeof(ret), (CmpFunc)host_cmp_name);
  return (h2);
}

/*------------------------------------------------------------------*/

void sethostent (int stayopen)
{
  hostClose = (stayopen == 0);
  if (!hostFname)
     return;

  if (!hostFile)
       hostFile = fopen (hostFname,"rt");
  else rewind (hostFile);
}

/*------------------------------------------------------------------*/

static __inline void free_aliases (struct _hostent *h)
{
  if (h->h_aliases)
  {
    int i;
    for (i = 0; i < MAX_HOST_ALIASES; i++)
    {
      if (h->h_aliases[i])
           free (h->h_aliases[i]);
      else break;
    }
    free (h->h_aliases);
  }
}

static void free_nodes (TreeNode *root)
{
  if (root)
  {
    free_aliases ((_hostent*)root->info);
    free_nodes (root->left);
    free_nodes (root->right);
  }
}

void endhostent (void)
{
  if (hostFile)
  {
    free (hostFname);
    fclose (hostFile);
    hostFname = NULL;
    hostFile  = NULL;

    free_nodes (host_root);
  }
  hostClose = 1;
}

/*------------------------------------------------------------------*/

u_long gethostid (void)
{
  return htonl (my_ip_addr);
}

u_long sethostid (u_long ip)
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

  q->h.ident   = 0x5271;         /* 7152h = 'qR' */
  q->h.flags   = intel16 (DRD);  /* recursion desired */
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
  strcpy (c,"\7in-addr\4arpa");
  c += 14;
  *(short*) c = intel16 (DTYPEPTR);
  c += sizeof(short);
  *(short*) c = intel16 (DIN);
}

/*
 * getresult() - read answer and extract host name
 *               return 0 on error, 1 on success
 */
static __inline int getresult (udp_Socket *s, char *name)
{
  answer_t a;
  struct   rrpart *rr;
  char    *c;
  int      len = sock_fastread ((sock_type*)s, (BYTE*)&a, sizeof(a));

  if (len < sizeof(struct dhead) ||
      a.h.qdcount != intel16(1) || a.h.ancount == 0)
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
  rr = (struct rrpart *) c;
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
  int        i, ret;
  int        ready = 0;
  udp_Socket dom_sock;

  if (!nameserver ||         /* no nameserver, give up */
      dns_timeout == 0)
     return (0);

  udp_open (&dom_sock, 997, nameserver, 53, NULL);

  for (i = 2; i < 17 && !_resolve_exit; i *= 2)
  {
    sock_write ((sock_type*)&dom_sock, (BYTE*)q, sizeof(*q));
    ip_timer_init (&dom_sock, i);
    do
    {
      kbhit();
      tcp_tick ((sock_type*)&dom_sock);

      if (watcbroke || (_resolve_hook && (*_resolve_hook)() == 0))
      {
        _resolve_exit = 1;
        break;
      }
      if (ip_timer_expired(&dom_sock) || chk_timeout(resolve_timeout))
         break;

      if (sock_dataready((sock_type*)&dom_sock))
         ready = 1;
    }
    while (!ready);
  }
  if (ready)
       ret = getresult (&dom_sock, name);
  else ret = 0;

  sock_close ((sock_type*)&dom_sock);
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
  WORD       oldhndlcbrk;
  int        i;

  if (dns_timeout == 0)
      dns_timeout = sock_delay << 2;

  resolve_timeout = set_timeout (100 * dns_timeout);
  oldhndlcbrk   = wathndlcbrk;
  wathndlcbrk   = 1;        /* enable special interrupt mode */
  watcbroke     = 0;
  *result       = 0;
  _resolve_exit = 0;

  qinit (&q,ip);

  for (i = 0; i < last_nameserver; ++i)
  {
    if (!reverse_lookup(&q,result,def_nameservers[i]));
       break;
  }
  watcbroke   = 0;          /* always clean up */
  wathndlcbrk = oldhndlcbrk;
  return (*result != 0);
}

/*------------------------------------------------------------------*/

static int host_cmp_name (struct _hostent *h1, struct _hostent *h2)
{
  return stricmp (h1->h_name, h2->h_name);
}

static int host_cmp_addr (struct _hostent *h1, struct _hostent *h2)
{
  return (int)(h1->h_address - h2->h_address);
}

/*------------------------------------------------------------------*/

#ifdef TEST_PROG

#include "sock_ini.h"
#include "pcdbug.h"

/*
 *  Print binary tree ASCII-sorted on `h_name' (in-order)
 */
static void print_hosts (TreeNode *node)
{
  if (node)
  {
    print_hosts (node->left);
    if (node->info)
    {
      int i;
      _hostent *h = (_hostent*) node->info;

      printf ("address = %-17.17s name = %s; ",
              inet_ntoa (*(struct in_addr*)&h->h_address), h->h_name);
      for (i = 0; h->h_aliases && h->h_aliases[i]; i++)
          printf ("%s,",h->h_aliases[i]);
      puts ("");
    }
    print_hosts (node->right);
  }
}

int main (void)
{
  dbug_init();
  sock_init();
  print_hosts (host_root);
  return (0);
}
#endif

#endif /* USE_BSD_FUNC */


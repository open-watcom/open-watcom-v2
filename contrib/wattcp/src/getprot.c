/*
 *  BSD getprotobyxx() functions
 *
 *  G. Vanem <giva@bgnett.no>
 *
 *  21.aug 1996 - Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "pcconfig.h"
#include "pcbsd.h"

#if defined(USE_BSD_FUNC)

struct _protoent {
        char   *p_name;          /* official name of protocol */
        char  **p_aliases;       /* alias list                */
        int     p_proto;         /* protocol number           */
        struct _protoent *next;  /* next entry in linked list */
      };

static struct _protoent *_proto0 = NULL;
static char   *protoFname        = NULL;
static FILE   *protoFile         = NULL;
static BOOL    protoClose        = 0;

/*------------------------------------------------------------------*/

void ReadProtoFile (const char *fname)
{
  static int been_here = 0;

  if (!fname || !*fname)
     return;

  if (been_here)  /* loading multiple protocol files */
  {
    free (protoFname);
    fclose (protoFile);
    protoFile = NULL;
  }
  been_here = 1;

  protoFname = strdup (fname);
  if (!protoFname)
     return;

  setprotoent (1);
  if (!protoFile)
     return;

  while (1)
  {
    struct _protoent *p, *p2 = (struct _protoent*) getprotoent();

    if (!p2)
       break;

    p = malloc (sizeof(*p));
    if (!p)
    {
      outsnl ("Protocol-file too big!\7");
      break;
    }
    *p = *p2;
    p->next = _proto0;
    _proto0 = p;
  }
  rewind (protoFile);
  atexit (endprotoent);

#if 0  /* test */
  {
    struct _protoent *p;
    for (p = _proto0; p; p = p->next)
      printf ("proto %3d = `%s'\n", p->p_proto,p->p_name);
  }
#endif
}

/*------------------------------------------------------------------*/

const char *GetProtoFile (void)
{
  return (protoFname);
}

void CloseProtoFile (void)
{
  fclose (protoFile);
  protoFile = NULL;
}

void ReopenProtoFile (void)
{
  ReadProtoFile (protoFname);
}

/*------------------------------------------------------------------*/

struct protoent * getprotoent (void)
{
  static struct _protoent p;
  char   buf[100];

  if (!netdb_init() || !protoFile)
     return (NULL);

  do
  {
    if (!fgets(buf,sizeof(buf)-1,protoFile))
       return (NULL);
  }
  while (buf[0] == '#' || buf[0] == '\n');

  if (protoClose)
     endprotoent();

/*  Protocol  Name
 *  ----------------------------------------
 *  0         reserved
 *  1         icmp, internet control message
 *  2         igmp, internet group management
 *  3         ggp, gateway-gateway protocol
 */

  p.p_proto = atoi  (strtok (buf," \t\n"));
  p.p_name  = strdup(strtok (NULL," ,\t\n"));
  if (!p.p_name)
     return (NULL);

  p.p_aliases = NULL;
  return (struct protoent*) &p;
}

/*------------------------------------------------------------------*/

struct protoent * getprotobyname (const char *proto)
{
  static struct protoent udp = { "udp", NULL, UDP_PROTO };
  static struct protoent tcp = { "tcp", NULL, TCP_PROTO };
  struct _protoent *p;

  if (!stricmp(proto,"udp"))  /* no chance these are renumbered !? */
     return (&udp);

  if (!stricmp(proto,"tcp"))
     return (&tcp);

  if (!netdb_init())
     return (NULL);

  for (p = _proto0; proto && p; p = p->next)
  {
    char **alias;

    if (p->p_name && !stricmp(p->p_name,proto))
       return (struct protoent*) p;

    /* aliases not supported yet
     */
    for (alias = p->p_aliases; alias && *alias; alias++) 
        if (!stricmp(*alias,proto))
           return (struct protoent*) p;
  }
  return (NULL);
}

/*------------------------------------------------------------------*/

struct protoent * getprotobynumber (int proto)
{
  struct _protoent *p;

  if (!netdb_init())
     return (NULL);

  for (p = _proto0; p && proto; p = p->next)
      if (p->p_proto == proto)
         return (struct protoent*) p;
  return (NULL);
}

/*------------------------------------------------------------------*/

void setprotoent (int stayopen)
{
  protoClose = (stayopen == 0);

  if (!netdb_init() || !protoFname)
     return;

  if (!protoFile)
       protoFile = fopen (protoFname,"rt");
  else rewind (protoFile);
}

/*------------------------------------------------------------------*/

void endprotoent (void)
{
  struct _protoent *p, *next = NULL;

  if (!netdb_init() || !protoFile)
     return;

  free (protoFname);
  fclose (protoFile);
  protoFname = NULL;
  protoFile  = NULL;

  for (p = _proto0; p; p = next)
  {
    next = p->next;
    free (p->p_name);
    free (p);
  }
  _proto0 = NULL;
  protoClose = 1;
}

#endif  /* USE_BSD_FUNC */

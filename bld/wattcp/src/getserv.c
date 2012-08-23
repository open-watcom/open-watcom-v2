/*
 *  Simple BSD-like services functions
 *
 *  G. Vanem <giva@bgnett.no>
 *
 *  20.aug 1996 - Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "language.h"
#include "pcconfig.h"
#include "pcbsd.h"

#if defined(USE_BSD_FUNC)

struct _servent {
        struct  servent  s;
        struct _servent *next;
      };


#define MAX_SERV_ALIASES  5

static struct _servent *serv0 = NULL;
static char   *servFname      = NULL;
static FILE   *servFile       = NULL;
static BOOL    servClose      = 0;

/* For DOSX targets we use a small hash table to speed up searches
 */
#if (DOSX)
  #define PORT_HASH_SIZE  256   /* must be 2^n */
  #define GET_HASH(p)     port_hash [(p) & (PORT_HASH_SIZE-1)]
  #define SET_HASH(se)    port_hash [se->s.s_port & (PORT_HASH_SIZE-1)] = se
  static struct _servent *port_hash [PORT_HASH_SIZE];
#else
  #define GET_HASH(p)     serv0
  #define SET_HASH(se)    ((void)0)
#endif

/*------------------------------------------------------------------*/

void ReadServFile (const char *fname)
{
  static int been_here = 0;

  if (!fname || !*fname)
     return;

  if (been_here)  /* loading multiple service files */
  {
    free (servFname);
    fclose (servFile);
    servFile = NULL;
  }
  been_here = 1;

  servFname = strdup (fname);
  if (!servFname)
     return;

  setservent (1);
  if (!servFile)
     return;

  while (1)
  {
    struct _servent *se, *se2 = (struct _servent*) getservent();

    if (!se2)
       break;

    if ((se = malloc(sizeof(*se))) == NULL)
    {
      outsnl (_LANG("Service-file too big!\7"));
      return;
    }
    *se = *se2;
    se->next = serv0;
    serv0    = se;
    SET_HASH (se);
  }
  rewind (servFile);
  atexit (endservent);
}

/*------------------------------------------------------------------*/

const char *GetServFile (void)
{
  return (servFname);
}

void CloseServFile (void)
{
  fclose (servFile);
  servFile = NULL;
}

void ReopenServFile (void)
{
  ReadServFile (servFname);
}

/*------------------------------------------------------------------*/

struct servent * getservent (void)
{
  static struct _servent se;
  WORD   port;
  char  *name, *alias, *proto;
  char   buf[200];

  if (!netdb_init() || !servFile)
     return (NULL);

  do
  {
    do
    {
      if (!fgets(buf,sizeof(buf)-1,servFile))
         return (NULL);
    }
    while (buf[0] == '#' || buf[0] == '\n');

    if (servClose)
       endservent();

    /*  # Service         port/prot       alias(es)
     *  #-------------------------------------------
     *    rtmp            1/udp   <- only support "udp"/"tcp"
     *    echo            7/tcp
     *    echo            7/udp
     *    discard         9/tcp           sink null
     *    discard         9/udp           sink null
     */

    name  = strtok (buf, " \t");
    port  = intel16 (atoi (strtok (NULL, "/ \t\n")));
    proto = strtok (NULL, " \t\n");

  }
  while (stricmp(proto,"udp") && stricmp(proto,"tcp"));

  se.s.s_name    = strdup (name);
  se.s.s_proto   = strdup (proto);
  se.s.s_port    = port;
  se.s.s_aliases = NULL;

  if (!se.s.s_name || !se.s.s_proto)
     return (NULL);

  alias = strtok (NULL, " \t\n");

  if (alias && *alias != '#' && *alias != ';')
  {
    char **alist = calloc ((1+MAX_SERV_ALIASES) * sizeof(char*), 1);
    int  i = 0;
    do
    {
      if (*alias == '#' || *alias == ';')
         break;
      if (!alist || (i == MAX_SERV_ALIASES) ||
          (alist[i++] = strdup(alias)) == NULL)
         break;
      alias = strtok (NULL, " \t\n");
    }
    while (alias);
    se.s.s_aliases = alist;
  }
  return (&se.s);
}

/*------------------------------------------------------------------*/

struct servent * getservbyname (const char *serv, const char *proto)
{
  struct _servent *se;

  if (!netdb_init())
     return (NULL);

  for (se = serv0; se && serv; se = se->next)
  {
    char **alias;
    BOOL chk_prot = 0;

    if (se->s.s_name && !stricmp(se->s.s_name,serv))
       chk_prot = TRUE;

    else for (alias = se->s.s_aliases; alias && *alias; alias++)
             if (!stricmp(serv,*alias))
             {
               chk_prot = TRUE;
               break;
             }
    if (chk_prot && (!proto || !stricmp(se->s.s_proto,proto)))
       return (&se->s);
  }
  return (NULL);
}

/*------------------------------------------------------------------*/

struct servent * getservbyport (int port, const char *proto)
{
  struct _servent *se;

  if (!netdb_init())
     return (NULL);

  for (se = GET_HASH(port); se && port; se = se->next)
  {
    if (se->s.s_port == port &&
        (!proto || !stricmp(se->s.s_proto,proto)))
       return (&se->s);
  }
  return (NULL);
}

/*------------------------------------------------------------------*/

void setservent (int stayopen)
{
  servClose = (stayopen == 0);
  if (!netdb_init() || !servFname)
     return;

  if (!servFile)
       servFile = fopen (servFname, "rt");
  else rewind (servFile);
}

/*------------------------------------------------------------------*/

void endservent (void)
{
  struct _servent *se, *next = NULL;

  if (!netdb_init() || !servFile)
     return;

  free (servFname);
  fclose (servFile);
  servFname = NULL;
  servFile  = NULL;

  for (se = serv0; se; se = next)
  {
    if (se->s.s_aliases)
    {
      int i;
      for (i = 0; i < MAX_SERV_ALIASES; i++)
          if (se->s.s_aliases[i])
             free (se->s.s_aliases[i]);
      free (se->s.s_aliases);
    }
    next = se->next;
    free (se->s.s_name);
    free (se->s.s_proto);
    free (se);
  }
  serv0 = NULL;
  servClose = 1;
}

#endif /* USE_BSD_FUNC */

#if defined(TEST_PROG)

#include "pcdbug.h"
#include "sock_ini.h"

int main (void)
{
  struct _servent *se;
  int    i;

  dbug_init();
  sock_init();

  for (se = serv0; se; se = se->next)
  {
    printf ("proto %-6.6s  port %4d  name %-10.10s  aliases:",
            se->s.s_proto, intel16(se->s.s_port), se->s.s_name);
    for (i = 0; se->s.s_aliases && se->s.s_aliases[i]; i++)
         printf (" %s", se->s.s_aliases[i]);
    puts ("");
  }
  return (0);
}
#endif /* TEST_PROG */



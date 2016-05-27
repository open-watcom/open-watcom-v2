/*
 *  Simple BSD-like network-entry functions
 *
 *  G. Vanem <giva@bgnett.no>
 *
 *  18.aug 1996 - Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "misc.h"
#include "pcconfig.h"
#include "pcbsd.h"

#if defined(USE_BSD_FUNC)

struct _netent {
        char   *n_name;         /* official name of net  */
        char  **n_aliases;      /* alias list            */
        int     n_addrtype;     /* net address type      */
        DWORD   n_net;          /* network #,host order! */
        struct _netent *next;
      };

#define MAX_NETW_ALIASES  5

static struct _netent *network0 = NULL;
static char   *networkFname     = NULL;
static FILE   *networkFile      = NULL;
static int     networkClose     = 0;

void ReadNetworksFile (const char *fname)
{
  static int been_here = 0;

  if (!fname || !*fname)
     return;

  if (been_here)  /* loading multiple network files */
  {
    free (networkFname);
    fclose (networkFile);
    networkFile = NULL;
  }
  been_here = 1;

  networkFname = strdup (fname);
  if (!networkFname)
     return;

  setnetent (1);
  if (!networkFile)
     return;

  while (1)
  {
    struct _netent *n, *n2 = (struct _netent*) getnetent();

    if (!n2 || (n = malloc(sizeof(*n))) == NULL)
       break;

    *n = *n2;
    n->next  = network0;
    network0 = n;
  }
  rewind (networkFile);
  atexit (endnetent);

#if 0  /* test */
  {
    struct _netent *n;

    printf ("%s entries:\n", fname);
    for (n = network0; n; n = n->next)
    {
      int i;
      printf ("net = %-15.15s name = %-10.10s  alias:",
              inet_ntoa(inet_makeaddr(n->n_net,0)), n->n_name);
      for (i = 0; n->n_aliases && n->n_aliases[i]; i++)
            printf (" %s", n->n_aliases[i]);
      puts ("");
    }
  }
#endif
}

/*
 * Return name of networks file
 */
const char *GetNetworksFile (void)
{
  return (networkFname);
}

/*
 * To prevent running out of file-handles, one should close the
 * 'networks' file before spawning a new shell.
 */
void CloseNetworksFile (void)
{
  fclose (networkFile);
  networkFile = NULL;
}

void ReopenNetworksFile (void)
{
  ReadNetworksFile (networkFname);
}

/*
 * Return the next (non-commented) line from the network-file
 * Format is:
 *   name [=] net [alias..] {\n | # ..}
 *
 * e.g.
 *   loopback     127
 *   arpanet      10   arpa
 */
struct netent * getnetent (void)
{
  static struct netent n;
  char  *name, *net, *alias;
  char   buf[100];

  if (!netdb_init())
     return (NULL);

  do
  {
    if (!fgets(buf,sizeof(buf)-1,networkFile))
       return (NULL);
  }
  while (buf[0] == '#' || buf[0] == ';' || buf[0] == '\n');

  if (networkClose)
     endnetent();

  name = strtok (buf, " \t");
  net  = strtok (NULL,"= \t\n");

  n.n_net  = inet_network (net);
  n.n_name = strdup (name);
  if (!n.n_name)
     return (NULL);

  n.n_addrtype = AF_INET;
  n.n_aliases  = NULL;
  alias        = strtok (NULL," \t\n");

  if (alias && *alias != '#' && *alias != ';')
  {
    char **alist = calloc ((1+MAX_NETW_ALIASES) * sizeof(char*), 1);
    int  i = 0;
    do
    {
      if (*alias == '#' || *alias == ';')
         break;
      if (!alist || (i == MAX_NETW_ALIASES) ||
          (alist[i++] = strdup(alias)) == NULL)
         break;
      alias = strtok (NULL," \t\n");
    }
    while (alias);
    n.n_aliases = alist;
  }
  return (&n);
}

/*
 * Return a 'netent' structure for network 'name' or
 * NULL if not found.
 */
struct netent * getnetbyname (const char *name)
{
  struct _netent *n;

  if (!netdb_init())
     return (NULL);

  for (n = network0; n; n = n->next)
  {
    char **alias;

    if (n->n_name && !stricmp(n->n_name,name))
       return (struct netent*) n;

    for (alias = n->n_aliases; alias && *alias; alias++)
        if (!stricmp(name,*alias))
           return (struct netent*) n;
  }
  return (NULL);
}

/*
 * Return a 'netent' structure for network number 'net' or
 * NULL if not found.
 */
struct netent *getnetbyaddr (long net, int type)
{
  struct _netent *n;

  if (!netdb_init())
     return (NULL);

  for (n = network0; n; n = n->next)
      if ((DWORD)net == n->n_net && type == n->n_addrtype)
         return (struct netent*) n;
  return (NULL);
}

/*
 * Open the network file.
 */
void setnetent (int stayopen)
{
  networkClose = (stayopen == 0);

  if (!netdb_init() || !networkFname)
     return;

  if (!networkFile)
       networkFile = fopen (networkFname, "rt");
  else rewind (networkFile);
}

/*
 * Close the network file and free associated memory
 */
void endnetent (void)
{
  struct _netent *n, *next = NULL;

  if (!netdb_init() || !networkFile)
     return;

  free (networkFname);
  fclose (networkFile);
  networkFname = NULL;
  networkFile  = NULL;

  for (n = network0; n; n = next)
  {
    if (n->n_aliases)
    {
      int i;
      for (i = 0; i < MAX_NETW_ALIASES; i++)
         if (n->n_aliases[i])
           free (n->n_aliases[i]);
      free (n->n_aliases);
    }
    next = n->next;
    free (n->n_name);
    free (n);
  }
  network0 = NULL;
  networkClose = 1;
}

#endif /* USE_BSD_FUNC */

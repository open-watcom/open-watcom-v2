/*
 *  `/etc/ethers' file functions for Watt-32
 *
 *  G. Vanem <giva@bgnett.no>
 *
 *  28.apr 2000 - Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "pcbsd.h"
#include "pcarp.h"
#include "pcconfig.h"
#include "udp_dom.h"

#if defined(USE_ETHERS)

#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_ether.h>

struct ethent {
       struct ether_addr eth_addr;  /* ether/MAC address of host */
       struct in_addr    ip_addr;   /* IP-address (host order)   */
       char             *name;      /* host-name for IP-address  */
       struct ethent    *next;
     };

static struct ethent *eth0 = NULL;
static char   *ethersFname = NULL;

static int  getethent (const char *line, struct ether_addr *e, char *name);
static void endethent (void);

void InitEthersFile (const char *fname)
{
  if (fname && *fname)
     ethersFname = strdup (fname);
}

void ReadEthersFile (void)
{
  FILE *file;
  char  buf [100];

  if (!ethersFname)
     return;

  file = fopen (ethersFname, "rt");
  if (!file)
     return;

  while (fgets (buf, sizeof(buf)-1, file))
  {
    struct ethent     *e;
    struct hostent    *h;
    struct ether_addr  eth;
    char   hostname [MAX_HOSTLEN];
    int    save;

    if (buf[0] == '#' || buf[0] == ';' || buf[0] == '\n')
       continue;

    if (!getethent (buf, &eth, hostname))
       continue;

    save = called_from_resolve;
    called_from_resolve = TRUE;   /* prevent a DNS lookup */
    h = gethostbyname (hostname);
    called_from_resolve = save;

    if (!h)
       continue;

    e = calloc (sizeof(*e), 1);
    if (!e)
       break;

    memcpy (&e->eth_addr, &eth, sizeof(e->eth_addr));
    e->ip_addr.s_addr = *(u_long*) h->h_addr;
    e->name = h->h_name;
    _arp_add_cache (ntohl(e->ip_addr.s_addr),
                    (eth_address*)&e->eth_addr, FALSE);
    e->next = eth0;
    eth0 = e;
  }

  fclose (file);
  atexit (endethent);

#if 0  /* test */
  {
    struct ethent *e;

    printf ("`%s' entries:\n", ethersFname);
    for (e = eth0; e; e = e->next)
        printf ("%02X:%02X:%02X:%02X:%02X:%02X = %-15.15s (%s)\n",
                (int)e->eth_addr.ether_addr_octet[0],
                (int)e->eth_addr.ether_addr_octet[1],
                (int)e->eth_addr.ether_addr_octet[2],
                (int)e->eth_addr.ether_addr_octet[3],
                (int)e->eth_addr.ether_addr_octet[4],
                (int)e->eth_addr.ether_addr_octet[5],
                inet_ntoa(e->ip_addr), e->name);
  }
#endif
}

/*
 * Parse a string of text containing an ethernet address and hostname
 * and separate it into its component parts.
 */
static int getethent (const char *line, struct ether_addr *e, char *name)
{
  int   len;
  int   tmp[6];
  BYTE  eth[6];
  const char *host;

  if (sscanf (line, "%x:%x:%x:%x:%x:%x",
              &tmp[0], &tmp[1], &tmp[2],
              &tmp[3], &tmp[4], &tmp[5]) != DIM(tmp))
    return (0);

  host = inet_atoeth (line, eth);
  len  = strlen (host);
  if (len < 1 || len >= MAX_HOSTLEN)
     return  (0);

  sscanf (host, "%s", name);
  memcpy (e, &eth, sizeof(*e));
  return (1);
}

/*
 * Free memory in 'eth0' array
 */
static void endethent (void)
{
  struct ethent *e, *next = NULL;

  for (e = eth0; e; e = next)
  {
    next = e->next;
    free (e);
  }
  eth0 = NULL;
}
#endif  /* USE_ETHERS */


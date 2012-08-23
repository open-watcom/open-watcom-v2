/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 */

#define BSD    /* in order to include SIOxx macros in <sys/ioctl.h> */

#include "socket.h"
#include "pcdbug.h"
#include "pcmulti.h"

#if defined(USE_BSD_FUNC)

static int file_ioctrl  (Socket *socket, long cmd, char *argp);
static int iface_ioctrl (Socket *socket, long cmd, char *argp);
static int arp_ioctrl   (Socket *socket, long cmd, char *argp);
static int waterm_ioctrl(Socket *socket, long cmd, char *argp);

#define NO_CMD(cmd) \
        SOCK_DEBUGF ((socket, ", unsupported cmd %d, group %c", \
                      (int)((cmd) & IOCPARM_MASK), (char)IOCGROUP(cmd)))
#ifdef USE_DEBUG
static const char *get_ioctl_cmd (long cmd);
#endif

#define NUM_IFACES 1

int ioctlsocket (int s, long cmd, char *argp)
{
  Socket *socket = _socklist_find (s);

  SOCK_PROLOGUE (socket, "\nioctlsocket:%d", s);

#ifdef USE_DEBUG
  _sock_debugf (socket, ", %s", get_ioctl_cmd(cmd));
#endif

  switch (IOCGROUP(cmd))
  {
    case 'f':
         return file_ioctrl (socket, cmd, argp);

    case 'I':
         return iface_ioctrl (socket, cmd, argp);

    case 's':
         return waterm_ioctrl (socket, cmd, argp);

    default:
         NO_CMD (cmd);
         SOCK_ERR (ESOCKTNOSUPPORT);
  }
  return (-1);
}

/*
 *  IO-control for "file" handles (i.e. stream/datagram sockets)
 */
static int file_ioctrl (Socket *socket, long cmd, char *argp)
{
  int len;

  VERIFY_RW (argp, sizeof(*argp));

  switch ((DWORD)cmd)
  {
    case FIONREAD:
         if (socket->so_type != SOCK_DGRAM &&
             socket->so_type != SOCK_STREAM)
         {
           SOCK_ERR (EBADF);
           return (-1);
         }
         if (socket->so_state & SS_PRIV)  /* must be SOCK_DGRAM */
              len = sock_recv_used (socket->udp_sock);
         else len = sock_rbused ((sock_type*)socket->tcp_sock);

         SOCK_DEBUGF ((socket, " %d", len));
         if (len >= 0)
            *(u_long*)argp = len;
         break;

    case FIONBIO:                 /* set nonblocking I/O on/off */
         if (socket->so_type != SOCK_STREAM)
         {
           SOCK_ERR (EBADF);
           return (-1);
         }
         if (*argp)
              socket->so_state |=  SS_NBIO;
         else socket->so_state &= ~SS_NBIO;
         SOCK_DEBUGF ((socket, " %d", (socket->so_state & SS_NBIO) ? 1 : 0));
         break;

    default:
         NO_CMD (cmd);
         SOCK_ERR (ESOCKTNOSUPPORT);
         return (-1);
  }
  return (0);
}

/*
 * Return name of this interface.
 * We only support a single interface at a time.
 */
static char eth_ifname [IFNAMSIZ] = "eth0";
static char tok_ifname [IFNAMSIZ] = "tr0";
static char fddi_ifname[IFNAMSIZ] = "fddi0";
static char ppp_ifname [IFNAMSIZ] = "ppp0";
static char slp_ifname [IFNAMSIZ] = "slp0";

static void get_ifname (char *if_name)
{
  switch (_pktdevclass)
  {
    case PD_ETHER:
         strcpy (if_name, eth_ifname);
         break;
    case PD_TOKEN:
         strcpy (if_name, tok_ifname);
         break;
    case PD_FDDI:
         strcpy (if_name, fddi_ifname);
         break;
    case PD_SLIP:
         strcpy (if_name, slp_ifname);
         break;
    case PD_PPP:
         strcpy (if_name, ppp_ifname);
         break;
    default:
         strcpy (if_name, "??");
         break;
  }
}

#ifdef NOT_USED_YET
/*
 * Set a new name for this interface.
 */
static void set_ifname (char *if_name)
{
  switch (_pktdevclass)
  {
    case PD_ETHER:
         strcpy (eth_ifname, if_name);
         break;
    case PD_TOKEN:
         strcpy (tok_ifname, if_name);
         break;
    case PD_SLIP:
         strcpy (slp_ifname, if_name);
         break;
    case PD_PPP:
         strcpy (ppp_ifname, if_name);
         break;
    default:
         break;
  }
}

static struct ifnet *eth_ifnet (void)
{
  static struct ifnet net;

  /* to-do: fill info */
  return (&net);
}

static struct ifnet *tok_ifnet (void)
{
  static struct ifnet net;

  /* to-do: fill info */
  return (&net);
}
#endif

/*
 * Handler for interface request get/set commands
 */
static int iface_ioctrl (Socket *socket, long cmd, char *argp)
{
  struct ifreq       *ifr = (struct ifreq *) argp;
  struct ifconf      *ifc = (struct ifconf*) argp;
  struct sockaddr_in *sin;
  /*const*/ eth_address  *eth;
  int   len;
 
  VERIFY_RW (argp, sizeof(*ifr));

  switch ((DWORD)cmd)
  {
    case SIOCSARP:
    case SIOCGARP:
    case SIOCDARP:
         return arp_ioctrl (socket, cmd, argp);

    case SIOCGIFADDR:                /* get interface address */
    case OSIOCGIFADDR:
         get_ifname (ifr->ifr_name);

         if (ifr->ifr_addr.sa_family == AF_INET)
         {
           struct sockaddr_in *sin = (struct sockaddr_in*) &ifr->ifr_addr;
           sin->sin_addr.s_addr = htonl (my_ip_addr);
           break;
         }
#ifdef USE_INET6
         if (ifr->ifr_addr.sa_family == AF_INET6)
         {
           struct sockaddr_in6 *sin = (struct sockaddr_in6*) &ifr->ifr_addr;
           memcpy (&sin->sin6_addr, my_ip6_addr, sizeof(sin->sin6_addr));
           break;
         }
#endif
         if (_pktdevclass == PD_TOKEN ||  /* otherwise return MAC addr? */
             _pktdevclass == PD_ETHER ||
             _pktdevclass == PD_FDDI)
              memcpy (&ifr->ifr_addr.sa_data, &_eth_addr, sizeof(_eth_addr));
         else memset (&ifr->ifr_addr.sa_data, 0, sizeof(ifr->ifr_addr.sa_data));
         break;

    case SIOCGIFMTU:                 /* get interface MTU */
         ifr->ifr_mtu = mtu;
         break;

#if 0
    case SIOCGIFNAME:                /* get interface name */
         break;
#endif

    case SIOCSIFADDR:                /* set interface address */
         if (ifr->ifr_addr.sa_family == AF_INET)
         {
           struct sockaddr_in *sin = (struct sockaddr_in*) &ifr->ifr_addr;
           my_ip_addr = ntohl (sin->sin_addr.s_addr);
           break;
         }

         eth = (eth_address*) &ifr->ifr_addr.sa_data;  /* ?? */
         if (!_eth_set_addr(eth))
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         break;

    case OSIOCGIFDSTADDR:            /* get point-to-point address */
    case SIOCGIFDSTADDR:
         /* to-do !! */
         break;

    case SIOCSIFDSTADDR:             /* set point-to-point address */
         /* to-do !! */
         break;

    case SIOCSIFFLAGS:               /* set ifnet flags */
         /* to-do !! */
         break;

    case SIOCGIFFLAGS:               /* get ifnet flags */
         ifr->ifr_flags = 0;
         if (_eth_is_init)
         {
           ifr->ifr_flags |= (IFF_UP | IFF_RUNNING);
           if (_pktdevclass == PD_PPP ||
               _pktdevclass == PD_SLIP)
              ifr->ifr_flags |= IFF_POINTOPOINT;
#if defined(USE_MULTICAST)
           if (_multicast_on)
              ifr->ifr_flags |= IFF_MULTICAST;
#endif
         }
         break;

    case SIOCGIFBRDADDR:             /* get broadcast address */
    case OSIOCGIFBRDADDR:
      /* ifr->ifr_broadaddr.sa_family = ?? */
         memset (&ifr->ifr_broadaddr.sa_data, 0, sizeof(ifr->ifr_addr.sa_data));
         memcpy (&ifr->ifr_broadaddr.sa_data, &_eth_brdcast, sizeof(_eth_brdcast));
         get_ifname (ifr->ifr_name);
         break;

    case SIOCSIFBRDADDR:             /* set broadcast address */
         memcpy (&_eth_brdcast, &ifr->ifr_broadaddr.sa_data, sizeof(_eth_brdcast));
         break;

    case SIOCGIFMETRIC:              /* get interface meteric */
         ifr->ifr_metric = 1;
         get_ifname (ifr->ifr_name);
         break;

    case SIOCSIFMETRIC:              /* set interface meteric */
         /* to-do !! */
         break;

    case SIOCDIFADDR:                /* delete interface addr */
         if (ifr->ifr_addr.sa_family == AF_INET)
         {
           my_ip_addr = 0;
           break;
         }
         /* to-do !! */
         break;

    case SIOCAIFADDR:                /* add/change interface alias */
         /* to-do !! */
         break;

    case SIOCGIFNETMASK:             /* get interface net-mask */
    case OSIOCGIFNETMASK:
      /* ifr->ifr_addr.sa_family = ?? */
         memset (&ifr->ifr_addr.sa_data, 0, sizeof(ifr->ifr_addr.sa_data));
         *(DWORD*) &ifr->ifr_addr.sa_data = htonl (sin_mask);
         get_ifname (ifr->ifr_name);
         break;

    case SIOCSIFNETMASK:             /* set interface net-mask */
         sin_mask = ntohl (*(DWORD*)&ifr->ifr_addr.sa_data);
         break;

    case SIOCGIFCONF:                /* get interfaces config */
    case OSIOCGIFCONF:
         len = ifc->ifc_len = min (ifc->ifc_len, NUM_IFACES*sizeof(*ifr));
         ifc = (struct ifconf*) ifc->ifc_buf; /* user's buffer */
         VERIFY_RW (ifc, len);

         ifr = (struct ifreq*) ifc;
         get_ifname (ifr->ifr_name);
         sin = (struct sockaddr_in*) &ifr->ifr_addr;
         sin->sin_addr.s_addr = htonl (my_ip_addr);
         sin->sin_family      = AF_INET;
         break;

    case SIOCGIFHWADDR:
         switch (_pktdevclass)
         {
           case PD_ETHER:
                ifr->ifr_hwaddr.sa_family = ARPHRD_ETHER;
                memcpy (&ifr->ifr_hwaddr.sa_data, &_eth_addr,
                        sizeof(ifr->ifr_hwaddr.sa_data));
                break;
           case PD_TOKEN:
                ifr->ifr_hwaddr.sa_family = ARPHRD_TOKEN;
                memcpy (&ifr->ifr_hwaddr.sa_data, &_eth_addr,
                        sizeof(ifr->ifr_hwaddr.sa_data));
                break;
           case PD_SLIP:
                ifr->ifr_hwaddr.sa_family = 0;
                memset (&ifr->ifr_hwaddr.sa_data, 0,
                        sizeof(ifr->ifr_hwaddr.sa_data));
                break;
           case PD_PPP:
                ifr->ifr_hwaddr.sa_family = 0;
                memset (&ifr->ifr_hwaddr.sa_data, 0,
                        sizeof(ifr->ifr_hwaddr.sa_data));
                break;
           default:
                return (-1);
         }
         break;

    default:
         NO_CMD (cmd);
         SOCK_ERR (ESOCKTNOSUPPORT);
         return (-1);
  }
  return (0);
}


/*
 * Handler for buffer hi/lo watermark and urgent data (OOB)
 */
static int waterm_ioctrl (Socket *socket, long cmd, char *argp)
{
  VERIFY_RW (argp, sizeof(*argp));

  switch ((DWORD)cmd)
  {
    case SIOCSHIWAT:      /* set high watermark */
         /* to-do !! */
         break;

    case SIOCGHIWAT:      /* get high watermark */
         /* to-do !! */
         break;

    case SIOCSLOWAT:      /* set low watermark */
         /* to-do !! */
         break;

    case SIOCGLOWAT:      /* get low watermark */
         /* to-do !! */
         break;

    case SIOCATMARK:      /* OOB data available? */
         /* to-do !! */
         break;

    default:
         NO_CMD (cmd);
         SOCK_ERR (ESOCKTNOSUPPORT);
         return (-1);
  }
  ARGSUSED (argp);
  return (0);
}


/*
 * Handler for ARP-cache interface commands
 */
static int arp_ioctrl (Socket *socket, long cmd, char *argp)
{
  struct arpreq    *arp = (struct arpreq*) argp;
  struct arp_table *tab;
  DWORD             ip;

  switch ((DWORD)cmd)
  {
    case SIOCSARP:      /* add given IP/MAC-addr pair to ARP cache */
         ip = intel (*(DWORD*) &arp->arp_pa.sa_data);
         _arp_add_cache (ip, (eth_address*)&arp->arp_ha.sa_data, TRUE);
         break;

    case SIOCGARP:      /* return ARP entry for given ip */
    case OSIOCGARP:
         tab = _arp_search (*(DWORD*)&arp->arp_pa.sa_data, FALSE);
         if (tab)
         {
           if (tab->flags >= ARP_FLAG_FOUND)
               arp->arp_flags |= (ATF_INUSE | ATF_COM);

           if (tab->flags == ARP_FLAG_FIXED)
               arp->arp_flags |= ATF_PERM;
           memcpy (&arp->arp_ha.sa_data, &tab->hardware, sizeof(eth_address));
         }
         else
         {
           arp->arp_flags = 0;
           memset (&arp->arp_ha.sa_data, 0, sizeof(arp->arp_ha.sa_data));
           SOCK_ERR (ENOENT);  /* correct errno? */
           return (-1);
         }
         break;

    case SIOCDARP:      /* delete ARP-entry for given ip */
         tab = _arp_search (*(DWORD*)&arp->arp_pa.sa_data, FALSE);
         if (tab)
         {
           tab->ip = tab->flags = 0;
           memset (&tab->hardware, 0, sizeof(tab->hardware));
         }
         else
         {
           SOCK_ERR (ENOENT);  /* correct errno? */
           return (-1);
         }
         break;

    default:
         NO_CMD (cmd);
         SOCK_ERR (ESOCKTNOSUPPORT);
         return (-1);
  }
  return (0);
}

/*
 * Return string for ioctlsocket() command
 */
#if defined (USE_DEBUG)
struct cmd_list {
       long        cmd_val;
       const char *cmd_name;
     };

struct cmd_list commands[] = {
                { FIOCLEX,         "FIOCLEX"         },
                { FIONCLEX,        "FIONCLEX"        },
                { FIONREAD,        "FIONREAD"        },
                { FIONBIO,         "FIONBIO"         },
                { FIOASYNC,        "FIOASYNC"        },
                { FIOSETOWN,       "FIOSETOWN"       },
                { FIOGETOWN,       "FIOGETOWN"       },
                { SIOCSPGRP,       "SIOCSPGRP"       },
                { SIOCGPGRP,       "SIOCGPGRP"       },
                { SIOCADDRT,       "SIOCADDRT"       },
                { SIOCDELRT,       "SIOCDELRT"       },
                { SIOCSIFADDR,     "SIOCSIFADDR"     },
                { OSIOCGIFADDR,    "OSIOCGIFADDR"    },
                { SIOCGIFADDR,     "SIOCGIFADDR"     },
                { SIOCSIFDSTADDR,  "SIOCSIFDSTADDR"  },
                { OSIOCGIFDSTADDR, "OSIOCGIFDSTADDR" },
                { SIOCGIFDSTADDR,  "SIOCGIFDSTADDR"  },
                { SIOCSIFFLAGS,    "SIOCSIFFLAGS"    },
                { SIOCGIFFLAGS,    "SIOCGIFFLAGS"    },
                { OSIOCGIFBRDADDR, "OSIOCGIFBRDADDR" },
                { SIOCGIFBRDADDR,  "SIOCGIFBRDADDR"  },
                { SIOCSIFBRDADDR,  "SIOCSIFBRDADDR"  },
                { OSIOCGIFCONF,    "OSIOCGIFCONF"    },
                { SIOCGIFCONF,     "SIOCGIFCONF"     },
                { OSIOCGIFNETMASK, "OSIOCGIFNETMASK" },
                { SIOCGIFNETMASK,  "SIOCGIFNETMASK"  },
                { SIOCSIFNETMASK,  "SIOCSIFNETMASK"  },
                { SIOCGIFMETRIC,   "SIOCGIFMETRIC"   },
                { SIOCSIFMETRIC,   "SIOCSIFMETRIC"   },
                { SIOCDIFADDR,     "SIOCDIFADDR"     },
                { SIOCAIFADDR,     "SIOCAIFADDR"     },
                { SIOCGIFMTU,      "SIOCGIFMTU"      },
                { SIOCSARP,        "SIOCSARP"        },
                { OSIOCGARP,       "OSIOCGARP"       },
                { SIOCGARP,        "SIOCGARP"        },
                { SIOCDARP,        "SIOCDARP"        },
                { SIOCSHIWAT,      "SIOCSHIWAT"      },
                { SIOCGHIWAT,      "SIOCGHIWAT"      },
                { SIOCSLOWAT,      "SIOCSLOWAT"      },
                { SIOCGLOWAT,      "SIOCGLOWAT"      },
                { SIOCATMARK,      "SIOCATMARK"      },
                { SIOCGIFHWADDR,   "SIOCGIFHWADDR"   }
              };

static __inline const char *lookup (long cmd, const struct cmd_list *list,
                                    int num)
{
  while (num)
  {
    if (list->cmd_val == cmd)
       return (list->cmd_name);
    num--;
    list++;
  }
  return (NULL);
}

static const char *get_ioctl_cmd (long cmd)
{
  static char buf[50];
  const  char *str;

  switch (IOCGROUP(cmd))
  {
    case 'f':
         strcpy (buf, "file: ");
         break;
    case 'I':
         strcpy (buf, "iface: ");
         break;

    case 's':
         strcpy (buf, "waterm: ");
         break;

    default:
         return ("??unknown");
  }
  str = lookup (cmd, commands, DIM(commands));
  if (str)
     strcat (buf, str);
  return (buf);
}
#endif


/*
 * A small test program for above functions.
 */
#if defined(TEST_PROG)

#undef  assert
#define assert(x) ((x) ? (void)0 : assert_fail(__LINE__))

void assert_fail (unsigned line)
{
  fprintf (stderr, "\nAssert failed at line %d, errno = %d (%s)\n",
           line, errno, strerror (errno));
  exit (-1);
}

char *eth_addr_string (struct ether_addr *eth)
{
  static char buf[20];

  sprintf (buf, "%02X:%02X:%02X:%02X:%02X:%02X",
           eth->ether_addr_octet[0],
           eth->ether_addr_octet[1],
           eth->ether_addr_octet[2],
           eth->ether_addr_octet[3],
           eth->ether_addr_octet[4],
           eth->ether_addr_octet[5]);
  return (buf);
}

int main (void)
{
  struct ifreq ifr;
  struct sockaddr_in *sin;
  int    sock, on = 1;

  dbug_init();

  sock = socket (AF_INET, SOCK_DGRAM, 0);
  assert (sock);
  assert (setsockopt(sock, SOL_SOCKET, SO_DEBUG, &on,sizeof(on)) == 0);

  ifr.ifr_addr.sa_family = AF_UNSPEC;  /* get MAC-address */

  assert (ioctlsocket (sock, SIOCGIFADDR, (char*)&ifr) == 0);
  printf ("Interface `%s':\n\t ether-addr: %s\n",
          ifr.ifr_name,
          eth_addr_string ((struct ether_addr*)&ifr.ifr_addr.sa_data));

  assert (ioctlsocket (sock, SIOCGIFBRDADDR, (char*)&ifr) == 0);
  printf ("\t bcast-addr: %s\n",
          eth_addr_string ((struct ether_addr*)&ifr.ifr_addr.sa_data));

  ifr.ifr_addr.sa_family = AF_INET;

  assert (ioctlsocket (sock, SIOCGIFADDR, (char*)&ifr) == 0);
  sin = (struct sockaddr_in*) &ifr.ifr_addr;
  printf ("\t inet-addr:  %s\n", inet_ntoa (sin->sin_addr));

  assert (close_s(sock) >= 0);
  return (0);
}

#endif  /* TEST_PROG */
#endif  /* USE_BSD_FUNC */

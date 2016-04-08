/*
 *  WatTCP config file handling
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>    /* open modes */
#include <ctype.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "language.h"
#include "udp_dom.h"
#include "udp_nds.h"
#include "bsdname.h"
#include "pcqueue.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pctcp.h"
#include "pcarp.h"
#include "pcrarp.h"
#include "pcbsd.h"
#include "pcbootp.h"
#include "pcicmp.h"
#include "sock_ini.h"
#include "fragment.h"
#include "ip_out.h"
#include "profile.h"
#include "pcconfig.h"

#ifdef __BORLANDC__
#pragma warn -pro
#endif

int  debug_on          = 0;    /* tcp-state machine debug */
int  sock_delay        = 30;
int  sock_inactive     = 0;    /* defaults to forever */
int  sock_data_timeout = 120;  /* after 2 minutes we give up EE 99.08.23 */
int  multihomes        = 0;    /* We have more than 1 IP-addresses */
int  ctrace_on         = 0;    /* tracing; HighC only */
int  dynamic_host      = 0;    /* reverse resolve assigned IP to true FQDN */

DWORD cookies [MAX_COOKIES];
WORD  last_cookie;

static void dummy (const char *name, const char *value)
{
  ARGSUSED (name);
  ARGSUSED (value);
}

void (*print_hook) (const char*) = NULL;
void (*usr_init)   (const char*, const char*) = dummy;

char *_watt_config_name   = "WATTCP.CFG";  /* name of config file in */
char *_watt_environ_name  = "WATTCP.CFG";  /* path specified by this var. */
char *_watt_environ_name1 = "WATTCP_CFG";  /* path specified by this var. */

/*
 * Parse string "ether-addr, ip-addr"
 *
 * inet_atoeth - read src, dump to ethernet buffer
 *               and return pointer to "ip-addr"
 */
const char *inet_atoeth (const char *src, BYTE *eth)
{
  eth[0] = atox (src-2);      /*   xx:xx:xx:xx:xx:xx */
  eth[1] = atox (src+1);      /* ^src-2              */
  eth[2] = atox (src+4);      /*    ^src+1           */
  eth[3] = atox (src+7);
  eth[4] = atox (src+10);
  eth[5] = atox (src+13);
  src += strlen ("xx:xx:xx:xx:xx:xx");
  if (*src == ',')
     ++src;

  while (*src == ' ' || *src == '\t')
        src++;

  return (src);
}

static void EtherAddr (const char *value, int set_it)
{
  eth_address eth;
  const char *ip     = inet_atoeth (value, (BYTE*)&eth);
  BOOL        is_own = memcmp (&eth, &_eth_addr, sizeof(eth)) == 0;

  if (!isdigit(*ip) && !set_it)
     return;

  if (isdigit(*ip) && !set_it && is_own)
  {
    my_ip_addr = _inet_addr (ip);
    return;
  }

  if (set_it && !is_own)
  {
    if (!_eth_set_addr(&eth))
       outsnl (_LANG("Cannot set Ether-addr"));
#if defined(USE_RARP)
    else
    {
      WORD  save_to = _rarptimeout;
      DWORD save_ip = my_ip_addr;
      _rarptimeout = 2;           /* use only 2 sec timeout */
      my_ip_addr   = 0;
      if (!_dorarp())
         outsnl (_LANG("Warning: Ether-addr already in use"));
      my_ip_addr   = save_ip;
      _rarptimeout = save_to;
    }
#endif
  }
  else
  {
    DWORD addr = _inet_addr (ip);
    if (addr)
      _arp_add_cache (addr, &eth, FALSE); /* add ip-addr to arp-table */
  }
}

void _add_server (WORD *counter, WORD max, DWORD *array, DWORD value)
{
  int i, duplicate = 0;

  if (value && *counter < max)
  {
    for (i = 0; i < *counter; i++)
        if (array[i] == value)
           duplicate = 1;

    if (!duplicate)
       array [(*counter)++] = value;
  }
}

/*
 * Return a string with a environment variable expanded (only 1).
 *
 * E.g. if environment variable ETC is "c:\network\watt\bin",
 * "$(ETC)\hosts" becomes "c:\network\watt\bin\hosts"
 */
static const char *ExpandVarStr (const char *str)
{
  static char buf[MAX_PATHLEN];
  char   env[30];
  const  char *e, *p  = strstr (str, "$(");
  const  char *dollar = p;
  int    i;

  if (!p)
     return (str);

  for (i = 0, p += 2; i < sizeof(env)-1; i++)
  {
    if (*p == ')')
       break;
    env[i] = *p++;
  }

  if (*p++ != ')')
     return (str);

  env[i] = 0;
  strupr (env);
  e = getenv (env);
  if (!e)
  {
    e = env;
    env[0] = 0;
  }

  i = dollar - str;
  strncpy (buf, str, i);
  buf[i] = 0;
  strncat (buf, e, sizeof(buf)-1-i);
  return strcat (buf, p);
}

/*
 * Convert hexstring "0x??" to hex
 */
BYTE atox (const char *value)
{
  unsigned hi = toupper (value[2]);
  unsigned lo = toupper (value[3]);

  hi -= isdigit (hi) ? '0' : 'A'-10;
  lo -= isdigit (lo) ? '0' : 'A'-10;
  return ((hi << 4) + lo);
}

/*
 * Parse the config-table and if match is found for
 * ('section'+'.'+)'name' either store variable to 'value' or
 * call function with 'value'.
 */
int parse_config_table (const struct config_table *tab,
                        const char *section,
                        const char *name,
                        const char *value)
{
  for ( ; tab && tab->keyword; tab++)
  {
    char keyword[MAX_STRING];

    if (section)
    {
      if (strlen(section) + strlen(tab->keyword) >= sizeof(keyword))
         continue;
      strcpy (keyword, section);
      strcat (keyword, tab->keyword);
    }
    else
    {
      strncpy (keyword, tab->keyword, sizeof(keyword)-1);
      keyword [sizeof(keyword)-1] = '\0';
    }

    if (!strcmp(name,keyword))
    {
      void *arg = tab->arg_func;
      char *p;

      if (!arg)     /* no storage for variable, okay */
         return (1);

      switch (tab->type)
      {
        case ARG_ATOI:
             *(int*)arg = atoi (value);
             break;

        case ARG_ATOB:
             *(BYTE*)arg = atoi (value);
             break;

        case ARG_ATOW:
             *(WORD*)arg = atoi (value);
             break;

        case ARG_ATON:
             *(DWORD*)arg = aton (value);
             break;

        case ARG_ATOX_B:
             *(BYTE*)arg = atox (value);
             break;

        case ARG_ATOX_W:
             *(WORD*)arg = (atox (value) << 8) + atox (value+2);
             break;

        case ARG_FUNC:
             (*(void(*)())arg) (value, strlen(value));
             break;

        case ARG_RESOLVE:
             {
               DWORD host = resolve (value);
               if (!host)
               {
                 outs (_LANG("Cannot resolve \""));
                 outs (value);
                 outsnl ("\"\n");
               }
               else
                 *(DWORD*)arg = host;
             }
             break;

        case ARG_STRDUP:
             p = strdup (value);
             if (!p)
             {
               outs (_LANG("No memory for \""));
               outs (name);
               outsnl ("\"\n");
             }
             else
               *(char**)arg = p;
             break;

#ifdef USE_DEBUG
        default:
             fprintf (stderr, "Something wrong in parse_config_table().\n"
                      "Section %s; %s = %s\n", section, name, value);
             exit (-1);
#endif
      }

#if 0  /* test */
      fprintf (stderr, "ARG_%s, matched `%s' = `%s'\n",
               tab->type == ARG_ATOI   ? "ATOI  "  :
               tab->type == ARG_ATON   ? "ATON  "  :
               tab->type == ARG_ATOX_B ? "ATOX_B"  :
               tab->type == ARG_ATOX_W ? "ATOX_W"  :
               tab->type == ARG_STRDUP ? "STRDUP"  :
               tab->type == ARG_RESOLVE? "RESOLVE" :
               tab->type == ARG_FUNC   ? "FUNC  "  :
               "??", keyword, value);
#endif
      return (1);
    }
  }
  return (0);
}

static void set_my_ip (const char *value)
{
  if (!stricmp(value,"bootp"))
       _bootpon = 1;
  else if (!stricmp(value,"dhcp"))
       _dhcpon = 1;
  else if (!stricmp(value,"rarp"))
       _rarpon = 1;
  else my_ip_addr = resolve (value);
}

static void set_hostname (const char *value)
{
  strncpy (hostname, value, sizeof(hostname)-1);
  hostname [sizeof(hostname)-1] = '\0';
}

static void set_gateway (const char *value)
{
  _arp_add_gateway (value, 0L);  /* accept gateip[,subnet[,mask]] */
}

static void set_nameserv (const char *value)
{
  _add_server (&last_nameserver, MAX_NAMESERVERS,
               def_nameservers, resolve(value));
}

static void set_cookie (const char *value)
{
  _add_server (&last_cookie, MAX_COOKIES, cookies, resolve(value));
}

static void set_eaddr (const char *value)
{
  EtherAddr (value, 1);
}

static void set_ethip (const char *value)
{
  EtherAddr (value, 0);
}

static void set_domain (const char *value)
{
  setdomainname (value, sizeof(defaultdomain)-1);
}

static void do_print (const char *str)
{
  if (print_hook)
       (*print_hook) (str);
  else outsnl (str);
}

static void do_include_file (const char *value, int len)
{
  const char *p = value;
  int   f;

  if (*p == '?' && len > 1)
     ++p;
  f = open (p, O_RDONLY | O_TEXT);
  if (f > -1)
  {
    close (f);

    /* Recursion, but we're reentrant.
     * !!Fix-me: recursion depth should be limited to prevent stack overflow
     */
    tcp_config (p);
  }
  else if (*value != '?')
  {
    outs (_LANG("\r\nUnable to open "));
    outsnl (p);
  }
}

static void do_profile (const char *value)
{
#if defined(__DJGPP__) || defined(__WATCOM386__)
  if (*value == '1')
     profile_init();
#else
  ARGSUSED (value);
#endif
}

/*
 * Our table of Wattcp "core" values. Other modules have their
 * own tables which are hooked into the chain via `usr_init'.
 * If `name' (left column below) isn't found in below table,
 * `usr_init' is called to pass on `name' and `value' to another
 * module or application.
 */
static const struct config_table normal_cfg[] = {
       { "MY_IP",         ARG_FUNC,   (void*)set_my_ip          },
       { "HOSTNAME",      ARG_FUNC,   (void*)set_hostname       },
       { "NETMASK",       ARG_ATON,   (void*)&sin_mask          },
       { "GATEWAY",       ARG_FUNC,   (void*)set_gateway        },
       { "NAMESERVER",    ARG_FUNC,   (void*)set_nameserv       },
       { "COOKIE",        ARG_FUNC,   (void*)set_cookie         },
       { "EADDR",         ARG_FUNC,   (void*)set_eaddr          },
       { "ETHIP",         ARG_FUNC,   (void*)set_ethip          },
       { "DEBUG",         ARG_ATOI,   (void*)&debug_on          },
       { "DOMAINSLIST",   ARG_FUNC,   (void*)set_domain         },
       { "DOMAIN_LIST",   ARG_FUNC,   (void*)set_domain         },
       { "BOOTP",         ARG_RESOLVE,(void*)&_bootphost        },
       { "BOOTPTO",       ARG_ATOI,   (void*)&_bootptimeout     },
       { "BOOTP_TO",      ARG_ATOI,   (void*)&_bootptimeout     },
       { "SOCKDELAY",     ARG_ATOI,   (void*)&sock_delay        },
       { "MSS",           ARG_ATOI,   (void*)&mss               },
       { "MTU",           ARG_ATOI,   (void*)&mtu               },
       { "MTU_DISC",      ARG_ATOI,   (void*)&mtu_discover      },
       { "DOMAINTO",      ARG_ATOI,   (void*)&dns_timeout       },
       { "DOMAIN_TO",     ARG_ATOI,   (void*)&dns_timeout       },
       { "DOMAIN_RECURSE",ARG_ATOI,   (void*)&dns_recurse       },
       { "MULTIHOMES",    ARG_ATOI,   (void*)&multihomes        },
       { "ARP_TO",        ARG_ATOI,   (void*)&arp_timeout       },
       { "ARP_ALIVE",     ARG_ATOI,   (void*)&arp_alive         },
       { "ICMP_MASK_REQ", ARG_ATOI,   (void*)&_domask_req       },
       { "TXRETRIES",     ARG_ATOB,   (void*)&_pktretries       },
       { "DYNAMIC_HOST",  ARG_ATOI,   (void*)&dynamic_host      },
       { "REDIRECTS",     ARG_FUNC,   (void*)icmp_redirect      },
       { "PRINT",         ARG_FUNC,   (void*)do_print           },
       { "INCLUDE",       ARG_FUNC,   (void*)do_include_file    },
       { "PROFILE",       ARG_FUNC,   (void*)do_profile         },

#if defined (USE_LANGUAGE)
       { "LANGUAGE",      ARG_FUNC,   (void*)lang_init          },
#endif
#if defined (USE_ETHERS)
       { "ETHERS",        ARG_FUNC,   (void*)InitEthersFile     },
#endif
#if defined (USE_BSD_FUNC)
       { "HOSTS",         ARG_FUNC,   (void*)ReadHostFile       },
       { "SERVICES",      ARG_FUNC,   (void*)ReadServFile       },
       { "PROTOCOLS",     ARG_FUNC,   (void*)ReadProtoFile      },
       { "NETWORKS",      ARG_FUNC,   (void*)ReadNetworksFile   },
       { "KEEPALIVE",     ARG_ATOI,   (void*)&tcp_keepalive     },
       { "NETDB_ALIVE",   ARG_ATOI,   (void*)&netdbCacheLife    },
       { "IP.DEF_TTL",    ARG_ATOI,   (void*)&_default_ttl      },
       { "IP.DEF_TOS",    ARG_ATOI,   (void*)&_default_tos      },
       { "IP.ID_INCR",    ARG_ATOI,   (void*)&_ip_id_increment  },
#endif
#if defined (USE_FRAGMENT)
       { "IP.FRAG_REASM", ARG_ATOI,   (void*)&_ip_frag_reasm    },
#endif
       { NULL, 0, NULL }
     };

/*
 * Used when DEBUG=x is defined in the config file
 * Moved from pctcp.c
 */
void tcp_set_debug_state (WORD x)
{
  debug_on = x;
}


/*
 * Using buffered I/O speeds up reading config-file, but uses more data/code.
 * Non-DOSX targets where memory is tight doesn't "#define USE_BUFFERED_IO"
 * by default (see config.h)
 */
#if defined (USE_BUFFERED_IO)
  typedef FILE*       WFILE;
  #define FOPEN(f,n)  ( f = fopen (n, "rt"), (f != NULL) )
  #define FREAD(p,f)  fread ((char*)(p), 1, 1, f)
  #define FCLOSE(f)   fclose(f)
#else
  typedef int         WFILE;
  #define FOPEN(f,n)  ( f = open (n, O_RDONLY | O_TEXT), (f != -1) )
  #define FREAD(p,f)  read (f, (char*)(p), 1)
  #define FCLOSE(f)   close(f)
#endif

#define MAX_NAMELEN   MAX_PATHLEN
#define MAX_VALUELEN  MAX_PATHLEN

int tcp_config (const char *path)
{
  char  name [MAX_NAMELEN], *temp;
  char  value[MAX_VALUELEN], ch[2];
  int   quotemode, mode, num;
  WFILE f;

  if (!path)
  {
#if (DOSX & DJGPP)     /* 2.02 bug; _osmajor/_osminor not set in crt0.o */
    (void) _get_dos_version (0);
#endif

    /* First try to open 'WATTCP.CFG' in the directory specified
     * by $(WATTCP.CFG) or $(WATTCP_CFG).
     */
    path = getenv (_watt_environ_name);
    if (path)
    {
      path = strcpy (name, path);
      strcat (name, "\\");
    }
    else
      path = getenv (_watt_environ_name1);
    if (path)
    {
      path = strcpy (name, path);
      strcat (name, "\\");
    }
    else if (_osmajor >= 3) /* env-var not found, get path from argv[0] */
    {
      int slash = '\\';

#if defined (__HIGHC__)
      CONFIG_INF cnf;
      char      *arg0;
      extern char *GetArg0 (USHORT sel);     /* except.lib */

      _dx_config_inf (&cnf, (UCHAR*)&cnf);
      arg0 = GetArg0 (cnf.c_env_sel);
      if (arg0)
           strncpy (name, arg0, sizeof(name)-1);
      else name[0] = 0;

#elif defined (__DJGPP__)
      extern char **__crt0_argv;
      if (!__crt0_argv || !__crt0_argv[0])
           name[0] = 0;
      else strncpy (name, __crt0_argv[0], sizeof(name)-1);
      slash = '/';

#elif defined (_MSC_VER)
      extern char **__argv;
      strncpy (name, __argv[0], sizeof(name)-1);
#else
      extern char **_argv;   /* Borland, Watcom */
      strncpy (name, _argv[0], sizeof(name)-1);
#endif

      name [sizeof(name)-1] = '\0';  /* in case strncpy() barfed */

      /* If argv[0] == "x:", extract path.
       * temp -> last slash in path.
       */
      path = (*name && name[1] == ':') ? name+2 : name;
      temp = strrchr (path, slash);
      if (temp == NULL)
          temp = (char*)path;
      *(++temp) = 0;         /* 'name' = path of our program ("x:\path\") */
    }
    strcat (name, _watt_config_name);    /* 'name' = "x:\path\wattcp.cfg" */
  }
  else
    strcpy (name, path);


  if (!FOPEN(f,name))
  {
    /* Try to open 'WATTCP.CFG' in current directory
     */
    if (!FOPEN(f,_watt_config_name))
    {
      outs (_watt_config_name);
      outsnl (_LANG(" not found"));
      return (0);
    }
  }

  *name = *value = mode = num = 0;
  quotemode = ch[1] = 0;

  while (FREAD(&ch,f) == 1)
  {
    switch (ch[0])
    {
      case '\"': quotemode ^= 1;
                 break;
      case ' ' :
      case '\t': if (quotemode)
                    goto addit;
                 break;

      case '=' : if (quotemode)
                    goto addit;
                 if (!mode)
                    mode = 1;
                 break;
      case '#' :
      case ';' : if (quotemode)
                    goto addit;
                 mode = 2;   /* comment */
                 break;
      case '\n':
      case '\r': if (*name && *value)
                 {
                   const char *var = ExpandVarStr (value);

                   if (var != value)
                      strcpy (value, var);
                   (void) strupr (name);
                   if (!parse_config_table(&normal_cfg[0], NULL,
                                           name, value) && usr_init)
                     (*usr_init) (name, value);
                 }
                 *name = *value = quotemode = 0;
                 mode = num = 0;
                 break;
      default  :
  addit:         switch (mode)
                 {
                   case 0: if (num < sizeof(name))
                           {
                             strcat (name, ch);
                             num++;
                           }
                           break;
                   case 1: if (num < sizeof(value))
                           {
                             strcat (value, ch);
                             num++;
                           }
                           break;
                 }
                 break;
    }
  }
  FCLOSE (f);
  return (1);
}

/*
 * Called from most <netdb.h> functions incase sock_init() wasn't
 * called first.
 */
int netdb_init (void)
{
  int rc, save = _watt_do_exit;

  _watt_do_exit = 0;    /* don't make sock_init() call exit() */
  rc = sock_init();
  _watt_do_exit = save;
  return (rc == 0);     /* return TRUE on zero */
}


/*
 * Return TRUE if ip address is a local address or on the
 * loopback network (127.x.x.x). `ip' is on host order.
 */
int is_local_addr (DWORD ip)
{
  return ((DWORD)(ip - my_ip_addr) <= multihomes || (ip >> 24) == 127);
    /* (my_ip_addr <= ip <= my_ip_addr+multihomes) or 127.x.x.x */
}

/*
 * Return TRUE if ip address is a legal address for
 * a unique host. `ip' is on host order.
 */
int is_unique_addr (DWORD ip)
{
  return (ip && (ip & ~sin_mask) == 0);
}

/*
 * return TRUE if ip packet is a (directed) broadcast packet.
 */
int is_ip_brdcast (const in_Header *ip)
{
  DWORD dst = intel (ip->destination);
  return ((~dst & ~sin_mask) == 0); /* (directed) ip-broadcast */
}

/*
 * is_multicast - determines if the given IP addr is Class D (Multicast)
 *
 * int is_multicast (DWORD ip)
 * Where:
 *    `ip' is on host order.
 * Returns:
 *    1   if ip is Class D
 *    0   if ip is not Class D
 *
 * Note: class-D is 224.0.0.0 - 239.255.255.255, but
 *       range      224.0.0.0 - 224.0.0.255 is reserved for mcast
 *                  routing information.
 */
int is_multicast (DWORD ip)
{
  if ((ip & 0xF0000000UL) == CLASS_D_MASK)  /* or IN_CLASSD(ip) */
     return (1);
  return (0);
}


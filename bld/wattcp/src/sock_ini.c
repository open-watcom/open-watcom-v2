/*
 * sock_init - easy way to guarentee:
 *    - card is ready
 *    - shutdown is handled
 *    - cbreaks are handled
 *    - config file is read
 *    - bootp/pcdhcp/rarp is run
 *
 * 0.1 : May 2, 1991  Erick - reorganized operations
 * 0.2 : 1998 Gisle V. - Major rewrite; added DHCP, additional
 *       startup checks for various targets. Exception handler
 *       releases PKTDRVR.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <io.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "crit.h"
#include "misc.h"
#include "pcbootp.h"
#include "pcrarp.h"
#include "pcdhcp.h"
#include "pcconfig.h"
#include "pc_cbrk.h"
#include "pcmulti.h"
#include "pcicmp.h"
#include "pctcp.h"
#include "pcarp.h"
#include "pcsed.h"
#include "pcqueue.h"
#include "pcpkt.h"
#include "pcdbug.h"
#include "pcbsd.h"
#include "syslog2.h"
#include "udp_dom.h"
#include "bsdname.h"
#include "fragment.h"
#include "sock_ini.h"

#if defined(USE_TFTP)
#include "tftp.h"
#endif

#if defined(USE_BSD_FUNC)
#include "socket.h"
#endif

#if defined(USE_BIND)
#include "resolver.h"
#endif

#if defined(USE_ECHO_DISC)
#include "echo.h"
#endif

#if defined(USE_PPPOE)
#include "pppoe.h"
#endif

#if (DOSX & PHARLAP)
#include <mw/exc.h>
#endif

int _bootpon        = 0;    /* Try booting using BOOTP ? */
int _dhcpon         = 0;    /* Try booting using DHCP ? */
int _rarpon         = 0;    /* Try booting using RARP ? */
int _domask_req     = 0;    /* do an "ICMP Mask Request" when configured */
int _watt_do_exit   = 1;    /* exit program when all boot attempts failed */
int _watt_no_config = 0;    /* run with no config file (embedded/diskless) */

#if defined(USE_DHCP)       /* Survive a failed BOOTP/DHCP/RARP attempt ? */
  int survivebootp = 1;
#else
  int survivebootp = 0;
#endif

#if defined(USE_RARP)
  int survivedhcp = 1;
#else
  int survivedhcp = 0;
#endif

int surviverarp = 0;

void (*_watt_post_hook) (void);

int   _watt_is_init     = 0;
int   _watt_fatal_error = 0;
DWORD _watt_start_time  = 0;
DWORD _watt_start_day   = 0;
int   _tcp_is_init      = 0;

static int old_brk = -1;

static int do_exit (int code)
{
  if (_watt_do_exit)
     exit (code);
  return (code);
}

/*
 *  Exit routine for Fortify. Report memory usage/statistics.
 *  Also requires USE_DEBUG.
 */
#if defined(USE_FORTIFY)
static void ExitFortify (void)
{
  Fortify_ListAllMemory();
  Fortify_OutputStatistics();
}
#endif


/*
 *  Some target dependent functions. Check stack-limits and
 *  install signal handlers.
 */

#if defined(USE_FRAGMENTS)
  #define NEEDED_STK  210000UL  /* pcdbug.c needs lots of stack */
#else
  #define NEEDED_STK  32000UL
#endif

#if defined (__HIGHC__)

static void CheckStackLimit (void)
{
  extern DWORD _mwstack_limit[];
  DWORD  stk;

  if ((DWORD)&stk < NEEDED_STK ||  /* subtract could wrap around */
      (DWORD)&stk - _mwstack_limit[1] < NEEDED_STK)
  {
    fprintf (stderr,
             "Stack size (%lu) is too small, %lu bytes needed.\r\n"
             "Recompile with linker-arg `-stack %lu'\r\n",
             (DWORD)&stk - _mwstack_limit[1], NEEDED_STK, NEEDED_STK);
    exit (-1);
  }
}
#endif  /* __HIGHC__ */


#if defined(USE_EXCHANDLER) && (defined(__BORLANDC__) || defined(__WATCOMC__))

static void (*old_sigfpe)(int);
static void (*old_sigsegv)(int);

/*
 * Note: Watcom's extension to SIGFPE is undocumented
 */
void WattExcHandler (int sig, int code)
{
#if defined(__WATCOMC__)
  if (sig == SIGFPE && code == FPE_IOVERFLOW)
  {
    _fpreset();
    outsnl (_LANG("ignoring SIGFPE (FPE_IOVERFLOW)"));
    return;
  }
#endif

  _watt_fatal_error = 1;
  _eth_release();

#if defined(USE_DEBUG)
  dbug_exit();
#if defined(USE_BSD_FUNC)
  _sock_dbug_exit();
#endif
#endif

  if (sig == SIGFPE)
  {
#if defined(__WATCOMC__)
    outs (_LANG("Trapping SIGFPE code 0x"));
    outhex (code);
#else
    outsnl (_LANG("Trapping SIGFPE."));
    ARGSUSED (code);
#endif
    if (old_sigfpe && old_sigfpe != SIG_IGN)
      (*old_sigfpe) (SIGFPE);
  }
  else if (sig == SIGSEGV)
  {
    outsnl (_LANG("Trapping SIGSEGV"));
    if (old_sigsegv && old_sigsegv != SIG_IGN)
      (*old_sigsegv) (SIGSEGV);
  }
  fflush (stdout);
  fflush (stderr);
  exit (-1);
}
#endif  /* USE_EXCHANDLER && (__BORLANDC__ || __WATCOMC__) */


#if defined (__WATCOM386__)
static void CheckStackLimit (void)
{
  if (stackavail() < NEEDED_STK)
  {
    fprintf (stderr,
             "Stack size (%lu) is too small, %lu bytes needed.\r\n"
             "Relink your application with \"option stack=%lu\"\r\n",
             stackavail(), NEEDED_STK, NEEDED_STK);
    exit (-1);
  }
}
#endif


#if defined (__BORLAND386__)  /* using WDOSX/PowerPak */
static void CheckStackLimit (void)
{
  UNFINISHED();  /* to-do !! */
}
#endif


#if defined (__DJGPP__)
#include <sys/exceptn.h>
#include <stubinfo.h>
#include <float.h>

static void CheckStackLimit (void)
{
  if (_stubinfo->minstack < NEEDED_STK)
  {
    fprintf (stderr,
             "Stack size (%lu) is too small, %lu bytes needed.\r\n"
             "Use `STUBEDIT' or `_stklen' to modify\r\n",
             _stubinfo->minstack, NEEDED_STK);
    exit (-1);
  }
}

#if defined(USE_EXCHANDLER)
void WattExcHandler (int sig)
{
  static int been_here = 0;
  static jmp_buf exc_buf;

  _watt_fatal_error = 1;

  if (been_here)
  {
    been_here = 0;
    signal (sig, SIG_DFL);
    __djgpp_exception_state_ptr = &exc_buf;
  }
  else
  {      /* save `*jmp_buf' in case of reentry */
    memcpy (&exc_buf, __djgpp_exception_state_ptr, sizeof(exc_buf));
    been_here = 1;
    psignal (sig, "TCP/IP shutdown");

    _eth_release();  /* don't do tcp_shutdown(), socket-list */
                     /* and data is most certainly damaged.. */

#if defined(USE_DEBUG)
    dbug_exit();
#if defined(USE_BSD_FUNC)
    _sock_dbug_exit();
#endif
#endif
  }

  raise (SIGABRT); /* this doesn't call `atexit()' functions */
}
#endif  /* USE_EXCHANDLER */
#endif  /* __DJGPP__ */


/*
 * Abort all tcp's and shut down the network driver
 */
static void tcp_shutdown (void)
{
  if (!_tcp_is_init)
     return;

  if (!_watt_fatal_error)
  {
#if !defined(USE_UDP_ONLY)
    while (_tcp_allsocs)
        tcp_abort (_tcp_allsocs);
#endif

#if defined(USE_DHCP)
    DHCP_release(); /* be nice and release our leased address */
#endif

#if defined(USE_PPPOE)
    pppoe_exit();
#endif

#if defined(USE_DEBUG)
    dbug_exit();
#endif
  }
  _eth_release();   /* release packet-driver */
  _tcp_is_init = 0;
}


#if !defined(USE_UDP_ONLY)
/*
 * tcp_parse_config - Parser for wattcp.cfg values related to TCP.
 */
static void (*prev_hook) (const char*, const char*) = NULL;

static struct config_table tcp_cfg[] = {
  { "NAGLE",               ARG_ATOI, (void*)&tcp_nagle         },
  { "DATATIMEOUT",         ARG_ATOI, (void*)&sock_data_timeout }, /* EE Aug-99 */
  { "INACTIVE",            ARG_ATOI, (void*)&sock_inactive     },
  { "TCP.OPT.TS",          ARG_ATOI, (void*)&tcp_opt_timstmp   },
  { "TCP.OPT.SACK",        ARG_ATOI, (void*)&tcp_opt_sackok    },
  { "TCP.OPT.WSCALE",      ARG_ATOI, (void*)&tcp_opt_wscale    },
  { "TCP.TIMER.OPEN_TO",   ARG_ATOI, (void*)&tcp_OPEN_TO       },
  { "TCP.TIMER.CLOSE_TO",  ARG_ATOI, (void*)&tcp_CLOSE_TO      },
  { "TCP.TIMER.RTO_ADD",   ARG_ATOI, (void*)&tcp_RTO_ADD       },
  { "TCP.TIMER.RTO_BASE",  ARG_ATOI, (void*)&tcp_RTO_BASE      },
  { "TCP.TIMER.RESET_TO",  ARG_ATOI, (void*)&tcp_RST_TIME      },
  { "TCP.TIMER.RETRAN_TO", ARG_ATOI, (void*)&tcp_RETRAN_TIME   },
  { NULL }
};

static void tcp_parse_config (const char *name, const char *value)
{
  if (!parse_config_table(&tcp_cfg[0], NULL, name, value) && prev_hook)
     (*prev_hook) (name, value);
}
#endif


/*
 * tcp_init - Initialise the tcp implementation
 *          - may be called more than once without hurting
 */
static int tcp_init (void)
{
  int rc = -1;

  if (!_tcp_is_init)
  {
    _tcp_is_init = 1;
    rc = _eth_init();          /* initialize ethernet interface */
    arp_last_gateway = 0;      /* reset # of gateways           */
    last_nameserver  = 0;      /* reset the nameserver table    */
    last_cookie = 0;           /* eat all remaining crumbs      */
    init_localport();          /* clear local ports in-use      */

    /* Try asking a DOS network extension for a host-name.
     */
    _get_machine_name (hostname, sizeof(hostname));

#if !defined(USE_UDP_ONLY)
    prev_hook = usr_init;
    usr_init  = tcp_parse_config;
#endif
  }
  return (rc);
}

/*
 * tcp_post_init - Initialise stuff based on configured values
 */
static void tcp_post_init (void)
{
  if (!_tcp_is_init)      /* tcp_init() not called */
     return;

  if (_watt_post_hook)    /* tell hook we're done */
    (*_watt_post_hook)();

  if (sin_mask == 0)
     outsnl ("\"sin_mask\"is 0.0.0.0 !\7");

#if defined(USE_BSD_FUNC) && defined(USE_DEBUG)
  if (_dbugxmit)          /* if dbug_init() called   */
     _sock_dbug_on();     /*  start BSD-socket debug */
#endif

#if defined (USE_ETHERS)
  ReadEthersFile(); /* requires gethostbyname(), hence called here */
#endif

  if (pkt_get_mtu() > 0)
     mtu = pkt_get_mtu(); /* driver knows correct MTU */

#define MSS_MAX (ETH_MAX_DATA - sizeof(tcp_Header) - sizeof(in_Header))
  if (mss > MSS_MAX)
      mss = MSS_MAX;
  if (mtu > ETH_MAX_DATA)
      mtu = ETH_MAX_DATA;
}


/*
 * This is the main initialisation routine.
 * Called only once (during program startup).
 */
int sock_init (void)
{
  static int been_here = 0;
  static int rc = 0;

  if (been_here)     /* return previous result */
     return (rc);

  been_here = 1;

#if defined(USE_FORTIFY)
  Fortify_EnterScope();
#if defined(USE_DEBUG)
  Fortify_SetOutputFunc (dbug_write_raw);
#endif
  atexit (ExitFortify);
#endif /* USE_FORTIFY */

#if (DOSX)
  CheckStackLimit();
#endif

#if defined(USE_EXCHANDLER)
  if (!getenv("WATT32-NOEXC"))
  {
#if (DOSX & PHARLAP)
    InstallExcHandler ((excHook)_eth_release);

#elif defined(__DJGPP__)
    signal (SIGSEGV, WattExcHandler);
    signal (SIGTRAP, WattExcHandler);
    signal (SIGFPE, WattExcHandler);
    signal (SIGILL, WattExcHandler);

#elif defined(__BORLANDC__) || defined(__WATCOMC__)
    /* Watcom v11.0 doesn't support SIGSEGV, but might do sometime...
     */
    old_sigsegv = signal (SIGSEGV,(void(*)(int))WattExcHandler);
    old_sigfpe  = signal (SIGFPE, (void(*)(int))WattExcHandler);

    /* If signal handler for SIGFPE is installed, then for some
     * unknown reason SIGFPE is raised in the BSD-socket API
     * (in select_s ??)
     */
#endif
  }
#endif /* USE_EXCHANDLER */


  /* DOSX: Set DOS far-ptr, get CPU type, use BSWAP instruction
   *       on 486+CPUs, setup DOS transfer buffer.
   */
  init_misc();

  /* Critical error handler, int24_isr() causes crashes under WinNT,
   * but djgpp's C-lib automatically fails critical I/O faults, so
   * it's not needed. Not implemented for bcc32.
   */
#if !defined(__DJGPP__) && !defined(__BORLAND386__)
  int24_init();
#endif

  /* Init PKTDRVR, get ether-addr, set config-hook for
   * parsing TCP-values
   */
  rc = tcp_init();
  if (rc)
     return do_exit (rc); /* failed */

  _watt_is_init    = 1;
  _watt_start_time = set_timeout (0);  /* used in nettime.c */
  _watt_start_day  = time (NULL) / (3600*24);

  /* Prepare to parse wattcp.cfg config options related
   * to DHCP, SYSLOG, BIND and TFTP
   */
#if defined(USE_DHCP)
  DHCP_init();
#endif

#if defined(USE_BSD_FUNC)
  syslog_init();
#endif

#if defined (USE_BIND)
  res_init0();
#endif

#if defined(USE_TFTP)
  tftp_init();
#endif

#if defined(USE_ECHO_DISC)
  echo_discard_init();
#endif

#if defined(USE_PPPOE)
  pppoe_init();
#endif

  atexit (sock_exit);
  old_brk = tcp_cbreak (0x10);

  if (_watt_no_config)
  {
    if (!my_ip_addr)
    {
      _bootpon = 1;   /* if no fixed IP, try BOOTP/DHCP/RARP */
      _dhcpon  = 1;
      _rarpon  = 1;
    }
    else if (debug_on)
      outsnl (_LANG("Fixed IP configuration."));
  }

  else if (!tcp_config(NULL))
  {
    _bootpon = 1;    /* if no config file try BOOTP/DHCP/RARP */
    _dhcpon  = 1;
    _rarpon  = 1;
  }

  tcp_post_init();   /* test and fix configured values */

#if defined(USE_BOOTP)
  if (_bootpon && !_dobootp())
  {
    outsnl (_LANG("failed"));
    _dhcpon = 1;
    if (!survivebootp)
       return do_exit (WERR_BOOTP_FAIL);
  }
#endif

#if defined(USE_DHCP)
  if (_dhcpon && !_dodhcp())
  {
    outsnl (_LANG("failed"));
    _rarpon = 1;
    if (!survivedhcp)
       return do_exit (WERR_DHCP_FAIL);
    _dodhcp_inform();
  }
#endif

#if defined(USE_RARP)
  if (_rarpon && !_dorarp())
  {
    outsnl (_LANG("failed"));
    if (!_surviverarp)
       return do_exit (WERR_RARP_FAIL);
  }
#endif

#if defined(USE_PPPOE)
  if (!pppoe_discovery())
  {
    outsnl (_LANG("Timeout waiting for PADS/PADO"));
    return do_exit (WERR_PPPOE_DISC);
  }
#endif

  if (!my_ip_addr)   /* all boot attempts failed */
  {
    outsnl (_LANG("Cannot set local ip-address"));
    return do_exit (WERR_NO_IPADDR);
  }

  if (dynamic_host && !reverse_lookup_myip())
  {
    outsnl (_LANG("Cannot reverse resolve local ip-address. "
                  "Set \"DYNAMIC_HOST\" = 0."));
    return do_exit (WERR_IP_RRESOLVE);
  }

#if defined(USE_MULTICAST)
  /* all multicast level 2 systems must join at startup
   */
  if (_multicast_on)
     join_mcast_group (ALL_SYSTEMS);
#endif

#if defined(USE_FRAGMENTS)
  /* Add a daemon to ceck for IP-fragment that have timed out.
   */
  addwattcpd (chk_timeout_frags);
#endif

  /* if set in wattcp.cfg send an ICMP_MASK_REQ
   */
  if (_domask_req)
     icmp_mask_req();

#if defined(USE_TFTP)
  /* If application supplied a writer hook, try to load
   * the specified BOOT-file.
   */
  if (tftp_writer)
     tftp_boot_load();
#endif

#if defined(USE_ECHO_DISC)
  echo_discard_start();       /* start echo/discard services */
#endif

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();    /* list memory we've used so far */
#endif

  return (0);
}

/*
 * This is the main library exit routine.
 * Normally called only once (during program shutdown).
 */
void sock_exit (void)
{
  if (_watt_is_init)
  {
    _watt_is_init = 0;
    tcp_shutdown();

    if (old_brk >= 0)
       set_cbreak (old_brk);
  }
}

/*
 * Make sure user links the correct 32-bit C-libs for Watcom
 */
#if defined(__WATCOM386__)
  #if defined(__SW_3R)
    #pragma library ("clib3r.lib");
    #pragma library ("math387r.lib");

  #else /* __SW_3S */
    #pragma library ("clib3s.lib");
    #pragma library ("math387s.lib");
  #endif
#endif


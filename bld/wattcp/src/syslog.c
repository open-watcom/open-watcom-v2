/*
 *  Simple syslog handler for Watt-32 & DOS.
 *
 *  Loosely based on BSD-version.
 *    by Gisle Vanem <giva@bgnett.no>  Jun-99
 *
 *  This module really belongs to the application layer,
 *  but is included in Watt-32 for convenience.
 *
 *  Note:  syslog_init() must be called in sock_init() (or prior to
 *         that). syslog(), openlog() cannot be used before sock_init()
 *         is called.
 *
 *  To-do: configure variables in syslog2.c when calling openlog() thus
 *         making syslog() independant of sock_init().
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/syslog.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

#include "wattcp.h"
#include "misc.h"
#include "printk.h"
#include "pctcp.h"
#include "pcstat.h"
#include "pcbsd.h"
#include "pcbuf.h"
#include "syslog2.h"
#include "syslog.h"

#if defined(USE_BSD_FUNC)

#define INTERNALLOG  (LOG_ERR|LOG_CONS|LOG_PERROR|LOG_PID)

static sock_type *logSock   = NULL;     /* UDP socket for log */
static FILE      *logFile   = NULL;     /* FILE* for log */
static DWORD      logHost   = 0;        /* IP-address of host (host order) */
static BOOL       logOpened = 0;        /* have done openlog() */
static int        logStat   = 0;        /* status bits, set by openlog() */
static char      *logTag    = NULL;     /* string to tag the entry with */
static int        logFacil  = LOG_USER; /* default facility code */
static int        logMask   = 0xFF;     /* mask of priorities to be logged */

extern int errno_s;

/*
 * syslog, vsyslog --
 *    print message on log file; output is intended for syslogd(8).
 */
void syslog (int pri, const char *fmt, ...)
{ 
  va_list args;
  va_start (args, fmt);
  vsyslog (pri, fmt, args);
  va_end (args);
}

void vsyslog (int pri, const char *fmt, va_list ap)
{ 
  char  *p;
  char   tbuffer [2048];
  int    left = sizeof(tbuffer);
  int    saved_errno = errno;
  time_t t;

#if 0
  /* Check for invalid bits
   */
  if (pri & ~(LOG_PRIMASK|LOG_FACMASK))
  {
    syslog (INTERNALLOG, "syslog: unknown facility/priority: 0x%X", pri);
    pri &= (LOG_PRIMASK | LOG_FACMASK);
  }
#endif

  /* Check priority against setlogmask values
   */
#if 0
  if (!(LOG_MASK(LOG_PRI(pri)) & logMask))
     return;
#else
  if (!(LOG_UPTO(LOG_PRI(pri))))
     return;
#endif

  /* Set default facility if none specified
   */
  if (!(pri & LOG_FACMASK))
     pri |= logFacil;

  /* Build the message
   */
  time (&t);
  p  = tbuffer;
  p += _snprintk (p, left, "<%3d>%.15s ", pri, ctime(&t)+4);
  left -= p - tbuffer;

  if (logTag)
  {
    p += _snprintk (p, left, logTag);
    left -= p - tbuffer;
  }
  if (logStat & LOG_PID)
  {
    p += _snprintk (p, left, "[%d]", getpid());
    left -= p - tbuffer;
  }
  if (logTag)
  {
    p += _snprintk (p, left, ": ");
    left -= p - tbuffer;
  }

  errno = errno_s = saved_errno;
  p += _vsnprintk (p, left, fmt, ap);
  if (*(p-1) != '\n')
  {
    *p++ = '\n';
    *p = 0;
  }

  if (!logOpened)
     openlog (logTag, logStat | LOG_NDELAY, logFacil);

  if (logFile)
    _fputsk (tbuffer, logFile);

  if (logSock)
  {
    int   len       = sock_puts (logSock, (const BYTE*)tbuffer);
    const char *err = sockerr ((const tcp_Socket*)logSock);
    if (len == 0 || err)
    {
      sock_close (logSock);
      free (logSock);
      logSock = NULL;
      syslog (LOG_ERR, "UDP-write failed: %s\n", err);
    }
  }
}


static const char *getlogname (void)
{
  const char *name;

#if (DOSX & PHARLAP)
  extern char *GetArg0 (USHORT sel);
  CONFIG_INF   cnf;
  char        *arg0;

  _dx_config_inf (&cnf, (UCHAR*)&cnf);  /* get config block */
  arg0 = GetArg0 (cnf.c_env_sel);       /* in exc_??.lib    */
  if (arg0)
       name = strdup (arg0);
  else name = NULL;

#elif defined (__DJGPP__)
  extern char **__crt0_argv;
  name = strdup (__crt0_argv[0]);

#elif defined (_MSC_VER)
  extern char **__argv;
  name = strdup (__argv[0]);

#else
  extern char **_argv;
  name = strdup (_argv[0]);
#endif

  if (name)
  {   
    char *exe = strrchr (name, '.');  /* ".exe" or ".exp" */
    if (exe)
    {
      strcpy (exe, ".log");
      return (name);
    }
  }
  return ("$unknown.log");
}

static void openloghost (void)
{
  struct servent *sp;
  struct hostent *hp;

  if (logSock)
  {
    sock_close (logSock);
    logSock = NULL;
  }

  sp = getservbyname ("syslog", "udp");
  if (sp)
     syslog_port = htons (sp->s_port);

  logHost = _inet_addr (syslog_hostName);
  if (!logHost)
  {
    hp = gethostbyname (syslog_hostName);
    if (!hp)
    {
      fprintf (stderr, "syslog: Unknown host `%s'\n", syslog_hostName);
      return;
    }
    logHost = ntohl (*(DWORD*)hp->h_addr);
  }

  if (!logSock)
  {
    logSock = malloc (sizeof(udp_Socket));
    if (!logSock)
    {
      perror ("syslog");
      return;
    }
  }
  
  if (!udp_open ((udp_Socket*)logSock, 0, logHost, syslog_port, NULL))
  {
    errno = errno_s = EHOSTUNREACH;  /* "No route to host" (ARP failed) */
    STAT (ipstats.ips_noroute++);
    perror ("syslog");
    sock_close (logSock);
    logSock = NULL;
  }
}

void openlog (const char *ident, int logstat, int logfac)
{
  if (ident)
     logTag = (char*)ident;
  logStat = logstat;

  if (logfac && (logfac &~ LOG_FACMASK) == 0)
     logFacil = logfac;

  if (!syslog_fileName)           /* not set in wattcp.cfg */
     syslog_fileName = (char*) getlogname();

  if (syslog_mask)
     setlogmask (syslog_mask);

  if (logStat & LOG_NDELAY)       /* open immediately */
  {
    logFile = fopen (syslog_fileName, "at");
    if (logFile && fputs("\n",logFile) != EOF)
    {
      atexit (closelog);
      logOpened = 1;
    }
    else
    {
      perror ("syslog");
      logFile = NULL;
    }
  }

  if (syslog_hostName)
     openloghost();

  if (logSock && !logFile)
  {
    atexit (closelog);
    logOpened = 1;
  }

  _printk_init (2000, NULL);
  _printk_file = logFile;

  if (logSock)
  {
    syslog (LOG_INFO, "syslog client at %I started.", htonl(my_ip_addr));
    syslog (LOG_INFO, "Logging to host %s (%I)", syslog_hostName, htonl(logHost));
  }
}

void closelog (void)
{
  if (logFile)
     fclose (logFile);

  if (logSock)
  {
    sock_close (logSock);
    free (logSock);
  }
  logSock   = NULL;
  logFile   = NULL;
  logOpened = 0;
}

int setlogmask (int pmask)
{
  int old = logMask;
  if (pmask)
     logMask = pmask;
  return (old);
}

char *setlogtag (char *new)
{
  char *old = logTag;
  logTag = new;
  return (old);
}

#endif /* USE_BSD_FUNC */


#if defined(TEST_PROG)

#include "sock_ini.h"
#include "pcdbug.h"

int main (void)
{
  dbug_init();
  sock_init();

  openlog ("test", LOG_PID|LOG_NDELAY, LOG_LOCAL2);

  syslog (LOG_NOTICE, "time now %t. file %s at line %d\n",
          __FILE__, __LINE__);

  errno = ENOMEM;
  syslog (LOG_ERR, "Allocation failed; %m");

  errno = ENETDOWN;
  syslog (LOG_ERR, "my_ip_addr = %I. Connection failed; %m", htonl(my_ip_addr));

  syslog (LOG_INFO, "Leaving main()");

  printf ("Done. ");
  if (logFile)
     printf ("Look at `%s'", syslog_fileName);

  closelog();
  return (0);
}
#endif /* TEST_PROG */

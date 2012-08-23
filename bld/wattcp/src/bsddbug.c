/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 *         Debug functions moved from socket.c
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

#include "pcconfig.h"
#include "pcdbug.h"

static int sk_scope = 0;  /* scope-level (indenting printout) */

#if defined(USE_DEBUG)

static void (*prev_hook) (const char*, const char*);
static FILE  *dbg_file    = NULL;
static BOOL   dbg_active  = FALSE;
static char  *dbg_fname   = "WATTCP.SK";
static char   dbg_mode[5] = "w+";

void _sock_debugf (const Socket *sock, const char *fmt, ...)
{
#if 0  /* for now, print debug for all sockets */
  if (sock && !(sock->so_options & SO_DEBUG))
     return;
#else
  ARGSUSED (sock);
#endif

  if (dbg_file && dbg_active)
  {
    va_list arg;
    va_start (arg, fmt);

    if (*fmt == '\n')
    {
      fprintf (dbg_file, "\n%9s: ", time_str(set_timeout(0)));
      fmt++;
      if (sk_scope > 0)
         fprintf (dbg_file, "%*s", 2*sk_scope, "");
    }
    vfprintf (dbg_file, fmt, arg);
    va_end (arg);
  }
}

void _sock_dbgflush (void)
{
  if (dbg_file)
     fflush (dbg_file);
}

/*
 * Callbacks for config-table parser
 */
static void set_dbg_fname (const char *value)
{
  if ((dbg_fname = strdup(value)) == NULL)
       outsnl (_LANG("No memory for \"SK_DEBUG.DEVICE\"."));
  else if (!stricmp(dbg_fname,"stderr"))
       dbg_file = stderr;
  else if (!stricmp(dbg_fname,"stdout") || !stricmp(dbg_fname,"con"))
       dbg_file = stdout;
  else if (!stricmp(dbg_fname,"nul"))
       dbg_file = NULL;
}

static void set_dbg_mode (const char *value)
{
  strncpy (dbg_mode, value, sizeof(dbg_mode)-1);
  dbg_mode [sizeof(dbg_mode)-1] = '\0';
}

/*
 *  Config-file hook routine called from parser
 *  (set_values() in pcconfig.c and via ourinit() in pcdbug.c)
 *
 *  Parses keywords:
 *    SK_DEBUG.DEVICE   = <file>
 *    SK_DEBUG.OPENMODE = <mode>
 */
static void dbug_parse (const char *name, const char *value)
{
  static struct config_table sk_debug[] = {
                { "DEVICE",   ARG_FUNC, (void*)set_dbg_fname },
                { "OPENMODE", ARG_FUNC, (void*)set_dbg_mode  },
                { NULL }
              };

  if (!parse_config_table(&sk_debug[0], "SK_DEBUG.", name, value) && prev_hook)
     (*prev_hook) (name, value);
}

/*
 * Normally called from dbug_init() (in pcdbug.c) if USE_DEBUG is defined.
 * Must be called before sock_init() and any other socket functions.
 */
void _sock_dbug_init (void)
{
  prev_hook = usr_init;
  usr_init  = dbug_parse;
}


static void print_local_ports_inuse (void)
{
  WORD port, num = 0;
  
  fprintf (dbg_file, "\nLocal ports still in use:\n");
  for (port = 1025; port < USHRT_MAX; port++)
  {
    if (reuse_localport(port) > 0)
    {
      num++;
      fprintf (dbg_file, "%5u%c", port, num % 12 ? ',' : '\n');
    }
  }
  fflush (dbg_file);
}

void bsd_fortify_print (const char *buf)
{
  if (dbg_file)
     fputs (buf, dbg_file);
}

void _sock_dbug_exit (void)
{
  if (dbg_file && !(dbg_file == stdout || dbg_file == stderr))
  {
    if (!_watt_fatal_error)
       print_local_ports_inuse();
    fclose (dbg_file);
  }
  dbg_file  = NULL;
  dbg_fname = NULL;
}

void _sock_dbug_on (void)
{
  static int is_open = 0;

  dbg_active = TRUE;

#if 0  /* !! test */
  printf ("_sock_dbug_on: is_open %d, fname %s, file %p, mode %s\n",
          is_open, dbg_fname, dbg_file, dbg_mode);
#endif

  if (!is_open && dbg_fname && !(dbg_file == stdout || dbg_file == stderr))
  {
    dbg_file = fopen (dbg_fname, dbg_mode);
    atexit (_sock_dbug_exit);
    is_open = 1;
  }
}

void _sock_dbug_off (void)
{
  dbg_active = FALSE;  /* pause debug printout */
}
#endif  /* USE_DEBUG */


/*
 * These two functions are meant to increase/decrease the suspension
 * depth of a "task". I.e. _sock_enter_scope() should awake other
 * sleeping "tasks" waiting to be signalled.
 * Now they're only used to make the sock-trace prettier.
 */
void _sock_enter_scope (void)
{
  sk_scope++;
}

void _sock_leave_scope (void)
{
  if (sk_scope >= 1)
      sk_scope--;
}
#endif  /* USE_BSD_FUNC */

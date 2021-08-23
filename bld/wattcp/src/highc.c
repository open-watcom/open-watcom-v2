/*
 *  This file is for Metaware HighC/C++ with Pharlap only.
 *  Handles tracing of entry/exit functions and replaces the buggy
 *  system() routine.
 *
 *  Copyright (c) G.Vanem 1997 <giva@bgnett.no>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>

#include "wattcp.h"
#include "strings.h"
#include "pcconfig.h"

#if defined(__HIGHC__) && (DOSX & PHARLAP)

/*
 * HighC's putenv() doesn't modify the inherited environment.
 * We must do it ourself.
 */
static char *SetupEnvironment (void)
{
  char  *copy;
  char  *start;
  char **env  = _environ; /* list of env.var + putenv() vars */
  UINT   size = 0;

  if (!env || !*env)
     return (NULL);

  while (env && *env)  /* find how many bytes we need to malloc */
       size += strlen (*env++) + 1;

  if ((copy = malloc(++size)) == NULL)
     return (NULL);

  start = copy;
  env   = _environ;
  while (env && *env)  /* make a new continuous environment block */
  {
    int len = strlen (*env) + 1;

    memcpy (copy, *env++, len);
    copy += len;
  }
  *copy = '\0';        /* final termination */
  return (start);
}

static int Execute (char *path, char *args)
{
  union  REGS reg;
  EXEC_BLK    blk;
  char        buf[256];
  int         len;
  ULONG       minPages, maxPages;
  UCHAR      *our_env = (UCHAR*) SetupEnvironment();

  if ((len = strlen(args)) > sizeof(buf)-2)
     return (0);

  _dx_cmem_usage (0, FALSE, &minPages, &maxPages);

  strcpy (buf+1, args);
  buf [0]   = len++;
  buf [len] = '\r';

  /* if `our_env' is NULL we'll inherit the environment without any
   * variables added by `putenv()'
   */
  blk.ex_eoff = our_env;        /* pass new environment */
  blk.ex_eseg = SS_DATA;
  blk.ex_coff = (UCHAR*)&buf;   /* command line address */
  blk.ex_cseg = SS_DATA;
  reg.x.ax    = 0x4B00;
  reg.x.bx    = (UINT)&blk;
  reg.x.dx    = (UINT)path;
  fflush (stdout);              /* flush buffers now */
  fflush (stderr);

  intdos (&reg, &reg);
  if (reg.x.cflag & 1)          /* carry set, return -1 */
     return (-1);

  reg.x.ax = 0x4D00;
  intdos (&reg, &reg);
  return (reg.h.al);            /* return child exit code */
}

/*
 *  This replaces system() in hc386.lib
 *  Note: this is aliased in <tcp.h>
 */
int _mw_watt_system (const char *cmd)
{
  char buf[150];
  char *env = getenv ("COMSPEC");

  if (!env || access(env,0))
  {
    (*_printf) ("Bad COMSPEC variable\r\n");
    return (0);
  }

  if (cmd && *cmd)
       _bprintf (buf, sizeof(buf), " /c %s", cmd);
  else _bprintf (buf, sizeof(buf), "%s", cmd);
  return Execute (env, buf);
}

/*
 * The following functions are called when call_trace, _prolog_trace
 * and epilog_trace pragmas are in effect.
 * Library must be compiled with:  -Hon=call_trace
 *                             and -Hon=epilog_trace
 */

#pragma Off(call_trace)      /* no trace of trace itself */
#pragma Off(prolog_trace)  
#pragma Off(epilog_trace)
#pragma Global_aliasing_convention("%r")

#if defined(USE_DEBUG)

#define MAX_LEVEL 1000

static int   level = 0;
static char *caller [MAX_LEVEL];

void _call_trace (char *from, char *module, unsigned line, char *to)
{
  char where[30];

  if (!ctrace_on || level == MAX_LEVEL-1)
     return;

  _bprintf (where, sizeof(where), "%s(%u)", module, line);

  (*_printf) ("%-14s:%*s %s() -> %s()\n", where, 2*level, "", from, to);

  if (caller[level] == NULL || strcmp(from,caller[level]))
      caller[level++] = from;
}

void _prolog_trace (char *func)
{
  (*_printf) ("%s()\n", func);
}

void _epilog_trace (char *func)
{
  if (level && ctrace_on)
    --level;
}
#else     /* !USE_DEBUG */

void _call_trace (char *from, char *module, unsigned line, char *to)
{
  ARGSUSED (from);
  ARGSUSED (module);
  ARGSUSED (line);
  ARGSUSED (to);
}

void _prolog_trace (char *func)
{
  ARGSUSED (func);
}

void _epilog_trace (char *func)
{
  ARGSUSED (func);
}

#endif   /* USE_DEBUG */
#endif   /* __HIGHC__ && (DOSX & PHARLAP) */

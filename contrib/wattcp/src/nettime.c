/*
 *  BSD sockets functionality for Waterloo TCP/IP
 *
 *  G. Vanem  <giva@bgnett.no>
 *
 *  14.may 1999 (GV)  - Created
 *
 *  This module implements time counters for "user" and "system"
 *  calls. NOTE: "system" time is counted only when signals are
 *  trapped. i.e. around select_s(), connect() loops.
 *
 *  Based on djgpp version of times():
 *
 *    Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

static DWORD sock_sys_time;
static DWORD start;

void _sock_start_timer (void)
{
  start = set_timeout (0);
}

void _sock_stop_timer (void)
{
  DWORD now = set_timeout (0);

  if (now > start)
       sock_sys_time += (now - start);
  else sock_sys_time += (start - now); /* msec/tick counter wrapped */
}

static DWORD sys_time (void)
{
  if (!_watt_is_init)
     return (0);
  return (sock_sys_time);
}

static DWORD usr_time (void)
{
  if (!_watt_is_init)
     return (0);
  return (set_timeout(0) - _watt_start_time - sock_sys_time);
}

unsigned long net_times (struct tms *tms)
{
  DWORD divisor;

  if (!tms)
  {
    SOCK_ERR (EINVAL);
    return (unsigned long)(-1);
  }
  memset (tms, 0, sizeof(*tms));
  divisor = has_8254 ? 1000 : 18;

  tms->tms_utime = (CLOCKS_PER_SEC * usr_time()) / divisor;
  tms->tms_stime = (CLOCKS_PER_SEC * sys_time()) / divisor;

  return (tms->tms_utime);
}
#endif  /* USE_BSD_FUNC */


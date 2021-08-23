/*
 *  sys_errlist[] with BSD-socket support
 *
 *  G. Vanem  <giva@bgnett.no>
 *
 *  05.jan 1998 (GV)  - Created
 *
 *  This module tries to add network related errno/strerror/perror
 *  for all supported Watt-32 targets.
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

char __syserr00[] = "No Error";

#undef  NEAR
#define NEAR

#define SYS_ERRLIST _sys_errlist
#define SYS_NERR    _sys_nerr
#include "watcom/syserr.c"

/*
 * A hack to make the linker use this sys_errlist[]
 * and not the normal one in the C-library.
 * Is there a better way?
 */
int __pull_neterr_module = 0;

/*
 * `errno_s' is always set to same value
 * as `errno' through macro SOCK_ERR() in socket.h
 */
int errno_s = 0;

int _WCDATA SYS_NERR = sizeof(SYS_ERRLIST) / sizeof(SYS_ERRLIST[0]);

void perror (const char *str)
{
  fprintf (stderr, "%s: %s", str, strerror(errno));
}

char *strerror (int errnum)
{
  static char buf[70];

  if (errnum >= 0 && errnum < SYS_NERR)
       strcpy (buf, SYS_ERRLIST[errnum]);
  else sprintf (buf, "Unknown error: %d", errnum);

 /*
  * All(?) except djgpp have a newline at the end.
  */
  strcat (buf, "\n");

  return (buf);
}

#define ALIAS(x)

/*
 * Incase you have trouble with multiple defined symbols,
 * make sure the following functions are used before
 * perror/strerror/sys_errlist in your C-library.
 */
void perror_s (const char *str)
{
  perror (str);
}

char *strerror_s (int errnum)
{
  return strerror (errnum);
}

#endif  /* USE_BSD_FUNC */

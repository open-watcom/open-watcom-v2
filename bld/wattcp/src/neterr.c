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

#if !defined(_MSC_VER)
  #undef  NEAR
  #define NEAR
#endif

#if defined(__TURBOC__) && (__TURBOC__ <= 0x410) /* TCC/BCC <= 3.1 */
  #define SYS_ERRLIST sys_errlist
  #define SYS_NERR    sys_nerr
  #include "borland/syserr.c"

#elif defined(__BORLANDC__)
  #define SYS_ERRLIST _sys_errlist
  #define SYS_NERR    _sys_nerr
  #include "borland/syserr.c"

#elif defined(__HIGHC__)
  #define SYS_ERRLIST _sys_errlist
  #define SYS_NERR    _sys_nerr
  #include "metaware/syserr.c"

#elif defined(__WATCOMC__)
  #define SYS_ERRLIST _sys_errlist
  #define SYS_NERR    _sys_nerr
  #include "watcom/syserr.c"

#elif defined(_MSC_VER) && (_MSC_VER <= 800)
  #define NEAR        _near    /* even for large model */
  #define SYS_ERRLIST NEAR sys_errlist
  #define SYS_NERR    sys_nerr
  #include "msoft/syserr.c"
  #undef  SYS_ERRLIST
  #define SYS_ERRLIST sys_errlist

#elif defined(_MSC_VER) && (_MSC_VER > 800) /* Visual C; cl32.exe */
  #define SYS_ERRLIST NEAR sys_errlist
  #define SYS_NERR    sys_nerr
  #include "msoft/syserr32.c"
  #undef  SYS_ERRLIST
  #define SYS_ERRLIST sys_errlist

#elif defined(__DJGPP__)
  #define SYS_ERRLIST sys_errlist
  #define SYS_NERR    sys_nerr
  #include "djgpp/syserr.c"

#else
  #error Unknown target!
#endif

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

#ifdef __WATCOMC__
int _WCDATA SYS_NERR = sizeof(SYS_ERRLIST) / sizeof(SYS_ERRLIST[0]);
#else
int NEAR SYS_NERR = sizeof(SYS_ERRLIST) / sizeof(SYS_ERRLIST[0]);
#endif

void perror (const char *str)
{
#ifdef __DJGPP__
  fprintf (stderr, "%s: %s\n", str, strerror(errno));
#else
  fprintf (stderr, "%s: %s", str, strerror(errno));
#endif
}

char *strerror (int errnum)
{
  static char buf[70];

  if (errnum >= 0 && errnum < SYS_NERR)
       strcpy (buf, SYS_ERRLIST[errnum]);
  else sprintf (buf, "Unknown error: %d", errnum);

#ifndef __DJGPP__
 /*
  * All(?) except djgpp have a newline at the end.
  */
  strcat (buf, "\n");
#endif

  return (buf);
}

#ifdef __GNUC__
  #define ALIAS(x) __attribute__((weak,alias(__CONCAT("w32_",__STRING(x)))))
// test  char *strerror2 (int err) ALIAS(strerror);
#else
  #define ALIAS(x)
#endif


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

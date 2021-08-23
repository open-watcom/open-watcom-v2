/*
 * Waterloo TCP
 *
 * sys_errlist[] and errno's for compilers with limited errnos
 *
 * G. Vanem <giva@bgnett.no> 1998
 */

#ifndef __SYS_WERRNO_H
#define __SYS_WERRNO_H

/* When doing "gcc -MM" with gcc 3.0 we must include <sys/version.h>
 * in order for __DJCPP__ to be defined
 */
#include <stdio.h>  
#include <errno.h>

#if defined(__HIGHC__)
  #undef EDEADLK
  #undef EDEADLOCK
  #include <sys/metaware.err>

#elif defined(__BORLANDC__)
  #ifdef __FLAT__
  #undef ENAMETOOLONG  /* bcc32 4.0 */
  #endif
  #include <sys/borland.err>

#elif defined(__TURBOC__)
  #include <sys/turboc.err>

#elif defined(__WATCOMC__)
  #include <sys/watcom.err>

#elif defined(__DJGPP__)
  #include <sys/djgpp.err>

#elif defined(_MSC_VER) && (_MSC_VER <= 800)  /* MSC 8.0 or older */
  #include <sys/msoft.err>

#elif defined(_MSC_VER) && (_MSC_VER > 800)   /* Visual C; cl32.exe */
  #undef EDEADLOCK
  #include <sys/msoft32.err>

#else
  #error Unknown target in <sys/werrno.h>.
#endif

/*
 * Incase you have trouble with duplicate defined symbols,
 * make sure these are used before perror/strerror/sys_errlist
 * in your C-library.
 */
extern void  perror_s   (const char *str);
extern char *strerror_s (int errnum);

#endif  /* __SYS_WERRNO_H */


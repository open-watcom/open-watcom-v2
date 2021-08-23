/*
 * Waterloo TCP
 *
 * sys_errlist[] and errno's for compilers with limited errnos
 *
 * G. Vanem <giva@bgnett.no> 1998
 */

#ifndef __SYS_WERRNO_H
#define __SYS_WERRNO_H

#include <stdio.h>
#include <errno.h>
#include <sys/watcom.err>

/*
 * Incase you have trouble with duplicate defined symbols,
 * make sure these are used before perror/strerror/sys_errlist
 * in your C-library.
 */
extern void  perror_s   (const char *str);
extern char *strerror_s (int errnum);

#endif  /* __SYS_WERRNO_H */


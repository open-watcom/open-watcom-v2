/*
 *  settimeofday() for non-djgpp targets
 *
 *  The exported prototype used is used is the one specified in the 
 *  XOpen/POSIX 1.3 standards and the one used on modern (ie 4.4BSD spec) 
 *  BSDs. ie 'int settimeofday(struct timeval *, ...)', ie the second
 *  arg, if specified, is ignored.
 */

#ifndef __SETTIMEOFDAY_H
#define __SETTIMEOFDAY_H

#if !defined(__DJGPP__) /* djgpp already has [g|s]ettimeofday() */

extern int settimeofday (struct timeval *tv, ...);

#endif
#endif

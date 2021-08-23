#ifndef __PRINTK_H
#define __PRINTK_H

#include <stdio.h>
#include <stdarg.h>

#if defined(__DJGPP__) || defined(__HIGHC__) || defined(__WATCOMC__)
#include <unistd.h>
#else
#include <process.h>
#endif

#if defined(__WATCOMC__)
#include <process.h>
#endif

#ifdef __WATTCP_H  /* if included after wattcp.h (Watt-32 compile) */
  #define _printk_safe   NAMESPACE (_printk_safe)
  #define _printk_file   NAMESPACE (_printk_file)
  #define _printk_init   NAMESPACE (_printk_init)
  #define _printk_exit   NAMESPACE (_printk_exit)
  #define _printk_flush  NAMESPACE (_printk_flush)
  #define _printk        NAMESPACE (_printk)
  #define _fputsk        NAMESPACE (_fputsk)
  #define _snprintk      NAMESPACE (_snprintk)
  #define _vsnprintk     NAMESPACE (_vsnprintk)
#endif

extern int   _printk_safe;  /* safe to print; we're not in a RMCB/ISR */
extern FILE *_printk_file;  /* what file to print to (or stdout/stderr) */

extern int  _printk_init  (int size, const char *file);
extern void _printk_exit  (void);
extern void _printk_flush (void);

extern void _printk    (const char *fmt, ...);
extern void _fputsk    (const char *buf, FILE *stream);
extern int  _vsnprintk (char *buf, int len, const char *fmt, va_list ap);
extern int  _snprintk  (char *buf, int len, const char *fmt, ...);

#endif  /* __PRINTK_H */


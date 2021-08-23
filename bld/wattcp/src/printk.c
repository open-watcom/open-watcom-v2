/*
 *  printk.c - Formatted printf style routines for syslog().
 *
 *             These are safe to use in interrupt handlers
 *             Inspired by Linux's printk().
 *
 *  Written by Gisle Vanem <giva@bgnett.no>  Jan-97
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "wattcp.h"
#include "strings.h"
#include "misc.h"
#include "pcbsd.h"
#include "printk.h"

#if defined(USE_BSD_FUNC)

int   _printk_safe = 1;     /* must be set to 0 in intr-handlers */
FILE *_printk_file = stderr;

static char *printk_ptr = NULL;
static char *printk_buf = NULL;
static char *printk_end = NULL;

static const char *str_signal (int sig);

/*
 *  _snprintk - format a message into a buffer.  Like sprintf except we
 *  also specify the length of the output buffer, and we handle
 *  %m (error message), %t (current time) and %I (IP address) formats.
 *  Doesn't do floating-point formats and long modifiers (e.g. "%lu").
 *  Returns the number of chars put into buf.
 */
int _snprintk (char *buf, int buflen, const char *fmt, ...)
{
  int     len;
  va_list args;
  va_start (args, fmt);

  len = _vsnprintk (buf, buflen, fmt, args);
  va_end (args);
  return (len);
}

void _printk (const char *fmt, ...)
{
  int left = (int)(printk_end - printk_ptr);

  if (_printk_file && fmt && left > 0)
  {
    va_list args;
    va_start (args, fmt);
    printk_ptr += _vsnprintk (printk_ptr, left, fmt, args);
    _printk_flush();
    va_end (args);
  }
}

void _fputsk (const char *buf, FILE *stream)
{
  if (stream && _printk_safe)
  {
    int len = strlen (buf);
    fwrite (buf, len, 1, stream);
  }
}

void _printk_flush (void)
{
  int len = printk_ptr - printk_buf;

  if (_printk_safe && _printk_file && len > 0)
  {
    fwrite (printk_buf, len, 1, _printk_file);
    printk_ptr = printk_buf;
  }
}

void _printk_exit (void)
{
  _printk_flush();
  if (_printk_file && _printk_file != stderr && _printk_file != stdout)
  {
    fclose (_printk_file);
    _printk_file = NULL;
  }
}

/*
 *  Called from openlog() to allocate print-buffer and
 *  optionally open syslog-file.
 */
int _printk_init (int size, const char *file)
{
  if (!printk_buf)
  {
    printk_ptr = printk_buf = malloc (size);
    if (!printk_ptr)
    {
      fprintf (stderr,"_printk_init: allocation failed\n");
      return (0);
    }
  }
  if (file && (_printk_file = fopen(file,"wt")) == NULL)
  {
    fprintf (stderr,"_printk_init: cannot open `%s'\n",file);
    return (0);
  }
  printk_end = printk_ptr + size;
  atexit (_printk_exit);
  return (1);
}


/*
 * _vsnprintk - like _snprintk, takes a va_list instead of a list of args.
 */
#define OUTCHAR(c)  (buflen > 0 ? (--buflen, *buf++ = (c)) : 0)

int _vsnprintk (char *buf, int buflen, const char *fmt, va_list args)
{
  int    c, i, n, width, prec, fillch;
  int    base, len, neg, quoted, upper;
  DWORD  val = 0L;
  BYTE  *p;
  char  *str, *f, *buf0 = buf, *_fmt = (char*)fmt;
  char   num[32];
  time_t t;

  if (--buflen < 0)
     return (-1);

  while (buflen > 0)
  {
    for (f = _fmt; *f != '%' && *f != 0; ++f)
        ;
    if (f > _fmt)
    {
      len = f - _fmt;
      if (len > buflen)
          len = buflen;
      memcpy (buf, _fmt, len);
      buf    += len;
      buflen -= len;
      _fmt = f;
    }
    if (*_fmt == 0)
       break;
    c = *++_fmt;
    width = prec = 0;
    fillch = ' ';
    if (c == '0')
    {
      fillch = '0';
      c = *++_fmt;
    }
    if (c == '*')
    {
      width = va_arg (args, int);
      c = *++_fmt;
    }
    else
      while (isdigit(c))
      {
        width = width * 10 + c - '0';
        c = *++_fmt;
      }

    if (c == '.')
    {
      c = *++_fmt;
      if (c == '*')
      {
        prec = va_arg (args, int);
        c = *++_fmt;
      }
      else
      {
        while (isdigit(c))
        {
          prec = prec * 10 + c - '0';
          c = *++_fmt;
        }
      }
    }
    str   = NULL;
    base  = 0;
    neg   = 0;
    upper = 0;
    ++_fmt;

    switch (c)
    {
      case 'd':
           i = va_arg (args, int);
           if (i < 0)
           {
             neg = 1;
             val = -i;
           }
           else
             val = i;
           base = 10;
           break;

      case 'u':
           val  = va_arg (args, unsigned);
           base = 10;
           break;

      case 'o':
           val  = va_arg (args, unsigned int);
           base = 8;
           break;

      case 'X':
           upper = 1;
           /* fall through */
      case 'x':
           val  = va_arg (args, unsigned int);
           base = 16;
           break;

      case 'p':
           val   = (DWORD)va_arg (args,void*);
           base  = 16;
           neg   = 2;
           upper = 1;
           break;

      case 's':
           str = va_arg (args, char*);
           break;

      case 'S':
           str = (char*) str_signal (va_arg(args, int));
           break;

      case 'c':
           num[0] = va_arg (args, int);
           num[1] = 0;
           str = num;
           break;

      case 'm':
           str = rip (strerror(errno));
           break;

      case 'I':
           str = _inet_ntoa (num, ntohl(va_arg(args,DWORD)));
           break;

      case 't':
           str = "??";
           if (!_printk_safe)
              break;
           time (&t);
           str  = ctime (&t);
           str += 4;           /* chop off the day name */
           str[15] = '\0';     /* chop off year and newline */
           break;

      case 'v':                /* "visible" string */
      case 'q':                /* quoted string */
           quoted = c == 'q';
           p = va_arg (args, BYTE*);
           if (fillch == '0' && prec > 0)
              n = prec;
           else
           {
             n = strlen ((char*)p);
             if (prec > 0 && prec < n)
                n = prec;
           }
           while (n > 0 && buflen > 0)
           {
             c = *p++;
             --n;
             if (!quoted && c >= 0x80)
             {
               OUTCHAR ('M');
               OUTCHAR ('-');
               c -= 0x80;
             }
             if (quoted && (c == '"' || c == '\\'))
                OUTCHAR ('\\');
             if (c < 0x20 || (0x7F <= c && c < 0xA0))
             {
               if (quoted)
               {
                 OUTCHAR ('\\');
                 switch (c)
                 {
                   case '\t': OUTCHAR ('t');   break;
                   case '\n': OUTCHAR ('n');   break;
                   case '\b': OUTCHAR ('b');   break;
                   case '\f': OUTCHAR ('f');   break;
                   default  : OUTCHAR ('x');
                              OUTCHAR (hex_chars[c >> 4]);
                              OUTCHAR (hex_chars[c & 0xf]);
                 }
               }
               else
               {
                 if (c == '\t')
                    OUTCHAR (c);
                 else
                 {
                   OUTCHAR ('^');
                   OUTCHAR (c ^ 0x40);
                 }
               }
             }
             else
               OUTCHAR (c);
           }
           continue;

      default:
           *buf++ = '%';
           if (c != '%')
              --_fmt;        /* so %z outputs %z etc. */
           --buflen;
           continue;
    }

    if (base)
    {
      str = num + sizeof(num);
      *--str = 0;
      while (str > num + neg)
      {
        *--str = upper ? hex_CHARS [val % base] : hex_chars [val % base];
        val = val / base;
        if (--prec <= 0 && val == 0)
           break;
      }
      switch (neg)
      {
        case 1: *--str = '-';
                break;
        case 2: *--str = 'x';
                *--str = '0';
                break;
      }
      len = num + sizeof(num) - 1 - str;
    }
    else
    {
      len = strlen (str);
      if (prec > 0 && len > prec)
         len = prec;
    }
    if (width > 0)
    {
      if (width > buflen)
          width = buflen;
      if ((n = width - len) > 0)
      {
        buflen -= n;
        for (; n > 0; --n)
            *buf++ = fillch;
      }
    }
    if (len > buflen)
        len = buflen;
    memcpy (buf, str, len);
    buf += len;
    buflen -= len;
  }
  *buf = '\0';
  return (buf - buf0);
}


/*
 * Return name for signal 'sig'
 */
#ifdef __HIGHC__
#undef SIGABRT   /* = SIGIOT */
#endif

static const char *str_signal (int sig)
{
  static char buf[20];
  switch (sig)
  {
    case 0:       return ("None");
#ifdef SIGINT
    case SIGINT:  return ("SIGINT");
#endif
#ifdef SIGABRT
    case SIGABRT: return ("SIGABRT");
#endif
#ifdef SIGFPE
    case SIGFPE:  return ("SIGFPE");
#endif
#ifdef SIGILL
    case SIGILL:  return ("SIGILL");
#endif
#ifdef SIGSEGV
    case SIGSEGV: return ("SIGSEGV");
#endif
#ifdef SIGTERM
    case SIGTERM: return ("SIGTERM");
#endif
#ifdef SIGALRM
    case SIGALRM: return ("SIGALRM");
#endif
#ifdef SIGHUP
    case SIGHUP:  return ("SIGHUP");
#endif
#ifdef SIGKILL
    case SIGKILL: return ("SIGKILL");
#endif
#ifdef SIGPIPE
    case SIGPIPE: return ("SIGPIPE");
#endif
#ifdef SIGQUIT
    case SIGQUIT: return ("SIGQUIT");
#endif
#ifdef SIGUSR1
    case SIGUSR1: return ("SIGUSR1");
#endif
#ifdef SIGUSR2
    case SIGUSR2: return ("SIGUSR2");
#endif
#ifdef SIGUSR3
    case SIGUSR3: return ("SIGUSR3");
#endif
#ifdef SIGNOFP
    case SIGNOFP: return ("SIGNOFP");
#endif
#ifdef SIGTRAP
    case SIGTRAP: return ("SIGTRAP");
#endif
#ifdef SIGTIMR
    case SIGTIMR: return ("SIGTIMR");
#endif
#ifdef SIGPROF
    case SIGPROF: return ("SIGPROF");
#endif
#ifdef SIGSTAK
    case SIGSTAK: return ("SIGSTAK");
#endif
#ifdef SIGBRK
    case SIGBRK:  return ("SIGBRK");
#endif
#ifdef SIGBUS
    case SIGBUS:  return ("SIGBUS");
#endif
#ifdef SIGIOT
    case SIGIOT:  return ("SIGIOT");
#endif
#ifdef SIGEMT
    case SIGEMT:  return ("SIGEMT");
#endif
#ifdef SIGSYS
    case SIGSYS:  return ("SIGSYS");
#endif
#ifdef SIGCHLD
    case SIGCHLD: return ("SIGCHLD");
#endif
#ifdef SIGPWR
    case SIGPWR:  return ("SIGPWR");
#endif
#ifdef SIGWINCH
    case SIGWINCH:return ("SIGWINCH");
#endif
#ifdef SIGPOLL
    case SIGPOLL: return ("SIGPOLL");
#endif
#ifdef SIGCONT
    case SIGCONT: return ("SIGCONT");
#endif
#ifdef SIGSTOP
    case SIGSTOP: return ("SIGSTOP");
#endif
#ifdef SIGTSTP
    case SIGTSTP: return ("SIGTSTP");
#endif
#ifdef SIGTTIN
    case SIGTTIN: return ("SIGTTIN");
#endif
#ifdef SIGTTOU
    case SIGTTOU: return ("SIGTTOU");
#endif
#ifdef SIGURG
    case SIGURG:  return ("SIGURG");
#endif
#ifdef SIGLOST
    case SIGLOST: return ("SIGLOST");
#endif
#ifdef SIGDIL
    case SIGDIL:  return ("SIGDIL");
#endif
#ifdef SIGXCPU
    case SIGXCPU: return ("SIGXCPU");
#endif
#ifdef SIGXFSZ
    case SIGXFSZ: return ("SIGXFSZ");
#endif 
  }
  strcpy (buf, "Unknown ");
  itoa (sig, buf+8, 10);
  return (buf);
}

#endif /* USE_BSD_FUNC */

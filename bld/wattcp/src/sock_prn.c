#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "pctcp.h"

int sock_printf (sock_type *s, const char *fmt, ...)
{
  char    buf [tcp_MaxBufSize];
  int     len;
  va_list args;

  va_start (args, fmt);

#if defined(__HIGHC__) || defined(__WATCOMC__)
  len = _vbprintf (buf, sizeof(buf)-1, fmt, args);
  if (len < 0)
  {
    outsnl (_LANG("ERROR: sock_printf() overrun"));
    len = sizeof(buf)-1;
    buf [len] = '\0';
  }
#else
  len = vsprintf (buf, fmt, args);
  if (len > sizeof(buf))
  {
    outsnl (_LANG("ERROR: sock_printf() overrun"));
    return (0);
  }
#endif

  sock_puts (s, (const BYTE*)&buf);
  va_end (args);
  return (len);
}


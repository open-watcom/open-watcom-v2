#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "misc.h"
#include "pctcp.h"

/*
 * MSC/DJGPP 2.01 doesn't have vsscanf()
 */
#if defined(__DJGPP__) && (DJGPP_MINOR < 2)
  static int vsscanf (const char *buf, const char *fmt, va_list arglist)
  {
    FILE *fil = stdin;
    fil->_ptr = (char*) buf;
    return _doscan (fil, fmt, arglist);
  }
#endif

#if defined(_MSC_VER)
  static int vsscanf (const char *buf, const char *fmt, va_list arglist)
  {
    extern _input (FILE*, const char*, va_list);
    FILE *fil = stdin;
    fil->_ptr = (char*) buf;
    return _input (fil, fmt, arglist);
  }
#endif

/*
 * sock_scanf - return number of fields returned
 */
int sock_scanf (sock_type *s, const char *fmt, ...)
{
  char buffer [tcp_MaxBufSize];
  int  fields;
  int  status;

  fields = 0;
  while ((status = sock_dataready(s)) == 0)
  {
    if (status == -1)
       return (-1);

    fields = sock_gets (s, (BYTE*)&buffer, tcp_MaxBufSize);
    if (fields)
    {
      va_list args; 
      va_start (args, fmt);
      fields = vsscanf (buffer, fmt, args);
      va_end (args);
    }
  }
  return (fields);
}


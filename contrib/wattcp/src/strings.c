#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>

#include "wattcp.h"
#include "strings.h"

/*
 * This file contains some gross hacks. Take great care !
 */

#if defined(__HIGHC__) || defined(__WATCOMC__) /* disable stack-checking */
#pragma off(check_stack)
#endif

#if (defined(__TURBOC__) || defined(__BORLANDC__)) && !defined(OLD_TURBOC)
#pragma option -N-
#endif

#ifdef __WATCOMC__
  extern void _outchar (char c);
  #pragma aux _outchar = \
          "mov ah, 2"    \
          "int 21h"      \
          parm [dl]      \
          modify [ax];
#endif

/*
 * Print a single character to stdout.
 */
static void outch (char c)
{
  if (c == (char)0x1A)  /* EOF (^Z) causes trouble to stdout */
     return;

#if defined(NO_INLINE_ASM) /* MSC <=6.0 need this hack */
  {
    union REGS r;
    r.h.ah = 2;
    r.h.dl = c;
    intdos (&r, &r);
  }
#elif defined(__TURBOC__) || defined(__BORLANDC__) || defined(_MSC_VER)
  asm mov dl,c
  asm mov ah,2
  asm int 21h

#elif defined(__HIGHC__)     /* outch() must not be optimised! */
  _inline (0x8A,0x55,0x08);  /* mov dl,[ebp+8] */
  _inline (0xB4,0x02);       /* mov ah,2       */
  _inline (0xCD,0x21);       /* int 21h        */

#elif defined(__GNUC__)
  __asm__ __volatile__
          ("movb %b0, %%dl\n\t"
           "movb $2, %%ah\n\t"
           "int $0x21\n\t"
           :
           : "r" (c)
           : "%eax", "%edx" );

#elif defined(__WATCOMC__)
  _outchar (c);

#else
  #error Tell me how to do this
#endif
}

void (*_outch)(char c) = outch;

/*---------------------------------------------------*/

#if defined(USE_DEBUG)
  int (*_printf) (const char*, ...) = printf;
#else
  static int EmptyPrint (const char *fmt, ...)
  {
    outsnl ("`(*_printf)()' called outside `USE_DEBUG'");
    ARGSUSED (fmt);
    return (0);
  }
  int (*_printf)(const char*, ...) = EmptyPrint;
#endif

/*---------------------------------------------------*/

void outs (const char *s)
{
  while (*s != '\0')
  {
    if (*s == '\n')
       (*_outch) ('\r');
    (*_outch) (*s++);
  }
}

void outsnl (const char *s)
{
  outs (s);
  (*_outch) ('\r');
  (*_outch) ('\n');
}

void outsn (const char *s, int n)
{
  while (*s != '\0' && n-- >= 0)
  {
    if (*s == '\n')
       (*_outch) ('\r');
    (*_outch) (*s++);
  }
}

void outhex (char c)
{
  char hi = (c & 0xF0) >> 4;
  char lo = c & 15;

  if (hi > 9)
       (*_outch) ((char)(hi-10+'A'));
  else (*_outch) ((char)(hi+'0'));

  if (lo > 9)
       (*_outch) ((char)(lo-10+'A'));
  else (*_outch) ((char)(lo+'0'));
}

void outhexes (const char *s, int n)
{
  while (n-- > 0)
  {
    outhex (*s++);
    (*_outch) (' ');
  }
}

/*---------------------------------------------------*/

char *rip (char *s)
{
  char *p;

  if ((p = strchr(s,'\n')) != NULL) *p = '\0';
  if ((p = strchr(s,'\r')) != NULL) *p = '\0';
  return (s);
}

/*---------------------------------------------------*/

#ifdef NOT_USED
int isstring (const char *string, unsigned stringlen)
{
  if ((unsigned) strlen(string) > stringlen - 1)
     return (0);

  while (*string)
  {
    if (!isprint (*string++))
    {
      string--;
      if (!isspace (*string++))
         return (0);
    }
  }
  return (1);
}
#endif

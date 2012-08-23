/*
 * ++Copyright++ 1985, 1993
 * -
 * Copyright (c) 1985, 1993
 *    The Regents of the University of California.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *   This product includes software developed by the University of
 *   California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

#include "resolver.h"

#if defined(USE_BIND)

static int mklower (int);
static int dn_find (u_char *, u_char *, u_char **, u_char **);

/*
 * Expand compressed domain name 'comp_dn' to full domain name.
 * 'msg' is a pointer to the begining of the message,
 * 'eomorig' points to the first location after the message,
 * 'exp_dn' is a pointer to a buffer of size 'length' for the result.
 * Return size of compressed name or -1 if there was an error.
 */
int dn_expand (const u_char *msg, const u_char *eomorig,
               const u_char *comp_dn, char *exp_dn, int length)
{
  const u_char *cp;
  char *dn, *eom;
  int   n, len = -1, checked = 0;

  dn  = exp_dn;
  cp  = comp_dn;
  eom = exp_dn + length;
  /*
   * fetch next label in domain name
   */
  while ((n = *cp++) != 0)
  {
    /*
     * Check for indirection
     */
    switch (n & INDIR_MASK)
    {
      case 0:
           if (dn != exp_dn)
           {
             if (dn >= eom)
                return (-1);
             *dn++ = '.';
           }
           if (dn+n >= eom)
              return (-1);
           checked += n + 1;
           while (--n >= 0)
           {
             int c = *cp++;
             if ((c == '.') || (c == '\\'))
             {
               if (dn + n + 2 >= eom)
                  return (-1);
               *dn++ = '\\';
             }
             *dn++ = c;
             if (cp >= eomorig)  /* out of range */
                return (-1);
           }
           break;

      case INDIR_MASK:
           if (len < 0)
              len = cp - comp_dn + 1;
           cp = msg + (((n & 0x3f) << 8) | (*cp & 0xff));
           if (cp < msg || cp >= eomorig)  /* out of range */
              return (-1);
           checked += 2;
           /*
            * Check for loops in the compressed name;
            * if we've looked at the whole message,
            * there must be a loop.
            */
           if (checked >= eomorig - msg)
              return (-1);
           break;

      default:
           return (-1);      /* flag error */
    }
  }

  *dn = '\0';
  {
    int c;
    for (dn = exp_dn; (c = *dn) != '\0'; dn++)
      if (isascii(c) && isspace(c))
         return (-1);
  }

  if (len < 0)
     len = cp - comp_dn;
  return (len);
}

/*
 * Compress domain name 'exp_dn' into 'comp_dn'.
 * Return the size of the compressed name or -1.
 * 'length' is the size of the array pointed to by 'comp_dn'.
 * 'dnptrs' is a list of pointers to previous compressed names. dnptrs[0]
 * is a pointer to the beginning of the message. The list ends with NULL.
 * 'lastdnptr' is a pointer to the end of the arrary pointed to
 * by 'dnptrs'. Side effect is to update the list of pointers for
 * labels inserted into the message as we compress the name.
 * If 'dnptr' is NULL, we don't try to compress names. If 'lastdnptr'
 * is NULL, we don't update the list.
 */
int dn_comp (const char *exp_dn, u_char *comp_dn, int length,
             u_char **dnptrs, u_char **lastdnptr)
{
  u_char  *cp, *dn;
  u_char **cpp, **lpp, *eob;
  u_char  *msg;
  u_char  *sp = NULL;
  int      c, l = 0;

  dn  = (u_char *)exp_dn;
  cp  = comp_dn;
  eob = cp + length;
  lpp = cpp = NULL;
  if (dnptrs)
  {
    if ((msg = *dnptrs++) != NULL)
    {
      for (cpp = dnptrs; *cpp; cpp++)
          ;
      lpp = cpp;  /* end of list to search */
    }
  }
  else
    msg = NULL;

  for (c = *dn++; c != '\0'; )
  {
    /* look to see if we can use pointers */
    if (msg)
    {
      if ((l = dn_find (dn-1, msg, dnptrs, lpp)) >= 0)
      {
        if (cp+1 >= eob)
           return (-1);
        *cp++ = (l >> 8) | INDIR_MASK;
        *cp++ = l % 256;
        return (cp - comp_dn);
      }
      /* not found, save it */
      if (lastdnptr && cpp < lastdnptr-1)
      {
        *cpp++ = cp;
        *cpp = NULL;
      }
    }
    sp = cp++;  /* save ptr to length byte */
    do
    {
      if (c == '.')
      {
        c = *dn++;
        break;
      }
      if (c == '\\')
      {
        if ((c = *dn++) == '\0')
           break;
      }
      if (cp >= eob)
      {
        if (msg)
           *lpp = NULL;
        return (-1);
      }
      *cp++ = c;
    }
    while ((c = *dn++) != '\0');

    /* catch trailing '.'s but not '..' */
    if ((l = cp - sp - 1) == 0 && c == '\0')
    {
      cp--;
      break;
    }
    if (l <= 0 || l > MAXLABEL)
    {
      if (msg)
         *lpp = NULL;
      return (-1);
    }
    *sp = l;
  }
  if (cp >= eob)
  {
    if (msg)
       *lpp = NULL;
    return (-1);
  }
  *cp++ = '\0';
  return (cp - comp_dn);
}

/*
 * Skip over a compressed domain name. Return the size or -1.
 */          
int __dn_skipname (const u_char *comp_dn, const u_char *eom)
{
  const u_char *cp = comp_dn;

  while (cp < eom)
  {
   int n = *cp++;
   if (!n)
      break;

    /* check for indirection
     */
    switch (n & INDIR_MASK)
    {
      case 0:           /* normal case, n == len */
           cp += n;
           continue;
      case INDIR_MASK:  /* indirection */
           cp++;
           break;
      default:          /* illegal type */
           return (-1);
    }
    break;
  }
  if (cp > eom)
     return (-1);
  return (cp - comp_dn);
}


static int mklower (int ch)
{
  if (isascii(ch) && isupper(ch))
     return (tolower(ch));
  return (ch);
}

/*
 * Search for expanded name from a list of previously compressed names.
 * Return the offset from msg if found or -1.
 * dnptrs is the pointer to the first name on the list,
 * not the pointer to the start of the message.
 */
static int dn_find (u_char *exp_dn, u_char *msg, u_char **dnptrs, u_char **lastdnptr)
{
  u_char **cpp;

  for (cpp = dnptrs; cpp < lastdnptr; cpp++)
  {
    u_char *dn = exp_dn;
    u_char *sp = *cpp;
    u_char *cp = *cpp;
    int     n;

    while ((n = *cp++) != 0)
    {
      /*
       * check for indirection
       */
      switch (n & INDIR_MASK)
      {
        case 0:    /* normal case, n == len */
             while (--n >= 0)
             {
               if (*dn == '.')
                  goto next;
               if (*dn == '\\')
                  dn++;
               if (mklower(*dn++) != mklower(*cp++))
                  goto next;
             }
             if ((n = *dn++) == '\0' && *cp == '\0')
                return (sp - msg);
             if (n == '.')
                continue;
             goto next;

        case INDIR_MASK:  /* indirection */
             cp = msg + (((n & 0x3f) << 8) | *cp);
             break;

        default:          /* illegal type */
             return (-1);
      }
    }
    if (*dn == '\0')
      return (sp - msg);
    next: ;
  }
  return (-1);
}
#endif /* USE_BIND */


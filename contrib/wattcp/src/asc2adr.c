/*
 * Copyright 1996 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that both the above copyright notice and this
 * permission notice appear in all copies, that both the above
 * copyright notice and this permission notice appear in all
 * supporting documentation, and that the name of M.I.T. not be used
 * in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  M.I.T. makes
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 * 
 * THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
 * ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
 * SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      $ANA: ascii2addr.c,v 1.2 1996/06/13 18:46:02 wollman Exp $
 *
 *  $Id: ascii2addr.c,v 1.2 1998/08/20 21:56:23 joel Exp $
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

int ascii2addr (int af, const char *ascii, void *result)
{
  struct in_addr *ina;
  char   strbuf [4*sizeof("123")];  /* long enough for V4 only */

  switch (af)
  {
    case AF_INET:
         ina = result;
         strbuf[0] = '\0';
         strncat (strbuf, ascii, sizeof(strbuf)-1);
         if (inet_aton (strbuf, ina))
            return sizeof (*ina);
         SOCK_ERR (EINVAL);
         break;

    case AF_LINK:
         link_addr (ascii, result);
         /* oops... no way to detect failure */
         return sizeof (struct sockaddr_dl);

    default:
         SOCK_ERR (EPROTONOSUPPORT);
         break;
  } 
  return (-1);
}
#endif

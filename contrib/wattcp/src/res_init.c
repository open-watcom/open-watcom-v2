/*
 * ++Copyright++ 1985, 1989, 1993
 * -
 * Copyright (c) 1985, 1989, 1993
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

/*----------------- info about "sortlist" -----------------------------
 *
 * Marc Majka    1994/04/16
 * Allan Nathanson  1994/10/29 (BIND 4.9.3.x)
 *
 * NetInfo resolver configuration directory support.
 *
 * Allow a NetInfo directory to be created in the hierarchy which
 * contains the same information as the resolver configuration file.
 *
 * - The local domain name is stored as the value of the "domain" property.
 * - The Internet address(es) of the name server(s) are stored as values
 *   of the "nameserver" property.
 * - The name server addresses are stored as values of the "nameserver"
 *   property.
 * - The search list for host-name lookup is stored as values of the
 *   "search" property.
 * - The sortlist comprised of IP address netmask pairs are stored as
 *   values of the "sortlist" property. The IP address and optional netmask
 *   should be seperated by a slash (/) or ampersand (&) character.
 * - Internal resolver variables can be set from the value of the "options"
 *   property.
 */

#undef  is_it
#define is_it(cp,x)      !strncmp(cp,x,sizeof(x)-1)

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK  (u_long)0x7F000001
#endif

char *res_cfg_options = NULL; /* resolver options */
char *res_cfg_aliases = NULL; /* hostaliases file */

/*
 * Resolver state default settings.
 */
struct __res_state _res = { 0 };

static void res_setoptions (char *options, char *source)
{
  char *cp = options;

  if (_res.options & RES_DEBUG)
     printf (";; res_setoptions(\"%s\", \"%s\")...\n", options, source);

  while (*cp)
  {
    /* skip leading and inner runs of spaces
     */
    while (*cp == ' ' || *cp == '\t')
          ++cp;

    /* search for and process individual options
     */
    if (is_it(cp,"ndots:"))
    {
      int i = atoi (cp + sizeof("ndots:") - 1);

      if (i <= RES_MAXNDOTS)
           _res.ndots = i;
      else _res.ndots = RES_MAXNDOTS;

      if (_res.options & RES_DEBUG)
         printf (";;\tndots=%d\n", _res.ndots);
    }
    else if (is_it(cp,"debug"))
    {
      if (!(_res.options & RES_DEBUG))
      {
        printf (";; res_setoptions(\"%s\", \"%s\")..\n", options, source);
        _res.options |= RES_DEBUG;
      }
      printf (";;\tdebug\n");
    }
    else
    {
      /* XXX - print a warning here? */
    }
    /* skip to next run of spaces
     */
    while (*cp && *cp != ' ' && *cp != '\t')
          cp++;
  }
}

u_int16_t res_randomid (void)
{
  return (0xFFFF & rand());
}


/*
 * Set up default settings.  If the configuration file exist, the values
 * there will have precedence.  Otherwise, the server address is set to
 * INADDR_ANY and the default domain name comes from the gethostname().
 *
 * An interrim version of this code (BIND 4.9, pre-4.4BSD) used 127.0.0.1
 * rather than INADDR_ANY ("0.0.0.0") as the default name server address
 * since it was noted that INADDR_ANY actually meant ``the first interface
 * you "ifconfig"'d at boot time'' and if this was a SLIP or PPP interface,
 * it had to be "up" in order for you to reach your own name server.  It
 * was later decided that since the recommended practice is to always 
 * install local static routes through 127.0.0.1 for all your network
 * interfaces, that we could solve this problem without a code change.
 *
 * The configuration file should always be used, since it is the only way
 * to specify a default domain.  If you are running a server on your local
 * machine, you should say "nameserver 0.0.0.0" or "nameserver 127.0.0.1"
 * in the configuration file.
 *
 * Return 0 if completes successfully, -1 on error
 */
int res_init (void)
{
  char *cp;
  char  dom[MAX_HOSTLEN];
  char  buf[BUFSIZ];
  int   nserv;

  sock_init();

  /*
   * These three fields used to be statically initialized.  This made
   * it hard to use this code in a shared library.  It is necessary,
   * now that we're doing dynamic initialization here, that we preserve
   * the old semantics: if an application modifies one of these three
   * fields of _res before res_init() is called, res_init() will not
   * alter them.  Of course, if an application is setting them to
   * _zero_ before calling res_init(), hoping to override what used
   * to be the static default, we can't detect it and unexpected results
   * will follow.  Zero for any of these fields would make no sense,
   * so one can safely assume that the applications were already getting
   * unexpected results.
   *
   * _res.options is tricky since some apps were known to diddle the bits
   * before res_init() was first called. We can't replicate that semantic
   * with dynamic initialization (they may have turned bits off that are
   * set in RES_DEFAULT).  Our solution is to declare such applications
   * "broken".  They could fool us by setting RES_INIT but none do (yet).
   */
  if (!_res.retrans)              _res.retrans = RES_TIMEOUT;
  if (!_res.retry)                _res.retry   = 4;
  if (!(_res.options & RES_INIT)) _res.options = RES_DEFAULT;

  /*
   * This one used to initialize implicitly to zero, so unless the app
   * has set it to something in particular, we can randomize it now.
   */

  if (_res.id == 0)
      _res.id = res_randomid();

  _res.nsaddr.sin_addr.s_addr = INADDR_ANY;
  _res.nsaddr.sin_family      = AF_INET;
  _res.nsaddr.sin_port        = htons (NAMESERVER_PORT);
  _res.nscount   = 0;
  _res.ndots     = 1;
  _res.pfcode    = 0;
  _res.dnsrch[0] = _res.defdname;
  _res.dnsrch[1] = NULL;

  if (getdomainname (dom,sizeof(dom)) == 0)
       strncpy (_res.defdname, dom, sizeof(_res.defdname)-1);
  else _res.defdname[0] = 0;

  for (nserv = 0; nserv < MAXNS; nserv++)
  {
    _res.nsaddr_list[nserv].sin_family = AF_INET;
    _res.nsaddr_list[nserv].sin_port   = htons (NAMESERVER_PORT);
    if (nserv < last_nameserver)
    {
      _res.nsaddr_list[nserv].sin_addr.s_addr = htonl (def_nameservers[nserv]);
      _res.nscount++;
    }
    else
      _res.nsaddr_list[nserv].sin_addr.s_addr = htonl (INADDR_LOOPBACK);
  }

  if (_res.options & RES_DEBUG)
  {
    int i;
    for (i = 1; i <= _res.nscount; i++)
        printf ("Nameserv #%d: %s (%d)\n",
                i, inet_ntoa(_res.nsaddr_list[i-1].sin_addr), NAMESERVER_PORT);
  }

  if (_res.defdname[0] == 0 &&
      gethostname (buf,sizeof(_res.defdname)-1) == 0 &&
      (cp = strchr(buf, '.')) != NULL)
    strcpy (_res.defdname, cp+1);

  cp = getenv ("RES_OPTIONS");
  if (!cp)
     cp = res_cfg_options;
  if (cp)
     res_setoptions (cp, "env");

  _res.options |= RES_INIT;
  return (0);
}

/*
 * Configure hook routine for bind resolver. Must be called
 * before res_init() (via sock_init).
 */
static void (*prev_hook) (const char*, const char*);

static struct config_table bind_cfg[] = {
              { "RES_OPTIONS", ARG_STRDUP, (void*)&res_cfg_options },
              { "HOSTALIASES", ARG_STRDUP, (void*)&res_cfg_aliases },
              { NULL }
            };

static void resolver_init (const char *name, const char *value)
{
  if (!parse_config_table(&bind_cfg[0], "BIND.", name, value) && prev_hook)
     (*prev_hook) (name, value);
}

void res_init0 (void)
{
  prev_hook = usr_init;
  usr_init  = resolver_init;
}
#endif /* USE_BIND */


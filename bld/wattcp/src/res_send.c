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

/*
 * 10.Dec-97   Adapted for Waterloo TCP/IP - G. Vanem (giva@bgnett.no)
 *
 */

/*
 * Send query to name server and wait for reply.
 */

#include "resolver.h"

#if defined(USE_BIND)

static sock_type *sock = NULL;  /* socket used for communications */
static int vc          = 0;     /* is the socket a virtual ciruit? */
static int connected   = 0;     /* is the socket connected */

static int  name_server_send (int ns, struct sockaddr_in *nsap);
static void resolve_close    (void);

#define SAME_NS (-1)
#define NEXT_NS (-2)


#define Dprint(cond,args) if (cond) do {  \
                             printf args; \
                           } while (0)

#define DprintQ(cond,args,query,size)                       \
                          if (cond) do {                    \
                             printf args;                   \
                             __fp_nquery(query,size,stdout);\
                          } while (0)

static void Aerror (const char *str, const char *error,
                    struct sockaddr_in address)
{
  if (_res.options & RES_DEBUG)
     fprintf (stderr, "res_send: %s (%s/%u): %s\n",
              str, inet_ntoa(address.sin_addr), ntohs(address.sin_port),
              error);
}

static void Perror (const char *str, const char *error)
{
  if (_res.options & RES_DEBUG)
     fprintf (stderr, "res_send: %s: %s\n", str, error);
}

static res_send_qhook Qhook = NULL;
static res_send_rhook Rhook = NULL;

void res_send_setqhook (res_send_qhook hook)
{
  Qhook = hook;
}

void res_send_setrhook (res_send_rhook hook)
{
  Rhook = hook;
}

/*
 * int res_isourserver(ina)
 *    looks up "ina" in _res.ns_addr_list[]
 * returns:
 *    0 : not found
 *   >0 : found
 * author:
 *    paul vixie, 29may94
 */
int res_isourserver (const struct sockaddr_in *inp)
{
  struct sockaddr_in ina = *inp;
  int    ns;

  for (ns = 0; ns < _res.nscount; ns++)
  {
    const struct sockaddr_in *srv = &_res.nsaddr_list[ns];

    if (srv->sin_family == ina.sin_family &&
        srv->sin_port   == ina.sin_port   &&
        (srv->sin_addr.s_addr == INADDR_ANY ||
         srv->sin_addr.s_addr == ina.sin_addr.s_addr))
      return (1);
  }
  return (0);
}

/*
 * int res_nameinquery(name, type, class, buf, eom)
 *    look for (name,type,class) in the query section of packet (buf,eom)
 * returns:
 *    -1 : format error
 *     0 : not found
 *    >0 : found
 * author:
 *     paul vixie, 29may94
 */
int res_nameinquery (const char *name, int type, int class,
                     const u_char *buf, const u_char *eom)
{
  const u_char *cp = buf + HFIXEDSZ;
  int   qdcount    = ntohs (((HEADER*)buf)->qdcount);

  while (qdcount-- > 0)
  {
    char tname[MAXDNAME+1];
    int n, ttype, tclass;

    n = dn_expand (buf, eom, cp, tname, sizeof tname);
    if (n < 0)
       return (-1);
    cp += n;
    ttype  = _getshort (cp);
    cp    += INT16SZ;
    tclass = _getshort (cp);
    cp    += INT16SZ;
    if (ttype == type && tclass == class && !stricmp(tname,name))
       return (1);
  }
  return (0);
}


/*
 * int res_queriesmatch(buf1, eom1, buf2, eom2)
 *    is there a 1:1 mapping of (name,type,class)
 *    in (buf1,eom1) and (buf2,eom2)?
 * returns:
 *    -1 : format error
 *     0 : not a 1:1 mapping
 *    >0 : is a 1:1 mapping
 * author:
 *     paul vixie, 29may94
 */
int res_queriesmatch (const u_char *buf1, const u_char *eom1,
                      const u_char *buf2, const u_char *eom2 )
{
  const u_char *cp = buf1 + HFIXEDSZ;
  int   qdcount    = ntohs (((HEADER*)buf1)->qdcount);

  if (qdcount != ntohs(((HEADER*)buf2)->qdcount))
     return (0);

  while (qdcount-- > 0)
  {
    char tname[MAXDNAME+1];
    int n, ttype, tclass;

    n = dn_expand (buf1, eom1, cp, tname, sizeof tname);
    if (n < 0)
       return (-1);
    cp += n;
    ttype  = _getshort (cp);
    cp    += INT16SZ;
    tclass = _getshort (cp);
    cp    += INT16SZ;
    if (!res_nameinquery(tname, ttype, tclass, buf2, eom2))
       return (0);
  }
  return (1);
}

/*--------------------------------------------------------------------*/

static HEADER *hp, *anhp;
static int     gotsomewhere, connreset, terrno, try;
static int     v_circuit, ns, n;
static u_long  badns;
static u_char *ns_buf,   *ns_ans;
static int     ns_buflen, ns_anssiz;

int res_send (const u_char *buf, int buflen, u_char *ans, int anssiz)
{
  hp   = (HEADER *) buf;
  anhp = (HEADER *) ans;

  ns_buf    = (u_char*)buf;
  ns_ans    = ans;
  ns_buflen = buflen;
  ns_anssiz = anssiz;

  if ((_res.options & RES_INIT) == 0 && res_init() == -1)
  {
    /* errno should have been set by res_init() in this case
     */
    return (-1);
  }
  DprintQ ((_res.options & RES_DEBUG) || (_res.pfcode & RES_PRF_QUERY),
           (";; res_send()\n"), buf, buflen);

  v_circuit    = (_res.options & RES_USEVC) || buflen > PACKETSZ;
  gotsomewhere = 0;
  connreset    = 0;
  terrno       = ETIMEDOUT;
  badns        = 0;

  /* Send request, RETRY times, or until successful
   */
  for (try = 0; try < _res.retry; try++)
  {
    for (ns = 0; ns < _res.nscount; ns++)
    {
      struct sockaddr_in *nsap = &_res.nsaddr_list[ns];
      int    rc;

      do
        rc = name_server_send (ns, nsap);
      while (rc == SAME_NS);

      if (rc != NEXT_NS)
         return (rc);
    }
  }

  resolve_close();
  if (!v_circuit)
  {
    if (!gotsomewhere)
         SOCK_ERR (ECONNREFUSED); /* no nameservers found */
    else SOCK_ERR (ETIMEDOUT);    /* no answer obtained */
  }
  else
    SOCK_ERR (terrno);
  return (-1);
}

/*------------------------------------------------------------------------*/

#if defined(_MSC_VER)
  #define ERROR_TYPE int volatile /* because '&errno' is used below */
#else
  #define ERROR_TYPE int
#endif

static int tcp_conn (tcp_Socket *sock, ERROR_TYPE *error, DWORD timeout)
{
  DWORD timer  = set_timeout (timeout);
  int   status = _ip_delay0 ((sock_type*)sock, (int)timeout, NULL, NULL);

  if (status == -1)
  {
    *error = chk_timeout(timer) ? ETIMEDOUT : ECONNREFUSED;
    return (0);
  }
  *error = 0;
  return (1);
}

/*------------------------------------------------------------------------*/

static int tcp_read (tcp_Socket *sock, u_char *buf, int len,
                     ERROR_TYPE *error, DWORD timeout)
{
  int status = _ip_delay1 ((sock_type*)sock, (int)timeout, NULL, NULL);

  if (status == -1)
  {
    *error = ETIMEDOUT;
    return (0);
  }
  if (status == 1)
  {
    *error = ECONNRESET;
    return (0);
  }
  *error = 0;
  return sock_fastread ((sock_type*)sock, (BYTE*)buf, len);
}

/*------------------------------------------------------------------------*/

static int udp_read (udp_Socket *sock, u_char *buf, int len,
                     ERROR_TYPE *error, DWORD timeout)
{
  int status = _ip_delay1 ((sock_type*)sock, (int)timeout, NULL, NULL);

  if (status == -1)
  {
    *error = ETIMEDOUT;
    return (0);
  }
  *error = 0;
  return sock_fastread ((sock_type*)sock, (BYTE*)buf, len);
}

/*------------------------------------------------------------------------*/

static int name_server_send (int ns, struct sockaddr_in *nsap)
{
  int resplen = 0;

  if (badns & (1 << ns))  /* this NameServer already marked bad */
  {
    resolve_close();
    return (NEXT_NS);
  }

  if (Qhook)
  {
    int done = 0;
    int loops = 0;
    do
    {
      res_sendhookact act = (*Qhook)(&nsap, (const u_char**)&ns_buf,
                                     &ns_buflen, ns_ans, ns_anssiz,
                                     &resplen);
      switch (act)
      {
        case res_goahead:
             done = 1;
             break;
        case res_nextns:
             resolve_close();
             return (NEXT_NS);
        case res_done:
             return (resplen);
        case res_modified:
             /* give the hook another try */
             if (++loops < 42)
                break;
             /* fallthrough */
        case res_error:
             /* fallthrough */
        default:
             return (-1);
      }
    }
    while (!done);
  }

  Dprint (_res.options & RES_DEBUG,
          (";; Querying server (# %d) address = %s\n",
           ns + 1, inet_ntoa(nsap->sin_addr)));

  if (v_circuit)  /* i.e. TCP */
  {
    int     truncated;
    u_short len;
    u_char *cp;

    /* Use virtual circuit; at most one attempt per server.
     */
    try = _res.retry;
    truncated = 0;
    if (!sock || !vc)
    {
      DWORD his_ip   = ntohl (nsap->sin_addr.s_addr);
      WORD  his_port = ntohs (nsap->sin_port);

      if (sock)
         resolve_close();

      sock = malloc (sizeof(tcp_Socket));
      if (!sock)
      {
        Perror ("malloc(vc)", "no memory");
        return (-1);
      }

      if (!tcp_open(&sock->tcp,0,his_ip,his_port,NULL) ||
          !tcp_conn(&sock->tcp,&errno,dns_timeout))
      {
        Aerror ("tcp_open/vc", "failed/timeout", *nsap);
        badns |= (1 << ns);
        resolve_close();
        return (NEXT_NS);
      }
      vc = 1;
    }

    /* Send length & message
     */
    {
      BYTE *send_buf = alloca (INT16SZ + ns_buflen);

      PUTSHORT (ns_buflen, send_buf);
      memcpy (&send_buf[INT16SZ],ns_buf,ns_buflen);
      if (sock_write(sock,send_buf,INT16SZ+ns_buflen) != INT16SZ+ns_buflen)
      {
        Perror ("sock_write() failed", sockerr(&sock->tcp));
        badns |= (1 << ns);
        resolve_close();
        return (NEXT_NS);
      }
    }

    /* Receive length & response
     */
    cp  = ns_ans;
    len = INT16SZ;
    while ((n = tcp_read(&sock->tcp,cp,len,&errno,dns_timeout)) > 0)
    {
      cp += n;
      if ((len -= n) <= 0)
         break;
    }
    if (n <= 0)
    {
      Perror ("tcp_read() failed", sockerr(&sock->tcp));
      resolve_close();
      return (NEXT_NS);
    }
    resplen = _getshort (ns_ans);
    if (resplen > ns_anssiz)
    {
      Dprint(_res.options & RES_DEBUG,(";; response truncated\n"));
      truncated = 1;
      len = ns_anssiz;
    }
    else
      len = resplen;

    cp = ns_ans;
    while (len && (n = tcp_read(&sock->tcp,cp,len,&errno,dns_timeout)) > 0)
    {
      cp  += n;
      len -= n;
    }
    if (n <= 0)
    {
      Perror ("tcp_read(vc)",sockerr(&sock->tcp));
      resolve_close();
      return (NEXT_NS);
    }
    if (truncated)
    {
      /* Flush rest of answer so connection stays in synch.
       */
      anhp->tc = 1;
      len = resplen - ns_anssiz;
      while (len)
      {
        u_char junk[PACKETSZ];

        n = (len > sizeof(junk) ? sizeof(junk) : len);
        n = tcp_read (&sock->tcp,junk,n,&errno,dns_timeout);
        if (n > 0)
             len -= n;
        else break;
      }
    }
  }
  else  /* !v_circuit, i.e. UDP */
  {
    DWORD timeout;

    if (!sock || vc)
    {
      if (vc)
         resolve_close();

      sock = malloc (sizeof(udp_Socket));
      if (!sock)
      {
        Perror ("malloc(dg)", "no memory");
        return (-1);
      }
      connected = 0;
    }

    /* Connect only if we are sure we won't
     * receive a response from another server.
     */
    if (!connected)
    {
      DWORD his_ip   = ntohl (nsap->sin_addr.s_addr);
      WORD  his_port = ntohs (nsap->sin_port);

      if (!udp_open(&sock->udp,0,his_ip,his_port,NULL))
      {
        Aerror ("connect/dg", "ARP failed", *nsap);
        badns |= (1 << ns);
        resolve_close();
        return (NEXT_NS);
      }
      connected = 1;
    }
    if (sock_write(sock,(BYTE*)ns_buf,ns_buflen) != ns_buflen)
    {
      Perror ("sock_write() failed", "");
      badns |= (1 << ns);
      resolve_close();
      return (NEXT_NS);
    }

    /* Wait for reply
     */
    timeout = (unsigned)_res.retrans << try;
    if (try > 0)
       timeout /= _res.nscount;
    if ((long)timeout <= 0)
       timeout = 1;

  wait:

    n = udp_read (&sock->udp, ns_ans, ns_anssiz, &errno, timeout);
    if (n == 0)
    {
      Dprint (_res.options & RES_DEBUG, (";; timeout\n"));
      gotsomewhere = 1;
      resolve_close();
      return (NEXT_NS);
    }
    gotsomewhere = 1;
    if (hp->id != anhp->id)
    {
      /* response from old query, ignore it.
       * XXX - potential security hazard could be detected here.
       */
      DprintQ ((_res.options & RES_DEBUG) || (_res.pfcode & RES_PRF_REPLY),
               (";; old answer:\n"), ns_ans, resplen);
      goto wait;
    }

    if (!(_res.options & RES_INSECURE2) &&
        !res_queriesmatch(ns_buf, ns_buf+ns_buflen, ns_ans, ns_ans+ns_anssiz))
    {
      /* response contains wrong query? ignore it.
       * XXX - potential security hazard could be detected here.
       */
      DprintQ ((_res.options & RES_DEBUG) || (_res.pfcode & RES_PRF_REPLY),
               (";; wrong query name:\n"), ns_ans, resplen);
      goto wait;
    }
    if (anhp->rcode == SERVFAIL ||
        anhp->rcode == NOTIMP   ||
        anhp->rcode == REFUSED)
    {
      DprintQ (_res.options & RES_DEBUG,("server rejected query:\n"),
               ns_ans,resplen);
      badns |= (1 << ns);
      resolve_close();

      /* don't retry if called from dig */
      if (!_res.pfcode)
         return (NEXT_NS);
    }
    if (!(_res.options & RES_IGNTC) && anhp->tc)
    {
      /* get rest of answer; use TCP with same server.
       */
      Dprint (_res.options & RES_DEBUG, (";; truncated answer\n"));
      v_circuit = 1;
      resolve_close();
      return (SAME_NS);
    }
  } /* if vcicuit / dg */

  Dprint ((_res.options & RES_DEBUG) ||
          ((_res.pfcode & RES_PRF_REPLY) && (_res.pfcode & RES_PRF_HEAD1)),
          (";; got answer:\n"));

  DprintQ ((_res.options & RES_DEBUG) || (_res.pfcode & RES_PRF_REPLY),
           (" \b"), ns_ans, resplen);

  /*
   * If using virtual circuits (TCP), we assume that the first server
   * is preferred over the rest (i.e. it is on the local machine) and
   * only keep that one open. If we have temporarily opened a virtual
   * circuit, or if we haven't been asked to keep a socket open,
   * close the socket.
   */
  if ((v_circuit && (!(_res.options & RES_USEVC) || ns != 0)) ||
      !(_res.options & RES_STAYOPEN))
     resolve_close();

  if (Rhook)
  {
    int done = 0, loops = 0;

    do
    {
      res_sendhookact act = (*Rhook)(nsap, ns_buf, ns_buflen,
                                     ns_ans, ns_anssiz, &resplen);
      switch (act)
      {
        case res_goahead:
        case res_done:
             done = 1;
             break;
        case res_nextns:
             resolve_close();
             return (NEXT_NS);
        case res_modified:
             /* give the hook another try */
             if (++loops < 42)
                break;
             /* fallthrough */
        case res_error:
             /* fallthrough */
        default:
             return (-1);
      }
    }
    while (!done);
  }
  return (resplen);
}

/*
 * This routine is for closing the socket if a virtual circuit is used and
 * the program wants to close it.  This provides support for endhostent()
 * which expects to close the socket.
 *
 * This routine is not expected to be user visible.
 */
static void resolve_close (void)
{
  if (sock)
  {
    if (sock->tcp.ip_type == TCP_PROTO &&
        sock->tcp.state < tcp_StateCLOSED)
    {
      sock_close (sock);
      sock_abort (sock);
    }
    free (sock);
    sock = NULL;
    connected = 0;
    vc = 0;
  }
}
#endif /* USE_BIND */


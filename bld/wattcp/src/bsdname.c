#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/werrno.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "udp_dom.h"
#include "pcbuf.h"
#include "pctcp.h"
#include "wdpmi.h"     /* for DOS4GW/WDOSX */
#include "misc.h"
#include "bsdname.h"

int _getpeername (const tcp_Socket *s, void *dest, int *len)
{
  struct watt_sockaddr temp;
  int    ltemp;

  memset (&temp, 0, sizeof(temp));
  temp.s_ip   = s->hisaddr;
  temp.s_port = s->hisport;

  if (!s->hisaddr || !s->hisport || !_chk_socket((sock_type*)s))
  {
    if (len)
       *len = 0;
    return (-1);
  }

  /* how much do we move
   */
  ltemp = (len ? *len : sizeof (struct watt_sockaddr));
  if (ltemp > sizeof (struct watt_sockaddr))
      ltemp = sizeof (struct watt_sockaddr);
  memcpy (dest, &temp, ltemp);

  if (len)
     *len = ltemp;
  return (0);
}

int _getsockname (const tcp_Socket *s, void *dest, int *len)
{
  struct watt_sockaddr temp;
  int    ltemp;

  memset (&temp, 0, sizeof(temp));
  temp.s_ip   = s->myaddr;
  temp.s_port = s->myport;

  if (!s->hisaddr || !s->hisport || !_chk_socket((sock_type*)s))
  {
    if (len)
       *len = 0;
    return (-1);
  }

  /* how much do we move
   */
  ltemp = (len ? *len : sizeof (struct watt_sockaddr));
  if (ltemp > sizeof (struct watt_sockaddr))
      ltemp = sizeof (struct watt_sockaddr);
  memcpy (dest, &temp, ltemp);

  if (len)
     *len = ltemp;
  return (0);
}

/*
 * Return the host's domain name.
 * As per BSD spec, the resultant buffer ...
 * ... "will be null-terminated _unless_there_is_insufficient_space_";
 *
 * Set errno on failure and return -1.
 */
int getdomainname (char *buffer, int buflen)
{
  const char *my_domainname = "";

  if (!buffer || buflen < 0)
  {
    errno = EINVAL;
    return (-1);
  }
#if (DOSX)
  if (!valid_addr((DWORD)buffer, buflen))
  {
    errno = EFAULT;
    return (-1);
  }
#endif

  if (def_domain)
     my_domainname = def_domain;
  strncpy (buffer, my_domainname, buflen);
  /* no terminating '\0' needs to be forced here per bsd spec */
  return (0);
}

/*
 * Set the host's domain name.
 * Set errno on failure and return -1.
 */
int setdomainname (const char *name, int len)
{
  if (!name || len < 0 || len > sizeof(defaultdomain)-1)
  {
    errno = EINVAL;
    return (-1);
  }

#if (DOSX)
  if (!valid_addr((DWORD)name, len))
  {
    errno = EFAULT;
    return (-1);
  }
#endif

  def_domain = strcpy (defaultdomain, name);
  return (0);
}

/*
 * Make a FQDN from `hostname' & `def_domain'.
 * Set errno on failure and return -1.
 */                                                   
int gethostname (char *buffer, int buflen)
{
  /* the fqdn when no hostname has been set is "localhost.localdomain".
   * the fqdn when a hostname has been set, but no domname is set, is 'hostname'
   * the fqdn when both are set is 'hostname.domname'
   */
  const char *my_hostname = "localhost";
  const char *my_domname  = "localdomain";
  int   pos;

  if (!buffer || buflen < 0)
  {
    errno = EINVAL;
    return (-1);
  }
  if (buflen == 0)
     return (0);

#if (DOSX)
  if (!valid_addr((DWORD)buffer, buflen))
  {
    errno = EFAULT;
    return (-1);
  }
#endif

  if (*hostname) /* otherwise use localhost.localdomain */
  {
    my_hostname = hostname;
    my_domname  = NULL;         /* have hostname but no domname */
    if (def_domain && *def_domain)
       my_domname = def_domain;
  }
  pos = 0;
  while (pos < buflen && *my_hostname)
      buffer[pos++] = *my_hostname++;

  if (pos < buflen && my_domname)
  {
    buffer[pos++] = '.';
    /*@-nullderef@*/
    while (pos < buflen && *my_domname)
        buffer[pos++] = *my_domname++;
  }
  if (pos < buflen)
     buffer[pos] = '\0';
  return (0);
}

/*
 * Expects a "Fully Qualified Domain Name" in `fqdn'.
 * Split at first `.' and extract `hostname' and `def_domain'.
 * Set errno on failure and return -1.
 */
int sethostname (const char *fqdn, int len)
{
  int pos;

  if (!fqdn || !*fqdn || len < 0 || len > MAX_HOSTLEN)
  {
    errno = EINVAL;
    return (-1);
  }

#if (DOSX)
  if (!valid_addr((DWORD)fqdn, len))
  {
    errno = EFAULT;
    return (-1);
  }
#endif

  pos = 0;
  while (pos < len && fqdn[pos] != '.')
  {
    if (!fqdn[pos]) /* should do complete alpha/digit/underscore check here */
    {
      pos = 0;
      break;
    }
    pos++;
  }
  if (pos == 0) /* leading '.' or invalid char in name */
  {
    errno = EINVAL;
    return (-1);
  }
  if (pos >= ((int)sizeof(hostname)))
  {
    errno = ERANGE;
    return (-1);
  }
  if (fqdn[pos] == '.') /* a trailing '.' is ok too */
  {
    if (setdomainname(&fqdn[pos+1], len-pos) != 0)
       return (-1);
  }
  memcpy(hostname, fqdn, pos);
  hostname[pos] = '\0';
  return (0);
}


/*
 * From Ralf Brown's interrupt list
 *
 * --------D-215E00-----------------------------
 * INT 21 - DOS 3.1+ network - GET MACHINE NAME
 *         AX = 5E00h
 *         DS:DX -> 16-byte buffer for ASCII machine name
 * Return: CF clear if successful
 *             CH = validity
 *                 00h name invalid
 *                 nonzero valid
 *                     CL = NetBIOS number for machine name
 *                     DS:DX buffer filled with blank-paded name
 *         CF set on error
 *             AX = error code (01h) (see #1366 at AH=59h)
 * Note:   supported by OS/2 v1.3+ compatibility box, PC-NFS
 */

/* Copyright (C) 1997 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */

/*
 * Try asking a LAN extension of DOS for a host-name.
 */
int _get_machine_name (char *buf, int size)
{
  char *h, dosbuf[16];
  int   len;

#if (DOSX & DJGPP)
  __dpmi_regs r;

  r.x.ax = 0x5E00;
  r.h.ch = 0;         /* Try to detect overloading of 0x5e00 */
  r.x.dx = __tb & 15;
  r.x.ds = __tb / 16;
  __dpmi_int (0x21, &r);
  if ((r.x.flags & 1) || r.h.ch == 0)
     return (-1);

  dosmemget (__tb, sizeof(dosbuf), dosbuf);

#elif (DOSX & PHARLAP)
  SWI_REGS r;

  if (_watt_dosTbSize < sizeof(dosbuf) || !_watt_dosTbr)
     return (-1);

  r.edx = RP_OFF (_watt_dosTbr);
  r.ds  = RP_SEG (_watt_dosTbr);
  r.eax = 0x5E00;
  r.ecx = 0;
  _dx_real_int (0x21, &r);
  if ((r.flags & 1) || hiBYTE(r.ecx) == 0) /* Carry set or CH == 0 */
     return (-1);

  ReadFarMem ((void*)&dosbuf, _watt_dosTbp, sizeof(dosbuf));

#elif (DOSX & (DOS4GW|WDOSX))
  struct DPMI_regs r;

  if (_watt_dosTbSize < sizeof(dosbuf) || !_watt_dosTbSeg)
     return (-1);

  r.r_ds = _watt_dosTbSeg;
  r.r_dx = 0;
  r.r_ax = 0x5E00;
  r.r_cx = 0;
  if (!dpmi_real_interrupt(0x21, &r) || hiBYTE(r.r_cx) == 0)
     return (-1);

  memcpy (&dosbuf, SEG_OFS_TO_LIN(_watt_dosTbSeg,0), sizeof(dosbuf));

#elif (DOSX & POWERPAK)
  UNFINISHED();
  strcpy (dosbuf, "random-pc");

#elif (DOSX == 0)
  union  REGS  r;
  struct SREGS s;

  r.x.ax = 0x5E00;
  r.h.ch = 0;
  r.x.dx = FP_OFF (dosbuf);
  s.ds   = FP_SEG (dosbuf);
  int86x (0x21, &r, &r, &s);
  if (r.x.cflag || r.h.ch == 0)
     return (-1);

#else
  #error Unknown target!
#endif

  /* Remove right space padding
   */
  h = dosbuf + strlen (dosbuf);
  while (h > dosbuf && h[-1] == ' ')
        h--;

  *h  = 0;
  h   = dosbuf;
  len = strlen (h);
  if (len + 1 > size)
  {
    errno = ERANGE;
    return (-1);
  }
  strcpy (buf, h);
  return (0);
}

/* so_ioctl.h derived from BSD's ioctl.h by hv and em 1994
 *
 * Copyright (c) 1982, 1986, 1990 The Regents of the University of California.
 * All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *
 *	from: @(#)ioctl.h	7.19 (Berkeley) 6/26/91
 */

#ifndef __SYS_SO_IOCTL_H
#define __SYS_SO_IOCTL_H

#define _IOC(a,b) ((a<<8)|b)
#define _IOW(a,b,c) _IOC(a,b)
#define _IOR(a,b,c) _IOC(a,b)
#define _IOWR(a,b,c) _IOC(a,b)

#define _TCPIP_FIONREAD _IOC('f', 127)
#define FIONBIO         _IOC('f', 126)
#define FIOASYNC        _IOC('f', 125)
#define FIOTCPCKSUM     _IOC('f', 128)
#define FIONSTATUS      _IOC('f', 120)
#define FIONURG         _IOC('f', 121)

/* socket i/o controls */
#define SIOCSHIWAT      _IOW('s',  0, int)	/* set high watermark */
#define SIOCGHIWAT      _IOR('s',  1, int)	/* get high watermark */
#define SIOCSLOWAT      _IOW('s',  2, int)	/* set low watermark */
#define SIOCGLOWAT      _IOR('s',  3, int)	/* get low watermark */
#define SIOCATMARK      _IOR('s',  7, int)	/* at oob mark? */
#define SIOCSPGRP       _IOW('s',  8, int)	/* set process group */
#define SIOCGPGRP       _IOR('s',  9, int)	/* get process group */

#define SIOCADDRT       _IOW('r', 10, struct ortentry)	/* add route */
#define SIOCDELRT       _IOW('r', 11, struct ortentry)	/* delete route */

#define SIOCSIFADDR     _IOW('i', 12, struct ifreq)	/* set ifnet address */
#define SIOCGIFADDR     _IOWR('i',13, struct ifreg)	/* get ifnet addres */
#define OSIOCGIFADDR	SIOCGIFADDR
#define SIOCSIFDSTADDR  _IOW('i', 14, struct ifreq)	/* set p-p address */
#define SIOCGIFDSTADDR  _IOWR('i',15, struct ifreq)	/* get p-p address */
#define OSIOCGIFDSTADDR	SIOCGIFDSTADDR
#define SIOCSIFFLAGS    _IOW('i', 16, struct ifreq)	/* set ifnet flags */
#define SIOCGIFFLAGS    _IOWR('i',17, struct ifreq)	/* get ifnet flags */
#define SIOCGIFBRDADDR  _IOWR('i',18, struct ifreq)	/* get broadcast addr */
#define OSIOCGIFBRDADDR	SIOCGIFBRDADDR
#define SIOCSIFBRDADDR  _IOW('i', 19, struct ifreq)	/* set broadcast addr */
#define SIOCGIFCONF     _IOWR('i',20, struct ifreq)	/* get ifnet list */
#define OSIOCGIFCONF	SIOCGIFCONF
#define SIOCGIFNETMASK  _IOWR('i',21, struct ifreq)	/* get net addr mask */
#define OSIOCGIFNETMASK	SIOCGIFNETMASK
#define SIOCSIFNETMASK  _IOW('i', 22, struct ifreq)	/* set net addr mask */
#define SIOCGIFMETRIC   _IOWR('i',23, struct ifreq)	/* get if metric */
#define SIOCSIFMETRIC   _IOW('i', 24, struct ifreq)	/* set if metric */

#define SIOCSARP        _IOW('i', 30, struct arpreq)	/* set arp entry */
#define SIOCGARP        _IOWR('i',31, struct arpreq)	/* get arp entry */
#define OSIOCGARP	SIOCGARP
#define SIOCDARP        _IOW('i', 32, struct arpreq)	/* delete arp entry */

#endif

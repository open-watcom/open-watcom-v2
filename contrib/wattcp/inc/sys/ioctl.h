/*-
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
 *	@(#)ioctl.h	7.19 (Berkeley) 6/26/91
 */

#ifndef __SYS_IOCTL_H
#define __SYS_IOCTL_H

/*
 * Commands for ioctlsocket(),  taken from the BSD file fcntl.h.
 *
 *
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */

#define IOCPARM_MASK 0x7f               /* parameters must be < 128 bytes */

#define	IOCPARM_LEN(x)	(((x) >> 16) & IOCPARM_MASK)
#define	IOCBASECMD(x)	((x) & ~IOCPARM_MASK)
#define	IOCGROUP(x)	(((x) >> 8) & 0xff)

#define	IOCPARM_MAX	4096		/* max size of ioctl */
#define	IOC_VOID	0x20000000	/* no parameters */
#define	IOC_OUT		0x40000000	/* copy out parameters */
#define	IOC_IN		0x80000000	/* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)  /* 0x20000000 distinguishes new &
                                             old ioctl's */
#define	IOC_DIRMASK	0xe0000000	/* mask for IN/OUT/VOID */

#define _IO(x,y)    (IOC_VOID|(x<<8)|y)
#define _IOR(x,y,t) (IOC_OUT|(((unsigned long)sizeof(t)&IOCPARM_MASK)<<16)|(x<<8)|y)
#define _IOW(x,y,t) (IOC_IN|(((unsigned long)sizeof(t)&IOCPARM_MASK)<<16)|(x<<8)|y)
/* this should be _IORW, but stdio got there first */
#define _IOWR(x,y,t) (IOC_INOUT|(((unsigned long)sizeof(t)&IOCPARM_MASK)<<16)|(x<<8)|y)

/* 
 * file i/o controls
 */
#define	FIOCLEX		_IO('f', 1)	       /* set close on exec on fd */
#define	FIONCLEX	_IO('f', 2)	       /* remove close on exec */
#define FIONREAD        _IOR('f', 127, int)    /* get # bytes to read */
#define FIONBIO         _IOW('f', 126, int)    /* set/clear non-blocking i/o */
#define FIOASYNC        _IOW('f', 125, int)    /* set/clear async i/o */
#define FIOSETOWN       _IOW('f', 124, int)    /* set owner (struct Task *) */
#define FIOGETOWN       _IOR('f', 123, int)    /* get owner (struct Task *) */

/* 
 * socket i/o controls
 *
 * SIOCSPGRP and SIOCGPGRP are identical to the FIOSETOWN and FIOGETOWN,
 * respectively.
 */
#define SIOCSPGRP       _IOW('s',  8, int)    /* set process group */
#define SIOCGPGRP       _IOR('s',  9, int)    /* get process group */

#ifdef BSD
#define	SIOCADDRT	_IOW('r', 10, struct ortentry) /* add route */
#define	SIOCDELRT	_IOW('r', 11, struct ortentry) /* delete route */

#define	SIOCSIFADDR	_IOW('I', 12, struct ifreq)    /* set ifnet address */
#define	OSIOCGIFADDR	_IOWR('I',13, struct ifreq)    /* get ifnet address */
#define	SIOCGIFADDR	_IOWR('I',33, struct ifreq)    /* get ifnet address */
#define	SIOCSIFDSTADDR	_IOW('I', 14, struct ifreq)    /* set p-p address */
#define	OSIOCGIFDSTADDR	_IOWR('I',15, struct ifreq)    /* get p-p address */
#define	SIOCGIFDSTADDR	_IOWR('I',34, struct ifreq)    /* get p-p address */
#define	SIOCSIFFLAGS	_IOW('I', 16, struct ifreq)    /* set ifnet flags */
#define	SIOCGIFFLAGS	_IOWR('I',17, struct ifreq)    /* get ifnet flags */
#define	OSIOCGIFBRDADDR	_IOWR('I',18, struct ifreq)    /* get broadcast addr */
#define	SIOCGIFBRDADDR	_IOWR('I',35, struct ifreq)    /* get broadcast addr */
#define	SIOCSIFBRDADDR	_IOW('I',19, struct ifreq)     /* set broadcast addr */
#define	OSIOCGIFCONF	_IOWR('I',20, struct ifconf)   /* get ifnet list */
#define SIOCGIFCONF     _IOWR('I',36, struct ifconf)   /* get ifnet list */
#define	OSIOCGIFNETMASK	_IOWR('I',21, struct ifreq)    /* get net addr mask */
#define	SIOCGIFNETMASK	_IOWR('I',37, struct ifreq)    /* get net addr mask */
#define	SIOCSIFNETMASK	_IOW('I',22, struct ifreq)     /* set net addr mask */
#define	SIOCGIFMETRIC	_IOWR('I',23, struct ifreq)    /* get IF metric */
#define	SIOCSIFMETRIC	_IOW('I',24, struct ifreq)     /* set IF metric */
#define	SIOCDIFADDR	_IOW('I',25, struct ifreq)     /* delete IF addr */
#define	SIOCAIFADDR	_IOW('I',26, struct ifaliasreq)	/* add/chg IF alias */
#define SIOCGIFMTU      _IOWR('I',27, struct ifreq)    /* get IF mtu */
#define SIOCGIFHWADDR   _IOWR('I',28, struct ifconf)   /* get hardware addr */
#define OSIOCGIFHWADDR  SIOCGIFHWADDR

#define	SIOCSARP	_IOW('I', 30, struct arpreq)   /* set arp entry */
#define	OSIOCGARP	_IOWR('I',31, struct arpreq)   /* get arp entry */
#define	SIOCGARP	_IOWR('I',38, struct arpreq)   /* get arp entry */
#define	SIOCDARP	_IOW('I', 32, struct arpreq)   /* delete arp entry */
#endif /* 4BSD */

/* MS-DOS */
#define SIOCSHIWAT  _IOW('s',  0, int)    /* set high watermark */
#define SIOCGHIWAT  _IOR('s',  1, int)    /* get high watermark */
#define SIOCSLOWAT  _IOW('s',  2, int)    /* set low watermark */
#define SIOCGLOWAT  _IOR('s',  3, int)    /* get low watermark */
#define SIOCATMARK  _IOR('s',  7, int)    /* at oob mark? */

#endif /* !SYS_IOCTL_H */

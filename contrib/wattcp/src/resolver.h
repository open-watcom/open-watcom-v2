/*
 * ++Copyright++
 * -
 * Copyright (c) 
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
 *     This product includes software developed by the University of
 *     California, Berkeley and its contributors.
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

/* Key:
 *    ucb = U C Berkeley 4.8.3 release
 *    vix = Paul Vixie of Digital
 *    del = Don Lewis of Harris
 *    mcsun = Piet Beertema of EUNet
 *    asp = Andrew Partan of UUNet
 *    pma = Paul Albitz of Hewlett Packard
 *    bb = Bryan Beecher of UMich
 *    mpa = Mark Andrews of CSIRO - DMS
 *    rossc = Ross Cartlidge of The Univeritsy of Sydney
 *    mtr = Marshall Rose of TPC.INT
 *    bg = Benoit Grange of INRIA
 *    ckd = Christopher Davis of Kapor Enterprises
 *    gns = Greg Shapiro of WPI
 */

/*
 * 10.Dec-97   Adapted for Waterloo TCP/IP - G. Vanem (giva@bgnett.no)
 *
 */

#ifndef __RESOLVER_H_
#define __RESOLVER_H_

#if defined(__TURBOC__) && (__TURBOC__ <= 0x301)
  /*
   * Prevent tcc <= 2.01 from even looking at this.
   */
#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <resolv.h>

#include "wattcp.h"
#include "misc.h"
#include "bsdname.h"
#include "sock_ini.h"
#include "udp_dom.h"
#include "udp_nds.h"
#include "pcbuf.h"
#include "pcconfig.h"
#include "pctcp.h"

extern const char *_res_opcodes[];
extern const char *_res_resultcodes[];

extern char *res_cfg_options, *res_cfg_aliases;

extern int  h_errno, errno_s;

extern void res_init0 (void);

/*
 * Set errno/errno_s, see ../inc/sys/werrno.h for list.
 */
#define SOCK_ERR(err)    errno_s = errno = err

#define CHECK_SRVR_ADDR  1  /* check nameserver address in responses */
#define RESOLVSORT       1  /* allow sorting of addresses in gethostbyname (mpa) */
#define RFC1535          1  /* use RFC 1535 default for "search" list (vix) */
#define ALLOW_T_UNSPEC   0  /* enable the "unspec" RR type for old athena (ucb) */
#define ALLOW_UPDATES    0  /* 1: enable updating resource records */

#endif  /* old __TURBOC__ */
#endif  /* __RESOLVER_H_  */

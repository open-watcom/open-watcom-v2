/*
 * Copyright (c) 1983, 1989 Regents of the University of California.
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
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
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
 *      @(#)ftp.h       8.1 (Berkeley) 6/2/93
 */

#ifndef __ARPA_FTP_H
#define __ARPA_FTP_H

/* Definitions for FTP; see RFC-765. */

/*
 * Reply codes.
 */
#define PRELIM          1       /* positive preliminary */
#define COMPLETE        2       /* positive completion */
#define CONTINUE        3       /* positive intermediate */
#define TRANSIENT       4       /* transient negative completion */
#define ERROR           5       /* permanent negative completion */

/*
 * Type codes
 */
#define TYPE_A          1       /* ASCII */
#define TYPE_E          2       /* EBCDIC */
#define TYPE_I          3       /* image */
#define TYPE_L          4       /* local byte size */

#ifdef FTP_NAMES
char *typenames[] =  {"0", "ASCII", "EBCDIC", "Image", "Local" };
#endif

/*
 * Form codes
 */
#define FORM_N          1       /* non-print */
#define FORM_T          2       /* telnet format effectors */
#define FORM_C          3       /* carriage control (ASA) */
#ifdef FTP_NAMES
char *formnames[] =  {"0", "Nonprint", "Telnet", "Carriage-control" };
#endif

/*
 * Structure codes
 */
#define STRU_F          1       /* file (no record structure) */
#define STRU_R          2       /* record structure */
#define STRU_P          3       /* page structure */
#ifdef FTP_NAMES
char *strunames[] =  {"0", "File", "Record", "Page" };
#endif

/*
 * Mode types
 */
#define MODE_S          1       /* stream */
#define MODE_B          2       /* block */
#define MODE_C          3       /* compressed */
#ifdef FTP_NAMES
char *modenames[] =  {"0", "Stream", "Block", "Compressed" };
#endif

/*
 * Protection levels
 */
#define PROT_C          1       /* clear */
#define PROT_S          2       /* safe */
#define PROT_P          3       /* private */
#define PROT_E          4       /* confidential */

#ifdef FTP_NAMES
char *levelnames[] =  {"0", "Clear", "Safe", "Private", "Confidential" };
#endif

#if defined(KERBEROS) && defined(NOENCRYPTION)
/* define away krb_rd_priv and krb_mk_priv.  Don't need them anyway. */
/* This might not be the best place for this ... */
#define krb_rd_priv(o,l,ses,s,h,c,m) krb_rd_safe(o,l,s,h,c,m)
#define krb_mk_priv(i,o,l,ses,s,h,c) krb_mk_safe(i,o,l,s,h,c)
#endif

/*
 * Record Tokens
 */
#define REC_ESC         '\377'  /* Record-mode Escape */
#define REC_EOR         '\001'  /* Record-mode End-of-Record */
#define REC_EOF         '\002'  /* Record-mode End-of-File */

/*
 * Block Header
 */
#define BLK_EOR         0x80    /* Block is End-of-Record */
#define BLK_EOF         0x40    /* Block is End-of-File */
#define BLK_ERRORS      0x20    /* Block is suspected of containing errors */
#define BLK_RESTART     0x10    /* Block is Restart Marker */

#define BLK_BYTECOUNT   2       /* Bytes in this block */

#endif

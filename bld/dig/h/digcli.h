/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  DIG client callback function prototypes.
*
****************************************************************************/


#ifndef DIGCLI_H_INCLUDED
#define DIGCLI_H_INCLUDED

/*
 * dig_fhandle can be pointer to file structure or handle number
 * therefore 0/NULL is reserved for errors
 * if handle number is used then handle must be 1 based
 */
#if defined( _WIN64 )
#define DIG_FID2PH(fid) (((int)(unsigned __int64)(fid)) - 1)
#define DIG_PH2FID(ph)  ((void *)(unsigned __int64)(ph + 1))
#else
#define DIG_FID2PH(fid) (((int)(unsigned long)(fid)) - 1)
#define DIG_PH2FID(ph)  ((void *)(unsigned long)(ph + 1))
#endif
#define DIG_FID2FH(fid) ((FILE *)(fid))
#define DIG_FH2FID(fh)  ((void *)(fh))

#define DIGCli(n)       DIGCli ## n
#define _DIGCli(n)      _DIGCli ## n n

#define DIGCLIENTRY(n)  DIGCLIENT DIGCli(n)

#define pick(r,n,p) typedef r (DIGCLIENT *_DIGCli ## n) p;
#include "_digcli.h"
#undef pick

#define pick(r,n,p) extern r DIGCLIENTRY( n ) p;
#include "_digcli.h"
#undef pick

#endif

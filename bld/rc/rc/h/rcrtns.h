/****************************************************************************
*
*                            Open Watcom Project
*
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
* Description:  wres library client routine prototypes.
*
****************************************************************************/


#ifndef RCRTNS_INLCUDED
#define RCRTNS_INLCUDED

#include "rcmem.h"

#define RESOPEN         res_open
#define RESCLOSE        res_close
#define RESWRITE        res_write
#define RESREAD         res_read
#define RESSEEK         res_seek
#define RESTELL         res_tell
#define RESIOERR        res_ioerr

#define RESALLOC        RcMemMalloc
#define RESFREE         RcMemFree

#define RCREALLOC       RcMemRealloc

extern FILE             *res_open( const char *, wres_open_mode );
extern bool             res_close( FILE * );
extern size_t           res_write( FILE *, const void *, size_t );
extern size_t           res_read( FILE *, void *, size_t );
extern bool             res_seek( FILE *, long, int );
extern long             res_tell( FILE * );
extern bool             res_ioerr( FILE *, size_t );

#endif

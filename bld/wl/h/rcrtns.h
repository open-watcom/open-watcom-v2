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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef RCRTNS_INLCUDED
#define RCRTNS_INLCUDED

#include "alloc.h"

#define RESOPEN         res_open
#define RESCLOSE        res_close
#define RESWRITE        res_write
#define RESREAD         res_read
#define RESSEEK         res_seek
#define RESTELL         res_tell
#define RESIOERR        res_ioerr

#define RESALLOC        ChkLAlloc
#define RESFREE         LFree

#define RCREALLOC       LnkReAlloc

extern WResFileID       res_open( const char *name, wres_open_mode omode );
extern bool             res_close( WResFileID fid );
extern size_t           res_read( WResFileID fid, void *buf, size_t len );
extern size_t           res_write( WResFileID fid, const void *buf, size_t len );
extern bool             res_seek( WResFileID fid, WResFileOffset off, int where );
extern WResFileOffset   res_tell( WResFileID fid );
extern bool             res_ioerr( WResFileID fid, size_t rc );

#endif

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

#include "wio.h"
#include "alloc.h"
#include "wressetr.h"

extern WResFileID       res_open( const char *name, wres_open_mode omode );
extern int              res_close( WResFileID fid );
extern WResFileSSize    res_read( WResFileID fid, void *buf, WResFileSize len );
extern WResFileSSize    res_write( WResFileID fid, const void *buf, WResFileSize len );
extern WResFileOffset   res_seek( WResFileID fid, WResFileOffset off, int where );
extern WResFileOffset   res_tell( WResFileID fid );

#define RCOPEN          res_open
#define RCCLOSE         res_close
#define RCWRITE         res_write
#define RCREAD          res_read
#define RCSEEK          res_seek
#define RCTELL          res_tell
#define RCALLOC         ChkLAlloc
#define RCFREE          LFree
#define RCREALLOC       LnkReAlloc

#endif

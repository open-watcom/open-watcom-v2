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
#include "trmemcvr.h"

extern WResFileID     wres_open( const char *name, wres_open_mode omode );
extern int            wres_close( WResFileID fid );
extern WResFileSSize  wres_read( WResFileID fid, void *buf, WResFileSize size );
extern WResFileSSize  wres_write( WResFileID fid, const void *buf, WResFileSize size );
extern WResFileOffset wres_seek( WResFileID fid, WResFileOffset pos, int where );
extern WResFileOffset wres_tell( WResFileID fid );

#define RCOPEN          wres_open
#define RCCLOSE         wres_close
#define RCWRITE         wres_write
#define RCREAD          wres_read
#define RCSEEK          wres_seek
#define RCTELL          wres_tell
#define RCALLOC         TRMemAlloc
#define RCFREE          TRMemFree
#define RCREALLOC       TRMemRealloc

#endif

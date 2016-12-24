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

#define RCOPEN          res_open
#define RCCLOSE         res_close
#define RCWRITE         res_write
#define RCREAD          res_read
#define RCSEEK          res_seek
#define RCTELL          res_tell

#define RCALLOC         RcMemMalloc
#define RCFREE          RcMemFree
#define RCREALLOC       RcMemRealloc

#define RCIOERR(fh,rc)  (rc == -1)

extern WResFileID       res_open( const char *, wres_open_mode );
extern int              res_close( WResFileID );
extern WResFileSSize    res_write( WResFileID, const void *, WResFileSize );
extern WResFileSSize    res_read( WResFileID, void *, WResFileSize );
extern WResFileOffset   res_seek( WResFileID, WResFileOffset, int );
extern WResFileOffset   res_tell( WResFileID );

#endif

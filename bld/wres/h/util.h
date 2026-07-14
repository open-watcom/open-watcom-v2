/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef WRESUTIL_INCLUDED
#define WRESUTIL_INCLUDED

#include <stddef.h>
#include "filefmt.h"
#include "resnamor.h"


#define AllocWResIDName(o,n)    WRESALLOC((o) + offsetof( WResIDName, Name ) + (n))
#define AllocWResIDNum(o)       WRESALLOC((o) + sizeof( ((WResID *)0)->ID.Num ))
#define AllocWResHelpIDNum(o)   WRESALLOC((o) + sizeof( ((WResHelpID *)0)->ID.Num ))

extern WResIDName           *WResStringIDNameFromStr( const char *str );

extern WResID               *WResIDFromStr( const char *str );
extern WResID               *WResIDFromNum( long num );
extern void                 WResInitIDFromNum( long num, WResID *id );
extern char                 *WResIDToStr( const WResID *id );
extern long                 WResIDToNum( const WResID *id );
extern void                 WResIDFree( WResID *id );
extern int                  WResIDNameCmp( const WResIDName *name_id1, const WResIDName *name_id2 );
extern int                  WResIDCmp( const WResID *id1, const WResID *id2 );
extern ResNameOrOrdinal     *WResIDToNameOrOrdinal( WResID *id );
extern WResID               *WResIDFromNameOrOrdinal( ResNameOrOrdinal *name );

extern WResHelpID           *WResHelpIDFromStr( const char *str );
extern WResHelpID           *WResHelpIDFromNum( long num );
extern void                 WResInitHelpIDFromNum( long num, WResHelpID *help_id );
extern char                 *WResHelpIDToStr( const WResHelpID *help_id );
extern long                 WResHelpIDToNum( const WResHelpID *help_id );
extern void                 WResHelpIDFree( WResHelpID *help_id );
extern int                  WResHelpIDCmp( const WResHelpID *help_id1, const WResHelpID *help_id2 );
extern ResNameOrOrdinal     *WResHelpIDToNameOrOrdinal( WResHelpID *help_id );
extern WResHelpID           *WResHelpIDFromNameOrOrdinal( ResNameOrOrdinal *name );

#endif

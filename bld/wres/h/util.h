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


#ifndef WRESUTIL_INCLUDED
#define WRESUTIL_INCLUDED

#include "filefmt.h"
#include "resnamor.h"

extern WResIDName          *WResIDNameFromStr( const char * string );
extern WResID              *WResIDFromNum( long );
extern void                WResInitIDFromNum( long newnum, WResID * newid );
extern WResID              *WResIDFromStr( const char * );
extern char                *WResIDToStr( const WResID * );
extern long                WResIDToNum( const WResID * );
extern void                WResIDFree( WResID * );
extern int                 WResIDExtraBytes( const WResID * );
extern int                 WResIDNameCmp( const WResIDName *, const WResIDName * );
extern int                 WResIDCmp( const WResID *, const WResID * );
extern ResNameOrOrdinal    *WResIDToNameOrOrd ( WResID * id );
extern WResID              *WResIDFromNameOrOrd( ResNameOrOrdinal * name );

extern WResHelpID          *WResHelpIDFromNum( long );
extern void                WResInitHelpIDFromNum( long newnum, WResHelpID * newid );
extern WResHelpID          *WResHelpIDFromStr( const char * );
extern char                *WResHelpIDToStr( const WResHelpID * );
extern long                WResHelpIDToNum( const WResHelpID * );
extern void                WResHelpIDFree( WResHelpID * );
extern int                 WResHelpIDExtraBytes( const WResHelpID * );
extern int                 WResHelpIDCmp( const WResHelpID *, const WResHelpID * );
extern ResNameOrOrdinal    *WResHelpIDToNameOrOrd ( WResHelpID * id );
extern WResHelpID          *WResHelpIDFromNameOrOrd( ResNameOrOrdinal * name );

#endif

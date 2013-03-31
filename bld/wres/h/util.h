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

WResIDName *        WResIDNameFromStr( char * string );
WResID *            WResIDFromNum( long );
void                WResInitIDFromNum( long newnum, WResID * newid );
WResID *            WResIDFromStr( const char * );
char *              WResIDToStr( const WResID * );
long                WResIDToNum( const WResID * );
void                WResIDFree( WResID * );
int                 WResIDExtraBytes( const WResID * );
int                 WResIDNameCmp( const WResIDName *, const WResIDName * );
int                 WResIDCmp( const WResID *, const WResID * );
ResNameOrOrdinal *  WResIDToNameOrOrd ( WResID * id );
extern WResID *     WResIDFromNameOrOrd( ResNameOrOrdinal * name );

WResHelpID *        WResHelpIDFromNum( long );
void                WResInitHelpIDFromNum( long newnum, WResHelpID * newid );
WResHelpID *        WResHelpIDFromStr( const char * );
char *              WResHelpIDToStr( const WResHelpID * );
long                WResHelpIDToNum( const WResHelpID * );
void                WResHelpIDFree( WResHelpID * );
int                 WResHelpIDExtraBytes( const WResHelpID * );
int                 WResHelpIDCmp( const WResHelpID *, const WResHelpID * );
ResNameOrOrdinal *  WResHelpIDToNameOrOrd ( WResHelpID * id );
extern WResHelpID * WResHelpIDFromNameOrOrd( ResNameOrOrdinal * name );

#endif

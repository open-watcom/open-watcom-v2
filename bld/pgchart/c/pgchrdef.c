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


#include <string.h>
#include "pgvars.h"
#include "fontsupp.h"


static char _WCI86FAR *GetCharPtr( short ch )
//======================================

{
    struct _fontinfo    info;
    short               dummy;

    _getfontinfo( &info );
    if( info.pixheight == 8 && info.pixwidth == 8 && info.type == 0 ) {
        return( _getbitmap( ch, &dummy ) );
    } else {
        return( NULL );
    }
}


_WCRTLINK short _WCI86FAR _CGRAPH _pg_getchardef( short charnum,
                                   unsigned char _WCI86FAR *chardef )
//==============================================================

/*  Sets the 8x8 pixel bitmap for the character with
    ASCII number "charnum". */

{
    char _WCI86FAR           *bitmap;

    bitmap = GetCharPtr( charnum );
    if( bitmap != NULL ) {
        MemoryCopy( chardef, bitmap, 8 * sizeof( char ) );
        return( 0 );
    } else {
        return( 1 );
    }
}

Entry( _PG_GETCHARDEF, _pg_getchardef ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _pg_setchardef( short charnum,
                                   unsigned char _WCI86FAR *chardef )
//==============================================================

/*  Sets the character with ASCII number "charnum" to the 8x8 pixel
    bitmap "chardef".   */

{
    char _WCI86FAR           *bitmap;

    bitmap = GetCharPtr( charnum );
    if( bitmap != NULL ) {
        MemoryCopy( bitmap, chardef, 8 * sizeof( char ) );
        return( 0 );
    } else {
        return( 1 );
    }
}

Entry( _PG_SETCHARDEF, _pg_setchardef ) // alternate entry-point

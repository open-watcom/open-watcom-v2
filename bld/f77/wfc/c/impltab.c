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
* Description:  Implicit type processing.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "csetinfo.h"
#include "types.h"
#include "impltab.h"

extern  character_set   CharSetInfo;


static  unsigned char   CharIndex( char chr ) {
//=============================================

// Map character to range 0..IMPL_SIZE.
// This will work for both EBCDIC and ASCII character set. It's
// complicated because EBCDIC character set is not contiguous.

    if( chr == '$' ) return( 26 );
    if( chr == '_' ) return( 27 );
#if _CSET == _EBCDIC
    if( chr <= 'I' ) return( chr - 'A' );
    if( chr <= 'R' ) return( chr - 'J' + 9 );
    if( chr <= 'Z' ) return( chr - 'S' + 18 );
#else
    return( chr - 'A' );
#endif
}


TYPE    ImplType( char chr ) {
//============================

// Determine the implicit type of a symbol starting with the given
// character.

    if( CharSetInfo.is_foreign( chr ) )
        return( FT_REAL );
    return( ImplicitTab[ CharIndex( chr ) ].typ );
}


uint    ImplSize( char chr ) {
//============================

// Determine the size of a storage unit a symbol starting with the
// given character.

    if( CharSetInfo.is_foreign( chr ) )
        return( StorageSize( FT_REAL ) );
    return( ImplicitTab[ CharIndex( chr ) ].size );
}


bool    SetImplType( char chr1, char chr2, TYPE typ, uint size ) {
//================================================================

// Set the implicit type of the given range of characters to the
// given type.

    int         i;
    int         j;

    i = CharIndex( chr1 );
    j = CharIndex( chr2 );
    for(;;) {
        if( i > j ) return( TRUE );
        if( ImplicitTab[i].impl_ed ) return( FALSE );
        ImplicitTab[i].impl_ed = TRUE; // indicate implicit type set
        ImplicitTab[i].size = size;
        ImplicitTab[i].typ = typ;
        ++i;
    }
}


void    InitImplTab( void ) {
//===========================

// Set default types in implicit table.

    TYPE        typ;
    int         i;

    for( i = 0; i <= IMPL_SIZE; ++i ) {
        typ = FT_REAL;
        if( ( i >= 8 ) && ( i <= 13 ) ) {
            typ = FT_INTEGER;
        }
        ImplicitTab[i].typ = typ;
        ImplicitTab[i].impl_ed = FALSE;
        ImplicitTab[i].size = StorageSize( typ );
    }
}

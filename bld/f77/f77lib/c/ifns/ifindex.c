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


//
// IFINDEX      : location of character substring
//

#include "ftnstd.h"
#include "ifenv.h"


intstar4        XINDEX( char PGM *s, char PGM *t, uint slen, uint tlen ) {
//========================================================================

    unsigned int        i;
    unsigned int        j;
    unsigned int        k;

    if( tlen > slen ) {
        return( 0 );
    }
    for( i = 0; i <= slen - tlen; i++ ) {
        for( j = i, k = 0; ( k < tlen ) && ( s[ j ] == t[ k ] ); j++, k++ ) {
        }
        if( k >= tlen ) {
            return( i+1 );
        }
    }
    return( 0 );
}


intstar4        INDEX( string PGM *arg1, string PGM *arg2 ) {
//===========================================================

// Return the index of the location of string arg2 in string arg1.

    return( XINDEX( arg1->strptr, arg2->strptr, arg1->len, arg2->len ) );
}

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
// KWLOOKUP  : look up a keyword in a table
//
// The binary lookup scheme does not always work for us. However, when it does,
// it works very quickly (a maximum of 7 tries (128 = 2**7) for the statement
// keyword list). It also works quite often. When it fails we resort to a
// sequential search of a subset of the keywords.

#include "ftnstd.h"
#include "kwlookup.h"

#include <string.h>


int     KwLookUp( char __FAR **table, int high, char *id, int id_len, bool exact ) {
//============================================================================

    char        __FAR *key;
    char        __FAR *ident;
    int         mid;
    int         low;
    int         kw_len;

    // index 0 is reserved
    // each keyword table has blank first item
    low = 1;
    while( low <= high ) {
        mid = (low + high) / 2;    // find mid point
        key = table[ mid ];
        ident = id;
        for(;;) {
            if( ident == id + id_len )
                break;
            if( *ident != *key )
                break;
            if( *key == NULLCHAR )
                break;
            ++ident;
            ++key;
        }
        // quit if perfect match
        if( ( ident == id + id_len ) && ( *key == NULLCHAR ) )
            return( mid );
        if( ( ident != id + id_len ) && ( *ident >= *key ) ) {
            // guess string is in 2nd half of subtable
            low = mid + 1;
        } else {
            // admit string is in 1st half of subtable
            high = mid - 1;
        }
    }
    if( exact )
        return( 0 );
    // Look sequentially through table (going backwards).
    mid = high;
    while( mid >= 0 ) {
        key = table[ mid ];
        if( *id > *key )
            break;
        kw_len = strlen( key );
        if( kw_len <= id_len ) {
            if( memcmp( id, key, kw_len ) == 0 ) {
                return( mid );
            }
        }
        mid--;
    }
    return( 0 );
}

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


// EncStats implementation
//
// 95/01/31 -- J.W.Welch        -- defined

#include <string.h>

#include "EncStats.h"
#include "Utility.h"


EncStats::EncStats
    ( void )
    : space_read_in( 0 )
    , space_put_out( 0 )
    , size_written( 0 )
    , max_message( 0 )
    , delim( '{' )
{
}


void EncStats::writeSizeWritten // WRITE OUT RUNNING size_written
    ( char *tgt                 // - output buffer
    , unsigned val )            // - update value
{
    *tgt++ = delim;
    *tgt++ = ' ';
    tgt = concatHex( tgt, size_written );
    delim = ',';
    size_written += val;
    space_put_out += 2;
}


void EncStats::writeName        // WRITE OUT NAME
    ( char * tgt                // - output buffer
    , char const * name )       // - name
{
    *tgt++ = delim;
    *tgt++ = ' ';
    tgt = concatHex( tgt, ::strlen( name ) );
    ++ space_put_out;
    delim = ',';
    for( ; *name != '\0'; ++name ) {
        *tgt++ =  ' ';
        *tgt++ =  ',';
        *tgt++ =  ' ';
        *tgt++ =  '\'';
        if( *name == '\'') {
            *tgt++ = '\\';
            *tgt++ = '\'';
        } else {
            *tgt++ = *name;
        }
        *tgt++ =  '\'';
        ++ space_put_out;
    }
    *tgt = '\0';
}

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


// Utility Routines
//
// 95/02/02 -- J.W.Welch        -- defined

#include <malloc.h>
#include <new.h>
#include <stdlib.h>

#include "Exc.h"
#include "Utility.h"

Space used;


char * concatStr                // CONCATENATE STRING
    ( char * tgt                // - target
    , char * src )              // - source
{
    while( *src != '\0' ) {
        *tgt++ = *src++;
    }
    *tgt = '\0';
    return tgt;
}


char* concatDec                 // CONCATENATE DECIMAL #
    ( char * tgt                // - target
    , Space value )             // - value
{
    char buf[16];               // - buffer

    ultoa( value, buf, 10 );
    return concatStr( tgt, buf );
}


char* concatHex                 // CONCATENATE HEX #
    ( char * tgt                // - target
    , Space value )             // - value
{
    char buf[16];               // - buffer

    ultoa( value, buf, 16 );
    tgt[0] = '0';
    tgt[1] = 'x';
    return concatStr( &tgt[2], buf );
}


// Cover ::new, ::new[]


static void* my_alloc           // COVER FOR ALLOCATION
    ( size_t size )             // - size
{
    used += size;
    void* p = _fmalloc( size );
    if( 0 == p ) {
        throw Exc( "MEM", "memory exhausted", 0 );
    }
    return p;
}


void* operator new              // ::new
    ( size_t size )             // - size required
{
    return my_alloc( size );
}


void* operator new[]            // ::new[]
    ( size_t size )             // - size required
{
    return my_alloc( size );
}

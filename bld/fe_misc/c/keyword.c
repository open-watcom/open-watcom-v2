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


#include <stddef.h>
#include "weights.gh"           // -- .gh means "generated H file"

unsigned keyword_hash( void )
{
    extern char id[];
    extern unsigned char weight[];
    size_t len;
    unsigned hash;

    hash = len + ( weight[ id[ FIRST_INDEX ] ] * FIRST_SCALE );
#if LAST_INDEX == 0
    hash += weight[ id[ len - ( LAST_INDEX + 1 ) ] ] * LAST_SCALE;
#else
    if( len >= LAST_INDEX + 1 ) {
        hash += weight[ id[ len - ( LAST_INDEX + 1 ) ] ] * LAST_SCALE;
    }
#endif
#ifdef KEYWORD_HASH_MASK
    hash &= KEYWORD_HASH_MASK;
#ifdef KEYWORD_HASH_EXTRA
    if( hash >= KEYWORD_HASH ) {
        hash -= KEYWORD_HASH;
    }
#endif
#else
    hash %= KEYWORD_HASH;
#endif
    return hash;
}

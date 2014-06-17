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
* Description:  Keyword hash function; must correspond to findhash.c.
*
****************************************************************************/


static unsigned keyword_hash( const char *name, const unsigned char *weights, size_t len )
{
    unsigned        hash;
    unsigned char   c;

    c = name[LEN_MIN];
#ifdef IGNORE_CASE
    hash = len + tolower( c );
#else
    hash = len + c;
#endif
    if( len > FIRST_INDEX ) {
        c = name[FIRST_INDEX];
    } else {
        c = name[len - 1];
    }
    hash += weights[c] * FIRST_SCALE;
    if( len > LAST_INDEX ) {
        c = name[len - ( LAST_INDEX + 1 )];
    } else {
        c = name[0];
    }
    hash += weights[c] * LAST_SCALE;
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
    return( hash );
}

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


#ifndef __HASHPJW_H__
#define __HASHPJW_H__

// HashPjw -- swiped from Dragon book
//
// 97/01/29 -- J.W.Welch        -- swiped code

// the following code fragments assume:
//  s -- contains the input C-string to be hashed
//  h -- is defined to contain the hash value

#define HashPjwCode                                     \
    unsigned h = 0;                                     \
{                                                       \
    unsigned g;                                         \
    for( ; ; ) {                                        \
        char ch = *s;                                   \
        if( 0 == ch ) break;                            \
        ++s;                                            \
        /* ( h & ~0x0fff ) == 0 is always true here */  \
        h = (h << 4) + ch;                              \
        g = h & ~0x0fff;                                \
        h ^= g;                                         \
        h ^= g >> 12;                                   \
    }                                                   \
}

#define HashPjwCodeIns                                  \
    unsigned h = 0;                                     \
{                                                       \
    unsigned g;                                         \
    for( ; ; ) {                                        \
        char ch = *s;                                   \
        if( 0 == ch ) break;                            \
        ++s;                                            \
        /* ( h & ~0x0fff ) == 0 is always true here */  \
        h = (h << 4) + (ch | ' ');                      \
        g = h & ~0x0fff;                                \
        h ^= g;                                         \
        h ^= g >> 12;                                   \
    }                                                   \
}

#ifdef __cplusplus
inline
unsigned HashPjw                // CASE-SENSITIVE HASH
    ( char const * s )
{
    HashPjwCode
    return h;
}


inline
unsigned HashPjwInsensitive     // CASE-INSENSITIVE HASH
    ( char const * s )
{
    HashPjwCodeIns
    return h;
}
#endif // __cplusplus

#endif // __HASHPJW_H__

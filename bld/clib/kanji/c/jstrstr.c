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


#include "variety.h"
#include <string.h>
#include <jstring.h>

/**
*
*  Name:        jstrstr         文字列からのある文字列のサーチ
*
*  Synopsis:    s3 = jstrstr( s1, s2 );
*
*               JSTRING s1;     サーチされる文字列
*               JSTRING s2;     string1の中から見つけたい文字列
*               JSTRING s3;     string1の中で見つけられた文字列
*
*  Description: 文字列 s1 の中から文字列 s2 を捜し、見つけられた文字列への
*               ポインタを返します。見つからなかった場合には、ヌルポインタを
*               返します。
*
*  Name:        jstrstr         Search a substring in a string
*
*  Synopsis:    s3 = jstrstr( s1, s2 );
*
*               JSTRING s1;     2-byte KANJI string
*               JSTRING s2;     2-byte KANJI substring
*               JSTRING s3;     Ponter to found string
*
*  Description: The jstrstr function locate the first occurrence in the 2-byte
*               KANJI string pointed to by s1 of sequence of characters
*               (excluding the terminating null character) un the 2-byte KANJI
*               string pointed to s2.
*
*  Return:      The jstrstr function returns a pointer to the located 2-byte
*               KANJI string, or NULL if the string is not found.
*
**/

_WCRTLINK JSTRING jstrstr( const JCHAR *s1, const JCHAR *s2 )
{
    JMOJI c1, c2;
    const JCHAR *sp2, *n;

    sp2 = jgetmoji( s2, &c2 );
    if( c2 == '\0' ) return( (JSTRING) s1 );
loop1:
    n = s1;
    s1 = jgetmoji( s1, &c1 );
    if ( c1 == '\0' ) return( (JSTRING) NULL );
loop1a:
    if( c1 != c2 ) {
        goto loop1;
    } else {
loop2:
        s1 = jgetmoji( s1, &c1 );
        sp2 = jgetmoji( sp2, &c2 );
        if( c2 == '\0' ) return( (JSTRING) n );
        if( c1 == '\0' ) return( (JSTRING) NULL );
        if ( c2 == c1 ) goto loop2;
        s1 = jgetmoji( ++n, &c1 );
        sp2 = jgetmoji( s2, &c2 );
        goto loop1a;
    }
}

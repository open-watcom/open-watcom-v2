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
#include <jstring.h>

/**
*
*  Name:        _fjstrstr               文字列からのある文字列のサーチ
*
*  Synopsis:    s3 = _fjstrstr( s1, s2 );
*
*               JSTRING s1;     サーチされる文字列
*               JSTRING s2;     string1の中から見つけたい文字列
*               JSTRING s3;     string1の中で見つけられた文字列
*
*  Description: 文字列 s1 の中から文字列 s2 を捜し、見つけられた文字列への
*               ポインタを返します。見つからなかった場合には、ヌルポインタを
*               返します。
*
*  Name:        _fjstrstr               Search a substring in a string
*
*  Synopsis:    s3 = _fjstrstr( s1, s2 );
*
*               JSTRING s1;     2-byte KANJI string
*               JSTRING s2;     2-byte KANJI substring
*               JSTRING s3;     Ponter to found string
*
*  Description: The _fjstrstr function locate the first occurrence in the
*               2-byte KANJI string pointed to by s1 of sequence of characters
*               (excluding the terminating null character) un the 2-byte KANJI
*               string pointed to s2. This function is a data independent
*               form that accept far pointer argunments. It is most useful
*               in mix memory model applications.
*
*  Return:      The _fjstrstr function returns a pointer to the located 2-byte
*               KANJI string, or NULL if the string is not found.
*
**/

_WCRTLINK FJSTRING _WCI86FAR _fjstrstr( const JCHAR _WCFAR *s1, const JCHAR _WCFAR *s2 )
{
     JMOJI c1, c2;
     const JCHAR _WCFAR *sp2, _WCFAR *n;

     sp2 = _fjgetmoji( s2, (JMOJI _WCFAR *)&c2 );
     if( c2 == '\0' ) return( (FJSTRING) s1 );
loop1:
     n = s1;
     s1 = _fjgetmoji( s1, (JMOJI _WCFAR *)&c1 );
     if( c1 == '\0' ) return( (FJSTRING) NULL );
loop1a:
    if( c1 != c2 ) {
          goto loop1;
    } else {
loop2:
        s1 = _fjgetmoji( s1, (JMOJI _WCFAR *)&c1 );
        sp2 = _fjgetmoji( sp2, (JMOJI _WCFAR *)&c2 );
        if( c2 == '\0' ) return( (FJSTRING) n );
        if( c1 == '\0' ) return( (FJSTRING) NULL );
        if( c2 == c1 ) {
            goto loop2;
        }
        s1 = _fjgetmoji( ++n, (JMOJI _WCFAR *)&c1 );
        sp2 = _fjgetmoji( s2, (JMOJI _WCFAR *)&c2 );
        goto loop1a;
    }
}

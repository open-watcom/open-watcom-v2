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

/******************************************************************************
*
*  Name:        jstrcspn        文字列の先頭から検索文字までの文字数を得る
*
*  Synopsis:    c = jstrcspn( str1, str2 );
*
*               int     c;      文字列の先頭から検索文字が見つかるまでの文字数
*               JSTRING str1;   被検索文字列
*               JSTRING str2;   検索文字のセット
*
*  Description: jstrcspn 関数は strcspn 関数を漢字コードに対応させた物です。
*               str1 で指定した文字列中で、str2 で指定した文字のいずれか１文字が*               最初に現れるまでの文字数を返します。
*
*  Returns:     str2 で指定した文字を除く文字だけからなる、str1 の先頭からの部分*               文字列の文字数を、整数値で返します。
*
*
*  Name:        jstrcspn        compute the length of the initial segment of
*                               the string consists letters not from the
*                               array of the searching letters.
*
*  Synopsis:    c = jstrcspn( str1, str2 );
*               int     c;      resoult length of computing
*               JSTRING str1;   pointer to be referenced string
*               JSTRING str2;   pointer to the array of searching letters
*
*  Description: jstrcspn function has same specification as strcspn function
*               except having advantage point for correspond to KANJI letters.
*               The jstrcspn function computes the length of the initial segment
*               of the string pointed to by str1 which consists entirely of
*               letters not from the string pointed to by str2. Japanese 2 byte
*               code letter is considered one character.
*
*  Returns:     The length of the initial segment is returned.
*/

_WCRTLINK size_t jstrcspn( const JCHAR *str1, const JCHAR *str2 )
{
    const JCHAR *s2;
    JMOJI c1, c2;
    size_t count;

    for( count = 0; str1 = jgetmoji( str1, &c1 ), c1; ++count ) {
        for( s2 = str2; s2 = jgetmoji( s2, &c2 ), c2; ) {
            if( c1 == c2 ) return count;
        }
    }
    return count;
}

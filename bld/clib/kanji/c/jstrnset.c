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

/*******************************************************************
*
*  Name:        jstrnset        文字列に文字を埋める
*
*  Synopsis:    p = jstrnset( str, c, n );
*
*               JSTRING p;      strと同じポインタ
*               JSTRING str;    文字を埋める文字列
*               JMOJI   c;      埋める文字の値
*               unsigned int n; 文字を埋める長さ
*
*  Description: jstrnset関数は文字列strを引数 c の値で埋めます。c が漢字の
*               場合、高位８ビットが、漢字の第１バイトで、低位８ビットが漢字の
*               第２バイトです。第２バイトが NULLの場合、第１バイトも NULL
*               として扱います。もし、 c に１バイト文字を指定する場合には、
*               上位バイトは、０にする必要があります。n の値が文字列の長さより
*               大きく、かつ、文字列の長さが奇数の場合には、c が漢字の時には、
*               文字列の最後のバイトは0x20になります。n 文字目が漢字の第１
*               バイトの場合、第２バイトを0x20に変更します。
*
*  Returns:     jstrnset関数は str と同じ値を返します。
*
*
*  Name:        jstrnset        fill the string with a letter
*
*  Synopsis:    p = jstrnset( str, c, n );
*
*               JSTRING p;      return value same as "str"
*               JSTRING str;    pointer to the string to be filled
*               JMOJI   c;      value of letter
*               unsigned int n; length to fill letter
*
*  Description: The jstrnset function fills the string "str" with the value of
*               the argument "c". If you specify "c" with KANJI letter, JMOJI
*               consist with 16 bit and its' high byte consist with first byte
*               code of KANJI letter, and low byte consist with second byte
*               code of KANJI letter.
*               If second byte code is NULL then also consider the first byte
*               code to NULL. If you specify the 1 byte character code, it must
*               be into low byte of JMOJI, and high byte has to consist with
*               NULL code.
*               If the value of "n" is greater than the length of the string
*               then, and if also length of the string is odd number then,
*               and if you have specified "c" with KANJI letter then, the last
*               byte of string is changed to 0x20.
*               Also When the value which "n" points to is the first byte code
*               of KANJI letter in the "str" string, the second byte code is
*               changed to 0x20.
*
*  Returns:     The jstrnset function returns the same value of "str".          *
******************************************************************************/

_WCRTLINK JSTRING jstrnset( JCHAR *str, JMOJI c, size_t n )
{
    JCHAR *dest, *keep = str;
    JMOJI c2;
    size_t byte, count, len;

    if( !n ) return str;
    byte = n * ( jiszen( c ) ? 2 : 1 );
    len = strlen( (char *)str );
    if( len <= byte ) {
        if( jiszen( c ) ) {
            for( count = 0; count < len/2; count++ ) {
                str = jputmoji( str, c );
            }
            if( len & 1 ) *str = ' ';
        } else {
            memset( str, c, len );
        }
    } else {
        for( dest = str; dest - str < byte - 1; ) {
            dest = jgetmoji( dest, &c2 );
        }
        jgetmoji( dest, &c2 );
        if( dest - str != byte && jiszen( c2 ) ) {
            *(dest + 1) = ' ';
        }
        if( jiszen( c ) ) {
            for( count = 0; count < n; count++ ) {
                str = jputmoji( str, c );
            }
        } else {
            memset( str, c, n );
        }
    }
    return( keep );
}

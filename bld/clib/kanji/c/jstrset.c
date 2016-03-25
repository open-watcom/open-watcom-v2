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

/********************************************************************
*
*  Name:        jstrset 文字列に文字を埋める
*
*  Synopsis:    p = jstrset( str, c, n );
*
*               JSTRING p;      strと同じポインタ
*               JSTRING str;    文字を埋める文字列
*               JMOJI   c;      埋める文字の値
*
*  Description: jstrset関数は文字列strを引数 c の値で埋めます。c が漢字の
*               場合、高位８ビットが、漢字の第１バイトで、低位８ビットが漢字の
*               第２バイトです。第２バイトが NULLの場合、第１バイトも NULL
*               として扱います。もし、 c に１バイト文字を指定する場合には、
*               上位バイトは、０にする必要があります。文字列の長さが奇数の場合
*               には、c が漢字の時には、文字列の最後のバイトは0x20になります。
*
*  Returns:     jstrset関数は str と同じ値を返します。
*
*
*  Name:        jstrset         fill the string with a letter
*
*  Synopsis:    p = jstrset( str, c );
*
*               JSTRING p;      return value same as "str"
*               JSTRING str;    pointer to the string to be filled
*               JMOJI   c;      value of letter
*
*  Description: The jstrset function fills the string "str" with the value of
*               the argument "c". If you specify "c" with KANJI letter, JMOJI
*               consist with 16 bit and its' high byte consist with first byte
*               code of KANJI letter, and low byte consist with second byte
*               code of KANJI letter.
*               If second byte code is NULL then also consider the first byte
*               code to NULL. If you specify the 1 byte character code, it must
*               be into low byte of JMOJI, and high byte has to consist with
*               NULL code.
*               If length of the string is odd number then, and if you have
*               specified "c" with KANJI letter then, the last byte of string
*               is changed to 0x20.
*
*  Returns:     The jstrset function returns the same value of "str".
*
***********************************************************************/


_WCRTLINK JSTRING jstrset( JCHAR *str, JMOJI c )
{
   JCHAR *keep = str;
   size_t count, len;

   len = strlen( (char *)str );
   if( jiszen( c ) ) {
      for( count = 0; count < len/2; count++ ) {
         str = jputmoji( str, c );
      }
      if( len & 1 ) *str = ' ';
   } else {
      memset( str, c, len );
   }
   return keep;
}

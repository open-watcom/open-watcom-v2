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
#include <ctype.h>

/**
*
*  Name:        jstricmp                文字列を比較する
*
*  Synopsis:    x = jstricmp( a, b );
*
*               int x;          結果
*               JSTRING a, b;   文字列
*
*  Description: ２つの文字列を比較する。大文字、小文字は無視します。
*               ＡＮＫ文字　＜　カナ文字　＜　漢字文字　の大小関係に
*               になっている。
*
*  Returns:     文字列ａの方がｂよりも小さい場合は負数を、大きい場合
*               は正数を返し、ａとｂが等しいときは０を返す。
*
*
*  Name:        jstricmp        compare strings with mixed KANJI letters
*
*  Synopsis:    x = jstricmp( a, b );
*
*               int x;          resoult as compared
*               JSTRING a, b;   two strings for compare each other
*
*  Description: The jstricmp function compares the string pointed to by a
*               to the string pointed to by b with case insensitivity.
*               All upper-case characters from a and b are mapped to
*               lower-case for the purposes of doing the comparison.
*               The rule of comparison is as below.
*                 Ascii and numeric letter < KATAKANA letter < KANJI letter
*
*  Returns:     The jstricmp function returns an integer less than, equal to
*               or greater than zero, indicating that the string pointed
*               to by a is less than, equal to, or greater than the string
*               pointed to by b.
**/

_WCRTLINK int jstricmp( const JCHAR *a, const JCHAR *b )
{
    JMOJI aa, bb;

    while( 1 ) {
        a = jgetmoji( a, &aa );
        b = jgetmoji( b, &bb );
        if( aa & 0xff00 )
            aa = jtoupper( aa );
        else
            aa = toupper( aa );
        if( bb & 0xff00 )
            bb = jtoupper( bb );
        else
            bb = toupper( bb );
        if( aa != bb ) return( ( aa < bb ) ? -1 : 1 );
        if( aa == 0 ) return( 0 );
    }
}

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
*  Name:        jstrcmp         文字列を比較する
*
*  Synopsis:    x = jstrcmp( a, b );
*
*               int x;          結果
*               JSTRING a, b;   文字列
*
*  Description: ２つの文字列を比較する。ｊｓｔｒｎｃｍｐは先頭から
*               ｎ文字分のみの比較をする。
*               ＡＮＫ文字　＜　カナ文字　＜　漢字文字　の大小関係に
*               になっている。
*
*  Returns:     文字列ａの方がｂよりも小さい場合は負数を、大きい場合
*               は正数を返し、ａとｂが等しいときは０を返す。
*
*  Name:        jstrcmp         compare strings with mixed KANJI letters
*
*  Synopsis:    x = jstrcmp( a, b );
*
*               int x;          resault as compared
*               JSTRING a,b;    two strings for compare each other
*
*  Description: The jstrcmp function compares the KANJI string pointed to by
*               "a" to the string pointed to by "b". The jstrncmp function
*               compares only N number letters from top of strings.
*               The rule of comparison is as below.
*                 ASCII and numeric letter < KATAKANA letter < KANJI letter
*
*  Returns:     The jstrcmp function returns an integer less than, equal to,
*               or greater than zero, indicating that the string pointed to
*               by "a" is less than, equal to, or greater than the string
*               pointed to by "b".
*
**/

_WCRTLINK int jstrcmp( const JCHAR *a, const JCHAR *b )
{
    JMOJI aa, bb;

    while( 1 ) {
        a = jgetmoji( a, &aa );
        b = jgetmoji( b, &bb );
        if( aa != bb ) return( ( aa < bb ) ? -1 : 1 );
        if( aa == 0 ) return( 0 );
    }
}

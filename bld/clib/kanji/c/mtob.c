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
*  Name:        mtob
*
*  Synopsis:    nb = mtob( s, nm );
*
*               int nb;         バイト数
*               JSTRING s;      文字列
*               int nm;         文字数
*
*  Description: 文字列ｓの先頭からｎｍで指定した文字数の間にあるバイト数
*               を返す。漢字コードは２バイトで１文字と数える。
*
*  Name:        mtob
*
*  Synopsis:    nb = mtob( s, nm );
*
*               int nb;         Length of string s (Unit:byte)
*               JSTRING s;      String
*               int nm;         The number of character
*
*  Description: Return the length of the string s by byte from the head of s
*               to the nm th character. Two bytes of KANJI code is treated as
*               one character.
*
**/

_WCRTLINK size_t mtob( const JCHAR *s, size_t nm )
{
    const JCHAR *ss;
    JMOJI m;

    ss = s;
    while( nm-- ) {
        ss = jgetmoji( ss, &m );
        if( m == 0 ) break;
    }
    return( ss - s );
}

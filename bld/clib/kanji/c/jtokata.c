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
#include <mbstring.h>

/**
*
*  Name:        jtokata         カタカナに変換
*
*  Synopsis:    nc = jtokata( c );
*
*               JMOJI nc;       変換した全角文字
*               JMOJI c;        変換する全角文字
*
*  Description: 与えられた全角文字を変換する。jtolowerは大文字を
*               小文字に、jtoupperは小文字を大文字に変換する。
*               jtohira はカタカナをひらがなに、jtokata はひらがな
*               をカタカナに変換する。
*
*  Returns:     変換不能の場合はなにもせずにc の値を返す。
*
*
*  Name:        jtokata         Convert to katakana of 2-byte KANJI code
*
*  Synopsis:    nc = jtokata( c );
*
*               JMOJI nc;       Katakana of 2-byte KANJI code
*               JMOJI c;        2-byte KANJI code
*
*  Description: Convert to given 2-byte KANJI code. Jtolower converts upper
*               case 2-byte KANJI code to lower case, jtoupper converts lower
*               case 2-byte KANJI code to upper case. Jtohira converts
*               katakana 2-byte KANJI code to hiragana and jtokata hiragana
*               2-byte KANJI code to katakana.
*
*  Returns:     If it is not able to be converted, it returns the value of c.
*
**/

_WCRTLINK unsigned int _mbctokata( unsigned int ch )
{
    return( jtokata( ch ) );
}


_WCRTLINK JMOJI jtokata( JMOJI c )
{
    if( jishira( c ) )
        c += ( c < 0x82de ) ? 0xa1 : 0xa2;
    return( c );
}

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
*  Name:        _fjstrrchr
*
*  Synopsis:    p = _fjstrrchr( s, c );
*
*               FJSTRING p;     文字のポインタ
*               FJSTRING s;     文字列
*               JMOJI c;        文字
*
*  Description: 文字列ｓを順に検索し、最初に文字ｃと一致した
*               位置のポインタを返す。ｊｓｔｒｃｈｒは先頭から
*               検索し、ｊｓｔｒｒｃｈｒは最後から検索する。
*
*  Returns:     一致がなかった場合はヌルポインタを返す。
*
*
*  Name:        _fjstrrchr      search out specified letter from the string
*
*  Synopsis:    p = _fjstrrchr( s, c );
*
*               FJSTRING        p;      return value which point to the
*                                       searched out letter
*               JSTRING s;      pointer to the string which is referenced
*               JMOJI   c;      the value of searching letter
*
*  Description: The _fjstrrchr function locates the final occurrence of "c"
*               in the string pointed to by "s". _fstrrchr function start to
*               search out from the end of string, and jstrchr function
*               start to search out from the top of string.
*               If you specify "c" with KANJI letter, JMOJI consist with 16
*               bit and its' high byte consist with first byte code of KANJI
*               letter, and low byte consist with second byte code of KANJI
*               letter. If second byte code is NULL then the first byte code
*               also consider to NULL. If you specify "c" with 1 byte character
*               then, it must be in the low byte of JMOJI and the high byte of
*               JMOJI has to NULL. This function is a data independent form
*               that accept far pointer argunments. It is most useful in mix
*               memory model applications.
*
*  Returns:     This function returns a pointer to the located letter, or NULL
*               if the letter does not occur in the string.
*
**/


_WCRTLINK FJSTRING _WCI86FAR _fjstrrchr( const JCHAR _WCFAR *s, JMOJI c )
{
    const JCHAR _WCFAR *p, _WCFAR *ss;
    JMOJI cc;

    p = (FJSTRING) NULL;
    while( 1 ) {
        ss = _fjgetmoji( s, (JMOJI _WCFAR *)&cc );
        if( cc == 0 ) break;
        if( cc == c ) p = s;
        s = ss;
    }
    return( (FJSTRING) p );
}

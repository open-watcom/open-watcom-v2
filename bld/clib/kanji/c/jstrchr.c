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
*  Name:        jstrchr         文字を検索する
*
*  Synopsis:    p = jstrchr( s, c );
*
*               JSTRING p;      文字のポインタ
*               JSTRING s;      文字列
*               JMOJI c;        文字
*
*  Description: 文字列ｓを順に検索し、最初に文字ｃと一致した
*               位置のポインタを返す。ｊｓｔｒｃｈｒは先頭から
*               検索し、ｊｓｔｒｒｃｈｒは最後から検索する。
*
*  Returns:     一致がなかった場合はヌルポインタを返す。
*
*
*  Name:        jstrchr         search out specified KANJI letter from string
*
*  Synopsis:    p = jstrchr( s, c );
*
*               JSTRING p;      a pointer located to the specified letter
*               JSTRING s;      a pointer to the KANJI string for reference
*               JMOJI   c;      KANJI letter for search out
*
*  Description: The jstrchr function locates the first occurrence of "c" in
*               the KANJI string pointed to by "s". Jstrchr function starts to
*               search out from the top of string , and jstrrchr function starts
*               to search out from the end of string.
*
*  Returns:     This function returns a pointer to the located letter, or NULL
*               if the letter does not occur in the string.
**/

_WCRTLINK JSTRING jstrchr( const JCHAR *s, JMOJI c )
{
    JMOJI cc;
    const JCHAR *ss;

    do {
        ss = jgetmoji( s, &cc );
        if( c == cc ) return( (JSTRING) s );
        s = ss;
    } while( cc );
    return( (JSTRING) NULL );
}

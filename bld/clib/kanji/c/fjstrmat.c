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
*  Name:        _fjstrmatch     指定文字を検索する
*
*  Synopsis:    p = _fjstrmatch( s, b );
*
*               FJSTRING p;     文字のポインタ
*               FJSTRING s;     文字列
*               FJSTRING b;     指定文字の文字列
*
*  Description: ＿ｆｊｓｔｒｍａｔｃｈは、文字列ｓを１文字づつ検索して
*               最初に文字列ｂに含まれる文字と一致した位置のポイン
*               タを返す。＿ｆｊｓｔｒｓｋｉｐは反対に、最初に文字列ｂ
*               に含まれない文字が現れた位置のポインタを返す。
*
*  Returns:     検索中にヌル文字（文字の終端）に達した場合はヌルポ
*               インタを返す。
*
*
*  Name:        _fjstrmatch     compute the initial position where searching
*                               letters occur in string
*
*  Synopsis:    p = _fjstrmatch( s, b );
*
*               FJSTRING        p;      pointer to the initial position of
*                                       searching letter occurrence
*               FJSTRING        s;      pointer to the string for reference
*               FJSTRING        b;      pointer to the searching letters
*
*  Description: The jstrmatch function compute the initial position of the
*               string pointed to by s where occur the letters from the string
*               pointed to by b.  jstrskip function have the opposite
*               specification to jstrmatch, at the view of initial position
*               where not occur the searching letter. This function is
*               a data independent form that accept far pointer argunments.
*               It is most useful in mix memory model applications.
*
*  Returns:     This function will return the NULL pointer if it not occured
*               the letters from string pointed to by b.
*
**/

_WCRTLINK FJSTRING _WCI86FAR _fjstrmatch( const JCHAR _WCFAR *s, const JCHAR _WCFAR *b )
{
    const JCHAR _WCFAR *st, _WCFAR *bt;
    JMOJI ss, bb;

    for( ; st = _fjgetmoji( s, (JMOJI _WCFAR *)&ss ), ss; s = st )
        for( bt = b; bt = _fjgetmoji( bt, (JMOJI _WCFAR *)&bb ), bb; )
            if( ss == bb ) return( (FJSTRING) s );
    return( (FJSTRING) NULL );
}

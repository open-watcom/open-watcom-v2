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
*  Name:        jstrskip        指定文字以外を検索する
*
*  Synopsis:    p = jstrskip( s, b );
*
*               JSTRING p;      文字のポインタ
*               JSTRING s;      文字列
*               JSTRING b;      指定文字の文字列
*
*  Description: ｊｓｔｒｍａｔｃｈは、文字列ｓを１文字づつ検索して
*               最初に文字列ｂに含まれる文字と一致した位置のポイン
*               タを返す。ｊｓｔｒｓｋｉｐは反対に、最初に文字列ｂ
*               に含まれない文字が現れた位置のポインタを返す。
*
*  Returns:     検索中にヌル文字（文字の終端）に達した場合はヌルポ
*               インタを返す。
*
*
*  Name:        jstrskip        compute the initial position of searching
*                               letter not occur in string
*
*  Synopsis:    p = jstrskip( s, b );
*
*               JSTRING p;      pointer to the initial position of searching
*                               letter not occur in string
*               JSTRING s;      pointer to the string for reference
*               JSTRING b;      pointer to the searching letters
*
*  Description: The jstrmatch function compute the initial position of the
*               string pointed to by s where occur the letters from the string
*               pointed to by b.  jstrskip function have the opposite
*               specification to jstrmatch, at the view of initial position
*               where not occur the searching letter.
*
*  Returns:     This function will return the NULL pointer if there is no the
*               other letters in the string "s".
**/


_WCRTLINK JSTRING jstrskip( const JCHAR *s, const JCHAR *b )
{
    const JCHAR *st, *bp;       /* add "bp" by M/M Mar/15/87 */
    JMOJI ss, bb;

    for( ; st = jgetmoji( s, &ss ), ss; s = st )
        for( bp = b;; ) {  /* reset the head pointer of character-set */
            bp = jgetmoji( bp, &bb ); /* by M/M Mar/15/87 */
            if( ss == bb ) break;
            if( bb == 0 ) return( (JSTRING) s );
        }
    return( (JSTRING) NULL );
}

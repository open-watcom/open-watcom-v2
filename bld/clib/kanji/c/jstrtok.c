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
#include <jctype.h>
#include <jstring.h>


/**
*
*  Name:        jstrtok         文字列を句切文字で分割する
*
*  Synopsis:    sp = jstrtok( targ, brk );
*
*               JSTRING sp;     分割した文字列
*               JSTRING targ;   文字列
*               JSTRING brk;    句切文字
*
*  Description: 文字列ｔａｒｇを文字列ｂｒｋ内にある文字を句切文字
*               として分割する。最初の呼び出しで１個目の分割した文
*               字列へのポインタが返される。２回目からはｔａｒｇを
*               ＮＵＬＬにして呼び出すことにより、順に分割した文字
*               列へのポインタが返される。
*
*  Returns:     ヌル文字に達した場合はヌル・ポインタが返される。
*
*  Caution:     元の文字列は、ヌル文字が上書きされる事により書き換
*               えられる。
*
*
*  Name:        jstrtok         Break 2-byte KANJI string into tokens of KANJI
*
*  Synopsis:    sp = jstrtok( targ, brk );
*
*               JSTRING sp;     Broken string
*               JSTRING targ;   2-byte KANJI string
*               JSTRING brk;    Sequence of 2-byte KANJI delimiters
*
*  Description: Break the 2-byte KANJI string pointed to by targ into a
*               sequence of tokens of 2-byte KANJI, each of which is delimited
*               by a character from the string pointed to by brk. The first
*               call to jstrtok will return a pointer to the first token in the
*               string pointed to by targ. Subsequence calls to jstrtok must
*               pass a NULL pointer as the first argument, in order to get the
*               next token in the string.
*
*  Returns:     jstrtok function returns a pointer to the first character of
*               token or NULL if there is no token found.
*
*  Caution:     The given string is overwritten by NULL character.
*
**/

_WCRTLINK JSTRING jstrtok( JCHAR *targ, const JCHAR *brk )
{
    static JCHAR *sp;
    JCHAR *p, *q, *r;

    p = ( targ == NULL ) ? sp : targ;
    if( p == NULL ) return( NULL );
    q = jstrskip( p, brk );
    if( q == NULL ) return( NULL );
    if( *q == '\0' || ( iskanji( *q ) && q[1] == '\0' ) )
        return( NULL );
    r = jstrmatch( q, brk );
    if( r == NULL ) return( NULL );
    if( *r == '\0' ) {
        sp = NULL;
    } else {
        if( iskanji( *r ) ) *r++ = '\0';
        *r = '\0';
        sp = ++r;
    }
    return( q );
}

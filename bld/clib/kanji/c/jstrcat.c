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
*  Name:        jstrcat         •¶Žš—ñ‚ðŒ‹‡‚·‚é
*               jstrncat
*
*  Synopsis:    p = jstrncat( to, from, n );
*               p = jstrcat( to, from );
*
*               JSTRING p;      ‚”‚‚Æ“¯‚¶’l
*               JSTRING to;     Œ‹‡‚³‚ê‚é•¶Žš—ñ
*               JSTRING from;   Œ‹‡‚·‚é•¶Žš—ñ
*               int n;          •¶Žš”
*
*  Description: •¶Žš—ñ‚”‚‚ÌŒã‚É•¶Žš—ñ‚†‚’‚‚‚Ìæ“ª‚©‚ç
*               ‚Ž•¶Žš•ª‚¾‚¯Œ‹‡‚·‚éB
*
*  Returns:     ‘æ‚Pˆø”‚”‚‚Æ“¯‚¶‚à‚Ì‚ð•Ô‚·B
*
*
*  Name:        jstrcat          concatenate KANJI strings
*               jstrncat
*
*  Synopsis:    p = jstrncat( to, from, n );
*               p = jstrcat( to, from );
*
*               JSTRING p;      same value as argument "to"
*               JSTRING to;     distination KANJI string pointer.
*               JSTRING from;   KANJI string pointer for appendage
*               int     n;      number of letters should be appended
*
* Description:  The jstrcat and jstrncat functions append a copy of the KANJI
*               string pointed to by "from" to the end of the KANJI string
*               pointed to by "to".  The first letter of "from" overwrite the
*               null character at the end of "to", and jstrncat appends only
*               "n" number of letters.
*
*  Returns:     The value of "to" is returned.
**/

_WCRTLINK JSTRING jstrncat( JCHAR *to, const JCHAR *from, size_t n )
{
    JCHAR *p;
    size_t m;
    JMOJI mm;

    p = to;
    while( p = jgetmoji( p, &mm ), mm );

    m = mtob( from, n );
    while( m-- > 0 ) *p++ = *from++;
    *p = '\0';
    return( to );
}

_WCRTLINK JSTRING jstrcat( JCHAR *to, const JCHAR *from )
{
    return jstrncat( to, from, jstrlen( from ) );
}

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
#include <jctype.h>
#include <ctype.h>
/**
*
*  Name:        _fjstrnicmp
*
*  Synopsis:    x = _fjstrnicmp( a, b, n );
*
*               int x;          Œ‹‰Ê
*               JSTRING a, b;   •¶Žš—ñ
*               int n;          •¶Žš”
*
*  Description: ‚Q‚Â‚Ì•¶Žš—ñ‚ð”äŠr‚·‚éB‚Š‚“‚”‚’‚Ž‚ƒ‚‚‚Íæ“ª‚©‚ç
*               ‚Ž•¶Žš•ª‚Ì‚Ý‚Ì”äŠr‚ð‚·‚éB
*               ‚`‚m‚j•¶Žš@ƒ@ƒJƒi•¶Žš@ƒ@Š¿Žš•¶Žš@‚Ì‘å¬ŠÖŒW‚É
*               ‚É‚È‚Á‚Ä‚¢‚éB
*
*  Returns:     •¶Žš—ñ‚‚Ì•û‚ª‚‚‚æ‚è‚à¬‚³‚¢ê‡‚Í•‰”‚ðA‘å‚«‚¢ê‡
*               ‚Í³”‚ð•Ô‚µA‚‚Æ‚‚‚ª“™‚µ‚¢‚Æ‚«‚Í‚O‚ð•Ô‚·B
*
*
*  Name:        _fjstrnicmp
*
*  Synopsis:    x = _fjstrnicmp( a, b, n );
*
*               int x;          Result
*               JSTRING a, b;   Compared strings
*               int n;          Compared number of characters
*
*  Description: Compare two strings ignoring case in n bytes. All ASCII
*               characters are less than any 1-byte KATAKANA chracters.
*               All 1-byte KATAKANA charaters are less than any 2-byte
*               KANJI charaters. This function is a data independent form
*               that accept far pointer argunments. It is most useful in mix
*               memory model applications.
*
*  Returns:     If the string a is less than the sring b, _fjstrnicmp returns
*               negative value, if a is greater than b, it returns positive.
*               If a is equal to b, it returns 0.
*
**/

#define _mjtoupper( c ) (( c >= 0x8281 && c <= 0x829a ) ? ( c - 0x21 ) : c )
#define _mtoupper( c ) (( c >= (unsigned char)'a' && c <= (unsigned char)'z' )\
                         ? ( c - ('a' - 'A') ) : c )

_WCRTLINK int _WCI86FAR _fjstrnicmp( const JCHAR _WCFAR *a, const JCHAR _WCFAR *b, size_t n )
{
    JMOJI aa, bb;

    while( n-- ) {
        a = _fjgetmoji( a, &aa );
        if( aa > 0x7f )
            aa = _mjtoupper( aa );
        else
            aa = _mtoupper( aa );
        b = _fjgetmoji( b, &bb );
        if( bb > 0x7f )
            bb = _mjtoupper( bb );
        else
            bb = _mtoupper( bb );
        if( aa != bb ) return( ( aa < bb ) ? -1 : 1 );
        if( aa == 0 ) break;
    }
    return( 0 );
}

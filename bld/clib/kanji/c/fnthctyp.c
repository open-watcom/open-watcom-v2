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
*  Name:        _fnthctype      ƒoƒCƒg’PˆÊ‚Ì•¶Žšƒ^ƒCƒv‚ð’²‚×‚é
*
*
*  Synopsis:    ret = _fnthctype( s, b );
*
*               int ret;        ƒ^ƒCƒv
*               FJSTRING s;     •¶Žš—ñ
*               int b;          ˆÊ’u
*
*  Description: ƒoƒCƒg’PˆÊ‚Ìƒ^ƒCƒv‚ð’²‚×‚éB
*               Q‚†‚Ž‚”‚ˆ‚ƒ‚”‚™‚‚…‚Í•¶Žš—ñ‚Ìæ“ª‚ð‚O‚Æ‚·‚éˆÊ’u‚‚‚É
*               ‚ ‚éƒoƒCƒg‚É‚Â‚¢‚Äƒ^ƒCƒv‚ð’²‚×‚éB
*
*  Returns:     ƒkƒ‹•¶Žš‚Ìê‡‚Í‚b‚sQ‚h‚k‚f‚k‚ð•Ô‚·B
*               ‚Ž‚”‚ˆ‚ƒ‚”‚™‚‚…‚Å‚ÍˆÊ’u‚‚‚Ü‚Å‚ÌŠÔ‚Éƒkƒ‹•¶Žš‚ª‚ 
*               ‚Á‚½ê‡‚à‚b‚sQ‚h‚k‚f‚k‚ð•Ô‚·B
*
*
*  Name:        _fnthctype
*
*  Synopsis:    ret = _fnthctype( s, b );
*
*               int ret;        Type
*               FJSTRING s;     String
*               int b;          Location (The b th byte from the head of s)
*
*  Description: Check the type of the b th byte of string s.
*               _fnthctype uses _fchkctype which checks whether argument is
*               the first byte of KANJI code if mode is except 1. If argument
*               is the first byte of KANJI, _fchkctype returns CT_KJ1,
*               otherwise it returns CT_ANK. If mode is 1, _fchkctype checks
*               whether argument is the second byte of KANJI code. If it is
*               the second byte, chkctype returns CT_KJ2, otherwise it returns
*               CT_ILGL.
*               _fnthctype checks the b th byte of string. This function is
*               a data independent form that accept far pointer argunments.
*               It is most useful in mix memory model applications.
*
*  Returns:     If checked character is NULL character, it returns CT_ILGL.
*               If _fnthctype finds NULL character until the b th byte,
*               it returns CT_ILGL, too.
*               Retrun symbol   Value   Meanings
*               CT_ANK          0       ANK(Except KANJI)
*               CT_KJ1          1       The first byte of KANJI
*               CT_KJ2          2       The second byte of KANJI
*               CT_ILGL         -1      Illegal
*
**/


_WCRTLINK int _WCI86FAR _fnthctype( const JCHAR _WCFAR *string, size_t nbyte )
{
    int ctype;

    ctype = CT_ILGL;
/*
    // nbyte is now unsigned so following code does not work    JBS 92/10/01
    while( nbyte-- >= 0 ) {
        if( *string == '\0' ) return( CT_ILGL );
        ctype = chkctype( *string++, ctype );
    }
    // following is revised to preserve the sense of the above code
    // (i.e., if nbyte==0 we go through the loop once)
*/
    do {
        if( *string == '\0' ) return( CT_ILGL );
        ctype = chkctype( *string++, ctype );
    } while( nbyte-- != 0 );
    return( ctype );
}

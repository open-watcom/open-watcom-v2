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
*  Name:        _fjgetmoji      １文字取り出す
*
*  Synopsis:    newp = _fjgetmoji( p, moji );
*
*               FJSTRING newp;  次のポインタ
*               FJSTRING p;     文字列
*               JMOJI __far *moji;      文字
*
*  Description: 文字列から先頭の１文字を取り出す。
*               第１バイトが漢字コードで第２バイトがヌル文字の時は
*               ヌル文字とみなし、第１バイトをヌル文字に書き換える。
*
*  Returns:     次の文字のポインタを返す。
*               ヌル文字の時はヌル文字自体のポインタを返す。
*
*
*  Name:        _fjgetmoji      Get one character
*
*  Synopsis:    newp = _fjgetmoji( p, moji );
*
*               FJSTRING newp;  Pointer to the next character to
*                               gotten character
*               FJSTRING p;     String
*               JMOJI *moji;    Character
*
*  Description: Get one character from the head of the string
*               If the first byte is KANJI code and ythe second is NULL,
*               this function treats it as NULL character and changes the
*               fist byte to NULL. This function is a data independent form
*               that accept far pointer argunments. It is most useful in mix
*               memory model applications.
*
*  Return:      Return a pointer to the next character to the gotten
*               character. If the head of the string is NULL, return a pointer
*               to the NULL character.
*
**/

_WCRTLINK FJSTRING _WCI86FAR _fjgetmoji( const JCHAR _WCFAR *p, JMOJI _WCFAR *moji )
{
    if( *moji = *p ) {
        p++;
        if( iskanji( *moji ) ) {
            if( *p )
                *moji = ( *moji << 8 ) | *p++;
            else
//              *--p = '\0';        // JBS 92/09/30
                *moji = '\0';       // JBS 92/09/30
        }
    }
    return( (FJSTRING) p );
}

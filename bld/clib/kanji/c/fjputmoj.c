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
*  Name:        _fjputmoji              文字列への文字の出力
*
*  Synopsis:    nstr = _fjputmoji( str, moji )
*
*               FJSTRING nstr;  次のポインタ
*               FJSTRING str;   文字列
*               JMOJI moji;     出力する文字
*
*  Description: 文字列ｓｔｒに文字ｍｏｊｉを格納する。
*
*  Return:      ｍｏｊｉを格納した次のアドレスを指す。
*
*  Name:        _fjputmoji              Output one KANJI character to string
*
*  Synopsis:    nstr = _fjputmoji( str, moji )
*
*               FJSTRING nstr;  Pointer to the next output character
*               FJSTRING str;   String
*               JMOJI moji;     Kanji character
*
*  Description: Output 2-byte KANJI character "moji" to string "str".
*               This function is a data independent form that accept
*               far pointer argunments. It is most useful in mix memory
*               model applications.
*
*  Return:      Pointer to the next output moji to the string.
*
**/

_WCRTLINK FJSTRING _WCI86FAR _fjputmoji( JCHAR _WCFAR *str, JMOJI moji )
{
    if( jiszen( moji ) )
        *str++ = (JCHAR)( moji >> 8 );
    *str++ = (JCHAR)moji;
//  *str = 0;                                       JBS 92/07/31
    return str;
}

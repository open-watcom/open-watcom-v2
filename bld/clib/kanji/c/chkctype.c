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

/**
*
*  Name:        chkctype        バイト単位の文字タイプを調べる
*
*  Synopsis:    ret = chkctype( c, mode );
*
*               int ret;        タイプ
*               unsigned char c;文字（バイト単位）
*               int mode;       モード
*
*  Description: バイト単位のタイプを調べる。ｃｈｋｃｔｙｐｅでは
*               ｍｏｄｅに１以外の値を与えると、漢字の第１バイト
*               かどうかを調べ、第１バイトならＣＴ＿ＫＪ１を返し
*               それ以外ならばＣＴ＿ＡＮＫを返す。ｍｏｄｅに１を
*               与えると漢字の第２バイトかどうかを調べ、第２バイ
*               トならばＣＴ＿ＫＪ２を返しそれ以外ならばＣＴ＿Ｉ
*               ＬＧＬを返す。
*               ｎｔｈｃｔｙｐｅは文字列の先頭を０とする位置ｂに
*               あるバイトについてタイプを調べる。
*
*  Returns:     ヌル文字の場合はＣＴ＿ＩＬＧＬを返す。
*               ｎｔｈｃｔｙｐｅでは位置ｂまでの間にヌル文字があ
*               った場合もＣＴ＿ＯＬＧＬを返す。
*
*
*  Name:        chkctype        Check type of character by byte unit
*
*  Synopsis:    ret = chkctype( c, mode );
*
*               int ret;        Type
*               unsigned char c;Character (byte unit)
*               int mode;       mode
*
*  Description: Check type of character by byte unit. Checktype
*               checks whether c is the first byte of KANJI character
*               when mode is something except 1, if c is the first byte,
*               it returns CT_KJ1. If c is other, it returns CT_ANK.
*               When mode is 1, it checks whether c is the second byte of
*               KANJI character and if c is the second byte of KANJI character,
*               it returns CT_KJ2. If c is other, it returns CT_ILGL.
*               Nthctype checks type of the b th byte of string.
*
*  Returns:     If c is NULL character, chkctype returns CT_ILGL.
*               Nthctype returns CT_ILGL when it find NULL character until
*               the b th byte.
*
**/


_WCRTLINK int chkctype( JCHAR c, int type )
{
    if( c == '\0' ) return( CT_ILGL );                  // JBS 92/10/01
    if( type == CT_KJ1 )
        return( iskanji2( c ) ? CT_KJ2 : CT_ILGL );
    else
        return( iskanji( c ) ? CT_KJ1 : CT_ANK );
}

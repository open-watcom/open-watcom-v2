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
#include "hanzentb.h"


/**
*
*  Name:        hantozen        全角文字に変換
*               zentohan        アスキー文字に変換
*
*  Synopsis:    nc = hantozen( c );
*               nc = zentohan( c );
*
*               JMOJI nc;       変換した文字
*               JMOJI c;        変換する文字
*
*  Description: hantozenはアスキー文字をそれに対応した全角文字に
*               変換する。zentohanは逆に全角文字をアスキー文字に
*               変換する。変換可能な文字の範囲は、アスキー・コード
*               の０ｘ２０から０ｘ７ｅに対応する文字である。
*
*  Returns:     変換不能の場合はなにもせずにc の値を返す。
*
*
*  Name:        hantozen        Change ASCII character to 2-byte KANJI
*                               character
*               zentohan        Change 2-byte KANJI character to ASCII
*                               character
*
*  Synopsis:    nc = hantozen( c );
*               nc = zentohan( c );
*
*               JMOJI nc;       Changed result character
*               JMOJI c;        Source character
*
*  Description: Hantozen changes ASCII character to 2-byte KANJI character
*               corresponding to it. Oppositly zentohan changes 2-byte KANJI
*               character to ASCII character corresponding to it. The range of
*               changable character set is more than or equal to 0x20 and less
*               than or equal to 0x7e.
*
*  Returns:     Return c if c is unchangable.
*
**/

_WCRTLINK JMOJI hantozen( JMOJI c )
{
    return( ( c >= 0x20 && c <= 0x7e ) ? __HanZen1[c - 0x20] : c );
}



_WCRTLINK JMOJI zentohan( JMOJI c )
{
    JMOJI i;

    for( i = 0; i <= 0x5e; i++ )
        if( c == __HanZen1[i] ) return( i + 0x20 );
    return( c );
}

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
*  Include:  <jstring.h>
*
*  Name:        jisprint        ２バイト漢字コードを含む全表示文字のテスト
*
*  Synopsis:    ret = jisprint(unsigned short c);
*
*               unsigned c;     文字
*               int ret;        真ならば０以外、偽ならば０
*
*  Description: ２バイト漢字コードを含む全表示文字のテスト。これは、以下の式に
*               相当する。
*                       isprint( c ) || iskana( c ) || jiszen( c )
*
*  Return:
*
*
*  Include:  <jstring.h>
*
*  Name:        jisprint        Check printable character including 2-byte
*                               KANJI code
*
*  Synopsis:    ret = jisprint(unsigned short c);
*
*               unsigned c;     Character including 2-byte KANJI code
*               int ret;        Non-zero if true, otherwize zero.
*
*  Description: Check printable character including 2-byte KANJI code.
*               This function is equal to the below representatoin.
*                       isprint( c ) || iskana( c ) || jiszen( c )
*
*  Return:      Non-zero if the argument is printable, otherwize zero.
*
**/

_WCRTLINK int jisprint( JMOJI c )
{
    return( isprint( c ) || iskana( c ) || jiszen( c ) );
}

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
*  Name:        jiszen          漢字コードのチェック
*
*  Synopsis:    ret = jiszen( c );
*
*               int ret;        漢字コードならば０以外の値
*               JMOJI c;        漢字コード
*
*  Description: 漢字コードを整数値として与え、それが正しい漢字コードなら
*               ば０以外の値を返す。
*
*
*  Name:        jiszen          Check 2-byte KANJI code
*
*  Synopsis:    ret = jiszen( c );
*
*               int ret;        non-zero if c is 2-byte KANJI code
*               JMOJI c;        2-byte KANJI code
*
*  Description: Return non-zero if c is valid 2-byte KANJI code.
*
**/

_WCRTLINK int jiszen( JMOJI c )
{
    return( iskanji( c >> 8 ) && iskanji2( c & 0xff ) );
}

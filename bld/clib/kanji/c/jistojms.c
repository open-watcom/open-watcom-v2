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
#include <mbstring.h>

/**
*
*  Name:        jistojms                JISコードをシフトJISコードに変換
*               jmstojis                シフトJISコードをJISコードに変換
*
*  Synopsis:    sc = jistojms( jc );
*               jc = jmstojis( sc );
*
*               unsigned short sc;      シフトJISコード
*               unsigned short jc;      JISコード
*
*  Description: jistojms 関数は、JISコードをシフトJISコードに変換する。
*               jmstojis 関数は、シフトJISコードをJISコードに変換する。
*               引数が範囲外の場合、0 を返す。
*
*
*  Name:        jistojms                Convert JIS code to shift-JIS code
*               jmstojis                Convert shift-JIS code to JIS code
*
*  Synopsis:    ret = jistojms( c );
*               ret = jmstojis( c);
*
*               unsigned short sc;      Shift-JIS code
*               unsigned short jc;      JIS code
*
*  Description: Jistomsj converts JIS code to shift-JIS code and msjtojis
*               converts shift-JIS code to JIS code. If argument is out of
*               range, they return 0.
*
*  Note:        JIS code is a kind of KANJI character set. It is defined
*               by JIS, Japan Industrial Standard Institutes. Shift-JIS is
*               another KANJI character set. It is defined by Microsoft for
*               MS-DOS which is based on JIS code. The first byte and the
*               second byte of JIS code are less than 0x80. Microsoft has
*               designed shift-JIS code can be mixed in string with
*               alpha-numeric. Thus shift-JIS code is more than or equal to
*               0x8140.
*
**/

_WCRTLINK unsigned int _mbcjistojms( unsigned int ch )
{
    return( jistojms( ch ) );
}


_WCRTLINK unsigned short jistojms( unsigned short c )
{
    unsigned char u, l;

    l = c;
    u = c >> 8;

    if( u < 0x21 || u > 0x7e || l < 0x21 || l >0x7e ) return 0;

    l += (u % 2) ? ((l < 0x60) ? 0x1f : 0x20) : 0x7e;
    u = (u - 1) / 2 + ((u < 0x5f) ? 0x71 : 0xb1);

    return( ( (unsigned short)u << 8 ) + l);
}

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


//
// IFISHC       : circular shift
//

#include "ftnstd.h"
#include "ifenv.h"

#ifdef _M_I86

unsigned short           _srotl(unsigned short,char);
#pragma aux _srotl =                                    \
        0xD3 0xC0               /*      rol AX,CL    */ \
        parm [ax] [cl]                                  \
        value [ax];

unsigned short           _srotr(unsigned short,char);
#pragma aux _srotr =                                    \
        0xD3 0xC8               /*      ror AX,CL    */ \
        parm [ax] [cl]                                  \
        value [ax];

#elif defined( __386__ )

unsigned short           _srotl(unsigned short,char);
#pragma aux _srotl =                                    \
        0x66 0xD3 0xC0          /*      rol AX,CL    */ \
        parm [ax] [cl]                                  \
        value [ax];

unsigned short           _srotr(unsigned short,char);
#pragma aux _srotr =                                    \
        0x66 0xD3 0xC8          /*      ror AX,CL    */ \
        parm [ax] [cl]                                  \
        value [ax];

#else

static unsigned_16 maskTable[16] = {    0x8000, // 1000 0000 0000 0000
                                        0xc000, // 1100 0000 0000 0000
                                        0xe000, // 1110 0000 0000 0000
                                        0xf000, // 1111 0000 0000 0000
                                        0xf800, // 1111 1000 0000 0000
                                        0xfc00, // 1111 1100 0000 0000
                                        0xfe00, // 1111 1110 0000 0000
                                        0xff00, // 1111 1111 0000 0000
                                        0xff80, // 1111 1111 1000 0000
                                        0xffc0, // 1111 1111 1100 0000
                                        0xffe0, // 1111 1111 1110 0000
                                        0xfff0, // 1111 1111 1111 0000
                                        0xfff8, // 1111 1111 1111 1000
                                        0xfffc, // 1111 1111 1111 1100
                                        0xfffe};// 1111 1111 1111 1110


static unsigned_16 _srotl( unsigned_16 arg, unsigned_16 cnt ) {
//=============================================================

    unsigned_16 mask;

    cnt &= 0x0f;                        // take count modula 16
    if( !cnt ) return( arg );
    mask = maskTable[cnt - 1];          // load appropriate mask
    return( ( ( arg & mask ) >> ( 16 - cnt ) ) | ( arg << cnt ) );
}


static unsigned_16 _srotr( unsigned_16 arg, unsigned_16 cnt ) {
//=============================================================

    cnt &= 0x0f;                        // take count modula 16
    return( _srotl( arg, 16 - cnt ) );  // simply do the other rotation
}

#endif


intstar2        I2SHC( unsigned_16 arg1, intstar2 arg2 ) {
//=========================================================

    if( arg2 > 0 ) {
        return( _srotl( arg1, arg2 ) );
    } else {
        return( _srotr( arg1, -arg2 ) );
    }
}

intstar2        XI2SHC( unsigned_16 *arg1, intstar2 *arg2 ) {
//============================================================

    return( I2SHC( *arg1, *arg2 ) );
}

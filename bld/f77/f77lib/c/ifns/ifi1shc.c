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

#if defined( _M_IX86 )
unsigned char           _crotl(unsigned char,char);
#pragma aux _crotl =                                    \
        0xD2 0xC0               /*      rol al, cl   */ \
        parm [al] [cl]          /*                   */ \
        value [al];

unsigned char           _crotr( unsigned char, char );
#pragma aux _crotr =                                    \
        0xD2 0xC8               /*      ror al,cl    */ \
        parm [al] [cl]          /*                   */ \
        value [al];
#else

static unsigned_8 maskTable[8] = {      0x80,   // 1000 0000
                                        0xc0,   // 1100 0000
                                        0xe0,   // 1110 0000
                                        0xf0,   // 1111 0000
                                        0xf8,   // 1111 1000
                                        0xfc,   // 1111 1100
                                        0xfe }; // 1111 1110


static unsigned_8 _crotl( unsigned_8 arg, unsigned_8 cnt ) {
//==========================================================

    unsigned_8  mask;

    cnt &= 0x07;                        // take count modula 8
    if( !cnt ) return( arg );
    mask = maskTable[cnt - 1];          // load appropriate mask
    return( ( ( arg & mask ) >> ( 8 - cnt ) ) | ( arg << cnt ) );
}


static unsigned_8 _crotr( unsigned_8 arg, unsigned_8 cnt ) {
//==========================================================

    cnt &= 0x07;                        // take count modula 8
    return( _crotl( arg, 8 - cnt ) );   // simply do the other rotation
}

#endif


intstar1        I1SHC( unsigned_8 arg1, intstar1 arg2 ) {
//=======================================================

    if( arg2 > 0 ) {
        return( _crotl( arg1, arg2 ) );
    } else {
        return( _crotr( arg1, -arg2 ) );
    }
}

intstar1        XI1SHC( unsigned_8 *arg1, intstar1 *arg2 ) {
//==========================================================

    return( I1SHC( *arg1, *arg2 ) );
}

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

unsigned long           _lrotl(unsigned long,short);
#pragma aux _lrotl =                                    \
        0x81 0xE1 0x1F 0x00     /*      and cx,001fH */ \
        0xE3 0x09               /*      jcxz L2      */ \
        0xD1 0xE0               /* L1:  shl ax,1     */ \
        0xD1 0xD2               /*      rcl dx,1     */ \
        0x15 0x00 0x00          /*      adc ax,0     */ \
        0xE2 0xF7               /*      loop L1      */ \
        parm [ax dx] [cx]       /* L2:               */ \
        value [ax dx];

unsigned long           _lrotr( unsigned long, short );
#pragma aux _lrotr =                                    \
        0x81 0xE1 0x1F 0x00     /*      and CX,001fH */ \
        0xE3 0x0A               /*      jcxz L2      */ \
        0xD1 0xCA               /* L1:  ror DX,1     */ \
        0xD1 0xC2               /*      rol DX,1     */ \
        0xD1 0xD8               /*      rcr AX,1     */ \
        0xD1 0xDA               /*      rcr DX,1     */ \
        0xE2 0xF6               /*      loop L1      */ \
        parm [ax dx] [cx]       /* L2:               */ \
        value [ax dx];

#elif defined( __386__ )

unsigned long           _lrotl(unsigned long,char);
#pragma aux _lrotl =                                    \
        0xD3 0xC0               /*      rol EAX,CL   */ \
        parm [eax] [cl]                                 \
        value [eax];

unsigned long           _lrotr(unsigned long,char);
#pragma aux _lrotr =                                    \
        0xD3 0xC8               /*      ror EAX,CL   */ \
        parm [eax] [cl]                                 \
        value [eax];

#else

static unsigned long maskTable[32] = {
        0x80000000,     // 1000 0000 0000 0000 0000 0000 0000 0000
        0xc0000000,     // 1100 0000 0000 0000 0000 0000 0000 0000
        0xe0000000,     // 1110 0000 0000 0000 0000 0000 0000 0000
        0xf0000000,     // 1111 0000 0000 0000 0000 0000 0000 0000
        0xf8000000,     // 1111 1000 0000 0000 0000 0000 0000 0000
        0xfc000000,     // 1111 1100 0000 0000 0000 0000 0000 0000
        0xfe000000,     // 1111 1110 0000 0000 0000 0000 0000 0000
        0xff000000,     // 1111 1111 0000 0000 0000 0000 0000 0000
        0xff800000,     // 1111 1111 1000 0000 0000 0000 0000 0000
        0xffc00000,     // 1111 1111 1100 0000 0000 0000 0000 0000
        0xffe00000,     // 1111 1111 1110 0000 0000 0000 0000 0000
        0xfff00000,     // 1111 1111 1111 0000 0000 0000 0000 0000
        0xfff80000,     // 1111 1111 1111 1000 0000 0000 0000 0000
        0xfffc0000,     // 1111 1111 1111 1100 0000 0000 0000 0000
        0xfffe0000,     // 1111 1111 1111 1110 0000 0000 0000 0000
        0xffff0000,     // 1111 1111 1111 1111 0000 0000 0000 0000
        0xffff8000,     // 1111 1111 1111 1111 1000 0000 0000 0000
        0xffffc000,     // 1111 1111 1111 1111 1100 0000 0000 0000
        0xffffe000,     // 1111 1111 1111 1111 1110 0000 0000 0000
        0xfffff000,     // 1111 1111 1111 1111 1111 0000 0000 0000
        0xfffff800,     // 1111 1111 1111 1111 1111 1000 0000 0000
        0xfffffc00,     // 1111 1111 1111 1111 1111 1100 0000 0000
        0xfffffe00,     // 1111 1111 1111 1111 1111 1110 0000 0000
        0xffffff00,     // 1111 1111 1111 1111 1111 1111 0000 0000
        0xffffff80,     // 1111 1111 1111 1111 1111 1111 1000 0000
        0xffffffc0,     // 1111 1111 1111 1111 1111 1111 1100 0000
        0xffffffe0,     // 1111 1111 1111 1111 1111 1111 1110 0000
        0xfffffff0,     // 1111 1111 1111 1111 1111 1111 1111 0000
        0xfffffff8,     // 1111 1111 1111 1111 1111 1111 1111 1000
        0xfffffffc,     // 1111 1111 1111 1111 1111 1111 1111 1100
        0xfffffffe };   // 1111 1111 1111 1111 1111 1111 1111 1110



static unsigned_32 _lrotl( unsigned_32 arg, unsigned_32 cnt ) {
//=============================================================

    unsigned_32 mask;

    cnt &= 0x1f;                        // take count modula 32
    if( !cnt ) return( arg );
    mask = maskTable[cnt - 1];          // load appropriate mask
    return( ( ( arg & mask ) >> ( 32 - cnt ) ) | ( arg << cnt ) );
}


static unsigned_32 _lrotr( unsigned_32 arg, unsigned_32 cnt ) {
//=============================================================

    cnt &= 0x1f;                        // take count modula 32
    return( _lrotl( arg, 32 - cnt ) );  // simply do the other rotation
}

#endif


intstar4        ISHC( unsigned_32 arg1, intstar4 arg2 ) {
//=======================================================

    if( arg2 > 0 ) {
        return( _lrotl( arg1, arg2 ) );
    } else {
        return( _lrotr( arg1, -arg2 ) );
    }
}

intstar4        XISHC( unsigned_32 *arg1, intstar4 *arg2 ) {
//==========================================================

    return( ISHC( *arg1, *arg2 ) );
}

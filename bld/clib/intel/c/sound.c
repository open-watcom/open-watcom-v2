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
#include <conio.h>
#ifndef __QNX__
    #include <dos.h>
#endif
#include "extender.h"
#include "nonibm.h"

#define _LOCATION       0x501


_WCRTLINK void sound( unsigned frequency )
    {
#if !(defined(__QNX__)||defined(__WARP__))
        if( !__NonIBM ) {
#endif
            unsigned        x;
            unsigned char   mask;

            if( frequency > 18 ) {
                x = 1193180 / frequency;
                mask = inp( 0x61 );
                if( (mask & 3) == 0 ) {
                    outp( 0x61, mask | 3 );
                    outp( 0x43, 0xB6 );
                }
                outp( 0x42, x );
                outp( 0x42, x >> 8 );
            }
#if !(defined(__QNX__)||defined(__WARP__))
        } else {
            unsigned                x;
            unsigned long           parm;
            unsigned                modeport, counterport;
            unsigned char _WCFAR *  ptr;

            #ifdef __386__
                ptr = MK_FP( _ExtenderRealModeSelector, _LOCATION );
            #else
                ptr = MK_FP( 0, _LOCATION );
            #endif
            if( *ptr & 0x80 ) {     // 8 MHz
                parm = 1996800;
            } else {                // 10 MHz
                parm = 2457600;
            }
            if( *ptr & 0x08 ) {     // High-resolution
                modeport = 0x77;
                counterport = 0x73;
            } else {                // Normal resolution
                modeport = 0x3FDF;
                counterport = 0x3FDB;
            }
            x = parm / frequency;
            outp( modeport, 0x76 );
            outp( counterport, x & 0xFF );
            outp( counterport, (x>>8) & 0xFF );
            outp( 0x37, 6 );
        }
#endif
    }


_WCRTLINK void nosound(void)
    {
#if !(defined(__QNX__)||defined(__WARP__))
        if( !__NonIBM ) {
#endif
            outp( 0x61, inp(0x61) & 0xFC );
#if !(defined(__QNX__)||defined(__WARP__))
        } else {
            outp( 0x37, 7 );
        }
#endif
    }

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
#include <dos.h>
#include <stdlib.h>
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
#include "dos16.h"
#pragma pack(__pop);

#if __WATCOMC__ < 1100
 #define _FAR_INTERRUPT __far
#else
 #define _FAR_INTERRUPT
#endif

extern void __chk8087();
extern void __interrupt __far __int7();
static void __interrupt (_FAR_INTERRUPT *old_int7)();
extern int __no87;
extern unsigned char __8087;
#pragma aux __int7 "*";
#pragma aux __no87 "*";
#pragma aux __8087 "*";
extern  int __get_DOS_version();
#pragma aux __get_DOS_version = 0xb4 0x30 0xcd 0x21;  /* mov ah,30h; int 21h */


void __far __hook8087()
/*********************/
    {
        __chk8087();
        if( __no87 != 0  ||  __8087 == 0 ) {
            old_int7 = _dos_getvect( 7 );
            _dos_setvect( 7, &__int7 );
            D16Emulate( 1 );            /* enable emulator */
            __get_DOS_version();        /* force switch to real mode & back */
//            D16pmInstall( 7, FP_SEG(__int7), FP_OFF(__int7), (char *) 0 );
        }
    }

void __far __unhook8087()
/***********************/
    {
        if( old_int7 != NULL ) {
            D16Emulate( 0 );            /* disable emulator */
            _dos_setvect( 7, old_int7 );/* reset interrupt to old handler */
        }
    }

#define EMULATING_87 4

int D16Emulate( int enable_flag )
    {
        if( enable_flag ) {
            /* Tell the kernel that we are now emulating the 80x87 */
            _d16info.has_87 = 1;
            _d16info.MSW_bits |= EMULATING_87;
        } else {                        /* disable emulation */
            _d16info.has_87 = 0;
            _d16info.MSW_bits &= ~EMULATING_87;
        }
        return(1);
    }

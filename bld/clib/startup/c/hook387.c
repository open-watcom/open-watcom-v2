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

extern void __interrupt __int7();
extern int __no87;
extern unsigned char __8087;
#pragma aux __int7 "*";
#pragma aux __hook387 "*" parm caller [EAX EDX];
#pragma aux __unhook387 "*" parm caller [EAX EDX];
#pragma aux __no87 "*";
#pragma aux __8087 "*";

extern  int gorealmode();
#pragma aux gorealmode = 0xb4 0x30 0xcd 0x21;  /* mov ah,30h; int 21h */

extern int int31( char *, short );
#pragma aux int31 = 0x06 0xcd 0x31 0x19 0xc0 0x07 parm [esi] [ax] modify [ edi ];

extern int getcr0();
#pragma aux getcr0 = 0x0f 0x20 0xc0 value [ eax ]; /* mov     eax,cr0 */

extern void putcr0(int);
#pragma aux putcr0 = 0x0f 0x22 0xc0 parm [ eax ];  /* mov     cr0,eax */

extern short getcs(void);
#pragma aux getcs = 0x8c 0xc8 value [ ax ];        /* mov     ax,cs */

extern short int2f( char, short, short, short );
#pragma aux int2f = \
        0xB4 0xfa       /* mov ah,0fa */ \
        0xcd 0x2f       /* int 2f */ \
        parm [ al ] [ dx ] [ cx ] [ bx ] value [ ax ];

extern char IsWindows( void );
#pragma aux IsWindows = \
        0x66 0xb8 0x00 0x16 /* mov ax,1600h */ \
        0xcd 0x2f       /* int 2f */ \
        value [al];

#define EMULATING_87 4

static char hooked = 0;
static char has_wgod_emu = 0;

static char FPArea[128];

char __hook387( D16INFO __far *_d16infop )
/****************************************/
{
    char        iswin;

    iswin = IsWindows();
    if( iswin != 0 && iswin != 0x80 ) {
        if( int2f( 0, 0, 0, 0 ) == 0x666 ) {    /* check version */
            if( !int2f( 0x22, 0, 0, 0 ) ) {     /* fpu present */
                has_wgod_emu = 1;
                int2f( 0x1e, 0, 0, 0 );         /* emu init */
                int2f( 0x20, getcs(),
                        (unsigned long)&FPArea >> 16, (unsigned long)&FPArea );
                return( 1 );
            }
        }
    }
    if( _d16infop != NULL ) {
        if( int31( "RATIONAL DOS/4G", 0x0A00 ) == 0 ) {
            _dos_setvect( 7,
                        (void (__interrupt *)(void))(void (__near *)
                                (void))&__int7 );
            _d16infop->has_87 = 1;              /* enable emulator */
            _d16infop->MSW_bits |= EMULATING_87;
            putcr0( getcr0() | EMULATING_87 );
            gorealmode();
            hooked = 1;
        }
    }
    return( 0 );
}

char __unhook387( D16INFO __far *_d16infop )
/******************************************/
{
    if( has_wgod_emu ) {
        int2f( 0x21, getcs(), 0, 0 );   /* unregister */
        int2f( 0x1f, 0, 0, 0 );         /* emu shutdown */
        return( 1 );
    } else if( hooked ) {
        _d16infop->has_87 = 0;  /* disable emulator */
        _d16infop->MSW_bits &= ~EMULATING_87;
        putcr0( getcr0() & ~EMULATING_87 );
        gorealmode();
    }
    return( 0 );
}

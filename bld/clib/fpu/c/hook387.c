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
* Description:  Hook Coprocessor Not Available exception (387 emulation).
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <dos16.h>
#include "wdebug.h"

extern void __int7( void );
#pragma aux __int7 "*";

extern  int gorealmode( void );
#pragma aux gorealmode = \
        "mov ah,30h" \
        "int 21h";

extern int DPMICheckVendorSpecificAPI( char __far * );
#pragma aux DPMICheckVendorSpecificAPI = \
        "push es" \
        "push ds" \
        "mov ax,0a00h" \
        "mov ds,ecx" \
        "int 31h" \
        "sbb eax,eax" \
        "pop ds" \
        "pop es" \
        parm [ cx esi ] modify [ edi ] value[ eax ];

extern int getcr0( void );
#pragma aux getcr0 = "mov eax,cr0" value [ eax ];

extern void putcr0( int );
#pragma aux putcr0 = "mov cr0,eax" parm [ eax ];

extern char IsWindows( void );
#pragma aux IsWindows = \
        "mov ax,1600h" \
        "int 2fh" \
        value [al];

void __set_dos_vector( unsigned, void __far * );
#pragma aux __set_dos_vector = \
        "push ds" \
        "mov ds,cx" \
        "mov ah,25h" \
        "int 21h" \
        "pop ds" \
        parm caller [al] [cx edx];

extern int EMURegister2( _word, _dword );
#pragma aux EMURegister2 = \
        "mov ecx,ebx" \
        "shr ecx,10h" \
        "mov ax, 0fa20h" \
        "int 2fh" \
        parm[dx] [ebx] modify[ecx] value[eax];

#define EMULATING_87 4

static char hooked = 0;
static char has_wgod_emu = 0;

static char FPArea[128];

#pragma aux __hook387 "*" parm caller [DX EAX];
char __hook387( D16INFO __far *_d16infop )
/****************************************/
{
    char        iswin;

    iswin = IsWindows();
    if( iswin != 0 && iswin != 0x80 ) {
        if( CheckWin386Debug() == WGOD_VERSION ) {
            if( !FPUPresent() ) {
                has_wgod_emu = 1;
                EMUInit();
                EMURegister2( CS(), (unsigned long)&FPArea );
                return( 1 );
            }
        }
    }
    if( _d16infop != NULL ) {
        if( DPMICheckVendorSpecificAPI( "RATIONAL DOS/4G" ) == 0 ) {
            __set_dos_vector( 7, &__int7 );
            _d16infop->has_87 = 1;              /* enable emulator */
            _d16infop->MSW_bits |= EMULATING_87;
            putcr0( getcr0() | EMULATING_87 );
            gorealmode();
            hooked = 1;
        }
    }
    return( 0 );
}

#pragma aux __unhook387 "*" parm caller [DX EAX];
char __unhook387( D16INFO __far *_d16infop )
/******************************************/
{
    if( has_wgod_emu ) {
        EMUUnRegister( CS() );
        EMUShutdown();
        return( 1 );
    } else if( hooked ) {
        _d16infop->has_87 = 0;  /* disable emulator */
        _d16infop->MSW_bits &= ~EMULATING_87;
        putcr0( getcr0() & ~EMULATING_87 );
        gorealmode();
    }
    return( 0 );
}

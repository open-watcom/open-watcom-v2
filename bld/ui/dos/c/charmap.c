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


#include <dos.h>
#include "uidef.h"
#include "biosui.h"
#include "charmap.h"


#ifdef _M_I86
#define LOAD_ES_BIOS_SEGMENT    "xor ax,ax" "mov es,ax"
#else
#define LOAD_ES_BIOS_SEGMENT    "mov ax,_ExtenderRealModeSelector" "mov es,eax"
#endif

extern void __SetWriteMap( void );
#pragma aux __SetWriteMap = \
        "MOV     DX,3C4h"   \
        "MOV     AX,402h"   \
        "OUT     DX,AX"     \
    modify [ax dx];

extern void __SetSequencer( void );
#pragma aux __SetSequencer = \
        "PUSHF"                 /* Disable interrupts          */   \
        "CLI"                                                       \
        "MOV     DX,3C4h"       /* Sequencer port address      */   \
        "MOV     AX,402h"       /* write to map 3              */   \
        "OUT     DX,AX"                                             \
        "PUSH    ES"                                                \
        LOAD_ES_BIOS_SEGMENT                                        \
        "MOV     AL,ES:[487h]"  /* check VC state              */   \
        "POP     ES"                                                \
        "AND     AL,60h"        /* if it's not 0,              */   \
        "MOV     AH,07h"                                            \
        "JNZ L1"                                                    \
        "MOV     AH,05h"                                            \
    "L1: MOV     AL,04h"        /* sequential addressing       */   \
        "OUT     DX,AX"                                             \
        "MOV     DX,3CEh"       /* Graphics Cntrl. port addr.  */   \
        "MOV     AX,406h"       /* Map starts at A000:0000     */   \
        "OUT     DX,AX"         /* (64K mode)                  */   \
        "MOV     AX,204h"       /* Select map 2 for CPU reads  */   \
        "OUT     DX,AX"                                             \
        "MOV     AX,5h"         /* Disable odd-even addressing */   \
        "OUT     DX,AX"                                             \
        "STI"                                                       \
        "POPF"                                                      \
    modify [ax dx];

extern void __ResetSequencer( void );
#pragma aux __ResetSequencer = \
        "PUSHF"                 /* reset sequencer and cont. */ \
        "CLI"                                                   \
        "MOV     DX,3C4h"       /* sequencer port            */ \
        "MOV     AX,302h"       /* write to maps 0 and 1     */ \
        "OUT     DX,AX"                                         \
        "PUSH    ES"                                            \
        LOAD_ES_BIOS_SEGMENT                                    \
        "MOV     AL,ES:[487h]"  /* check VC state            */ \
        "AND     AL,60h"        /* if it's not 0,            */ \
        "MOV     AH,03h"                                        \
        "JNZ L1"                                                \
        "MOV     AH,01h"                                        \
    "L1: MOV     AL,04h"        /* use odd-even addressing   */ \
        "OUT     DX,AX"                                         \
        "MOV     AL,07h"                                        \
        "CMP     ES:[449h],AL"  /* Get current video mode    */ \
        "MOV     AH,0Eh"        /* Map starts at B800:0000   */ \
        "JNE L2"                                                \
        "MOV     AH,0Ah"        /* Map starts at B000:0000   */ \
    "L2: MOV     AL,06h"                                        \
        "MOV     DX,3CEh"       /* controller port           */ \
        "OUT     DX,AX"                                         \
        "MOV     AX,04h"        /* read map 0                */ \
        "OUT     DX,AX"                                         \
        "MOV     AX,1005h"      /* use odd-even addressing   */ \
        "OUT     DX,AX"                                         \
        "POP     ES"                                            \
        "STI"                                                   \
        "POPF"                                                  \
    modify [ax dx];


#if 0
extern bool __IsEgaVga( void );
#pragma aux __IsEgaVga = \
        "MOV     AX,1A00h"  /* BIOS VIDEO FUNCTION 1Ah  */  \
        "INT     10h"       /* (Read Display Code)      */  \
        "CMP     AH,1Ah"    /* Supported?               */  \
        "JE      IsEga"     /* No                       */  \
        "XOR     AX,AX"                                     \
        "CMP     BL,07h"    /* VGA w/mono display?      */  \
        "JE      IsVga"     /* Yes                      */  \
        "CMP     BL,08h"    /* VGA w/colour display?    */  \
        "JNE     IsEga"     /* No                       */  \
    "IsVga:"                                                \
        "MOV     AX,01h"                                    \
        "JMP short Last"                                    \
    "IsEga:"                                                \
        "MOV     AH,12h"    /* EGA BIOS function        */  \
        "MOV     BL,10h"                                    \
        "INT     10h"                                       \
        "CMP     BL,10h"    /* Is EGA BIOS present?     */  \
        "JNE     IsVga"     /* EGA is on the system.    */  \
        "XOR     AX,AX"                                     \
    "Last:"                                                 \
    value [al] modify [ah bx];
#endif

void    SetSequencer( void )
{
    __SetSequencer();
}

void    ResetSequencer( void )
{
    __ResetSequencer();
}

void    SetWriteMap( void )
{
    __SetWriteMap();
}

#if 0
bool    IsEgaVga( void )
{
    return( __IsEgaVga() );
}
#endif

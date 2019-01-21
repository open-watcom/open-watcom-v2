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
        "mov  dx,3C4h"  \
        "mov  ax,402h"  \
        "out  dx,ax"    \
    __parm      [] \
    __value     \
    __modify [__ax __dx]

extern void __SetSequencer( void );
#pragma aux __SetSequencer = \
        "pushf"                                                 \
        "cli"               /* Disable interrupts          */   \
        "mov  dx,3C4h"      /* Sequencer port address      */   \
        "mov  ax,402h"      /* write to map 3              */   \
        "out  dx,ax"                                            \
        "push es"                                               \
        LOAD_ES_BIOS_SEGMENT                                    \
        "mov  al,es:[487h]" /* check VC state              */   \
        "pop  es"                                               \
        "and  al,60h"       /* if it's not 0,              */   \
        "mov  ah,7"                                             \
        "jnz short L1"                                          \
        "mov  ah,5"                                             \
    "L1: mov  al,4"         /* sequential addressing       */   \
        "out  dx,ax"                                            \
        "mov  dx,3ceh"      /* Graphics Cntrl. port addr.  */   \
        "mov  ax,406h"      /* Map starts at A000:0000     */   \
        "out  dx,ax"        /* (64K mode)                  */   \
        "mov  ax,204h"      /* Select map 2 for CPU reads  */   \
        "out  dx,ax"                                            \
        "mov  ax,5"         /* Disable odd-even addressing */   \
        "out  dx,ax"                                            \
        "sti"                                                   \
        "popf"                                                  \
    __parm      [] \
    __value     \
    __modify    [__ax __dx]

extern void __ResetSequencer( void );
#pragma aux __ResetSequencer = \
        "pushf"             /* reset sequencer and cont. */ \
        "cli"                                               \
        "mov  dx,3c4h"      /* sequencer port            */ \
        "mov  ax,302h"      /* write to maps 0 and 1     */ \
        "out  dx,ax"                                        \
        "push es"                                           \
        LOAD_ES_BIOS_SEGMENT                                \
        "mov  al,es:[487h]" /* check VC state            */ \
        "and  al,60h"       /* if it's not 0,            */ \
        "mov  ah,3"                                         \
        "jnz short L1"                                      \
        "mov  ah,1"                                         \
    "L1: mov  al,4"         /* use odd-even addressing   */ \
        "out  dx,ax"                                        \
        "mov  al,7"                                         \
        "cmp  es:[449h],al" /* Get current video mode    */ \
        "mov  ah,0eh"       /* Map starts at B800:0000   */ \
        "jne short L2"                                      \
        "mov  ah,0ah"       /* Map starts at B000:0000   */ \
    "L2: mov  al,6"                                         \
        "mov  dx,3ceh"      /* controller port           */ \
        "out  dx,ax"                                        \
        "mov  ax,4"         /* read map 0                */ \
        "out  dx,ax"                                        \
        "mov  ax,1005h"     /* use odd-even addressing   */ \
        "out  dx,ax"                                        \
        "pop  es"                                           \
        "sti"                                               \
        "popf"                                              \
    __parm      [] \
    __value     \
    __modify    [__ax __dx]


#if 0
extern bool __IsEgaVga( void );
#pragma aux __IsEgaVga = \
        "mov  ax,1A00h"     /* BIOS VIDEO FUNCTION 1Ah  */  \
        "int 10h"           /* (Read Display Code)      */  \
        "cmp  ah,1Ah"       /* Supported?               */  \
        "je short IsEga"    /* No                       */  \
        "xor  ax,ax"                                        \
        "cmp  bl,7"         /* VGA w/mono display?      */  \
        "je short IsVga"    /* Yes                      */  \
        "cmp  bl,08h"       /* VGA w/colour display?    */  \
        "jne short IsEga"   /* No                       */  \
    "IsVga:"                                                \
        "mov  ax,1"                                         \
        "jmp short Last"                                    \
    "IsEga:"                                                \
        "mov  ah,12h"       /* EGA BIOS function        */  \
        "mov  bl,10h"                                       \
        "int 10h"                                           \
        "cmp  bl,10h"       /* Is EGA BIOS present?     */  \
        "jne short IsVga"   /* EGA is on the system.    */  \
        "xor  ax,ax"                                        \
    "Last:"                                                 \
    __parm      [] \
    __value     [__al] \
    __modify    [__ah __bx]
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

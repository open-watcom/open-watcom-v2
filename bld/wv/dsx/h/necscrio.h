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


/*
 * OKAY - once and for all  I am defining these here!!!!!
 */

#define NEC_DEBUG_SCREEN        1
#define NEC_USER_SCREEN         0

extern void BIOSSetPage( char );
#pragma aux BIOSSetPage =                               \
    "       cmp     al, 01h                     "       \
    "       je      Page1                       "       \
    "       mov     dx, 0                       "       \
    "       jmp short foo                       "       \
    "Page1: mov     dx, 1000h                   "       \
    "foo:   mov     ah, 0Eh                     "       \
    "       int    18h                          "       \
        parm caller [ al ]                              \
        modify [ eax edx ];


extern char BIOSGetMode();
#pragma aux BIOSGetMode =                               \
    "mov    ah, 0Bh                             "       \
    "int    18h                                 "       \
        value [ al ]                                     \
        modify [ eax ];


extern void BIOSSetMode( char );
#pragma aux BIOSSetMode =                               \
    "mov    ah, 0Ah                             "       \
    "int    18h                                 "       \
        parm [ al ]                                     \
        modify [ eax ];


extern void BIOSTextOn();
#pragma aux BIOSTextOn =                                \
    "mov    ah, 0Ch                             "       \
    "int    18h                                 "       \
        modify[ ah ];


extern void BIOSTextOff();
#pragma aux BIOSTextOff =                               \
    "mov    ah, 0Dh                             "       \
    "int    18h                                 "       \
        modify[ ah ];


extern void BIOSGraphOn();
#pragma aux BIOSGraphOn =                               \
    "mov    ah, 40h                             "       \
    "int    18h                                 "       \
        modify[ ah ];


extern void BIOSGraphOff();
#pragma aux BIOSGraphOff =                              \
    "mov    ah, 41h                             "       \
    "int    18h                                 "       \
        modify[ ah ];


extern void DoRingBell( void );
#pragma aux DoRingBell = \
        "mov ah, 0x02h " \
        "mov dl, 0x07h " \
        "int 21h      " \
        modify [ eax edx ];


extern unsigned GdcSetMask( unsigned new );
#pragma aux GdcSetMask =        \
    "pushfd"                    \
    "or         eax,eax"        \
    "jnz        set"            \
    "cli"                       \
    "jmp        done"           \
    "set:"                      \
    "sti"                       \
    "done:"                     \
    "pop        eax"            \
    "and        eax,0x200"      \
    parm [eax] value [eax]


extern void GdcDelay( void );
#pragma aux GdcDelay =                                  \
    "       jmp short dl1                       "       \
    "dl1:   jmp short dl2                       "       \
    "dl2:   jmp short dl3                       "       \
    "dl3:                                       "


extern void BIOSSetCurPos( short );
#pragma aux BIOSSetCurPos =                             \
    "mov    ah, 13h                             "       \
    "int    18h                                 "       \
        parm caller [ dx ]                              \
        modify [ ah ];


extern void BIOS_OPEN_APP( void );
#pragma aux BIOS_OPEN_APP =                             \
    "mov    ax, 0                               "       \
    "mov    cl, 0xe0                            "       \
    "int    0DCh                                "       \
        modify      [ ax cl ];


extern void BIOS_CLOSE_APP( void );
#pragma aux BIOS_CLOSE_APP =                            \
    "mov    cl, 0xe1                            "       \
    "int    0DCh                                "       \
        modify      [ ax cl ];


extern unsigned short BIOS_JIS_SHF( unsigned short );
#pragma aux BIOS_JIS_SHF =                              \
    "mov    cl, 0xf3                            "       \
    "int    0DCh                                "       \
        parm caller [ ax ]                              \
        modify      [ ax cl ]                           \
        value       [ ax ];


extern void BIOSSetCursorOn( void );
#pragma aux BIOSSetCursorOn =                           \
    "mov    ah, 11h                             "       \
    "int    18h                                 "       \
        modify [ ah ];


extern void BIOSSetCursorOff( void );
#pragma aux BIOSSetCursorOff =                          \
    "mov    ah, 12h                             "       \
    "int    18h                                 "       \
        modify [ ah ];


extern void BIOSSetCurBlinkOn( void );
#pragma aux BIOSSetCurBlinkOn =                         \
    "mov    ax, 1000h                           "       \
    "int    18h                                 "       \
        modify [ ax ];


extern void BIOSSetCurBlinkOff( void );
#pragma aux BIOSSetCurBlinkOff =                        \
    "mov    ax, 1001h                           "       \
    "int    18h                                 "       \
        modify [ ax ];

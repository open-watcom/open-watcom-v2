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
#include "charmap.h"

#pragma aux __SetWriteMap =           \
        "   MOV     DX, 3C4h    "   \
        "   MOV     AX, 0402h   "   \
        "   OUT     DX, AX      ";

#pragma aux __SetSequencer =                                                  \
        "   PUSHF                   "   /* Disable interrupts       */      \
        "   CLI                     "                                       \
        "   MOV     DX, 3C4h        "   /* Sequencer port address   */      \
        "   MOV     AX,0402h        "   /* write to map 3           */      \
        "   OUT     DX,AX           "                                       \
        "   PUSH    ES              "                                       \
        "   MOV     ES, SI          "                                       \
        "   MOV     AL,ES:[87h]     "   /* check VC state           */      \
        "   POP     ES              "                                       \
        "   AND     AL,60h          "   /* if it's not 0,           */      \
        "   JNZ     VC_st1          "                                       \
        "   MOV     AH,05h          "                                       \
        "   JMP     Short VC_st2    "                                       \
        "VC_st1:                    "                                       \
        "   MOV     AH,07h          "                                       \
        "VC_st2:                    "                                       \
        "   MOV     AL,04h          "   /* sequential addressing      */    \
        "   OUT     DX,AX           "                                       \
        "   MOV     DX, 3CEh        "   /* Graphics Cntrl. port addr. */    \
        "   MOV     AX, 0406h       "   /* Map starts at A000:0000    */    \
        "   OUT     DX, AX          "   /* (64K mode)                 */    \
        "   MOV     AX, 0204h       "   /* Select map 2 for CPU reads */    \
        "   OUT     DX, AX          "                                       \
        "   MOV     AX, 0005h       "   /* Disable odd-even addressing*/    \
        "   OUT     DX, AX          "                                       \
        "   STI                     "                                       \
        "   POPF                    "                                       \
        parm [ SI ] modify [ SI ];

#pragma aux __ResetSequencer =                                              \
        "   PUSHF                   "   /* reset sequencer and cont.*/      \
        "   CLI                     "                                       \
        "   MOV     DX, 3C4h        "   /* sequencer port           */      \
        "   MOV     AX, 0302h       "   /* write to maps 0 and 1    */      \
        "   OUT     DX, AX          "                                       \
        "   PUSH    ES              "                                       \
        "   MOV     ES, SI          "                                       \
        "   MOV     AL, ES:[87h]    "   /* check VC state           */      \
        "   AND     AL, 60h         "   /* if it's not 0,           */      \
        "   JNZ     rs_00           "                                       \
        "   MOV     AH,01h          "                                       \
        "   JMP     Short rs_00a    "                                       \
        "rs_00:                     "                                       \
        "   MOV     AH,03h          "                                       \
        "rs_00a:                    "                                       \
        "   MOV     AL,04h          "   /* use odd-even addressing  */      \
        "   OUT     DX,AX           "                                       \
        "   MOV     DX,3CEh         "   /* controller port          */      \
        "   MOV     AX,0E06h        "   /* Map starts at B800:0000  */      \
        "   MOV     BL, 7           "                                       \
        "   CMP     ES:[49h], BL   "   /* Get current video mode   */      \
        "   JNE     rs_01           "                                       \
        "   MOV     AX, 0A06h       "   /* Map starts at B000:0000  */      \
        "rs_01:                     "                                       \
        "   OUT     DX,AX           "                                       \
        "   MOV     AX,0004h        "   /* read map 0               */      \
        "   OUT     DX,AX           "                                       \
        "   MOV     AX,1005h        "   /* use odd-even addressing  */      \
        "   OUT     DX,AX           "                                       \
        "   POP     ES              "                                       \
        "   STI                     "                                       \
        "   POPF                    "                                       \
        parm [ SI ] modify [ SI ];


#if 0
#pragma aux __IsEgaVga =                                                    \
        "   MOV     AX, 1A00h       "   /* BIOS VIDEO FUNCTION 1Ah  */      \
        "   INT     10h             "   /* (Read Display Code)      */      \
        "   CMP     AH, 1Ah         "   /* Supported?               */      \
        "   JE      IsEga           "   /* No                       */      \
        "   XOR     AX, AX          "                                       \
        "   CMP     BL, 7           "   /* VGA w/mono display?      */      \
        "   JE      IsVga           "   /* Yes                      */      \
        "   CMP     BL, 8           "   /* VGA w/colour display?    */      \
        "   JNE     IsEga           "   /* No                       */      \
        "IsVga:                     "                                       \
        "   MOV     DX, 1           "                                       \
        "   JMP     Short Last      "                                       \
        "IsEga:                     "                                       \
        "   MOV     AH, 12h         "   /* EGA BIOS function        */      \
        "   MOV     BL, 10h         "                                       \
        "   INT     10h             "                                       \
        "   CMP     BL, 10h         "   /* Is EGA BIOS present?     */      \
        "   JNE     IsVga           "   /* EGA is on the system.    */      \
        "   XOR     DX, DX          "                                       \
        "Last:                      "                                       \
            modify      [ ax bx dx ]                                        \
            value       [ dx ];         /* IsEgaVga()               */
#endif

#pragma aux __CallVid =                       \
        "   INT     10h             "       \
            parm caller [ax] [bx] [cx] [dx] \
            value       [ ax ];

extern  void        __SetSequencer( unsigned );
extern  void        __ResetSequencer( unsigned );
extern  void        __SetWriteMap( void );
extern  short int   __CallVid( short int, short int, short int, short int );

void    SetSequencer( void )
{
    __SetSequencer( FP_SEG( firstmeg( BIOS_PAGE, 0 ) ) );
}

void    ResetSequencer( void )
{
    __ResetSequencer( FP_SEG( firstmeg( BIOS_PAGE, 0 ) ) );
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

short int CallVid( short int a, short int b, short int c, short int d )
{
    return( __CallVid( a, b, c, d ) );
}

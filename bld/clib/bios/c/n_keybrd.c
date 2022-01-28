/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include <bios98.h>
#include "rtdata.h"


/*
 * FUNCTION: __nec98_bios_keybrd
 * DESCRIPTION: uses int 18h to access bios keyboard functions
 *      _KEYBRD_READ - read a byte from the keyboard buffer.
 *              -if no keystroke is available wait until one is
 *      _KEYBRD_READY - check if a keystroke is available
 *              -if one is return it without removing it from the buffer
 *      _KEYBRD_SHIFTSTATUS - get the status of the shift keys
 *      _KEYBRD_INITIALIZE - initialize the keyboad buffer
 *      _KEYBRD_INPUTSTATUS - get status of the 16 key groups
 *
 *      The parameter __buf is ignored except for _KEYBRD_INPUTSTATUS
 *
 * RETURNS:
 * _KEYBRD_READ - ASCII value in low byte
 *              - Scan Code in high byte
 * _KEYBRD_READY - 0 if no character in buffer
 *               - same as _KEYBRD_READ otherwise
 * _KEYBRD_SHIFTSTATUS - low order 5 bits are significant
 *                      - bit 0         shift key pressed
 *                      - bit 1         caps key pressed
 *                      - bit 2         kana key pressed
 *                      - bit 3         graph key pressed
 *                      - bit 4         ctrl key pressed
 * _KEYBRD_INITIALIZE - no meaningful return value
 * _KEYBRD_INPUTSTATUS - status of the 16 key groups placed in __buf
 */

#if defined( _M_I86 )
#define INTX86  int86
#else
#define INTX86  int386
#endif

_WCRTLINK unsigned short __nec98_bios_keybrd( unsigned __cmd, unsigned char *__buf )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        union REGS regs;
        int i;

        switch( regs.h.ah = __cmd ) {
        case _KEYBRD_READ:
            INTX86( 0x18, &regs, &regs );
            break;
        case _KEYBRD_READY:
            INTX86( 0x18, &regs, &regs );
            if( !regs.h.bh )
                regs.w.ax = 0;
            break;
        case _KEYBRD_SHIFTSTATUS:
            INTX86( 0x18, &regs, &regs );
            regs.h.ah = 0;
            break;
        case _KEYBRD_INITIALIZE:
            INTX86( 0x18, &regs, &regs );
            break;
        default:
            for( i = 0; i < 16; i++ ){
                regs.h.al = i;
                INTX86( 0x18, &regs, &regs );
                __buf[i] = regs.h.ah;
            }
        }
        return( regs.w.ax );
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}


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


#include <i86.h>
#include "trpimp.h"

extern int GtKey();
extern unsigned KeyWaiting( void );

#if defined(_FMR_PC)
#pragma aux GtKey =             \
    "mov AL,01h"                \
    "mov AH,09h"                \
    "int 90h"                   \
    "mov AX,DX"                 \
    modify [ax bx dx];

#pragma aux KeyWaiting =        \
    "mov AH,07h"                \
    "int 90h"                   \
    modify [ax bx dx];

#elif defined(_NEC_PC)
#pragma aux GtKey =             \
    "mov ah,0"                  \
    "int 18h"                   \
    parm caller [ax];

#pragma aux KeyWaiting =        \
    "mov ah,1"                  \
    "int 18h"                   \
    "mov ax,bx"                 \
    "and ax,0100h"              \
    "xor ax,0100h"              \
    modify [ax bh]              \
    parm caller [ax];

#else
#pragma aux GtKey =             \
    "mov AH,00h"                \
    "int 16h"                   \
    parm caller [ax];

#pragma aux KeyWaiting =        \
    "mov AH,01h"                \
    "int 16h"                   \
    "lahf"                      \
    "and AX,4000h"              \
    parm caller [ax];

#endif

unsigned ReqRead_user_keyboard()
{
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->key = 0;
#ifdef _NEC_PC
        /*
            For some reason, any attempt to perform a software interrupt
            in this routine causes a crash. Therefore, we cheat and
            look at the keyboard buffer count in low memory. If it's
            non-zero, we know that a character is available to be read.
        */
        while( *(volatile unsigned char *)MK_FP( 0x50, 0x28 ) == 0 ) {
        }
#else
    if( acc->wait != 0 ) {
        unsigned long   end_time;
        unsigned long   *cur_time;

        cur_time = MK_FP( 0x40, 0x6c ); /* set up pointer to the BIOS clock */
        end_time = *cur_time + ( acc->wait * 18 );
        for( ;; ) {
            if( KeyWaiting() == 0 ) {
                break;
            } else  if( end_time <= *cur_time ) {
                return( sizeof( *ret ) );
            }
        }
    }
    ret->key = GtKey();
#endif
    return( sizeof( *ret ) );
}

unsigned ReqSet_user_screen()
{
    return( 0 );
}

unsigned ReqSet_debug_screen()
{
    return( 0 );
}

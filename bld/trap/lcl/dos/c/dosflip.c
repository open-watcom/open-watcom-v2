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
* Description:  DOS debugger screen flip support.
*
****************************************************************************/


#include <i86.h>
#include "trpimp.h"

extern int GtKey( void );
extern unsigned KeyWaiting( void );

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


unsigned ReqRead_user_keyboard( void )
{
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->key = 0;
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
    return( sizeof( *ret ) );
}

unsigned ReqSet_user_screen( void )
{
    return( 0 );
}

unsigned ReqSet_debug_screen( void )
{
    return( 0 );
}

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


#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include "uidef.h"


#ifndef __386__
#pragma aux vertsync =                                          \
                    0xba 0xda 0x03      /* mov dx,3da   */      \
                    0xec                /* in al,dx     */      \
                    0xa8 0x08           /* test al,8    */      \
                    0x74 0xfb           /* jz -5        */      \
                modify [ax dx];
extern void vertsync( void );
#endif

void intern vertretrace( void )
/*****************************/
{
#ifndef __386__
    if( ( UIData->colour == M_CGA ) && ( UIData->no_snow == FALSE ) ) {
        /* wait for vertical retrace */
        vertsync();
    }
#endif
}

bool intern issnow( BUFFER *bptr )
/********************************/
{
    register    bool                    snow;

    snow = FALSE;
    if( isscreen( bptr ) ) {
        uioffmouse();
        if( ( UIData->colour == M_CGA ) && ( UIData->no_snow == FALSE ) ) {
            snow = TRUE;
        }
    }
    return( snow );
}


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


#include <conio.h>
#include <dos.h>
#include "uidef.h"


#ifdef _M_I86
extern void vertsync( void );
#pragma aux vertsync = \
        "mov  dx,3dah"  \
    "L1: in   al,dx"    \
        "test al,8"     \
        "jz short L1"   \
    __parm      [] \
    __value     \
    __modify    [__ax __dx]
#endif

void intern vertretrace( void )
/*****************************/
{
#ifdef _M_I86
    if( ( UIData->colour == M_CGA ) && !UIData->no_snow ) {
        /* wait for vertical retrace */
        vertsync();
    }
#endif
}

bool intern issnow( BUFFER *bptr )
/********************************/
{
    bool            snow;

    snow = false;
    if( isscreen( bptr ) ) {
        uioffmouse();
        if( ( UIData->colour == M_CGA ) && !UIData->no_snow ) {
            snow = true;
        }
    }
    return( snow );
}

/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS mouse help routines for VI
*
****************************************************************************/


#include "vi.h"
#include <i86.h>
#include "mouse.h"
#include "int33.h"

/*
 * SetMouseSpeed - set mouse movement speed
 */
void SetMouseSpeed( int speed )
{
    _BIOSMouseSetMickeysToPixelsRatio( speed, speed * 2 );
    EditVars.MouseSpeed = speed;

} /* SetMouseSpeed */

/*
 * SetMousePosition - set the mouse position
 */
void SetMousePosition( windim row, windim col )
{
    MouseRow = row;
    MouseCol = col;
    _BIOSMouseSetPointerPosition( col * MOUSE_SCALE, row * MOUSE_SCALE );

} /* SetMousePosition */

/*
 * PollMouse - poll the mouse for it's state
 */
void PollMouse( int *status, windim *row, windim *col )
{
    mouse_status        state;

    _BIOSMouseGetPositionAndButtonStatus( &state );
    *status = state.button_status;
    *col = state.x / MOUSE_SCALE;
    *row = state.y / MOUSE_SCALE;

} /* PollMouse */

/*
 * InitMouse - initialize the mouse
 */
void InitMouse( void )
{
    int             and_mask;
    int             or_mask;
    unsigned short  _FAR *vector;
    unsigned char   _FAR *intrtn;

    if( !EditFlags.UseMouse ) {
        return;
    }

#if defined( _M_I86 )
    vector = _MK_FP( 0, VECTOR_MOUSE * 4 );
    intrtn = _MK_FP( vector[1], vector[0] );
#elif defined( __4G__ )
    vector = (unsigned short _FAR *)(VECTOR_MOUSE * 4);
    intrtn = (unsigned char _FAR *)((((unsigned)vector[1]) << 4) + vector[0]);
#else
    vector = _MK_FP( 0x34, VECTOR_MOUSE * 4 );
    intrtn = _MK_FP( 0x34, (((unsigned) vector[1]) << 4) + vector[0]);
#endif
    if( ( intrtn == NULL ) || ( *intrtn == 0xcf ) ) {
        EditFlags.UseMouse = false;
        return;
    }

    _BIOSMouseDriverResetSoft();

    _BIOSMouseSetHorizontalLimitsForPointer( 0, (EditVars.WindMaxWidth - 1) * MOUSE_SCALE );
    _BIOSMouseSetVerticalLimitsForPointer( 0, (EditVars.WindMaxHeight - 1) * MOUSE_SCALE );

    and_mask = ( EditFlags.Monocolor ? 0x79ff : 0x7fff );
    or_mask = ( EditFlags.Monocolor ? 0x7100 : 0x7700 );
    _BIOSMouseSetTextPointerType( SOFTWARE_CURSOR, and_mask, or_mask );
    _BIOSMouseSetPointerExclusionArea( 0, 0, 0, 0 );
    SetMousePosition( EditVars.WindMaxWidth / 2 - 1, EditVars.WindMaxHeight / 2 - 1 );
    SetMouseSpeed( EditVars.MouseSpeed );
    PollMouse( &MouseStatus, &MouseRow, &MouseCol );

} /* InitMouse */

/*
 * FiniMouse - done with the mouse
 */
void FiniMouse( void )
{
} /* FiniMouse */

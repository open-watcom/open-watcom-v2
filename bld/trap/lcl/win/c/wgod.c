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


#include <stdio.h>
#include <dos.h>
#include <string.h>
#include "stdwin.h"
#include "wdebug.h"

/*
 * CopyMemory
 */
short CopyMemory( WORD dseg, DWORD doff, WORD sseg, DWORD soff, short size )
{
    CopySize = size;
    return( _CopyMemory( dseg, doff, sseg, soff ) );

} /* CopyMemory */

/*
 * StartWDebug386 - see if wgod is here
 */
void StartWDebug386( void )
{
    if( CheckWin386Debug() != WGOD_VERSION ) return;
    WDebug386 = TRUE;
    UseHotKey( 1 );

} /* StartWDebug386 */

/*
 * KillWDebug386 - done with wgod
 */
void KillWDebug386( void )
{
    if( WDebug386 ) {
        WDebug386 = FALSE;
        UseHotKey( 0 );
    }

} /* KillWDebug386 */

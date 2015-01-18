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
#include <string.h>
#include "pcscrnio.h"
#include "dbgdefn.h"
#include "kbio.h"
#include "dbgmem.h"
#include "stdui.h"
#include "dosscrn.h"
#include "tinyio.h"
#include "dbgswtch.h"
#include "dbginstr.h"

/*
Warning(1028): _ActFontTbls is an undefined reference
Warning(1028): ForceLines_ is an undefined reference
Warning(1028): _ScrnMode is an undefined reference
Warning(1028): _FlipMech is an undefined reference
Warning(1028): ConfigScreen_ is an undefined reference
Warning(1028): RingBell_ is an undefined reference
Warning(1028): InitScreen_ is an undefined reference
Warning(1028): FiniScreen_ is an undefined reference
Warning(1028): UsrScrnMode_ is an undefined reference
Warning(1028): UserScreen_ is an undefined reference
Warning(1028): DebugScreen_ is an undefined reference
Warning(1028): DbgScrnMode_ is an undefined reference
*/

flip_types          FlipMech = FLIP_OVERWRITE;
mode_types          ScrnMode = MD_COLOUR;

char                ActFontTbls;  /* assembly file needs access */

/*
 * ForceLines -- force a specified number of lines for MDA/CGA systems
 */

void ForceLines( unsigned lines )
{
    lines = lines;
}

/*
 * ConfigScreen -- figure out screen configuration we're going to use.
 */

unsigned ConfigScreen( void )
{
    return( 25 * (80 * 2) + 256 );
}

void Ring_Bell( void )
{
}

/*
 * InitScreen
 */

void InitScreen( void )
{
}

/*
 * FiniScreen -- finish screen swapping/paging
 */

void FiniScreen( void )
{
}

void DbgScrnMode( void )
{
}

/*
 * DebugScreen -- swap/page to debugger screen
 */

bool DebugScreen( void )
{
    bool    usr_vis;

    usr_vis = TRUE;
    uiswap();
    return( usr_vis );
}

/*
 * UsrScrnMode -- setup the user screen mode
 */

bool UsrScrnMode( void )
{
    bool    usr_vis;

    usr_vis = FALSE;
    return( usr_vis );
}

/*
 * UserScreen -- swap/page to user screen
 */

bool UserScreen( void )
{
    bool    dbg_vis;

    dbg_vis = TRUE;
    uiswap();
    return( dbg_vis );
}


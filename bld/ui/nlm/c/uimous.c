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
* Description:  Netware mouse input handling.
*
****************************************************************************/


#include <dos.h>
#include "uidef.h"
#include "uimouse.h"


void intern mousespawnstart( void )
/*********************************/
{
}

void intern mousespawnend( void )
/*******************************/
{
}

void intern checkmouse( MOUSESTAT *pstatus, MOUSEORD *prow, MOUSEORD *pcol, MOUSETIME *ptime )
/********************************************************************************************/
{
    /* unused parameters */ (void)pstatus; (void)prow; (void)pcol; (void)ptime;
}

void uimousespeed( unsigned speed )
/*********************************/
{
    /* unused parameters */ (void)speed;
}

bool UIAPI initmouse( init_mode install )
/***************************************/
{
    /* unused parameters */ (void)install;

    MouseInstalled = false;
    return( false );
}

void UIAPI finimouse( void )
/**************************/
{
}

void UIAPI uisetmouseposn( ORD row, ORD col )
{
    /* unused parameters */ (void)row; (void)col;
}

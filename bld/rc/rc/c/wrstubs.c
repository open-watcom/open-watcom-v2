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
* Description:  Stub routines for GUI resource editors (those only support
*               Win16 and Win32 resources, not OS/2 resources).
*
****************************************************************************/


#include "global.h"
#include "rccore.h"
#include "semsingl.h"
#include "exelxobj.h"


void SemOS2WriteFontDir( void )
{
}

void SemOS2WriteStringTable( FullStringTable *currtable, WResID *type )
{
}

bool RcBuildLXResourceObjects( void )
{
    return( true );
}

bool CopyLXExeObjects( void )
{
    return( true );
}

RcStatus RcWriteLXResourceObjects( void )
{
    return( RS_WRITE_ERROR );
}

RcStatus CopyOS2Resources( void )
{
    return( RS_READ_ERROR );
}

RcStatus InitOS2ResTable( int *err_code )
{
    return( RS_READ_ERROR );
}

RcStatus WriteOS2ResTable( int handle, OS2ResTable *restab, int *err_code )
{
    return( RS_READ_ERROR );
}

uint_32 ComputeOS2ResSegCount( WResDir dir )
{
    return( 0 );
}

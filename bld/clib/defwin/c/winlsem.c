/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include <wwindows.h>
#include "rtinit.h"
#include "win.h"


static HANDLE __WinLines_sem;

static void __InitWinLinesSem( void )
{
    __WinLines_sem = CreateMutex( NULL, FALSE, NULL );
}

static void __FiniWinLinesSem( void )
{
    CloseHandle( __WinLines_sem );
}

void _AccessWinLines( void )
{
    WaitForSingleObject( __WinLines_sem, INFINITE );
}

void _ReleaseWinLines( void )
{
    ReleaseMutex( __WinLines_sem );
}

AXI( __InitWinLinesSem, INIT_PRIORITY_LIBRARY )
AYI( __FiniWinLinesSem, INIT_PRIORITY_LIBRARY )

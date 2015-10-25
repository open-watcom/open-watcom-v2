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
* Description:  RDOS startup module
*
****************************************************************************/


#include <stdio.h>
#include <process.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <direct.h>
#include <stdarg.h>
#include "rdos.h"
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include "autoenv.h"
#include "strutil.h"
#include "dbgmain.h"
#include "dbginit.h"

#include "clibint.h"

static char       CmdData[256];

void GUImain( void )
{
    const char *cmdline;

    // fix up env vars if necessary
    watcom_setup_env();

    cmdline = RdosGetCmdLine();
    if( cmdline ) {
        while( *cmdline != 0 && *cmdline != ' ' && *cmdline != 0x9 ) 
            cmdline++;
    
        strcpy( CmdData, cmdline );
    } else
        CmdData[0] = 0;

    DebugMain();
}


int GUISysInit( int param )
{
    param=param;
    return( 1 );
}

void GUISysFini( void  )
{
    DebugFini();
}

void WndCleanUp( void )
{
}

char *GetCmdArg( int num )
{
    if( num != 0 ) return( NULL );
    return( CmdData );
}

void SetCmdArgStart( int num, char *ptr )
{
    num = num;

    if( ptr )
        strcpy( CmdData, ptr );
    else
        CmdData[0] = 0;
}

void KillDebugger( int ret_code )
{
    RdosUnloadExe( ret_code );
}

void GrabHandlers( void )
{
}

void RestoreHandlers( void )
{
}

long _fork( const char *cmd, size_t len )
{
    cmd = cmd; len = len;
    return( 0 );
}

bool TBreak( void )
{
    return( false );
}

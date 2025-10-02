/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS specific trap I/O.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "tinyio.h"
#include "trptypes.h"
#include "digcli.h"
#include "digld.h"
#include "servio.h"
#include "int16.h"


void OutputLine( const char *str )
{
    TinyWrite( TINY_ERR, str, strlen( str ) );
    TinyWrite( TINY_ERR, "\r\n", 2 );
}

void ServTerminate( int return_code )
{
    TinyTerminateProcess( return_code );
    // never return
}

void StartupErr( const char *err )
{
    OutputLine( err );
}

int KeyPress( void )
{
    return( _BIOSKeyboardHit( KEYB_STD ) );
}

int KeyGet( void )
{
    unsigned short value;

    value = _BIOSKeyboardGet( KEYB_STD );
    return( value & 0x00ff );
}

int WantUsage( const char *ptr )
{
    if( (*ptr == '-') || (*ptr == '/') )
        ++ptr;
    return( *ptr == '?' );
}

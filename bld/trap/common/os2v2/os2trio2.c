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
* Description:  OS/2 2.x system specific trap I/O.
*
****************************************************************************/


#include <conio.h>
#include <stddef.h>
#define INCL_DOSPROCESS
#include <os2.h>
#include "servio.h"

void Output( const char *str )
{
    while( *str ) {
        putch( *str );
        ++str;
    }
}

void SayGNiteGracey( int return_code )
{
    DosExit( 1, return_code );
}

void StartupErr( const char *err )
{
    Output( err );
    Output( "\r\n" );
    SayGNiteGracey( 1 );
}

int KeyPress()
{
    return( kbhit() );
}

int KeyGet()
{
    return( getch() );
}


int WantUsage( const char *ptr )
{
    LONG    lReq = 20;
    ULONG   ulCurMax;

    /* This is a stupid place to do this, but it's the only system
       specific hook that I've got. */
    DosSetRelMaxFH( &lReq, &ulCurMax );

    if( (*ptr == '-') || (*ptr == '/') )
        ++ptr;
    return( *ptr == '?' );
}

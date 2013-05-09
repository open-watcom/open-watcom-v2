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


#include <windows.h>
#include "nmp.h"
#include "bool.h"

bool Terminate( void )
{
    // a sideways dive to terminate the link (with failure)
    return( FALSE );
}

bhandle myopen( char *name )
{
    return( CreateFile( name, GENERIC_READ+GENERIC_WRITE,
                        FILE_SHARE_READ+FILE_SHARE_WRITE, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) );
}

void myclose( bhandle handle )
{
    CloseHandle( handle );
}

int myread( bhandle handle, char *buff, int len )
{
    DWORD               bytes;

    if( !ReadFile( handle, buff, len, &bytes, NULL ) ) {
        bytes = 0;
    }
    return( bytes );
}


int mywrite( bhandle handle, char *buff, int len )
{
    DWORD               bytes;

    if( !WriteFile( handle, buff, len, &bytes, NULL ) ) {
        bytes = 0;
    }
    return( bytes );
}

void mysnooze()
{
    Sleep( 100 );
}

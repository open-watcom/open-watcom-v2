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
* Description: get console dimensions 
*
****************************************************************************/

#if defined( __OS_dosos2__ ) || defined( __OS_os2v2__ )
#define INCL_SUB
#include <os2.h>

int GetConsoleWidth()
{
    struct _VIOMODEINFO vio_mode;

    vio_mode.cb = sizeof( vio_mode );
    if( VioGetMode( &vio_mode, 0 ) != 0 ) {
        return( 0 );
    }
    return( vio_mode.col );
}

int GetConsoleHeight()
{
    struct _VIOMODEINFO vio_mode;

    vio_mode.cb = sizeof( vio_mode );
    if( VioGetMode( &vio_mode, 0 ) != 0 ) {
        return( 0 );
    }
    return( vio_mode.row );
}
#elif defined( __OS_nt__ )
    #include <windows.h>

int GetConsoleWidth()
{
    CONSOLE_SCREEN_BUFFER_INFO  buffer_info;
    HANDLE                      output_handle;

    output_handle = GetStdHandle( STD_OUTPUT_HANDLE );
    GetConsoleScreenBufferInfo( output_handle, &buffer_info );
    return( buffer_info.dwMaximumWindowSize.X );
}

int GetConsoleHeight()
{
    CONSOLE_SCREEN_BUFFER_INFO  buffer_info;
    HANDLE                      output_handle;

    output_handle = GetStdHandle( STD_OUTPUT_HANDLE );
    GetConsoleScreenBufferInfo( output_handle, &buffer_info );
    return( buffer_info.dwMaximumWindowSize.Y );
}
#elif defined( __OS_dos__ ) || defined( __OS_pharlap__ )
unsigned char getVideoMode( void );
    #pragma aux getVideoMode = \
            "mov ah, 0fh" \
            "int 010h" \
            value [ah] modify[bx];

int GetConsoleWidth( void )
{
    return( getVideoMode() );
}

unsigned char getRowCount( void );
    #pragma aux getRowCount = \
                "push   es" \
                "push   bp" \
                "mov    ax, 01130h" \
                "mov    bh, 0" \
                "mov    dl, 018h" \
                "int    010h" \
                "pop    bp" \
                "pop    es" \
                value [dl] modify [ax bx cx dx];

int GetConsoleHeight( void )
{
    return( getRowCount() + 1 );
}
#else
int GetConsoleWidth( void )
{
    return( 80 );
}

int GetConsoleHeight( void )
{
    return( 25 );
}
#endif

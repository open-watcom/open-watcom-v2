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


#include "variety.h"
#if defined(__WINDOWS__) || defined(__WINDOWS_386__) || defined(__NT__)
    #include <windows.h>
    #include "osver.h"
#endif
#include "hostos.h"


#if defined(__WINDOWS__) || defined(__WINDOWS_386__) || defined(__NT__)

static int win_nt( void )
/***********************/
{
    #ifdef __NT__                               /* Win32 app */
        return( WIN32_IS_NT );
    #else                                       /* Win16 app */
        DWORD           flags;

        flags = GetWinFlags() & 0x4000;
        if( flags > 0 ) {
            return( 1 );
        } else {
            return( 0 );
        }
    #endif
}


static int win_95( void )
/***********************/
{
    #ifdef __NT__                               /* Win32 app */
        return( WIN32_IS_WIN95 );
    #else                                       /* Win16 app */
        DWORD           ver;

        ver = GetVersion() & 0xFFFF;
        if( ver/256 == 95 ) {
            return( 1 );
        } else {
            return( 0 );
        }
    #endif
}


static int win_32s( void )
/************************/
{
    #ifdef __NT__                               /* Win32 app */
        return( WIN32_IS_WIN32S );
    #else                                       /* Win16 app */
        return( 0 );
    #endif
}


static int win_3x( void )
/************************/
{
    #if defined(__WINDOWS__) || defined(__WINDOWS_386__)
        if( !win_nt() && !win_95() ) {
            return( 1 );
        } else {
            return( 0 );
        }
    #else
        return( 0 );
    #endif
}

#endif


_WCRTLINK int _host_os_id( void )
/*******************************/
{
    int                 id;

    #if defined(__WINDOWS__) || defined(__WINDOWS_386__) || defined(__NT__)
        if( win_nt() ) {
            id = HOST_OS_WINNT;
        } else if( win_95() ) {
            id = HOST_OS_WIN95;
        } else if( win_32s() ) {
            id = HOST_OS_WIN32S;
        } else if( win_3x() ) {
            id = HOST_OS_WIN3X;
        } else {
            id = HOST_OS_WIN_UNKNOWN;
        }
    #else
        #error Unknown system.
    #endif

    return( id );
}


_WCRTLINK const char *_host_os_name( void )
/*****************************************/
{
    const char *        osname;
    int                 id = _host_os_id();

    #if defined(__WINDOWS__) || defined(__WINDOWS_386__) || defined(__NT__)
        if( id == HOST_OS_WINNT ) {
            osname = "Windows NT";
        } else if( id == HOST_OS_WIN95 ) {
            osname = "Windows 95";
        } else if( id == HOST_OS_WIN32S ) {
            osname = "Win32s under Windows 3.x";
        } else if( id == HOST_OS_WIN3X ) {
            osname = "Windows 3.x";
        } else {
            osname = "Unknown Windows version";
        }
    #else
        #error Unknown system.
    #endif

    return( osname );
}

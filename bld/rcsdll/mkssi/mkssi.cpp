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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "rcsdll.hpp"

mksSISystem MksSI;

typedef int (WINAPI *WSICHECKIN)(HWND main_window_handle, int files, char const FAR * FAR flist[] );
typedef int (WINAPI *WSICHECKOUT)(HWND main_window_handle, int files, char const FAR * FAR flist[] );
typedef int (WINAPI *WSILAUNCH)(LPSTR filelist);
typedef int (WINAPI *WSIINIT)(void);
typedef int (WINAPI *WSICLEANUP)(void);

static WSICHECKIN       ci_fp = NULL;
static WSICHECKOUT      co_fp = NULL;
static WSILAUNCH        rs_fp = NULL;
static WSIINIT          in_fp = NULL;
static WSICLEANUP       cl_fp = NULL;

int mksSISystem::init( userData * )
{
    HINSTANCE dll;

    dll = LoadLibrary( "WSIHOOK.DLL" );

#ifdef __WINDOWS__
    if( (UINT)dll < 32 )
        return( false );
    ci_fp = (WSICHECKIN)GetProcAddress( dll, "WSICHECKIN" );
    co_fp = (WSICHECKOUT)GetProcAddress( dll, "WSICHECKOUT" );
    rs_fp = (WSILAUNCH)GetProcAddress( dll, "WSILAUNCH" );
    in_fp = (WSIINIT)GetProcAddress( dll, "WSIINIT" );
    cl_fp = (WSICLEANUP)GetProcAddress( dll, "WSICLEANUP" );
#else
    if( dll == NULL )
        return( false );
    ci_fp = (WSICHECKIN)GetProcAddress( dll, "wsiCheckIn" );
    co_fp = (WSICHECKOUT)GetProcAddress( dll, "wsiCheckOut" );
    rs_fp = (WSILAUNCH)GetProcAddress( dll, "wsiLaunch" );
    in_fp = (WSIINIT)GetProcAddress( dll, "wsiInit" );
    cl_fp = (WSICLEANUP)GetProcAddress( dll, "wsiCleanup" );
#endif
    dllId = (long)dll;

    if( in_fp == NULL )
        return( false );
    in_fp();
    return( true );
};

int mksSISystem::fini()
{
    if( cl_fp != NULL ) cl_fp();
    FreeLibrary( (HINSTANCE)dllId );
    return( true );
};

int mksSISystem::checkout( userData *d, rcsstring name,
                            rcsstring pj, rcsstring tgt )
{
    pj = pj; tgt = tgt;
    if( d == NULL )
        return( false );
    if( co_fp == NULL )
        return( false );
    return( (*co_fp)( (HWND)d->window, 1, &name ) );
}
int mksSISystem::checkin( userData *d, rcsstring name,
                            rcsstring pj, rcsstring tgt )
{
    pj = pj; tgt = tgt;
    if( d == NULL )
        return( false );
    if( ci_fp == NULL )
        return( false );
    return( (*ci_fp)( (HWND)d->window, 1, &name ) );
}

int mksSISystem::runShell()
{
    if( rs_fp == NULL )
        return( false );
    return( (*rs_fp)( NULL ) );
}

#ifdef __WATCOMC__
// Complain about defining trivial constructor inside class
// definition only for warning levels above 8 
#pragma warning 657 9
#endif

mksSISystem::~mksSISystem() {
};


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

objectCycleSystem ObjCycle;

typedef int WINAPI (*OCINIT)( long instance, HWND window );
typedef int WINAPI (*OCFINI)( void );
typedef int WINAPI (*OCCHECKIN)( const char *fname, const char *objname );
typedef int WINAPI (*OCCHECKOUT)( const char *fname, const char *objname );
typedef int WINAPI (*OCRUNOCM)( void );

static OCCHECKIN        ci_fp = NULL;
static OCCHECKOUT       co_fp = NULL;
static OCRUNOCM         rs_fp = NULL;
static OCINIT           in_fp = NULL;
static OCFINI           cl_fp = NULL;

int objectCycleSystem::init( userData *d )
{
    HINSTANCE dll;

    dll = LoadLibrary( "OCHOOK.DLL" );

#ifdef __WINDOWS__
    if( (UINT)dll < 32 ) return( FALSE );
    ci_fp = (OCCHECKIN)GetProcAddress( dll, "OCCHECKIN" );
    co_fp = (OCCHECKOUT)GetProcAddress( dll, "OCCHECKOUT" );
    rs_fp = (OCRUNOCM)GetProcAddress( dll, "OCRUNOCM" );
    in_fp = (OCINIT)GetProcAddress( dll, "OCINIT" );
    cl_fp = (OCFINI)GetProcAddress( dll, "OCFINI" );
#else
    if( dll == NULL ) return( FALSE );
    ci_fp = (OCCHECKIN)GetProcAddress( dll, "_OCCheckin@8" );
    co_fp = (OCCHECKOUT)GetProcAddress( dll, "_OCCheckout@8" );
    rs_fp = (OCRUNOCM)GetProcAddress( dll, "_OCRunOCM@0" );
    in_fp = (OCINIT)GetProcAddress( dll, "_OCInit@8" );
    cl_fp = (OCFINI)GetProcAddress( dll, "_OCFini@0" );
#endif
    dllId = (long)dll;

    if( in_fp == NULL ) return( FALSE );
    in_fp( dllId, (HWND)d->window );
    return( TRUE );
};

int objectCycleSystem::fini()
{
    if( cl_fp != NULL ) cl_fp();
    FreeLibrary( (HINSTANCE)dllId );
    return( TRUE );
};

int objectCycleSystem::checkout( userData *d, rcsstring name,
                            rcsstring pj, rcsstring tgt )
{
    pj = pj; tgt = tgt;
    if( d == NULL ) return( FALSE );
    if( co_fp == NULL ) return( FALSE );
    return( (*co_fp)( name, NULL ) );
}
int objectCycleSystem::checkin( userData *d, rcsstring name,
                            rcsstring pj, rcsstring tgt )
{
    pj = pj; tgt = tgt;
    if( d == NULL ) return( FALSE );
    if( ci_fp == NULL ) return( FALSE );
    return( (*ci_fp)( name, NULL ) );
}

int objectCycleSystem::runShell()
{
    if( rs_fp == NULL ) return( FALSE );
    return( (*rs_fp)() );
};

// Complain about defining trivial constructor inside class
// definition only for warning levels above 8 
#pragma warning 657 9

objectCycleSystem::~objectCycleSystem() {
};

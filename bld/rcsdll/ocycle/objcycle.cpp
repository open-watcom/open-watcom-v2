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

#ifdef __WINDOWS__
#define ENTRY_OCCHECKIN     "OCCHECKIN"
#define ENTRY_OCCHECKOUT    "OCCHECKOUT"
#define ENTRY_OCRUNOCM      "OCRUNOCM"
#define ENTRY_OCINIT        "OCINIT"
#define ENTRY_OCFINI        "OCFINI"
#elif defined( __NT__ ) && defined( __386__ )
#define ENTRY_OCCHECKIN     "_OCCheckin@8"
#define ENTRY_OCCHECKOUT    "_OCCheckout@8"
#define ENTRY_OCRUNOCM      "_OCRunOCM@0"
#define ENTRY_OCINIT        "_OCInit@8"
#define ENTRY_OCFINI        "_OCFini@0"
#else
#define ENTRY_OCCHECKIN     "OCCheckin"
#define ENTRY_OCCHECKOUT    "OCCheckout"
#define ENTRY_OCRUNOCM      "OCRunOCM"
#define ENTRY_OCINIT        "OCInit"
#define ENTRY_OCFINI        "OCFini"
#endif

objectCycleSystem ObjCycle;

typedef int (WINAPI *OCINIT)( long instance, HWND window );
typedef int (WINAPI *OCFINI)( void );
typedef int (WINAPI *OCCHECKIN)( const char *fname, const char *objname );
typedef int (WINAPI *OCCHECKOUT)( const char *fname, const char *objname );
typedef int (WINAPI *OCRUNOCM)( void );

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
    if( dll == NULL )
        return( false );
#else
    if( (UINT)dll < 32 )
        return( false );
#endif
    ci_fp = (OCCHECKIN)GetProcAddress( dll, ENTRY_OCCHECKIN );
    co_fp = (OCCHECKOUT)GetProcAddress( dll, ENTRY_OCCHECKOUT );
    rs_fp = (OCRUNOCM)GetProcAddress( dll, ENTRY_OCRUNOCM );
    in_fp = (OCINIT)GetProcAddress( dll, ENTRY_OCINIT );
    cl_fp = (OCFINI)GetProcAddress( dll, ENTRY_OCFINI );

    dllId = (long)dll;

    if( in_fp == NULL )
        return( false );
    in_fp( dllId, (HWND)d->window );
    return( true );
};

int objectCycleSystem::fini()
{
    if( cl_fp != NULL )
        cl_fp();
    FreeLibrary( (HINSTANCE)dllId );
    return( true );
};

int objectCycleSystem::checkout( userData *d, rcsstring name,
                            rcsstring pj, rcsstring tgt )
{
    pj = pj; tgt = tgt;
    if( d == NULL )
        return( false );
    if( co_fp == NULL )
        return( false );
    return( (*co_fp)( name, NULL ) );
}
int objectCycleSystem::checkin( userData *d, rcsstring name,
                            rcsstring pj, rcsstring tgt )
{
    pj = pj; tgt = tgt;
    if( d == NULL )
        return( false );
    if( ci_fp == NULL )
        return( false );
    return( (*ci_fp)( name, NULL ) );
}

int objectCycleSystem::runShell()
{
    if( rs_fp == NULL )
        return( false );
    return( (*rs_fp)() );
};

#ifdef __WATCOMC__
// Complain about defining trivial constructor inside class
// definition only for warning levels above 8 
#pragma warning 657 9
#endif

objectCycleSystem::~objectCycleSystem() {
};

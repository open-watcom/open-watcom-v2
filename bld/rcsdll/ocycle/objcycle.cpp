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

#ifdef __WINDOWS__
        #define FNTYPE __pascal
#else
        #define FNTYPE __stdcall
#endif

typedef int FNTYPE (*OCINIT)( long instance, HWND window );
typedef int FNTYPE (*OCFINI)( void );
typedef int FNTYPE (*OCCHECKIN)( const char *fname, const char *objname );
typedef int FNTYPE (*OCCHECKOUT)( const char *fname, const char *objname );
typedef int FNTYPE (*OCRUNOCM)( void );

static OCCHECKIN        ci_fp = NULL;
static OCCHECKOUT       co_fp = NULL;
static OCRUNOCM         rs_fp = NULL;
static OCINIT           in_fp = NULL;
static OCFINI           cl_fp = NULL;

int objectCycleSystem::init( userData *d )
{
    dllId = (long)LoadLibrary( "OCHOOK.DLL" );
    if( dllId < HINSTANCE_ERROR ) return( FALSE );

#ifdef __WINDOWS__
    ci_fp = (OCCHECKIN)GetProcAddress( (HINSTANCE)dllId, "OCCHECKIN" );
    co_fp = (OCCHECKOUT)GetProcAddress( (HINSTANCE)dllId, "OCCHECKOUT" );
    rs_fp = (OCRUNOCM)GetProcAddress( (HINSTANCE)dllId, "OCRUNOCM" );
    in_fp = (OCINIT)GetProcAddress( (HINSTANCE)dllId, "OCINIT" );
    cl_fp = (OCFINI)GetProcAddress( (HINSTANCE)dllId, "OCFINI" );
#else
    ci_fp = (OCCHECKIN)GetProcAddress( (HINSTANCE)dllId, "_OCCheckin@8" );
    co_fp = (OCCHECKOUT)GetProcAddress( (HINSTANCE)dllId, "_OCCheckout@8" );
    rs_fp = (OCRUNOCM)GetProcAddress( (HINSTANCE)dllId, "_OCRunOCM@0" );
    in_fp = (OCINIT)GetProcAddress( (HINSTANCE)dllId, "_OCInit@8" );
    cl_fp = (OCFINI)GetProcAddress( (HINSTANCE)dllId, "_OCFini@0" );
#endif

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

objectCycleSystem::~objectCycleSystem() {};

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

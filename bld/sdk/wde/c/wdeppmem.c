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
* Description:  Memory allocation client routines for preprocessor.
*
****************************************************************************/


#include "wdeglbl.h"
#include "wrdll.h"
#include "wdestken.h"
#include "wdedebug.h"

static jmp_buf Env;

void PP_OutOfMemory( void )
{
    if( WdePopEnv( &Env ) ) {
        longjmp( Env, 1 );
    } else {
        WdeWriteTrail( "Wde PreProc: Fatal error!" );
        exit( -1 );
    }
}

void *PP_Malloc( unsigned size )
{
    void        *p;

    p = WRMemAlloc( size );
    if( p == NULL ) {
        PP_OutOfMemory();
    }
    return( p );
}

void PP_Free( void *p )
{
    WRMemFree( p );
}

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
* Description:  Several functions taking Suicide()-using function parameters
*
****************************************************************************/


#include <stdlib.h>
#include <setjmp.h>
#include "spawn.h"

extern void PopErrBox( const char *buff );

static jmp_buf  *ExitSP;

/*
 * Spawn* - mark a level to pop back to on an error.
 *
 * These functions only differ in the specification of the called function.
 * The defining declaration used to be made by a call of a macro.
 * Debugger-stepping through the code could not be done with that technique.
 * 
 */
int Spawn( wv_spawn_func *func )
{
    jmp_buf env;
    jmp_buf *old;
    int     ret;

    old = ExitSP;
    ExitSP = &env;
    if( setjmp( env ) == 0 ) {
        func();
        ret = 0;
    } else {
        ret = 1;
    }
    ExitSP = old;
    return( ret );
}

int SpawnP( wv_spawn_funcP *func, void *parm )
{
    jmp_buf env;
    jmp_buf *old;
    int     ret;

    old = ExitSP;
    ExitSP = &env;
    if( setjmp( env ) == 0 ) {
        func( parm );
        ret = 0;
    } else {
        ret = 1;
    }
    ExitSP = old;
    return( ret );
}

int SpawnPP( wv_spawn_funcPP *func, void *p1, void *p2 )
{
    jmp_buf env;
    jmp_buf *old;
    int     ret;

    old = ExitSP;
    ExitSP = &env;
    if( setjmp( env ) == 0 ) {
        func( p1, p2 );
        ret = 0;
    } else {
        ret = 1;
    }
    ExitSP = old;
    return( ret );
}

void Suicide( void )
{
    if( ExitSP != NULL ) {
        longjmp( *ExitSP, 1 );
    } else {
        PopErrBox( "Internal Error: missing Spawn" );
    }
}

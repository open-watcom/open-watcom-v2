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


#include <setjmp.h>

static jmp_buf          *ExitSP;

/*
 * Spawn - mark a level to pop back to on an error
 */

#define SpawnMacro( Header, Call ) \
int Header \
{ \
    jmp_buf env; \
    jmp_buf *old; \
    int     ret; \
 \
    old = ExitSP; \
    ExitSP = env; \
    if( setjmp( env ) == 0 ) { \
        Call; \
        ret = 0; \
    } else { \
        ret = 1; \
    } \
    ExitSP = old; \
    return( ret ); \
}

SpawnMacro( ( Spawn( void (*func)(void) ) ), func() )
SpawnMacro( ( SpawnP( void (*func)(void*), void *parm ) ), func( parm ) )
SpawnMacro( ( SpawnPP( void (*func)(void*, void*), void *p1, void *p2 ) ), func( p1, p2 ) )

extern void     PopErrBox(char*);

void Suicide()
{
    if( ExitSP ) {
        longjmp( ExitSP, 1 );
    } else {
        PopErrBox( "Internal Error: missing Spawn" );
    }
}

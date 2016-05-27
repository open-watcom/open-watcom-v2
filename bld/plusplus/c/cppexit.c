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


#include "plusplus.h"

#include <setjmp.h>

#include "cppexit.h"

#define MAX_EXITS 2

static bool exiting;                            // true ==> in exit phase
static void (*exit_list[MAX_EXITS])( void );    // exit list
static unsigned exit_count;                     // # in list


void CppExitInit(               // INITIALIZE CPP EXIT
    void )
{
    exit_count = 0;
    exiting = false;
}


void CppExitFini(               // COMPLETE CPP EXITING
    void )
{
    if( ! exiting ) {
        exiting = true;
        for( ; exit_count > 0; ) {
            -- exit_count;
            (*exit_list[exit_count])();
        }
    }
}


void CppExit(                   // C++ EXIT ROUTINE
    int status )                // - status
{
    longjmp( *(jmp_buf*)CompInfo.exit_jmpbuf, status );
}


void CppAtExit(                 // C++ AT-EXIT ROUTINE
    void (*fun)( void ) )       // - exit function
{
    if( ! exiting ) {
        DbgVerify( exit_count < MAX_EXITS, "CppAtExit: too many exits" );
        exit_list[ exit_count++ ] = fun;
    }
}

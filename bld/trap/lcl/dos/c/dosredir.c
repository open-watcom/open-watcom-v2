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


#include <string.h>
#include <i86.h>
#include "tinyio.h"
#include "trpimp.h"
#include "doshdl.h"

extern void            SetUsrTask(void);
extern void            SetDbgTask(void);

static tiny_handle_t   SaveStdIn;
static tiny_handle_t   SaveStdOut;

static unsigned Redirect( bool input )
{
    tiny_ret_t      bigint;
    tiny_handle_t   std_hndl;
    tiny_handle_t   *var;
    char            *name;
    redirect_stdin_ret  *ret;

    ret = GetOutPtr( 0 );
    name = GetInPtr( sizeof( redirect_stdin_req ) );
    ret->err = 0;
    if( input ) {
        std_hndl = TINY_IN;
        var = &SaveStdIn;
    } else {
        std_hndl = TINY_OUT;
        var = &SaveStdOut;
    }
    if( *name == '\0' ) {
        if( *var != NIL_DOS_HANDLE ) {
            SetUsrTask();
            bigint = TinyDup2( *var, std_hndl );
            if( TINY_ERROR( bigint ) ) {
                ret->err = 1;
            } else {
                TinyClose( *var );
                *var = NIL_DOS_HANDLE;
            }
        }
    } else {
        SetUsrTask();
        if( *var == NIL_DOS_HANDLE ) *var = TinyDup( std_hndl );
        if( input ) {
            bigint = TinyOpen( name, TIO_READ );
        } else {
            bigint = TinyCreate( name, TIO_NORMAL );
        }
        if( TINY_ERROR( bigint ) ) {
            ret->err = 1;
        } else {
            TinyDup2( (tiny_handle_t) bigint, std_hndl );
            TinyClose( (tiny_handle_t) bigint );
        }
    }
    SetDbgTask();
    return( sizeof( *ret ) );
}

unsigned ReqRedirect_stdin()
{
    return( Redirect( TRUE ) );
}

unsigned ReqRedirect_stdout()
{
    return( Redirect( FALSE ) );
}

InitRedirect()
{
    SaveStdIn = NIL_DOS_HANDLE;
    SaveStdOut = NIL_DOS_HANDLE;
}

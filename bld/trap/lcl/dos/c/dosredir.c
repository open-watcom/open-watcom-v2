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
#include "trpimp.h"
#if defined( ACAD ) || defined( PHARLAP )
#include <fcntl.h>
#include <io.h>
#else
#include "tinyio.h"
#endif
#include "dosredir.h"
#include "doscomm.h"

#if defined( ACAD ) || defined( PHARLAP )
#define REDIR_dup       dup
#define REDIR_open(x)   open( x, O_BINARY | O_RDWR, 0 )
#define REDIR_create(x) open( x, O_BINARY | O_RDWR | O_TRUNC | O_CREAT, 0 )
#define REDIR_dup2      dup2
#define REDIR_close     close
#define HANDLE_INP      0
#define HANDLE_OUT      1
#define t_rc            int
#define t_handle        int
#define IsError(x)      (x == -1)
#define Handle(x)       (x)
#else
#define REDIR_dup       TinyDup
#define REDIR_open(x)   TinyOpen( x, TIO_READ )
#define REDIR_create(x) TinyCreate( x, TIO_NORMAL )
#define REDIR_dup2      TinyDup2
#define REDIR_close     TinyClose
#define HANDLE_INP      TINY_IN
#define HANDLE_OUT      TINY_OUT
#define t_rc            tiny_ret_t
#define t_handle        tiny_handle_t
#define IsError(x)      TINY_ERROR(x)
#define Handle(x)       TINY_INFO(x)
#endif

#define NIL_DOS_HANDLE  (t_handle)-1

static t_handle SaveStdIn;
static t_handle SaveStdOut;

static trap_retval Redirect( bool input )
{
    t_handle            std_hndl;
    t_handle            *var;
    char                *name;
    redirect_stdin_ret  *ret;
    t_rc                rc;

    ret = GetOutPtr( 0 );
#if defined( PHARLAP )
    if( !SetUsrTask() ) {
        ret->err = 1;
        return( sizeof( *ret ) );
    }
#else
    SetUsrTask();
#endif
    name = GetInPtr( sizeof( redirect_stdin_req ) );
    ret->err = 0;
    if( input ) {
        std_hndl = HANDLE_INP;
        var = &SaveStdIn;
    } else {
        std_hndl = HANDLE_OUT;
        var = &SaveStdOut;
    }
    if( *name == '\0' ) {
        if( *var != NIL_DOS_HANDLE ) {
            rc = REDIR_dup2( *var, std_hndl );
            if( IsError( rc ) ) {
                ret->err = 1;  // error!
            } else {
                REDIR_close( *var );
                *var = NIL_DOS_HANDLE;
            }
        }
    } else {
        if( *var == NIL_DOS_HANDLE ) {
            *var = REDIR_dup( std_hndl );
        }
        if( input ) {
            rc = REDIR_open( name );
        } else {
            rc = REDIR_create( name );
        }
        if( IsError( rc ) ) {
            ret->err = 1;  // error!
        } else {
            REDIR_dup2( Handle( rc ), std_hndl );
            REDIR_close( Handle( rc ) );
        }
    }
    SetDbgTask();
    return( sizeof( *ret ) );
}

trap_retval ReqRedirect_stdin( void )
{
    return( Redirect( TRUE ) );
}

trap_retval ReqRedirect_stdout( void )
{
    return( Redirect( FALSE ) );
}

void RedirectInit( void )
{
    SaveStdIn = NIL_DOS_HANDLE;
    SaveStdOut = NIL_DOS_HANDLE;
}

void RedirectFini( void )
{
    SaveStdIn = NIL_DOS_HANDLE;
    SaveStdOut = NIL_DOS_HANDLE;
}

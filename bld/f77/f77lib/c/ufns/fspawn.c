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


//
// FSPAWN       : execute a program
//

#include "ftnstd.h"

#include <malloc.h>
#include <stddef.h>
#include <string.h>
#include <process.h>
#include <pgmacc.h>


intstar4        fortran FSPAWN( string PGM *cmd, string PGM *args ) {
//===================================================================

    int         len;
    int         arg_len;
    char        *local_cmd;
    char        *local_args;

    len = cmd->len;
    for(;;) {
        if( len == 0 ) return( -1 );
        if( cmd->strptr[ len - 1 ] != ' ' ) break;
        --len;
    }
    local_cmd = alloca( len + 1 );
    pgm_memget( local_cmd, cmd->strptr, len );
    local_cmd[len] = NULLCHAR;

    arg_len = args->len;
    for(;;) {
        if( arg_len == 0 ) break;
        if( args->strptr[ arg_len - 1 ] != ' ' ) break;
        --arg_len;
    }
    local_args = alloca( arg_len + 1 );
    pgm_memget( local_args, args->strptr, arg_len );
    local_args[arg_len] = NULLCHAR;
    return( spawnlp( P_WAIT, local_cmd, local_cmd, local_args, NULL ) );
}

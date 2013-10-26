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


#include "variety.h"
#include <stddef.h>
#include <signal.h>
#include <dos.h>
#define INCL_DOSSIGNALS
#include <wos2.h>
#include "rtdata.h"
#include "sigfunc.h"
#include "_int23.h"

static PFNSIGHANDLER handler = 0;
static USHORT        action;

static void _WCFAR __pascal break_handler( USHORT sigarg, USHORT signum )
{
    if( __int23_exit != __null_int23_exit ) {
        raise( SIGINT );
    }
}

static void restore_handler( void )
{
    DosSetSigHandler( handler, &handler, &action, action, SIG_CTRLC );
    handler = 0;
    __int23_exit = __null_int23_exit;
}


void __grab_int23( void )
{
    USHORT          action;

    if( handler != 0 )
        return;
    DosSetSigHandler( (PFNSIGHANDLER)break_handler, &handler, &action, 2, SIG_CTRLC );
    __int23_exit = restore_handler;
}

static FPEhandler   *__old_FPE_handler = NULL;

void __restore_FPE_handler( void )
{
    if( __old_FPE_handler == NULL ) {
        return;
    }
    __FPE_handler = __old_FPE_handler;
    __old_FPE_handler = NULL;
}

void __grab_FPE_handler( void )
{
    if( __old_FPE_handler == NULL ) {
        __old_FPE_handler = __FPE_handler;
        __FPE_handler = __sigfpe_handler;
    }
}

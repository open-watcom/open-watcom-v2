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
* Description:  OS/2 16-bit signal handling (based on OS signal handling).
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <signal.h>
#include <dos.h>
#include <float.h>
#define INCL_DOSSIGNALS
#include <wos2.h>
#include "rtdata.h"
#include "sigtab.h"
#include "sigfunc.h"
#include "_int23.h"
#include "errorno.h"
#include "thread.h"


static struct sigtab _SignalTable[] = {
    { SIG_IGN, NULL, 0, 0 },                /* unused  */
    { SIG_DFL, NULL, 0, 0 },                /* SIGABRT */
    { SIG_DFL, NULL, 0, 0 },                /* SIGFPE  */
    { SIG_DFL, NULL, 0, 0 },                /* SIGILL  */
    { SIG_DFL, NULL, 0, SIG_CTRLC },        /* SIGINT  */
    { SIG_DFL, NULL, 0, 0 },                /* SIGSEGV */
    { SIG_DFL, NULL, 0, SIG_KILLPROCESS },  /* SIGTERM */
    { SIG_DFL, NULL, 0, SIG_CTRLBREAK },    /* SIGBREAK */
    { SIG_IGN, NULL, 0, SIG_PFLG_A },       /* SIGUSR1 */
    { SIG_IGN, NULL, 0, SIG_PFLG_B },       /* SIGUSR2 */
    { SIG_IGN, NULL, 0, SIG_PFLG_C },       /* SIGUSR3 */
    { SIG_DFL, NULL, 0, 0 },                /* SIGIDIVZ */
    { SIG_DFL, NULL, 0, 0 }                 /* SIGIOVFL */
};

void __sigabort( void )
{
    raise( SIGABRT );
}


_WCRTLINK void _WCI86FAR __sigfpe_handler( int fpe_type )
{
    __sig_func  func;

    func = _RWD_sigtab[ SIGFPE ].func;
    if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
        _RWD_sigtab[ SIGFPE ].func = SIG_DFL;      /* 09-nov-87 FWC */
        (*(__sigfpe_func)func)( SIGFPE, fpe_type );        /* so we can pass 2'nd parm */
    }
}


static void _WCFAR __pascal break_handler( USHORT sigarg, USHORT signum )
{
    int sig;

    sigarg = sigarg;
    for( sig = 1; sig <= __SIGLAST; sig++ ) {
        if( _RWD_sigtab[ sig ].os_sig_code == signum ) {
            raise( sig );
            break;
        }
    }
}


static void restore_handler( void )
{
    int sig;

    for( sig = 1; sig <= __SIGLAST; sig++ ) {
        if( _RWD_sigtab[ sig ].os_func != NULL ) {
                DosSetSigHandler( _RWD_sigtab[ sig ].os_func,
                                  &_RWD_sigtab[ sig ].os_func,
                                  &_RWD_sigtab[ sig ].prev_action,
                                  _RWD_sigtab[ sig ].prev_action,
                                  _RWD_sigtab[ sig ].os_sig_code );
        }
    }
    __int23_exit = __null_int23_exit;
}


_WCRTLINK __sig_func signal( int sig, __sig_func func )
{
    __sig_func  prev_func;

    if(( sig < 1 ) || ( sig > __SIGLAST )) {
        _RWD_errno = EINVAL;
        return( SIG_ERR );
    }
    _RWD_abort = __sigabort;           /* change the abort rtn address */
    if( _RWD_sigtab[ sig ].os_sig_code != 0 ) {
        if( func != SIG_DFL  &&  func != SIG_ERR ) {
            if( _RWD_sigtab[ sig ].os_func == NULL ) {
                DosSetSigHandler( (PFNSIGHANDLER)break_handler,
                    &_RWD_sigtab[ sig ].os_func,
                    &_RWD_sigtab[ sig ].prev_action,
                    2,
                    _RWD_sigtab[ sig ].os_sig_code );
                __int23_exit = restore_handler;
            }
        }
    } else if( sig == SIGFPE ) {
        if( func == SIG_DFL ) {
            __restore_FPE_handler();
        } else if( func != SIG_ERR ) {
            __grab_FPE_handler();
        }
    }
    prev_func = _RWD_sigtab[ sig ].func;
    _RWD_sigtab[ sig ].func = func;
    return( prev_func );
}

_WCRTLINK int raise( int sig )
{
    __sig_func  func;

    func = _RWD_sigtab[ sig ].func;
    switch( sig ) {
    case SIGFPE:
        __sigfpe_handler( FPE_EXPLICITGEN );
        break;
    case SIGABRT:
        if( func == SIG_DFL ) {
            __terminate();
        }
    case SIGILL:
    case SIGINT:
    case SIGSEGV:
    case SIGTERM:
    case SIGBREAK:
    case SIGUSR1:
    case SIGUSR2:
    case SIGUSR3:
    case SIGIDIVZ:
    case SIGIOVFL:
        if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
            _RWD_sigtab[ sig ].func = SIG_DFL;      /* 09-nov-87 FWC */
            (*func)( sig );
        }
        break;
    default:
        return( -1 );
    }
    return( 0 );
}

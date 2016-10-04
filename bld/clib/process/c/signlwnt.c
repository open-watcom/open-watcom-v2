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
* Description:  Win32 signal handling (based on OS exception handling).
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include <float.h>
#include <windows.h>
#include <excpt.h>
#include "rtdata.h"
#include "rtfpehdl.h"
#include "rterrno.h"
#include "sigfunc.h"
#include "signlwnt.h"
#include "rtinit.h"
#include "thread.h"
#include "sigtab.h"
#include "initsig.h"
#include "exitwmsg.h"


static sigtab  _SignalTable[] = {
    { SIG_IGN, -1 },                                /* unused  */
    { SIG_DFL, -1 },                                /* SIGABRT */
    { SIG_DFL, -1 },                                /* SIGFPE  */
    { SIG_DFL, STATUS_ILLEGAL_INSTRUCTION },        /* SIGILL  */
    { SIG_DFL, STATUS_CONTROL_C_EXIT },             /* SIGINT  */
    { SIG_DFL, STATUS_ACCESS_VIOLATION },           /* SIGSEGV */
    { SIG_DFL, STATUS_CONTROL_C_EXIT },             /* SIGTERM */
    { SIG_DFL, STATUS_CONTROL_C_EXIT },             /* SIGBREAK */
    { SIG_IGN, -1 },                                /* SIGUSR1 */
    { SIG_IGN, -1 },                                /* SIGUSR2 */
    { SIG_IGN, -1 },                                /* SIGUSR3 */
    { SIG_DFL, STATUS_INTEGER_DIVIDE_BY_ZERO },     /* SIGIDIVZ */
    { SIG_DFL, STATUS_INTEGER_OVERFLOW }            /* SIGIOVFL */
};

static char CtrlHandlerRunning = FALSE;


static __sig_func __SetSignalFunc( int sig, __sig_func new_func )
{
    __sig_func  prev_func = NULL;

    if(( sig == SIGBREAK ) || ( sig == SIGINT )) {
        prev_func = _SignalTable[sig].func;
        _SignalTable[sig].func = new_func;
    } else {
        prev_func = _RWD_sigtab[sig].func;
        _RWD_sigtab[sig].func = new_func;
    }
    return( prev_func );
}


static __sig_func __GetSignalFunc( int sig )
{
    if(( sig == SIGBREAK ) || ( sig == SIGINT ))
        return( _SignalTable[sig].func );

    return( _RWD_sigtab[sig].func );
}


static long __GetSignalOSCode( int sig )
{
    if(( sig == SIGBREAK ) || ( sig == SIGINT ))
        return( _SignalTable[sig].os_sig_code );

    return( _RWD_sigtab[sig].os_sig_code );
}


static __sig_func __CheckSignalExCode( int sig, long code )
{
    if( code == __GetSignalOSCode( sig ) )
        return( __GetSignalFunc( sig ) );

    return( NULL );
}


static BOOL WINAPI CtrlSignalHandler( IN ULONG Event )
{
    __sig_func  func;

    switch( Event ) {
    case CTRL_C_EVENT:
        func = __GetSignalFunc( SIGINT );
        if( !func )
            return( FALSE );
        raise( SIGINT );
        break;
    case CTRL_BREAK_EVENT:
        func = __GetSignalFunc( SIGBREAK );
        if( !func )
            return( FALSE );
        raise( SIGBREAK );
        break;
    default:
        return( FALSE );
    }

    if(( func == SIG_DFL ) || ( func == SIG_ERR ))
        return( FALSE );

    return( TRUE );
}


static BOOL CtrlHandlerIsNeeded( void )
{
    __sig_func  int_func = __GetSignalFunc( SIGINT );
    __sig_func  brk_func = __GetSignalFunc( SIGBREAK );

    return((( int_func != SIG_DFL ) && ( int_func != SIG_ERR ))
        || (( brk_func != SIG_DFL ) && ( brk_func != SIG_ERR )));
}


static BOOL StartCtrlHandler( void )
{
    if( !CtrlHandlerRunning && SetConsoleCtrlHandler( CtrlSignalHandler, TRUE ) )
        CtrlHandlerRunning = TRUE;

    return( (BOOL)CtrlHandlerRunning );
}


static BOOL KillCtrlHandler( void )
{
    if( CtrlHandlerRunning && SetConsoleCtrlHandler( CtrlSignalHandler, FALSE ) )
        CtrlHandlerRunning = FALSE;

    return( !(BOOL)CtrlHandlerRunning );
}


static void __sigabort( void )
{
    raise( SIGABRT );
}

_WCRTLINK int __sigfpe_handler( int fpe )
{
    __sig_func  func;

    func = __GetSignalFunc( SIGFPE );
    if(( func != SIG_IGN ) && ( func != SIG_DFL ) && ( func != SIG_ERR )) {
        __SetSignalFunc( SIGFPE, SIG_DFL );
        (*(__sigfpe_func)func)( SIGFPE, fpe );
        return( 0 );
    } else if( func == SIG_IGN ) {
        return( 0 );
    }
    return( -1 );
}


_WCRTLINK __sig_func signal( int sig, __sig_func func )
{
    __sig_func  prev_func;

    if(( sig < 1 ) || ( sig > __SIGLAST )) {
        _RWD_errno = EINVAL;
        return( SIG_ERR );
    }

    _RWD_abort = __sigabort;               /* change the abort rtn address */

    prev_func = __GetSignalFunc( sig );
    __SetSignalFunc( sig, func );
    if( CtrlHandlerIsNeeded() )
        StartCtrlHandler();
    else
        KillCtrlHandler();

    return( prev_func );
}


_WCRTLINK int raise( int sig )
{
    __sig_func  func;

    func = __GetSignalFunc( sig );
    switch( sig ) {
    case SIGFPE:
        __sigfpe_handler( FPE_EXPLICITGEN );
        break;
    case SIGABRT:
        if( func == SIG_DFL ) {
            __terminate();
        }
        /* fall down */
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
        if(( func != SIG_IGN ) && ( func != SIG_DFL ) && ( func != SIG_ERR )) {
            __SetSignalFunc( sig, SIG_DFL );
            (*func)( sig );
        }
        /*
         * If the CtrlSignalHandler was needed before we processed the
         * signal but is not needed NOW then we need to remove it since it
         * has just now become unnecessary.
         *
         * NOTE: This MAY be a bad thing to do since raise() might have been
         *       called from within CtrlSignalHandler() in which case we are
         *       removing the handler from within the handler.  Does NT care?
         */
        if( !CtrlHandlerIsNeeded() )
            KillCtrlHandler();
        break;
    default:
        return( -1 );
    }
    return( 0 );
}


static void __SigInit( void )
{
#ifdef __SW_BM
    int         i;

    for( i = 1; i <= __SIGLAST; ++i ) {
        _RWD_sigtab[i] = _SignalTable[i];
    }
#endif

    __oscode_check_func = __CheckSignalExCode;
    __raise_func        = raise;
}


static void __SigFini( void )
{
    /*
     * If there are still SIGINT or SIGBREAK handlers in the sig table,
     * then the CtrlSignalHandler is still loaded and we want to get
     * rid of it.
     */
    if( CtrlHandlerIsNeeded() ) {
        KillCtrlHandler();
        __SetSignalFunc( SIGINT, SIG_DFL );
        __SetSignalFunc( SIGBREAK, SIG_DFL );
    }
}



static void __sig_init( void )
{
    __sig_init_rtn = __SigInit;
    __sig_fini_rtn = __SigFini;
    _RWD_FPE_handler = (FPEhandler *)__sigfpe_handler;
}


AXI( __sig_init, INIT_PRIORITY_LIBRARY )

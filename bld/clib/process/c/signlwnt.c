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
#include <stdio.h>
#include <unistd.h>
#include <dos.h>
#include <errno.h>
#include <float.h>
#include <string.h>
#include <windows.h>
#include <excpt.h>
#include "ntex.h"
#include "rtdata.h"
#include "errorno.h"
#include "sigtab.h"
#include "extfunc.h"
#include "fpusig.h"
#include "seterrno.h"
#include "signlwnt.h"
#include "rtinit.h"

_WCRTLINK extern void (*__sig_init_rtn)(void);
_WCRTLINK extern void (*__sig_fini_rtn)(void);

typedef void sig_func();

//#define       XCPT_FPE        -1      /* trap all floating-point exceptions */
sigtab  SignalTable[] = {
        { SIG_IGN, -1 },                                /* unused  */
        { SIG_DFL, -1 },                                /* SIGABRT */
        { SIG_IGN, -1 },                                /* SIGFPE  */
        { SIG_DFL, STATUS_ILLEGAL_INSTRUCTION },        /* SIGILL  */
        { SIG_DFL, STATUS_CONTROL_C_EXIT },             /* SIGINT  */
        { SIG_DFL, STATUS_ACCESS_VIOLATION },           /* SIGSEGV */
        { SIG_DFL, STATUS_CONTROL_C_EXIT },             /* SIGTERM */
        { SIG_DFL, STATUS_CONTROL_C_EXIT },             /* SIGBREAK */
        { SIG_IGN, -1 },                                /* SIGUSR1 */
        { SIG_IGN, -1 },                                /* SIGUSR2 */
        { SIG_IGN, -1 },                                /* SIGUSR3 */
        { SIG_DFL, STATUS_INTEGER_DIVIDE_BY_ZERO },     /* SIGIDIVZ */
        { SIG_IGN, STATUS_INTEGER_OVERFLOW }            /* SIGIOVFL */
};

static char CtrlHandlerRunning = FALSE;


void *__SetSignalFunc(int sig, void (*new_func)(int))
{
    void (*prev_func)(int) = NULL;

    if ((sig == SIGBREAK) || (sig == SIGINT))
    {
        prev_func = SignalTable[sig].func;
        SignalTable[sig].func = new_func;
    }
    else
    {
        prev_func = _RWD_sigtab[sig].func;
        _RWD_sigtab[sig].func = new_func;
    }

    return prev_func;
} /* __SetSignalFunc() */


void *__GetSignalFunc(int sig)
{
    if ((sig == SIGBREAK) || (sig == SIGINT))
        return SignalTable[sig].func;

    return _RWD_sigtab[sig].func;
} /* __GetSignalFunc() */


long __GetSignalOSCode(int sig)
{
    if ((sig == SIGBREAK) || (sig == SIGINT))
        return SignalTable[sig].os_sig_code;

    return _RWD_sigtab[sig].os_sig_code;
} /* __GetSignalOSCode() */


void *__CheckSignalExCode(int sig, long code)
{
    if (code == __GetSignalOSCode(sig))
        return __GetSignalFunc(sig);

    return NULL;
} /* __CheckSignalExCode() */


static BOOL WINAPI CtrlSignalHandler(IN ULONG Event)
{
    sig_func *func;

    switch (Event)
    {
        case CTRL_C_EVENT:
            func = __GetSignalFunc(SIGINT);
            if (!func)
                return FALSE;
            raise(SIGINT);
            break;

        case CTRL_BREAK_EVENT:
            func = __GetSignalFunc(SIGBREAK);
            if (!func)
                return FALSE;
            raise(SIGBREAK);
            break;

        default:
            return FALSE;
    }

    if ((func == SIG_DFL) || (func == SIG_ERR))
        return FALSE;

    return TRUE;
} /* CtrlSignalHandler() */


static BOOL CtrlHandlerIsNeeded(void)
{
    void (*int_func)(void) = __GetSignalFunc(SIGINT);
    void (*brk_func)(void) = __GetSignalFunc(SIGBREAK);

    return (((int_func != SIG_DFL) && (int_func != SIG_ERR))
                    || ((brk_func != SIG_DFL) && (brk_func != SIG_ERR)));
} /* CtrlHandlerIsNeeded() */


static BOOL StartCtrlHandler(void)
{
    if (!CtrlHandlerRunning && SetConsoleCtrlHandler(CtrlSignalHandler, TRUE))
        CtrlHandlerRunning = TRUE;

    return (BOOL)CtrlHandlerRunning;
} /* StartCtrlHandler() */


static BOOL KillCtrlHandler(void)
{
    if (CtrlHandlerRunning && SetConsoleCtrlHandler(CtrlSignalHandler, FALSE))
        CtrlHandlerRunning = FALSE;

    return !(BOOL)CtrlHandlerRunning;
} /* KillCtrlHandler() */


void __sigabort( void )
{
    raise( SIGABRT );
}

_WCRTLINK int __sigfpe_handler( int fpe )
{
    sig_func *func;

    func = __GetSignalFunc(SIGFPE);
    if( (func != SIG_IGN) && (func != SIG_DFL) && (func != SIG_ERR) ) {
        __SetSignalFunc(SIGFPE, SIG_DFL);
        (*func)( SIGFPE, fpe );
        return( 0 );
    }
    return( -1 );
}


_WCRTLINK void (*signal( int sig, void (*func)(int) ))( int )
{
    void (*prev_func)(int);

    if( ( sig < 1 ) || ( sig > __SIGLAST ) ) {
        __set_errno( EINVAL );
        return( SIG_ERR );
    }

    __abort = __sigabort;               /* change the abort rtn address */

    if( (func != SIG_DFL) && (func != SIG_ERR) ) {
        if( __GetSignalOSCode(sig) != 0 ) {
            if( sig == SIGFPE ) {
                #if defined(__AXP__) || defined(__PPC__)
                    // __FIXME__
                #else
                    /* enable all interrupts, except precision exception */
                    /* - precision exceptions are very common */
                    _control87( 0, ( MCW_EM & ~EM_PRECISION ) | 0x80 );
                #endif
            }
        }
    } else {
    }


    prev_func = __GetSignalFunc(sig);
    __SetSignalFunc(sig, func);

    if (CtrlHandlerIsNeeded())
        StartCtrlHandler();
    else
        KillCtrlHandler();

    return( prev_func );
}


_WCRTLINK int raise( int sig )
{
    sig_func *func;

    func = __GetSignalFunc(sig);
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
        if( (func != SIG_IGN) && (func != SIG_DFL) && (func != SIG_ERR) ) {
            __SetSignalFunc(sig, SIG_DFL);
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
        if (!CtrlHandlerIsNeeded())
                KillCtrlHandler();
        break;
    default:
        return( -1 );
    }
    return( 0 );
} /* raise() */


void __SigInit( void )
{
#if defined( __SW_BM )
    int         i;

    for( i = 1; i <= __SIGLAST; ++i ) {
        _RWD_sigtab[ i ] = SignalTable[ i ];
    }
#endif

    __oscode_check_func = __CheckSignalExCode;
    __raise_func        = raise;
} /* __SigInit() */


void __SigFini( void )
{
    /*
     * If there are still SIGINT or SIGBREAK handlers in the sig table,
     * then the CtrlSignalHandler is still loaded and we want to get
     * rid of it.
     */
    if (CtrlHandlerIsNeeded())
    {
        KillCtrlHandler();
        __SetSignalFunc(SIGINT, SIG_DFL);
        __SetSignalFunc(SIGBREAK, SIG_DFL);
    }
} /* __SigFini() */



void __sig_init(void)
{
    __sig_init_rtn = __SigInit;
    __sig_fini_rtn = __SigFini;
} /* __sig_init() */


AXI(__sig_init, INIT_PRIORITY_LIBRARY)

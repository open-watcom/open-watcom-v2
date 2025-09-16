/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 32-bit signal handling (based on OS exception handling).
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <stdio.h>
#include <signal.h>
#include <dos.h>
#include <float.h>
#include <wos2.h>
#include "rtdata.h"
#include "rtfpehdl.h"
#include "rtfpesig.h"
#include "seterrno.h"
#include "sigfunc.h"
#include "fpusig.h"
#include "rtinit.h"
#include "_int23.h"
#include "thread.h"
#include "sigtab.h"
#include "initsig.h"
#include "exitwmsg.h"
#include "rtexcpt.h"
#include "rtexcpfl.h"


#ifndef __MT__
__EXCEPTION_RECORD  *__XCPTHANDLER;
#endif

unsigned        char    __ExceptionHandled;

#define XCPT_FPE        -1      /* trap all floating-point exceptions */

static sigtab  _SignalTable[] = {
    { SIG_IGN, 0 },                                 /* unused  */
    { SIG_DFL, 0 },                                 /* SIGABRT */
    { SIG_DFL, XCPT_FPE },                          /* SIGFPE  */
    { SIG_DFL, XCPT_ILLEGAL_INSTRUCTION },          /* SIGILL  */
    { SIG_DFL, XCPT_SIGNAL },                       /* SIGINT  */
    { SIG_DFL, XCPT_ACCESS_VIOLATION },             /* SIGSEGV */
    { SIG_DFL, XCPT_SIGNAL },                       /* SIGTERM */
    { SIG_DFL, XCPT_SIGNAL },                       /* SIGBREAK */
    { SIG_IGN, 0 },                                 /* SIGUSR1 */
    { SIG_IGN, 0 },                                 /* SIGUSR2 */
    { SIG_IGN, 0 },                                 /* SIGUSR3 */
    { SIG_DFL, XCPT_INTEGER_DIVIDE_BY_ZERO },       /* SIGIDIVZ */
    { SIG_DFL, XCPT_INTEGER_OVERFLOW }              /* SIGIOVFL */
};


_WCRTLINK int   __sigfpe_handler( int fpe_type )
/**********************************************/
{
    __sig_func  func;

    func = _RWD_sigtab[SIGFPE].func;
    if(( func != SIG_IGN ) && ( func != SIG_DFL ) && ( func != SIG_ERR )) {
        _RWD_sigtab[SIGFPE].func = SIG_DFL;
        SIGFPE_CALL( func, fpe_type );    /* so we can pass 2'nd parm */
        return( 0 );
    } else if( func == SIG_IGN ) {
        return( 0 );
    }
    return( -1 );
}


static  ULONG   __syscall xcpt_handler( PEXCEPTIONREPORTRECORD pxcpt,
                                  PEXCEPTIONREGISTRATIONRECORD registration,
                                                PCONTEXTRECORD context,
                                                         PVOID unknown ) {
/************************************************************************/

    int                 sig;
    int                 fpe_type;
    unsigned char       *ip;
    unsigned short      fp_tw;
    status_word         fp_sw;

    registration = registration;
    unknown = unknown;

    if(( pxcpt->ExceptionNum >= XCPT_FLOAT_DENORMAL_OPERAND ) &&
       ( pxcpt->ExceptionNum <= XCPT_FLOAT_UNDERFLOW )) {
        switch( pxcpt->ExceptionNum ) {
        case XCPT_FLOAT_DENORMAL_OPERAND :
            fpe_type = FPE_DENORMAL;
            break;
        case XCPT_FLOAT_DIVIDE_BY_ZERO :
            fpe_type = FPE_ZERODIVIDE;
            break;
        case XCPT_FLOAT_INEXACT_RESULT :
            fpe_type = FPE_INEXACT;
            break;
        case XCPT_FLOAT_INVALID_OPERATION :
            fpe_type = FPE_INVALID;
            ip = (unsigned char *)context->ctx_env[3];
            if( *(unsigned short *)ip == 0xfad9 ) {
                // exception caused by "fsqrt" instruction
                fpe_type = FPE_SQRTNEG;
            } else if( *(unsigned short *)ip == 0xf1d9 ) {
                // exception caused by "fyl2x" instruction
                fpe_type = FPE_LOGERR;
            } else if( *(unsigned short *)ip == 0xf8d9 ) {
                // exception caused by "fprem" instruction
                fpe_type = FPE_MODERR;
            } else {
                if( ( ip[0] == 0xdb ) || ( ip[0] == 0xdf ) ) {
                    if( ( ip[1] & 0x30 ) == 0x10 ) {
                        // exception caused by "fist(p)" instruction
                        fpe_type = FPE_IOVERFLOW;
                    }
                }
                if( !(ip[0] & 0x01) ) {
                    if( (ip[1] & 0x30) == 0x30 ) {
                        // it's a "fdiv" or "fidiv" instruction
                        fp_tw = context->ctx_env[2] & 0x0000ffff;
                        fp_sw.sw = context->ctx_env[1] & 0x0000ffff;
                        if( ((fp_tw >> (fp_sw.b.st << 1)) & 0x01) == 0x01 ) {
                            fpe_type = FPE_ZERODIVIDE;
                        }
                    }
                }
            }
            break;
        case XCPT_FLOAT_OVERFLOW :
            fpe_type = FPE_OVERFLOW;
            break;
        case XCPT_FLOAT_STACK_CHECK :
            if( context->ctx_env[1] & SW_C1 ) {
                fpe_type = FPE_STACKOVERFLOW;
            } else {
                fpe_type = FPE_STACKUNDERFLOW;
            }
            break;
        case XCPT_FLOAT_UNDERFLOW :
            fpe_type = FPE_UNDERFLOW;
            break;
        }
        _fpreset();
        __ExceptionHandled = 1;
        if(( __sigfpe_handler( fpe_type ) == 0 ) && ( __ExceptionHandled )) {
            context->ctx_env[1] &= ~( SW_BUSY | SW_XCPT_FLAGS | SW_IREQ );
            return( XCPT_CONTINUE_EXECUTION );
        }
    } else {
        if( pxcpt->ExceptionNum == XCPT_SIGNAL ) {
            DosAcknowledgeSignalException( pxcpt->ExceptionInfo[0] );
        }
        for( sig = 1; sig <= __SIGLAST; sig++ ) {
            if( pxcpt->ExceptionNum == _RWD_sigtab[sig].os_sig_code ) {
                if( sig == SIGINT &&
                    pxcpt->ExceptionInfo[0] != XCPT_SIGNAL_INTR ) {
                    continue;
                }
                if( sig == SIGBREAK &&
                    pxcpt->ExceptionInfo[0] != XCPT_SIGNAL_BREAK ) {
                    continue;
                }
                if( sig == SIGTERM &&
                    pxcpt->ExceptionInfo[0] != XCPT_SIGNAL_KILLPROC ) {
                    continue;
                }
                if( (_RWD_sigtab[sig].func == SIG_IGN) ) {
                    return( XCPT_CONTINUE_EXECUTION );
                }
                if( (_RWD_sigtab[sig].func == SIG_DFL) ||
                    (_RWD_sigtab[sig].func == SIG_ERR) ) {
                    return( XCPT_CONTINUE_SEARCH );
                }
                __ExceptionHandled = 1;
                raise( sig );
                if( __ExceptionHandled ) {
                    return( XCPT_CONTINUE_EXECUTION );
                } else {
                    return( XCPT_CONTINUE_SEARCH );
                }
            }
        }
    }
    return( XCPT_CONTINUE_SEARCH );
}


static void __SigInit( void )
/***************************/
{

#ifdef __MT__
    int         i;

    for( i = 1; i <= __SIGLAST; ++i ) {
        _RWD_sigtab[i] = _SignalTable[i];
    }
#endif
    __XCPTHANDLER->prev_structure = NULL;
    __XCPTHANDLER->ExceptionHandler = &xcpt_handler;
}


static void __SigFini( void )
/***************************/
{
#ifdef __MT__
    ULONG               nesting;
    APIRET              rc;
    __EXCEPTION_RECORD  *rr;

    rr = __XCPTHANDLER;
    if( rr && rr->prev_structure ) {
        do {
            rc = DosSetSignalExceptionFocus( SIG_UNSETFOCUS, &nesting );
        } while( rc == NO_ERROR && nesting > 0 );
        DosUnsetExceptionHandler( rr );
    }
#endif
}


static void __sigabort( void )
/****************************/
{
    raise( SIGABRT );
}


static void _WCNEAR restore_handler( void )
/*****************************************/
{

    __SigFini();
    __int23_exit = __null_int23_exit;
}


_WCRTLINK __sig_func signal( int sig, __sig_func func ) {
/***************************************************************/

    __sig_func  prev_func;
    ULONG       nesting;

    if(( sig < 1 ) || ( sig > __SIGLAST )) {
        _RWD_errno = EINVAL;
        return( SIG_ERR );
    }
    _RWD_abort = __sigabort;            /* change the abort rtn address */
    if(( func != SIG_DFL ) && ( func != SIG_ERR )) {
        if( _RWD_sigtab[sig].os_sig_code != 0 ) {
            if( __XCPTHANDLER->prev_structure == NULL ) {
                DosSetExceptionHandler( __XCPTHANDLER );
                __int23_exit = restore_handler;
            }
            if( _RWD_sigtab[sig].os_sig_code == XCPT_SIGNAL ) {
                DosSetSignalExceptionFocus( SIG_SETFOCUS, &nesting );
            }
        }
    } else {
        if( _RWD_sigtab[sig].os_sig_code == XCPT_SIGNAL ) {
            APIRET rc;
            do {
                rc = DosSetSignalExceptionFocus( SIG_UNSETFOCUS, &nesting );
            } while( rc == NO_ERROR && nesting > 0 );
        }
    }
    prev_func = _RWD_sigtab[sig].func;
    _RWD_sigtab[sig].func = func;
    return( prev_func );
}


_WCRTLINK int raise( int sig )
/****************************/
{
    __sig_func  func;

    func = _RWD_sigtab[sig].func;
    switch( sig ) {
    case SIGFPE:
        __sigfpe_handler( FPE_EXPLICITGEN );
        break;
    case SIGABRT:
        if( func == SIG_DFL ) {
            __terminate();
            /* never return */
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
            _RWD_sigtab[sig].func = SIG_DFL;
            if( func ) {
                (*func)( sig );
            }
        }
        break;
    default:
        return( -1 );
    }
    return( 0 );
}


static void __sig_init( void )
{
    __sig_init_rtn = __SigInit;
    __sig_fini_rtn = __SigFini;
    _RWD_FPE_handler = __sigfpe_handler;
}

AXI( __sig_init, INIT_PRIORITY_LIBRARY )

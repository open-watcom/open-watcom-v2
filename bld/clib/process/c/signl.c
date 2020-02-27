/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Signal handling for DOS and Windows 3.x (16 and 32-bit).
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <signal.h>
#include <float.h>
#include "rtdata.h"
#include "rterrno.h"
#include "rtfpehdl.h"
#include "rtfpesig.h"
#include "sigtab.h"
#include "sigfunc.h"
#include "_int23.h"
#include "_ctrlc.h"


#define __SIGLAST       SIGIOVFL

#if defined( __WINDOWS_286__ )

extern void __far _fpmath( void );
#pragma aux _fpmath "__fpmath";

unsigned int win87em_get_sw( void );
#pragma aux win87em_get_sw = \
        "push bx"               \
        "mov  bx,8"             \
        "call far ptr _fpmath"  \
        "pop  bx"               \
    __value [__ax]

#endif

static __sig_func _HUGEDATA _SignalTable[] = {
    SIG_IGN,        /* unused  */
    SIG_DFL,        /* SIGABRT */
    SIG_DFL,        /* SIGFPE  */
    SIG_DFL,        /* SIGILL  */
    SIG_DFL,        /* SIGINT  */
    SIG_DFL,        /* SIGSEGV */
    SIG_DFL,        /* SIGTERM */
    SIG_DFL,        /* SIGBREAK */
    SIG_IGN,        /* SIGUSR1 */
    SIG_IGN,        /* SIGUSR2 */
    SIG_IGN,        /* SIGUSR3 */
    SIG_DFL,        /* SIGIDIVZ */
    SIG_DFL         /* SIGIOVFL */
};

static void __sigabort( void )
{
    raise( SIGABRT );
}

#if defined( __DOS__ )

static FPEhandler   *__old_FPE_handler = NULL;

static void __restore_FPE_handler( void )
{
    if( __old_FPE_handler == NULL ) {
        return;
    }
    _RWD_FPE_handler = __old_FPE_handler;
    __old_FPE_handler = NULL;
}

static void __grab_FPE_handler( void )
{
    if( __old_FPE_handler == NULL ) {
        __old_FPE_handler = _RWD_FPE_handler;
        _RWD_FPE_handler = __sigfpe_handler;
    }
}

#endif


_WCRTLINK void _WCI86FAR __sigfpe_handler( int fpe_type )
{
    __sig_func  func;
#if defined( __WINDOWS_286__ )
    unsigned int  fp_sw;

    fp_sw = win87em_get_sw();
    if( fp_sw & EM_INVALID ) {
        fpe_type = FPE_INVALID;
    } else if( fp_sw & EM_DENORMAL ) {
        fpe_type = FPE_DENORMAL;
    } else if( fp_sw & EM_ZERODIVIDE ) {
        fpe_type = FPE_ZERODIVIDE;
    } else if( fp_sw & EM_OVERFLOW ) {
        fpe_type = FPE_OVERFLOW;
    } else if( fp_sw & EM_UNDERFLOW ) {
        fpe_type = FPE_UNDERFLOW;
    } else if( fp_sw & EM_INEXACT ) {
        fpe_type = FPE_INEXACT;
    }
#endif

    func = _SignalTable[SIGFPE];
    if( func != SIG_IGN && func != SIG_DFL && func != SIG_ERR ) {
        _SignalTable[SIGFPE] = SIG_DFL;
        SIGFPE_CALL( func, fpe_type );    /* so we can pass 2nd parm */
    }
}

_WCRTLINK __sig_func signal( int sig, __sig_func func )
{
    __sig_func  prev_func;

    if(( sig < 1 ) || ( sig > __SIGLAST )) {
        _RWD_errno = EINVAL;
        return( SIG_ERR );
    }
    _RWD_abort = __sigabort;           /* change the abort rtn address */
#if !defined( __WINDOWS_386__ )
    if( sig == SIGINT ) {
        if( func == SIG_DFL ) {
            __restore_int23();
        } else if( func != SIG_ERR ) {
            __grab_int23();
        }
    } else if( sig == SIGBREAK ) {
        if( func == SIG_DFL ) {
            __restore_int_ctrl_break();
        } else if( func != SIG_ERR ) {
            __grab_int_ctrl_break();
        }
  #if defined( __DOS__ )
    } else if( sig == SIGFPE ) {
        if( func == SIG_DFL ) {
            __restore_FPE_handler();
        } else if( func != SIG_ERR ) {
            __grab_FPE_handler();
        }
  #endif
    }
#endif
    prev_func = _RWD_sigtab[sig];
    _RWD_sigtab[sig] = func;
    return( prev_func );
}


_WCRTLINK int raise( int sig )
{
    __sig_func  func;

    func = _RWD_sigtab[sig];
    switch( sig ) {
#if !defined( __WINDOWS_386__ )
    case SIGFPE:
        __sigfpe_handler( FPE_EXPLICITGEN );
        break;
    case SIGABRT:
        if( func == SIG_DFL ) {
            __terminate();
        }
        /* fall down */
    case SIGINT:
        if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
            _RWD_sigtab[sig] = SIG_DFL;
            __restore_int23();
            (*func)( sig );
        }
        break;
    case SIGBREAK:
        if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
            _RWD_sigtab[sig] = SIG_DFL;
            __restore_int_ctrl_break();
            (*func)( sig );
        }
        break;
#else
    case SIGABRT:
    case SIGFPE:
    case SIGINT:
    case SIGBREAK:
#endif
    case SIGILL:
    case SIGSEGV:
    case SIGTERM:
    case SIGUSR1:
    case SIGUSR2:
    case SIGUSR3:
    case SIGIDIVZ:
    case SIGIOVFL:
        if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
            _RWD_sigtab[sig] = SIG_DFL;
            (*func)( sig );
        }
        break;
    default:
        return( -1 );
    }
    return( 0 );
}

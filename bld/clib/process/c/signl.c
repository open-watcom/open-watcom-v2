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
#include <signal.h>
#include <dos.h>
#include <errno.h>
#include <float.h>
#include "rtdata.h"
#include "sigtab.h"
#include "extfunc.h"
#include "seterrno.h"

typedef void sig_func();
#ifdef _M_IX86
    #pragma aux (__outside_CLIB) sig_func;
#endif

#ifndef __WINDOWS_386__
#ifndef __NETWARE__
    extern      void    __grab_int23();
    extern      void    __restore_int23();
    extern      void    __grab_int_ctrl_break();
    extern      void    __restore_int_ctrl_break();
    extern      void    __grab_fpe();
#endif

extern  void    __terminate();
#endif

extern  void    (*__abort)();

#define __SIGLAST       SIGIOVFL

static void (* _HUGEDATA SignalTable[])(int) = {
        SIG_IGN,        /* unused  */
        SIG_DFL,        /* SIGABRT */
        SIG_IGN,        /* SIGFPE  */
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

void __sigabort()
    {
        raise( SIGABRT );
    }

#ifndef __WINDOWS_386__
/* arguments always passed in registers, even if stack calling convention */

#if defined( __386__ )
#pragma aux __sigfpe_handler parm [eax]
#else
#pragma aux __sigfpe_handler parm [ax]
#endif

#if defined(__WINDOWS__)

// called from emulator callback
void __based(__segname("_TEXT")) __raise_fpe(void) {
    raise(SIGFPE);
}

extern __far _fpmath;

unsigned int win87em_get_sw(void);
#pragma aux win87em_get_sw = \
        "push   bx"                                     \
        "mov    bx, 8h"                                 \
        "call   far _fpmath"                            \
        "pop    bx"                                     \
        value [ax]
#endif

_WCRTLINK void _WCI86FAR __sigfpe_handler( int fpe_type )
    {
        sig_func     *func;

        #if defined(__WINDOWS__)
            unsigned int  sw;
            sw = win87em_get_sw();

            if (sw & EM_INVALID) {
                fpe_type = FPE_INVALID;
            } else if (sw & EM_DENORMAL) {
                fpe_type = FPE_DENORMAL;
            } else if (sw & EM_ZERODIVIDE) {
                fpe_type = FPE_ZERODIVIDE;
            } else if (sw & EM_OVERFLOW) {
                fpe_type = FPE_OVERFLOW;
            } else if (sw & EM_UNDERFLOW) {
                fpe_type = FPE_UNDERFLOW;
            } else if (sw & EM_INEXACT) {
                fpe_type = FPE_INEXACT;
            }
        #endif

        func = (sig_func *) SignalTable[ SIGFPE ];
        if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
            SignalTable[ SIGFPE ] = SIG_DFL;      /* 09-nov-87 FWC */
            (*func)( SIGFPE, fpe_type );        /* so we can pass 2'nd parm */
        }
    }
#endif

_WCRTLINK void (*signal( int sig, void (*func)(int) ))(int)
    {
        void (*prev_func)(int);

        if( (sig < 1) || (sig > __SIGLAST) ) {
            __set_errno( EINVAL );
            return( SIG_ERR );
        }
        __abort = __sigabort;           /* change the abort rtn address */
#if !defined( __WINDOWS_386__ ) && !defined( __NETWARE__ )
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
        } else if( sig == SIGFPE ) {
            if( func == SIG_DFL ) // Turn exceptions on
            {
                /*
                 * Mask all exceptions.
                 */
                _control87(~0, 0x002F);
            }
            else if( func != SIG_ERR ) // Turn exceptions off
            {
                /*
                 * Unmask the default exceptions.
                 */
                _control87(0, EM_INVALID | EM_ZERODIVIDE | EM_OVERFLOW
                                | EM_UNDERFLOW);
#ifndef __WINDOWS__
                __grab_fpe();
#endif
            }
        }
#endif
        prev_func = _RWD_sigtab[ sig ];
        _RWD_sigtab[ sig ] = func;
        return( prev_func );
    }


_WCRTLINK int raise(int sig)
    {
        sig_func *func;

        func = _RWD_sigtab[ sig ];
        switch( sig ) {
#if !defined( __WINDOWS_386__ ) && !defined( __NETWARE__ )
        case SIGFPE:
            __sigfpe_handler( FPE_EXPLICITGEN );
            break;
        case SIGABRT:
            if( func == SIG_DFL ) {
                __terminate();
            }
        case SIGINT:
            if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
                _RWD_sigtab[ sig ] = SIG_DFL;      /* 09-nov-87 FWC */
                __restore_int23();
                (*func)(sig);
            }
            break;
        case SIGBREAK:
            if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
                _RWD_sigtab[ sig ] = SIG_DFL;      /* 09-nov-87 FWC */
                __restore_int_ctrl_break();
                (*func)(sig);
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
                _RWD_sigtab[ sig ] = SIG_DFL;      /* 09-nov-87 FWC */
                (*func)(sig);
            }
            break;
        default:
            return( -1 );
        }
        return( 0 );
    }

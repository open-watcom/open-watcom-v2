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
* Description:  Signal handling for DOS and Windows 3.x.
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include <float.h>
#include "rtdata.h"
#include "rterrno.h"
#include "sigtab.h"
#include "sigfunc.h"

#ifndef __WINDOWS_386__
extern      void    __grab_int23( void );
extern      void    __restore_int23( void );
extern      void    __grab_int_ctrl_break( void );
extern      void    __restore_int_ctrl_break( void );
#endif

#define __SIGLAST       SIGIOVFL

/* defined in sigtab.c */
extern __sig_func _HUGEDATA _SignalTable[];

void __sigabort( void )
{
    raise( SIGABRT );
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
    prev_func = _RWD_sigtab[ sig ];
    _RWD_sigtab[ sig ] = func;
    return( prev_func );
}


_WCRTLINK int raise( int sig )
{
    __sig_func  func;
    
    func = _RWD_sigtab[ sig ];
    switch( sig ) {
#if !defined( __WINDOWS_386__ )
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
            (*func)( sig );
        }
        break;
    case SIGBREAK:
        if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
            _RWD_sigtab[ sig ] = SIG_DFL;      /* 09-nov-87 FWC */
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
            _RWD_sigtab[ sig ] = SIG_DFL;      /* 09-nov-87 FWC */
            (*func)( sig );
        }
        break;
    default:
        return( -1 );
    }
    return( 0 );
}

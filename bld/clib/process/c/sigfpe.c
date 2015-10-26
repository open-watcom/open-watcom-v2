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
* Description:  FPE signal handler for DOS and Win16.
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include <float.h>
#include "sigfunc.h"

/* defined in sigtab.c */
extern __sig_func _HUGEDATA _SignalTable[];

#if defined( __WINDOWS__ )

extern void __far _fpmath( void );
#pragma aux _fpmath "__fpmath";

unsigned int win87em_get_sw( void );
#pragma aux win87em_get_sw = \
    "push   bx"                                     \
    "mov    bx, 8h"                                 \
    "call   far ptr _fpmath"                        \
    "pop    bx"                                     \
    value [ax];
#endif

_WCRTLINK void _WCI86FAR __sigfpe_handler( int fpe_type )
{
    __sig_func  func;
    
  #if defined( __WINDOWS__ )
    unsigned int  sw;
    sw = win87em_get_sw();
    
    if( sw & EM_INVALID ) {
        fpe_type = FPE_INVALID;
    } else if( sw & EM_DENORMAL ) {
        fpe_type = FPE_DENORMAL;
    } else if( sw & EM_ZERODIVIDE ) {
        fpe_type = FPE_ZERODIVIDE;
    } else if( sw & EM_OVERFLOW ) {
        fpe_type = FPE_OVERFLOW;
    } else if( sw & EM_UNDERFLOW ) {
        fpe_type = FPE_UNDERFLOW;
    } else if( sw & EM_INEXACT ) {
        fpe_type = FPE_INEXACT;
    }
  #endif
    
    func = _SignalTable[ SIGFPE ];
    if( func != SIG_IGN  &&  func != SIG_DFL  &&  func != SIG_ERR ) {
        _SignalTable[ SIGFPE ] = SIG_DFL;              /* 09-nov-87 FWC */
        (*(__sigfpe_func)func)( SIGFPE, fpe_type );    /* so we can pass 2nd parm */
    }
}

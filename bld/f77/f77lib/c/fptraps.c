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
* Description:  floating-point exception handling
*
****************************************************************************/


#include "ftnstd.h"
#include <signal.h>
#include <float.h>
#include "ftextfun.h"
#include "errcod.h"

#if (defined( __386__ ) && defined( __OS2__ )) || defined( __NT__ )
  extern        byte    __ExceptionHandled;
#endif

#ifndef __NETWARE__

static void     FLIB_FPEHandler( int sig_num, int xcpt ) {
//===========================================================

    sig_num = sig_num;
    if( (xcpt == FPE_STACKOVERFLOW) || (xcpt == FPE_STACKUNDERFLOW) ) {
        RTErr( CP_TERMINATE );
    } else if( xcpt == FPE_OVERFLOW ) {
        RTErr( KO_FOVERFLOW );
    } else if( xcpt == FPE_UNDERFLOW ) {
        RTErr( KO_FUNDERFLOW );
    } else if( xcpt == FPE_ZERODIVIDE ) {
        RTErr( KO_FDIV_ZERO );
    } else if( xcpt == FPE_SQRTNEG ) {
        RTErr( LI_ARG_NEG );
    } else if( xcpt == FPE_LOGERR ) {
        RTErr( LI_ARG_GT_ZERO );
    } else if( xcpt == FPE_MODERR ) {
        RTErr( LI_ARG_GT_ZERO );
    } else if( xcpt == FPE_IOVERFLOW ) {
        RTErr( KO_IOVERFLOW );
#if (defined( __386__ ) && defined( __OS2__ )) || defined( __NT__ )
    } else {
        __ExceptionHandled = 0;
#endif
    }
}


void    __MaskDefaultFPE( void ) {
//==========================
#if defined( _M_IX86 )
    // By default we don't report the following exceptions;
    // the user has to make his own call to _control87.  This has to
    // be done in the "fpc" model as well in case there is an 80x87
    // present.
    _control87( ~0, EM_UNDERFLOW | EM_DENORMAL | EM_PRECISION );
#endif
}

#endif


void    FPTrapInit( void ) {
//====================

#ifndef __NETWARE__     /* FP Exceptions can't be trapped under Netware */
    if( __EnableF77RTExceptionHandling() ) {
        signal( SIGFPE, (__sig_func)FLIB_FPEHandler );
#if defined( _M_IX86 )
    } else {
        // we still want to enable the floating point exceptions (just like
        // signal() does for SIGFPE
        _control87( 0, ( MCW_EM & ~EM_PRECISION ) | 0x80 );
#endif
    }
    __MaskDefaultFPE();
#endif
}


void    FPTrapFini( void ) {
//====================

}

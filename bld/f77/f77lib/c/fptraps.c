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
* Description:  floating-point exception handling
*
****************************************************************************/


#include "ftnstd.h"
#include <signal.h>
#include <float.h>
#include "rterr.h"
#include "errcod.h"
#include "fptraps.h"
#include "rttraps.h"
#include "rtfpesig.h"
#include "rtexcpfl.h"


#if !defined( __NETWARE__ ) && !defined( __UNIX__ )

static __sigfpe_func   FLIB_FPEHandler;

static void     FLIB_FPEHandler( int sig_num, int fpe_type )
//==========================================================
{
    /* unused parameters */ (void)sig_num;

    switch( fpe_type ) {
    case FPE_STACKOVERFLOW:
    case FPE_STACKUNDERFLOW:
        RTErr( CP_TERMINATE );
        break;
    case FPE_OVERFLOW:
        RTErr( KO_FOVERFLOW );
        break;
    case FPE_UNDERFLOW:
        RTErr( KO_FUNDERFLOW );
        break;
    case FPE_ZERODIVIDE:
        RTErr( KO_FDIV_ZERO );
        break;
    case FPE_SQRTNEG:
        RTErr( LI_ARG_NEG );
        break;
    case FPE_LOGERR:
        RTErr( LI_ARG_GT_ZERO );
        break;
    case FPE_MODERR:
        RTErr( LI_ARG_GT_ZERO );
        break;
    case FPE_IOVERFLOW:
        RTErr( KO_IOVERFLOW );
        break;
#if !defined( _M_I86 )
#if defined( __OS2__ ) || defined( __NT__ )
    default:
        __ExceptionHandled = 0;
        break;
#endif
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

#endif /* !defined( __NETWARE__ ) && !defined( __UNIX__ ) */


void    FPTrapInit( void ) {
//====================

#if !defined( __NETWARE__ ) && !defined( __UNIX__ )
    /* FP Exceptions can't be trapped under Netware */
    if( __EnableF77RTExceptionHandling() ) {
        SET_SIGFPE( FLIB_FPEHandler );
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

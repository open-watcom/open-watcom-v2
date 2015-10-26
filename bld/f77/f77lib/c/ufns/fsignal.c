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


//
// FSIGNAL      : allow user to specify exception handlers
//

#include "ftnstd.h"
#include <signal.h>
#include <stddef.h>
#include <float.h>

#if defined( _M_IX86 )
  #pragma aux fsignal "^"
#endif

extern  void                    __MaskDefaultFPE(void);

typedef void (*fsig_func)( intstar4 );

fsig_func   fsignal( intstar4 xcpt, fsig_func handler ) {
//===========================================================

#if defined( __DOS__ )

    // C library signal() doesn't handle SIGBREAK, SIGIOVFL, SIGIDIVZ

    if( xcpt == SIGBREAK ) {
        {
            extern      fsig_func __UserBreakHandler;

            fsig_func        prev_Break;

            prev_Break = __UserBreakHandler;
            __UserBreakHandler = handler;
            return( prev_Break );
        }
    }
#endif

#if defined( __WINDOWS_386__ )

#elif defined( __DOS__ ) || defined( __WINDOWS__ )
    if( xcpt == SIGIOVFL ) {
        {
            extern      fsig_func __UserIOvFlHandler;

            fsig_func   prev_IOvFl;

            prev_IOvFl = __UserIOvFlHandler;
            __UserIOvFlHandler = handler;
            return( prev_IOvFl );
        }
    }
    if( xcpt == SIGIDIVZ ) {
        {
            extern      fsig_func __UserIDivZHandler;

            fsig_func   prev_IDivZ;

            prev_IDivZ = __UserIDivZHandler;
            __UserIDivZHandler = handler;
            return( prev_IDivZ );
        }
    }
#endif

    if( xcpt == SIGFPE ) {
        if( handler == (fsig_func) SIG_IGN ) {
            _control87( ~0, MCW_EM );
        } else if( handler == (fsig_func) SIG_DFL ) {
            __MaskDefaultFPE();
        }
    }
    handler = (fsig_func) signal( xcpt, (__sig_func) handler );
    return( handler );
}

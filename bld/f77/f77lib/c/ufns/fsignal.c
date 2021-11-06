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
#include <float.h>
#include "fptraps.h"
#include "fsignal.h"


fsig_func   fsignal( intstar4 sig_num, fsig_func handler )
//========================================================
{

    // C library signal() doesn't handle SIGBREAK, SIGIOVFL, SIGIDIVZ

    switch( sig_num ) {
#if defined( __DOS__ )
    case SIGBREAK:
        {
            extern      fsig_func __UserBreakHandler;

            fsig_func        prev_Break;

            prev_Break = __UserBreakHandler;
            __UserBreakHandler = handler;
            return( prev_Break );
        }
        break;
#endif
#if defined( __DOS__ ) || defined( __WINDOWS__ ) && defined( _M_I86 )
    case SIGIOVFL:
        {
            extern      fsig_func __UserIOvFlHandler;

            fsig_func   prev_IOvFl;

            prev_IOvFl = __UserIOvFlHandler;
            __UserIOvFlHandler = handler;
            return( prev_IOvFl );
        }
        break;
    case SIGIDIVZ:
        {
            extern      fsig_func __UserIDivZHandler;

            fsig_func   prev_IDivZ;

            prev_IDivZ = __UserIDivZHandler;
            __UserIDivZHandler = handler;
            return( prev_IDivZ );
        }
        break;
#endif
    case SIGFPE:
        if( handler == (fsig_func) SIG_IGN ) {
            _control87( ~0, MCW_EM );
        } else if( handler == (fsig_func) SIG_DFL ) {
            __MaskDefaultFPE();
        }
        break;
    }
    handler = (fsig_func) signal( sig_num, (__sig_func)handler );
    return( handler );
}

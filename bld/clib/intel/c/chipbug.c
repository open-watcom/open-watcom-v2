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
* Description:  verify Pentium processor divide bug
*
****************************************************************************/


#include "variety.h"
#include "rtinit.h"
#include "rtdata.h"
#include "chipbug.h"


enum {
    PROB_P5_DIV = 0x0001
};

_WCRTLINK void __verify_pentium_fdiv_bug( void )
{
    /*
        Verify we have got the Pentium FDIV problem.
        The volatiles are to scare the optimizer away.
    */
    volatile double     num     = 4195835;
    volatile double     denom   = 3145727;

    if( _RWD_real87 >= 3 ) {
        if( (num - (num/denom)*denom) > 1.0e-8 ) {
            __chipbug |= PROB_P5_DIV;
        }
    }
}

AXI( __verify_pentium_fdiv_bug, INIT_PRIORITY_FPU + 4 );

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
* Description:  Implementation of _status87() and _clear87().
*
****************************************************************************/


#include "variety.h"
#include <math.h>
#include <float.h>
#include "rtdata.h"

extern  void        __fclex( void );
extern  unsigned    __fstsw( void );

#if defined(__386__)
#pragma aux __fclex =               \
                ".387"              \
                "fnclex";
#pragma aux __fstsw =               \
                ".387"              \
                "xor eax,eax"       \
                "fnstsw ax"         \
                value [eax];
#else
#pragma aux __fclex =               \
                ".8087"             \
                float "fclex";
/* On the 287, we could use the more sensible fnstsw ax variant. */
/* On the 8087, do it the hard way */
#pragma aux __fstsw =               \
                ".8087"             \
                "push bx"           \
                "mov bx,sp"         \
                float "fstsw [bx]"  \
                float "fnop"        \
                "pop bx"            \
                value [bx];
#endif


_WMRTLINK unsigned _status87( void )
/**********************************/
{
    if( _RWD_8087 ) {
        return( __fstsw() );
    } else {
        return( 0 );
    }
}


_WMRTLINK unsigned _clear87( void )
/*********************************/
{
    unsigned    status;

    status = 0;
    if( _RWD_8087 ) {
        status = __fstsw();
        __fclex();
    }
    return( status );
}

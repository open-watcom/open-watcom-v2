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
* Description:  subscripting routines
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "rtenv.h"
#include "errcod.h"
#include "symdefs.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* Forward declarations */
static  void    SubscriptError( int dims, va_list args, char *name );

intstar4        Subscript( int dims, adv_entry *adv, ... ) {
//==========================================================

// Perform subscript.

    intstar4    offset;
    intstar4    multiplier;
    intstar4    ss;
    va_list     args;
    int         i;

    multiplier = 1;
    offset = 0;
    i = 0;
    va_start( args, adv );
    for(;;) {
        ss = va_arg( args, intstar4 );
        // 0 elements in a dimension implies assumed-size dimension
        if( adv->num_elts != 0 ) {
            if( ( ss < adv->lo_bound ) ||
                ( ss > adv->lo_bound + (signed_32)(adv->num_elts) - 1 ) ) {
                va_start( args, adv );
                SubscriptError( dims, args, *((char **)&adv[ dims - i ]) );
            }
        }
        offset += ( ss - adv->lo_bound ) * multiplier;
        multiplier *= adv->num_elts;
        ++i;
        if( i == dims ) break;
        ++adv;
    }
    va_end( args );
    return( offset );
}


static  void    SubscriptError( int dims, va_list args, char *name ) {
//====================================================================

    char        *ptr;
    char        buff[1+MAX_DIM*(MAX_INT_SIZE+1)+1];
    int         len;

    ptr = buff;
    len = *name;
    ++name;
    memcpy( buff, name, len );
    ptr += len;
    *ptr = '(';
    ++ptr;
    for(;;) {
        ltoa( va_arg( args, intstar4 ), ptr, 10 );
        ptr += strlen( ptr );
        --dims;
        if( dims == 0 ) break;
        *ptr = ',';
        ++ptr;
    }
    *ptr = ')';
    ++ptr;
    *ptr = NULLCHAR;
    RTErr( SS_SSCR_RANGE, buff );
}


void    ADVFillHi( adv_entry *adv, unsigned ss, intstar4 hi ) {
//=============================================================

    intstar4    lo;

    lo = adv[ss-1].lo_bound;
#if defined( _M_I86 )
    if( hi - lo + 1 > 65535 ) {
        RTErr( SV_DIMENSION_LIMIT );
    }
#endif
    if( lo > hi ) {
        RTErr( SV_BAD_SSCR );
    }
    adv[ss-1].num_elts = hi - lo + 1;
}


void    ADVFillHiLo1( adv_entry *adv, unsigned ss, intstar4 hi ) {
//================================================================

#if defined( _M_I86 )
    if( hi > 65535 ) {
        RTErr( SV_DIMENSION_LIMIT );
    }
#endif
    adv[ss-1].num_elts = hi;
}

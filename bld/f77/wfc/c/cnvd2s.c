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
// CNVD2S       : format floating point number to string representation
//

#include <stdlib.h>

#include "ftnstd.h"
#include "target.h"

// IMPORTANT: READ ME
// After the next branch, i.e. when we start using 11.0 libraries we
// this should be changed back to gcvt() from __gcvt().
// __gcvt() makes use of the __LDCvt which gives us the required amount
// of precision.  In the 11.0 and higher libraries gcvt() also use __LDCvt().
extern char     *__gcvt( double __val, int __ndig, char *__buf );


void    CnvD2S( double *val, char *buff ) {
// Convert floating point number to string.

    __gcvt( *val, CONVERSION_DIGITS, buff );
}


void    CnvX2S( extended *val, char *buff ) {
// Convert floating point number to string.

    __gcvt( *val, CONVERSION_DIGITS, buff );
}

void    CnvS2S( float *val, char *buff ) {
// Convert floating point number to string.

    double      dval;

    dval = *val;
    CnvD2S( &dval, buff );
}


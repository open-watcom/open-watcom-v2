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


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %     Copyright (C) 1992, by WATCOM International Inc.  All rights    %
// %     reserved.  No part of this software may be reproduced or        %
// %     used in any form or by any means - graphic, electronic or       %
// %     mechanical, including photocopying, recording, taping or        %
// %     information storage and retrieval systems - except with the     %
// %     written permission of WATCOM International Inc.                 %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//  Modified    By              Reason
//  ========    ==              ======
//  93/05/26    Greg Bentz      pull floating point out of istream/ostream
//  93/10/08    Greg Bentz      make LDFloatToString set scale_factor to 1
//                              for _Ftos when 'G' format
//  93/10/25    Raymond Tang    Split into separate files.
//  94/04/06    Greg Bentz      combine header files
//  95/04/20    F.W.Crigger     use the new __LDcvt routine
//  95/05/24    F.W.Crigger     newer version of __LDcvt
//  95/06/19    Greg Bentz      indirect call to math library

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iostream.h>
#endif
#include "ioutil.h"
#include "iofhdr.h"
#include "xfloat.h"

void __LDFloatToString( char *buf, double const *f, int precision,
/*********************************************************************/
    ios::fmtflags format_flags ) {

    int         i;
    CVT_INFO    cvt;
    long_double ld;
    auto char   stkbuf[34];
#ifdef _LONG_DOUBLE_
    double      double_value;

    /* convert this double into a long double */
    double_value = *f;
    __FDLD( (double near *)&double_value, (long_double near *)&ld );
#else
    ld.value = *f;
#endif

    if( (format_flags & (ios::scientific|ios::fixed)) == ios::scientific ) {
        cvt.flags = E_FMT;
        cvt.scale = 1;
    } else if( (format_flags & (ios::scientific|ios::fixed)) == ios::fixed ) {
        cvt.flags = F_FMT;
        cvt.scale = 0;
    } else {
        cvt.flags = G_FMT;
        cvt.scale = 1;
        if( precision == 0 ) {
            precision = 1;
        }
    }
    if( (format_flags & ios::showpoint) ) {
        cvt.flags |= F_DOT;
    }
    cvt.ndigits = precision;
    cvt.expchar = (format_flags & ios::uppercase) ? 'E' : 'e';
    cvt.expwidth = 0;
    __EFG_LDcvt( &ld, &cvt, stkbuf );
    // put all the pieces together
    i = 0;
    if( cvt.sign < 0 ) {
        buf[i++] = '-';
    } else if( format_flags & ios::showpos ) {
        buf[i++] = '+';
    }
    if( cvt.n1 != 0 ) {
        memcpy( &buf[i], &stkbuf[0], cvt.n1 );
        i += cvt.n1;
    }
    memset( &buf[i], '0', cvt.nz1 );
    i += cvt.nz1;
    if( cvt.n2 != 0 ) {
        memcpy( &buf[i], &stkbuf[cvt.n1], cvt.n2 );
        i += cvt.n2;
    }
    memset( &buf[i], '0', cvt.nz2 );
    i += cvt.nz2;
    buf[i] = '\0';
}

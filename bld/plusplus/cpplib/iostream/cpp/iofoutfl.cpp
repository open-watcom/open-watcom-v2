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
//  99/10/12    J.B.Schueler    ensure that we don't walk over end of buffer

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iostream.h>
#endif
#include "ioutil.h"
#include "lock.h"
#include "iofhdr.h"


enum {
    FLOAT_OVERHEAD  = 7,            // 2 signs, decimal, E, 3-digit exponent
    EXTRA_DIG       = 10,           // some extra for useless mantissa digits
    MAX_PREC        = LDBL_DIG + EXTRA_DIG,
    LONGEST_FLOAT   = LDBL_DIG + EXTRA_DIG + FLOAT_OVERHEAD
};

#define MAX_DIGITS LDBL_DIG+EXTRA_DIG

ostream &ostream::__outfloat( long double const &f ) {
/****************************************************/
// Write a "long double" floating-point value.

    int   digit_offset;
    int   precision;
    char  buf[LONGEST_FLOAT+1];

    ::memset( buf, 0, sizeof( buf ) );
    __lock_it( __i_lock );
    precision = this->precision();
    if( precision > MAX_PREC ) precision = MAX_PREC;
    __LDFloatToString( buf, (double const *)&f, precision, this->flags() );
    if( buf[0] == '-' || buf[0] == '+' ) {
        digit_offset = 1;
    } else {
        digit_offset = 0;
    }
    if( opfx() ) {
        setstate( __WATCOM_ios::writeitem( *this, buf, ::strlen(buf), digit_offset ) );
        osfx();
    }
    return( *this );
}


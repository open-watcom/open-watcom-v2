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
//  92/02/04    Steve McDowell  Initial implementation.
//  92/09/08    Greg Bentz      Cleanup.
//  93/03/15    Greg Bentz      fix uninitialized state variables.
//  93/07/29    Greg Bentz      - change istream::op>>(streambuf &) to
//                                istream::op>>( streambuf * )
//                              - fix istream::getline() to not set failbit
//                                if no input stored in user buffer
//  93/09/15    Greg Bentz      change getline() back to set ios::failbit
//                              if not input stored in user buffer
//  93/10/21    Greg Bentz      change get() and getline() to not set failbit
//                              if the delim character has been seen
//  93/10/28    Raymond Tang    Split into separate files.
//  93/12/08    Raymond Tang    Change ipfx() to an inline function.
//  94/04/06    Greg Bentz      combine header files

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iostream.h>
#endif
#include "lock.h"

int istream::do_ipfx( int noskipws ) {
/*********************************/
// Input prefix.

    __lock_it( __i_lock );
    if( !good() ) {
        setstate( ios::failbit );
        return( 0 );
    }
    __last_read_length = 0;     // should this be moved to the functions that
                                // actually set a value?

    // Flush any tied streams, if noskipws == 0
    if( tie() != NULL ) {
        tie()->flush();
    }

    // Skip whitespace if ios::skipws is set and noskipws == 0.
    if( !noskipws && ((flags() & ios::skipws) != 0) ) {
        ws( *this );
        if( eof() ) {
            setstate( ios::failbit );
        }
    }

    // Ensure the error state is still 0:
    return( good() );
}

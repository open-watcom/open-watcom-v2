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
//  92/01/22    Steve McDowell  Initial implementation.
//  92/09/08    Greg Bentz      Cleanup.
//  93/03/05    Greg Bentz      added exception handling
//  93/03/15    Greg Bentz      fix ios::clear() mistake during exceptions
//  93/10/29    Raymond Tang    Split into separate files.
//  94/04/06    Greg Bentz      combine header files

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iostream.h>
#endif
#include "ioutil.h"
#include "lock.h"

ios &ios::operator = ( const ios &is ) {
/**************************************/
// Construct an "ios" and initialize it.
// New "ios" gets all fields of "is" except streambuf.

    __lock_it( __lock_first( __i_lock, is.__i_lock ) );
    __lock_it( __lock_second( __i_lock, is.__i_lock ) );
    __lock_it( __x_lock );
    __error_state        = is.__error_state;
    __format_flags       = is.__format_flags;
    __enabled_exceptions = is.__enabled_exceptions;
    __float_precision    = is.__float_precision;
    __field_width        = is.__field_width;
    __fill_character     = is.__fill_character;
    __tied_stream        = is.__tied_stream;


    // copy over the xalloc vector
    if( __xalloc_index > 0 ) {
        // make sure that 'this' has enough entries in it
        (void)__WATCOM_ios::find_user_word( this, __xalloc_index-1 );

        int index = 0;
        ios_word_values *src_word;
        src_word = (ios_word_values *)is.__xalloc_list;
        for( ; src_word != NULL; src_word = src_word->next ) {
            ipvalue *src, *tgt;
            for( int i = 0 ; i < src_word->count ; i++ ) {
                src = (ipvalue *)&(src_word->values[i]);
                tgt = (ipvalue *)__WATCOM_ios::find_user_word( this, index );
                if( tgt == NULL ) {
                    setstate( ios:: failbit );
                    return( *this );
                }
                *tgt = *src;
                index++;
            }
        }
    }
    return( *this );
}

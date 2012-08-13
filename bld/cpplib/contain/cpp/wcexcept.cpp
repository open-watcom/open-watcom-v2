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


#include "variety.h"
#include <stdlib.h>
#include <wcdefs.h>
#include <wcexcept.h>

#if defined(__SW_XSS) || defined(__SW_XST)
#define __SW_XS
#endif


//
// Throw an empty_container exception if it is enabled
//

_WPRTLINK void WCExcept::base_throw_empty_container() const {
#ifdef __SW_XS
    if( (exceptions() & empty_container) != 0 ) {
        failure oops( empty_container );
        throw( oops );
    }
#endif
}



//
// Throw an index_range exception if it is enabled
//

_WPRTLINK void WCExcept::base_throw_index_range() const {
#ifdef __SW_XS
    if( (exceptions() & index_range) != 0 ) {
        failure oops( index_range );
        throw( oops );
    }
#endif
}



//
// Throw a not_empty exception if it is enabled
//

_WPRTLINK void WCExcept::base_throw_not_empty() {
#ifdef __SW_XS
    if( (exceptions() & not_empty) != 0 ) {
        failure oops( not_empty );
        // to help prevent more exceptions being thrown
        exception_flags = 0;
        throw( oops );
    }
#endif
}



//
// Throw a not_unique exception if it is enabled
//

_WPRTLINK void WCExcept::base_throw_not_unique() const {
#ifdef __SW_XS
    if( (exceptions() & not_unique) != 0 ) {
        failure oops( not_unique );
        throw( oops );
    }
#endif
}



//
// Throw an out_of_memory exception if it is enabled
//

_WPRTLINK void WCExcept::base_throw_out_of_memory() const {
#ifdef __SW_XS
    if( (exceptions() & out_of_memory) != 0 ) {
        failure oops( out_of_memory );
        throw( oops );
    }
#endif
}



//
// Throw a resize_required exception if it is enabled
//

_WPRTLINK void WCExcept::base_throw_resize_required() const {
#ifdef __SW_XS
    if( (exceptions() & resize_required) != 0 ) {
        failure oops( resize_required );
        throw( oops );
    }
#endif
}



//
// Throw a zero_buckets exception if it is enabled
//

_WPRTLINK void WCExcept::base_throw_zero_buckets() const {
#ifdef __SW_XS
    if( (exceptions() & zero_buckets) != 0 ) {
        failure oops( zero_buckets );
        throw( oops );
    }
#endif
}



//
// Throw a iter_range exception if it is enabled
//

_WPRTLINK void WCIterExcept::base_throw_iter_range() const {
#ifdef __SW_XS
    if( (exceptions() & iter_range) != 0 ) {
        failure oops( iter_range );
        throw( oops );
    }
#endif
}



//
// Throw a undef_item exception if it is enabled
//

_WPRTLINK void WCIterExcept::base_throw_undef_item() const {
#ifdef __SW_XS
    if( (exceptions() & undef_item) != 0 ) {
        failure oops( undef_item );
        throw( oops );
    }
#endif
}



//
// Throw a undef_iter exception if it is enabled
//

_WPRTLINK void WCIterExcept::base_throw_undef_iter() const {
#ifdef __SW_XS
    if( (exceptions() & undef_iter) != 0 ) {
        failure oops( undef_iter );
        throw( oops );
    }
#endif
}

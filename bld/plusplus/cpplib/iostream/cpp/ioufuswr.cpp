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

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iostream>
#endif
#include "ioutil.h"
#include "liballoc.h"

void *__WATCOM_ios::find_user_word( std::ios *pios, int index ) {

    ios_word_values *wptr;
    ios_word_values *last_wptr;
    ipvalue         *user_word_ptr;

    if( (index < 0) || (index >= ios::__xalloc_index) ) {
        return( NULL );
    }
    last_wptr = NULL;
    wptr = (ios_word_values *) pios->__xalloc_list;
    for( ; wptr != NULL; wptr = wptr->next ) {
        if( wptr->count > index ) {
            return( &(wptr->values[index]) );
        }
        index    -= wptr->count;
        last_wptr = wptr;
    }

// "index" now contains 1 less than the number of (long) or (void *)
// values we need.
// "last_wptr" points at the last group of values.

    wptr = (ios_word_values *) _plib_malloc( sizeof( ios_word_values )
                                   + index * sizeof( ipvalue ) );
    if( wptr == NULL ) {
        return( NULL );
    }
    if( last_wptr == NULL ) {
        pios->__xalloc_list = wptr;
    }  else {
        last_wptr->next = wptr;
    }
    wptr->next  = NULL;
    wptr->count = (short)(index + 1);
    user_word_ptr = &(wptr->values[0]);
    for( int i = 0 ; i <= index ; i++ ) {
        user_word_ptr->iword = 0L;
        user_word_ptr++;
    }
    return( &(wptr->values[index]) );
}

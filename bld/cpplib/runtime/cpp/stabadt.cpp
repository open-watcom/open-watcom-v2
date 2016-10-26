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


#include "cpplib.h"
#include "rtexcept.h"
#include "rtarctor.h"


void * dtor_the_array(          // CALL DESTRUCTORS FOR ARRAY ELEMENTS
    _RTARCTOR* cta )            // - array-control
{
    void *array;                // - array
    RT_TYPE_SIG sig;            // - signature of array
    pFUNdtor dtor;              // - destructor for an element
    size_t size;                // - size of an array element

    sig = cta->_sig;
    size = sig->clss.size;
    array = ( (char*)cta->_array + size * cta->_count );
    dtor = sig->clss.dtor;
    while( cta->_count > 0 ) {
        -- *cta;
        array = ( (char*)array - size );
        (*dtor)( array, 0 );
    }
    cta->complete();
    return cta->_array;
}


#ifdef RT_EXC_ENABLED
_RTARCTOR::~_RTARCTOR           // DESTRUCTOR FOR R/T AR-CTOR
    ( void )
{
    if( alive() ) {
        dtor_the_array( this );
    }
}
#endif

#ifndef NDEBUG
_RTARCTOR::_RTARCTOR            // COPY CTOR FOR R/T AR-CTOR
    ( _RTARCTOR const & )
    : _count( 0 )
    , _sig( 0 )
    , _array( 0 )
{
    GOOF_EXC( "Attempt to copy RTARCTOR" );
}
#endif


extern "C"
_WPRTLINK
void * CPPLIB( dtor_array )(    // CALL DESTRUCTORS FOR ARRAY ELEMENTS
    void *array,                // - array
    unsigned count,             // - number of elements
    RT_TYPE_SIG sig )           // - signature of array
{
    // - array-ctor control
    _RTARCTOR ar_ctor( count, sig, array );

    dtor_the_array( &ar_ctor );
    return array;
}

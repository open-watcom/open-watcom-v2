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
#include "rtdtor.h"

#ifdef RT_EXC_ENABLED

struct _CTAS;
typedef void (*pFunDealloc)( _CTAS* );

struct _CTAS                    // CTAS -- controls CTOR or ARRAY_STORAGE
    : public _RTDTOR
{
    pFunDelete op_del;          // - operator delete[] to be used
    void* memory;               // - addr[ memory to delete ]
    RT_TYPE_SIG sig;            // - type signature
    pFunDealloc dealloc;        // - deallocation function


    ~_CTAS();                   // - destructor
};


_CTAS::~_CTAS()                 // DESTRUCTOR
{
    if( alive() ) {
        (*dealloc)( this );
    }
}


// The following are fake DTORs which are called only during exception
// handling.
//
// Their purpose is to delete the memory acquired by new[] when the CTOR'ing
// of the memory causes an exception to be thrown which is caught past
// the new[] point.
//
// When CTOR'ing succeeds, the run-time support pops the registration
// without destructing.
//
static void ctas_delete_1(          // DELETION FUNCTION - 1 ARG.
    _CTAS* ctas )                   // - control data
{
    (*ctas->op_del.delete_1)( ctas->memory );
}


static void ctas_delete_2(          // DELETION FUNCTION - 2 ARGS.
    _CTAS* ctas )                   // - control data
{
    ARRAY_STORAGE* ar               // - array storage for array
        = (ARRAY_STORAGE*)ctas->memory;

    (*ctas->op_del.delete_2)( ar
                            , ( CPPLIB( ts_size )( ctas->sig ) *
                                ar->element_count ) +
                                sizeof( ar->element_count )
                            );
}


#endif


static void* ctor_array             // CTOR THE ARRAY
    ( void *array                    // - array memory
    , unsigned count                // - number of elements
    , RT_TYPE_SIG sig               // - type signature for array type
#ifdef RT_EXC_ENABLED
    , void (*op_del)()              // - operator delete[] to be used
    , pFunDealloc dealloc           // - deallocation function
    , void *mem                     // - allocated memory
#endif
    )
{
    void* (*rtn)( void*, unsigned, RT_TYPE_SIG ); // - ctor R/T routine
#ifdef RT_EXC_ENABLED
    _CTAS ctas;                     // - controls CTOR or ARRAY_STORAGE

    ctas.memory = mem;
    ctas.op_del.delete_1 = (pFunDelete1)op_del;
    ctas.dealloc = dealloc;
    ctas.sig = sig;
#else
    if( 0 != array ) {
#endif
        if( sig->hdr.type == THROBJ_CLASS_VIRT ) {
            rtn = &CPPLIB( ctor_varray );
        } else {
            rtn = &CPPLIB( ctor_array );
        }
        array = (*rtn)( array, count, sig );
#ifdef RT_EXC_ENABLED
        ctas.complete();
#else
    }
#endif
    return array;
}


#ifdef RT_EXC_ENABLED
static void* ctor_array_memory(     // CTOR ARRAY MEMORY
    void* array,                    // - array memory
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig,                // - type signature for array type
    void (*op_del)(),               // - operator delete[] to be used
    pFunDealloc dealloc )           // - deallocation function
{
    if( array != 0 ) {
        array = ctor_array( array
                          , count
                          , sig
                          , op_del
                          , dealloc
                          , array
                          );
    }
    return array;
}
#endif


static void* ctor_array_storage     // CTOR ARRAY_STORAGE
    ( ARRAY_STORAGE *base           // - array storage
    , unsigned count                // - number of elements
    , RT_TYPE_SIG sig               // - type signature for array type
#ifdef RT_EXC_ENABLED
    , void (*op_del)()              // - operator delete[] to be used
    , pFunDealloc dealloc           // - deallocation function
#endif
    )
{
    void* array;                    // - array

    if( base == 0 ) {
        array = 0;
    } else {
        base->element_count = count;
        array = ctor_array( base->apparent_address
                          , count
                          , sig
#ifdef RT_EXC_ENABLED
                          , op_del
                          , dealloc
                          , base
#endif
                          );
    }
    return array;
}


extern "C"
_WPRTLINK
void* CPPLIB( ctor_array_storage_1m )( // CTOR ARRAY MEMORY, SUPPLIED DELETE[]
    void* array,                    // - array memory
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig,                // - type signature for array type
    void (*op_del)()                // - operator delete[] to be used
    )
{
#ifdef RT_EXC_ENABLED
    return ctor_array_memory( array
                            , count
                            , sig
                            , op_del
                            , &ctas_delete_1
                            );
#else
    op_del = op_del;
    return ctor_array( array, count, sig );
#endif
}


extern "C"
_WPRTLINK
void* CPPLIB( ctor_array_storage_1s )( // CTOR ARRAY_STORAGE, SUPPLIED DELETE[]
    ARRAY_STORAGE *base,            // - array storage
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig,                // - type signature for array type
    void (*op_del)() )              // - operator delete[] to be used
{
#ifndef RT_EXC_ENABLED
    op_del = op_del;
#endif
    return ctor_array_storage( base
                             , count
                             , sig
#ifdef RT_EXC_ENABLED
                             , op_del
                             , &ctas_delete_1
#endif
                             );
}


extern "C"
_WPRTLINK
void* CPPLIB( ctor_array_storage_2s )( // CTOR ARRAY_STORAGE, SUPPLIED DELETE[]
    ARRAY_STORAGE *base,            // - array storage
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig,                // - type signature for array type
    void (*op_del)() )              // - operator delete[] to be used
{
#ifndef RT_EXC_ENABLED
    op_del = op_del;
#endif
    return ctor_array_storage( base
                             , count
                             , sig
#ifdef RT_EXC_ENABLED
                             , op_del
                             , &ctas_delete_2
#endif
                             );
}


extern "C"
_WPRTLINK
void* CPPLIB( ctor_array_storage_gm )( // CTOR ARRAY MEMORY, default
    void* array,                    // - array memory
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - type signature for array type
{
#ifdef RT_EXC_ENABLED
    auto void (*op_del)();          // - operator delete[] to be used

    op_del = ( __typeof(op_del) )&operator delete[];
    return ctor_array_memory( array
                            , count
                            , sig
                            , op_del
                            , &ctas_delete_1
                            );
#else
    return ctor_array( array, count, sig );
#endif
}


extern "C"
_WPRTLINK
void* CPPLIB( ctor_array_storage_gs )( // CTOR ARRAY_STORAGE, default
    ARRAY_STORAGE *base,            // - array storage
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - type signature for array type
{
#ifdef RT_EXC_ENABLED
    auto void (*op_del)();          // - operator delete[] to be used

    op_del = ( __typeof(op_del) )&operator delete[];
#endif
    return ctor_array_storage( base
                             , count
                             , sig
#ifdef RT_EXC_ENABLED
                             , op_del
                             , &ctas_delete_1
#endif
                             );
}

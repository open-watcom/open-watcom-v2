
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
* Description:  Definitions for C++ library
*
****************************************************************************/

#ifndef __CPPLIB_H__
#define __CPPLIB_H__

#ifndef __cplusplus
    #error CPPLIB.H only compiles under C++
#endif

#include "variety.h"
#ifdef _WPRTLINK_RESIDENT
  #undef _WPRTLINK
  #define _WPRTLINK __declspec(__watcall)
  #undef _WPRTDATA
  #define _WPRTDATA __declspec(__watcall)
#endif

struct  ACTIVE_EXC;
struct  DISPATCH_EXC;
union   RW_DTREG;
union   RO_DTREG;
struct  THREAD_CTL;
struct  _EXC_PR;

#include <stddef.h>

#define PointUsingOffset( type, base, offset ) \
    ( (type*)( (char*)base + offset ) )
#define PointOffset( base, offset ) \
    PointUsingOffset( void, base, offset )

// this is necessary to avoid unnecessary overhead on pointer arithmetic
#ifdef __HUGE__
#define RT_FAR __far
typedef unsigned    RT_STATE_VAR;       // State variable
typedef unsigned    offset_t;           // offset
#else
#define RT_FAR
typedef size_t      RT_STATE_VAR;       // State variable
typedef size_t      offset_t;           // offset
#endif

// TYPEDEFS : c vs c++ language insensitive typedefs

typedef void (*pFUNVOIDVOID)                    // function returns void
                ( void );                       // - takes void
typedef int  (*pFUNINTUNSIGNED)                 // function returns int
                ( unsigned );                   // - takes unsigned
typedef void (*pFUNpVOIDVOID)                   // function returns void
                ( void * );                     // - takes void *
typedef void *(*pFUNdtor)                       // dtor
                ( void *                        // - address
                , unsigned );                   // - dtor vbase flag
typedef void (*pFUNctor)                        // ctor
                ( void * );                     // - address
typedef void (*pFUNctorV)                       // ctor
                ( void *                        // - address
                , unsigned );                   // - ctor vbase flag
typedef void (*pFUNcopy)                        // copy ctor
                ( void *                        // - target
                , void * );                     // - source
typedef void (*pFUNcopyV)                       // copy ctor
                ( void *                        // - target
                , unsigned                      // - ctor vbase flag
                , void * );                     // - source

// twits at Microsoft define "boolean" in their Win32 header files
typedef int rboolean;

// set up indication of -xs

#if defined( __SW_XS ) || defined( __SW_XSS ) || defined( __SW_XST )
#define RT_EXC_ENABLED
#endif

extern "C" {

#include "wcpp.h"

#include "prtdata.h"

#ifdef __SW_BM
    #include "thread.h"
    #include <lock.h>
#endif

#if defined( __USE_FS ) || defined( __USE_RW ) || defined( __USE_PD )
    #include "fsreg.h"
#endif

#if defined( _M_I86 )
    // pad to 2-byte boundary
    #define AlignPad1 char padding[1];
    #define AlignPad2 ;
#elif defined( __ALPHA__ ) || defined( __386__ )
    // pad to 4-byte boundary
    #define AlignPad1 char padding[3];
    #define AlignPad2 char padding[2];
#else
    #error bad target
#endif


//************************************************************************
// Type Signatures
//************************************************************************

union rt_type_sig;                  // union of all signatures
typedef rt_type_sig *RT_TYPE_SIG;   // RT_TYPE_SIG

struct TYPE_SIG_HDR             // TYPE SIGNATURE: BASE
{   THROBJ type;                // - object type
};

struct TYPE_SIG_SCALAR          // TYPE SIGNATURE: SCALAR, POINTER TO CLASS
{   TS_HDR hdr;                 // - header
    size_t size;                // - size of scalar
    char const * name;          // - type-signature name
};

struct TYPE_SIG_CLASS           // TYPE SIGNATURE: CLASS (NO VIRTUAL BASES)
{   TS_HDR hdr;                 // - header
    pFUNctor ctor;              // - addr[ default CTOR ]
    pFUNcopy copyctor;          // - addr[ copy CTOR ]
    pFUNdtor dtor;              // - addr[ DTOR ]
    size_t size;                // - size of object thrown
    char const * name;          // - type-signature name
};

struct TYPE_SIG_CLASS_VIRT      // TYPE SIGNATURE: CLASS (VIRTUAL BASES)
{   TS_HDR hdr;                 // - header
    pFUNctorV ctor;             // - addr[ default CTOR ]
    pFUNcopyV copyctor;         // - addr[ copy CTOR ]
    pFUNdtor dtor;              // - addr[ DTOR ]
    size_t size;                // - size of object thrown
    char const * name;          // - type-signature name
};

struct TYPE_SIG_BASE            // TYPE SIGNATURE: INDIRECTED
{   TS_HDR base;                // - header
    RT_TYPE_SIG sig;            // - indirected signature
};

union rt_type_sig               // RT_TYPE_SIG: one of
{   TYPE_SIG_HDR hdr;           // - general header: object
    TS_HDR base;                // - general header: base
    TYPE_SIG_BASE indirected;   // - indirected
    TYPE_SIG_SCALAR scalar;     // - scalar
    TYPE_SIG_CLASS clss;        // - class (no virtual bases)
    TYPE_SIG_CLASS_VIRT clss_v; // - class (with virtual bases)
};



//************************************************************************
// Throw Definitions
//************************************************************************

struct THROW_CNV                // THROW CONVERSION
{   RT_TYPE_SIG signature;      // - type signature
    size_t offset;              // - offset
};

struct THROW_RO
{   size_t count;               // - number of throw conversions
    THROW_CNV cnvs[1];          // - throw conversions
};


//************************************************************************
// Miscellaneous definitions
//************************************************************************

struct ARRAY_STORAGE            // ARRAY_STORAGE -- structure of array
{   size_t element_count;       // - number of elements
    char apparent_address[];    // - address used in code
};

#define ArrayStorageFromArray( array ) \
    (ARRAY_STORAGE*) ( (char*)(array) - sizeof( ARRAY_STORAGE ) )

//*** PLEASE NOTE !!! ****************************************************
// If struct thread_ctl changes in size, be sure to update
// struct wint_thread_ctl in clib\h\thread.h to reflect the change.
//
// See CGBKRREG.C
//  - offset of thread_ctl.registered, thread_ctl.flags is known
//
//************************************************************************

struct THREAD_CTL               // THREAD_CTL -- control execution thread
{
    union {
        RW_DTREG* registered;   // - list of registrations, execution
        THREAD_CTL* call_base;  // - furthest THREAD_CTL down call chain
    };
    union {
        struct {                    // - flags:
            unsigned terminated :1; // - - "terminate" called
            unsigned executable :1; // - - is .EXE (not .DLL)
        } flags;
        void *d0;               // - padding
    };
    ACTIVE_EXC *excepts;        // - exceptions being handled
    char *abort_msg;            // - abortion message
    pFUNVOIDVOID unexpected;    // - "unexpected" routine
    pFUNVOIDVOID terminate;     // - "terminate" routine
    pFUNVOIDVOID new_handler;   // - new_handler routine (ANSI)
    pFUNINTUNSIGNED _new_handler;//- _new_handler routine (Microsoft)
    _EXC_PR* exc_pr;            // - controls exceptional processing
};


//************************************************************************
// GLOBAL DATA
//************************************************************************

//************************************************************************
// Per Thread Data
// Storage is allocated in in cppdata.obj for non multi-thread or
// by the clib BeginThread() routine for multi-thread.
//************************************************************************
#ifndef __SW_BM
_WPRTLINK extern THREAD_CTL _wint_thread_data;
#elif defined( _M_I86 )
#else
_WPRTLINK extern unsigned   _wint_thread_data_offset;
#endif

//*** PLEASE NOTE !!! ****************************************************
// If the _ThreadData macro changes for multi-thread, be sure to
// update clib\h\thread.h
//************************************************************************
#ifndef __SW_BM
  #define _ThreadData       _wint_thread_data
#elif defined( _M_I86 )
  #define _ThreadData (*((THREAD_CTL*)&(__THREADDATAPTR->_wint_thread_data)))
#else
  #define _ThreadData (*((THREAD_CTL*)(((char *)__THREADDATAPTR)+_wint_thread_data_offset)))
#endif

//************************************************************************
// Per Process Data
// storage is allocated in prwdata.asm for everyting
//************************************************************************
extern short                _wint_pure_error_flag;
extern short                _wint_undef_vfun_flag;
extern RW_DTREG*            _wint_module_init;
#define _PureErrorFlag      _wint_pure_error_flag
#define _UndefVfunFlag      _wint_undef_vfun_flag
#define _ModuleInit         _wint_module_init

#ifdef __SW_BM
extern __lock               _wint_static_init_sema;
#define _StaticInitSema     _wint_static_init_sema
#endif

//************************************************************************
// PROTOTYPES
//************************************************************************

_NORETURN
void CPPLIB( call_terminate )(      // CALL "terminate" SET BY "set_terminate"
    char* abort_msg,                // - abortion message
    THREAD_CTL *thr )               // - thread-specific data
;
_NORETURN
void CPPLIB( call_unexpected )(     // CALL "unexpected" SET BY "set_unexpected"
    void )
;
_WPRTLINK
void * CPPLIB( copy_array )(        // CALL COPY CONSTRUCTORS FOR ARRAY
    void *tgt_array,                // - target array (element w/o virtual base)
    void *src_array,                // - source array
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - signature of array
;
_WPRTLINK
void * CPPLIB( copy_varray )(       // CALL COPY CONSTRUCTORS FOR ARRAY
    void *tgt_array,                // - target array (element has virtal base)
    void *src_array,                // - source array
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - signature of array
;
_WPRTLINK
void * CPPLIB( ctor_array )(        // CALL CONSTRUCTORS FOR NEW ARRAY ELEMENTS
    void *array,                    // - array
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - signature of array
;
_WPRTLINK
void * CPPLIB( ctor_varray )(       // CALL CONSTRUCTORS FOR NEW ARRAY ELEMENTS
    void *array,                    // - array
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - signature of array
;
_WPRTLINK
void * CPPLIB( dtor_array )(        // CALL DESTRUCTORS FOR ARRAY ELEMENTS
    void *array,                    // - array
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - signature of array
;
_WPRTLINK
ARRAY_STORAGE * CPPLIB( dtor_array_store )(  // CALL DTORS FOR ARRAY_STORAGE
    void *array,                    // - array
    RT_TYPE_SIG sig )               // - type signature for array type
;
void CPPLIB( dtor_del_array )(      // CALL DTORS FOR ARRAY ELEMENTS AND DELETE
    void *array,                    // - array
    RT_TYPE_SIG sig )               // - type signature for array type
;
void CPPLIB( dtor_del_array_x )(    // CALL DTORS FOR ARRAY ELEMENTS AND DELETE
    void *array,                    // - array
    RT_TYPE_SIG sig )               // - type signature for array type
;
_WPRTLINK
pFUNVOIDVOID CPPLIB( lj_handler )   // GET ADDRESS OF longjmp HANDLER
    ( void )
;
void CPPLIB( module_dtor )          // DTOR STATICS FOR PROGRAM
    ( void )
;
void CPPLIB( multi_thread_init )    // INITIALIZER FOR MULTI-THREAD DATA
    ( void )
;
void* CPPLIB( new_allocator )(      // DEFAULT ALLOCATOR FOR NEW, NEW[]
    size_t size )                   // - size required
;
void * CPPLIB( new_array )(         // CALL NEW AND CTORS FOR ARRAY ELEMENTS
    ARRAY_STORAGE *new_alloc,       // - what was allocated
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - type signature for array type
;
void * CPPLIB( new_varray )(        // CALL NEW AND CTORS FOR ARRAY ELEMENTS
    ARRAY_STORAGE *new_alloc,       // - what was allocated
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - type signature for array type
;
void * CPPLIB( new_darray )(        // CALL NEW AND CTORS FOR ARRAY ELEMENTS
    ARRAY_STORAGE *new_alloc,       // - what was allocated
    unsigned count )                // - number of elements
;
_WPRTLINK
void CPPLIB( pure_error )(          // TRAP NON-OVERRIDDEN PURE VIRTUAL CALLS
    void )
;
_NORETURN
void CPPLIB( fatal_runtime_error )( // FATAL RUNTIME ERROR
    char *msg,                      // - diagnostic message
    int code )                      // - exit code
;
rboolean CPPLIB( ts_equiv )(        // TEST IF TYPE SIG.S ARE EQUIVALENT
    RT_TYPE_SIG tgt,                // - target type signature
    RT_TYPE_SIG src,                // - source type signature
    rboolean zero_thrown )          // - true ==> zero was thrown
;
RT_TYPE_SIG CPPLIB( ts_pnted )(     // POINT PAST POINTER TYPE-SIG
    RT_TYPE_SIG sig )               // - the signature
;
RT_TYPE_SIG CPPLIB( ts_refed )(     // POINT PAST REFERENCE TYPE-SIG, IF REQ'D
    RT_TYPE_SIG sig )               // - the signature
;
size_t CPPLIB( ts_size )(           // GET SIZE OF ELEMENT FROM TYPE SIGNATURE
    RT_TYPE_SIG sig )               // - type signature
;
_NORETURN
_WPRTLINK
void CPPLIB( undefed_cdtor )(       // ISSUE ERROR FOR UNDEFINED CTOR, DTOR
    void )
;
void CPPLIB( unmark_bitvect )(      // UNMARK LAST BIT IN BIT-VECTOR
    uint_8 *bit_vect,               // - bit vector
    size_t bit_count )              // - # bits in vector
;

#ifndef NDEBUG
    #define GOOF( msg ) CPPLIB( fatal_runtime_error )( "*** GOOF **" msg, 1 )
#else
    #define GOOF( msg )
#endif

#define KLUGE_CPPLIB_END
#include "kluges.h"

};  // extern "C"


#endif

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


#ifndef __FSREG_H__
#define __FSREG_H__

#include <variety.h>

// FSREG.H -- registration using fs register
//
// 94/10/11 -- J.W.Welch            -- defined
//
// compilation command line defines:
//  __USE_FS : combined with target system, FS_REGISTRATION_.. is set
//  __USE_RW : causes SYSIND_REGISTRATION to be set
//
// RW_REGISTRATION should be defined in the library whenever
// CompFlags.rw_registration is true in the compiler
//  - means that R/W block contains link to R/O block
//  - set when either FS_REGISTRATION or SYSIND_REGISTRATION is defined
//
// FS_REGISTRATION should be defined in the library whenever
// CompFlags.fs_registration is true in the compiler
//  - requires RW_REGISTRATION to be defined
//  - means that R/W blocks are chained from ptr located by FS register
//
// One of the following will be defined when FS_REGISTRATION is defined:
//  FS_REGISTRATION_OS2 -- OS/2
//  FS_REGISTRATION_NT  -- NT
//  FS_REGISTRATION_RW  -- system-independent, but using FS register

struct FsExcRec;

#if defined( __NT__ ) && defined( __USE_FS )

#ifdef __cplusplus
    extern "C" {
#endif

    #ifdef __NEED_SYSTEM_HEADER
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <excpt.h>
    // the idiots at MS define these C++ keywords as macros
    #undef try
    #undef catch
    #undef throw

    #define EXC_HAND_CONTINUE   ExceptionContinueSearch
    #define EXC_HAND_CATCH      ExceptionContinueExecution
    #define EXC_HAND_UNWOUND    ExceptionContinueExecution

    void __stdcall RtlUnwind            // can't find def'n
                    ( CONTEXT *
                    , void *
                    , EXCEPTION_RECORD *
                    , void * );

    #define FS_UNWIND_GLOBAL( a, b, c )                             \
                RtlUnwind( (CONTEXT*)(a)                            \
                         , (void*)(b)                               \
                         , (EXCEPTION_RECORD*)(c)                   \
                         , 0 )

    #define FS_RAISE_EXCEPTION( a )                                 \
                RaiseException( (a)->code                       \
                              , (a)->flags                      \
                              , (a)->parm_count                 \
                              , (DWORD const *)&((a)->object) )

    #define EXC_TYPE_UNWIND_EXIT   4    // can't find def'n
    #define EXC_TYPE_UNWIND_NORMAL 2    // can't find def'n

    #endif

    #define ThreadLookup CPPLIB( fs_lookup )


#ifdef __cplusplus
    };
#endif

    #define EXCREC_FLAGS        0

    #define FS_REGISTRATION
    #define FS_REGISTRATION_NT

    #define FS_REGISTRATION_CONVENTION  __cdecl

    #define RW_REGISTRATION



#elif defined( __OS2__ ) && defined( __USE_FS )

    #ifdef __cplusplus
        extern "C" {
    #endif

    #ifdef __NEED_SYSTEM_HEADER

    #define INCL_DOSEXCEPTIONS
    #include <os2.h>

    #define EXC_TYPE_UNWIND_EXIT   EH_EXIT_UNWIND
    #define EXC_TYPE_UNWIND_NORMAL EH_UNWINDING

    #define EXC_HAND_CONTINUE   XCPT_CONTINUE_SEARCH
    #define EXC_HAND_CATCH      XCPT_CONTINUE_EXECUTION
    #define EXC_HAND_UNWOUND    XCPT_CONTINUE_EXECUTION

    #define FS_UNWIND_GLOBAL( a, b, c )                             \
                DosUnwindException( (PEXCEPTIONREGISTRATIONRECORD)a \
                                  , (PVOID)b                        \
                                  , (PEXCEPTIONREPORTRECORD)c )

    #define FS_RAISE_EXCEPTION( a )                                 \
                DosRaiseException( (PEXCEPTIONREPORTRECORD)a )

    #endif

    #define ThreadLookup CPPLIB( fs_lookup )

    #ifdef __cplusplus
        };
    #endif

    #define EXCREC_FLAGS        0

    #define FS_REGISTRATION
    #define FS_REGISTRATION_OS2

    #define FS_REGISTRATION_CONVENTION  __syscall

    #define RW_REGISTRATION



#elif defined( __USE_RW )

    // System-independent exception handling

    #if defined( __USE_FS )
        #define FS_REGISTRATION
        #define FS_REGISTRATION_RW
    #endif

    #define SYSIND_REGISTRATION

    #define EXC_TYPE_UNWIND_EXIT   1
    #define EXC_TYPE_UNWIND_NORMAL 2

    #define EXC_HAND_CONTINUE   1
    #define EXC_HAND_CATCH      2
    #define EXC_HAND_UNWOUND    3

    #define FS_UNWIND_GLOBAL( a, b, c )                             \
                CPPLIB( unwind_global )( a, b, c )

    #define FS_RAISE_EXCEPTION( a )                                 \
                CPPLIB( raise_exception )( a )

    #define EXCREC_FLAGS        0

    #define FS_REGISTRATION_SYSIND

    #define FS_REGISTRATION_CONVENTION

    #define RW_REGISTRATION

    #define ThreadLookup CPPLIB( fs_lookup )



#elif defined( __AXP__ ) && defined( __USE_PD )

    // procedure-descriptor exception handling (alpha)

#ifdef __cplusplus
    extern "C" {
#endif

    struct PData;

    #ifdef __NEED_SYSTEM_HEADER
    #include <windows.h>
    #include <excpt.h>
    // the idiots at MS define these C++ keywords as macros
    #undef try
    #undef catch
    #undef throw

    #define EXC_HAND_CONTINUE   ExceptionContinueSearch
    #define EXC_HAND_CATCH      ExceptionContinueExecution
    #define EXC_HAND_UNWOUND    ExceptionContinueExecution

    void RtlUnwind              // can't find def'n
            ( void *            // - frame ptr
            , void *            // - target PC
            , FsExcRec*         // - exception record
            , void * );         // - return

    void RtlUnwindRfp           // can't find def'n
            ( void *            // - frame ptr
            , void *            // - target PC
            , FsExcRec*         // - exception record
            , void * );         // - return

    void RtlCaptureContext      // can't find def'n
            ( _CONTEXT* );      // - context record

    void* RtlVirtualUnwind      // can't find def'n
            ( void *            // - control pc
            , PData *           // - function entry
            , _CONTEXT *        // - context record
            , unsigned *        // - in-function ptr
            , unsigned *        // - establisher frame
            , unsigned * );     // - context ptrs.

    typedef IMAGE_RUNTIME_FUNCTION_ENTRY ProcDesc;

    ProcDesc* RtlLookupFunctionEntry    // can't find def'n
            ( void * );

    #define FS_UNWIND_GLOBAL( a, b, c )                             \
                RtlUnwind( (CONTEXT*)(a)                            \
                         , (void*)(b)                               \
                         , (EXCEPTION_RECORD*)(c)                   \
                         , 0 )

    #define FS_RAISE_EXCEPTION( a )                                 \
                RaiseException( (a)->code                       \
                              , (a)->flags                      \
                              , (a)->parm_count                 \
                              , (DWORD const *)&((a)->object) )

    #define EXC_TYPE_UNWIND_EXIT   4    // can't find def'n
    #define EXC_TYPE_UNWIND_NORMAL 2    // can't find def'n

    #define GetCtxReg( ctx, reg ) (*(void**)&((ctx)->Int##reg))


    #else

    struct ProcDesc;
    struct _CONTEXT;

    #endif

    struct PData                // Procedure Descriptor
    {   void* entry;            // - entry point
        void* end;              // - end address
        void* exc;              // - exception handler (or NULL)
        void* exc_data;         // - exception data (or NULL)
        void* endpr;            // - end of prologue
    };

    typedef unsigned RISC_INS;              // risc instruction
    #define RISC_INS_SIZE sizeof(RISC_INS)  // size of risc instruction
    #define RISC_MOV_SP_FP 0x47FE040F       // mov sp,fp
    #define RISC_REG_SIZE 8                 // size of risc register

    struct PD_DISP_CTX          // Dispatcher context
    {   void *pc;               // - program ctr.
        PData* pdata;           // - PDATA ptr
        void* fp_entry;         // - fp, on routine entry
        _CONTEXT* ctx;          // - context registers
        void* sp_actual;        // - sp, actual
        void* fp_actual;        // - fp, actual
        void* sp_entry;         // - ?guess?
        void* fp_alternate;     // - ?guess?
    };

    #define ThreadLookup CPPLIB( pd_lookup )



#ifdef __cplusplus
    };
#endif

    #define EXCREC_FLAGS        0

    #define EXCREC_CODE_NO_HANDLER 0xC0000028

    #define PD_REGISTRATION
    #define PD_REGISTRATION_RW

    #define FS_REGISTRATION_CONVENTION

#else

    #error Invalid Library Target

#endif


// System-independent definitions

#ifndef __cplusplus
    typedef struct FsCtxRec FsCtxRec;
    typedef struct FsEstFrame FsEstFrame;
    typedef struct FsExcRec FsExcRec;
#endif

struct FsExcRec {               // Exception record
    uint_32 code;               // - exception code
    uint_32 flags;              // - exception flags
    FsExcRec *rec;              // - stacked exception record
    uint_32 addr;               // - exception address
    uint_32 parm_count;         // - # parameters
    void* object;               // - thrown object
    DISPATCH_EXC* dispatch;     // - dispatching control
};

#define EXC_TYPE_UNWINDING  \
            ( EXC_TYPE_UNWIND_NORMAL | EXC_TYPE_UNWIND_EXIT )

#define EXCREC_PARM_COUNT ( sizeof( void* ) * 2 / sizeof( unsigned ) )
#define EXCREC_CODE_WATCOM 0x25671234
#define EXCREC_CODE_SETJMP 0x25670123
#define EXCREC_CODE ( 0xC0000000 | EXCREC_CODE_WATCOM )
#define EXCREC_SJCD ( 0xC0000000 | EXCREC_CODE_SETJMP )
#define EXCREC_CODE_MASK 0x3fffffff

struct FsCtxRec {               // Context record
    uint_32 not_used;
};

typedef unsigned FS_REGISTRATION_CONVENTION FS_HANDLER
                           ( FsExcRec*
                           , RW_DTREG*
                           , FsCtxRec*
                           , unsigned );

#ifdef __cplusplus
extern "C" {
#endif

_WPRTLINK
unsigned FS_REGISTRATION_CONVENTION CPPLIB( fs_handler )   // HANDLER FOR FS REGISTRATIONS
    ( FsExcRec* rec_exc         // - exception record
    , RW_DTREG* rw              // - current R/W block
    , FsCtxRec* rec_ctx         // - context record
    , unsigned context          // - dispatch context
    )
;

#ifdef __cplusplus
}
#endif

#if defined( FS_REGISTRATION )  // FS definitions

    extern RW_DTREG* FsLink( RW_DTREG* blk );   // link into fs stack
    extern void  FsPop();                       // pop fs stack
    extern RW_DTREG* FsPush( RW_DTREG* blk );   // push on fs stack
    extern RW_DTREG* FsTop();                   // top of fs stack

    #pragma aux FsLink      \
        = "mov  fs:0,eax"   \
        , parm [eax]

    #pragma aux FsPop       \
        = "mov eax,fs:0"    \
        , "mov eax,[eax]"   \
        , "mov fs:0,eax"    \
        , modify [eax]

    #pragma aux FsPush      \
        = "push fs:0"       \
        , "pop  [eax]"      \
        , "mov  fs:0,eax"   \
        , parm [eax]

    #pragma aux FsTop       \
        = "mov eax,fs:0"    \
        , modify [eax]

#else

    #define FsTop()         ((PgmThread())->registered)

#endif



#endif

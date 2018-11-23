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


#ifndef __CONVCTL_H__
#define __CONVCTL_H__

// CONVERSION CONTROL INFORMATION

#include "typerank.h"
#include "cgfront.h"
#include "fnovload.h"

#define SrcTgtKind( a, b ) ( ( a << 3 ) + b )

typedef struct convctl  CONVCTL;
typedef struct convtype CONVTYPE;

#define DfnCRUFFs               \
 DfnCRUFF( CRUFF_NO_CL )        \
,DfnCRUFF( CRUFF_SC_TO_CL )     \
,DfnCRUFF( CRUFF_CL_TO_CL )     \
,DfnCRUFF( CRUFF_CL_TO_SC )

#define DfnCRUFF(a) a
typedef enum                    // CONVRUFF -- rough conversion classification
{ DfnCRUFFs
} CRUFF;
#undef DfnCRUFF

struct convtype                         // CONVTYPE -- information about a type
{
    TYPE        orig;                   // - original type
    TYPE        unmod;                  // - unmodified type
    TYPE        pted;                   // - type pointed at, unmodified
    TYPE        class_type;             // - class type when class_operand==true
    type_flag   modflags;               // - modifier flags (orig)
    type_flag   ptedflags;              // - modifier flags (pted)
    RKD         kind;                   // - kind of element
    PC_PTR      pc_ptr;                 // - classification of PC ptr type
    uint_8      reference       : 1;    // - true ==> is reference
    uint_8      array           : 1;    // - true ==> is array
    uint_8      bit_field       : 1;    // - true ==> is bit_field
    uint_8      class_operand   : 1;    // - true ==> is class or ref-class operand
    unsigned                    : 0;    // alignment
};

#define CONVCTL_FLAGS \
CONVCTL_FLAG( has_err_operand )  /* - true ==> at least one operand is error        */ \
CONVCTL_FLAG( converts )         /* - true ==> converts trivially (in-place)        */ \
CONVCTL_FLAG( to_base )          /* - true ==> converts to base                     */ \
CONVCTL_FLAG( to_derived )       /* - true ==> converts to derived                  */ \
CONVCTL_FLAG( to_ambiguous )     /* - true ==> to/from ambiguous class              */ \
CONVCTL_FLAG( to_private )       /* - true ==> to private base                      */ \
CONVCTL_FLAG( to_protected )     /* - true ==> to protected base                    */ \
CONVCTL_FLAG( to_void )          /* - true ==> ptr converts to void*                */ \
CONVCTL_FLAG( from_void )        /* - true ==> ptr converts from void*              */ \
CONVCTL_FLAG( ptr_integral_ext ) /* - true ==> ptr to integral extension            */ \
CONVCTL_FLAG( cv_err_0 )         /* - true ==> CV error, level 0                    */ \
CONVCTL_FLAG( const_cast_ok )    /* - true ==> const_cast ok                        */ \
CONVCTL_FLAG( static_cast_ok )   /* - true ==> static_cast ok                       */ \
CONVCTL_FLAG( reint_cast_ok )    /* - true ==> reinterpret_cast ok                  */ \
CONVCTL_FLAG( dynamic_cast_ok )  /* - true ==> dynamic_cast ok                      */ \
CONVCTL_FLAG( implicit_cast_ok ) /* - true ==> implicit_cast ok                     */ \
CONVCTL_FLAG( explicit_cast_ok ) /* - true ==> explicit_cast ok                     */ \
CONVCTL_FLAG( keep_cast )        /* - true ==> keep implicit cast node              */ \
CONVCTL_FLAG( used_destination ) /* - true ==> initialized the destination          */ \
CONVCTL_FLAG( dtor_destination ) /* - true ==> dtor destination if req'd            */ \
/* used for class->class conversions                                                */ \
CONVCTL_FLAG( clscls_derived )   /* - true ==> allows base to derived               */ \
CONVCTL_FLAG( clscls_cv )        /* - true ==> check CV                             */ \
CONVCTL_FLAG( clscls_implicit )  /* - true ==> implicit cast underway               */ \
CONVCTL_FLAG( clscls_static )    /* - true ==> static cast underway                 */ \
CONVCTL_FLAG( clscls_explicit )  /* - true ==> explicit cast underway               */ \
CONVCTL_FLAG( clscls_refundef )  /* - true ==> references not reqd to be def'd      */ \
CONVCTL_FLAG( clscls_copy_init ) /* - true ==> use ctor's + udcf's to convert       */ \
/* used for diagnosis                                                               */ \
CONVCTL_FLAG( cv_mismatch )      /* - true ==> const/vol casted away                */ \
CONVCTL_FLAG( bad_mptr_class )   /* - true ==> level(0) memb-ptr class not derived  */ \
CONVCTL_FLAG( diff_mptr_class )  /* - true ==> level(0) memb-ptr class differs      */ \
CONVCTL_FLAG( size_ptr_to_int )  /* - true ==> ptr -> integral won't fit            */ \
CONVCTL_FLAG( used_cv_convert )  /* - true ==> cv-promotion occurred                */ \
CONVCTL_FLAG( diag_ext_conv )    /* - true ==> extended conversion diag'ed          */ \
CONVCTL_FLAG( diag_bind_ncref )  /* - true ==> diag'ed non-const ref binding        */


struct convctl                  // CONVCTL -- conversion control information
{
    CONVTYPE    src;            // - source data type
    CONVTYPE    tgt;            // - target data type
    PTREE       expr;           // - expression being converted
    CNV_DIAG    * diag_good;    // - error-message list: conversion must pass
    CNV_DIAG    * diag_cast;    // - error-message list: for cast
    SYMBOL      conv_fun;       // - CTOR or UDF conversion function
    TYPE        conv_type;      // - conversion type, when UDF
    MSG_NUM     msg_no;         // - message # when error
    FNOV_DIAG   fnov_diag;      // - diagnosis list
    PTREE       destination;    // - node for destination
    CNV_REQD    reqd_cnv;       // - conversion request
    CRUFF       rough;          // - rough conversion classification
    CTD         ctd;            // - common-type derivation
    type_flag   mismatch;       // - what was removed on cv mismatch
    #define CONVCTL_FLAG( name ) uint_8 name : 1;
    CONVCTL_FLAGS               // - define flags
    #undef CONVCTL_FLAG
    unsigned            : 0;    // alignment
};



// PROTOTYPES:

CNV_RETN CastPtrToPtr           // EXPLICIT CAST PTR -> PTR
    ( CONVCTL* ctl )            // - conversion control
;
bool ConvCtlAnalysePoints       // ANALYSE CONVERSION INFORMATION FOR POINTS
    ( CONVCTL* info )           // - pointer-conversion information
;
void ConvCtlClassAnalysis       // ANALYSE CLASS TYPE
    ( CONVTYPE* ctype )         // - control info.
;
void ConvCtlInit                // INITIALIZE CONVCTL
    ( CONVCTL* ctl              // - control info.
    , PTREE expr                // - expression
    , CNV_REQD reqd_cnv         // - type of conversion
    , CNV_DIAG* diag )          // - diagnosis
;
void ConvCtlInitCast            // INITIALIZE CONVCTL FOR CAST EXPRESSION
    ( CONVCTL* ctl              // - control info.
    , PTREE expr                // - expression
    , CNV_DIAG* diag )          // - diagnosis
;
void ConvCtlInitTypes           // INITIALIZE CONVCTL, TYPES
    ( CONVCTL* ctl              // - control info.
    , TYPE src_type             // - source type
    , TYPE tgt_type )           // - target type
;
void ConvCtlTypeDecay           // TYPE DECAY CONVCTL TYPE
    ( CONVCTL* ctl              // - control info.
    , CONVTYPE *ctype )         // - type to decay
;
PTREE ConvCtlDiagnose           // DIAGNOSE CASTING ERROR
    ( CONVCTL* ctl )            // - conversion control
;
PTREE ConvCtlDiagnoseTypes      // DIAGNOSE CASTING TYPES
    ( CONVCTL* ctl )            // - conversion control
;
bool ConvCtlTypeInit            // INITIALIZE CONVTYPE
    ( CONVCTL* ctl              // - conversion control
    , CONVTYPE* ctype           // - control info.
    , TYPE type )               // - type
;
bool ConvCtlWarning             // ISSUE WARNING
    ( CONVCTL* ctl              // - conversion control
    , MSG_NUM msg_no )          // - message number
;
PTREE DynamicCast               // DYNAMIC CAST CODE
    ( CONVCTL* ctl )            // - conversion information
;
CNV_RETN PcPtrValidate(         // VALIDATE PC-FORMAT PTRS
    TYPE typ_tgt,               // - target type
    TYPE typ_src,               // - source type
    PC_PTR pcp_tgt,             // - target classification
    PC_PTR pcp_src,             // - source classification
    PTREE expr,                 // - expression for error
    CNV_REQD reqd_cnv )         // - type of conversion
;
#if 0
unsigned OldConvertExprDiagResult( // CONVERT AND DIAGNOSE RESULT
    PTREE *expr_addr,           // - addr( expression to be converted )
    TYPE proto,                 // - prototype
    unsigned conversion,        // - type of conversion
    CNV_DIAG *diagnosis )       // - diagnosis information
;
#endif
#ifndef NDEBUG
void DbgConvType                // DUMP CONVTYPE INFORMATION
    ( CONVTYPE* type )          // - structure
;

void DbgConvCtl                 // DUMP CONVCTL INFORMATION
    ( CONVCTL* info )           // - structure
;
#endif

#endif

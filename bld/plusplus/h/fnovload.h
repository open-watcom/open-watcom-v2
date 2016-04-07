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


#ifndef __FNOVLOAD_H__
#define __FNOVLOAD_H__
#define TypesSameFnov( t1, t2 ) \
    TypesSameExclude( t1, t2, TC1_NOT_ENUM_CHAR | TC1_FUN_LINKAGE )

typedef enum                    // overload resolution results
{
#define __DEFINE_FNOVRESULT
#include "fnovrslt.h"
        FNOV_INVALID
} FNOV_RESULT;

typedef enum                    // overload coarse ranks
{
#define __DEFINE_FNOVRANK
#include "fnovrank.h"
        OV_RANK_INVALID
} FNOV_COARSE_RANK;

typedef enum
{   FNC_EXCLUDE_ELLIPSIS    = 0x0001,       // don't consider ellipsis
    FNC_EXCLUDE_UDCONV      = 0x0002,       // don't consider u-d conversions
    FNC_RANK_RETURN         = 0x0004,       // rank by return value
    FNC_DISTINCT_CHECK      = 0x0008,       // distinct function checking
    FNC_MEMBER              = 0x0010,       // member function ranking
    FNC_EXCLUDE_DEFARG      = 0x0020,       // don't consider fns with def args
    FNC_ONLY_SYM            = 0x0040,       // use only sym, not all of ring
    FNC_TEMPLATE            = 0x0080,       // ring contains function template
    FNC_STDOPS              = 0x0100,       // check for & on first arg
    FNC_EXCLUDE_CONV        = 0x0200,       // no conversions allowed
    FNC_RANKING_CTORS       = 0x0400,       // the functions being overloaded are ctors
    FNC_8_5_3_ANSI          = 0x0800,
    FNC_STDOP_CV_VOID       = 0x1000,       // check cv-qual adjustments on stdop void
    FNC_NO_DEALIAS          = 0x2000,       // don't dealias syms before overload
    FNC_USE_WP13332         = 0x4000,       // abide by wp13332 rules
    FNC_ONLY_TEMPLATE       = 0x8000,       // only template functions
    FNC_ONLY_NON_TEMPLATE   = 0x10000,      // only non-template functions
    FNC_DEFAULT             = 0x0000        // default behaviour
} FNOV_CONTROL;

typedef enum
{
    FNOV_UDC_CTOR           = 0x01,             // consider ctors
    FNOV_UDC_UDCF           = 0x02,             // consider udcfs
    FNOV_UDC_USE_EXPLICIT   = 0x04,             // use explicit ctors
    FNOV_UDC_DIRECT         = FNOV_UDC_CTOR | FNOV_UDC_USE_EXPLICIT,
                                                // do direct initialization
    FNOV_UDC_COPY           = FNOV_UDC_CTOR | FNOV_UDC_UDCF,
} FNOV_UDC_CONTROL;

#define IsCopy( c )   ( ( c & FNOV_UDC_COPY ) == FNOV_UDC_COPY )

typedef enum
{
    FNOV_INTRNL_NONE            = 0x00,         // no internal control bits
    FNOV_INTRNL_ONCE_ONLY       = 0x01,         // only use UDC once
    FNOV_INTRNL_DERIVED         = 0x02,         // src is derived from base
    FNOV_INTRNL_EXCLUDE_UDCONV_PARAM    = 0x04, // don't allow another udc on
                                                // param (extension case)
    FNOV_INTRNL_8_5_3_ANSI      = 0x08,         // do 8.5.3 ansi
    FNOV_INTRNL_STDOP_CV_VOID   = 0x10          // check cv-qual adjustments on stdop void
} FNOV_INTRNL_CONTROL; // for internal overloading use only

typedef struct {
    unsigned                udcnv;          // # udcnv
    unsigned                standard;       // # standard conversions
    unsigned                trivial : 1;    // trivial conversion
    unsigned                promotion : 1;  // promotion
    unsigned                not_exact : 1;  // not an exact match
} FNOV_SCALAR;

typedef struct {
    union {
        FNOV_SCALAR         no_ud;          // no user-defined conversion
        struct {
            FNOV_SCALAR     in;             // on input to u-d conversion
            FNOV_SCALAR     out;            // on output from u-d conversion
        } ud;                               // user-defined conversion
    } u;
    FNOV_COARSE_RANK        rank;           // coarse rank
    FNOV_CONTROL            control;        // how to perform ranking
    unsigned                userdef : 1;    // use of user-defined conversion
} FNOV_RANK;

#ifndef FNOV_LIST_DEFINED
#define FNOV_LIST_DEFINED
typedef struct func_list FNOV_LIST;
#endif
struct func_list {
    FNOV_LIST               *next;          // next entry
    SYMBOL                  sym;            // associated symbol
    FNOV_RANK               *rankvector;    // vector of ranks
    arg_list                *alist;         // arguments
    unsigned                num_args;       // number of arguments
    type_flag               flags;          // flags for function
    FNOV_RANK               thisrank;       // rank for this pointer
    unsigned                free_args : 1;  // flag for mock arg list
    unsigned                member : 1;     // flag for member function
    unsigned                stdops : 1;     // flag for std operator
};

typedef struct {
    FNOV_LIST   *diag_ambig;    // list of ambiguous functions
    FNOV_LIST   *diag_reject;   // list of rejected functions
    int         num_candidates; // number of candidates found
} FNOV_DIAG;

void FnovArgRank(               // RANK AN ARGUMENT LIST
    TYPE src,                   // - source type
    TYPE tgt,                   // - target type
    PTREE *pt,                  // - addr[ argument ]
    FNOV_RANK *rank )           // - ranking
;
bool FnovCvFlagsRank(           // RANK USING CV FLAGS
    type_flag src,              // - source type
    type_flag tgt,              // - target type
    FNOV_RANK *rank )           // - ranking
;
void FnovDiagListsInit(         // INITIALIZE DIAGNOSIS LISTS
    void )
;
void SetFnovDiagnosticAmbigList( // SET DIAGNOSTIC LIST
    FNOV_DIAG *fnov_diag,        // diagnosis information
    FNOV_LIST **ambig )          // - list of ambiguous entries
;
SYMBOL FnovGetAmbiguousEntry(   // TRAVERSE LIST OF AMBIGUOUS SYMBOLS
    FNOV_DIAG *fnov_diag,       // - diagnosis information
    FNOV_LIST **ptr )           // - list is not destroyed as it is traversed
                                //   on first call *ptr == NULL
                                //   on subsequent calls pass back same ptr
;
void FnovListFree(              // FREE AN OVERLOAD LIST
    FNOV_LIST **plist )         // - the list
;
void FnovMemFlagsRank(          // RANK USING MEMORY FLAGS
    type_flag src,              // - source type
    type_flag tgt,              // - target type
    void *srcbase,              // - source base
    void *tgtbase,              // - target base
    FNOV_RANK *rank )           // - ranking
;
SYMBOL FnovNextAmbiguousEntry(  // TRAVERSE LIST OF AMBIGUOUS SYMBOLS
    FNOV_DIAG *fnov_diag )      // - list is destroyed as it is traversed
;
SYMBOL FnovNextRejectEntry(     // TRAVERSE LIST OF REJECTED SYMBOLS
    FNOV_DIAG *fnov_diag )      // - list is destroyed as it is traversed
;
void FnovNumCandidatesSet(      // SET # CANDIDATES FROM LIST
    FNOV_LIST *candidates )     // - the candidates list
;
int FnovRejectParm(             // GIVE PARM # OF FIRST REJECTED SYMBOL
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;

FNOV_DIAG *FnovInitDiag(        // INITIALIZE DIAGNOSIS LIST POINTERS TO NULL
    FNOV_DIAG *                 // - pointer to (allocated) struct to init
);
void FnovFreeDiag(              // FREE MEMORY IN DIAGNOSIS LISTS
    FNOV_DIAG *                 // - pointer to struct containing list pointers
);

#ifndef NDEBUG
void PrintFnovList( FNOV_LIST *root );
void PrintFnovResolution(       // PRETTY DISPLAY OF OVERLOAD RESOLUTION
    FNOV_RESULT result,         // - resolution result
    arg_list *alist,            // - args compared
    FNOV_LIST *match,           // - list of match functions
    FNOV_LIST *reject,          // - list of reject functions
    SYMBOL sym )                // - symbol being overloaded
;
#endif
FNOV_RESULT FuncOverloadedLimitDiag(// FIND OVERLOADED FUNCTION FOR ARGUMENTS
    SYMBOL *resolved,           // - resolved symbol (filled in)
    SEARCH_RESULT *result,      // - search result
    SYMBOL sym,                 // - starting symbol
    arg_list *alist,            // - arguments
    PTREE *ptlist,              // - parse tree nodes for each argument
    FNOV_CONTROL control,       // - bits to regulate overloading
    PTREE templ_args,           // - explicit template arguments
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
FNOV_RESULT FuncOverloaded(     // FIND OVERLOADED FUNCTION FOR ARGUMENTS
    SYMBOL *resolved,           // - resolved symbol (filled in)
    SEARCH_RESULT *result,      // - search result
    SYMBOL sym,                 // - starting symbol
    arg_list *alist,            // - arguments
    PTREE *ptlist )             // - parse tree nodes for each argument
;
FNOV_RESULT FuncOverloadedDiag( // FIND OVERLOADED FUNCTION FOR ARGUMENTS
    SYMBOL *resolved,           // - resolved symbol (filled in)
    SEARCH_RESULT *result,      // - search result
    SYMBOL sym,                 // - starting symbol
    arg_list *alist,            // - arguments
    PTREE *ptlist,              // - parse tree nodes for each argument
    PTREE templ_args,           // - explicit template arguments
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
FNOV_COARSE_RANK UdcLocate(     // SELECT A DIRECT OR COPY INITIALIZTION
                                // FROM SRC TO TGT TYPE
    FNOV_UDC_CONTROL control,   // - indicates type of udcs to consider
    TYPE src,                   // - source class type
    TYPE tgt,                   // - target class type
    PTREE *src_ptree,           // - source PTREE node
    bool *isctor,               // - set to TRUE if ctor selected
                                //   FALSE if UDCF selected
    FNOV_LIST **match,          // - will point to selected UDC if
                                //   non-ambig match
    FNOV_DIAG *fnov_diag        // - ambig and reject lists
);
FNOV_COARSE_RANK UdcLocateRef(  // SELECT A DIRECT OR COPY INITIALIZTION
                                // FROM SRC TO TGT TYPE
                                // HANDLE CASE OF TGT BEING A REF
    FNOV_UDC_CONTROL control,   // - indicates type of udcs to consider
    TYPE src,                   // - source class type
    TYPE tgt,                   // - target class type
    PTREE *src_ptree,           // - source PTREE node
    bool *isctor,               // - set to TRUE if ctor selected
                                //   FALSE if UDCF selected
    FNOV_LIST **match,          // - will point to selected UDC if
                                //   non-ambig match
    FNOV_DIAG *fnov_diag        // - ambig and reject lists
);
FNOV_RESULT UdcOverloaded(      // FIND OVERLOADED U-D CONVERSION TO TYPE
    SYMBOL *resolved,           // - resolved symbol
    SEARCH_RESULT *result,      // - search result
    SYMBOL sym,                 // - starting symbol
    TYPE type,                  // - target type
    type_flag this_qualifier )  // - T cv-qual *this; (cv-qual flags)
;
FNOV_RESULT UdcOverloadedDiag(      // FIND OVERLOADED U-D CONVERSION TO TYPE
    SYMBOL *resolved,           // - resolved symbol
    SEARCH_RESULT *result,      // - search result
    SYMBOL sym,                 // - starting symbol
    TYPE type,                  // - target type
    type_flag this_qualifier,   // - T cv-qual *this; (cv-qual flags)
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
FNOV_RESULT OpOverloadedDiag(       // SELECT OVERLOADED OPERATOR
    SYMBOL *resolved,           // - resolved symbol or NULL
    SEARCH_RESULT *member,      // - member operators
    SEARCH_RESULT *nonmember,   // - non member operators
    SEARCH_RESULT *nonmember_namespace,   // - non member operators
    SYMBOL stdops,              // - standard arithmetic operators
    arg_list *alist,            // - operands provided
    PTREE *ptlist,              // - parse tree nodes for operands
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
FNOV_RESULT OpOverloadedLimitDiag(       // SELECT OVERLOADED OPERATOR
    SYMBOL *resolved,           // - resolved symbol or NULL
    SEARCH_RESULT *member,      // - member operators
    SEARCH_RESULT *nonmember,   // - non member operators
    SEARCH_RESULT *nonmember_namespace,   // - non member operators
    SYMBOL stdops,              // - standard arithmetic operators
    arg_list *alist,            // - operands provided
    PTREE *ptlist,              // - parse tree nodes for operands
    FNOV_CONTROL control,
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
bool IsOverloadedFunc(          // TEST IF A FUNCTION HAS BEEN OVERLOADED
    SYMBOL sym )                // - symbol
;
bool IsActualOverloadedFunc(    // TEST IF ACTUAL (IGNORE SC_DEFAULT) OVERLOAD
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - search result
;
SYMBOL ActualNonOverloadedFunc( // GET SYMBOL FOR ACTUAL NON-OVERLOADED FUNC.
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - search result
;
FNOV_RESULT IsOverloadedFuncDistinct(// TEST IF AN OVERLOADED FUNCTION IS DISTINCT
    SYMBOL *pold_sym,           // - existing symbols (returns best SYMBOL)
    SYMBOL new_sym,             // - new symbol
    NAME name,                  // - name of new symbol
    FNOV_CONTROL control )      // - control word
;
FNOV_RESULT AreFunctionsDistinct(// - TEST if TWO FUNCTIONS ARE DISTINCT
    SYMBOL *pold_sym,           // - existing symbols (returns best SYMBOL)
    SYMBOL new_sym,             // - new symbol
    NAME name )                 // - name of new symbol
;
void BuildUdcList(              // BUILD FNOV_LIST FOR USER-DEFD CONVERSIONS
    FNOV_LIST **pcandidates,    // - pointer to candidate list
    SYMBOL sym )                // - symbol to add
;
void BuildCtorList(             // BUILD FNOV_LIST FOR CTOR CONVERSIONS
    FNOV_LIST **pcandidates,    // - pointer to candidate list
    SYMBOL sym,                 // - symbol to add
    arg_list *alist )           // - argument list
;
FNOV_RESULT ResolveUdcList(     // SELECT BEST USER-DEFD CONVERSION TO 'type'
    FNOV_LIST *candidates,      // - candidate list
    type_flag this_qualifier,   // - T cv-qual *this; (cv-qual flags)
    TYPE type,                  // - destination type
    SYMBOL *sym )               // - best user-defd conversion
;
FNOV_COARSE_RANK RankandResolveUDCsDiag( // RANK CTOR an UDCF LISTS
                                         // RESOLVE AS IF ONE MERGED LIST
    FNOV_LIST **ctorList,       // - list of CTORs for class tgt
    FNOV_LIST **udcfList,       // - list of udcfs from class src
    TYPE src,                   // - src class of UDC
    TYPE tgt,                   // - tgt class of UDC
    bool *isctor,               // - TRUE if ctor in match list
    FNOV_LIST **pmatch,         // - will point to selected UDC if
                                //   non-ambig match
    FNOV_UDC_CONTROL control,   // - control copy or direct init
    FNOV_INTRNL_CONTROL ictl,   // - internal control of process
    PTREE *src_ptree,           // - source PTREE node
    FNOV_RANK *rank,
    FNOV_DIAG *fnov_diag        // - diagnosis information
);
TYPE *CompareWP13332(           // IMPLEMENT WP13332 (RETURNS BETTER TYPE)
    TYPE *first_type            // - first type
  , TYPE *second_type           // - seond type
);
#endif

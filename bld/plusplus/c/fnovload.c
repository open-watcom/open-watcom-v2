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


#include <string.h>
#include <assert.h>

#include "plusplus.h"
#include "errdefns.h"
#include "fnovload.h"
#include "cgfront.h"
#include "memmgr.h"
#include "carve.h"
#include "vbuf.h"
#include "ring.h"
#include "fmtsym.h"
#include "toggle.h"
#include "template.h"
#include "initdefs.h"
#include "dbg.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif


//#define NEW_TRIVIAL_RULE

typedef enum                    // comparison and resolution results
{   OV_CMP_SAME,                // no difference
    OV_CMP_BETTER_FIRST,        // first argument is better
    OV_CMP_BETTER_SECOND,       // second argument is better
    OV_CMP_UNDEFINED            // result is undefined
} OV_RESULT;

typedef struct                  // FNOV_INFO -- overload information
{
    arg_list* alist;            // - argument definition
    PTREE* plist;               // - arguments used
    FNOV_LIST** pcandidates;    // - hdr: candidates
    FNOV_LIST** pmatch;         // - hdr: matches
    FNOV_LIST* candfunc;        // - candidate function
    FNOV_CONTROL control;       // - control item
    FNOV_RESULT result;         // - result for candidates
    FNOV_DIAG *fnov_diag;       // - information used for diagnosis
    uint_8 has_template :1;     // - TRUE ==> has template candidate
} FNOV_INFO;

#define CACHE_FNOVRANK_SIZE     16

// carve storage for fnov list structures
#define BLOCK_FNOVLIST      16
static carve_t carveFNOVLIST;
#define BLOCK_ManyFNOVRANK  16
static carve_t carveManyFNOVRANK;

// static storage for diagnostic lists
#define LIST_FREE       -1


//--------------------------------------------------------------------
// Manage # of candidates
//--------------------------------------------------------------------

static void fnovNumCandidatesSet( FNOV_DIAG *fnov_diag, FNOV_LIST *candidates )
{
    if( fnov_diag != NULL ) {
        if( fnov_diag->num_candidates == LIST_FREE ) {
            fnov_diag->num_candidates = RingCount( candidates );
        }
    }
}


//--------------------------------------------------------------------
// FNOV_LIST Support
//--------------------------------------------------------------------

void FnovListFree( FNOV_LIST **plist )
/************************************/
// free memory used by list of overloaded functions
{
    FNOV_LIST *ptr;

    RingIterBeg( *plist, ptr ) {
        if( ptr->rankvector ) {
            deleteRank( ptr->rankvector, ptr->num_args );
        }
        if( ptr->free_args ) {
            CMemFree( ptr->alist );
        }
    } RingIterEnd( ptr )
    RingCarveFree( carveFNOVLIST, plist );
}

typedef enum                // LENT -- flags for addListEntry
{   LENT_DEFAULT    =0x00   // - default: add to candidates
,   LENT_MATCH      =0x01   // - add to matches
,   LENT_TPL_CHECK  =0x02   // - check for template candidate
,   LENT_FREE_ARGS  =0x04   // - mark args to be freed
} LENT;

static void addListEntry( FNOV_CONTROL control, FNOV_INFO *info, SYMBOL sym,
/**************************************************************************/
    arg_list *alist, LENT flags )
// add an entry to list of overloaded functions
{
    FNOV_LIST    *new;
    FNOV_LIST    **hdr;     // - header for list addition

    if( flags & LENT_MATCH ) {
        hdr = info->pmatch;
    } else {
        hdr = info->pcandidates;
    }
    // allocate and initialize the storage
    new = RingCarveAlloc( carveFNOVLIST, hdr );
    new->sym  = sym;
    new->alist = alist;
    if( control & FNC_RANK_RETURN ) {
        new->num_args = 1;
    } else {
        new->num_args = alist->num_args;
    }
    if( flags & LENT_FREE_ARGS ) {
        new->free_args = TRUE;
    } else {
        new->free_args = FALSE;
    }
    new->member = ((control & FNC_MEMBER) != 0 );
    new->stdops = ((control & FNC_STDOPS) != 0 );
    new->rankvector = NULL;
    initRankVector( FNC_DEFAULT, &new->thisrank, 1 );
    new->thisrank.rank = OV_RANK_EXACT;
    new->flags = TF1_NULL;
    if( flags & LENT_TPL_CHECK ) {
        if( SymIsFunctionTemplateModel( sym ) ) {
            info->has_template = TRUE;
        }
    }
}


//--------------------------------------------------------------------
// Diagnostic Support
//--------------------------------------------------------------------

void FnovFreeDiag( FNOV_DIAG *fnov_diag )
{
    if( fnov_diag != NULL ) {
        fnov_diag->num_candidates = LIST_FREE;
        FnovListFree( &fnov_diag->diag_ambig );
        fnov_diag->diag_ambig = NULL;
        FnovListFree( &fnov_diag->diag_reject );
        fnov_diag->diag_reject = NULL;
    }
}

static void setFnovDiagnosticAmbigList( FNOV_DIAG *fnov_diag, FNOV_LIST **ambig )
/************************************************************************/
{
    if( fnov_diag == NULL ) {
        FnovListFree( ambig );
    } else {
        if( fnov_diag->diag_ambig != NULL ) {
            FnovListFree( &fnov_diag->diag_ambig );
        }
        fnov_diag->diag_ambig = *ambig;
    }
}

static void setFnovDiagnosticRejectList( FNOV_DIAG *fnov_diag,
                                         FNOV_LIST **reject )
/***********************************************************/
{
    if( fnov_diag == NULL ) {
        FnovListFree( reject );
    } else {
        if( fnov_diag->diag_reject != NULL ) {
            FnovListFree( &fnov_diag->diag_reject );
        }
        fnov_diag->diag_reject = *reject;
    }
}

static SYMBOL getNextDiagnosticEntry( FNOV_LIST **list )
/******************************************************/
{
    SYMBOL      sym = NULL;
    FNOV_LIST   *head;

    head = RingPop( list );
    if( head != NULL ) {
        sym = head->sym;
        head->next = head;
        FnovListFree( &head );
    }
    return( sym );
}

SYMBOL FnovNextAmbiguousEntry( FNOV_DIAG *fnov_diag )
/**********************************************/
{
    return getNextDiagnosticEntry( &fnov_diag->diag_ambig );
}

SYMBOL FnovGetAmbiguousEntry( FNOV_DIAG *fnov_diag, FNOV_LIST **ptr )
/*******************************************************************/
{
    SYMBOL sym;

    sym = NULL;
    if( *ptr == NULL ) {
        if( fnov_diag != NULL && fnov_diag->diag_ambig != NULL ) {
            sym = fnov_diag->diag_ambig->sym;
            *ptr = fnov_diag->diag_ambig->next;
        }
    } else if( ( fnov_diag == NULL) || (*ptr != fnov_diag->diag_ambig) ) {
        sym = (*ptr)->sym;
        *ptr = (*ptr)->next;
    }
    return( sym );
}

SYMBOL FnovNextRejectEntry( FNOV_DIAG *fnov_diag )
/*******************************************/
{
    return getNextDiagnosticEntry( &fnov_diag->diag_reject );
}

int FnovRejectParm( FNOV_DIAG *fnov_diag )
/****************************************/
{
    FNOV_RANK   *rank;
    int         index;
    int         num_args;

    if( ( fnov_diag != NULL ) && ( fnov_diag->diag_reject != NULL ) ) {
        rank = fnov_diag->diag_reject->rankvector;
        num_args = fnov_diag->diag_reject->num_args;
        if( (num_args == 0) || (rank->control & FNC_RANK_RETURN) ) {
            num_args = 1;
        }
        for( index = 0; index < num_args ; index++, rank++ ) {
            if( rank->rank >= OV_RANK_NO_MATCH ) return( index );
        }
        if( fnov_diag->diag_reject->member != 0 ) {
            if( fnov_diag->diag_reject->thisrank.rank >= OV_RANK_NO_MATCH ) return( -1 );
        }
    }
    return( -2 );
}


//--------------------------------------------------------------------
// FNOV_RANK Support
//--------------------------------------------------------------------

static FNOV_RANK *newRank( unsigned n )
{
    FNOV_RANK *rank;

    if( n > CACHE_FNOVRANK_SIZE ) {
        rank = CMemAlloc( sizeof( *rank ) * n );
    } else {
        rank = CarveAlloc( carveManyFNOVRANK );
    }
    return( rank );
}

static void deleteRank( FNOV_RANK *rank, unsigned n )
{
    if( n > CACHE_FNOVRANK_SIZE ) {
        CMemFree( rank );
    } else {
        CarveFree( carveManyFNOVRANK, rank );
    }
}

static void initRankVector( FNOV_CONTROL control, FNOV_RANK *rv, int num_args )
/***************************************************************************/
// initialize vector of rank information
{
    int        i;
    FNOV_RANK *rank;

    if( num_args == 0 ) num_args = 1;
    rank = rv;
    memset( rank, 0, sizeof( *rank ) * num_args );
    for( i = num_args ; i > 0 ; i--, rank++ ) {
        rank->rank = OV_RANK_INVALID;
        rank->control = control;
    }
    // for member functions, don't check u-d conversion on this ptr
    if( control & FNC_MEMBER ) {
        rv->control |= FNC_EXCLUDE_UDCONV;
    }
    // for standard operators which are member functions,
    // don't check any conversion on first parm
    if( ( control & FNC_STDOPS) && ( control & FNC_MEMBER ) ) {
        rv->control |= FNC_EXCLUDE_CONV;
    }
}

static void addRankVector( FNOV_LIST *candidate, FNOV_CONTROL control )
/*********************************************************************/
// allocate and initialize vector of rank information
{
    FNOV_RANK *rank;
    unsigned num_args;

    num_args = candidate->num_args;
    if( num_args == 0 ) {
        num_args = 1;
    }
    rank = newRank( num_args );
    initRankVector( control, rank, num_args );
    candidate->rankvector = rank;
}

static boolean hasOneArg( arg_list *arg )
{
    if( arg->num_args > 0  &&
       (arg->type_list[arg->num_args-1])->id  == TYP_DOT_DOT_DOT ) {
        return (arg->num_args == 2 || arg->num_args == 1);
        // ellipsis args take up space in array, but we want to match
        // fns with one real arg plus ellipsis args as having one arg
        // also, fns with just ellipsis args can match as having one arg
    }
    return (arg->num_args == 1);
}

static void udcRankCtor( FNOV_LIST *list, TYPE src, TYPE tgt,
/**********************************************************/
FNOV_UDC_CONTROL control, FNOV_INTRNL_CONTROL ictl, PTREE *src_ptree )
// rank ctors in list, going from src to tgt.
{
    FNOV_LIST   *curr;          // current element in list
    arg_list    *arg;           // arg list to ctor
    FNOV_CONTROL fnov_control;
    TYPE        inter;          // intermediate type (src->inter->tgt)
    FNOV_CONTROL curr_control;  // control for current ctor;

    if( ictl & FNOV_INTRNL_ONCE_ONLY ) {
        fnov_control = FNC_EXCLUDE_UDCONV;
    } else {
        if( !IsCopy(control)  ||                // direct or
            ( ictl & FNOV_INTRNL_DERIVED ) ) {  // copy and derived
            fnov_control = FNC_DEFAULT; // allow second UDC
        } else {
            fnov_control = FNC_EXCLUDE_UDCONV;
        }
    }
    RingIterBeg( list, curr ) {
        arg = SymFuncArgList( curr->sym );
        curr_control = fnov_control;
        if( hasOneArg( arg ) &&
            ( ( control & FNOV_UDC_USE_EXPLICIT ) ||
              ( curr->sym->sym_type->flag & TF1_EXPLICIT ) == 0 ) ) {
            inter = arg->type_list[0];
            if( ( ictl & FNOV_INTRNL_EXCLUDE_UDCONV_PARAM ) == 0 &&
                IsCopy(control) && CompFlags.extensions_enabled &&
                ( ictl & FNOV_INTRNL_ONCE_ONLY ) == 0 ) {
                TYPE    tgt_type;
                TYPE    inter_type;
                tgt_type = ClassTypeForType( tgt );
                inter_type = ClassTypeForType(inter);
                if( tgt_type != inter_type ) {
                    // copy case, extensions enabled
                    // tgt and inter not same
                    // allow second UDC, but not another
                    curr_control = FNC_DEFAULT | FNC_RANKING_CTORS;
                }
            }
            if( ictl & FNOV_INTRNL_8_5_3_ANSI ) {
                curr_control |= FNC_8_5_3_ANSI;
            }
            if( ictl & FNOV_INTRNL_STDOP_CV_VOID ) {
                curr_control |= FNC_STDOP_CV_VOID;
            }
            addRankVector( curr, curr_control );
            FnovArgRank( src, inter, src_ptree, curr->rankvector );
        } else {
            addRankVector( curr, curr_control );
            curr->rankvector->rank = OV_RANK_NO_MATCH;
        }
    } RingIterEnd( curr )

}

static FNOV_SCALAR rankCtorReturn(
    FNOV_LIST *match,
    TYPE tgt,
    PTREE *src_ptree )
{
    TYPE ret_type;
    FNOV_RANK rank;

    ret_type = SymFuncReturnType( match->sym );
    // ctor type returned above is class &, but the type
    // of a ctor is defined to be just the class
    ret_type = ClassTypeForType( ret_type );
    initRankVector( FNC_EXCLUDE_UDCONV, &rank, 0 );
    FnovArgRank( ret_type, tgt, src_ptree, &rank );
    return( rank.u.no_ud );
}

static void udcRankUDCF( FNOV_LIST *list, TYPE src, TYPE tgt,
FNOV_INTRNL_CONTROL ictl, PTREE *src_ptree )
/**********************************************************/
// rank UDCFs in list, going from src to tgt.
{
    FNOV_LIST           *curr;          // current element in list
    type_flag           srcflags;
    type_flag           tgtflags;
    void                *unused;
    TYPE                ret_type;
    FNOV_CONTROL        control;

    TypeModExtract( TypeReferenced( src )
                  , &srcflags
                  , &unused
                  , TC1_NOT_ENUM_CHAR|TC1_NOT_MEM_MODEL );
    control = FNC_EXCLUDE_UDCONV;
    if( ictl & FNOV_INTRNL_STDOP_CV_VOID ) {
        control |= FNC_STDOP_CV_VOID;
    }
    RingIterBeg( list, curr ) {
        addRankVector( curr, control );
        ret_type = SymFuncReturnType( curr->sym );
        FnovArgRank( ret_type, tgt, src_ptree, curr->rankvector );
        if( SymIsThisFuncMember( curr->sym ) ) { // rank src->this
            tgtflags = curr->alist->qualifier;
            FnovCvFlagsRank( srcflags, tgtflags, &curr->thisrank );
        }
    } RingIterEnd( curr )
}

//--------------------------------------------------------------------
// General Support
//--------------------------------------------------------------------

static boolean isEllipsisCandidate( TYPE type, int num_args )
/***********************************************************/
// determine if type is a candidate based on:
//      type having more than zero arguments
//      type having fewer arguments than num_args and
//              last argument is ellipsis, or
//      type having exactly one more argument than num_args which is ellipsis
{
    TYPE argtype;
    arg_list *alist;

    type = FunctionDeclarationType( type );
    if( type != NULL ) {
        alist = TypeArgList( type );
        if( alist->num_args != 0 ) {
            if( ( alist->num_args < num_args )
              ||( alist->num_args == num_args+1 ) ) {
                argtype = alist->type_list[alist->num_args-1];
                if( argtype->id == TYP_DOT_DOT_DOT ) {
                    return( TRUE );
                }
            }
        }
    }
    return( FALSE );
}

static boolean isMemberCandidate( TYPE type, int num_args )
/*********************************************************/
// determine if sym is a candidate based on the number of arguments
// including a this pointer
{
    type = FunctionDeclarationType( type );
    if( type != NULL ) {
        if( (TypeArgList( type )->num_args+1) == num_args ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static boolean isSimpleCandidate( TYPE type, int num_args )
/*********************************************************/
// determine if sym is a candidate based on the number of arguments
{
    type = FunctionDeclarationType( type );
    if( type != NULL ) {
        if( TypeArgList( type )->num_args == num_args ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static void processSym( FNOV_CONTROL control, FNOV_INFO* info, SYMBOL sym )
/****************************************************************************/
{
    arg_list    *mock_args;
    arg_list    *func_args;

    if(( control & FNC_NO_DEALIAS ) == 0 ) {
        sym = SymDeAlias( sym );
    }
    func_args = SymFuncArgList( sym );
    if( control & FNC_RANK_RETURN ) {
        addListEntry( control, info, sym, func_args, LENT_DEFAULT );
    } else {
        int num_args = info->alist->num_args;
        TYPE sym_type = sym->sym_type;
        if( control & FNC_MEMBER ) {
            if( isMemberCandidate( sym_type, num_args ) ) {
                mock_args = MakeMemberArgList( sym, num_args );
                addListEntry( control, info, sym, mock_args, LENT_FREE_ARGS );
            }
        } else {
            if( !( SymIsDefArg(sym) && (control & FNC_EXCLUDE_DEFARG ) ) ) {
                if( isSimpleCandidate( sym_type, num_args ) ) {
                    addListEntry( control, info, sym, func_args, LENT_TPL_CHECK );
                } else if( !(control & FNC_EXCLUDE_ELLIPSIS) ) {
                    if( isEllipsisCandidate( sym_type, num_args ) ) {
                        mock_args = MakeMockArgList( sym_type, num_args );
                        addListEntry( control
                                    , info
                                    , sym
                                    , mock_args
                                    , LENT_FREE_ARGS | LENT_TPL_CHECK );
                    }
                }
            }
        }
    }
}

static void buildUdcListDiag(   // BUILD FNOV_LIST FOR USER-DEFD CONVERSIONS
/***************************/
    FNOV_LIST **pcandidates,    // - current list
    SYMBOL sym,                 // - symbol to add
    FNOV_DIAG *fnov_diag )      // - diagnosis information (can be NULL)
{
    FNOV_INFO info;

    info.control = FNC_RANK_RETURN;
    info.plist = NULL;
    info.alist = NULL;
    info.pcandidates = pcandidates;
    info.pmatch = NULL;
    info.has_template = FALSE;
    info.fnov_diag = fnov_diag;
    processSym( FNC_RANK_RETURN, &info, sym );
}

void BuildUdcList(              // BUILD FNOV_LIST FOR USER-DEFD CONVERSIONS
/****************/
    FNOV_LIST **pcandidates,    // - current list
    SYMBOL sym )                // - symbol to add
{
    buildUdcListDiag( pcandidates, sym, NULL );
}

static void buildOverloadListFromSym( FNOV_CONTROL control,
    FNOV_INFO *info, SYMBOL sym )
/*********************************************************/
{
    SYMBOL  ptr;

    if( control & FNC_ONLY_SYM ) {
        processSym( control, info, sym );
    } else {
        RingIterBeg( sym, ptr ) {
            processSym( control, info, ptr );
        } RingIterEnd( ptr )
    }
}

static void buildOverloadListFromRegion( FNOV_CONTROL control,
/*********************************************************/
FNOV_INFO *info, SYM_REGION *region )
{
    SYMBOL      sym;
    SYM_REGION  *ptr;

    DbgAssert( (control & FNC_ONLY_SYM) == 0 );
    RingIterBeg( region, ptr ) {

        RingIterBegFrom( ptr->from, sym ) {
            processSym( control, info, sym );
        } RingIterEndTo( sym, ptr->to )

    } RingIterEnd( ptr )
}

static OV_RESULT compareInt( int first, int second )
/**************************************************/
{
    OV_RESULT   retn;

    if( first < second ) {
        retn = OV_CMP_BETTER_FIRST;
    } else if( first > second ) {
        retn = OV_CMP_BETTER_SECOND;
    } else {
        retn = OV_CMP_SAME;
    }
    return( retn );
}

static boolean myTypesSame( TYPE first_type, TYPE second_type )
{
    void        *refbase;
    type_flag   flag;
    boolean     same;

    while( ( first_type->id == TYP_POINTER || first_type->flag & TF1_REFERENCE )
        && ( second_type->id == TYP_POINTER || second_type->flag & TF1_REFERENCE ) ) {
        first_type = TypeModExtract( first_type->of
                              , &flag
                              , &refbase
                              , TC1_NOT_ENUM_CHAR| TC1_FUN_LINKAGE  );
        second_type = TypeModExtract( second_type->of
                              , &flag
                              , &refbase
                              , TC1_NOT_ENUM_CHAR| TC1_FUN_LINKAGE  );
    }
    same = TypesSameExclude( first_type, second_type,
                             TC1_NOT_ENUM_CHAR| TC1_FUN_LINKAGE  );
    if( !same ) {
        if( first_type->flag & TF1_STDOP || second_type->flag & TF1_STDOP ) {
            same = TRUE;
        }
    }
    return same;
}

// code for WP13332
static OV_RESULT WP13332(
/***********************/
    TYPE * first_type,
    TYPE * second_type,
    OV_RESULT def_retn )
{
    TYPE *better;

    if( ( first_type == NULL ) || ( second_type == NULL ) ) {
        return( def_retn );
    }
    if( ( *first_type == NULL ) || ( *second_type == NULL ) ) {
        return( def_retn );
    }
    if( !myTypesSame( *first_type, *second_type ) ) {
        return( OV_CMP_SAME );
    }
    better = CompareWP13332( first_type, second_type );
    if( better != NULL ) {
        if( better == first_type ) {
            return( OV_CMP_BETTER_FIRST );
        } else {
            return( OV_CMP_BETTER_SECOND );
        }
    }
    return( OV_CMP_SAME );
}

static OV_RESULT compareScalar( FNOV_SCALAR *first, TYPE *first_type,
/**********************************************************************/
 FNOV_SCALAR *second, TYPE *second_type, boolean isUDC,
 FNOV_CONTROL control )
{
    OV_RESULT   retn;

    if( first->udcnv != second->udcnv ) {
        retn = compareInt( first->udcnv, second->udcnv );
    } else if( first->standard != second->standard ) {
        retn = compareInt( first->standard, second->standard );
    } else if( first->promotion != second->promotion ) {
        retn = compareInt( first->promotion, second->promotion );
    } else if( first->trivial != second->trivial &&
               (
#ifndef NEW_TRIVIAL_RULE
TRUE ||
#endif
               !isUDC || first_type == NULL || second_type == NULL ||
               myTypesSame( *first_type, *second_type ) ) ) {
        // all else being equal, can decide on number of added cv-qualifiers
        // if two implicit comversion sequences yield types identical except
        // for their cv-qualification
        // [over.ics.rank]
        retn = compareInt( first->trivial, second->trivial );

        if( CompFlags.overload_13332 && ( control & FNC_USE_WP13332 ) ) {
            // this is for full implementation of WP13332
            retn = WP13332( first_type, second_type, retn );
        }
    } else {
        retn = OV_CMP_SAME;
    }
    return( retn );
}


static OV_RESULT compareDerived(TYPE first_type, TYPE second_type )
/******************************************************************/
/*
  13.3.3.2 paragraph 4
   If class B is derived directly or indirectly from class A and class
   C is derived directly or indirectly from B:
   - converting C* -> B* is better than C* -> A*
   - binding    C  -> B&  is better than C -> A&
   - converting C -> B is better than C -> A
   - converting A::* -> B::* is better than A::* to C::*
   etc.
   basically, it the two target class are related, the one that is closer
   to the source was a better conversion
*/
{
    SCOPE first_scope;
    SCOPE second_scope;

    TYPE base_first = first_type;
    TYPE base_second = second_type;

    if( first_type->id != second_type->id ) {
        // if they're not both references, classes, pointers of member
        // pointers that we can't examine them here
        return( OV_CMP_SAME );
    }
    // check for reference to reference conversion
    if( first_type->id == TYP_POINTER && second_type->id == TYP_POINTER ) {
        // both either pointers or references
        base_first = TypeReference( first_type );
        base_second = TypeReference( second_type );

        if( base_first == NULL && base_second == NULL ) {
            // neither is a reference so both are pointers
            base_first = first_type->of;
            base_second = second_type->of;
        } else {
            // else they are both references
            DbgAssert( base_first != NULL && base_second != NULL );
        }
    } else if( MemberPtrType( first_type) != NULL &&
               MemberPtrType( second_type ) != NULL ) {
        // this is intentional
        // converting A::* -> B::* is better than A::* to C::*
        // so here we're converting from the least derived so the
        // longest distance will be to the most derived, the opposite
        // of what the test below looks for, so just switch them
        base_second = MemberPtrClass( first_type );
        base_first = MemberPtrClass( second_type );
    }

    first_scope = TypeScope( base_first );
    second_scope = TypeScope( base_second );

    if( first_scope == second_scope ) {
        return( OV_CMP_SAME );
    }
    if( ScopeDerived( first_scope, second_scope ) != DERIVED_NO  ) {
        return( OV_CMP_BETTER_FIRST );
    } else if( ScopeDerived( second_scope, first_scope ) != DERIVED_NO ) {
        return( OV_CMP_BETTER_SECOND );
    }
    return( OV_CMP_SAME );
}

#ifdef NEW_TRIVIAL_RULE
static FNOV_COARSE_RANK equateExactSameTrivial( FNOV_COARSE_RANK result )
/****************************************************************/
// make OV_RANK_SAME and OV_RANK_EXACT the same
{
    if( result == OV_RANK_EXACT ) {
        result = OV_RANK_TRIVIAL;
    }
    if( result == OV_RANK_SAME ) {
        result = OV_RANK_TRIVIAL;
    }
    return( result );
}
#else
static FNOV_COARSE_RANK equateExactSame( FNOV_COARSE_RANK result )
/****************************************************************/
// make OV_RANK_SAME and OV_RANK_EXACT the same
{
    if( result == OV_RANK_EXACT ) {
        result = OV_RANK_SAME;
    }
    return( result );
}
#endif

static FNOV_COARSE_RANK equateUDCAmbigUDC( FNOV_COARSE_RANK result )
/****************************************************************/
// make OV_RANK_UD_CONV and OV_RANK_UD_CONV_AMBIG the same
// so a class with two UDC is not made unviable, allowing another
// class with only one UDC to be selected
{
    if( result == OV_RANK_UD_CONV_AMBIG ) {
        result = OV_RANK_UD_CONV;
    }
    return( result );
}

#ifdef NEW_TRIVIAL_RULE
static OV_RESULT compareArgument( FNOV_RANK *first, TYPE * first_type,
/*********************************************************************/
FNOV_RANK *second, TYPE *second_type )
{
    OV_RESULT         retn;
    FNOV_COARSE_RANK  firstrank;
    FNOV_COARSE_RANK  secondrank;

    firstrank = equateExactSameTrivial( first->rank );
    secondrank = equateExactSameTrivial( second->rank );
    firstrank = equateUDCAmbigUDC( firstrank );
    secondrank = equateUDCAmbigUDC( secondrank );

    if( firstrank != secondrank ) {
        retn = compareInt( firstrank, secondrank );
    } else switch( firstrank ) {
    case OV_RANK_NO_MATCH:
    case OV_RANK_ELLIPSIS:
        retn = OV_CMP_SAME;
        break;
    case OV_RANK_UD_CONV:
        retn = compareScalar( &first->u.ud.out
                            , first_type
                            , &second->u.ud.out
                            , second_type, TRUE );
        break;
    case OV_RANK_STD_CONV_DERIV:
        if( ( first_type != NULL ) && ( second_type != NULL ) ) {
            retn = compareDerived( *first_type, *second_type );
            if( retn != OV_CMP_SAME ) {
                break;
            }
        }
        // two target types are not releated
        // do the following comparison
    case OV_RANK_STD_CONV_VOID:
    case OV_RANK_STD_CONV:
    case OV_RANK_PROMOTION:
    case OV_RANK_TRIVIAL:
        retn = compareScalar( &first->u.no_ud
                             , first_type
                             , &second->u.no_ud
                             , second_type, FALSE );
        break;
    case OV_RANK_UD_CONV_AMBIG:
    case OV_RANK_SAME:
    case OV_RANK_EXACT:
        DbgAssert( FALSE ); // made into something else above
        break;
    DbgDefault( "funny rank\n" );
    }
    return( retn );
}
#else
static OV_RESULT compareArgument(
/*******************************/
    FNOV_RANK *first
  , TYPE * first_type
  , FNOV_RANK *second
  , TYPE *second_type
  , FNOV_CONTROL control )
{
    OV_RESULT         retn;
    FNOV_COARSE_RANK  firstrank;
    FNOV_COARSE_RANK  secondrank;

    firstrank = equateExactSame( first->rank );
    secondrank = equateExactSame( second->rank );
    firstrank = equateUDCAmbigUDC( firstrank );
    secondrank = equateUDCAmbigUDC( secondrank );

    if( firstrank != secondrank ) {
        retn = compareInt( firstrank, secondrank );
        if( CompFlags.overload_13332 && ( control & FNC_USE_WP13332 ) ) {
            if( ( ( firstrank == OV_RANK_SAME )
               && ( secondrank == OV_RANK_TRIVIAL ) )
             || ( ( firstrank == OV_RANK_TRIVIAL )
               && ( secondrank == OV_RANK_SAME ) ) ) {
                  retn = WP13332( first_type, second_type, retn );
            }
        }
    } else switch( firstrank ) {
    case OV_RANK_NO_MATCH:
    case OV_RANK_ELLIPSIS:
    case OV_RANK_UD_CONV_AMBIG:
        retn = OV_CMP_SAME;
        break;
    case OV_RANK_UD_CONV:
        retn = compareScalar( &first->u.ud.out
                            , first_type
                            , &second->u.ud.out
                            , second_type
                            , TRUE
                            , control );
        break;
    case OV_RANK_STD_CONV_DERIV:
        if( ( first_type != NULL ) && ( second_type != NULL ) ) {
            retn = compareDerived( *first_type, *second_type );
            if( retn != OV_CMP_SAME ) {
                break;
            }
        }
        // two target types are not releated
        // otherwise, do the following comparison
    case OV_RANK_STD_CONV_VOID:
    case OV_RANK_STD_CONV:
    case OV_RANK_PROMOTION:
    case OV_RANK_TRIVIAL:
        retn = compareScalar( &first->u.no_ud
                             , first_type
                             , &second->u.no_ud
                             , second_type
                             , FALSE
                             , control);
        break;
    case OV_RANK_SAME:
    case OV_RANK_EXACT:
        retn = OV_CMP_SAME;
        break;
    DbgDefault( "funny rank\n" );
    }
    return( retn );
}
#endif

static OV_RESULT compareFunction(
/*******************************/
    FNOV_LIST *first
  , FNOV_LIST *second
  , FNOV_CONTROL control )
// compare ranking of first and second
// returns one of:
//       OV_CMP_BETTER_FIRST
//       OV_CMP_BETTER_SECOND
//       OV_CMP_SAME
{
    int         index;
    FNOV_RANK   *first_arg;
    FNOV_RANK   *second_arg;
    OV_RESULT   result;
    OV_RESULT   retn;
    TYPE        *first_type;
    TYPE        *second_type;

    retn = OV_CMP_UNDEFINED;
    index = first->num_args;
    first_arg  = first->rankvector;
    second_arg = second->rankvector;
    first_type = first->alist->type_list;
    second_type = second->alist->type_list;
    for(;;) {
        if( retn == OV_CMP_SAME ) break;
        if( index == 0 ) break;
        result = compareArgument( first_arg
                                , first_type
                                , second_arg
                                , second_type
                                , control );
        switch( result ) {
        case OV_CMP_BETTER_FIRST:
            switch( retn ) {
            case OV_CMP_UNDEFINED:
                retn = OV_CMP_BETTER_FIRST;
                break;
            case OV_CMP_BETTER_SECOND:
                retn = OV_CMP_SAME;
                break;
            }
            break;
        case OV_CMP_BETTER_SECOND:
            switch( retn ) {
            case OV_CMP_UNDEFINED:
                retn = OV_CMP_BETTER_SECOND;
                break;
            case OV_CMP_BETTER_FIRST:
                retn = OV_CMP_SAME;
                break;
            }
            break;
        }
        index--;
        first_arg++;
        second_arg++;
        first_type++;
        second_type++;
    }
    if( retn == OV_CMP_UNDEFINED ) {
        result = compareArgument( &first->thisrank
                                , NULL
                                , &second->thisrank
                                , NULL, FNC_DEFAULT );
        switch( result ) {
        case OV_CMP_BETTER_FIRST:
            retn = OV_CMP_BETTER_FIRST;
            break;
        case OV_CMP_BETTER_SECOND:
            retn = OV_CMP_BETTER_SECOND;
            break;
        default:
            retn = OV_CMP_SAME;
            break;
        }
    }
    return( retn );
}

static boolean isRank( FNOV_LIST *entry, FNOV_COARSE_RANK level )
/***************************************************************/
// see if rank of entry is all <= level
// if so, return TRUE, else FALSE
{
    int                 index;
    FNOV_RANK           *rank;
    boolean             retn;

    retn = TRUE;
    rank = entry->rankvector;
    index = entry->num_args;
    for(;;) {
        if( !retn ) break;
        if( index == 0 ) break;
        retn = ( rank->rank <= level );
        index--;
        rank++;
    }
    if( retn ) {
        retn = ( entry->thisrank.rank <= level );
    }
    return( retn );
}

static void setRank( FNOV_LIST *entry, FNOV_COARSE_RANK level )
/*************************************************************/
// make rank = level
{
    int                 index;
    FNOV_RANK           *rank;

    rank = entry->rankvector;
    index = entry->num_args;
    for(;;) {
        if( index == 0 ) break;
        rank->rank = level;
        index--;
        rank++;
    }
    entry->thisrank.rank = level;
}

static boolean isReturnIdentical( TYPE sym1, TYPE sym2 )
/******************************************************/
// see if two functions have indentical return types
{
    boolean retn;

    retn = TypesSameExclude( FunctionDeclarationType( sym1 )->of
                           , FunctionDeclarationType( sym2 )->of
                           , TC1_NOT_ENUM_CHAR );

    return( retn );
}

static void doComputeArgRank( SYMBOL sym, TYPE src, TYPE tgt, PTREE *pt,
/**********************************************************************/
    FNOV_RANK *rank )
{
    if( (rank->control & FNC_TEMPLATE) &&
        SymIsFunctionTemplateModel( sym ) ) {
        rank->rank = OV_RANK_NO_MATCH;
        return;
    }
    FnovArgRank( src, tgt, pt, rank );
    if( (rank->control & FNC_EXCLUDE_CONV) &&
        (rank->rank > OV_RANK_TRIVIAL) &&
        (TypeReference( tgt ) != NULL) ) {
        rank->rank = OV_RANK_NO_MATCH;
    } else if( (rank->control & FNC_TEMPLATE) &&
        (rank->rank > OV_RANK_TRIVIAL) &&
        SymIsFunctionTemplateInst( sym ) ) {
        rank->rank = OV_RANK_NO_MATCH;
    } else if( (rank->control & FNC_DISTINCT_CHECK) &&
        (rank->rank == OV_RANK_SAME) ) {
        FnovArgRank( tgt, src, NULL, rank );
    }
}


static boolean computeUdcRank( FNOV_INFO* info )
/***************************************************************/
// fill in rankvector, ranking of conversion func return value to arg_list
// if u-d conversion is a candidate, return TRUE, else FALSE
{
    FNOV_RANK   *frv;
    TYPE        src;
    TYPE        tgt;
    FNOV_LIST   *func;

    func = info->candfunc;
    src = FunctionDeclarationType( func->sym->sym_type )->of;
    tgt = info->alist->type_list[0];

    frv = func->rankvector;
    doComputeArgRank( func->sym, src, tgt, NULL, frv );
    if( frv->rank == OV_RANK_NO_MATCH ) {
        return( FALSE );
    }
    return( TRUE );
}

static void moveRingFromTo( FNOV_LIST **from, FNOV_LIST **to )
/************************************************************/
// remove each element in from list, add each into to list
{
    FNOV_LIST *curr;

    RingIterBegSafe( *from, curr ) {
        RingPrune( from, curr );
        RingInsert( to, curr, NULL );
    } RingIterEndSafe( curr );
}

static void resolveOneList( FNOV_LIST **list, FNOV_LIST **match,
/******************************************************************/
FNOV_LIST **rejects, boolean *ambiguous, boolean is_ctor )
// Continue resolving, starting from the current 'match' and 'rejects' lists.
{
    FNOV_LIST  *curr;           // current element of list
    OV_RESULT   result;         // result of comparision call
    arg_list    *arg;           // arg list for ctor

    RingIterBegSafe( *list, curr ) {
        if( curr->rankvector->rank != OV_RANK_NO_MATCH ) {
            RingPrune( list, curr );
            if( *match != NULL ) {
                result = compareArgument( (*match)->rankvector, NULL,
                                          curr->rankvector, NULL, FNC_DEFAULT );
                if( result == OV_CMP_SAME && !is_ctor ) {
                    result = compareArgument( &(*match)->thisrank, NULL
                                              , &curr->thisrank, NULL, FNC_DEFAULT );
                }
                switch( result ) {
                case OV_CMP_BETTER_FIRST:
                    RingInsert( rejects, curr, NULL );
                    break;
                case OV_CMP_BETTER_SECOND:
                    moveRingFromTo( match, rejects );
                    RingInsert( match, curr, NULL );
                    *ambiguous = FALSE;
                    break;
                case OV_CMP_SAME:
                    RingInsert( match, curr, NULL );
                    *ambiguous = TRUE;
                    break;
                }
            } else {
                RingInsert( match, curr, NULL );
            }
        } else {
            if( is_ctor ) {
                arg = SymFuncArgList( curr->sym );
                if( arg->num_args == 1 ) {
                    RingPrune( list, curr );
                    RingInsert( rejects, curr, NULL );
                }
            } else {
                RingPrune( list, curr );
                RingInsert( rejects, curr, NULL );
            }
        }
    } RingIterEndSafe( curr )
}

FNOV_COARSE_RANK RankandResolveUDCsDiag( FNOV_LIST **ctorList,
/***********************************************************************/
FNOV_LIST **udcfList, TYPE src, TYPE tgt, boolean *isctor,
FNOV_LIST **pmatch, FNOV_UDC_CONTROL control, FNOV_INTRNL_CONTROL ictl,
PTREE *src_ptree, FNOV_RANK *rank, FNOV_DIAG *fnov_diag)
// rank elements of both the ctor list an the udcf list
// resolve ranks of both lists as if they were in one merged list
{
    boolean             ambiguous;
    FNOV_LIST           *between_match;
    FNOV_LIST           *rejects;
    FNOV_COARSE_RANK    result;
    FNOV_LIST           *match;

    udcRankCtor( *ctorList, src, tgt, control, ictl, src_ptree );
    udcRankUDCF( *udcfList, src, tgt, ictl, src_ptree );
    match = NULL;
    rejects = NULL;
    ambiguous = FALSE;
    resolveOneList( ctorList, &match, &rejects, &ambiguous, TRUE );
    between_match = match;
    resolveOneList( udcfList, &match, &rejects, &ambiguous, FALSE );
    if( isctor != NULL ) {
        *isctor = ( between_match == match);
    }


    if( ambiguous ) {
        result = OV_RANK_UD_CONV_AMBIG;
    } else {
        if( match == NULL ) {
            result = OV_RANK_NO_MATCH;
        } else {
            result = OV_RANK_UD_CONV;
        }
    }
    if( rank != NULL ) {
        rank->rank = result;
        if( result != OV_RANK_NO_MATCH ) {
            rank->userdef = 1;
            if( between_match == match ) { // ctor
                rank->u.ud.in = match->rankvector->u.no_ud;
                // need rank->u.ud.out
                rank->u.ud.out = rankCtorReturn( match, tgt, src_ptree );
                if( match->rankvector->rank == OV_RANK_UD_CONV ) {
                    rank->u.ud.out.udcnv++;
                }
            } else { // UDCF
                rank->u.ud.out = match->rankvector->u.no_ud;
            }
        }
    }
    if( fnov_diag != NULL ) {
        fnov_diag->num_candidates = RingCount( match ) + RingCount( rejects );
    }
    if( ambiguous ) {
        setFnovDiagnosticAmbigList( fnov_diag, &match );
        match = NULL;
    }
    if( pmatch != NULL ) {
        *pmatch = match;
    } else {
        FnovListFree( &match );
    }
    setFnovDiagnosticRejectList( fnov_diag, &rejects );
    FnovListFree( ctorList ); // will contain those with other than one parm
    DbgAssert( *udcfList == NULL );
    return result;
}

static void computeFuncRankSym( SYMBOL fsym, SYMBOL curr, TYPE *tgt,
/***********************************************************************/
// ranks curr, updating bestrank
FNOV_RANK *bestrank, FNOV_RANK *curr_rank, boolean src_mptr )
{
    OV_RESULT       result;
    TYPE            curr_type;

    curr_rank->rank = OV_RANK_NO_MATCH;
    curr_type = NULL;
    if( SymIsThisFuncMember( curr ) ) {
        if( src_mptr ) {
            // curr is member function
            // src can be member function
            curr_type = MakeMemberPointerTo( SymClass( curr )
                                           , curr->sym_type );
        }
    } else {
        // curr is static member function
        // src can be static member functions
        curr_type = curr->sym_type;
    }
    if( curr_type != NULL ) {
        doComputeArgRank( fsym, curr_type, *tgt, NULL, curr_rank );
        if( curr_rank->rank != OV_RANK_NO_MATCH ) {
            result = compareArgument( bestrank, NULL, curr_rank, NULL, FNC_DEFAULT );
            if( result == OV_CMP_BETTER_SECOND ) {
                *bestrank = *curr_rank;
            }
        }
    }
}

static void computeFuncRank( SYMBOL fsym, SYMBOL sym, TYPE *tgt,
/***********************************************************************/
// called only when an argument is a (possible overloaded) function symbol
FNOV_RANK *bestrank, PTREE *ptlist )
{
    boolean             src_mptr;   // can src be a pointer to member
    PTREE               fn;
    unsigned            retn;
    SYMBOL              curr;
    SYM_REGION          *region;
    auto FNOV_RANK      curr_rank;
    SEARCH_RESULT       *result;

    initRankVector( FNC_DEFAULT, &curr_rank, 0 );
    retn = NodeAddrOfFun( *ptlist, &fn );
    DbgAssert( retn != ADDR_FN_NONE );
    src_mptr = FALSE;
    if( fn->flags & PTF_COLON_QUALED ) { // i.e. S::foo not just foo
        // &S::foo can be a mptr
        // just S:: can also be a mptr if extensions enabled
        src_mptr = ( retn == ADDR_FN_MANY || retn == ADDR_FN_ONE ||
                     CompFlags.extensions_enabled );

    }
    bestrank->rank = OV_RANK_NO_MATCH;
    result = fn->u.symcg.result;
    if( result == NULL || result->region == NULL ) {
        RingIterBeg( sym, curr ) {
            computeFuncRankSym( fsym
                              , curr
                              , tgt
                              , bestrank
                              , &curr_rank
                              , src_mptr );
        } RingIterEnd( curr )
    } else {

        RingIterBeg( result->region, region ) {
            RingIterBegFrom( region->from, curr ) {
                computeFuncRankSym( fsym
                                  , curr
                                  , tgt
                                  , bestrank
                                  , &curr_rank
                                  , src_mptr );
            } RingIterEndTo( curr, region->to )
        } RingIterEnd( region )

    }
}

static boolean computeFunctionRank( FNOV_INFO* info )
/***************************************************/
// fill in rankvector, ranking of conversion of arg_list to func arguments
// if function is a candidate, return TRUE, else FALSE
{
    int         index;
    TYPE        *tgt;
    TYPE        *src;
    FNOV_RANK   *rank;
    SYMBOL      sym;
    PTREE       *ptlist;
    FNOV_LIST   *func;

    index = info->alist->num_args;
    src   = info->alist->type_list;
    func  = info->candfunc;
    tgt   = func->alist->type_list;
    rank  = func->rankvector;
    ptlist = info->plist;
    for(;;) {
        if( index == 0 ) break;
        if( ptlist != NULL ) {
            sym = FunctionSymbol( *ptlist );
            if( sym != NULL ) {
                computeFuncRank( func->sym, sym, tgt, rank, ptlist );
            } else {
                doComputeArgRank( func->sym, *src, *tgt, ptlist, rank );
            }
        } else {
            doComputeArgRank( func->sym, *src, *tgt, NULL, rank );
        }
        if( rank->control & FNC_DISTINCT_CHECK ) {
            if( rank->rank > OV_RANK_SAME ) {
                // short circuit for distinct check
                return( FALSE );
            }
        }
        if( rank->rank == OV_RANK_NO_MATCH ) {
            return( FALSE );
        }
        index--;
        tgt++;
        src++;
        if( ptlist != NULL ) ptlist++;
        rank++;
    }
    return( TRUE );
}

static boolean getRank( FNOV_INFO* info )
/***************************************/
// get a rankvector (allocate if necessary) and compute the rank
// returns TRUE if rank is a contender, else FALSE
{
    boolean contender;
    FNOV_LIST* candidate = info->candfunc;

    if( candidate->rankvector != NULL ) {
        contender = TRUE;
    } else {
        FNOV_CONTROL control = info->control;
        if( candidate->member ) {
            control |= FNC_MEMBER;
        }
        if( candidate->stdops ) {
            control |= FNC_STDOPS;
        }
        addRankVector( candidate, control );
        assert( info->alist != NULL );
        if( control & FNC_RANK_RETURN ) {
            contender = computeUdcRank( info );
        } else {
            contender = computeFunctionRank( info );
        }
        if( contender && SymIsThisFuncMember( candidate->sym ) ) {
            type_flag   srcflags = info->alist->qualifier;
            type_flag   tgtflags = candidate->alist->qualifier;
            if( !FnovCvFlagsRank( srcflags, tgtflags, &candidate->thisrank ) ) {
                FnovMemFlagsRank( srcflags, tgtflags, NULL, NULL,
                              &candidate->thisrank );
            }
        }
    }
    return( contender );
}

static OV_RESULT updateMatchList( FNOV_INFO *info )
/*************************************************/
// walk pmatch list and compare candidate with each entry:
// if candidate is worse than entry then quit
// if candidate is better than entry then remove entry from pmatch
//      and insert entry onto the head of pcandidates
// if candidate is the same as entry then leave it be
// if we get to the end of the loop,
//      remove candidate from pcandidate list
//      and add candidate to the pmatch list
{
    FNOV_LIST   *entry;
    OV_RESULT   comparison;
    int         ambiguous;

    ambiguous = FALSE;
    RingIterBegSafe( *info->pmatch, entry ) {
        comparison = compareFunction( entry, info->candfunc, info->control );
        if( comparison == OV_CMP_BETTER_FIRST ) {
            return( OV_CMP_BETTER_FIRST );
        } else if( comparison == OV_CMP_BETTER_SECOND ) {
            RingPrune( info->pmatch, entry );
            RingInsert( info->pcandidates, entry, NULL );
        } else if( comparison == OV_CMP_SAME ) {
            if( entry->sym == info->candfunc->sym ) {
                return( OV_CMP_BETTER_FIRST ); // have the same symbol twice
            }
            ambiguous = TRUE;
        }
    } RingIterEndSafe( entry )
    RingPrune( info->pcandidates, info->candfunc );
    RingAppend( info->pmatch, info->candfunc );
    if( ambiguous ) {
        return( OV_CMP_SAME );
    }
    return( OV_CMP_BETTER_SECOND );
}

static void updateFnovList( FNOV_INFO *info )
/*******************************************/
// update lists and summary result
{
    OV_RESULT   comparison;

    comparison = updateMatchList( info );
    if( comparison == OV_CMP_SAME ) {
        // candidate is ambiguous with pmatch list
        info->result = FNOV_AMBIGUOUS;
    } else if( comparison == OV_CMP_BETTER_SECOND ) {
        // candidate is better than pmatch list
        info->result = FNOV_NONAMBIGUOUS;
    }
        // otherwise candidate is worse than pmatch list which doesn't
        // change the presult value
}

static FNOV_RESULT resolveOverload( FNOV_INFO* info )
/***************************************************/
// go through each member of pcandidates looking for best match
// returns FNOV_NO_MATCH     - no match found,
//                              pmatch = NULL
//         FNOV_AMBIGUOUS    - no unique match found,
//                              pmatch = list of ambiguous entries
//         FNOV_NONAMBIGUOUS - unique match found,
//                              pmatch = unique entry
{
    info->result = FNOV_NO_MATCH;
    *info->pmatch = NULL;
    RingIterBegSafe( *info->pcandidates, info->candfunc ) {
        if( getRank( info ) ) {
            updateFnovList( info );
        }
    } RingIterEndSafe( info->candfunc )
    return info->result;
}

static FNOV_RESULT genTemplateFunction( arg_list *alist, SYMBOL *psym,
/********************************************************************/
    TOKEN_LOCN *locn, SYMBOL *ambigs, boolean ok_if_no_exact_bind )
{
    FNOV_RESULT result;
    SYMBOL sym;

    sym = *psym;
    result = TemplateFunctionGenerate( &sym, alist, locn, ambigs, ok_if_no_exact_bind );
    if( result == FNOV_NONAMBIGUOUS ) {
        *psym = sym;
    }
    return( result );
}

static SYMBOL findFunctionTemplate( FNOV_INFO *info )
/***************************************************/
{
    SYMBOL sym;
    FNOV_LIST *curr;

    RingIterBeg( *info->pcandidates, curr ){
        sym = curr->sym;
        if( SymIsFunctionTemplateModel( sym ) ) {
            return( sym );
        }
    } RingIterEnd( curr )
#ifndef NDEBUG
    CFatal( "can't find function template during overload" );
#endif
    return( NULL );
}

static TOKEN_LOCN *extractAGoodLocn( FNOV_INFO* info )
/****************************************************/
{
    TOKEN_LOCN *locn;
    PTREE *plist;

    plist = info->plist;
    /* simple for now; but who knows? */
    if( plist == NULL ) {
        // NYI: what location do we give to a "return foo;" when there is
        // more than one foo (currently a NULL locn because there are no args)
        return( NULL );
    }
    locn = &((*plist)->locn);
    return( locn );
}

static FNOV_RESULT doOverload( FNOV_INFO* info )
/**********************************************/
{
    FNOV_RESULT result = FNOV_NO_MATCH;
    FNOV_LIST   *match = NULL;
    TOKEN_LOCN *locn;

    *info->pmatch = NULL;
    if( *info->pcandidates != NULL ) {
        result = resolveOverload( info );
        if( result != FNOV_NONAMBIGUOUS
         || ! isRank( *info->pmatch, OV_RANK_SAME ) ) {
            if( ( info->control & ( FNC_TEMPLATE | FNC_DISTINCT_CHECK ) )
                == FNC_TEMPLATE ) {
                SYMBOL      sym;
                FNOV_RESULT t_result;
                auto SYMBOL ambigs[2];

                sym = findFunctionTemplate( info );
                t_result = FNOV_NO_MATCH;
                if( sym != NULL ) {
                    locn = extractAGoodLocn( info );
                    t_result = genTemplateFunction( info->alist
                                                  , &sym
                                                  , locn
                                                  , ambigs
                                                  , result == FNOV_NONAMBIGUOUS );
                }
                if( t_result == FNOV_NONAMBIGUOUS ) {
                    FnovListFree( info->pmatch );
                    addListEntry( FNC_DEFAULT
                                , info
                                , sym
                                , SymFuncArgList( sym )
                                , LENT_MATCH );
                    addRankVector( *info->pmatch, info->control );
                    setRank( *info->pmatch, OV_RANK_SAME );
                    result = FNOV_NONAMBIGUOUS;
                } else if( t_result == FNOV_AMBIGUOUS ) {
                    FnovListFree( info->pmatch );
                    result = FNOV_AMBIGUOUS;
                    addListEntry( FNC_DEFAULT
                                , info
                                , ambigs[0]
                                , SymFuncArgList( ambigs[0] )
                                , LENT_MATCH );
                    addListEntry( FNC_DEFAULT
                                , info
                                , ambigs[1]
                                , SymFuncArgList( ambigs[1] )
                                , LENT_MATCH );
                }
            }
            if( result != FNOV_NONAMBIGUOUS ) {
                match = *info->pmatch;
                *info->pmatch = NULL;
            }
        }
    }

#ifndef NDEBUG
    if( PragDbgToggle.dump_rank ) {
        printf( "\nOverloaded Symbol Resolution" );
        PrintFnovResolution( result
                           , info->alist
                           , match == NULL ? *info->pmatch : match
                           , *info->pcandidates
                           , NULL );
    }
#endif

    if( result != FNOV_NONAMBIGUOUS ) {
        setFnovDiagnosticAmbigList( info->fnov_diag, &match );
    }
    setFnovDiagnosticRejectList( info->fnov_diag, info->pcandidates );
    return( result );
}

FNOV_DIAG * FnovInitDiag( FNOV_DIAG *fnov_diag )
/**********************************************/
{
    if( fnov_diag != NULL ) {
        fnov_diag->diag_ambig = NULL;
        fnov_diag->diag_reject = NULL;
        fnov_diag->num_candidates = LIST_FREE;
    }
    return( fnov_diag );
}

FNOV_RESULT FuncOverloadedLimitDiag( SYMBOL *resolved,
/****************************************************************/
SEARCH_RESULT *result_in, SYMBOL sym, arg_list *alist, PTREE *ptlist,
FNOV_CONTROL control, FNOV_DIAG *fnov_diag )
// find overloaded function from sym for alist specified
{
    FNOV_LIST   *candidates = NULL;
    FNOV_INFO   info;
    FNOV_LIST   *match;
    FNOV_RESULT result;

    *resolved = NULL;

    info.alist = alist;
    info.plist = ptlist;
    info.pcandidates = &candidates;
    info.pmatch = &match;
    info.control = control;
    info.has_template = FALSE;
    info.fnov_diag = fnov_diag;

    if( ( result_in == NULL) || (result_in->region == NULL) ) {
        buildOverloadListFromSym( info.control, &info, sym );
    } else {
        buildOverloadListFromRegion( info.control, &info, result_in->region );
    }
    if( info.has_template ) {
        info.control |= FNC_TEMPLATE;
    }
    fnovNumCandidatesSet( info.fnov_diag, *info.pcandidates );
    result = doOverload( &info );
    if( match != NULL ) {
        *resolved = match->sym;
        FnovListFree( &match );
    }
    return( result );
}

FNOV_RESULT FuncOverloadedDiag( SYMBOL *resolved, SEARCH_RESULT *result,
/***********************************************************************/
SYMBOL sym, arg_list *alist, PTREE *ptlist, FNOV_DIAG *fnov_diag )
// find overloaded function from sym for alist specified
{
    fnov_diag = FnovInitDiag( fnov_diag );
    return( FuncOverloadedLimitDiag( resolved, result, sym, alist, ptlist,
                                     FNC_DEFAULT, fnov_diag ) );
}

FNOV_RESULT FuncOverloaded( SYMBOL *resolved, SEARCH_RESULT *result,
/***********************************************************************/
SYMBOL sym, arg_list *alist, PTREE *ptlist )
// find overloaded function from sym for alist specified
{
    return( FuncOverloadedDiag( resolved, result, sym, alist, ptlist, NULL ) );
}

FNOV_RESULT UdcOverloadedDiag( SYMBOL *resolved, SEARCH_RESULT *result,
/*****************************************************************/
SYMBOL sym, TYPE type, type_flag this_qualifier, FNOV_DIAG *fnov_diag )
// find overloaded user-defined conversion from sym for return type specified
// use this_qualifiers from arg_list
{
    arg_list        alist;

    InitArgList( &alist );
    alist.num_args = 1;
    alist.qualifier = this_qualifier;
    alist.type_list[0] = type;
    fnov_diag = FnovInitDiag( fnov_diag );
    return( FuncOverloadedLimitDiag( resolved
                                   , result
                                   , sym
                                   , &alist
                                   , NULL
                                   , FNC_RANK_RETURN
                                   , fnov_diag ));
}

FNOV_RESULT UdcOverloaded( SYMBOL *resolved, SEARCH_RESULT *result,
/*****************************************************************/
SYMBOL sym, TYPE type, type_flag this_qualifier )
// find overloaded user-defined conversion from sym for return type specified
// use this_qualifiers from arg_list
{
    return( UdcOverloadedDiag( resolved, result, sym, type, this_qualifier, NULL ) );

}

static FNOV_RESULT opOverloadedLimitExDiag( SYMBOL *resolved, SEARCH_RESULT *member,
/******************************************************************/
    SEARCH_RESULT *nonmember, SEARCH_RESULT *namesp, SYMBOL stdops, arg_list *alist, PTREE *ptlist,
    boolean scalar_convert, FNOV_CONTROL control, FNOV_DIAG *fnov_diag )
// find overloaded operator from member, nonmember and stdops for alist specified
// return FNOV_RESULT same as FuncOverloaded
// result points at the symbol chosen, if non-ambiguous
{
    FNOV_RESULT     result;
    FNOV_LIST       *match = NULL;
    FNOV_LIST       *candidates = NULL;
    SYMBOL          sym;
    FNOV_INFO       info;

    info.alist = alist;
    info.plist = ptlist;
    info.pcandidates = &candidates;
    info.pmatch = &match;
    info.control = control;
    info.has_template = FALSE;
    info.fnov_diag = FnovInitDiag( fnov_diag );

    *resolved = NULL;
    if( member != NULL ) {
        if( member->region == NULL ) {
            sym = member->sym_name->name_syms;
            buildOverloadListFromSym( FNC_MEMBER, &info, sym );
        } else {
            // namespaces
            buildOverloadListFromRegion( FNC_MEMBER
                                       , &info
                                       , member->region );
        }

    }
    if( nonmember != NULL ) {
        if( nonmember->region == NULL ) {
            sym = nonmember->sym_name->name_syms;
            buildOverloadListFromSym( FNC_DEFAULT, &info, sym );
        } else {
            buildOverloadListFromRegion( FNC_DEFAULT
                                       , &info
                                       , nonmember->region );
        }
    }
    if( namesp != NULL ) {
        if( namesp->region != NULL ) {
            // namespaces
            buildOverloadListFromRegion( FNC_DEFAULT
                                       , &info
                                       , namesp->region );
        } else {
            sym = namesp->sym_name->name_syms;
            buildOverloadListFromSym( FNC_DEFAULT, &info, sym );
        }
    }
    // don't include stdops
    fnovNumCandidatesSet( info.fnov_diag, candidates );
    if( stdops != NULL ) {
        FNOV_CONTROL ctl = scalar_convert ? FNC_DEFAULT : FNC_STDOPS;
        buildOverloadListFromSym( ctl, &info, stdops );
    }

    if( info.has_template ) {
        info.control |= FNC_TEMPLATE;
    }
    result = doOverload( &info );
    if( match != NULL ) {
        *resolved = match->sym;
        FnovListFree( &match );
    }
    return( result );
}

FNOV_RESULT OpOverloadedLimitDiag( SYMBOL *resolved, SEARCH_RESULT *member,
/****************************************************************/
    SEARCH_RESULT *nonmember,  SEARCH_RESULT *namesp, SYMBOL stdops, arg_list *alist, PTREE *ptlist,
    FNOV_CONTROL control, FNOV_DIAG *fnov_diag )
// find overloaded operator from member, nonmember and stdops for alist specified
// return FNOV_RESULT same as FuncOverloaded
// result points at the symbol chosen, if non-ambiguous
{
    return opOverloadedLimitExDiag( resolved, member, nonmember, namesp, stdops,
                alist, ptlist, FALSE, control, fnov_diag );
}

FNOV_RESULT OpOverloadedDiag( SYMBOL *resolved, SEARCH_RESULT *member,
/****************************************************************/
    SEARCH_RESULT *nonmember, SEARCH_RESULT *namesp, SYMBOL stdops, arg_list *alist, PTREE *ptlist,
    FNOV_DIAG *fnov_diag )
// find overloaded operator from member, nonmember and stdops for alist specified
// return FNOV_RESULT same as FuncOverloaded
// result points at the symbol chosen, if non-ambiguous
{
    return opOverloadedLimitExDiag( resolved, member, nonmember, namesp, stdops,
        alist, ptlist, FALSE, FNC_DEFAULT, fnov_diag );
}

static SYMBOL findNonDefargSym( FNOV_LIST *match)
{
    FNOV_LIST *curr;

    RingIterBeg( match, curr ) {
        if( !SymIsDefArg(curr->sym) ) {
            return( curr->sym );
        }
    } RingIterEnd( curr )
    DbgAssert( FALSE );         // should never get here
    return match->sym;
}

static FNOV_RESULT doFunctionDistinctCheck( FNOV_CONTROL control,
/***************************************************************/
    SYMBOL *pold_sym, SYMBOL new_sym, char*name )
// determine if new_sym is a distinct function w.r.t old_sym
// returns:
//    FNOV_DISTINCT             - completely distinct
//    FNOV_NOT_DISTINCT         - not distinct because of arguments
//    FNOV_NOT_DISTINCT_RETURN  - args match exactly but return type is wrong
//    FNOV_EXACT_MATCH          - function matches exactly
//    *pold_sym = match->sym if return != FNOV_DISTINCT
//
{
    SYMBOL          old_sym = *pold_sym;
    FNOV_RESULT     result = FNOV_DISTINCT;
    FNOV_RESULT     overload_result = FNOV_NO_MATCH;
    FNOV_LIST       *candidates = NULL; // list of symbols to resolve
    FNOV_LIST       *match = NULL;
    auto arg_list   alist;
    FNOV_INFO       info;

    *pold_sym = NULL;

    // check for conversion operator
    if( name == CppConversionName() ) {
        control |= FNC_RANK_RETURN;
        InitArgList( &alist );
        alist.num_args = 1;
        alist.type_list[0] = FunctionDeclarationType( new_sym->sym_type )->of;
        alist.qualifier = FunctionThisFlags( new_sym );
        info.alist = &alist;
    } else {
        info.alist = SymFuncArgList( new_sym );
    }

    info.plist = NULL;
    info.pcandidates = &candidates;
    info.pmatch = &match;
    info.control = control;
    info.has_template = FALSE;
    info.fnov_diag = NULL;

    buildOverloadListFromSym( control, &info, old_sym );
    fnovNumCandidatesSet( info.fnov_diag, *info.pcandidates );

    if( *info.pcandidates != NULL ) {
        overload_result = resolveOverload( &info );
        if( overload_result != FNOV_NO_MATCH ) {
            DbgAssert( overload_result == FNOV_NONAMBIGUOUS ||
                       overload_result == FNOV_AMBIGUOUS );
            *pold_sym = match->sym;
            if( isRank( match, OV_RANK_EXACT ) ) {
                if( isReturnIdentical( match->sym->sym_type
                                     , new_sym->sym_type ) ) {
                    if( overload_result == FNOV_AMBIGUOUS ) {
                        // need ambig list here!!
                        *pold_sym = findNonDefargSym( match );
                    }
                    result = FNOV_EXACT_MATCH;
                } else {
                    result = FNOV_NOT_DISTINCT_RETURN;
                }
            } else if( isRank( match, OV_RANK_SAME ) ) {
                result = FNOV_NOT_DISTINCT;
            }
        }
    }

#ifndef NDEBUG
    if( PragDbgToggle.dump_rank ) {
        printf( "Overloaded Symbol Distinct Check" );
        PrintFnovResolution( result
                           , info.alist
                           , *info.pmatch
                           , *info.pcandidates
                           , new_sym );
    }
#endif

    setFnovDiagnosticRejectList( info.fnov_diag, info.pcandidates );
    setFnovDiagnosticAmbigList( info.fnov_diag, info.pmatch );
    return result;
}


FNOV_RESULT IsOverloadedFuncDistinct( SYMBOL *pold_sym,SYMBOL new_sym,char*name
                                    , FNOV_CONTROL control )
/******************************************************************************/
// determine if new_sym is a distinct function w.r.t old_sym
// returns:
//    FNOV_DISTINCT             - completely distinct
//    FNOV_NOT_DISTINCT         - not distinct because of arguments
//    FNOV_NOT_DISTINCT_RETURN  - args match exactly but return type is wrong
//    FNOV_EXACT_MATCH          - function matches exactly
//    *pold_sym = new_sym if arguments match exactly else NULL
{
    DbgAssert(( control & ~(FNC_NO_DEALIAS) ) == 0 );
    control |= FNC_EXCLUDE_ELLIPSIS | FNC_DISTINCT_CHECK;
    return doFunctionDistinctCheck( control, pold_sym, new_sym, name );
}

FNOV_RESULT AreFunctionsDistinct( SYMBOL *pold_sym,SYMBOL new_sym,char*name )
/***************************************************************************/
// determine if new_sym and old_sym are distinct functions
// returns:
//    FNOV_DISTINCT             - completely distinct
//    FNOV_NOT_DISTINCT         - not distinct because of arguments
//    FNOV_NOT_DISTINCT_RETURN  - args match exactly but return type is wrong
//    FNOV_EXACT_MATCH          - function matches exactly
//    *pold_sym = new_sym if arguments match exactly else NULL
{
    return( doFunctionDistinctCheck( FNC_EXCLUDE_ELLIPSIS
                                   | FNC_DISTINCT_CHECK
                                   | FNC_ONLY_SYM,
                                   pold_sym,
                                   new_sym,
                                   name ) );
}

boolean IsOverloadedFunc( SYMBOL sym )
/************************************/
// test if a function has been overloaded
{
    boolean retn = FALSE;

    if( sym != NULL ) {
        if( SymIsFunction( sym ) ) {
            retn = ( sym->next != sym );
        }
    }

#ifndef NDEBUG
    if( PragDbgToggle.dump_rank ) {
        VBUF name;
        FormatSym( sym, &name );
        printf( "Function '%s' is%soverloaded\n",
            name.buf,
            retn ? " " : " not " );
        VbufFree( &name );
    }
#endif
    return( retn );
}

static boolean doneCheckIdentical( SYMBOL curr, boolean isUDC, TYPE udc_retn,
/********************************/
SYMBOL *retn )
{
    boolean identical;
    boolean done;

    done = FALSE;
    identical = FALSE;
    if( isUDC ) {
        identical = TypesIdentical( SymFuncReturnType( curr ), udc_retn );
    } else {
        if( SymIsFunctionTemplateModel( curr ) ) {
            *retn = NULL;
            done = TRUE;
        } else {
            identical = !SymIsDefArg( curr );
        }
    }
    if( identical ) {
        if( *retn == NULL ) {
            *retn = curr;
        } else {
            *retn = NULL;
            done = TRUE;
        }
    }
    return done;
}

SYMBOL ActualNonOverloadedFunc( // GET SYMBOL FOR ACTUAL NON-OVERLOADED FUNC.
/*****************************/
SYMBOL sym,                     // - function for lookup
SEARCH_RESULT *result )         // - search result for function
{
    SYMBOL      retn;           // - actual non-overloaded function
    SYMBOL      curr;           // - current function in overload ring
    TYPE        udc_retn;       // - return type for UDC function
    boolean     isUDC;
    SYM_REGION  *region;
    boolean     done;

    retn = NULL;
    isUDC = SymIsUDC( sym );
    if( isUDC ) {
        udc_retn = SymFuncReturnType( sym );
    }
    if( result == NULL || result->region == NULL ) {
        RingIterBeg( sym, curr ) {
            if( doneCheckIdentical( curr, isUDC, udc_retn, &retn ) ) {
                break;
            }
        } RingIterEnd( curr )
    } else{
        done = FALSE;
        RingIterBeg( result->region, region ) {
            RingIterBegFrom( region->from, curr ) {
                done =  doneCheckIdentical( curr, isUDC, udc_retn, &retn );
                if( done ) {
                    break;
                }
            } RingIterEndTo( curr, region->to )
            if( done ) {
                break;
            }
        } RingIterEnd( region )
    }
    return retn;
}


boolean IsActualOverloadedFunc( // TEST IF ACTUAL (IGNORE SC_DEFAULT) OVERLOAD
/*****************************/
SYMBOL sym, SEARCH_RESULT *result ) // - function to be tested
// test if a function has been really overloaded
// ignore functions with SC_DEFAULT id's
{
    boolean retn = ( NULL == ActualNonOverloadedFunc( sym, result ) );
#ifndef NDEBUG
    if( PragDbgToggle.dump_rank ) {
        VBUF name;
        FormatSym( sym, &name );
        printf( "Function '%s' is%soverloaded (ignoring default arguments)\n",
            name.buf,
            retn ? " " : " not " );
        VbufFree( &name );
    }
#endif
    return( retn );
}

static void fnovInit( INITFINI* defn )
/************************************/
{
    defn = defn;
    carveFNOVLIST = CarveCreate( sizeof( FNOV_LIST ), BLOCK_FNOVLIST );
    carveManyFNOVRANK = CarveCreate( CACHE_FNOVRANK_SIZE * sizeof( FNOV_RANK ),
                                     BLOCK_ManyFNOVRANK );
}

static void fnovFini( INITFINI* defn )
/************************************/
{
    defn = defn;
#ifndef NDEBUG
    CarveVerifyAllGone( carveFNOVLIST, "FNOVLIST" );
    CarveVerifyAllGone( carveManyFNOVRANK, "ManyFNOVRANK" );
#endif
    CarveDestroy( carveFNOVLIST );
    CarveDestroy( carveManyFNOVRANK );
}


INITDEFN( fn_overload, fnovInit, fnovFini )


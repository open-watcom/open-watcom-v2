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


#include "plusplus.h"

#include <assert.h>

#include "fnovload.h"
#include "cgfront.h"
#include "stats.h"
#include "initdefs.h"
#include "typerank.h"
#include "convctl.h"


typedef struct                  // FNOV_TYPE -- conversion being classified
{
    PTREE       *ptnode;        // addr( ptr to parse tree node for arguemnt )
    TYPE        original;       // the original type provided
    TYPE        reftype;        // reference type that was skipped
    TYPE        basic;          // basic type (skipping any references)
    TYPE        final;          // final basic type after 'of' from basic type
    type_flag   leadflag;       // any leading flags
    type_flag   refflag;        // any flags after reference removed
    type_flag   finalflag;      // any flags after basic type
    void        *leadbase;      // any leading base
    void        *refbase;       // any base after reference removed
    void        *finalbase;     // any base after basic type
    unsigned    reference : 1;  // was a reference removed?
} FNOV_TYPE;

typedef struct                  // FNOV_CONV -- control for a conversion
{
    FNOV_TYPE wsrc;             // - source
    FNOV_TYPE wtgt;             // - target
    FNOV_RANK* rank;            // - ranking
} FNOV_CONV;

bool FnovCvFlagsRank( type_flag src, type_flag tgt, FNOV_RANK *rank )
/*******************************************************************/
{
    src &= TF1_CV_MASK;
    tgt &= TF1_CV_MASK;
    if( src != tgt ) {
        // check for allowable conversions
        if( src & ~tgt ) {
            // not allowed to remove const or volatile
            rank->rank = OV_RANK_NO_MATCH;
            return TRUE;                     // <<<--- early return
        } else {
            // special trivial cases:
            //      T* -> {const|volatile} T*
            //      T& -> {const|volatile} T&
            rank->rank = OV_RANK_TRIVIAL;
            rank->u.no_ud.trivial = 1;
        }
    }
    return FALSE;
}

void FnovMemFlagsRank( type_flag src, type_flag tgt,
    void *srcbase, void *tgtbase, FNOV_RANK *rank )
/**************************************************/
{
    src &= TF1_MEM_MODEL;
    tgt &= TF1_MEM_MODEL;
    if( src != tgt ) {
        rank->rank = OV_RANK_STD_CONV;
        rank->u.no_ud.standard++;
    } else if( (src & TF1_BASED) && !TypeBasesEqual( src, srcbase, tgtbase ) ) {
        rank->rank = OV_RANK_STD_CONV;
        rank->u.no_ud.standard++;
    }
}


#if 0
static bool functionsAreIdentical( TYPE fn_type1, TYPE fn_type2 )
/***************************************************************/
{
    bool retn;

    retn = TypesSameFnov( fn_type1, fn_type2 );
    return( retn );
}
#else

#define functionsAreIdentical( fn_type1, fn_type2 ) \
    TypesSameFnov( fn_type1, fn_type2 )

#endif


static RKD initFNOV_TYPE( FNOV_TYPE *ft, TYPE basic, PTREE* pt )
/**************************************************************/
{
    ft->original = basic;
    ft->ptnode   = pt;
    ft->final = NULL;
    ft->refflag = TF1_NULL;
    ft->finalflag = TF1_NULL;
    basic = TypeModExtract( basic
                          , &ft->leadflag
                          , &ft->leadbase
                          , TC1_NOT_ENUM_CHAR|TC1_NOT_MEM_MODEL );
    if( ( basic->id == TYP_POINTER ) && ( basic->flag & TF1_REFERENCE ) ) {
        ft->reference = TRUE;
        ft->reftype = basic;
        basic = TypeModExtract( basic->of
                              , &ft->refflag
                              , &ft->refbase
                              , TC1_NOT_ENUM_CHAR|TC1_NOT_MEM_MODEL );
    } else {
        ft->reference = FALSE;
        ft->reftype = NULL;
    }
    basic = PointerTypeForArray( basic );
    ft->basic = basic;
    return RkdForTypeId( basic->id );
}

static void completeFNOV_TYPE( FNOV_TYPE* ft )
/********************************************/
{
    type_flag finalflag;

#if 0
    TYPE basic = ft->basic;

    if( ( basic->id == TYP_POINTER )
      ||( basic->id == TYP_MEMBER_POINTER ) ) {
        ft->final = TypeModExtract( basic->of
                                  , &ft->finalflag
                                  , &ft->finalbase
                                  , TC1_NOT_ENUM_CHAR | TC1_NOT_MEM_MODEL );
    }
#else
    ft->final = TypeModExtract( ft->basic->of
                              , &ft->finalflag
                              , &ft->finalbase
                              , TC1_NOT_ENUM_CHAR | TC1_NOT_MEM_MODEL );
    if( ft->final != NULL ) {
        finalflag = TF1_NULL;

        while( ( ft->final->id == TYP_POINTER )
            || ( ft->final->id == TYP_ARRAY ) ) {

            if( ft->final->id != TYP_ARRAY ) {
                finalflag = TF1_NULL;
            }

            ft->final = TypeModExtract( ft->final->of
                                      , &ft->finalflag
                                      , &ft->finalbase
                                      , TC1_NOT_ENUM_CHAR | TC1_NOT_MEM_MODEL );

            ft->finalflag |= finalflag;
        }
    }
#endif
}

static bool trivialRankPtrToPtr( FNOV_CONV *conv )
/************************************************/
// return TRUE if a qualification conversion is needed
// but not possible, otherwise return FALSE
{
    CONVCTL     info;
    TYPE        src;
    TYPE        tgt;

    // check for stdop conversion that should ignore cv-qualifiers
    if( ( conv->wtgt.final->id == TYP_VOID )
      && ( conv->wtgt.final->flag & TF1_STDOP )
      && ( (conv->rank->control & FNC_STDOP_CV_VOID) == 0 ) ) {
        return FALSE;
    } else {
        // need to look down all levels here
        src = conv->wsrc.original;
        tgt = conv->wtgt.original;
        if( (conv->rank->control & FNC_DISTINCT_CHECK) == 0 ) {
            src = BindTemplateClass( src, NULL, TRUE );
            tgt = BindTemplateClass( tgt, NULL, TRUE );
        }
        ConvCtlInitTypes( &info, src, tgt );
        ConvCtlTypeDecay( &info, &info.src );
        ConvCtlTypeDecay( &info, &info.tgt );
        if( !ConvCtlAnalysePoints( &info ) ) {
            conv->rank->rank = OV_RANK_NO_MATCH;
            return TRUE;
        } else if( info.used_cv_convert ) {
            conv->rank->rank = OV_RANK_TRIVIAL;
            conv->rank->u.no_ud.trivial = 1;
        }
        return FALSE;
    }
}

TYPE *CompareWP13332(
/*******************/
    TYPE *first_type
  , TYPE *second_type )
{
    TYPE *better = NULL;
    RKD rkd_src;
    RKD rkd_tgt;
    FNOV_CONV conv;             // - conversion data
    rkd_src = initFNOV_TYPE( &conv.wsrc, *first_type, NULL );
    rkd_tgt = initFNOV_TYPE( &conv.wtgt, *second_type, NULL );

    if( conv.wsrc.reference && conv.wtgt.reference ) {
        type_flag first_flag = conv.wsrc.refflag & TF1_CV_MASK;
        type_flag second_flag = conv.wtgt.refflag & TF1_CV_MASK;
        // two references
        // both are references

        // first: S cv1 &
        // second: S cv1 cv2 &
        // first is better
        if( (first_flag & ~second_flag) == 0 ) { // didn't remove anything
            better = first_type;
        } else {
            // first: S cv1 cv2 &    or S cv1 cv2 *
            // second: S cv1 &       or S cv1 *
            // second is better
            if( (second_flag & ~first_flag) == 0 ) {
                better = second_type;
            }
        }
    } else if( !conv.wsrc.reference && !conv.wtgt.reference ) {
        if( rkd_src == RKD_POINTER && rkd_tgt == RKD_POINTER ) {
            FNOV_RANK rank;

            rank.rank = OV_RANK_NO_MATCH;
            conv.rank = &rank;

            completeFNOV_TYPE( &conv.wsrc );
            completeFNOV_TYPE( &conv.wtgt );
            trivialRankPtrToPtr( &conv );
            if( conv.rank->rank != OV_RANK_NO_MATCH ) {
                better = first_type;
            } else {
                initFNOV_TYPE( &conv.wsrc, *second_type, NULL );
                initFNOV_TYPE( &conv.wtgt, *first_type, NULL );
                completeFNOV_TYPE( &conv.wsrc );
                completeFNOV_TYPE( &conv.wtgt );
                trivialRankPtrToPtr( &conv );
                if( conv.rank->rank != OV_RANK_NO_MATCH ) {
                    better = second_type;
                }
            }
        }
    }
    return( better );
}

static bool fromConstZero( FNOV_CONV *conv )
/******************************************/
{
    PTREE *pnode;
    PTREE node;

    pnode = conv->wsrc.ptnode;
    if( pnode == NULL ) {
        return( FALSE );
    }
    node = *pnode;
    if( node != NULL && NodeIsZeroIntConstant( node ) ) {
        /* we don't want 0's that are cast to pointer types here */
        if( IntegralType( conv->wsrc.basic ) ) {
            conv->rank->rank = OV_RANK_STD_CONV;
            conv->rank->u.no_ud.standard++;
            return TRUE;
        }
    }
    return FALSE;
}


static bool exactRank( FNOV_CONV *conv )
/**************************************/
// return TRUE if these are identical, otherwise FALSE
{
    bool     retn;

    // check for non-ranking reference or flag change (for distinctness)
    if( ( conv->wsrc.reference == conv->wtgt.reference )
      &&( conv->wsrc.leadflag  == conv->wtgt.leadflag )
      &&( conv->wsrc.refflag   == conv->wtgt.refflag )
      &&( TypesSameFnov( conv->wsrc.basic, conv->wtgt.basic ) ) ) {
        conv->rank->rank = OV_RANK_EXACT;
        retn = TRUE;
    } else {
        retn = FALSE;

    }

    return( retn );
}

static bool sameRankPtrToPtr( FNOV_CONV *conv )
/*********************************************/
// return TRUE if wsrc must be the same as wtgt to be convertable and
// they are not, otherwise return FALSE
// this routine handles enum->enum and invalid function->function conversions
{
    TYPE        src_final;
    TYPE        tgt_final;

    tgt_final = conv->wtgt.final;
    if( tgt_final->id == TYP_FUNCTION ) {
        src_final = conv->wsrc.final;
        if( src_final->id == TYP_FUNCTION ) {
            if( !functionsAreIdentical( src_final, tgt_final ) ) {
                conv->rank->rank = OV_RANK_NO_MATCH;
                return TRUE;
            }
        }
    }
    return FALSE;
}


static bool trivialRank( FNOV_CONV *conv )
/****************************************/
// return TRUE if no standard conversion possible, else FALSE
{
    bool     quit;

    // check for allowable conversions
    // only care if changing reference or pointer
    if( conv->wsrc.reference ) {
        quit = FnovCvFlagsRank( conv->wsrc.refflag
                              , conv->wtgt.refflag
                              , conv->rank );
    } else {
        quit = FnovCvFlagsRank( conv->wsrc.leadflag
                              , conv->wtgt.refflag
                              , conv->rank );
    }
    return quit;
}


static bool rankTgtRefCvMem( FNOV_CONV *conv )
/********************************************/
{
    bool triv;                  // - TRUE ==> trivial conversion
    type_flag first;            // - first CV flag

    if( conv->wsrc.reference ) {
        first = conv->wsrc.refflag;
    } else {
        first = conv->wsrc.leadflag;
        if( conv->wtgt.reference ) {
            // add "const" flag to values so they can only be bound to
            // const references
            first |= TF1_CONST;
        }
    }
    triv = FnovCvFlagsRank( first
                          , conv->wtgt.refflag
                          , conv->rank );
    if( ! triv ) {
        if( conv->rank->rank < OV_RANK_NO_MATCH ) {
            if( conv->wsrc.reference ) {
                FnovMemFlagsRank( conv->wsrc.refflag
                                , conv->wtgt.refflag
                                , conv->wsrc.refbase
                                , conv->wtgt.refbase
                                , conv->rank );
            } else {
                FnovMemFlagsRank( first
                                , conv->wtgt.refflag
                                , conv->wsrc.leadbase
                                , conv->wtgt.refbase
                                , conv->rank );
            }
        }
        triv = FALSE;
    }
    return triv;
}

static bool functionRank( FNOV_CONV *conv )
/*****************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
// this routine handles valid function->function conversions
{
    TYPE src_final;
    TYPE tgt_final;

    tgt_final = conv->wtgt.final;
    if( tgt_final->id == TYP_FUNCTION ) {
        src_final = conv->wsrc.final;
        if( src_final->id == TYP_FUNCTION ) {
            if( !functionsAreIdentical( src_final, tgt_final ) ) {
                conv->rank->rank = OV_RANK_NO_MATCH;
            }
            return TRUE;
        }
    }
    return FALSE;
}


static void rankRefMemFlags( FNOV_CONV *conv )
/********************************************/
{
    if( conv->wsrc.reference ) {
        FnovMemFlagsRank( conv->wsrc.refflag
                        , conv->wtgt.refflag
                        , conv->wsrc.refbase
                        , conv->wtgt.refbase
                        , conv->rank );
    } else {
        FnovMemFlagsRank( conv->wsrc.leadflag
                        , conv->wtgt.refflag
                        , conv->wsrc.leadbase
                        , conv->wtgt.refbase
                        , conv->rank );
    }
}

static bool toBoolRank( FNOV_CONV *conv )
/***************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
{
    type_id src_id;
    type_id tgt_id;

    tgt_id = conv->wtgt.basic->id;
    if( tgt_id == TYP_BOOL ) {
        src_id = conv->wsrc.basic->id;
        switch( src_id ) {
        case TYP_POINTER:
        case TYP_MEMBER_POINTER:
            conv->rank->rank = OV_RANK_STD_BOOL;
            conv->rank->u.no_ud.standard++;
            return TRUE;
            break;
        }
    }
    return FALSE;
}

static bool fnovScopeDerived( TYPE src, type_flag srcflags, TYPE tgt,
    type_flag tgtflags, FNOV_RANK *rank )
/*******************************************************************/
// if src is derived from tgt, return TRUE and set rank, else return FALSE
{
    SCOPE       src_scope;
    SCOPE       tgt_scope;
    bool        retn;

    retn = FALSE;
    src_scope = TypeScope( src );
    tgt_scope = TypeScope( tgt );
    if( src_scope != NULL && tgt_scope != NULL ) {
        // we should not worry about access when checking whether two
        // classes are related (04/02/96 AFS)
        if( ScopeDerived( src_scope, tgt_scope ) != DERIVED_NO ) {
            if( !FnovCvFlagsRank( srcflags, tgtflags, rank ) ) {
                // should this check mem flags too?
                rank->rank = OV_RANK_STD_CONV_DERIV;
                rank->u.no_ud.standard++;
                retn = TRUE;
            }
        }
    }
    return( retn );
}


static bool rankPtrToVoid( FNOV_CONV* conv )
/******************************************/
{
    type_id     src_basic_id;
    bool        retn;

    src_basic_id = conv->wsrc.basic->id;
    if( conv->wtgt.final == NULL
     || conv->wtgt.final->id != TYP_VOID ) {
        retn = FALSE;
    } else if( src_basic_id == TYP_POINTER ) {
        // check for tgt special void * that can't come from void *
        if( conv->wtgt.final->flag & TF1_STDOP_ARITH ) {
            conv->rank->rank = OV_RANK_NO_MATCH;
        } else {
            // if TF1_STDOP then this is not a conversion to void but an
            // exact match because the void * is a stand-in for all T*
            if( (conv->wtgt.final->flag & TF1_STDOP) == 0 ) {
                conv->rank->rank = OV_RANK_STD_CONV_VOID;
                conv->rank->u.no_ud.standard++;
            }
        }
        retn = TRUE;
    } else if( src_basic_id == TYP_FUNCTION ) {
        conv->rank->rank = OV_RANK_STD_CONV_VOID;
        conv->rank->u.no_ud.standard++;
        retn = TRUE;
    } else if( src_basic_id != TYP_CLASS ) {
        conv->rank->rank = OV_RANK_NO_MATCH;
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


static bool toPtrRank( FNOV_CONV *conv )
/**************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
{
    TYPE        src_basic;
    TYPE        tgt_basic, tgt_final;
    bool        retn;

    tgt_basic = conv->wtgt.basic;
    if( tgt_basic->id == TYP_POINTER ) {
        src_basic = conv->wsrc.basic;
        tgt_final = conv->wtgt.final;
        // check for NULL to pointer
        if( ( src_basic->id != TYP_POINTER )
          && fromConstZero( conv ) ) {
            retn = TRUE;
        // check for src pointer to void *
        } else if( rankPtrToVoid( conv ) ) {
            retn = TRUE;
        // check for src FUNCTION to tgt POINTER to FUNCTION
        } else if( src_basic->id == TYP_FUNCTION ) {
            if( tgt_final->id != TYP_FUNCTION ) {
                // already determined that functions are the same
                // and whether a pointer size conversion is needed
                conv->rank->rank = OV_RANK_NO_MATCH;
            }
            retn = TRUE;
        } else if( src_basic->id != TYP_CLASS ) {
            conv->rank->rank = OV_RANK_NO_MATCH;
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return( retn );
}


static bool toMbrPtrFromMbrPtrRank( FNOV_CONV *conv )
/***************************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
{
    TYPE        src_basic, tgt_basic, magic;
    bool        retn = FALSE;

    tgt_basic = conv->wtgt.basic;
    src_basic = conv->wsrc.basic;
    magic = TypeGetCache( TYPC_VOID_MEMBER_PTR );
    // check for all member pointers considered same
    // this is flagged with the magic member pointer
    if( ( src_basic == magic ) || ( tgt_basic == magic ) ) {
        conv->rank->rank = OV_RANK_SAME;
        conv->rank->u.no_ud.not_exact = 1;
        retn = TRUE;
    // check for NULL to member pointer
    } else if( fromConstZero( conv ) ) {
        retn = TRUE;
    // check for src MEMBER FUNCTION to tgt POINTER to MEMBER FUNCTION
    } else {
        if( TypesSameFnov( conv->wsrc.final, conv->wtgt.final ) ) {
            if( !TypesSameFnov( src_basic, tgt_basic ) ) {
                if( !fnovScopeDerived( MemberPtrClass( tgt_basic ),
                                       TF1_NULL,
                                       MemberPtrClass( src_basic ),
                                       TF1_NULL,
                                       conv->rank ) ) {
                    conv->rank->rank = OV_RANK_NO_MATCH;
                }
            }
        } else {
            conv->rank->rank = OV_RANK_NO_MATCH;
        }
        retn = TRUE;
    }
    return( retn );
}


static bool toMbrPtrRank( FNOV_CONV *conv )
/*****************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
{
    TYPE        src_basic, tgt_basic, tgt_final;
    bool        retn = FALSE;

    tgt_basic = conv->wtgt.basic;
    if( tgt_basic->id == TYP_MEMBER_POINTER ) {
        src_basic = conv->wsrc.basic;
        if( fromConstZero( conv ) ) {
            retn = TRUE;
        // check for src MEMBER FUNCTION to tgt POINTER to MEMBER FUNCTION
        } else if( src_basic->id == TYP_FUNCTION ) {
            tgt_final = conv->wtgt.final;
            if( tgt_final->id == TYP_FUNCTION ) {
                // TYP_FUNCTION -> TYP_MEMBPTR of TYP_FUNCTION
                conv->rank->rank = OV_RANK_NO_MATCH;
                // can't match a static member function or a
                // regular function to a member function prototype
            } else {
                // already determined that functions are the same
                // and whether a pointer size conversion is needed
                conv->rank->rank = OV_RANK_NO_MATCH;
            }
            retn = TRUE;
        } else if( src_basic->id != TYP_CLASS ) {
            conv->rank->rank = OV_RANK_NO_MATCH;
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return( retn );
}

static FNOV_COARSE_RANK fnovUdcLocate( FNOV_UDC_CONTROL control,
    FNOV_INTRNL_CONTROL ictl, FNOV_TYPE *wsrc, FNOV_TYPE *wtgt,
    bool *isctor, FNOV_LIST **pmatch, FNOV_RANK *rank,
    FNOV_DIAG *fnov_diag )
/**************************************************************/
// If direct
//    use direct initialization to perform conv
// else
//    use copy initialization to perform conv
// fill in fnov_diag if non-null
{
    TYPE                src_basic, tgt_basic, cl_type_src, cl_type_tgt;
    type_flag           srcflags;
    type_flag           tgtflags;
    FNOV_LIST           *udcfRank;
    FNOV_LIST           *ctorRank;
    derived_status      derived;

    fnov_diag = FnovInitDiag( fnov_diag );
    src_basic = wsrc->basic;
    tgt_basic = wtgt->basic;

    if( wsrc->reference ) {
        srcflags = wsrc->refflag;
    } else {
        srcflags = wsrc->leadflag;
    }
    if( wtgt->reference ) {
        tgtflags = wtgt->refflag;
    } else {
        tgtflags = wtgt->leadflag;
    }

    // see 8.5 [dcl.init]
    // get list of possible ctors for type tgt_basic
    cl_type_tgt = StructType( tgt_basic );
    ctorRank = NULL;
    if( ( control & FNOV_UDC_CTOR ) && NULL != cl_type_tgt ) {
        ctorRank = CtorFindList( wsrc->original, cl_type_tgt );
    }

    // only copy initialization involving source which is not derived
    // from target get to use UD conversion functions
    cl_type_src = StructType( src_basic );
    derived = TypeDerived( cl_type_src, cl_type_tgt );
    udcfRank = NULL;
    if( NULL != cl_type_src     // can't have UDCF unless src is a class
     && ( NULL == cl_type_tgt   // always allow UDCF to scalar tgt
          || ( derived == DERIVED_NO && ( control & FNOV_UDC_UDCF ) ) ) ) { // two classes, not derived
        // returns list of UDCF from ctype src_basic
        udcfRank = ScopeConversionList( cl_type_src->u.c.scope
                                      , srcflags & TF1_THIS_MASK
                                      , src_basic );
    }
    if( IsCopy( control ) && derived != DERIVED_NO ) {
        ictl |= FNOV_INTRNL_DERIVED; // copy and derived
    }
    // rank each function in each list of UDCs
    return( RankandResolveUDCsDiag( &ctorRank
                                  , &udcfRank
                                  , wsrc->original
                                  , wtgt->original
                                  , isctor
                                  , pmatch
                                  , control
                                  , ictl
                                  , wsrc->ptnode
                                  , rank
                                  , fnov_diag ) );
}

static FNOV_COARSE_RANK fnovUdcLocateRef( FNOV_UDC_CONTROL control,
    FNOV_INTRNL_CONTROL ictl, FNOV_TYPE *wsrc, FNOV_TYPE *wtgt,
    bool *isctor, FNOV_LIST **pmatch, FNOV_RANK *rank,
    FNOV_DIAG *fnov_diag )
/**************************************************************/
{
    bool                my_isctor;
    FNOV_LIST           *match;
    FNOV_COARSE_RANK    coarse;
    FNOV_DIAG           my_fnov_diag;
    bool                try_again;

    if( wtgt->reference ) {
        // see 8.5 [dcl.init] and 8.5.3 [dcl.init.ref]
        FnovInitDiag( &my_fnov_diag );
        coarse = fnovUdcLocate( FNOV_UDC_COPY
                              , ictl
                              , wsrc
                              , wtgt
                              , &my_isctor
                              , &match
                              , rank
                              , &my_fnov_diag );
        try_again = ( coarse == OV_RANK_UD_CONV && ( my_isctor ||
              NULL == TypeReference( SymFuncReturnType( match->sym ) ) ) );

        if( !try_again && coarse == OV_RANK_UD_CONV_AMBIG ) {
            if( my_isctor ) {
                try_again = TRUE;
            } else {
                FNOV_LIST* amb_list;    // - ambiguity list
                SYMBOL next;            // - next symbol
                bool found_ref = FALSE;
                for( amb_list = NULL; ; ) {
                    next = FnovGetAmbiguousEntry( &my_fnov_diag, &amb_list );
                    if( next == NULL ) break;
                    found_ref = ( NULL != TypeReference( SymFuncReturnType( next ) ) );
                    if( found_ref ) break;
                }
                if( !found_ref ) {
                    try_again = TRUE;
                }
            }
        }
        if( try_again ) {
            coarse = OV_RANK_NO_MATCH;
            if( rank != NULL ) {
                rank->rank = coarse;
            }
            if( TF1_CONST == ( wtgt->refflag & TF1_CV_MASK ) ) {
                bool found_non_ref = FALSE;
                TYPE tgt_type;
                FNOV_TYPE conv_class;
                tgt_type = StructType( wtgt->basic );
                if( NULL == tgt_type ) {
                    tgt_type = wtgt->basic;
                }
                DbgVerify( wtgt->basic == tgt_type, "Hypothesis failed" );
                initFNOV_TYPE( &conv_class, tgt_type, NULL );
                completeFNOV_TYPE( &conv_class );
                FnovFreeDiag( &my_fnov_diag );
                FnovListFree( &match );
                coarse = fnovUdcLocate( FNOV_UDC_CTOR | ( control & FNOV_UDC_USE_EXPLICIT )
                                      , ictl | FNOV_INTRNL_8_5_3_ANSI
                                      , wsrc
                                      , &conv_class
                                      , &my_isctor
                                      , &match
                                      , rank
                                      , &my_fnov_diag );

                // the following is an extension to take the UDCF
                // returning non-ref if nothing else exists
                if( coarse == OV_RANK_UD_CONV ) {
                    if( match->rankvector->rank != OV_RANK_UD_CONV_AMBIG ) {
                        // only really found a non-ref conversion if it
                        // works without ambiguity
                        found_non_ref = TRUE;
                    }
                }

                if( !found_non_ref && (ictl & FNOV_INTRNL_8_5_3_ANSI) == 0 ) {
                    FnovFreeDiag( &my_fnov_diag );
                    FnovListFree( &match );
                    coarse = fnovUdcLocate( FNOV_UDC_COPY
                                          , ictl
                                          , wsrc
                                          , wtgt
                                          , &my_isctor
                                          , &match
                                          , rank
                                          , &my_fnov_diag );
                }
            }
        }
        if( fnov_diag != NULL ) {
            *fnov_diag = my_fnov_diag;
        } else {
            FnovFreeDiag( &my_fnov_diag );
        }
        if( pmatch == NULL ) {
            FnovListFree( &match );
        } else {
            *pmatch = match;
        }
        if( isctor != NULL ) {
            *isctor = my_isctor;
        }
        return coarse;
    } else {
        return( fnovUdcLocate( control
                     , ictl
                     , wsrc
                     , wtgt
                     , isctor
                     , pmatch
                     , rank
                     , fnov_diag ) );
    }
}

FNOV_COARSE_RANK UdcLocateRef( FNOV_UDC_CONTROL control, TYPE src, TYPE tgt,
    PTREE *src_expr, bool *isctor, FNOV_LIST **pmatch, FNOV_DIAG *fnov_diag )
/***************************************************************************/
{
    FNOV_TYPE   wsrc, wtgt;

    initFNOV_TYPE( &wsrc, src, src_expr );
    initFNOV_TYPE( &wtgt, tgt, NULL );
    return( fnovUdcLocateRef( control
                         , FNOV_INTRNL_NONE
                         , &wsrc
                         , &wtgt
                         , isctor
                         , pmatch
                         , NULL
                         , fnov_diag ) );
}

FNOV_COARSE_RANK UdcLocate( FNOV_UDC_CONTROL control, TYPE src, TYPE tgt,
    PTREE *src_expr, bool *isctor, FNOV_LIST **pmatch, FNOV_DIAG *fnov_diag )
/***************************************************************************/
{
    FNOV_TYPE   wsrc, wtgt;
    initFNOV_TYPE( &wsrc, src, src_expr );
    initFNOV_TYPE( &wtgt, tgt, NULL );
    return( fnovUdcLocate( control
                         , FNOV_INTRNL_NONE
                         , &wsrc
                         , &wtgt
                         , isctor
                         , pmatch
                         , NULL
                         , fnov_diag ) );
}

static void toClsRank( FNOV_CONV *conv )
/**************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
// only type of conversions that need to be considered here are ctors
{
    FNOV_INTRNL_CONTROL ictl;

    if( conv->rank->control & FNC_EXCLUDE_UDCONV ) {
        conv->rank->rank = OV_RANK_NO_MATCH;
    } else {
        ictl = FNOV_INTRNL_ONCE_ONLY;
        if( conv->rank->control & FNC_RANKING_CTORS ) {
            ictl |= FNOV_INTRNL_EXCLUDE_UDCONV_PARAM;
        }
        if( conv->rank->control & FNC_STDOP_CV_VOID ) {
            ictl |= FNOV_INTRNL_STDOP_CV_VOID;
        }
        fnovUdcLocate( FNOV_UDC_COPY
                        , ictl
                        , &conv->wsrc
                        , &conv->wtgt
                        , NULL
                        , NULL
                        , conv->rank
                        , NULL );
    }
}

static void fromClsRank( FNOV_CONV *conv )
/*******************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
// only need to consider user defined conversions here
// trouble is, have to consider u-d conversions in base classes as well,
// unless they are hidden by the u-d conversions in the derived class
{
    FNOV_INTRNL_CONTROL ictl;

    if( conv->rank->control & FNC_EXCLUDE_UDCONV ) {
        conv->rank->rank = OV_RANK_NO_MATCH;
    } else {
        if( conv->wsrc.basic->u.c.info->has_udc ) {
            ictl = FNOV_INTRNL_ONCE_ONLY;
            if( conv->rank->control & FNC_RANKING_CTORS ) {
                ictl |= FNOV_INTRNL_EXCLUDE_UDCONV_PARAM;
            }
            if( conv->rank->control & FNC_STDOP_CV_VOID ) {
                ictl |= FNOV_INTRNL_STDOP_CV_VOID;
            }
            if( conv->rank->control & FNC_8_5_3_ANSI ) {
                ictl |= FNOV_INTRNL_8_5_3_ANSI;
            }
            fnovUdcLocateRef( FNOV_UDC_COPY
                            , ictl
                            , &conv->wsrc
                            , &conv->wtgt
                            , NULL
                            , NULL
                            , conv->rank
                            , NULL );
        } else {
            conv->rank->rank = OV_RANK_NO_MATCH;
        }
    }
}

static void clstoClsRank( FNOV_CONV *conv )
/*****************************************/
// return TRUE if conversion found or no conversion possible, else FALSE
// need to consider user defined conversions that go the the appropriate
// target class or a derived class of the target class
// also consider constructors that go to the target class
{
    TYPE                src_basic, tgt_basic;
    type_flag           srcflags, tgtflags;
    FNOV_INTRNL_CONTROL ictl;
    FNOV_RANK* rank = conv->rank;

    src_basic = conv->wsrc.basic;
    tgt_basic = conv->wtgt.basic;
    if( (rank->control & FNC_DISTINCT_CHECK) == 0 ) {
        src_basic = BindTemplateClass( src_basic, NULL, FALSE );
        tgt_basic = BindTemplateClass( tgt_basic, NULL, FALSE );
    }
    if( !TypesIdentical( src_basic, tgt_basic ) ) {
        if( rank->control & FNC_DISTINCT_CHECK ) {
            rank->rank = OV_RANK_NO_MATCH;
            return;
        }
        // can move from derived class to base class
        if( conv->wsrc.reference ) {
            srcflags = conv->wsrc.refflag;
        } else {
            srcflags = conv->wsrc.leadflag;
        }
        if( conv->wtgt.reference ) {
            tgtflags = conv->wtgt.refflag;
        } else {
            tgtflags = conv->wtgt.leadflag;
        }
        // derived to base is a standard conversion, which is the current
        // rank, so just return.  Any UDC would be ranked worse than the
        // to-base conversion, anyway
        if( !fnovScopeDerived( src_basic
                             , srcflags
                             , tgt_basic
                             , tgtflags
                             , rank ) ) {
            if( rank->control & FNC_EXCLUDE_UDCONV )  {
                rank->rank = OV_RANK_NO_MATCH;
            } else {
                // copy initialization
                ictl = FNOV_INTRNL_ONCE_ONLY;
                if( conv->rank->control & FNC_RANKING_CTORS ) {
                    ictl |= FNOV_INTRNL_EXCLUDE_UDCONV_PARAM;
                }
                if( conv->rank->control & FNC_8_5_3_ANSI ) {
                    ictl |= FNOV_INTRNL_8_5_3_ANSI;
                }
                if( conv->rank->control & FNC_STDOP_CV_VOID ) {
                    ictl |= FNOV_INTRNL_STDOP_CV_VOID;
                }
                fnovUdcLocateRef( FNOV_UDC_COPY
                                , ictl
                                , &conv->wsrc
                                , &conv->wtgt
                                , NULL
                                , NULL
                                , rank
                                , NULL );
            }
        }
    }
}


static void rankFuncToPtr(          // RANK: FUNCTION --> PTR
    FNOV_CONV *conv )               // - conversion information
{
    if( fromConstZero( conv )
     || rankPtrToVoid( conv ) ) {
        // all done
    } else if( conv->wtgt.final->id == TYP_FUNCTION ) {
        if( functionsAreIdentical( conv->wsrc.basic, conv->wtgt.final ) ) {
            conv->rank->rank = OV_RANK_EXACT;
            FnovMemFlagsRank( conv->wsrc.leadflag
                            , conv->wtgt.finalflag
                            , conv->wsrc.leadbase
                            , conv->wtgt.finalbase
                            , conv->rank );
        } else {
            conv->rank->rank = OV_RANK_NO_MATCH;
        }
    } else {
        conv->rank->rank = OV_RANK_NO_MATCH;
    }
}


static void rankPtrToFunc(          // RANK: PTR --> FUNCTION
    FNOV_CONV *conv )               // - conversion information
{
    if( conv->wsrc.final->id == TYP_FUNCTION ) {
        if( functionsAreIdentical( conv->wsrc.final, conv->wtgt.basic ) ) {
            conv->rank->rank = OV_RANK_EXACT;
            // no ranking of memory flags in original
        } else {
            conv->rank->rank = OV_RANK_NO_MATCH;
        }
    } else {
        conv->rank->rank = OV_RANK_NO_MATCH;
    }
}


static void rankPtrToPtr(           // RANK: PTR --> PTR
    FNOV_CONV *conv )               // - conversion information
{
    bool triv_fail;             // true only if triv conversion impossible
    if( exactRank( conv ) ) return;
    if( sameRankPtrToPtr( conv ) ) return;

    // asume rank is same
    conv->rank->rank = OV_RANK_SAME;
    conv->rank->u.no_ud.not_exact = 1;

    // check for trivial conversion or no std conversion possible
    triv_fail = FALSE;
    if( conv->wtgt.reference ) {
        triv_fail = trivialRank( conv );
    }
    triv_fail |= trivialRankPtrToPtr( conv );
    if( ! triv_fail ) {
        // try to find simple conversion
        bool retn = functionRank( conv );
        if( conv->rank->rank < OV_RANK_NO_MATCH ) {
            if( conv->wtgt.reference ) {
                rankRefMemFlags( conv );
            } else {
                FnovMemFlagsRank( conv->wsrc.finalflag
                                , conv->wtgt.finalflag
                                , conv->wsrc.finalbase
                                , conv->wtgt.finalbase
                                , conv->rank );
            }
        }
        if( retn ) return;
        if( ! fromConstZero( conv )
         && ! rankPtrToVoid( conv ) ) {
            // if the pointees are the same, okey dokey
            // if the pointees are different, check for class derivations
            if( !TypesSameFnov( conv->wsrc.final, conv->wtgt.final ) ) {
                if( ( conv->wsrc.final->id == TYP_CLASS )
                  &&( conv->wtgt.final->id == TYP_CLASS ) ) {
                    if( !fnovScopeDerived( conv->wsrc.final
                                         , conv->wsrc.finalflag
                                         , conv->wtgt.final
                                         , conv->wtgt.finalflag
                                         , conv->rank ) ) {
                        conv->rank->rank = OV_RANK_NO_MATCH;
                    }
                } else {
                    conv->rank->rank = OV_RANK_NO_MATCH;
                }
            }
        }
    }
}

static bool sameMemberPtrFun( FNOV_CONV *conv )
/*********************************************/
{
    TYPE        src_final;
    TYPE        tgt_final;

    tgt_final = conv->wtgt.final;
    if( tgt_final->id == TYP_FUNCTION ) {
        src_final = conv->wsrc.final;
        if( src_final->id == TYP_FUNCTION ) {
            if( !TypesIdentical( src_final, tgt_final ) ) {
                conv->rank->rank = OV_RANK_NO_MATCH;
                return TRUE;
            }
        }
    }
    return FALSE;
}


static void rankMbrPtrToMbrPtr(     // RANK: MEMBER PTR --> MEMBER PTR
    FNOV_CONV *conv )               // - conversion information
{
    bool triv;
    if( exactRank( conv ) ) return;
    if( sameMemberPtrFun( conv ) ) return;

    // asume rank is same
    conv->rank->rank = OV_RANK_SAME;
    conv->rank->u.no_ud.not_exact = 1;

    // check for trivial conversion or no std conversion possible
    triv = FALSE;
    if( conv->wtgt.reference ) {
        triv = trivialRank( conv );
    }
    triv |= FnovCvFlagsRank( conv->wsrc.finalflag
                           , conv->wtgt.finalflag
                           , conv->rank );
    if( ! triv ) {
        // try to find simple conversion
        if( conv->rank->rank < OV_RANK_NO_MATCH ) {
            if( conv->wtgt.reference ) {
                rankRefMemFlags( conv );
            } else {
                FnovMemFlagsRank( conv->wsrc.finalflag
                                , conv->wtgt.finalflag
                                , conv->wsrc.finalbase
                                , conv->wtgt.finalbase
                                , conv->rank );
            }
        }
        if( toMbrPtrFromMbrPtrRank( conv ) ) return;
    }

    conv->rank->rank = OV_RANK_NO_MATCH;
}


static FNOV_COARSE_RANK arithArithRank( type_id src, type_id tgt )
/****************************************************************/
// returns OV_RANK_EXACT     if identical
//    else OV_RANK_PROMOTION if integral promotion
//    else OV_RANK_STD_CONV  if standard conversion
{
    FNOV_COARSE_RANK result;
    type_id promo_id;

    if( src == tgt ) {
        result = OV_RANK_EXACT;
    } else {
        switch( src ) {
        case TYP_BOOL:
        case TYP_CHAR:
        case TYP_SCHAR:
        case TYP_UCHAR:
        case TYP_SSHORT:
#if TARGET_WIDE_CHAR < TARGET_INT
        case TYP_WCHAR:
#endif
#if TARGET_SHORT < TARGET_INT
        case TYP_USHORT:
#endif
            promo_id = TYP_SINT;
            break;
#if TARGET_WIDE_CHAR >= TARGET_INT
        case TYP_WCHAR:
        #define NEED_TEST
#endif
#if TARGET_SHORT >= TARGET_INT
        case TYP_USHORT:
        #define NEED_TEST
#endif
#ifdef NEED_TEST
            promo_id = TYP_UINT;
            break;
#undef NEED_TEST
#endif
        case TYP_FLOAT:
            promo_id = TYP_DOUBLE;
            break;
        default :
            promo_id = TYP_ERROR;   // used to force inequality
            break;
        }
        if( tgt == promo_id ) {
            result = OV_RANK_PROMOTION;
        } else {
            result = OV_RANK_STD_CONV;
        }
    }
    return( result );
}


static FNOV_COARSE_RANK enumArithRank( type_id src, type_id tgt )
/***************************************************************/
// returns OV_RANK_EXACT     if identical
//    else OV_RANK_PROMOTION if integral promotion
//    else OV_RANK_STD_CONV  if standard conversion
{
    FNOV_COARSE_RANK result;
    type_id promo_id;

    switch( src ) {
    case TYP_CHAR:
    case TYP_UCHAR:
    case TYP_SCHAR:
    case TYP_SSHORT:
#if TARGET_WIDE_CHAR < TARGET_INT
    case TYP_WCHAR:
#endif
#if TARGET_SHORT < TARGET_INT
    case TYP_USHORT:
#endif
        promo_id = TYP_SINT;
        break;
#if TARGET_WIDE_CHAR >= TARGET_INT
    case TYP_WCHAR:
    #define NEED_TEST
#endif
#if TARGET_SHORT >= TARGET_INT
    case TYP_USHORT:
    #define NEED_TEST
#endif
#ifdef NEED_TEST
        promo_id = TYP_UINT;
        break;
#undef NEED_TEST
#endif
    case TYP_SINT:
    case TYP_UINT:
    case TYP_SLONG:
    case TYP_ULONG:
    case TYP_SLONG64:
    case TYP_ULONG64:
        promo_id = src;
        break;
    default :
        promo_id = TYP_ERROR;   // used to force inequality
        break;
    }
    if( tgt == promo_id ) {
        result = OV_RANK_PROMOTION;
    } else {
        result = OV_RANK_STD_CONV;
    }
    return( result );
}


static void rankArithEnumToArith(   // RANK: Arith, Enum --> Arith
    FNOV_CONV *conv )               // - conversion information
{
    if( exactRank( conv ) ) return;

    // assume rank is same
    conv->rank->rank = OV_RANK_SAME;
    conv->rank->u.no_ud.not_exact = 1;

    // check for trivial conversion or no std conversion possible
    if( ! conv->wtgt.reference
     || ! rankTgtRefCvMem( conv ) ) {
        FNOV_COARSE_RANK    result;
        type_id tgt_id;
        TYPE src;
        tgt_id = conv->wtgt.basic->id;
        src = conv->wsrc.basic;
        if( conv->wsrc.basic->id == TYP_ENUM ) {
            result = enumArithRank( src->of->id, tgt_id );
        } else {
            result = arithArithRank( src->id, tgt_id );
        }
        switch( result ) {
        case OV_RANK_EXACT:
            break;
        case OV_RANK_PROMOTION:
            conv->rank->rank = result;
            conv->rank->u.no_ud.promotion = 1;
            break;
        case OV_RANK_STD_CONV:
            conv->rank->rank = result;
            conv->rank->u.no_ud.standard++;
            break;
        case OV_RANK_NO_MATCH:
            conv->rank->rank = result;
            break;
        }
        if( conv->wtgt.reference ) {
            rankRefMemFlags( conv );
        }
    }
}


static uint_8 rkdTable[RKD_MAX][RKD_MAX] = // ranking-combinations table
//      source operand
//      --------------
//         a           c           m       g
//         r   e       l   f   v           e
//     e   i   n   p   a   u   o   p   .   n
//     r   t   u   t   s   n   i   t   .   e
//     r   h   m   r   s   c   d   r   .   r
//                                               target operand
//                                               --------------
    {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1  // error
    ,  1, 10, 10, 14, 12,  1,  1,  0,  2,  0  // arithmetic
    ,  1,  1,  3,  1, 18,  1,  1,  1,  2,  0  // enumeration
    ,  1,  7,  7,  8, 17,  5,  1, 19,  2, 19  // pointer
    ,  1, 16, 16, 15, 11, 16,  1, 15,  2, 15  // class
    ,  1,  1,  1,  6, 18,  4,  1,  0,  2,  0  // function
    ,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1  // void
    ,  1,  7,  7,  1, 17, 19,  1,  9,  2, 19  // member pointer
    ,  1,  2,  2,  2,  2,  2,  1,  2,  3,  2  // ellipsis
    ,  1,  0,  0,  0, 18,  0,  1,  0,  2,  0  // generic
    };

#if 0

Argument ranking combinations (stl)

//               a                 c                 m           g
//               r     e           l     f     v                 e
//         e     i     n     p     a     u     o     p     .     n
//         r     t     u     t     s     n     i     t     .     e
//         r     h     m     r     s     c     d     r     .     r

   0:      0     0     0     0     0     0     0     0     0     0
   1:      0  5282    94  5841 15111  2088     0     0     0     3
   2:      0     0     0     0     0     0     0     0     0     0
   3:      0   837     8  5020  4759  1218     0     0     0     0
   4:      0   301     0  1585 14968     0     0     0     0     0
   5:      0     0     0     0     0     0     0     0     0     0
   6:      0     0     0     0     0     0     0     0     0     0
   7:      0     0     0     2   415     0     0     0     0     0
   8:      0     0     0     0     0     0     0     0     0     0
   9:      0    12     0   193   524     0     0     0     0   680

#endif

// values:
//  0 ==> do a general check
//  1 ==> OV_RANK_NO_MATCH
//  2 ==> OV_RANK_ELLIPSIS
//  3 ==> result determined by TypesSameFnov
//  4 ==> result determined by functionsIdentical
//  5 ==> rank func to ptr.
//  6 ==> rank ptr to func.
//  7 ==> OV_RANK_NO_MATCH unless src is 0 constant
//  8 ==> ptr to ptr
//  9 ==> m.ptr to m.ptr
// 10 ==> ( arith, enum ) --> arith
// 11 ==> class --> class
// 12 ==> class --> arith
// 14 ==> OV_RANK_NO_MATCH unless tgt is bool (then do general check)
// 15 ==> (ptr,m.ptr) --> class
// 16 ==> ? --> class (except 11,12,15)
// 17 ==> class --> (ptr, m.ptr)
// 18 ==> class --> ? (except 11,12,15,16,17)
// 19 ==> ? --> ( ptr, m.ptr ) except( 8, 9, 17 )

ExtraRptTable( type_combos, RKD_MAX, RKD_MAX ); // counts arg rankings

#ifndef NDEBUG
static int FNOV_infinite_ctr = 0;
#define INFINITY 10

#define FNOV_ARG_RANK my_fnov_arg_rank

void FNOV_ARG_RANK( TYPE src, TYPE tgt, PTREE *pt, FNOV_RANK *rank );

void FnovArgRank( TYPE src, TYPE tgt, PTREE *pt, FNOV_RANK *rank )
{
    DbgVerify( ++FNOV_infinite_ctr <= INFINITY, "Infinite Overload LOOP" );
    FNOV_ARG_RANK( src, tgt, pt, rank );
    DbgVerify( --FNOV_infinite_ctr >= 0, "Bad Overload Unwind" );

}
#else

#define FNOV_ARG_RANK FnovArgRank

#endif


void FNOV_ARG_RANK( TYPE src, TYPE tgt, PTREE *pt, FNOV_RANK *rank )
/*****************************************************************/
// compute rank of conversion required to go from src to tgt
{
    FNOV_CONV conv;             // - conversion data
    int rkd_index;              // - (RKD,RKD) for conversion
    RKD rkd_src, rkd_tgt;       // - ranking kinds
    bool triv;                  // - TRUE ==> trivial conversion

    if( src == tgt ) {
        rank->rank = OV_RANK_EXACT;
        return;
    }
    conv.rank = rank;
    rkd_src = initFNOV_TYPE( &conv.wsrc, src, pt );
    rkd_tgt = initFNOV_TYPE( &conv.wtgt, tgt, NULL );
    ExtraRptTabIncr( type_combos, rkd_tgt, rkd_src );
    rkd_index = rkdTable[ rkd_tgt ][ rkd_src ];
    switch( rkd_index ) {
      case 7 :
        if( fromConstZero( &conv ) ) return;
        // drops thru
      case 1 :
        conv.rank->rank = OV_RANK_NO_MATCH;
        return;
      case 2 :
        conv.rank->rank = OV_RANK_ELLIPSIS;
        return;
      case 3 :
        if( TypesSameFnov( conv.wsrc.basic, conv.wtgt.basic ) ) {
            // check for non-ranking reference or flag change (for distinctness)
            if( ( conv.wsrc.reference == conv.wtgt.reference )
              &&( conv.wsrc.leadflag  == conv.wtgt.leadflag )
              &&( conv.wsrc.refflag   == conv.wtgt.refflag ) ) {
                conv.rank->rank = OV_RANK_EXACT;
            } else {
                // assume rank is same
                conv.rank->rank = OV_RANK_SAME;
                conv.rank->u.no_ud.not_exact = 1;

                // check for trivial conversion
                if( conv.wtgt.reference ) {
                    if( ! rankTgtRefCvMem( &conv ) ) {
                        rankRefMemFlags( &conv );
                    }
                }
            }
        } else {
            conv.rank->rank = OV_RANK_NO_MATCH;
        }
        return;
      case 4 :
        if( functionsAreIdentical( conv.wsrc.basic, conv.wtgt.basic ) ) {
            conv.rank->rank = OV_RANK_EXACT;
        } else {
            conv.rank->rank = OV_RANK_NO_MATCH;
        }
        return;
      case 5 :
        completeFNOV_TYPE( &conv.wtgt );
        rankFuncToPtr( &conv );
        return;
      case 6 :
        completeFNOV_TYPE( &conv.wtgt );
        rankPtrToFunc( &conv );
        return;
      case 8 :
        completeFNOV_TYPE( &conv.wsrc );
        completeFNOV_TYPE( &conv.wtgt );
        rankPtrToPtr( &conv );
        return;
      case 9 :
        completeFNOV_TYPE( &conv.wsrc );
        completeFNOV_TYPE( &conv.wtgt );
        rankMbrPtrToMbrPtr( &conv );
        return;
      case 10 :
        rankArithEnumToArith( &conv );
        return;
      case 11:                          // CLASS --> CLASS
        if( exactRank( &conv ) ) return;

        // asume rank is same
        conv.rank->rank = OV_RANK_SAME;
        conv.rank->u.no_ud.not_exact = 1;

        // check for trivial conversion or no std conversion possible
        if( conv.wtgt.reference ) {
            rankTgtRefCvMem( &conv );
        }
        clstoClsRank( &conv );
        return;
      case 12:                          // CLASS --> ARITHMETIC
        // asume rank is same
        conv.rank->rank = OV_RANK_SAME;
        conv.rank->u.no_ud.not_exact = 1;

        // check for trivial conversion or no std conversion possible
        triv = FALSE;
        if( conv.wtgt.reference ) {
            triv = rankTgtRefCvMem( &conv );
        }
        if( ! triv ) {
            if( toBoolRank( &conv ) ) return;
        }
        // try to find user-defined conversion
        if( rank->control & FNC_DISTINCT_CHECK  ) {
            conv.rank->rank = OV_RANK_NO_MATCH;
        } else {
            fromClsRank( &conv );
        }
        return;
      case 13:                          // CLASS --> PTR
        completeFNOV_TYPE( &conv.wtgt );

        // asume rank is same
        conv.rank->rank = OV_RANK_SAME;
        conv.rank->u.no_ud.not_exact = 1;

        // check for trivial conversion or no std conversion possible
        triv = FALSE;
        if( conv.wtgt.reference ) {
            triv = rankTgtRefCvMem( &conv );
        }
        if( ! triv ) {
            if( toPtrRank( &conv ) ) return;
        }
        // try to find user-defined conversion
        if( rank->control & FNC_DISTINCT_CHECK  ) {
            conv.rank->rank = OV_RANK_NO_MATCH;
        } else {
            fromClsRank( &conv );
        }
        return;
      case 14:
        if( conv.wtgt.basic->id != TYP_BOOL ) {
            conv.rank->rank = OV_RANK_NO_MATCH;
            return;
        } else {
        // drops thru
      case 19 :
        completeFNOV_TYPE( &conv.wtgt );
        }
        // drops thru
      default :
        if( rkd_src == RKD_POINTER
         || rkd_src == RKD_MEMBPTR ) {
            completeFNOV_TYPE( &conv.wsrc );
        }
        if( rkd_src == rkd_tgt && exactRank( &conv ) ) return;

        // asume rank is same
        conv.rank->rank = OV_RANK_SAME;
        conv.rank->u.no_ud.not_exact = 1;

        // check for trivial conversion or no std conversion possible
        triv = FALSE;
        if( conv.wtgt.reference ) {
            triv = rankTgtRefCvMem( &conv );
        }
        switch( rkd_tgt ) {
          case RKD_ARITH :
            if( triv ) break;
            if( toBoolRank( &conv ) ) return;
            break;
          case RKD_POINTER :
            if( triv ) break;
            if( toPtrRank( &conv ) ) return;
            break;
          case RKD_MEMBPTR :
            if( triv ) break;
            if( toMbrPtrRank( &conv ) ) return;
            break;
        }
        // try to find user-defined conversion
        if( rank->control & FNC_DISTINCT_CHECK  ) {
            conv.rank->rank = OV_RANK_NO_MATCH;
        } else if( rkd_src == RKD_CLASS ) {
            fromClsRank( &conv );
        } else {
            conv.rank->rank = OV_RANK_NO_MATCH;
        }
        return;
      case 15 :                         // ???? --> class
        completeFNOV_TYPE( &conv.wsrc );
        // drops thru
      case 16 :
        // asume rank is same
        conv.rank->rank = OV_RANK_SAME;
        conv.rank->u.no_ud.not_exact = 1;

        // check for trivial conversion or no std conversion possible
        if( conv.wtgt.reference ) {
            rankTgtRefCvMem( &conv );
        }
        if( rank->control & FNC_DISTINCT_CHECK  ) {
            conv.rank->rank = OV_RANK_NO_MATCH;
        } else {
            toClsRank( &conv );
        }
        return;
      case 17 :                         // class --> ?
        completeFNOV_TYPE( &conv.wtgt );
        // drops thru
      case 18 :
        // asume rank is same
        conv.rank->rank = OV_RANK_SAME;
        conv.rank->u.no_ud.not_exact = 1;

        // check for trivial conversion or no std conversion possible
        triv = FALSE;
        if( conv.wtgt.reference ) {
            triv = rankTgtRefCvMem( &conv );
        }
        // try to find user-defined conversion
        if( rank->control & FNC_DISTINCT_CHECK  ) {
            conv.rank->rank = OV_RANK_NO_MATCH;
        } else {
            fromClsRank( &conv );
        }
        return;
    }
}


#ifdef XTRA_RPT

static void init(               // START OF RANK REPORTING
    INITFINI* defn )            // - definition
{
    static char const * const rankNames[] = {
        #define dfnRKD(a) #a
        dfnRKDs
        #undef dfnRKD
    };
    defn = defn;
    ExtraRptRegisterTab( "Argument ranking combinations"
                       , rankNames
                       , &type_combos[0][0]
                       , RKD_MAX
                       , RKD_MAX );
}


INITDEFN( rpt_rankings, init, InitFiniStub );

#endif

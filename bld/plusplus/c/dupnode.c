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
#include "cgfront.h"
#include "carve.h"
#include "ring.h"
#include "initdefs.h"
#ifndef NDEBUG
    #include <stdio.h>
    #include "pragdefn.h"
    #include "errdefns.h"
    #include "dbg.h"
    #include "toggle.h"
#endif

typedef struct promo PROMO;
struct promo                    // DUPLICATED EXPRESSION TO BE PROMOTED
{   PROMO *next;                // - next in ring
    PTREE dup_src;              // - first occurance of duplicate
    PTREE anc_src;              // - source ancestor going back to common
    PTREE anc_tgt;              // - target ancestor going back to common
    PROMO *container;           // - PROMO containing src/tgt
};

static carve_t carvePromo;      // carving control
static PROMO *promoRing;        // duplicates to be promoted (common found)
static PROMO *promoHdr;         // duplicates to be promoted (common search)
static TOKEN_LOCN* exprPosition;// position for expression



#ifndef NDEBUG

static void printPromo
    ( PROMO const * promo
    , char const* text )
{
    printf( "  %x: ( %x %x %x %x ) %s\n"
          , promo
          , promo->dup_src
          , promo->anc_src
          , promo->anc_tgt
          , promo->container
          , text );
}

static void printPromoRing
    ( PROMO const * ring
    , char const* text )
{
    PROMO const * curr;
    if( NULL != ring ) {
        printf( "%s\n\n", text );
        RingIterBeg( ring, curr ) {
            printPromo( curr, "" );
        } RingIterEnd( curr )
        printf( "\n" );
    }
}

#endif


static PTREE convertToTemp(     // CONVERT NODE TO FETCH OF TEMPORARY
    PTREE node,                 // - node to be converted
    SYMBOL temp )               // - the temporary
{
    node->op = PT_UNARY;
    node->cgop = CO_FETCH;
    node->type = temp->sym_type;
    node->u.subtree[0] = MakeNodeSymbol( temp );
    node->u.subtree[1] = NULL;
    node->flags &= ~ PTF_DUP_VISITED;
    return node;
}


// In order to avoid relocation of pointers to nodes, the tree update must
// preserve the location of the original nodes: anc_src, dup_src
//
static PTREE dupPromote(        // PROMOTE A DUPLICATE
    PROMO *promo )              // - promotion entry
{
    PTREE anc;                  // - common ancestor
    PTREE dup;                  // - duplicated expression
    PTREE node;                 // - new node
    PTREE dup1;                 // - duplication source
    PTREE dup2;                 // - duplication target
    SYMBOL temp;                // - temporary
    TYPE type;                  // - type of temporary

    dup1 = promo->dup_src;
    dup2 = dup1->u.dup.node;
    dup = dup2->u.subtree[0];
    type = NodeType( dup );
    temp = TemporaryAlloc( type );
    temp->sym_type = type;
    convertToTemp( dup1, temp );
    convertToTemp( dup2, temp );
    dup = NodeBinary( NULL == TypeReference( type ) ? CO_EQUAL : CO_EQUAL_REF
                    , MakeNodeSymbol( temp )
                    , dup );
    dup->type = type;
    dup->flags = PTF_LVALUE | PTF_SIDE_EFF | PTF_MEANINGFUL;
    anc = promo->anc_src;
    anc->flags &= ~PTF_DUP_VISITED;
    node = PTreeAlloc();
    *node = *anc;
    anc->op = PT_BINARY;
    anc->cgop = CO_COMMA;
    anc->decor = NULL;
    anc->u.subtree[0] = dup;
    anc->u.subtree[1] = node;
    return anc;
}


static void addPromo(           // ADD PROMOTION ENTRY
    PTREE dup )                 // - duplication node
{
    PROMO *promo;               // - current promotion
    PROMO *srch;                // - current promotion during search
    PTREE partner;              // - partner node

    promo = CarveAlloc( carvePromo );
    promo->dup_src = dup;
    promo->anc_src = dup;
    promo->container = NULL;
    partner = dup->u.dup.node;
    promo->anc_tgt = partner;
    partner->flags |= PTF_DUP_VISITED;
    dup->flags |= PTF_DUP_VISITED;
    // The following forces contained promotions to be assigned higher
    // in the tree by delaying the transfer to the promoRing ring of a PROMO
    // which has a non-NULL container.
    RingIterBeg( promoHdr, srch ) {
        if( NULL == srch->container ) {
            if( dup == srch->anc_src
             || partner == srch->anc_tgt ) {
                srch->container = promo;
            }
        }
    } RingIterEnd( srch )
    RingPush( &promoHdr, promo );
}


// note: keeping previous and using RingPruneWithPrev has been tested to
//       be slower than not keeping the previous and using RingPrune
//
static void setAncestor(        // SET ANCESTRY FOR CURRENT NODE
    PTREE zap_left,             // - parent (left)
    PTREE zap_right,            // - parent (right)
    PTREE node )                // - child node (or NULL)
{
    PROMO *promo;               // - current promotion
    PROMO *srch;                // - search promotion

    if( node != NULL ) {
        RingIterBegSafe( promoHdr, promo ) {
            PTREE common = promo->anc_src;
            if( node == common ) {
                common = zap_left;
                promo->anc_src = zap_left;
            }
            if( node == promo->anc_tgt ) {
                promo->anc_tgt = zap_right;
            }
            if( common == promo->anc_tgt
             && promo->container == NULL
             && ( ( common->op != PT_BINARY )   // use inline code for
                ||( common->cgop != CO_COLON    // efficiency
                 && common->cgop != CO_LIST )
                )
              ) {
                RingPrune( &promoHdr, promo );
#if 1
                RingAppend( &promoRing, promo );
#else
                RingPush( &promoRing, promo );
#endif
                // note that nodes with container set are further on in
                // the promoHdr ring than the PROMO pointed at by the
                // container field
                RingIterBeg( promoHdr, srch ) {
                    if( srch->container == promo ) {
                        srch->container = NULL;
                    }
                } RingIterEnd( srch )
#ifndef NDEBUG
            if( PragDbgToggle.dump_dups ) {
                printf( "Node %x ", zap_left );
                printPromo( promo, "-- added to promotion ring" );
            }
#endif
            }
        } RingIterEndSafe( promo )
    }
}


static PTREE dupProc(           // PROCESS NODE IN TREE
    PTREE dup )                 // - tree to be processed
{
#if 1
    // don't include defarg trees
    if(( dup->flags & PTF_DEFARG_COPY ) == 0 ) {
        SRCFILE dup_src_file = dup->locn.src_file;
        if( dup_src_file != NULL ) {
            SRCFILE curr_src_file = exprPosition->src_file;
            if( curr_src_file == NULL ) {
                TokenLocnAssign( *exprPosition, dup->locn );
            } else if( curr_src_file == dup_src_file ) {
                if( dup->locn.line < exprPosition->line ) {
                    TokenLocnAssign( *exprPosition, dup->locn );
                }
            } else {
                // not first locn and not same source file
                // better be a defarg copy!
                DbgAssert( dup->flags & PTF_DEFARG_COPY );
                DbgNever();
            }
        }
    }
#else
    // AFS: if src-file changed (due to defarg being spliced in), line #
    // would change wildly
    if(( exprPosition->src_file == NULL )||( dup->locn.line < exprPosition->line ) ) {
        *exprPosition = dup->locn;
    }
#endif
    if( promoHdr == NULL ) {
        if( dup->op == PT_DUP_EXPR ) {
            addPromo( dup );
        }
    } else {
        switch( dup->op ) {
          case PT_BINARY :
            setAncestor( dup, dup, dup->u.subtree[1] );
            // drops thru
          case PT_UNARY :
            setAncestor( dup, dup, dup->u.subtree[0] );
            break;
          case PT_DUP_EXPR :
            setAncestor( dup, dup->u.dup.node, dup->u.subtree[0] );
            addPromo( dup );
            break;
        }
    }
#ifndef NDEBUG
    if( PragDbgToggle.dump_dups
     && NULL != promoHdr ) {
        printf( "Node %x ", dup );
        printPromoRing( promoHdr, "Duplicates Ring" );
    }
#endif
    return dup;
}


PTREE NodePromoteDups(          // PROMOTE/REMOVE DUPLICATE NODES
    PTREE dup,                  // - tree to be processed
    TOKEN_LOCN* locn )          // - expression location (this expression)
{
    PROMO *promo;               // - current promotion

#ifndef NDEBUG
    DbgAssert( dup != NULL );
    if( PragDbgToggle.dump_dups ) {
        printf( "Original Tree\n\n" );
        DumpPTree( dup );
    }
#endif
    exprPosition = locn;
    locn->src_file = NULL;
    promoHdr = NULL;
    promoRing = NULL;
    dup = PTreeTraversePostfix( dup, &dupProc );
    DbgVerify( promoHdr == NULL, "NodePromoteDups -- common nodes not found" );
#ifndef NDEBUG
    if( PragDbgToggle.dump_dups ) {
        printPromoRing( promoRing, "Promotion Ring" );
        printf( "Original Tree\n\n" );
        DbgPrintPTREE( dup );
    }
#endif
    RingIterBegSafe( promoRing, promo ) {
        dupPromote( promo );
#ifndef NDEBUG
        if( PragDbgToggle.dump_dups ) {
            printf( "promo[%x]: ", promo );
            printPromo( promo, "Unduplicated Tree\n" );
            DbgPrintPTREE( dup );
        }
#endif
        CarveFree( carvePromo, promo );
    } RingIterEndSafe( promo )
    return dup;
}


static void init(               // INITIALIZATION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    carvePromo = CarveCreate( sizeof( PROMO ), 16 );
}


static void fini(               // COMPLETION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carvePromo );
}


INITDEFN( node_promo, init, fini )

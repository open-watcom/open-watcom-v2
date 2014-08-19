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
#include "cdopt.h"
#include "cgswitch.h"
#include "fnovload.h"
#include "ring.h"
#include "vstk.h"
#include "initdefs.h"
#include "pcheader.h"
#include "rtngen.h"
#include "context.h"
#include "stats.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif

#define TIS_DEFS                /* types in input stack     */ \
  TIS_DEF( DT_INPUT  )          /* - dtor, unprocessed      */ \
, TIS_DEF( DT_STR    )          /* - dtor, stretching       */ \
, TIS_DEF( DT_NO_OPT )          /* - dtor, no optimization  */ \
, TIS_DEF( DT_OPT    )          /* - dtor, optimized ok     */


#define CDOPT_DEFS              /* types of optimization    */ \
  CDO_DEF( CDOPT_DTOR  )        /* - dtor                   */ \
, CDO_DEF( CDOPT_CTOR  )        /* - default ctor           */ \
, CDO_DEF( CDOPT_OPREQ )        /* - operator =             */

typedef enum {                  // CDOPT_TYPE -- type of optimization
    #define CDO_DEF(a) a
    CDOPT_DEFS
    #undef CDO_DEF
,   MAX_CDOPT_TYPE
} CDOPT_TYPE;

typedef enum {                  // TIS -- types in input stack
    #define DT_DEF(a) a
    DT_DEFS
    #undef DT_DEF
,   MAX_TIS_TYPE
} TIS;


struct cdopt_cache              // CDOPT_CACHE -- cache from CLASSINFO
{   CD_DESCR* cdopt_ctor;       // - NULL or descriptor for default ctor
    CD_DESCR* cdopt_dtor;       // - NULL or descriptor for dctor
    CD_DESCR* cdopt_opeq;       // - NULL or descriptor for operator =
};


typedef struct memb_vfuns       MEMB_VFUNS;
typedef struct cl_elem          CL_ELEM;
typedef struct acc_fun          ACC_FUN;


struct acc_fun                  // ACC_SYM -- function accessed directly
{   ACC_FUN* next;              // - next symbol
    SYMBOL fun;                 // - function accessed
};

struct cd_descr                 // CD_DESCR -- ctor/dtor description
{   CD_DESCR*       next;       // - next in cache ring
    const MEMB_VFUNS* mfuns;    // - member functions
    TYPE        orig_type;      // - originating class type
    CL_ELEM*    elements;       // - significant elements
    ACC_FUN*    accessed;       // - functions accessed
    CDOPT_TYPE  opt;            // - type of optimization
    unsigned    err_occurred:1; // - TRUE ==> error detected during lookup
    unsigned    has_vbt     :1; // - TRUE ==> has virtual base table
    unsigned    has_vft     :1; // - TRUE ==> has virtual function table
    unsigned    has_acc     :1; // - TRUE ==> has functions accessed
    unsigned    chk_acc     :1; // - TRUE ==> accesses have been checked
    PAD_UNSIGNED
};

struct cl_elem                  // CL_ELEM -- significant class element
{   CL_ELEM* next;              // - next in ring
    TYPE cltype;                // - class/array type
    SYMBOL cdtor;               // - CTOR,DTOR,OP=
    SYMBOL sym;                 // - symbol, when data element
    CD_DESCR* descr;            // - description of class for element
    target_offset_t offset;     // - offset
    TOB otype;                  // - type of object
    unsigned    elim_intermed:1;// - intermediate function can be eliminated
    unsigned    cannot_define:1;// - function could not be defined
    unsigned    must_call:1;    // - item must be ctored,dtored,assigned
    PAD_UNSIGNED
};

typedef struct stkin STKIN;
struct stkin                    // STKIN -- stack (input) entry
{   CD_DESCR* info;             // - information for class being expanded
    CL_ELEM* elem;              // - element being expanded
};

typedef struct                  // CDOPT_DEFN : defines CDOPT type
{   MEMB_VFUNS const* vfuns;    // - virtual functions
    CDOPT_TYPE otype;           // - type of CDOPT
    unsigned :0;                // alignment
} CDOPT_DEFN;

typedef struct                  // CL_EXPAND -- expansion information
{   CD_DESCR* info;             // - class description
    STKIN* source;              // - source item
} CL_EXPAND;

struct memb_vfuns               // VFT for CD_DESCR
{   bool (*basePushable)        // - base class pushable ?
        ( BASE_CLASS* );        // - - base class
    bool (*membPushable)        // - member pushable ?
        ( SYMBOL );             // - - symbol for member
    SYMBOL (*find)              // - find symbol for type
        ( CL_ELEM* );           // - - type
    bool (*optimizable)         // - optimizable function ?
        ( CL_ELEM* );           // - - class entry
    bool (*include)             // - include function after expansion ?
        ( CL_ELEM* );           // - - class entry
    SYMBOL (*array_cdtor)       // - get array cdtor
        ( SYMBOL );             // - - array member symbol
    void (*schedule_acc_chk)    // - schedule access check
        ( TYPE );               // - - type for function
    SYMBOL (*array_acc)         // - get symbol for array access
        ( TYPE );               // - - array symbol
#ifdef XTRA_RPT
    void (*rpt_desc)            // - report: descriptor defined
        ( void );
    void (*elem_proc)           // - report: element processed
        ( void );
    void (*elem_kept)           // - report: element kept
        ( void );
#endif
};

typedef struct                  // CL_ITER: iteration info. per class
{   CD_DESCR    *info;          // - class information
    CL_ELEM     *elem;          // - current element
    TOB comp_otype;             // - TOB for component being expanded
} CL_ITER;

struct cdopt_iter               // CDOPT_ITER: iterator for traversals
{   CD_DESCR    *info;          // - original class information
    VSTK_CTL    stack;          // - stack of elements
    BASE_CLASS  *vbase;         // - NULL or virtual base
    target_offset_t off_comp;   // - offset of component being expanded
    target_offset_t off_elem;   // - offset of element (exact)
    target_offset_t off_vbase;  // - offset of virtual base
    TITER       orig_otype;     // - object type of original component
    unsigned    gened_comp  :1; // - TRUE ==> gen'ed component element
    unsigned    at_end      :1; // - TRUE ==> processing TITER_NONE at end
    PAD_UNSIGNED
};

static carve_t carveCD_DESCR;       // carving control: descriptors
static carve_t carveCL_ELEM;        // carving control: elements
static carve_t carveCDOPT_ITER;     // carving control: iterators
static carve_t carveACC_FUN;        // carving control: functions accessed
static carve_t carveCDOPT_CACHE;    // carving control: caching

static CDOPT_CACHE allDescriptors;  // all allocated descriptors
static VSTK_CTL stackSTKIN;         // stack of elements

ExtraRptCtr( ctr_ctor_desc );   // # ctor descriptors
ExtraRptCtr( ctr_ctor_elem );   // # ctor elements processed
ExtraRptCtr( ctr_ctor_kept );   // # ctor elements kept
ExtraRptCtr( ctr_dtor_desc );   // # dtor descriptors
ExtraRptCtr( ctr_dtor_elem );   // # dtor elements processed
ExtraRptCtr( ctr_dtor_kept );   // # dtor elements kept
ExtraRptCtr( ctr_opeq_desc );   // # opeq descriptors
ExtraRptCtr( ctr_opeq_elem );   // # opeq elements processed
ExtraRptCtr( ctr_opeq_kept );   // # opeq elements kept
ExtraRptCtr( ctr_caches    );   // # caches

#ifndef NDEBUG

#include "dbg.h"
#include "toggle.h"


static const char *tob_names[] = {
    #define TOB_DEF(a) # a
    TOB_DEFS
    #undef TOB_DEF
};


static const char *cdopt_names[] = {
    #define CDO_DEF(a) # a
    CDOPT_DEFS
    #undef CDO_DEF
};


static const char *tis_names[] = {
    #define TIS_DEF(a) # a
    TIS_DEFS
    #undef TIS_DEF
};


static const char *titer_names[] = {
    #define TITER_DEF(a) # a
    TITER_DEFS
    #undef TITER_DEF
};


char* __fmt_TOB( TOB tob )      // FORMAT TOB
{
    return ( tob >= MAX_TOB_DEF ) ? "*** BAD TOB ***" : tob_names[ tob ];
}


char* __fmt_CDOPT_TYPE( CDOPT_TYPE cdot )
{
    return ( cdot >= MAX_CDOPT_TYPE )
           ? "*** BAD CDOPT ***"
           : cdopt_names[ cdot ];
}


char* __fmt_TIS( TIS tis )
{
    return ( tis >= MAX_TIS_TYPE ) ? "*** BAD TIS ***" : tis_names[ tis ];
}


char* __fmt_TITER( TITER val )
{
    return ( val >= MAX_TITER_DEF )
           ? "*** BAD TITER ***"
           : titer_names[ val ];
}


void DumpCdoptIter(             // DUMP ITERATOR
    CDOPT_ITER* iter,           // - iterator
    const char* text1,          // - and some text
    const char* text2 )         // - and some text
{
    if( PragDbgToggle.cdopt ) {
        printf( "CDOPT_ITER[%p] info(%p) orig_otype(%s) %s %s\n"
                "    offsets: comp(%x) elem(%x) vbase(%p) gened_comp(%x) at_end(%x)\n"
              , iter
              , iter->info
              , __fmt_TITER( iter->orig_otype )
              , text1
              , text2
              , iter->off_comp
              , iter->off_elem
              , iter->vbase
              , iter->gened_comp
              , iter->at_end
              );
    }
}


void DumpCdoptClElem(           // DUMP A CLASS ELEMENT
    CL_ELEM* elem )             // - the element
{
    if( elem != NULL ) {
        VBUF vbuf1;
        VBUF vbuf2;

        printf( "  CL_ELEM[%p]: next(%p) type(%p) offset(%x) type(%s)\n"
                "       descr(%p) cdtor(%s) sym(%s)\n"
                "       elim_intermed(%d) cannot_define(%d) must_call(%d)\n"
              , elem
              , elem->next
              , elem->cltype
              , elem->offset
              , __fmt_TOB( elem->otype )
              , elem->descr
              , DbgSymNameFull( elem->cdtor, &vbuf1 )
              , DbgSymNameFull( elem->sym, &vbuf2 )
              , elem->elim_intermed
              , elem->cannot_define
              , elem->must_call
              );
        VbufFree( &vbuf1 );
        VbufFree( &vbuf2 );
    }
}


void DumpCdoptInfo(             // DUMP CD_DESCR
    CD_DESCR *info )            // - control information
{
    CL_ELEM* elem;              // - current element
    ACC_FUN* af;                // - current function access

    if( PragDbgToggle.cdopt ) {
        VBUF vbuf;

        printf( "CD_DESCR[%p]: type(%p) opt(%s) elements(%p)\n"
                "              accessed(%p) vft(%d) vbt(%d)\n"
              , info
              , info->orig_type
              , __fmt_CDOPT_TYPE( info->opt )
              , info->elements
              , info->accessed
              , info->has_vft
              , info->has_vbt );
        DumpFullType( info->orig_type );
        RingIterBeg( info->elements, elem ) {
            DumpCdoptClElem( elem );
        } RingIterEnd( elem )
        VbufInit( &vbuf );
        RingIterBeg( info->accessed, af ) {
            printf( "       Accessed: %s\n", DbgSymNameFull( af->fun, &vbuf ) );
        } RingIterEnd( af )
        VbufFree( &vbuf );
    }
}

void DumpCdoptIn(               // DUMP INPUT STACK ENTRY
    STKIN *inp,                 // - input stack
    const char* text )          // - descriptive text
{
    if( PragDbgToggle.cdopt ) {
        printf( "STKIN[%p]: info(%p) elem(%p) %s\n"
              , inp
              , inp->info
              , inp->elem
              , text );
        DumpCdoptClElem( inp->elem );
    }
}


void DumpClIter(                // DUMP STACK ENTRY
    CL_ITER* exp,               // - stack entry
    const char* text )          // - and some text
{
    if( PragDbgToggle.cdopt ) {
        printf( "CL_ITER[%p]: info(%p) elem(%p) comp_otype(%s) %s\n"
              , exp
              , exp->info
              , exp->elem
              , __fmt_TOB( exp->comp_otype )
              , text
              );
        DumpCdoptClElem( exp->elem );
    }
}


static void dumpRing(           // DUMP A RING
    CD_DESCR* cache )           // - ring to be dumped
{
    CD_DESCR* dump;             // - current descriptor

    RingIterBeg( cache, dump ) {
        DumpCdoptInfo( dump );
    } RingIterEnd( dump )
}


void DumpCdoptCaches(           // DUMP CDOPT CACHES
    void )
{
    int saved = PragDbgToggle.cdopt;

    PragDbgToggle.cdopt = 1;
    dumpRing( allDescriptors.cdopt_ctor );
    dumpRing( allDescriptors.cdopt_dtor );
    dumpRing( allDescriptors.cdopt_opeq );
    PragDbgToggle.cdopt = saved;
}




#else

#define DumpCdoptInfo(a)
#define DumpCdoptIter(a,b,c)
#define DumpCdoptClElem(a)
#define DumpCdoptIn(a,b)
#define DumpClIter(a,b)

#endif

//--------------------------------------------------------------------
// ACC_FUN support
//--------------------------------------------------------------------

static void addAccFun(          // ADD A FUNCTION ACCESS
    CD_DESCR* descr,            // - descriptor
    SYMBOL fun )                // - the function
{
    ACC_FUN* curr;              // - current access
    ACC_FUN* af;                // - access for function

    if( fun != NULL ) {
        if( ArrayType( fun->sym_type ) != NULL ) {
            fun = (*descr->mfuns->array_acc)
                    ( ArrayBaseType( fun->sym_type ) );
        }
    }
    if( fun != NULL ) {
        af = NULL;
        RingIterBeg( descr->accessed, curr ) {
            if( fun == curr->fun ) {
                af = curr;
                break;
            }
        } RingIterEnd( curr )
        if( af == NULL ) {
            af = RingCarveAlloc( carveACC_FUN, &descr->accessed );
            af->fun = fun;
            descr->has_acc = TRUE;
        }
    }
}


void CDoptNoAccFun(             // INDICATE FUNCTIONS ACCESSED FOR CLASS
    CD_DESCR* descr )           // - descriptor for a class
{
    descr->chk_acc = TRUE;
}


void CDoptChkAccFun(            // CHECK FUNCTIONS ACCESS FOR CLASS
    CD_DESCR* descr )           // - descriptor for a class
{
    SCOPE old;                  // - old scope
    ACC_FUN* af;                // - accessed function
    NAME name;                  // - name of function
    SEARCH_RESULT* result;      // - search result for function

    if( ! descr->chk_acc ) {
        descr->chk_acc = TRUE;
        if( NULL != descr->accessed ) {
            old = GetCurrScope();
            SetCurrScope(TypeScope( descr->orig_type ));
            RingIterBeg( descr->accessed, af ) {
                name = af->fun->name->name;
                result = ScopeFindBaseMember( SymScope( af->fun ), name );
                ScopeCheckSymbol( result, af->fun );
                ScopeFreeResult( result );
            } RingIterEnd( af )
            SetCurrScope(old);
        }
    }
}


static void cdoptChkAccFunGen(  // CHECK ACCESS FOR CLASS FUNCTION
    SYMBOL fun,                 // - the class function
    CD_DESCR* descr )           // - descriptor for a class
{
    CtxFunction( fun );
    CDoptChkAccFun( descr );
}



//--------------------------------------------------------------------
// Traversals
//--------------------------------------------------------------------


static CL_ELEM *activeElement   // GET ACTIVE ELEMENT
    ( CDOPT_ITER *iter )        // - iterator
{
    CL_ITER* exp;               // - expansion information
    CL_ELEM* elem;              // - class information

    exp = VstkTop( &iter->stack );
    if( iter->at_end ) {
        exp = VstkNext( &iter->stack, exp );
    }
    elem = NULL;
    if( NULL != exp ) {
        elem = exp->elem;
    }
    return elem;
}


static CL_ITER* pushClElem(     // PUSH CLASS ELEMENT
    CD_DESCR* info,             // - descriptor
    CDOPT_ITER *iter,           // - iterator
    TOB object_type )           // - kind of expansion
{
    CL_ITER* exp;               // - expansion information

    exp = VstkPush( &iter->stack );
    exp->info = info;
    exp->elem = NULL;
    exp->comp_otype = object_type;
    DumpClIter( exp, "-- PUSHED" );
    return exp;
}


static CL_ITER* popClElem(      // POP CLASS ELEMENT
    CDOPT_ITER *iter )          // - iterator
{
    CL_ITER* exp;               // - expansion information

    exp = VstkPop( &iter->stack );
    DumpClIter( exp, "-- POPPED" );
    exp = VstkTop( &iter->stack );
    return exp;
}


CDOPT_ITER* CDoptIterBeg(       // START OF ITERATION
    CD_DESCR* info )            // - information for class
{
    CDOPT_ITER* iter;           // - iteration information

    iter = CarveAlloc( carveCDOPT_ITER );
    iter->info = info;
    iter->orig_otype = TITER_NONE;
    VstkOpen( &iter->stack, sizeof( CL_ITER ), 16 );
    pushClElem( iter->info, iter, TOB_MEMB );
    DumpCdoptIter( iter, "-- DEFINED", "" );
    return iter;
}


void CDoptIterEnd(              // COMPLETION OF ITERATION
    CDOPT_ITER* iter )          // - iteration data
{
    DumpCdoptIter( iter, "-- DESTROYED", "" );
    VstkClose( &iter->stack );
    CarveFree( carveCDOPT_ITER, iter );
}


TITER CDoptIterNextComp(        // GET NEXT COMPONENT
    CDOPT_ITER* iter )          // - iteration data
{
    TITER retn;                 // - type of next component
    CD_DESCR* info;             // - original info for class
    CL_ITER* exp;               // - expansion information
    CL_ELEM* elem;              // - class information

    iter->vbase = NULL;
    iter->off_vbase = 0;
    iter->off_elem = 0;
    iter->off_comp = 0;
    iter->gened_comp = FALSE;
    iter->at_end = FALSE;
    info = iter->info;
    retn = TITER_NONE;
    exp = VstkTop( &iter->stack );
    if( exp->elem != info->elements ) {
        if( exp->elem == NULL ) {
            elem = info->elements;
        } else {
            elem = exp->elem;
        }
        elem = elem->next;
        exp->elem = elem;
        iter->off_comp = elem->offset;
        if( elem->otype == TOB_ARRAY ) {
            retn = TITER_MEMB;
        } else if( elem->cdtor == NULL ) {
            retn = TITER_NAKED_DTOR;
        } else {
            switch( elem->otype ) {
              case TOB_VBASE :
                iter->off_vbase = elem->offset;
                iter->vbase = ScopeFindVBase( TypeScope( info->orig_type )
                                            , elem->cltype );
                retn = TITER_VBASE;
                break;
              case TOB_DBASE :
                retn = TITER_DBASE;
                break;
              case TOB_MEMB :
                retn = TITER_MEMB;
                break;
            }
        }
    }
    iter->orig_otype = retn;
    DumpCdoptIter( iter, "-- Next Comp: ", __fmt_TITER( retn ) );
    return retn;
}


TITER CDoptIterNextElement(     // GET NEXT ELEMENT ACTION FOR COMPONENT
    CDOPT_ITER* iter )          // - iteration data
{
    TITER retn;                 // - type of next component
    CD_DESCR* info;             // - original info for class
    CL_ITER* exp;               // - expansion information
    CL_ELEM* elem;              // - current element

    iter->at_end = FALSE;
    for( ; ; ) {
        exp = VstkTop( &iter->stack );
        elem = exp->elem;
        info = exp->info;
        if( iter->info == info && iter->gened_comp ) {
            iter->at_end = TRUE;
            retn = TITER_NONE;
            break;
        } else if( elem != NULL
                 && elem == info->elements
                 && iter->gened_comp ) {
            exp = popClElem( iter );
            iter->off_elem -= exp->elem->offset;
            continue;
        } else {
            if( iter->gened_comp ) {
                if( elem == NULL ) {
                    elem = info->elements;
                }
                elem = elem->next;
                exp->elem = elem;
            } else {
                iter->gened_comp = TRUE;
            }
            info = elem->descr;
#if 0
            if( elem->otype == TOB_VBASE ) {
                unsigned depth = VstkDimension( &iter->stack );
                switch( iter->otype ) {
                  case TOB_VBASE :
                  case TOB_DBASE :
                    if( depth == 0 ) break;
                    DumpCdoptIter( iter, "--NO VBASE FROM BASE", "" );
                    continue;
                  case TOB_MEMB :
                    if( depth == 1 ) break;
                    DumpCdoptIter( iter, "--NO VBASE FROM MEMB", "" );
                    continue;
                }
            }
#else
            if( TOB_VBASE == elem->otype
             && ( TOB_DBASE == exp->comp_otype
               || TOB_VBASE == exp->comp_otype ) ) {
                continue;
            }
#endif
            if( info == NULL
             || info->elements == NULL
             || elem->must_call ) {
                if( elem->otype == TOB_ARRAY ) {
                    retn = ( exp->comp_otype == TOB_VBASE )
                                ? TITER_ARRAY_VBASE : TITER_ARRAY_EXACT;
                } else if( elem->cdtor == NULL ) {
                    retn = TITER_NAKED_DTOR;
            #if 0
                } else {
                    retn = ( exp->comp_otype == TOB_VBASE )
                                ? TITER_CLASS_VBASE : TITER_CLASS_EXACT;
            #else
                } else if( elem->otype == TOB_VBASE ) {
                    retn = TITER_CLASS_VBASE;
                } else if( elem->otype == TOB_DBASE ) {
                    retn = TITER_CLASS_DBASE;
                } else {
                    retn = TITER_CLASS_EXACT;
            #endif
                }
                DumpCdoptIter( iter, "--PROCESS", __fmt_TITER( retn ) );
                break;
            } else {
                iter->off_elem += elem->offset;
                pushClElem( elem->descr, iter, elem->otype );
                continue;
            }
        }
    }
    DumpCdoptIter( iter, "-- Next Element: ", __fmt_TITER( retn ) );
    return retn;
}


#define isLevelZero( iter ) ( 0 == VstkDimension( &iter->stack ) )


DTC_KIND CDoptObjectKind        // DETERMINE DTC_... OBJECT KIND
    ( CDOPT_ITER* iter )        // - iteration data
{
    DTC_KIND retn = 0;          // - return value: DTC_...
    CL_ELEM* elem;              // - current element

    elem = activeElement( iter );
    switch( elem->otype ) {
      case TOB_ARRAY :
      case TOB_MEMB :
      case TOB_ORIG :
        retn = DTC_COMP_MEMB;
        break;
      case TOB_DBASE :
        retn = isLevelZero( iter ) ? DTC_COMP_DBASE : DTC_ACTUAL_DBASE;
        break;
      case TOB_VBASE :
        retn = isLevelZero( iter ) ? DTC_COMP_VBASE : DTC_ACTUAL_VBASE;
        break;
      DbgDefault( "CDoptObjectKind -- bad TOB_..." );
    }
    return( retn );
}


target_offset_t CDoptIterOffsetComp( // GET EXACT OFFSET OF CURRENT COMPONENT
    CDOPT_ITER* iter )          // - iteration data
{
    return iter->off_comp;
}


target_offset_t CDoptIterOffsetExact( // GET EXACT OFFSET OF CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
{
    CL_ITER* exp;               // - expansion information
    CL_ELEM* elem;              // - NULL (at component) or element
    target_offset_t offset;     // - offset of element/component

    offset = iter->off_elem;
    exp = VstkTop( &iter->stack );
    elem = exp->elem;
    if( elem != NULL ) {
        offset += elem->offset;
    }
    return offset;
}


SYMBOL CDoptIterFunction(       // GET FUNCTION SYMBOL FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
{
    CL_ITER* exp;               // - expansion information

    exp = VstkTop( &iter->stack );
    return exp->elem->cdtor;
}


#if 0
SYMBOL CDoptIterSymbol(         // GET SYMBOL FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
{
    CL_ITER* exp;               // - expansion information

    exp = VstkTop( &iter->stack );
    return exp->elem->sym;
}
#endif


bool CDoptIterExact(            // GET cdarg "exact" FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
{
    bool retn;                  // - return value: true ==> is eact
    CL_ITER* exp;               // - expansion information
    CL_ELEM* elem;              // - current element

    exp = VstkTop( &iter->stack );
    retn = FALSE;
    elem = exp->elem;
    if( NULL != elem ) {
        switch( elem->otype ) {
          case TOB_ARRAY :
          case TOB_MEMB :
          case TOB_ORIG :
            retn = TRUE;
            break;
        }
    }
    return retn;
}


TYPE CDoptIterType(             // GET TYPE FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
{
    TYPE type;                  // - type of element or component
    CL_ELEM* elem;              // - NULL (at component) or element
#if 0
    CL_ITER* exp;               // - expansion information

    exp = VstkTop( &iter->stack );
    elem = exp->elem;
    if( elem == NULL ) {
        type = exp->info->orig_type;
    } else {
        type = elem->cltype;
    }
    return type;
#else
    elem = activeElement( iter );
    type = NULL;
    if( NULL != elem ) {
        type = elem->cltype;
    }
    return type;
#endif
}


BASE_CLASS* CDoptIterVBase(     // GET VIRTUAL BASE CLASS FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
{
    return iter->vbase;
}


bool CDoptIterCannotDefine(     // CHECK FOR UNDEFINEABLE CTOR
    CDOPT_ITER* iter )          // - iteration data
{
    CL_ITER* exp;               // - expansion information
    CL_ELEM* elem;              // - NULL (at component) or element
    bool retn;                  // - TRUE ==> issue "cannot define error"

    exp = VstkTop( &iter->stack );
    elem = exp->elem;
    if( elem == NULL ) {
        retn = FALSE;
    } else {
        retn = elem->cannot_define;
    }
    return retn;
}


bool CDoptInlinedMember(        // DETERMINE IF INLINED MEMBER
    CDOPT_ITER* iter )          // - iteration data
{
    CL_ITER* exp;               // - expansion information
    bool retn;                  // - TRUE ==> inlined member
    SYMBOL sym;                 // - NULL or symbol for member

    exp = VstkTop( &iter->stack );
    sym = exp->elem->sym;
    if( sym == NULL ) {
        retn = FALSE;
    } else {
        retn = SymScope( sym ) != TypeScope( iter->info->orig_type );
    }
    return retn;
}


//--------------------------------------------------------------------
// Caching
//--------------------------------------------------------------------


static CD_DESCR** refCiPtr      // GET REF TO CD_DESCR PTR IN CLASSINFO
    ( TYPE cltype               // - class type
    , CDOPT_TYPE opt )          // - type of optimization
{
    CLASSINFO* ci;              // - class information
    CD_DESCR **retn = NULL;     // - ref[ ptr to descriptor ]
    CDOPT_CACHE* cache;         // - CDOPT cache for class information

    ci = cltype->u.c.info;
    cache = ci->cdopt_cache;
    if( NULL == cache ) {
        ExtraRptIncrementCtr( ctr_caches );
        cache = CarveAlloc( carveCDOPT_CACHE );
        ci->cdopt_cache = cache;
        cache->cdopt_ctor = NULL;
        cache->cdopt_dtor = NULL;
        cache->cdopt_opeq = NULL;
    }
    switch( opt ) {
      case CDOPT_CTOR :
        retn = &cache->cdopt_ctor;
        break;
      case CDOPT_DTOR :
        retn = &cache->cdopt_dtor;
        break;
      case CDOPT_OPREQ :
        retn = &cache->cdopt_opeq;
        break;
      DbgDefault( "refCiPtr -- bad opt" );
    }
    return retn;
}


static void cacheAdd(           // ADD TO A CACHE
    CD_DESCR* info )            // - to be added to a cache
{
    CD_DESCR** ring;            // - ring for type of descriptor

    *( refCiPtr( info->orig_type, info->opt ) ) = info;
    switch( info->opt ) {
      case CDOPT_CTOR :
        ring = &allDescriptors.cdopt_ctor;
        break;
      case CDOPT_DTOR :
        ring = &allDescriptors.cdopt_ctor;
        break;
      case CDOPT_OPREQ :
        ring = &allDescriptors.cdopt_ctor;
        break;
      DbgDefault( "cacheAdd -- bad opt" );
    }
    RingAppend( ring, info );
}


static CD_DESCR* cacheFind(     // FIND IN CACHE
    TYPE type,                  // - class type
    CDOPT_DEFN const* defn )    // - definition
{
    CD_DESCR* retn;             // - descriptor

    if( NULL == type ) {
        retn = NULL;
    } else {
        retn = *refCiPtr( type, defn->otype );
    }
    return retn;
}


static void descFree            // FREE A CDOPT DESCRIPTOR
    ( CD_DESCR* descr )         // - current descriptor
{
    if( NULL != descr ) {
        RingCarveFree( carveCL_ELEM, &descr->elements );
        RingCarveFree( carveACC_FUN, &descr->accessed );
        *( refCiPtr( descr->orig_type, descr->opt ) ) = NULL;
        CarveFree( carveCD_DESCR, descr );
    }
}


static void descrRingFree(      // FREE ALL IN A RING
    CD_DESCR** ring )           // - addr[ ring[ descriptors ] ]
{
    CD_DESCR* descr;            // - current descriptor

    RingIterBegSafe( *ring, descr ) {
        descFree( descr );
    } RingIterEndSafe( descr )
    *ring = NULL;
}


static void cacheEmpty(         // FREE DESCRIPTORS FROM A CACHE
    CDOPT_CACHE* cache )        // - cache entry
{
    descFree( cache->cdopt_ctor );
    descFree( cache->cdopt_dtor );
    descFree( cache->cdopt_opeq );
}


//-------------------------------------------------------------------------
// Virtual functions for DTOR processing
//-------------------------------------------------------------------------

static bool classCanBeDtorOpt(  // IS CLASS DTORABLE UNDER ANY CONDITIONS ?
    CLASSINFO *ci )             // - class info
{
    ci->dtor_user_code_checked = TRUE;
    return ! ( ( ci->has_dtor && ci->dtor_user_code )
              || ci->corrupted
             );
}


static bool dtorOptimizable(    // CAN DTOR BE OPTIMIZED UNDER ANY CONDITIONS?
    CL_ELEM* elem )             // - class element
{
    bool retn;                  // - FALSE ==> can never be optimized
    SYMBOL dtor;                // - the DTOR in question

    dtor = elem->cdtor;
    if( dtor == NULL ) {
        retn = TRUE;
    } else if( ! CompFlags.inline_functions ) {
        retn = FALSE;
    } else {
        retn = classCanBeDtorOpt( SymClassInfo( dtor ) );
    }
    return retn;
}


static SYMBOL dtorLocate(       // FIND DTOR FOR TYPE
    TYPE cltype )               // - class type
{
    cltype = StructType( cltype );
    return ( NULL == cltype ) ? NULL : RoDtorFindType( cltype );
}


static SYMBOL dtorFind(         // FIND DTOR FOR CLASS ELEMENT
    CL_ELEM* elem )             // - class element
{
    TYPE cltype;                // - type for class

    cltype = StructType( elem->cltype );
    return ( NULL == cltype ) ? NULL : RoDtorFindType( cltype );
}


static bool dtorBasePushable(   // TEST IF BASE CLASS FOR DTOR NEEDS PUSHING
    BASE_CLASS *bcl )           // - base class
{
    return TypeRequiresDtoring( bcl->type );
}


static bool dtorMembPushable(   // TEST IF MEMBER FOR DTOR NEEDS PUSHING
    SYMBOL sym )                // - symbol for dtor
{
    return SymIsThisDataMember( sym ) && SymRequiresDtoring( sym );
}


static SYMBOL dtorArraySym(     // GET ARRAY DTOR SYMBOL
    SYMBOL arr )                // - array symbol
{
    return dtorLocate( arr->sym_type );
}


static bool dtorInclude(        // TEST IF DTOR NEEDS TO BE INCLUDED
    CL_ELEM* elem )             // - the element
{
    bool retn;                  // - TRUE ==> include anyway
    CD_DESCR* info;             // - class infomation

    info = elem->descr;
    if( info->has_vft ) {
        retn = TRUE;
    } else if( info->elements == NULL ) {
        retn = FALSE;
    } else {
        elem->elim_intermed = TRUE;
        retn = TRUE;
    }
    return retn;
}


static SYMBOL dtorArrayAcc(     // GET ARRAY-ACCESS SYMBOL
    TYPE type )                 // - array base type
{
    return RoDtorFindType( type );
}


static void defDtorSchAccChk(   // SCHEDULE ACCESS CHECKING
    TYPE type )                 // - class type
{
    RtnGenAddType( RGTYPE_CdoptAccDtor, type );
}


void RtnGenCallBackCdoptAccDtor(// CHECK ACCESS FROM DTOR, IF REQ'D
    TYPE type )                 // - type for DTOR
{
    CLASSINFO* ci = TypeClassInfo( type );
    if( ci->dtor_defined ) {
        cdoptChkAccFunGen( dtorLocate( type ), CDoptDtorBuild( type ) );
    }
}


#ifdef XTRA_RPT


static void dtorRptDescr(       // RPT: descriptor defined
    void )
{
    ExtraRptIncrementCtr( ctr_dtor_desc );
}


static void dtorRptElProc(      // RPT: element processed
    void )
{
    ExtraRptIncrementCtr( ctr_dtor_elem );
}


static void dtorRptElKept(      // RPT: element kept
    void )
{
    ExtraRptIncrementCtr( ctr_dtor_kept );
}


#endif


//-------------------------------------------------------------------------
// Virtual functions for Default CTOR processing
//-------------------------------------------------------------------------

static bool classCanBeCtorOpt(  // CAN CLASS BE DEF-CTOR OPTIMIZED
    CLASSINFO *ci )             // - class info for ctor
{
    ci->ctor_user_code_checked = TRUE;
    return ! ( ( ci->has_ctor && ci->ctor_user_code )
            || ci->corrupted
            || ( ! CompFlags.inline_functions )
             );
}


static bool defCtorOptSym(      // CAN DEF.CTOR BE OPTIMIZED UNDER ANY CONDITIONS?
    SYMBOL ctor )               // - the CTOR in question
{
    bool retn;                  // - FALSE ==> can never be optimized

    if( ctor == NULL ) {
        retn = TRUE;
    } else {
        retn = classCanBeCtorOpt( SymClassInfo( ctor ) );
    }
    return retn;
}


static bool defCtorOptimizable( // CAN DEF.CTOR TYPE BE OPTIMIZED UNDER ANY CONDITIONS?
    CL_ELEM* elem )             // - class element
{
    bool retn;                  // - TRUE ==> might be optimizable

    if( TypeRequiresCtoring( elem->cltype ) ) {
        retn = defCtorOptSym( elem->cdtor );
    } else {
        retn = FALSE;
    }
    return retn;
}


static SYMBOL getDefedCtor(     // FIND DEFINED DEFAULT CTOR
    TYPE cltype )               // - class type
{
    FNOV_RESULT fnov_status;    // - overload status
    SYMBOL ctor;                // - CTOR for type
    SEARCH_RESULT *result;      // - lookup result
    NAME name;                  // - name for lookup
    SCOPE scope;                // - scope for lookup
    arg_list arglist;           // - dummy arguments list

    InitArgList( &arglist );
    cltype = StructType( cltype );
    if( cltype == NULL ) {
        return( NULL );
    }
    scope = cltype->u.c.scope;
    name = CppConstructorName();
    result = ScopeContainsMember( scope, name );
    if( result == NULL ) {
        ctor = NULL;
    } else {
        ctor = result->sym_name->name_syms;
        fnov_status = FuncOverloaded( &ctor, result, ctor, &arglist, NULL );
        if( fnov_status == FNOV_NO_MATCH ) {
            ctor = NULL;
        }
        ScopeFreeResult( result );
    }
    return ctor;
}


static SYMBOL defCtorLocate(    // FIND DEF.CTOR FOR TYPE
    TYPE cltype )               // - class type
{
    SYMBOL ctor;                // - CTOR for type

    ctor = getDefedCtor( cltype );
    if( ctor == NULL ) {
        ScopeGenAccessSet( cltype );
        ClassDefaultCtorDefine( cltype, &ctor );
        ScopeGenAccessReset();
    }
    return( ctor );
}


static SYMBOL defCtorFind(      // FIND DEF.CTOR FOR TYPE
    CL_ELEM* elem )
{
    SYMBOL ctor;                // - default CTOR
    TYPE cltype;                // - class type for CTOR

    cltype = StructType( elem->cltype );
    ctor = defCtorLocate( cltype );
    if( NULL == ctor && NULL != cltype ) {
        elem->cannot_define = TRUE;
    }
    return ctor;
}


static bool defCtorBasePushable(// TEST IF BASE CLASS PUSHABLE FOR DEF.CTOR
    BASE_CLASS *bcl )           // - base class
{
    return TypeRequiresCtoring( bcl->type )
        || TypeRequiresDtoring( bcl->type );
}


static bool defCtorMembPushable(// TEST IF MEMBER PUSHABLE FOR DEF.CTOR
    SYMBOL sym )                // - symbol for ctor
{
    return SymIsThisDataMember( sym )
        && ( SymRequiresCtoring( sym ) || SymRequiresDtoring( sym ) );
}


static bool defCtorInclude(     // TEST IF CTOR REQUIRED TO BE INCLUDED
    CL_ELEM* elem )             // - the element
{
    bool retn;                  // - TRUE ==> include anyway
    CD_DESCR* info;             // - class infomation

    info = elem->descr;
    if( ( info->has_vft || info->has_vbt )
      &&( elem->otype == TOB_MEMB ) ) {
        elem->must_call = TRUE;
        retn = TRUE;
    } else if( info->elements == NULL ) {
        retn = FALSE;
    } else {
        retn = TRUE;
    }
    return retn;
}


static SYMBOL defCtorArrayAcc(  // GET ARRAY-ACCESS SYMBOL
    TYPE type )                 // - array base type
{
    return defCtorLocate( type );
}


static void defCtorSchAccChk(   // SCHEDULE ACCESS CHECKING
    TYPE type )                 // - class type
{
    RtnGenAddType( RGTYPE_CdoptAccCtor, type );
}


void RtnGenCallBackCdoptAccCtor(// CHECK ACCESS FROM CTOR, IF REQ'D
    TYPE type )                 // - type for CTOR
{
    CLASSINFO* ci = TypeClassInfo( type );
    if( ci->ctor_defined ) {
        cdoptChkAccFunGen( defCtorLocate( type ), CDoptDefCtorBuild( type ) );
    }
}


#ifdef XTRA_RPT


static void defCtorRptDescr(    // RPT: descriptor defined
    void )
{
    ExtraRptIncrementCtr( ctr_ctor_desc );
}


static void defCtorRptElProc(   // RPT: element processed
    void )
{
    ExtraRptIncrementCtr( ctr_ctor_elem );
}


static void defCtorRptElKept(   // RPT: element kept
    void )
{
    ExtraRptIncrementCtr( ctr_ctor_kept );
}


#endif


//-------------------------------------------------------------------------
// Virtual functions for Default Assignment generation
//-------------------------------------------------------------------------

static bool defAssForType(      // CAN OP= TYPE BE OPTIMIZED AWAY FOR A CLASS
    CLASSINFO *ci )             // - class info for type
{
    ci->assign_user_code_checked = TRUE;
    return ! ( ci->has_def_opeq
            || ci->const_ref
            || ci->corrupted
             );
}


static bool defAssOptimizable(  // CAN OP= BE OPTIMIZED AWAY AT ALL
    CL_ELEM* elem )             // - class element
{
    bool retn;                  // - FALSE ==> cannot be optimized
    SYMBOL assop;               // - the op= in question

    assop = elem->cdtor;
    if( assop == NULL ) {
        retn = FALSE;
    } else {
        retn = defAssForType( SymClassInfo( assop ) );
    }
    return retn;
}


static SYMBOL defAssFind(       // GET DEFAULT OP= (OR NULL IF SCALAR )
    CL_ELEM *elem )             // - class element
{
    TYPE type;                  // - type
    SYMBOL opeq;                // - default operator= or NULL

    type = StructType( elem->cltype );
    if( type == NULL ) {
        opeq = NULL;
    } else {
        opeq = ClassDefaultOpEq( type, type );
    }
    return opeq;
}


static bool defAssBasePushable( // TEST IF BASE CLASS PUSHABLE FOR OP=
    BASE_CLASS *bcl )           // - base class
{
    bcl = bcl;
    return TRUE;
}


static bool defAssMembPushable( // TEST IF MEMBER PUSHABLE FOR OP=
    SYMBOL sym )                // - symbol for ctor
{
    sym = sym;
    return TRUE;
}


static bool defAssInclude(      // TEST IF OP= MUST BE INCLUDED
    CL_ELEM* elem )             // - the element
{
    bool retn;                  // - TRUE ==> include anyway
    CD_DESCR* info;             // - class infomation

    info = elem->descr;
    if( info == NULL ) {
        retn = TRUE;
    } else if( info->elements == NULL ) {
        retn = FALSE;
    } else {
        retn = TRUE;
    }
    return retn;
}


static SYMBOL defAssArrayAcc(   // GET ARRAY-ACCESS SYMBOL
    TYPE type )                 // - array base type
{
    SYMBOL opeq;                // - symbol to be used

    if( NULL == StructType( type ) ) {
        opeq = NULL;
    } else {
        opeq = ClassDefaultOpEq( type, type );
    }
    return opeq;
}


static void defAssSchAccChk(    // SCHEDULE ACCESS CHECKING
    TYPE type )                 // - class type
{
    RtnGenAddType( RGTYPE_CdoptAccOpEq, type );
}


void RtnGenCallBackCdoptAccOpEq(// CHECK ACCESS FROM OP=, IF REQ'D
    TYPE type )                 // - type for DTOR
{
    CLASSINFO* ci = TypeClassInfo( type );
    if( ci->assign_defined ) {
        CD_DESCR* descr = CDoptDefOpeqBuild( type );
        cdoptChkAccFunGen( ClassDefaultOpEq( type, descr->orig_type )
                         , descr );
    }
}


#ifdef XTRA_RPT


static void defAssRptDescr(     // RPT: descriptor defined
    void )
{
    ExtraRptIncrementCtr( ctr_opeq_desc );
}


static void defAssRptElProc(    // RPT: element processed
    void )
{
    ExtraRptIncrementCtr( ctr_opeq_elem );
}


static void defAssRptElKept(    // RPT: element kept
    void )
{
    ExtraRptIncrementCtr( ctr_opeq_kept );
}


#endif



//-------------------------------------------------------------------------
// General support
//-------------------------------------------------------------------------


static SYMBOL origArraySym(     // RETURN ORIGINAL ARRAY SYMBOL
    SYMBOL arr )                // - symbol
{
    return arr;
}


static void addInfoElement(     // ADD ELEMENT TO CD_DESCR
    CD_DESCR* info,             // - class infomation
    CL_ELEM* elem )             // - the element
{
#ifndef NDEBUG
    if( PragDbgToggle.cdopt ) {
        printf( "-- ADDED %p TO %p\n", elem, info );
    }
#endif
    if( info->opt == CDOPT_DTOR ) {
        RingAppend( &info->elements, elem );
    } else {
        RingInsert( &info->elements, elem, NULL );
    }
    addAccFun( info, elem->cdtor );
#ifdef XTRA_RPT
    (*info->mfuns->elem_kept)();
#endif
}


static STKIN* pushElement(      // PUSH AN ELEMENT
    TYPE type,                  // - element type
    target_offset_t offset,     // - element offset
    SYMBOL sym,                 // - data member or NULL
    CL_EXPAND* expansion,       // - expansion data
    TOB otype )                 // - type of object
{
    CL_ELEM* elem;              // - class element
    STKIN* stk;                 // - stack element
    CD_DESCR* info;             // - hosting descriptor
    const MEMB_VFUNS* mfuns;    // - member functions

    elem = CarveAlloc( carveCL_ELEM );
    elem->cltype = type;
    elem->offset = offset;
    elem->sym    = sym;
    elem->descr  = NULL;
    elem->otype  = otype;
    elem->next   = NULL;
    elem->elim_intermed = FALSE;
    elem->cannot_define = FALSE;
    elem->must_call     = FALSE;
    info = expansion->info;
    mfuns = info->mfuns;
    elem->cdtor  = ( otype == TOB_ARRAY )
                    ? (*mfuns->array_cdtor)( sym )
                    : (*mfuns->find)( elem );
    stk = VstkPush( &stackSTKIN );
    stk->info = expansion->info;
    stk->elem = elem;
    DumpCdoptIn( stk, "-- PUSHED" );
    return stk;
}


static void pushBase(           // PUSH MEMBER FUNCTION FOR A BASE
    BASE_CLASS *bcl,            // - base class
    CL_EXPAND* expansion,       // - expansion data
    TOB otype )                 // - type of object
{
    if( (*expansion->info->mfuns->basePushable)( bcl ) ) {
        pushElement( bcl->type
                   , bcl->delta
                   , NULL
                   , expansion
                   , otype );
    }
}


static void pushVbase(          // PUSH MEMBER FUNCTION FOR A VIRTUAL BASE
    BASE_CLASS *bcl,            // - base class
    void *_expansion )      // - expansion data
{
    CL_EXPAND* expansion = _expansion;
    pushBase( bcl, expansion, TOB_VBASE );
}


static void pushDbase(          // PUSH MEMBER FUNCTION FOR A DIRECT BASE
    BASE_CLASS *bcl,            // - base class
    void * _expansion )      // - expansion data
{
    CL_EXPAND* expansion = _expansion;
    pushBase( bcl, expansion, TOB_DBASE );
}


static void pushMember(         // PUSH MEMBER FUNCTION FOR MEMBER
    SYMBOL sym,                 // - symbol for member
    void * _expansion )      // - expansion data
{
    CL_EXPAND* expansion = _expansion;
    TYPE sym_type;
    TYPE array_type;
    TOB otype;

    if( (*expansion->info->mfuns->membPushable)( sym ) ) {
        sym_type = sym->sym_type;
        array_type = ArrayType( sym_type );
        otype = TOB_MEMB;
        if( array_type != NULL ) {
            otype = TOB_ARRAY;
            if( array_type->flag & TF1_ZERO_SIZE ) {
                DbgAssert( array_type->u.a.array_size == 1 );
                // zero-sized array members should not be ctor/dtor/opeq'd
                return;
            }
        }
        pushElement( sym_type, sym->u.member_offset, sym, expansion, otype );
    }
}


static CD_DESCR* cdoptExpandClass( // EXPAND A CLASS
    TYPE cltype,                // - class
    CDOPT_DEFN const *defn )    // - definition
{
    SCOPE scope;                // - scope for class
    CL_EXPAND expansion;        // - expansion data
    CD_DESCR* info;             // - class descriptor
    CLASSINFO *ci;              // - information for class

#ifdef XTRA_RPT
    ( *defn->vfuns->rpt_desc )();
#endif
    info = CarveAlloc( carveCD_DESCR );
    info->orig_type = cltype;
    info->mfuns = defn->vfuns;
    info->err_occurred = FALSE;
    info->has_acc = FALSE;
    info->chk_acc = FALSE;
    info->opt = defn->otype;
    info->elements = NULL;
    info->accessed = NULL;
    cacheAdd( info );
    ( *info->mfuns->schedule_acc_chk )( cltype );
    ci = TypeClassInfo( cltype );
    info->has_vft = ci->has_vfptr;
    info->has_vbt = ci->has_vbptr;
    DumpCdoptInfo( info );
    expansion.info = info;
    expansion.source = VstkTop( &stackSTKIN );
    scope = TypeScope( cltype );
    ScopeWalkVirtualBases( scope, &pushVbase, &expansion );
    ScopeWalkDirectBases( scope, &pushDbase, &expansion );
    ScopeWalkDataMembers( scope, &pushMember, &expansion );
    return info;
}


static CD_DESCR* cdoptBuildOrig(// BUILD FOR ORIGINATING FUNCTION
    TYPE cltype,                // - type for dtor
    CDOPT_DEFN const * defn )   // - definition
{
    CD_DESCR *info;             // - information structure: original
    CD_DESCR *descr;            // - information structure: current
    STKIN* stk;                 // - stacked element
    CL_ELEM* elem;              // - current element
    const MEMB_VFUNS* mfuns;    // - member functions

    cltype = StructType( cltype );
    info = cacheFind( cltype, defn );
    if( info == NULL ) {
        info = cdoptExpandClass( cltype, defn );
        mfuns = info->mfuns;
        for( ; ; ) {
            stk = VstkTop( &stackSTKIN );
            if( stk == NULL ) break;
            DumpCdoptIn( stk, "--PROCESS" );
            elem = stk->elem;
            if( elem->otype == TOB_ARRAY ) {
                elem->must_call = TRUE;
                addInfoElement( stk->info, elem );
                DumpCdoptIn( stk, "--POPPED" );
                VstkPop( &stackSTKIN );
            } else {
                TYPE elem_type;
                elem_type = StructType( elem->cltype );
                descr = elem->descr;
                if( descr == NULL ) {
#ifdef XTRA_RPT
                    (*mfuns->elem_proc)();
#endif
                    descr = cacheFind( elem_type, defn );
                    elem->descr = descr;
                }
                if( descr == NULL ) {
                    if( elem_type == NULL ) {
                        if( (*mfuns->include)( elem ) ) {
                            elem->must_call = TRUE;
                            addInfoElement( stk->info, elem );
                        }
                        DumpCdoptIn( stk, "--POPPED" );
                        VstkPop( &stackSTKIN );
                    } else {
                        elem->descr = cdoptExpandClass( elem_type, defn );
                    }
                } else {
                    if( elem->cannot_define ) {
                        addInfoElement( stk->info, elem );
                    } else if( ! (*mfuns->optimizable)( elem ) ) {
                        elem->must_call = TRUE;
                        addInfoElement( stk->info, elem );
                    } else if( (*mfuns->include)( elem ) ) {
                        addInfoElement( stk->info, elem );
                    } else {
                        addAccFun( stk->info, elem->cdtor );
                    }
                    DumpCdoptIn( stk, "--POPPED" );
                    VstkPop( &stackSTKIN );
                }
            }
        }
        DumpCdoptInfo( info );
    }
    return info;
}


static const MEMB_VFUNS mfDtor= // "MEMBER FUNCTIONS" FOR DTOR
{   dtorBasePushable
,   dtorMembPushable
,   dtorFind
,   dtorOptimizable
,   dtorInclude
,   dtorArraySym
,   defDtorSchAccChk
,   dtorArrayAcc
#ifdef XTRA_RPT
,   dtorRptDescr
,   dtorRptElProc
,   dtorRptElKept
#endif
};

static CDOPT_DEFN const dtor_defn =   // DEFINITION FOR DTOR
{   &mfDtor
,   CDOPT_DTOR
};


CD_DESCR* CDoptDtorBuild(       // BUILD LIST OF OBJECTS TO BE DTOR'D
    TYPE cltype )               // - type for dtor
{
    return cdoptBuildOrig( cltype, &dtor_defn );
}


static const MEMB_VFUNS mfDefCtor= // "MEMBER FUNCTIONS" FOR DEFAULT CTOR
{   defCtorBasePushable
,   defCtorMembPushable
,   defCtorFind
,   defCtorOptimizable
,   defCtorInclude
,   origArraySym
,   defCtorSchAccChk
,   defCtorArrayAcc
#ifdef XTRA_RPT
,   defCtorRptDescr
,   defCtorRptElProc
,   defCtorRptElKept
#endif
};


static CDOPT_DEFN ctor_defn =   // DEFINITION FOR DEFAULT CTOR
{   &mfDefCtor
,   CDOPT_CTOR
};


CD_DESCR* CDoptDefCtorBuild(    // BUILD LIST OF OBJECTS TO BE DEFAULT CTOR'ED
    TYPE cltype )               // - type for dtor
{
    return cdoptBuildOrig( cltype, &ctor_defn );
}


static const MEMB_VFUNS mfDefAss= // "MEMBER FUNCTIONS" FOR DEFAULT OP=
{   defAssBasePushable
,   defAssMembPushable
,   defAssFind
,   defAssOptimizable
,   defAssInclude
,   origArraySym
,   defAssSchAccChk
,   defAssArrayAcc
#ifdef XTRA_RPT
,   defAssRptDescr
,   defAssRptElProc
,   defAssRptElKept
#endif
};

static CDOPT_DEFN opeq_defn =   // DEFINITION FOR OP=
{   &mfDefAss
,   CDOPT_OPREQ
};


CD_DESCR* CDoptDefOpeqBuild(    // BUILD LIST OF OBJECTS TO BE DEFAULT OP='ED
    TYPE cltype )               // - type for dtor
{
    return cdoptBuildOrig( cltype, &opeq_defn );
}


bool CDoptErrorOccurred(        // TEST IF ERROR OCCURRED
    CD_DESCR* info )
{
    return info->err_occurred;
}


bool TypeHasDtorableObjects(    // TEST IF TYPE HAS DTORABLE SUB-OBJECTS
    TYPE type )                 // - a class type
{
    CD_DESCR* dtor_info;        // - DTOR information
    CDOPT_ITER* iter;           // - and an iterator for it
    bool retn;                  // - return: TRUE ==> really dtor it

    dtor_info = CDoptDtorBuild( type );
    if( CDoptErrorOccurred( dtor_info ) ) {
        retn = FALSE;
    } else {
        TITER comp_beg;
        iter = CDoptIterBeg( dtor_info );
        comp_beg = CDoptIterNextComp( iter );
        CDoptIterEnd( iter );
        if( TITER_NONE == comp_beg ) {
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    }
    return retn;
}


static bool typeDtorable(       // TEST IF TYPE REALLY NEEDS DTOR'ING
    TYPE type,                  // - declared type
    bool is_exact )             // - TRUE ==> exact type is known
{
    SYMBOL dtor;                // - destructor to be used
    bool retn;                  // - return: TRUE ==> really dtor it
    TYPE base_type;             // - base type

    if( type == NULL ) {
        retn = FALSE;
    } else {
        base_type = ArrayType( type );
        if( base_type != NULL ) {
            type = ArrayBaseType( base_type );
        }
        type = StructType( type );
        if( type == NULL ) {
            retn = FALSE;
        } else if( TypeRequiresDtoring( type ) ) {
            dtor = dtorLocate( type );
            if( ! is_exact && SymIsVirtual( dtor ) ) {
                retn = TRUE;
            } else if( classCanBeDtorOpt( TypeClassInfo( type ) ) ) {
                retn = TypeHasDtorableObjects( type );
            } else {
                retn = TRUE;
            }
        } else {
            retn = FALSE;
        }
    }
    return retn;
}


bool TypeReallyDtorable(        // TEST IF TYPE REALLY NEEDS DTOR'ING
    TYPE type )                 // - declared type
{
    return typeDtorable( type, FALSE );
}


bool TypeExactDtorable(         // TEST IF EXACT TYPE REALLY NEEDS DTOR'ING
    TYPE type )                 // - declared type
{
    return typeDtorable( type, TRUE );
}


bool TypeReallyDefCtorable(     // TEST IF TYPE REALLY NEEDS DEFAULT CTOR'ING
    TYPE type )                 // - declared type
{
    CD_DESCR* ctor_info;        // - DTOR information
    CDOPT_ITER* iter;           // - and an iterator for it
    bool retn;                  // - return: TRUE ==> really dtor it

    if( TypeRequiresCtoring( type ) ) {
        if( classCanBeCtorOpt( TypeClassInfo( type ) ) ) {
            ctor_info = CDoptDefCtorBuild( type );
            if( ctor_info->has_vbt || ctor_info->has_vft ) {
                retn = TRUE;
            } else if( CDoptErrorOccurred( ctor_info ) ) {
                retn = FALSE;
            } else {
                TITER comp_beg;
                iter = CDoptIterBeg( ctor_info );
                comp_beg = CDoptIterNextComp( iter );
                CDoptIterEnd( iter );
                if( TITER_NONE == comp_beg ) {
                    retn = FALSE;
                } else {
                    retn = TRUE;
                }
            }
        } else {
            retn = TRUE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


void CDoptBackEnd(              // START OF BACK-END PROCESSING
    void )
{
#ifndef NDEBUG
    if( PragDbgToggle.dump_cdopt ) {
        DumpCdoptCaches();
    }
#endif
    descrRingFree( &allDescriptors.cdopt_ctor );
    descrRingFree( &allDescriptors.cdopt_opeq );
}


static void cdoptInit(          // INITIALIZATION FOR CDOPT
    INITFINI* defn )            // - definition
{
    defn = defn;
    allDescriptors.cdopt_ctor = NULL;
    allDescriptors.cdopt_dtor = NULL;
    allDescriptors.cdopt_opeq = NULL;
    carveCD_DESCR = CarveCreate( sizeof( CD_DESCR ), 32 );
    carveCL_ELEM = CarveCreate( sizeof( CL_ELEM  ), 64 );
    carveCDOPT_ITER = CarveCreate( sizeof( CDOPT_ITER ), 16 );
    carveACC_FUN = CarveCreate( sizeof( ACC_FUN ), 64 );
    carveCDOPT_CACHE = CarveCreate( sizeof( CDOPT_CACHE ), 32 );
    VstkOpen( &stackSTKIN, sizeof( STKIN ), 16 );
    ExtraRptRegisterCtr( &ctr_ctor_desc, "CDopt CTOR descriptors" );
    ExtraRptRegisterCtr( &ctr_ctor_elem, "CDopt CTOR elements processed" );
    ExtraRptRegisterCtr( &ctr_ctor_kept, "CDopt CTOR elements kept" );
    ExtraRptRegisterCtr( &ctr_dtor_desc, "CDopt DTOR descriptors" );
    ExtraRptRegisterCtr( &ctr_dtor_elem, "CDopt DTOR elements processed" );
    ExtraRptRegisterCtr( &ctr_dtor_kept, "CDopt DTOR elements kept" );
    ExtraRptRegisterCtr( &ctr_opeq_desc, "CDopt Op = descriptors" );
    ExtraRptRegisterCtr( &ctr_opeq_elem, "CDopt Op = elements processed" );
    ExtraRptRegisterCtr( &ctr_opeq_kept, "CDopt Op = elements kept" );
    ExtraRptRegisterCtr( &ctr_caches,    "CDopt caches" );
}


static void cdoptFini(          // COMPLETION FOR CDOPT
    INITFINI* defn )            // - definition
{
    defn = defn;
    cacheEmpty( &allDescriptors );
    CarveDestroy( carveCD_DESCR );
    CarveDestroy( carveCL_ELEM );
    CarveDestroy( carveCDOPT_ITER );
    CarveDestroy( carveACC_FUN );
    CarveDestroy( carveCDOPT_CACHE );
    VstkClose( &stackSTKIN );
}


INITDEFN( cd_opt, cdoptInit, cdoptFini )

pch_status PCHWriteCdOptData( void )
{
    return( PCHCB_OK );
}

pch_status PCHReadCdOptData( void )
{
    allDescriptors.cdopt_ctor = NULL;
    allDescriptors.cdopt_dtor = NULL;
    allDescriptors.cdopt_opeq = NULL;
    carveCD_DESCR = CarveRestart( carveCD_DESCR );
    carveCL_ELEM = CarveRestart( carveCL_ELEM );
    carveCDOPT_ITER = CarveRestart( carveCDOPT_ITER );
    carveACC_FUN = CarveRestart( carveACC_FUN );
    carveCDOPT_CACHE = CarveRestart( carveCDOPT_CACHE );
    return( PCHCB_OK );
}

pch_status PCHInitCdOptData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniCdOptData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

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
#include "rtfuns.h"
#include "fnovload.h"
#include "dbg.h"
#include "cgback.h"
#include "extrf.h"
#include "cgaux.h"
#include "cgio.h"
#include "ring.h"
#include "vfun.h"
#include "initdefs.h"
#include "stats.h"


ExtraRptCtr( ctr_cgfiles );     // number of CGFILE lookups


typedef struct _ovfn OVFN;
struct _ovfn {                  // ORIGINATING VIRTUAL FUNCTION
    OVFN *next;                 // - next in ring
    SYMBOL vfun;                // - the virtual function
};


                                // static data
static carve_t carveOVFN;       // - memory control: originating vfun.s
static carve_t carveRingHead;   // - memory control: rings of vfun ref.s


static void extrefAddOvfn(      // ADD ORIGINATING VIRTUAL FUNCTION
    void *_ring,                // - functions to date
    SYMBOL latest )             // - latest
{
    OVFN **ring = _ring;        // - functions to date
    OVFN *ovfn;                 // - new entry

    ovfn = RingCarveAlloc( carveOVFN, ring );
    ovfn->vfun = latest;
}


static void extrefPruneOvfn(    // PRUNE ANY NON-ORIGINATING VIRT FUNC.S
    void *_ring,                // - functions to date
    SYMBOL latest )             // - latest
{
    OVFN **ring = _ring;        // - functions to date
    OVFN *ovfn;                 // - existing items
    SCOPE scope;                // - class scope

    scope = SymScope( latest );
    RingIterBegSafe( *ring, ovfn ) {
        if( ( latest == ovfn->vfun ) || ( ScopeDerived( SymScope( ovfn->vfun ), scope ) != DERIVED_NO ) ) {
            RingPrune( (void*)ring, ovfn );
            CarveFree( carveOVFN, ovfn );
            break;
        }
    } RingIterEndSafe( ovfn )
}


void *ExtrefImportType(         // GET NEXT IMPORT TYPE FOR A SYMBOL
    EXTRF *rinfo )              // - resolution information
{
    void *retn = NULL;          // - return type

    if( rinfo->type == EXTRF_DATA_WEAK ) {
        retn = (void*)IMPORT_IS_WEAK;
    } else if( ! CompFlags.virtual_stripping ) {
        retn = (void*)IMPORT_IS_LAZY;
        switch( rinfo->type ) {
          case EXTRF_FN_LAZY :
            retn = (void*)IMPORT_IS_LAZY;
            break;
          case EXTRF_FN_WEAK :
            retn = (void*)IMPORT_IS_WEAK;
            break;
          case EXTRF_VFN_CONDITIONAL :
          case EXTRF_PURE_VFN_CONDITIONAL :
            break;
          DbgDefault( "IMPORT_TYPE: funny type" );
        }
    } else {
        switch( rinfo->type ) {
          case EXTRF_FN_LAZY :
            retn = (void*)IMPORT_IS_LAZY;
            break;
          case EXTRF_FN_WEAK :
            retn = (void*)IMPORT_IS_WEAK;
            break;
          case EXTRF_VFN_CONDITIONAL :
            retn = (void*)IMPORT_IS_CONDITIONAL;
            break;
          case EXTRF_PURE_VFN_CONDITIONAL :
            retn = (void*)IMPORT_IS_CONDITIONAL_PURE;
            break;
          DbgDefault( "IMPORT_TYPE: funny type" );
        }
    }
    return retn;
}


static void extrefClassVisit(   // VISIT ANCESTRAL CLASS
    SYMBOL sym,                 // - original symbol
    SYMBOL csym,                // - corresponding class symbol
    void * _rinfo )              // - resolution information
{
    EXTRF *rinfo = _rinfo;
    sym = sym;
    extrefPruneOvfn( &rinfo->syms, csym );
    extrefAddOvfn( &rinfo->syms, csym );
}


static void virtualListBuild(   // BUILD VIRTUALS LIST FOR SYMBOL
    SYMBOL sym,                 // - symbol
    EXTRF *rinfo )              // - resolution information
{
    OVFN *curr;                 // - current entry

    rinfo->syms = NULL;
    rinfo->sym = sym;
    VfnAncestralWalk( sym, &extrefClassVisit, rinfo );
    RingIterBegSafe( rinfo->syms, curr ) {
        if( curr->vfun == sym ) {
            RingPrune( &rinfo->syms, curr );
            CarveFree( carveOVFN, curr );
            break;
        }
    } RingIterEndSafe( curr )
}


static SYMBOL extrefPopVfun(    // POP A VIRTUAL FUNCTION
    EXTRF *rinfo )              // - resolution information
{
    OVFN *top;                  // - top of stack
    SYMBOL retn;                // - next symbol

    top = RingPop( &rinfo->syms );
    if( top == NULL ) {
        retn = NULL;
    } else {
        retn = top->vfun;
        CarveFree( carveOVFN, top );
    }
    return retn;
}


void *ExtrefVirtualSymbol(      // GET NEXT DEPENDENCY SYMBOL
    EXTRF *rinfo )              // - resolution information
{
    SYMBOL retn;                // - next symbol

    retn = extrefPopVfun( rinfo );
    if( retn == rinfo->sym ) {
        retn = extrefPopVfun( rinfo );
    }
    return retn;
}

static bool pureSymCanBeUndefd( SYMBOL sym )
{
    if( SymIsPure( sym ) && ! SymIsDtor( sym ) ) {
        return( true );
    }
    return( false );
}

static bool symIsCtorDtor( SYMBOL sym )
{
    return( SymIsCtorOrDtor( sym )
        || SymIsThunkCtorDflt( sym )
        || SymIsThunkCtorCopy( sym )
        || SymIsThunkDtor( sym ) );
}

static void* resolveInitedSym(  // RESOLVE AN INITIALIZED SYMBOL
    SYMBOL sym,                 // - symbol
    EXTRF *rinfo )              // - resolution information
{
    void *retn;                 // - type of resolution
    CGFILE *cgfile;             // - virtual file

    ExtraRptIncrementCtr( ctr_cgfiles );
    cgfile = CgioLocateAnyFile( sym );
    if( cgfile != NULL && cgfile->u.s.refed ) {
        retn = NULL;
    } else {
        // function is initialized and inlined everywhere it is referenced
        // (i.e, out of line copy won't be generated)
        // two cases:
        //      (1) address is taken in debug info
        //      (2) normal virtual call to function in this module
        // in both of these cases we want a WEAK import
        rinfo->type = EXTRF_FN_WEAK;
        if( SymIsVirtual( sym ) ) {
            if( pureSymCanBeUndefd( sym ) ) {
                retn = RunTimeCallSymbol( RTF_PURE_VIRT );
            } else {
                retn = RunTimeCallSymbol( RTF_UNDEF_VFUN );
            }
        } else {
            if( symIsCtorDtor( sym ) ) {
                retn = RunTimeCallSymbol( RTF_UNDEF_CDTOR );
            } else {
                retn = RunTimeCallSymbol( RTF_UNDEF_MEMBER );
            }
        }
    }
    return retn;
}


void *ExtrefResolve(            // DETERMINE RESOLUTION FOR A SYMBOL
    SYMBOL sym,                 // - symbol
    EXTRF *rinfo )              // - resolution information
{
    void *retn;                 // - type of resolution

    if( SymIsStaticDataMember( sym ) ) {
        if( SymIsInitialized( sym ) || SymIsReferenced( sym ) ) {
            retn = NULL;
        } else {
            rinfo->type = EXTRF_DATA_WEAK;
            retn = RunTimeCallSymbol( RTF_UNDEF_DATA );
        }
    } else if( ! SymIsFunction( sym ) ) {
        retn = NULL;
    } else if( SymIsReferenced( sym ) ) {
        if( SymIsInitialized( sym ) ) {
            retn = resolveInitedSym( sym, rinfo );
        } else {
            retn = NULL;
        }
    } else if( SymIsVirtual( sym ) ) {
        if( ! CompFlags.virtual_stripping ) {
            if( pureSymCanBeUndefd( sym ) ) {
                rinfo->type = EXTRF_PURE_VFN_CONDITIONAL;
                retn = RunTimeCallSymbol( RTF_PURE_VIRT );
            } else {
                retn = NULL;
            }
        } else {
            // TODO!!!!!!!!!!!!!!
            // there are allocated memory blocks by virtualListBuild
            // which are not freed
            // 
            virtualListBuild( sym, rinfo );
            if( pureSymCanBeUndefd( sym ) ) {
                rinfo->type = EXTRF_PURE_VFN_CONDITIONAL;
                retn = RunTimeCallSymbol( RTF_PURE_VIRT );
            } else {
                if( SymInVft( sym ) ) {
                    rinfo->type = EXTRF_VFN_CONDITIONAL;
                } else {
                    rinfo->type = EXTRF_FN_WEAK;
                }
                retn = RunTimeCallSymbol( RTF_UNDEF_VFUN );
            }
        }
    } else {
        if( SymIsFuncMember( sym ) ) {
            if( SymAddrTaken( sym ) ) {
                if( symIsCtorDtor( sym ) ) {
#if 0
                    if( SymDbgAddrTaken( sym ) ) {
                        rinfo->type = EXTRF_FN_WEAK;
                    } else {
                        rinfo->type = EXTRF_FN_LAZY;
                    }
#else
                    rinfo->type = EXTRF_FN_WEAK;
#endif
                    retn = RunTimeCallSymbol( RTF_UNDEF_CDTOR );
                } else if( SymIsInitialized( sym ) ) {
                    // symbol is def'd but only ref'd from debugging info
                    retn = resolveInitedSym( sym, rinfo );
                } else {
                    rinfo->type = EXTRF_FN_WEAK;
                    retn = RunTimeCallSymbol( RTF_UNDEF_MEMBER );
                }
            } else {
                retn = NULL;
            }
        } else {
            retn = NULL;
        }
    }
    return retn;
}


static void extrefVfunRegister( // REGISTER AN ANCESTRAL FUNCTION
    SYMBOL sym,                 // - original symbol
    SYMBOL csym,                // - corresponding class symbol
    void * _orig )              // - ring of functions
{
    OVFN **orig = _orig;
    sym = sym;
    extrefPruneOvfn( orig, csym );
    extrefAddOvfn( orig, csym );
}

static OVFN **startVRing( void )
{
    OVFN** orig_funs;           // - originating functions

    orig_funs = CarveAlloc( carveRingHead );
    *orig_funs = NULL;
    return( orig_funs );
}


void *ExtrefVfunInfo(           // GET INFORMATION FOR VIRTUAL FUN. REFERENCE
    SYMBOL sym )                // - symbol used in indirect call
{
    OVFN** orig_funs;           // - originating functions

    orig_funs = NULL;
    if( sym->id == SC_VIRTUAL_FUNCTION ) {
        if( CompFlags.virtual_stripping ) {
            orig_funs = startVRing();
            VfnAncestralWalk( sym->u.virt_fun, &extrefVfunRegister, orig_funs );
        } else if( ! CompFlags.vfun_reference_done ) {
            CompFlags.vfun_reference_done = true;
            orig_funs = startVRing();
            extrefAddOvfn( orig_funs, NULL );
        }
    }
    CgBackFreeIndCall( sym );
    return orig_funs;
}


void *ExtrefVfunSym(            // GET ORIGNATING FUN. FOR VIRTUAL CALL
    void *funs )                // - originating functions
{
    OVFN *curr;                 // - next function OVFN
    void *retn;                 // - next function symbol
    OVFN** orig_funs;           // - originating functions

    orig_funs = (OVFN**)funs;
    curr = RingTop( orig_funs );
    if( curr == NULL ) {
        DbgNever();
        retn = NULL;
    } else {
        // ->vfun may be NULL
        retn = curr->vfun;
    }
    return retn;
}


void *ExtrefNextVfunSym(         // MOVE TO NEXT ORIGNATING FUN. FOR VIRTUAL CALL
    void *funs )                // - originating functions
{
    OVFN *curr;                 // - next function OVFN
    void *retn;                 // - next function symbol
    OVFN** orig_funs;           // - originating functions

    orig_funs = (OVFN**)funs;
    curr = RingPop( orig_funs );
    if( curr != NULL ) {
        CarveFree( carveOVFN, curr );
    }
    if( RingTop( orig_funs ) == NULL ) {
        // nothing left on stack; free head
        CarveFree( carveRingHead, orig_funs );
        retn = NULL;
    } else {
        retn = funs;
    }
    return retn;
}


static void extrefInit(         // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveOVFN = CarveCreate( sizeof( OVFN ), 32 );
    carveRingHead = CarveCreate( sizeof( OVFN* ), 8 );
    ExtraRptRegisterCtr( &ctr_cgfiles, "number of CGFILE lookups (extref)" );
}


static void extrefFini(         // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
// TODO!!!!!!!!!!!!!!
// there are allocated memory blocks by virtualListBuild
// which are not freed
// 
    DbgStmt( CarveVerifyAllGone( carveOVFN, "OVFN" ) );
    DbgStmt( CarveVerifyAllGone( carveRingHead, "OVFN* ring heads" ) );
    CarveDestroy( carveOVFN );
    CarveDestroy( carveRingHead );
}


INITDEFN( extref_86, extrefInit, extrefFini )

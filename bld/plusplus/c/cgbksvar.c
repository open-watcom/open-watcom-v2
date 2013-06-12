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
* Description:  Back-end support for state-variable version.
*
****************************************************************************/

#include "plusplus.h"

#include <float.h>

#include "cgfront.h"
#include "cgback.h"
#include "memmgr.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include <stdio.h>
    #include "dbg.h"
    #include "toggle.h"
    #include "pragdefn.h"
#endif

//**********************************************************************
// Static Data
//**********************************************************************

static carve_t carveSE_SYM_AUTO;    // allocations for SE_SYM_AUTO
static carve_t carveSE_SYM_STATIC;  // allocations for SE_SYM_STATIC
static carve_t carveSE_SUBOBJ;      // allocations for SE_SUBOBJ
static carve_t carveSE_TRY;         // allocations for SE_TRY
static carve_t carveSE_CATCH;       // allocations for SE_CATCH
static carve_t carveSE_FN_EXC;      // allocations for SE_FN_EXC
static carve_t carveSE_SET_SV;      // allocations for SE_SET_SV
static carve_t carveSE_TEST_FLAG;   // allocations for SE_TEST_FLAG
static carve_t carveSE_COMPONENT;   // allocations for SE_COMPONENT
static carve_t carveSE_ARRAY_INIT;  // allocations for SE_ARRAY_INIT
static carve_t carveSE_DLT_1;       // allocations for SE_DLT_1
static carve_t carveSE_DLT_2;       // allocations for SE_DLT_2
static carve_t carveSE_DLT_1_ARRAY; // allocations for SE_DLT_1_ARRAY
static carve_t carveSE_DLT_2_ARRAY; // allocations for SE_DLT_2_ARRAY
static carve_t carveSE_CTOR_TEST;   // allocations for SE_CTOR_TEST
static carve_t carveSTAB_DEFN;      // allocations for STAB_DEFN
static carve_t carveSTAB_CTL;       // allocations for STAB_CTL


//**********************************************************************
// internal support
//**********************************************************************

#define carver_DTC_ARRAY        NULL
#define carver_DTC_SYM_AUTO     &carveSE_SYM_AUTO
#define carver_DTC_SYM_STATIC   &carveSE_SYM_STATIC
#define carver_DTC_SUBOBJ       &carveSE_SUBOBJ
#define carver_DTC_TEST_FLAG    &carveSE_TEST_FLAG
#define carver_DTC_COMP_VBASE   &carveSE_COMPONENT
#define carver_DTC_COMP_DBASE   &carveSE_COMPONENT
#define carver_DTC_COMP_MEMB    &carveSE_COMPONENT
#define carver_DTC_ACTUAL_VBASE &carveSE_COMPONENT
#define carver_DTC_ACTUAL_DBASE &carveSE_COMPONENT
#define carver_DTC_TRY          &carveSE_TRY
#define carver_DTC_CATCH        &carveSE_CATCH
#define carver_DTC_FN_EXC       &carveSE_FN_EXC
#define carver_DTC_SET_SV       &carveSE_SET_SV
#define carver_DTC_ARRAY_INIT   &carveSE_ARRAY_INIT
#define carver_DTC_DLT_1        &carveSE_DLT_1
#define carver_DTC_DLT_2        &carveSE_DLT_2
#define carver_DTC_DLT_1_ARRAY  &carveSE_DLT_1_ARRAY
#define carver_DTC_DLT_2_ARRAY  &carveSE_DLT_2_ARRAY
#define carver_DTC_CTOR_TEST    &carveSE_CTOR_TEST

static carve_t* seCarvers[] = {
    #define DTC_DEF( a ) carver_ ## a
    #define DTC_MRK( a )
    #define DTC_VAL( a, b ) DTC_DEF( a )
    DTC_DEFS
    #undef  DTC_DEF
    #undef  DTC_MRK
    #undef  DTC_VAL
};


#ifndef NDEBUG
static carve_t seCarver(        // GET CARVER FOR AN SE TYPE
    uint_8 se_type )            // - code for entry
{
    DbgVerify( se_type < MAX_DTC_DEF, "seCarver -- BAD DTC_..." );
    DbgVerify( se_type != DTC_ARRAY,  "seCarver -- BAD DTC_..." );
    return *seCarvers[ se_type ];
}
#else
    #define seCarver(a) *seCarvers[a]
#endif


static void seFree(             // FREE AN SE ENTRY
    SE* se )                    // - entry
{
    CarveFree( seCarver( se->base.se_type ), se );
}


static void pruneSE(            // PRUNE STATE ENTRY
    SE** a_stab,                // - addr[ state table pointer ]
    SE* se )                    // - entry to be removed
{
    SE* prev                    // - previous entry
        = se->base.prev;
    SE* next                    // - next entry
        = se->base.next;

    RingPrune( a_stab, se );
#ifndef NDEBUG
    if( PragDbgToggle.dump_stab ) {
        printf( "State Table[%x] removed: %x\n", a_stab, se );
        DbgDumpStateEntry( se );
    }
#endif
    next->base.prev = prev;
    prev->base.next = next;
    seFree( se );
}


static boolean sameSE(          // DETERMINE IF SAME STATE ENTRY
    SE* se,                     // - state entry to be added
    SE* state_table )           // - state table
{
    SE* last;                   // - last state entry in table
    boolean retn;               // - TRUE ==> same entry

    last = state_table;
    if( last == NULL ) {
        retn = FALSE;
    } else {
        if( last->base.se_type == DTC_CTOR_TEST ) {
            last = last->base.prev;
        }
        if( last->base.se_type == se->base.se_type ) {
            switch( se->base.se_type ) {
              case DTC_SET_SV :
                last->set_sv.se = se->set_sv.se;
// check for SET_SV to previous
#ifndef NDEBUG
                if( PragDbgToggle.dump_stab ) {
                    printf( "State Table replacement %x\n", se );
                    DbgDumpStateEntry( last );
                }
#endif
                retn = TRUE;
                break;
              case DTC_TEST_FLAG :
                if(  last->test_flag.index    == se->test_flag.index
                  && last->test_flag.se_true  == se->test_flag.se_true
                  && last->test_flag.se_false == se->test_flag.se_false ) {
                    retn = TRUE;
                } else {
                    retn = FALSE;
                }
                break;
              default :
                retn = FALSE;
                break;
            }
        } else {
            retn = FALSE;
        }
    }
    return retn;
}



static SE* stateTableAddSe(     // ADD TO STATE TABLE
    SE* se,                     // - state entry
    SE** a_stab )               // - addr[ state table pointer ]
{
    SE* next;                   // - next in doubly linked ring
    SE* state_table;            // - end of state table
    unsigned state_var;         // - value of state variable

    state_table = *a_stab;
    if( sameSE( se, state_table ) ) {
        seFree( se );
        se = state_table;
    } else {
        if( se->base.se_type == DTC_TEST_FLAG
         && NULL != state_table
         && state_table->base.se_type == DTC_SET_SV ) {
            pruneSE( a_stab, *a_stab );
            state_table = *a_stab;
        }
        if( NULL == state_table ) {
            state_var = 0;
        } else {
            state_var = state_table->base.state_var;
        }
        if( se->base.gen ) {
            ++ state_var;
        }
        se->base.state_var = state_var;
        RingAppend( a_stab, se );
        next = se->base.next;
        se->base.prev = next->base.prev;
        next->base.prev = se;
#ifndef NDEBUG
        if( PragDbgToggle.dump_stab ) {
            printf( "State Table[%x] added: %x\n", a_stab, se );
            DbgDumpStateEntry( se );
        }
#endif
    }
    return se;
}


//**********************************************************************
// STAB_DEFN Support
//**********************************************************************

STAB_DEFN* StabDefnInit(        // INITIALIZE STAB_DEFN
    STAB_DEFN* defn,            // - definition
    unsigned kind )             // - kind of table
{
    defn->kind = kind;
    defn->state_table = NULL;
    defn->ro = NULL;
    return defn;
}


STAB_DEFN* StabDefnAllocate(    // ALLOCATE STAB_DEFN
    unsigned kind )             // - kind of table
{
    return StabDefnInit( CarveAlloc( carveSTAB_DEFN ), kind );
}


void StabDefnFree(              // FREE AN STAB_DEFN
    STAB_DEFN* defn )           // - entry to be freed
{
    CarveFree( carveSTAB_DEFN, defn );
}


SE* StabDefnAddSe(              // ADD STATE ENTRY TO STATE TABLE
    SE* se,                     // - entry to be added
    STAB_DEFN* defn )           // - state table definition
{
    return stateTableAddSe( se, &defn->state_table );
}


void StabDefnFreeStateTable(    // FREE A STATE TABLE
    STAB_DEFN* defn )           // - definition for state table
{
    SE* se;                     // - current state entry

    if( NULL != defn ) {
        RingIterBegSafe( defn->state_table, se ) {
            seFree( se );
        } RingIterEndSafe( se )
    }
}


static void stabDefnRemove(     // REMOVE TOP ENTRY
    STAB_DEFN* defn )           // - definition for state table
{
    pruneSE( &defn->state_table, defn->state_table );
}



//**********************************************************************
// STAB_CTL Support
//**********************************************************************

STAB_CTL* StabCtlInit(          // INITIALIZE STAB_CTL
    STAB_CTL* stab,             // - control info: instance
    STAB_DEFN* defn )           // - control information: definition
{
    stab->rw = NULL;
    stab->defn = defn;
    return stab;
}


SE* StateTableCurrPosn(         // GET STATE ENTRY FOR CURRENT POSITION
    STAB_CTL* sctl )            // - control info
{
    SE* curr_posn;              // - current position

    if( sctl->defn == NULL ) {
        curr_posn = NULL;
    } else {
        curr_posn = SeSetSvPosition( sctl->defn->state_table );
    }
    return curr_posn;
}


SE* StateTableActualPosn(       // GET (UN-OPTIMIZED) CURRENT STATE ENTRY
    STAB_CTL* sctl )            // - control info
{
    return sctl->defn->state_table;
}


SE* StabCtlPrecedes(            // GET PRECEDING ENTRY OR NULL
    STAB_CTL* sctl,             // - control info
    SE* se )                    // - state entry
{
    se = se->base.prev;
    if( se == sctl->defn->state_table ) {
        se = NULL;
    }
    return se;
}


SE* StabCtlPrevious(            // GET PREVIOUS ENTRY OR NULL
    STAB_CTL* sctl,             // - control info
    SE* se )                    // - state entry
{
    return SeSetSvPosition( StabCtlPrecedes( sctl, se ) );
}


static void pruneFixUp(         // FIX-UP PTR. TO ENTRY, FOR PRUNING
    SE** a_se )                 // - addr[ SE ptr ]
{
    SE* se;                     // - entry
    STATE_VAR state_var;        // - state value

    se = *a_se;
    if( se != NULL && ! se->base.gen ) {
        state_var = se->base.state_var;
        if( 0 == state_var ) {
            se = NULL;
        } else {
            for( ; ; ) {
                se = se->base.prev;
                if( se->base.state_var == state_var ) {
                    if( se->base.gen ) break;
                }
            }
        }
        *a_se = se;
    }
}


void StabCtlPrune(              // PRUNE UN-GENNED ENTRIES UP TO AN ENTRY
    SE* ending,                 // - ending entry
    STAB_CTL* sctl )            // - state table definition
{
    SE* se;                     // - current entry
    SE* prev;                   // - previous entry
    SE** hdr;                   // - addr[ state table ]
    boolean done;               // - terminator

    hdr = &sctl->defn->state_table;
    se = *hdr;
    if( se == NULL ) {
        DbgVerify( ending == NULL, "StabCtlPrune: ending != 0 when empty" );
    } else {
        done = FALSE;
        for( ; ; se = prev ) {
            prev = se->base.prev;
            if( ending == 0  ) {
                if( prev == *hdr ) {
                    done = TRUE;
                }
            } else {
                if( se == ending ) break;
            }
            if( ! se->base.gen ) {
                pruneSE( hdr, se );
            } else {
                switch( se->base.se_type ) {
                  case DTC_SET_SV :
                    if( *hdr != se ) {
                        pruneFixUp( &se->set_sv.se );
                    }
                    break;
                  case DTC_TEST_FLAG :
                    pruneFixUp( &se->test_flag.se_true );
                    pruneFixUp( &se->test_flag.se_false );
                    break;
                  default :
                    break;
                }
            }
            if( done ) break;
        }
    }
}


void StabCtlRemove(             // REMOVE LAST STATE ENTRY
    STAB_CTL* sctl )            // - state-table information
{
    stabDefnRemove( sctl->defn );
}


SE* StabCtlPosnGened(           // GET GENERATED POSITION IF REQUIRED
    STAB_CTL* sctl,             // - control info
    SE* src,                    // - source entry
    SE* tgt )                   // - target entry
{
    sctl = sctl;
    if( tgt != NULL && src->base.gen && ! tgt->base.gen ) {
        STATE_VAR sv = tgt->base.state_var;
        if( 0 == sv ) {
            tgt = NULL;
        } else {
            for( ; ; tgt = tgt->base.prev ) {
                if( tgt->base.gen && tgt->base.state_var == sv ) break;
            }
        }
    }
    return tgt;
}


#ifndef NDEBUG


void DbgDumpTypeSigEnt(         // DUMP TYPE_SIG_ENT entries
    TYPE_SIG_ENT* hdr )         // - first entry
{
    TYPE_SIG_ENT* sig;          // - current entry

    RingIterBeg( hdr, sig ) {
        printf( "         TYPE_SIG_ENT[%x]: next[%x] type-sig[%x]\n"
              , sig
              , sig->next
              , sig->sig );
    } RingIterEnd( sig )
}


char const * DbgSeName          // DUMP DTC_... name
    ( unsigned se_type )        // - type of state entry
{
    static char* dtc_names[] = {// - names
        #define DTC_DEF( a ) # a
        #define DTC_MRK( a )
        #define DTC_VAL( a, b ) DTC_DEF(a)
        DTC_DEFS
        #undef  DTC_DEF
        #undef  DTC_MRK
        #undef  DTC_VAL
    };
    char const * sv_name;       // - name to be returned
    if( se_type >= MAX_DTC_DEF ) {
        sv_name = "*** UNKNOWN ***";
    } else {
        sv_name = dtc_names[ se_type ];
    }
    return sv_name;
}


void DbgDumpStateEntry(         // DUMP STATE ENTRY
    void* arg )                 // - state entry
{
    SE* se;                     // - state entry
    char const* sv_name;        // - name of entry

    se = arg;
    sv_name = DbgSeName( se->base.se_type );
    printf( "State Entry[%x]: gen(%d) next(%x) prev(%x) var(%d) %s"
          , se
          , se->base.gen
          , se->base.next
          , se->base.prev
          , se->base.state_var
          , sv_name );
    switch( se->base.se_type ) {
      case DTC_CTOR_TEST :
        printf( "\n    flag(%d)\n"
              , se->ctor_test.flag_no );
        break;
      case DTC_SYM_AUTO :
        printf( "\n    sym(%s) dtor(%s) offset(%x)\n"
              , DbgSymNameFull( se->sym_auto.sym )
              , DbgSymNameFull( se->sym_auto.dtor )
              , se->sym_auto.offset );
        break;
      case DTC_SYM_STATIC :
        printf( "\n    sym(%s) dtor(%s)\n"
              , DbgSymNameFull( se->sym_static.sym )
              , DbgSymNameFull( se->sym_static.dtor ) );
        break;
      case DTC_SET_SV :
        printf( "\n    se(%x = %d)\n"
              , se->set_sv.se
              , SeStateVar( se->set_sv.se ) );
        break;
      case DTC_ACTUAL_VBASE :
      case DTC_ACTUAL_DBASE :
      case DTC_COMP_VBASE :
      case DTC_COMP_DBASE :
      case DTC_COMP_MEMB :
        printf( "\n    reg(%x) offset(%x) dtor(%s)\n"
              , se->component.obj
              , se->component.offset
              , DbgSymNameFull( se->component.dtor ) );
        break;
      case DTC_TEST_FLAG :
        printf( "\n    index(%d) true(%x = %d) false(%x = %d)\n"
              , se->test_flag.index
              , se->test_flag.se_true
              , SeStateVar( se->test_flag.se_true )
              , se->test_flag.se_false
              , SeStateVar( se->test_flag.se_false ) );
        break;
      case DTC_TRY :
        printf( "\n    impl(%x) sigs(%x) sym(%s)\n"
              , se->try_blk.try_impl
              , se->try_blk.sigs
              , DbgSymNameFull( se->try_blk.sym ) );
        DbgDumpTypeSigEnt( se->try_blk.sigs );
        break;
      case DTC_CATCH :
        printf( "\n    try(%x) sig(%x)\n"
              , se->catch_blk.try_blk
              , se->catch_blk.sig );
        break;
      case DTC_FN_EXC :
        printf( "\n    sigs(%x)\n"
              , se->fn_exc.sigs );
        DbgDumpTypeSigEnt( se->fn_exc.sigs );
        break;
      case DTC_ARRAY :
        printf( "\n    offset(%x) sig(%x) count(%x)\n"
              , se->array.offset
              , se->array.sig
              , se->array.count );
        break;
      case DTC_SUBOBJ :
        printf( "\n    offset(%x) original(%x) kind(%s) dtor(%s)\n"
              , se->subobj.offset
              , se->subobj.original
              , DbgSeName( se->subobj.kind )
              , DbgSymNameFull( se->subobj.dtor ) );
        break;
      case DTC_ARRAY_INIT :
        printf( "\n    reg(%x)\n"
              , se->array_init.reg );
        break;
      case DTC_DLT_1 :
      case DTC_DLT_1_ARRAY :
        printf( "\n    op_del(%s) offset(%x)\n"
              , DbgSymNameFull( se->del_1.op_del )
              , se->del_1.offset );
        break;
      case DTC_DLT_2 :
      case DTC_DLT_2_ARRAY :
        printf( "\n    op_del(%s) offset(%x) size(%x)\n"
              , DbgSymNameFull( se->del_2.op_del )
              , se->del_2.offset
              , se->del_2.size );
        break;
    }
}


void DbgDumpStateTableDefn(     // DUMP STATE TABLE DEFINITION
    void* arg )                 // - control info
{
    STAB_DEFN* defn;            // - definition
    SE* se;                     // - current state entry:

    defn = arg;
    if( defn != NULL ) {
        printf( "State Table Definition[%x] R/O(%s) kind(%x)\n"
              , defn
              , DbgSymNameFull( defn->ro ) );
        if( defn->state_table != NULL ) {
            for( se = defn->state_table; ; se = se->base.prev ) {
                DbgDumpStateEntry( se );
                if( se == defn->state_table->base.next ) break;
            }
        }
        fflush( stdout );
    }
}


void DbgDumpStateTable(         // DUMP STATE TABLE INSTANCE
    void* arg )                 // - control info
{
    STAB_CTL* sctl;             // - control info: instance
    STAB_DEFN* defn;            // - control info: definition

    sctl = arg;
    defn = sctl->defn;
    printf( "State Table Instance[%x] definition(%x) R/W(%s)\n"
          , sctl
          , defn
          , DbgSymNameFull( sctl->rw ) );
    DbgDumpStateTableDefn( defn );
    fflush( stdout );
}

#endif



//**********************************************************************
// SE (state entries) Support
//**********************************************************************



SE* StateTableAdd(              // ADD TO STATE TABLE
    SE* se,                     // - state entry
    STAB_CTL* sctl )            // - state table information
{
    return stateTableAddSe( se, &sctl->defn->state_table );
}


SE* SeAlloc(                    // ALLOCATE AN SE ENTRY
    uint_8 se_type )            // - code for entry
{
    SE* se;                     // - new entry

    se = CarveAlloc( seCarver( se_type ) );
    se->base.se_type = se_type;
    se->base.gen = BlkPosnUseStab();;
    return se;
}


SE* SeSetSvPosition(            // LOCATE STATE ENTRY PAST OPTIONAL SET_SV'S
    SE* se )                    // - starting position
{
    for( ; se != NULL; ) {
        switch( se->base.se_type ) {
          case DTC_SET_SV :
            se = se->set_sv.se;
            continue;
          case DTC_CTOR_TEST :
            se = se->base.prev;
            continue;
          default :
            break;
        }
        break;
    }
    return se;
}


STATE_VAR SeStateVar(           // GET STATE VARIABLE AT CURRENT POSITION
    SE* se )                    // - state entry
{
    STATE_VAR state_var;        // - state variable

    if( se == NULL ) {
        state_var = 0;
    } else {
        state_var = se->base.state_var;
    }
    return state_var;
}


STATE_VAR SeStateOptimal(       // GET STATE VALUE FOR POSITION (OPTIMAL)
    SE* se )                    // - state entry
{
    return SeStateVar( SeSetSvPosition( se ) );
}



//**********************************************************************
// Initialization and Completion
//**********************************************************************

static void stabInit(           // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveSE_SYM_STATIC  = CarveCreate( sizeof( SE_SYM_STATIC ),     32 );
    carveSE_SYM_AUTO    = CarveCreate( sizeof( SE_SYM_AUTO ),       32 );
    carveSE_SUBOBJ      = CarveCreate( sizeof( SE_SUBOBJ ),         16 );
    carveSE_TRY         = CarveCreate( sizeof( SE_TRY ),            4  );
    carveSE_CATCH       = CarveCreate( sizeof( SE_CATCH ),          8  );
    carveSE_FN_EXC      = CarveCreate( sizeof( SE_FN_EXC ),         4  );
    carveSE_SET_SV      = CarveCreate( sizeof( SE_SET_SV ),         8  );
    carveSE_TEST_FLAG   = CarveCreate( sizeof( SE_TEST_FLAG ),      16 );
    carveSE_COMPONENT   = CarveCreate( sizeof( SE_COMPONENT ),      16 );
    carveSE_ARRAY_INIT  = CarveCreate( sizeof( SE_ARRAY_INIT ),     4  );
    carveSE_DLT_1       = CarveCreate( sizeof( SE_DLT_1 ),          4  );
    carveSE_DLT_2       = CarveCreate( sizeof( SE_DLT_2 ),          4  );
    carveSE_DLT_1_ARRAY = CarveCreate( sizeof( SE_DLT_1_ARRAY ),    4  );
    carveSE_DLT_2_ARRAY = CarveCreate( sizeof( SE_DLT_2_ARRAY ),    4  );
    carveSE_CTOR_TEST   = CarveCreate( sizeof( SE_CTOR_TEST ),      4  );
    carveSTAB_DEFN      = CarveCreate( sizeof( STAB_DEFN ),         8  );
    carveSTAB_CTL       = CarveCreate( sizeof( STAB_CTL ),          8  );
}


static void stabFini(           // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    CarveDestroy( carveSE_SYM_AUTO );
    CarveDestroy( carveSE_SYM_STATIC );
    CarveDestroy( carveSE_SUBOBJ );
    CarveDestroy( carveSE_TRY );
    CarveDestroy( carveSE_CATCH );
    CarveDestroy( carveSE_FN_EXC );
    CarveDestroy( carveSE_SET_SV );
    CarveDestroy( carveSE_TEST_FLAG );
    CarveDestroy( carveSE_COMPONENT );
    CarveDestroy( carveSE_ARRAY_INIT );
    CarveDestroy( carveSE_DLT_1 );
    CarveDestroy( carveSE_DLT_2 );
    CarveDestroy( carveSE_DLT_1_ARRAY );
    CarveDestroy( carveSE_DLT_2_ARRAY );
    CarveDestroy( carveSE_CTOR_TEST );
    CarveDestroy( carveSTAB_DEFN );
    CarveDestroy( carveSTAB_CTL );
}


INITDEFN( state_table, stabInit, stabFini )

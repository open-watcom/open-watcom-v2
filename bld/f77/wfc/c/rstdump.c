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
* Description:  Resident symbol table dump module.
*
****************************************************************************/


#include "ftnstd.h"
#include "ecflags.h"
#include "errcod.h"
#include "undef.h"
#include "global.h"
#include "progsw.h"
#include "segsw.h"
#include "cpopt.h"
#include "iflookup.h"
#include "ferror.h"
#include "insert.h"


extern  sym_id          STFreeName(sym_id);
extern  bool            IsIFUsed(IFF);
extern  sym_id          AddSP2GList(sym_id);
extern  sym_id          AddCB2GList(sym_id);
extern  unsigned_32     GetStmtNum(sym_id);
extern  char            *STGetName(sym_id,char *);
extern  char            *STNmListName(sym_id,char *);
extern  sym_id          STFree(sym_id);
extern  void            FreeSFHeader(sym_id);
extern  void            AllocGlobal(unsigned_32,bool);
extern  bool            ForceStatic(unsigned_16);
extern  void            CkSymDeclared( sym_id );

extern  uint            DataThreshold;
extern  sym_id          ReturnValue;
extern  sym_id          EPValue;

static  void            DumpStmtNos( void );
static  void            DumpNameLists( void );
static  void            DumpLocalVars( void );
static  void            DumpConsts( void );
static  void            DumpStrings( void );
static  void            UnrefSym( sym_id sym );
static  void            CkDataOk( sym_id sym );



void    STDump( void ) {
//================

// Dump symbol table.

    sym_id      sym;

    // Merge IFList with NList
    if( IFList != NULL ) {
        sym = IFList;
        while( sym->u.ns.link != NULL ) {
            sym = sym->u.ns.link;
        }
        sym->u.ns.link = NList;
        NList = IFList;
        IFList = NULL;
    }
    if( !(ProgSw & PS_DONT_GENERATE) ) return;
    ProgSw |= PS_SYMTAB_PROCESS;
    DumpStmtNos();              // must be before DumpMagSyms()
    DumpNameLists();            // dump the namelist data structures
    DumpLocalVars();
    DumpConsts();
    DumpStrings();              // dump string data (must be last)
    ProgSw &= ~PS_SYMTAB_PROCESS;
}



static  bool    CkInCommon( sym_id sym ) {
//========================================

// Check for variables in common.

    sym_id      leader;
    com_eq      *eq_ext;
    unsigned_32 offset;
    bool        global_item;

    global_item = FALSE;
    if( sym->u.ns.flags & SY_IN_EQUIV ) {
        offset = 0;
        leader = sym;
        for(;;) {
            eq_ext = leader->u.ns.si.va.vi.ec_ext;
            if( eq_ext->ec_flags & LEADER ) break;
            offset += eq_ext->offset;
            leader = eq_ext->link_eqv;
        }
        if( ( eq_ext->ec_flags & MEMBER_IN_COMMON ) == 0 ) {
            if( !( eq_ext->ec_flags & EQUIV_SET_ALLOC ) ) {
                if( ForceStatic(sym->u.ns.flags) || !(Options & OPT_AUTOMATIC) ) {
                    if( ( ProgSw & PS_BLOCK_DATA ) == 0 ) {
                        AllocGlobal( eq_ext->high - eq_ext->low,
                             (eq_ext->ec_flags & MEMBER_INITIALIZED) != 0 );
                        eq_ext->ec_flags |= EQUIV_SET_ALLOC;
                    }
                }
            }
        }
    }
    return( global_item );
}


static  bool    DumpVariable( sym_id sym ) {
//==========================================

// Allocate space for the given variable.

    unsigned_16         flags;
    act_dim_list        *dim_list;
    bool                global_item;
    bool                cp_reloc; // is array compile-time relocatable

    global_item = FALSE;
    flags = sym->u.ns.flags;
    CkDataOk( sym );
    if( !(flags & (SY_SUB_PARM | SY_DATA_INIT | SY_IN_EC)) ) {
        if( (flags & SY_REFERENCED) && !(sym->u.ns.u1.s.xflags & SY_DEFINED) ) {
            NameWarn( VA_USED_NOT_DEFINED, sym );
        }
    }
    if( ( flags & SY_IN_EQUIV ) &&
        ( sym->u.ns.si.va.vi.ec_ext->ec_flags & LEADER ) &&
        ( ( sym->u.ns.si.va.vi.ec_ext->ec_flags & ES_TYPE ) == ES_MIXED ) ) {
        Extension( EV_MIXED_EQUIV );
    }
    if( flags & SY_SUBSCRIPTED ) {
        dim_list = sym->u.ns.si.va.u.dim_ext;
        cp_reloc = !( flags & SY_SUB_PARM ) && !_Allocatable( sym );
        if( cp_reloc && ( dim_list->num_elts == 0 ) ) {
            NameErr( SV_ARR_PARM, sym );
        } else {
            if( cp_reloc ) {
                global_item = TRUE;
                if( flags & SY_IN_EC ) {
                    global_item = CkInCommon( sym );
                } else if( ( ProgSw & PS_BLOCK_DATA ) == 0 ) {
                    if( ForceStatic( flags ) || !(Options & OPT_AUTOMATIC) ) {
                        if( _SymSize( sym ) * _ArrElements( sym ) >
                            DataThreshold ) {
                            AllocGlobal( _SymSize( sym )*_ArrElements( sym ),
                                         (flags & SY_DATA_INIT) != 0 );
                        }
                    }
                }
            }
        }
    } else if( sym->u.ns.u1.s.typ == FT_STRUCTURE ) {
        if( ( flags & SY_SUB_PARM ) == 0 ) {
            if( (flags & SY_IN_DIMEXPR) && (flags & SY_IN_COMMON) == 0 ) {
                NameErr( SV_ARR_DECL, sym );
            } else {
                global_item = TRUE;
                if( flags & SY_IN_EC ) {
                    global_item = CkInCommon( sym );
                } else if( ( ProgSw & PS_BLOCK_DATA ) == 0 ) {
                    if( ForceStatic( flags ) || !(Options & OPT_AUTOMATIC) ) {
                        if( sym->u.ns.xt.record->size > DataThreshold ) {
                            AllocGlobal( sym->u.ns.xt.record->size,
                                         (flags & SY_DATA_INIT) != 0 );
                        }
                    }
                }
            }
        }
    } else if( sym->u.ns.u1.s.typ == FT_CHAR ) {
        if( ( flags & SY_SUB_PARM ) == 0 ) {
            global_item = !_Allocatable( sym );
            if( ( sym->u.ns.xt.size == 0 ) && !_Allocatable( sym ) ){
                NameErr( CV_CHARSTAR_ILLEGAL, sym );
            } else {
                if( _Allocatable( sym ) ) {
                    Extension( VA_ALLOCATABLE_STORAGE, sym->u.ns.name );
                }
                if( flags & SY_IN_EC ) {
                    global_item = CkInCommon( sym );
                } else if( ( ProgSw & PS_BLOCK_DATA ) == 0 ) {
                    if( ForceStatic( flags ) || !(Options & OPT_AUTOMATIC) ) {
                        AllocGlobal( sym->u.ns.xt.size,
                            (flags & SY_DATA_INIT) != 0 );
                    }
                }
            }
        }
    } else if( ( flags & ( SY_IN_EC | SY_SUB_PARM ) ) == 0 ) {
        if( flags & SY_IN_DIMEXPR ) {
            NameErr( SV_ARR_DECL, sym );
        }
    } else {
        if( ( flags & SY_SUB_PARM ) == 0 ) {
            global_item = CkInCommon( sym );
        }
    }
    return( global_item );
}


static  void    DumpLocalVars( void ) {
//===============================

// Dump local variables.

    unsigned_16 flags;
    unsigned_16 class;
    unsigned_16 subprog_type;
    sym_id      sym;

    sym = NList;
    while( sym != NULL ) {
        flags = sym->u.ns.flags;
        class = flags & SY_CLASS;
        if( class == SY_VARIABLE ) {
            if( (sym != ReturnValue) && (sym != EPValue) ) {
                CkSymDeclared( sym );
                if( (flags & (SY_REFERENCED | SY_IN_EC)) == 0 ) {
                    UnrefSym( sym );
                }
                DumpVariable( sym );
            }
        } else if( class == SY_SUBPROGRAM ) {
            if( ( flags & ( SY_REFERENCED | SY_EXTERNAL ) ) == 0 ) {
                UnrefSym( sym );
            }
            subprog_type = flags & SY_SUBPROG_TYPE;
            if( subprog_type == SY_REMOTE_BLOCK ) {
                if( ( flags & SY_RB_DEFINED ) == 0 ) {
                    NameErr( SP_RB_UNDEFINED, sym );
                }
            } else if( subprog_type == SY_STMT_FUNC ) {
            } else if( subprog_type == SY_BLOCK_DATA ) {
                AddSP2GList( sym );
            } else if( !IsIntrinsic( flags ) ) {
                AddSP2GList( sym );
            } else if( IsIFUsed( sym->u.ns.si.fi.index ) ) {
                AddSP2GList( sym );
            }
        } else { // if( class == SY_PARAMETER ) {
            if( ( flags & SY_REFERENCED ) == 0 ) {
                UnrefSym( sym );
            }
        }
        sym = sym->u.ns.link;
    }
    // Common block list must be dumped after the name list so that the
    // SY_COMMON_INIT bit gets set in the common block flags
    // before we add the common block to the global list so that we can
    // detect whether common blocks appear in more than one block data
    // subprogram.
    sym = BList;
    while( sym != NULL ) {    // common block list
        if( ( SgmtSw & SG_BIG_SAVE ) || (Options & OPT_SAVE) ) {
            sym->u.ns.flags |= SY_SAVED;
        }
        AddCB2GList( sym );
        sym = STFreeName( sym );
    }
    BList = NULL;
    // Free NList after processing it since we need to compute offsets
    // of equivalenced names in the equivalence set
    while( NList != NULL ) {
        NList = STFreeName( NList );
    }
}


static  void    UnrefSym( sym_id sym ) {
//======================================

// Issue a warning for unreferenced symbol.

    if( !(sym->u.ns.u1.s.xflags & SY_FAKE_REFERENCE) ) {
        NameWarn( VA_UNREFERENCED, sym );
    }
}



static  void    CkDataOk( sym_id sym ) {
//======================================

// Check that a data initialized variable in common is OK.

    sym_id      name;
    com_eq      *eq_ext;
    unsigned_16 flags;

    flags = sym->u.ns.flags;
    if( ( flags & SY_DATA_INIT ) == 0 ) return;
    name = sym;
    if( flags & SY_IN_EC ) {
        if( flags & SY_IN_COMMON ) {
            sym = sym->u.ns.si.va.vi.ec_ext->com_blk;
        } else { // if( flags & SY_IN_EQUIV ) {
            for(;;) {
                eq_ext = sym->u.ns.si.va.vi.ec_ext;
                if( ( eq_ext->ec_flags & LEADER ) != 0 ) break;
                sym = eq_ext->link_eqv;
            }
            if( ( eq_ext->ec_flags & MEMBER_IN_COMMON ) == 0 ) {
                if( ProgSw & PS_BLOCK_DATA ) {
                    NameErr( BD_BLKDAT_NOT_COMMON, name );
                }
                return;
            } else {
                sym = eq_ext->com_blk;
            }
        }
        if( ( sym->u.ns.flags & SY_BLANK_COMMON ) != 0 ) {
            NameErr( DA_BLANK_INIT, name );
        } else {
            sym->u.ns.flags |= SY_COMMON_INIT;
            if( ( ProgSw & PS_BLOCK_DATA ) == 0 ) {
                NameExt( CM_COMMON, name );
            }
        }
    } else {
        if( ProgSw & PS_BLOCK_DATA ) {
            NameErr( BD_BLKDAT_NOT_COMMON, name );
        }
    }
}


bool    StmtNoRef( sym_id sn ) {
//==============================

// Check if statement number has been referenced.

    if( StNumbers.wild_goto ) return( TRUE );
    if( ( sn->u.st.flags & SN_AFTR_BRANCH ) == 0 ) return( TRUE );
    if( sn->u.st.flags & ( SN_ASSIGNED | SN_BRANCHED_TO ) ) return( TRUE );
    return( FALSE );
}


static  void    DumpStmtNos( void ) {
//=============================

// Check that statement numbers are all defined.

    sym_id      sn;
    unsigned_16 sn_flags;
    unsigned_32 st_number;

    sn = SList;
    while( sn != NULL ) {
        sn_flags = sn->u.st.flags;
        st_number = GetStmtNum( sn );
        if( ( sn_flags & SN_DEFINED ) == 0 ) {
            Error( ST_UNDEFINED, st_number, sn->u.st.line );
        } else {
            if( !StmtNoRef( sn ) ) {
                Warning( ST_UNREFERENCED, st_number );
            }
        }
        sn = STFree( sn );
    }
    SList = NULL;
}



static  void    DumpConsts( void ) {
//============================

// Dump constants from the symbol table.


    while( CList != NULL ) {
        CList = STFree( CList );
    }
}


static  void    DumpStrings( void ) {
//=============================

// Dump constant literals and space for static temporaries.

    while( LList != NULL ) {
        LList = STFree( LList );
    }
    for( ; MList != NULL; MList = STFree( MList ) ) {
        // check if shadow for function return value
        if( MList->u.ns.flags & SY_PS_ENTRY ) continue;
        if( MList->u.ns.u1.s.typ == FT_CHAR ) {
            if( ( MList->u.ns.xt.size != 0 ) && !( Options & OPT_AUTOMATIC ) ) {
                AllocGlobal( MList->u.ns.xt.size, FALSE );
            }
        }
    }
}


static  void    DumpNameLists( void ) {
//===============================

// Dump NAMELIST information.

    char        buff1[MAX_SYMLEN+1];
    char        buff2[MAX_SYMLEN+1];
    sym_id      nl;
    grp_entry   *ge;
    sym_id      sym;
    unsigned_16 flags;

    nl = NmList;
    while( nl != NULL ) {
        ge = nl->u.nl.group_list;
        while( ge != NULL ) {
            sym = ge->sym;
            flags = sym->u.ns.flags;
            // do error checks
            if( ((flags & SY_CLASS) != SY_VARIABLE) || (flags & SY_SUB_PARM) ||
                (sym->u.ns.u1.s.typ == FT_STRUCTURE) || _Allocatable( sym ) ) {
                STGetName( sym, buff1 );
                STNmListName( nl, buff2 );
                Error( VA_BAD_SYM_IN_NAMELIST, buff1, buff2 );
            }

            ge = ge->link;
        }
        nl = nl->u.nl.link;
    }
}

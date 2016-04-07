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
* Description:  compile SUBROUTINE, (type)FUNCTION, and related statements
*
****************************************************************************/


//      Note : "type{*n} FUNCTION" statement handled by PR_type

#include "ftnstd.h"
#include "segsw.h"
#include "errcod.h"
#include "progsw.h"
#include "opr.h"
#include "namecod.h"
#include "global.h"
#include "cpopt.h"
#include "fmemmgr.h"
#include "recog.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "argchk.h"
#include "compprog.h"
#include "compstmt.h"
#include "proctbl.h"
#include "cpsubpgm.h"
#include "csutls.h"
#include "impltab.h"
#include "symtab.h"
#include "cgmagic.h"
#include "gdbginfo.h"
#include "gsublist.h"
#include "gsubprog.h"
#include "declare.h"
#include "implicit.h"
#include "rstmgr.h"
#include "fcodes.h"
#include "gflow.h"


extern  void            BIStartRBorEP( sym_id );
extern  void            BIStartBlockData( sym_id );
extern  void            BIStartSubroutine( void );

/* forward declarations */
static void CkSubEnd( void );
static void StartProg( void );
static void ParmList( bool star_ok, entry_pt *entry );

void CpProgram( void )
{
    CkSubEnd();
    if( ReqName( NAME_PROGRAM ) ) {
        SubProgId = LkProgram();    // use default name
    } else {
        SubProgId = LkProgram();        // use default name
    }
    StartProg();
    AdvanceITPtr();
    ReqEOS();
}

void DefProg( void )
{
// Define the program unit since no PROGRAM, SUBROUTINE, FUNCTION or
// BLOCK DATA statements were specified.

    SubProgId = LkProgram();
    StartProg();
}

static void StartProg( void )
{
    ProgSw &= ~PS_IN_SUBPROGRAM;
    if( ProgSw & PS_PROGRAM_DONE ) {
        Error( SR_TWO_PROGRAMS );
    }
    ProgSw |= PS_PROGRAM_DONE;
    GPgmLabel();
    BIStartSubroutine();
}


static entry_pt *AddEntryPt( sym_id sym_ptr )
{
    entry_pt    *ptr;

    if( Entries == NULL ) {
        ArgList = FMemAlloc( sizeof( entry_pt ) );
        Entries = ArgList;
        ptr     = Entries;
    } else {
        ptr = Entries;
        while( ptr->link != NULL ) {
            ptr = ptr->link;
        }
        ptr->link = FMemAlloc( sizeof( entry_pt ) );
        ptr = ptr->link;
        if( ArgList == NULL ) {
            ArgList = ptr;
        }
    }
    ptr->link = NULL;
    ptr->id = sym_ptr;
    ptr->parms = NULL;
    return( ptr );
}


static  entry_pt *SubProgName( TYPE typ, unsigned_16 flags,
                                uint def_size, bool len_spec )
{
// Process the symbolic name of a SUBROUTINE or FUNCTION.

    entry_pt    *entry;
    itnode      *name_node;
    itnode      *next_node;
    sym_id      sym_ptr;
    uint        size;
    sym_ptr = LkSym();
    SubProgId = sym_ptr;
    GSegLabel();    // must be before DumpStatement() so that ISN code for
    if( Options & OPT_TRACE ) {
        GSetSrcLine();
    }
    name_node = CITNode;
    sym_ptr->u.ns.flags = flags;
    name_node->flags = flags;
    size = def_size;
    next_node = CITNode->link;
    if( !len_spec ) {
        AdvanceITPtr();
        if( !LenSpec( typ, &size ) ) {
            size = StorageSize( typ );
        }
        next_node = CITNode;
    }
    sym_ptr->u.ns.xt.size = size;
    name_node->size = size;
    typ = MapTypes( typ, size );
    sym_ptr->u.ns.u1.s.typ = typ;
    name_node->typ = typ;
    CITNode = name_node;
    entry = AddEntryPt( sym_ptr );
    CITNode = next_node;
    return( entry );
}


void CpSubroutine( void )
{
    entry_pt    *entry;

    CkSubEnd();
    ProgSw |= PS_IN_SUBPROGRAM;
    if( ReqName( NAME_SUBROUTINE ) ) {
        entry = SubProgName( FT_NO_TYPE, SY_USAGE | SY_SUBPROGRAM | SY_PENTRY |
                              SY_SUBROUTINE | SY_REFERENCED, 0, TRUE );
        if( RecOpenParen() ) {
            ParmList( TRUE, entry );
            ReqCloseParen();
            ReqNOpn();
            AdvanceITPtr();
        }
        ReqEOS();
    } else {
        // We still want to start a subprogram even though there is no name.
        SubProgId = LkProgram();        // use default name
        GSegLabel();
    }
    BIStartSubroutine();
}


void    Function( TYPE typ, uint size, bool len_spec )
{
// Compile [type] [*len] FUNCTION NAME[*len] ([d,d,...])
//            \                /
//             Already scanned
//

    unsigned_16 flags;
    entry_pt    *entry;

    flags = SY_USAGE | SY_SUBPROGRAM | SY_PENTRY | SY_FUNCTION;
    if( typ == FT_NO_TYPE ) {
        typ = ImplType( *(CITNode->opnd) );
    } else {
        flags |= SY_TYPE;
    }
    CkSubEnd();
    ProgSw |= PS_IN_SUBPROGRAM;
    if( ReqName( NAME_FUNCTION ) ) {
        entry = SubProgName( typ, flags, size, len_spec );
        STFnShadow( SubProgId );
        if( ReqOpenParen() ) {
            ParmList( FALSE, entry );
        }
        ReqCloseParen();
        ReqNOpn();
        AdvanceITPtr();
        ReqEOS();
    } else {
        // We still want to start a subprogram even though there is no name.
        SubProgId = LkProgram();        // use default name
        GSegLabel();
    }
    BIStartSubroutine();
}


void CpFunction( void )
{
    Function( FT_NO_TYPE, -1, FALSE );
}

void CpEntry( void )
{
    entry_pt    *entry;
    bool        in_subr;
    sym_id      sym;

    if( ( ProgSw & PS_IN_SUBPROGRAM ) == 0 ) {
        StmtErr( SR_ILL_IN_PROG );
    }
    if( !EmptyCSList() ) {
        Error( EY_NOT_IN_CS );
    }
    if( ReqName( NAME_FUNCTION ) ) {
        in_subr = (SubProgId->u.ns.flags & SY_SUBPROG_TYPE) == SY_SUBROUTINE;
        sym = LkSym();
        if( ( sym->u.ns.flags & (SY_USAGE|SY_SUB_PARM|SY_IN_EC|SY_SAVED) ) ||
            ( in_subr && (sym->u.ns.flags & SY_TYPE) ) ) {
            IllName( sym );
        } else {
            sym->u.ns.u1.s.typ = CITNode->typ;
            sym->u.ns.flags &= SY_TYPE;
            if( in_subr ) {
                sym->u.ns.flags |= SY_USAGE | SY_SUBPROGRAM | SY_SENTRY |
                                     SY_SUBROUTINE | SY_REFERENCED;
            } else {
                sym->u.ns.flags |= SY_USAGE | SY_SUBPROGRAM | SY_SENTRY |
                                     SY_FUNCTION;
            }
            STFnShadow( sym );
            entry = AddEntryPt( sym );
            AdvanceITPtr();
            if( Options & OPT_TRACE ) {
                GSetSrcLine();
            }
            if( RecOpenParen() ) {
                ParmList( in_subr, entry );
                ReqCloseParen();
                ReqNOpn();
                AdvanceITPtr();
            }
            BIStartRBorEP( sym );
            ReqEOS();
        }
    }
    SgmtSw &= ~SG_PROLOG_DONE;       // indicate we need prologue
}

static void CkRemBlock( void )
{
    csnode      *csptr;

    csptr = CSHead;
    for(;;) {
        if( csptr->typ == CS_EMPTY_LIST ) return;
        if( csptr->typ == CS_REMOTEBLOCK ) break;
        csptr = csptr->link;
    }
    Error( SP_RET_IN_REMOTE );
}

void CpReturn( void )
{
    if( !(ProgSw & PS_IN_SUBPROGRAM) ) {
        Extension( RE_IN_PROGRAM );
    }
    CkRemBlock();
    if( RecNOpn() && RecNextOpr( OPR_TRM ) ) {
        if( ( ( SubProgId->u.ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) &&
            ( ( SubProgId->u.ns.flags & SY_SUBPROG_TYPE ) == SY_SUBROUTINE ) ) {
            GNullRetIdx();
        }
    } else {
        IntSubExpr();
        if( ( ( SubProgId->u.ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) &&
            ( ( SubProgId->u.ns.flags & SY_SUBPROG_TYPE ) == SY_SUBROUTINE ) ) {
            GRetIdx();
        } else {
            Error( RE_ALT_IN_SUBROUTINE );
        }
    }
    AdvanceITPtr();
    ReqEOS();
    GGotoEpilog();
    Remember.transfer = TRUE;
    Remember.stop_or_return = TRUE;
}

static void CkSubEnd( void )
{
// Check if we had an END statement.

    if( ( SgmtSw & SG_STMT_PROCESSED ) && !Remember.endstmt ) {
        FiniSubProg();
        InitSubProg();
    }
    CkDefStmtNo();
}

static parameter *NameParm( entry_pt *entry )
{
// Process a symbolic dummy argument.

    parameter           *result;
    sym_id              sym;
    act_dim_list        *dim_ptr;
    unsigned_16         flags;
    unsigned_16         class;

    sym = LkSym();
    flags = sym->u.ns.flags;
    class = flags & SY_CLASS;
    if( class == SY_VARIABLE ) {
        if( InArgList( entry, sym ) ) {
            NameErr( AR_DUPLICATE_PARM, sym );
            return( NULL );
        } else if( flags & SY_SAVED ) {
            Error( SA_SAVED );
            return( NULL );
        } else if( flags & SY_IN_EC ) {
            IllName( sym );
            return( NULL );
        } else if( flags & SY_SUBSCRIPTED ) {
            dim_ptr = sym->u.ns.si.va.u.dim_ext;
            if( dim_ptr->dim_flags & DIM_PVD ) {
                dim_ptr->dim_flags |= DIM_ASSUMED;
                NameExt( SV_PVD, sym );
                if( dim_ptr->dim_flags & DIM_USED_IN_IO ) {
                    NameErr( SV_CANT_USE_ASSUMED, sym );
                    return( NULL );
                }
            }
        }
    } else if( class == SY_PARAMETER ) {
        IllName( sym );
        return( NULL );
    } else { // subroutine name
        class = flags & SY_SUBPROG_TYPE;
        if( ( class != SY_FUNCTION ) && ( class != SY_SUBROUTINE ) &&
            ( class != SY_FN_OR_SUB ) ) {
            IllName( sym );
            return( NULL );
        } else if( flags & ( SY_PS_ENTRY | SY_INTRINSIC ) ) {
            IllName( sym );
            return( NULL );
        }
    }
    result = FMemAlloc( sizeof( parameter ) );
    result->link = NULL;
    result->id = sym;
    result->flags = 0;
    if( sym->u.ns.flags & SY_SUB_PARM ) {
        result->flags |= ARG_DUPLICATE;
    }
    sym->u.ns.flags |= SY_SUB_PARM;
    return( result );
}

static parameter *StarParm( void )
{
// Process an asterisk dummy argument.

    parameter   *result;

    result = FMemAlloc( sizeof( parameter ) );
    result->link = NULL;
    result->flags |= ARG_STMTNO;
    return( result );
}


static void ParmList( bool star_ok, entry_pt *entry )
{
// Process the formal parameter list of a FUNCTION/SUBROUTINE.

    parameter   **args;
    parameter   *new_arg;

    args = &entry->parms;
    if( RecNOpn() && RecNextOpr( OPR_RBR ) ) {
        // consider name()
        AdvanceITPtr();
    } else {
        for(;;) {       // process parm list
            if( star_ok && RecNOpn() ) {
                AdvanceITPtr();
                if( ReqMul() && ReqNOpn() ) {
                    *args = StarParm();
                    args = &(*args)->link;
                }
            } else if( ReqName( NAME_ARGUMENT ) ) {
                new_arg = NameParm( entry );
                if( new_arg != NULL ) {
                    *args = new_arg;
                    args = &(*args)->link;
                }
            }
            AdvanceITPtr();
            if( !RecComma() ) break;
        }
    }
}

static void DoWarps( void )
{
// Generate calls to warp code.

    parameter   *parm;
    sym_id      sym;

    for( parm = ArgList->parms; parm != NULL; parm = parm->link ) {
        if( parm->flags & ARG_STMTNO ) continue;
        sym = parm->id;
        if( ( sym->u.ns.flags & SY_CLASS ) != SY_VARIABLE ) continue;
        if( ( sym->u.ns.flags & SY_SUBSCRIPTED ) == 0 ) continue;
        if( _AdvRequired( sym->u.ns.si.va.u.dim_ext ) == 0 ) continue;
        GWarp( sym );
    }
}

static void InitParms( void )
{
    DoWarps();
}

void Prologue( void )
{
// Generate a FUNCTION/SUBROUTINE/ENTRY prologue starting with ArgList.

    label_id    skip_label;

    SgmtSw |= SG_PROLOG_DONE;
    if( ( ArgList != NULL ) && ( ArgList->id == SubProgId ) ) {
        GSPProlog();
        InitParms();
        ArgList = ArgList->link;
    }
    while( ArgList != NULL ) {
        skip_label = NextLabel();
        GBranch( skip_label );
        GEPProlog();
        InitParms();
        GLabel( skip_label );
        FreeLabel( skip_label );
        ArgList = ArgList->link;
    }
}

void Epilogue( void )
{
// Generate an epilogue.

    if( ( SgmtSw & SG_PROLOG_DONE ) == 0 ) {
        Prologue();
    }
    if( SubProgId != NULL ) {
        GEpilog();
    }
}

void CpBlockData( void )
{
    sym_id  sym_ptr;

    CkSubEnd();
    ProgSw |= PS_IN_SUBPROGRAM | PS_BLOCK_DATA;
    if( RecName() ) {
        sym_ptr = LkSym();
        sym_ptr->u.ns.flags = SY_USAGE | SY_SUBPROGRAM | SY_BLOCK_DATA |
                            SY_PENTRY | SY_REFERENCED;
    } else {
        ReqNOpn();
        sym_ptr = LkBlkData();
    }
    SubProgId = sym_ptr;
    GBlockLabel();
    AdvanceITPtr();
    ReqEOS();
    BIStartBlockData( SubProgId );
}

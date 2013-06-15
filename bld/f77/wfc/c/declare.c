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


//
// DECLARE   : symbol declaration processor
//

#include "ftnstd.h"
#include "opn.h"
#include "opr.h"
#include "errcod.h"
#include "segsw.h"
#include "progsw.h"
#include "ctrlflgs.h"
#include "namecod.h"
#include "global.h"
#include "stmtsw.h"
#include "recog.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"

#include <string.h>

extern  void            Function( TYPE, uint, bool );
extern  act_dim_list    *STSubsList( act_dim_list * );
extern  sym_id          LkSym( void );
extern  bool            LenSpec( TYPE, uint * );
extern  warp_label      GBegSList( void );
extern  void            GSLoBound( int,sym_id );
extern  void            GSHiBound( int,sym_id );
extern  void            GForceHiBound( int, sym_id );
extern  void            GEndSList( sym_id );
extern  void            DataInit( itnode * );
extern  void            RemKeyword( itnode *, int );
extern  void            CkDefStmtNo( void );
extern  void            DefProg( void );
extern  void            FreeWarpLabel( warp_label );
extern  sym_id          STField( char *, uint );

extern  char            *StmtKeywords[];

/* Forward declarations */
void    ArrayDecl( sym_id sym );


#define ERR_MASK                (SY_DATA_INIT | SY_CLASS | SY_USAGE)
#define SSB_CONSTANT            0    // lo/hi subscript bound is constant
#define SSB_NOT_CONSTANT        1    // lo/hi subscript bound not constant
#define SSB_ERROR               2    // lo/hi subscript bound error


bool    IsFunctionDefn( void ) {
//========================

// Check to see if type declaration is a function definition.

    if( !RecName() ) return( FALSE );
    if( memcmp( StmtKeywords[ PR_FUNC ], CITNode->opnd, 8 ) ) return( FALSE );
    // allow INTEGER FUNCTION(10)
    if( CITNode->opnd_size == 8 ) return( FALSE );
    // allow "INTEGER FUNCTIONA"
    if( RecNextOpr( OPR_TRM ) ) return( FALSE );
    // We now have INTEGER[*lenspec] FUNCTION name ...
    // allow INTEGER FUNCTIONA,
    if( StmtSw & SS_COMMA_FOUND ) return( FALSE );
    RemKeyword( CITNode, 8 );
    return( TRUE );
}


sym_id  VarDecl( TYPE typ ) {
//===========================

// Process variable declaration statement according to type parameter.

    unsigned_16 flags;
    unsigned_16 sp_type;
    unsigned_16 class;
    sym_id      sym;

    sym = LkSym();
    flags = sym->ns.flags;
    if( flags & SY_TYPE ) {
        NameTypeErr( TY_TYP_PREV_DEF, sym );
    } else {
        class = flags & SY_CLASS;
        if( ( class == SY_COMMON ) || ( class == SY_PARAMETER ) ) {
            IllName( sym );
        } else {
            if( class == SY_SUBPROGRAM ) {
                sp_type = flags & SY_SUBPROG_TYPE;
                if( sp_type != 0 ) {
                    if( sp_type != SY_FUNCTION ) {
                        IllName( sym );
                        AdvanceITPtr();
                        return( sym );
                    } else if( flags & SY_INTRINSIC ) {
                        if( sym->ns.u1.s.typ != typ ) {
                            NameTypeErr( TY_TYP_PREV_DEF, sym );
                            AdvanceITPtr();
                            return( sym );
                        }
                    }
                } else {
                    flags |= SY_FUNCTION;
                }
            } else if( flags & SY_IN_DIMEXPR ) {
                if( !_IsTypeInteger( typ ) ) {
                    NameTypeErr( TY_TYP_PREV_DEF, sym );
                    AdvanceITPtr();
                    return( sym );
                }
            }
            sym->ns.flags = flags | SY_TYPE;
            sym->ns.u1.s.typ = typ;
            sym->ns.xt.size = TypeSize( typ );
        }
    }
    AdvanceITPtr();
    return( sym );
}


sym_id  FieldDecl( void ) {
//===================

// Define a field in a structure.

    sym_id      sym;

    sym = STField( CITNode->opnd, CITNode->opnd_size );
    AdvanceITPtr();
    return( sym );
}


TYPE    MapTypes( TYPE typ, uint size ) {
//======================================

// Given a type and size, return an equivalent type.
// For example REAL*8 is equivalent to DOUBLE PRECISION.

    if( typ == FT_REAL ) {
        switch( size ) {
        case( sizeof( double ) ):       return( FT_DOUBLE );
        // kludge until long doubles are implemented.
        case( 16 ):                     return( FT_EXTENDED );
        default:        return( typ );
        }
    }
    if( typ == FT_COMPLEX ) {
        switch( size ) {
        case( sizeof( dcomplex ) ):     return( FT_DCOMPLEX );
        // cludge until long doubles are implemented.
        case( 32 ):                     return( FT_XCOMPLEX );
        default:        return( typ );
        }
    }
    if( typ == FT_INTEGER ) {
        switch( size ) {
        case( sizeof( intstar2 ) ):     return( FT_INTEGER_2 );
        case( sizeof( intstar1 ) ):     return( FT_INTEGER_1 );
        default:        return( typ );
        }
    }
    if( typ == FT_LOGICAL ) {
        switch( size ) {
        case( sizeof( logstar1 ) ):     return( FT_LOGICAL_1 );
        default:        return( typ );
        }
    }
    return( typ );
}

void    MustBeTypeDecl( void ) {
//========================

// Called when type declaration is not a function definition.

    SgmtSw |= SG_NO_MORE_IMPLICIT;
    CkDefStmtNo();
    if( ( SgmtSw & SG_STMT_PROCESSED ) == 0 ) {
        CtrlFlgs &= ~CF_SUBPROGRAM;        // not TYPE*LEN FUNCTION
        DefProg();
    }
}

static  void    TypeDecl( TYPE typ ) {
//====================================

// Process a type declaration statement.

    uint        default_size;
    itnode      *var_node;
    bool        len_spec;
    sym_id      sym;
    uint        size = ~0;

    default_size = StorageSize( typ );
    if( RecNOpn() ) {
        AdvanceITPtr();
        ReqMul();
    }
    len_spec = LenSpec( typ, &size );
    if( IsFunctionDefn() ) {
        Function( typ, size, len_spec );
    } else {
        MustBeTypeDecl();
        if( len_spec ) { // if TYPE*LEN
            default_size = size;
            if( !RecNOpr() && !RecCloseParen() ) {
                ReqComma();
            }
        }
        for(;;) {
            size = ~0;
            if( ReqName( NAME_VAR_OR_ARR ) ) {
                var_node = CITNode;
                if( SgmtSw & SG_DEFINING_STRUCTURE ) {
                    sym = FieldDecl();
                } else {
                    sym = VarDecl( MapTypes( typ, default_size ) );
                }
                len_spec = LenSpec( typ, &size );
                if( RecOpenParen() ) {
                    if( len_spec && ( typ == FT_CHAR ) ) {
                        Extension( TY_CHAR_BEFORE_PAREN );
                    }
                    ArrayDecl( sym );
                    if( !len_spec ) {
                        len_spec = LenSpec( typ, &size );
                    }
                }
                if( !len_spec ) {
                    size = default_size;
                }
                if( SgmtSw & SG_DEFINING_STRUCTURE ) {
                    sym->fd.typ = MapTypes( typ, size );
                    sym->fd.xt.size = size;
                    if( sym->fd.dim_ext != NULL ) {
                        size *= sym->fd.dim_ext->num_elts;
                    }
                    if( (typ == FT_CHAR) && (size == 0) ) {
                        NameErr( CV_CHARSTAR_ILLEGAL, sym );
                    }
                } else {
                    sym->ns.u1.s.typ = MapTypes( typ, size );
                    sym->ns.xt.size = size;
                    if( RecDiv() || RecCat() ) {
                        StmtExtension( DA_IN_TYPE_STMT );
                        DataInit( var_node );
                    }
                }
            } else {
                AdvanceITPtr();
            }
            if( !RecComma() ) break;
        }
        ReqEOS();
    }
}


void    CpCharVar( void ) {
//===================

// Process CHARACTER variable declaration statement.

    TypeDecl( FT_CHAR );
}


void    CpIntVar( void ) {
//==================

// Process INTEGER variable declaration statement.

    TypeDecl( FT_INTEGER );
}


void    CpRealVar( void ) {
//===================

// Process REAL variable declaration statement.

    TypeDecl( FT_REAL );
}


void    CpDbleVar( void ) {
//===================

// Process DOUBLE PRECISION variable declaration statement.

    TypeDecl( FT_DOUBLE );
}


void    CpXDbleVar( void ) {
//====================

// Process EXTENDED PRECISION variable declaration statement.

    TypeDecl( FT_EXTENDED );
}


void    CpCmplxVar( void ) {
//====================

// Process COMPLEX variable declaration statement.

    TypeDecl( FT_COMPLEX );
}


void    CpDCmplxVar( void ) {
//=====================

// Process DOUBLE COMPLEX variable declaration statement.

    TypeDecl( FT_DCOMPLEX );
}


void    CpXCmplxVar( void ) {
//=====================

// Process EXTENDED COMPLEX variable declaration statement.

    TypeDecl( FT_XCOMPLEX );
}


void    CpLogVar( void ) {
//==================

// Process LOGICAL variable declaration statement.

    TypeDecl( FT_LOGICAL );
}



void    CpDimension( void ) {
//=====================

// Process the DIMENSION statement.

    sym_id    sym;

    for(;;) {
        if( ReqName( NAME_ARRAY ) ) {
            sym = LkSym();
            AdvanceITPtr();
            if( ReqOpenParen() ) {
                ArrayDecl( sym );
            }
        } else {
            AdvanceITPtr();
        }
        if( !RecComma() ) break;
    }
    ReqEOS();
}


void    ArrayDecl( sym_id sym ) {
//===============================

// Process an array declarator.

    intstar4            *subs_ptr;
    intstar4            lo_bound;
    intstar4            hi_bound;
    signed_32           num_elts;
    unsigned_32         dim_elts;
    uint                num_subs;
    bool                var_dim;
    bool                const_lo;
    bool                assumed;
    bool                pvd_ok;
    bool                allocatable;
    act_dim_list        dim_list;

    dim_list.num_elts = 0;
    dim_list.dim_flags = 0;
    dim_list.l.init_label = 0;
    allocatable = RecNOpn() && RecNextOpr( OPR_COL );
    if( ( SgmtSw & SG_DEFINING_STRUCTURE ) == 0 ) {
        if( ( sym->ns.flags & ERR_MASK ) != SY_VARIABLE ) {
            IllName( sym );
            return;
        } else if( allocatable ) {
            if( sym->ns.flags & ( SY_IN_EC | SY_SUB_PARM ) ) {
                IllName( sym );
                return;
            }
        }
        sym->ns.si.va.u.dim_ext = &dim_list;
        if( ( ProgSw & PS_IN_SUBPROGRAM ) &&
            !( ProgSw & PS_BLOCK_DATA ) && !allocatable ) {
            dim_list.l.init_label = GBegSList();
        }
    }
    subs_ptr = &dim_list.subs_1_lo;
    num_elts = 1;
    num_subs = 0;
    assumed = FALSE;
    var_dim = FALSE;
    for(;;) {
        num_subs++;
        pvd_ok = FALSE;
        hi_bound = 0;
        lo_bound = 1;
        assumed = RecNOpn() && RecNextOpr( OPR_MUL );
        if( allocatable ) {
            ReqNOpn();
            AdvanceITPtr();
            ReqColon();
            ReqNOpn();
        } else if( !assumed ) {
            DimExpr();
            if( AError ) {
                const_lo = SSB_ERROR;
            } else if( CITNode->opn.us == USOPN_CON ) {
                lo_bound = ITIntValue( CITNode );
                hi_bound = lo_bound - 1;
                const_lo = SSB_CONSTANT;
            } else {
                const_lo = SSB_NOT_CONSTANT;
            }
            if( RecNextOpr( OPR_COL ) ) {
                if( const_lo == SSB_CONSTANT ) {
                    _SetLoConstBound( dim_list.dim_flags, num_subs );
                } else if( const_lo == SSB_NOT_CONSTANT ) {
                    GSLoBound( num_subs, sym );
                    var_dim = TRUE;
                }
                AdvanceITPtr();
                assumed = RecNOpn() && RecNextOpr( OPR_MUL );
                if( assumed ) {
                    AdvanceITPtr();
                    ReqNOpn();
                } else {
                    DimExpr();
                    if( !AError ) {
                        if( CITNode->opn.us == USOPN_CON ) {
                            hi_bound = ITIntValue( CITNode );
                            if( const_lo == SSB_NOT_CONSTANT ) {
                                lo_bound = hi_bound + 1;
                                GForceHiBound( num_subs, sym );
                            } else if( const_lo == SSB_CONSTANT ) {
                                if( lo_bound <= hi_bound ) {
                                    dim_elts = hi_bound - lo_bound + 1;
#if _CPU == 8086
                                    if( dim_elts > 65535 ) {
                                        Error( SV_DIMENSION_LIMIT );
                                    }
#endif
                                    num_elts *= dim_elts;
                                    if( num_elts <= 0 ) {
                                        Error( SV_BAD_SSCR );
                                    }
                                } else {
                                    Error( SV_BAD_SSCR );
                                }
                            }
                        } else {
                            if( const_lo == SSB_CONSTANT ) {
                                hi_bound = lo_bound - 1;
                            }
                            GSHiBound( num_subs, sym );
                            var_dim = TRUE;
                        }
                    }
                }
            } else {
                _SetLoConstBound( dim_list.dim_flags, num_subs );
                if( const_lo == SSB_NOT_CONSTANT ) {
                    GSHiBound( num_subs, sym );
                    var_dim = TRUE;
                } else if( const_lo == SSB_CONSTANT ) {
                    pvd_ok = ( lo_bound == 1 );
                    hi_bound = lo_bound;
                    lo_bound = 1;
                    if( hi_bound > 0 ) {
#if _CPU == 8086
                        if( hi_bound > 65535 ) {
                            Error( SV_DIMENSION_LIMIT );
                        }
#endif
                        num_elts *= hi_bound;
                        if( num_elts <= 0 ) {
                            Error( SV_BAD_SSCR );
                        }
                    } else {
                        Error( SV_BAD_SSCR );
                    }
                }
            }
        } else {
            _SetLoConstBound( dim_list.dim_flags, num_subs );
            AdvanceITPtr();
            ReqNOpn();
        }
        *subs_ptr = lo_bound;
        subs_ptr++;
        *subs_ptr = hi_bound;
        subs_ptr++;
        AdvanceITPtr();
        if( !RecComma() || assumed || ( num_subs == MAX_DIM ) ) break;
    }
    _SetDimCount( dim_list.dim_flags, num_subs );
    ReqCloseParen();
    ReqNOpn();
    AdvanceITPtr();
    if( SgmtSw & SG_DEFINING_STRUCTURE ) {
        if( var_dim || assumed ) {
            FieldErr( SV_ARR_PARM, sym );
        } else if( allocatable ) {
            Error( SP_ALLOC_NOT_IN_STRUCT );
        } else {
            dim_list.num_elts = num_elts;
            sym->fd.dim_ext = STSubsList( &dim_list );
        }
    } else {
        if( (ProgSw & PS_IN_SUBPROGRAM) && !(ProgSw & PS_BLOCK_DATA) &&
            pvd_ok ) {
            dim_list.num_elts = num_elts;
            // for Psuedo-Variable Dimensioning ( WATFIVish )
            dim_list.dim_flags |= DIM_PVD;
            if( sym->ns.flags & SY_SUB_PARM ) {
                NameExt( SV_PVD, sym );
                dim_list.dim_flags |= DIM_ASSUMED;
            }
            GEndSList( sym );
        } else if( assumed ) {
            // check for 'assumed' before checking for 'var_dim' otherwise
            // DIMENSION A(N,*) will not be processed as an assumed array
            dim_list.dim_flags |= DIM_ASSUMED;
            GEndSList( sym );
        } else if( var_dim ) {
            dim_list.dim_flags |= DIM_VARIABLE;
            GEndSList( sym );
        } else if( allocatable ) {
            Extension( VA_ALLOCATABLE_STORAGE, sym->ns.name );
            dim_list.dim_flags |= DIM_ALLOCATABLE;
            sym->ns.u1.s.xflags |= SY_ALLOCATABLE;
        } else {
            dim_list.num_elts = num_elts;
            if( sym->ns.flags & SY_SUB_PARM ) {
                // we don't want an ADV generated in the following case:
                //      SUBROUTINE SAM( A )
                //      DIMENSION A(10)
                //          ...
                //      END
                FreeWarpLabel( dim_list.l.init_label );
                dim_list.l.init_label = 0;
            } else if( dim_list.l.init_label != 0 ) {
                FreeWarpLabel( dim_list.l.init_label );
                dim_list.l.init_label = 0;
            }
        }
        sym->ns.si.va.u.dim_ext = STSubsList( &dim_list );
        sym->ns.flags |= ( SY_USAGE | SY_SUBSCRIPTED );
    }
}

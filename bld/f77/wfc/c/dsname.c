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
* Description:  Downscan a name.
*
****************************************************************************/


#include "ftnstd.h"
#include "opr.h"
#include "opn.h"
#include "astype.h"
#include "errcod.h"
#include "segsw.h"
#include "stmtsw.h"
#include "iflookup.h"
#include "global.h"
#include "cpopt.h"
#include "namecod.h"
#include "recog.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"

#include <string.h>

extern  void            BIOutSymbol( sym_id ste_ptr );
extern  void            ScanExpr( void );
extern  void            AdvError(int);
extern  sym_id          LkSym( void );
extern  sym_id          LkField(sym_id);
extern  sym_id          FindShadow(sym_id);
extern  sym_id          FindStruct(char *,int);

/* Forward declarations */
static  void    CkFieldNoList( void );
static  void    ChkStructName( void );
static  void    CkNameNoList( void );
static  void    SetTypeUsage( unsigned_16 type_usage );
// Rename  Function -> ScanningFunction to prevent clash with
// other external Function declaration
static  void    ScanningFunction( void );
static  void    SubProg( void );
static  void    CkIntrinsic( void );
void             CkTypeDeclared( void );



bool    SubStrung( void ) {
//===================

// Determine whether name is substrung or not.

    itnode      *save_citnode;
    OPR         opr;

    save_citnode = CITNode;
    CITNode = CITNode->link;
    ScanExpr();
    opr = CITNode->opr;
    CITNode = save_citnode;
    return( opr == OPR_COL );
}


void    DSName( void ) {
//================

// Downscan a name.

    sym_id      sym_ptr;

    CITNode->opn.us = USOPN_NNL;
    if( RecNextOpr( OPR_LBR ) ) {
        CITNode->link->opr = OPR_FBR;
        CITNode->opn.us = USOPN_NWL;
    }
    if( ( FieldNode != NULL ) &&
        ( ( CITNode->opr == OPR_FLD ) || ( CITNode->opr == OPR_DPT ) ) ) {
        if( FieldNode->opn.us & USOPN_FLD ) {
            LkField( FieldNode->sym_ptr->fd.xt.sym_record );
        } else {
            LkField( FieldNode->sym_ptr->ns.xt.sym_record );
        }
        CITNode->opn.us |= USOPN_FLD;
        if( CITNode->sym_ptr != NULL ) { // make sure field name exists
            if( CITNode->sym_ptr->fd.dim_ext != NULL ) { // field is an array
                if( ( CITNode->opn.us & USOPN_WHAT ) != USOPN_NWL ) {
                    CITNode->opn.us &= ~USOPN_WHAT;
                    CITNode->opn.us |= USOPN_ARR;
                    CkFieldNoList();
                }
            } else if( ( CITNode->opn.us & USOPN_WHAT ) == USOPN_NWL ) {
                // field better be character and substrung
                if( ( CITNode->sym_ptr->fd.typ != FT_CHAR ) || !SubStrung() ) {
                    AdvError( PC_SURP_PAREN );
                }
            }
            // setup FieldNode for the next field lookup
            if( CITNode->sym_ptr->fd.typ == FT_STRUCTURE ) {
                // chain fields for CkFieldNoList()
                CITNode->value.sc.struct_chain = FieldNode;
                FieldNode = CITNode; // must come after LkField()
            } else {
                // this field is not structured so go back to the previous
                // FieldNode
                // consider:    STRUCTURE /S1/
                //                  INTEGER J
                //              END STRUCTURE
                //              STRUCTURE /S2/
                //                  RECORD /S1/ A(10)
                //              END STRUCTURE
                //              RECORD /S2/ X
                //              RECORD /S1/ I
                //              X%A(I%J)%K
                // when we are done with I%J, set FieldNode back to "A" so we
                // can compute offset of "K" (see ChkStructName())
                //
                // we must go back as many as necessary, i.e. a(b.c.d.e).f
                while( (FieldNode->opr==OPR_DPT) || (FieldNode->opr==OPR_FLD) ){
                        FieldNode = FieldNode->value.sc.struct_chain;
                }
                FieldNode = FieldNode->value.sc.struct_chain;
            }
        }
        return;
    }
    if( ASType & AST_ISIZEOF ) {
        //      Case 1:                         Case 2:
        //      STRUCTURE /XXX/                 STRUCTURE /XXX/
        //          INTEGER I                       INTEGER I
        //      END STRUCTURE                   END STRUCTURE
        //      INTEGER XXX                     PRINT *, ISIZEOF( XXX )
        //      PRINT *, ISIZEOF( XXX )

        // Case 1:
        // We want to issue an "unreferenced" error for XXX so don't set
        // SY_REFERENCED bit in symbol table.
        // Case 2:
        // We don't want to issue "undeclared type" error for XXX if XXX
        // did not appear in a type declaration statement.

        sym_id  sd;

        ASType &= ~AST_ISIZEOF;
        sd = FindStruct( CITNode->opnd, CITNode->opnd_size );
        if( sd != NULL ) {
            CITNode->opn.us = USOPN_CON;
            CITNode->typ = FT_STRUCTURE;
            CITNode->value.intstar4 = sd->sd.size;
            return;
        }
    }
    sym_ptr = LkSym();
    if( ( ASType & AST_DEXP ) && ( sym_ptr != InitVar ) &&
        ( ( sym_ptr->ns.flags & SY_CLASS ) != SY_PARAMETER ) &&
        ( ( ( sym_ptr->ns.flags & SY_CLASS ) != SY_VARIABLE ) ||
            ( ( sym_ptr->ns.flags & SY_SPECIAL_PARM ) == 0 ) ) ) {
        NameErr( DA_BAD_VAR_IN_EXPR, sym_ptr );
    }
    if( ClassIs( SY_VARIABLE ) ) {
        ChkStructName();
        if( Subscripted() ) {    // if dimensioned
            if( ASType & AST_DIM ) {
                ClassErr( DM_SYM_PARM, sym_ptr );
            } else if( !RecNWL() ) {
                CITNode->opn.us = USOPN_ARR; // array without subscript list
                CkNameNoList();
            }
            SetTypeUsage( SY_TYPE | SY_USAGE );
        } else if( RecNWL() ) {     // if name with list, not dimensioned
            if( ASType & AST_DIM ) {
                IllName( sym_ptr );
            } else if( (CITNode->typ == FT_CHAR) && SubStrung() ) {
                SetTypeUsage( SY_TYPE | SY_USAGE );
            } else {
                ScanningFunction();
            }
        } else {
            if( ASType & AST_DIM ) {
                CITNode->flags |= SY_IN_DIMEXPR;
                SetTypeUsage( SY_USAGE );
            } else {
                SetTypeUsage( SY_TYPE | SY_USAGE );
            }
        }
    } else if( ClassIs( SY_SUBPROGRAM ) ) {
        ChkStructName();
        if( ASType & AST_DIM ) {
            ClassErr( DM_SYM_PARM, sym_ptr );
        } else {
            SubProg();
        }
    } else { // if( ClassIs( SY_PARAMETER ) ) {
        if( RecNWL() ) {
            IllName( sym_ptr );
        } else {
            CITNode->opn.us = USOPN_CON;
            CITNode->sym_ptr = sym_ptr->ns.si.pc.value;
            if( CITNode->typ == FT_CHAR ) {
                if( StmtSw & SY_DATA_INIT ) {
                    CITNode->sym_ptr->lt.flags |= LT_DATA_STMT;
                } else {
                    CITNode->sym_ptr->lt.flags |= LT_EXEC_STMT;
                }
                CITNode->value.cstring.strptr = (char *)&CITNode->sym_ptr->lt.value;
                CITNode->value.cstring.len = CITNode->sym_ptr->lt.length;
            } else {
                memcpy( &CITNode->value, &CITNode->sym_ptr->cn.value,
                        CITNode->size );
            }
        }
    }
    BIOutSymbol( sym_ptr );
    if( ( sym_ptr->ns.flags & SY_REFERENCED ) == 0 ) {
        if( ASType & AST_ASF ) {
            if( sym_ptr != SFSymId ) {
                sym_ptr->ns.flags |= SY_REFERENCED;
            }
        } else if( ( ASType & AST_DEXP ) == 0 ) {
            if( SgmtSw & SG_SYMTAB_RESOLVED ) {
                sym_ptr->ns.flags |= SY_REFERENCED;
            } else if( ASType & ( AST_DIM | AST_CEX | AST_DIEXP ) ) {
                sym_ptr->ns.flags |= SY_REFERENCED;
            }
        }
    }
}


static  void    ChkStructName( void ) {
//===============================

    if( CITNode->typ == FT_STRUCTURE ) {
        // save the current FieldNode
        // consider:            STRUCTURE /S1/
        //                          INTEGER J
        //                      END STRUCTURE
        //                      STRUCTURE /S2/
        //                          RECORD /S1/ A(10)
        //                      END STRUCTURE
        //                      RECORD /S2/ X
        //                      RECORD /S1/ I
        //                      X%A(I%J)%K
        // when we are about to process I%J, save FieldNode for "A" so we can
        // later compute the offset of "K"
        CITNode->value.sc.struct_chain = FieldNode;
        FieldNode = CITNode;
    }
}


void    GetFunctionShadow( void ) {
//===========================

    sym_id      fn_shadow;

    // update type in function shadow symbol in case an
    // IMPLICIT statement changed the type of the function
    fn_shadow = FindShadow( CITNode->sym_ptr );
    fn_shadow->ns.u1.s.typ = CITNode->sym_ptr->ns.u1.s.typ;
    if( fn_shadow->ns.u1.s.typ == FT_STRUCTURE ) {
        fn_shadow->ns.xt.record = CITNode->sym_ptr->ns.xt.record;
    } else {
        fn_shadow->ns.xt.size = CITNode->sym_ptr->ns.xt.size;
    }
    CITNode->sym_ptr = fn_shadow;
    CITNode->flags = fn_shadow->ns.flags;
}


static  void    SubProg( void ) {
//=========================

// Make sure subprograms are used correctly.

    unsigned_16 sp_type;

    sp_type = CITNode->flags & SY_SUBPROG_TYPE;
    if( ( sp_type == SY_REMOTE_BLOCK ) || ( sp_type == SY_PROGRAM ) ) {
        IllName( CITNode->sym_ptr );
    } else if( sp_type == SY_STMT_FUNC ) {
        if( RecNWL() ) {
            if( ASType & AST_ASF ) { // if defining s. f.
                if( CITNode->sym_ptr == SFSymId ) {
                    Error( SR_TRIED_RECURSION ); // check recursion
                }
            }
        } else {
            if( ( ASType & AST_ASF ) == 0 ) { // if not defining s. f.
                IllName( CITNode->sym_ptr );
            }
        }
    } else if( sp_type == SY_SUBROUTINE ) {
        if( RecNWL() ) {
            if( ( StmtProc == PR_CALL ) && RecTrmOpr() ) {
                if( ( CITNode->flags & SY_PS_ENTRY ) != 0 ) {
                    Extension( SR_TRIED_RECURSION );
                }
            } else {
                IllName( CITNode->sym_ptr );
            }
        } else if( ( ASType & AST_CNA ) == 0 ) {
            CkNameNoList();
        } else if( ( CITNode->flags & SY_PS_ENTRY ) != 0 ) {
            Extension( SR_TRIED_RECURSION );
        }
    } else if( sp_type == SY_FUNCTION ) {
        if( RecNWL() && SubStrung() && (CITNode->typ == FT_CHAR) &&
            (CITNode->flags & SY_PS_ENTRY) ) {
            GetFunctionShadow();
        } else if( !RecNWL() && !(ASType & AST_CNA) ) {
            if( CITNode->flags & SY_PS_ENTRY ) {
                GetFunctionShadow();
            } else {
                CkNameNoList();
            }
        } else if( ( CITNode->flags & SY_PS_ENTRY ) != 0 ) {
            Extension( SR_TRIED_RECURSION );
        } else if( CITNode->flags & SY_INTRINSIC ) {
            if( CITNode->sym_ptr->ns.si.fi.index == IF_ISIZEOF ) {
                ASType |= AST_ISIZEOF;
            }
        }
    } else if( sp_type == SY_FN_OR_SUB ) {
        if( RecNWL() ) {                        // if it's in a CALL statement
            CITNode->flags |= SY_FUNCTION;      // the class will already be
            SetTypeUsage( SY_TYPE | SY_USAGE ); // SUBROUTINE and we won't
        } else {                                // be in this part of the code
            CkNameNoList();
        }
    }
}


static  void    ScanningFunction( void ) {
//==========================

// Must be scanning a function.

    if( CITNode->flags & SY_PS_ENTRY ) {
        Extension( SR_TRIED_RECURSION );
    } else if( CITNode->flags & ( SY_USAGE | SY_DO_PARM | SY_IN_EC ) ) {
        IllName( CITNode->sym_ptr );
    } else if( CITNode->flags & SY_SAVED ) {
        Error( SA_SAVED );
    } else {
        CkIntrinsic();
        CITNode->flags |= SY_SUBPROGRAM | SY_FUNCTION;
        SetTypeUsage( SY_TYPE | SY_USAGE );
    }
}


static  void    CkIntrinsic( void ) {
//=============================

// Check for intrinsic functions.
//
//     CASE 1: integer abs
//             y == abs( -1.0 )    -- this should call generic i.f. abs
//
//     CASE 2: real iabs
//             y == iabs( -1 )     -- this should give type mismatch error
//

    sym_id      sym_ptr;
    TYPE        typ;
    IFF         func;

    sym_ptr = CITNode->sym_ptr;
    if( ( CITNode->flags & SY_SUB_PARM ) == 0 ) {
        typ = CITNode->typ;
        func = IFLookUp();
        if( func > 0 ) {
            sym_ptr->ns.si.fi.index = func;
            if( func == IF_ISIZEOF ) {
                ASType |= AST_ISIZEOF;
            }
            sym_ptr->ns.si.fi.u.num_args = 0;
            CITNode->flags |= SY_INTRINSIC;
            IFChkExtension( func );
            if( !IFIsGeneric( func ) ) {
                CITNode->typ = IFType( func );
                CITNode->size = TypeSize( CITNode->typ );
                sym_ptr->ns.u1.s.typ = CITNode->typ;
                sym_ptr->ns.xt.size = CITNode->size;
                if( ( CITNode->typ != typ ) && ( CITNode->flags & SY_TYPE ) ) {
                    Error( LI_WRONG_TYPE );
                }
            }
        }
    }
}


static  void    CkNameNoList( void ) {
//==============================

// Check that array/subprogram with no list is alright.

    if( ( ASType & AST_IO ) && RecTrmOpr() && RecNextOpr( OPR_TRM ) ) {
        if( ( CITNode->opn.us & USOPN_WHAT ) != USOPN_ARR ) {
            ClassErr( SV_NO_LIST, CITNode->sym_ptr );
        }
        return;
    }
    if( ( !RecNextOpr( OPR_COM ) && !RecNextOpr( OPR_RBR ) ) ||
        ( !RecComma() && !RecFBr() ) ) {
        ClassErr( SV_NO_LIST, CITNode->sym_ptr );
    }
}


static  void    CkFieldNoList( void ) {
//===============================

// Check that array field with no list is alright.

    itnode      *opr_node;

    // find the node that contains the structured symbol
    opr_node = FieldNode;
    while( opr_node->value.sc.struct_chain != NULL ) {
        opr_node = opr_node->value.sc.struct_chain;
    }
    if( ( ASType & AST_IO ) && ( opr_node->opr == OPR_TRM ) &&
        RecNextOpr( OPR_TRM ) ) {
        if( ( CITNode->opn.us & USOPN_WHAT ) != USOPN_ARR ) {
            KnownClassErr( SV_NO_LIST, NAME_ARRAY );
        }
        return;
    }
    if( ( !RecNextOpr( OPR_COM ) && !RecNextOpr( OPR_RBR ) ) ||
        ( ( opr_node->opr != OPR_COM ) && ( opr_node->opr != OPR_FBR ) ) ) {
        KnownClassErr( SV_NO_LIST, NAME_ARRAY );
    }
}


static  void    SetTypeUsage( unsigned_16 type_usage ) {
//=====================================================

// Indicate that a names' use and type has been established.

    CITNode->sym_ptr->ns.flags = CITNode->flags;
    CkTypeDeclared();
    CITNode->flags |= type_usage;
    CITNode->sym_ptr->ns.flags |= type_usage;
}


void    CkTypeDeclared( void ) {
//========================

// Make sure type has been explicitly declared.

    unsigned_16 flags;

    if( ( SgmtSw & SG_IMPLICIT_NONE ) || ( Options & OPT_EXPLICIT ) ) {
        flags = CITNode->sym_ptr->ns.flags;
        if( ( flags & SY_TYPE ) == 0 ) {
            if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
                if( flags & SY_INTRINSIC ) return;
            }
            NameErr( TY_UNDECLARED, CITNode->sym_ptr );
        }
    }
}

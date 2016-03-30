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
* Description:  STRUCTURE/RECORD statement processor.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "segsw.h"
#include "namecod.h"
#include "errcod.h"
#include "recog.h"
#include "insert.h"
#include "utility.h"
#include "cpsubpgm.h"
#include "csutls.h"
#include "data.h"
#include "rststruc.h"
#include "proctbl.h"
#include "union.h"
#include "declare.h"


extern  char            *StmtKeywords[];


static  sym_id  StructName( void ) {
//============================

// Get structure name.

    sym_id      sd;

    ReqNOpn();
    AdvanceITPtr();
    ReqDiv();
    if( ReqName( NAME_STRUCTURE ) ) {
        sd = STStruct( CITNode->opnd, CITNode->opnd_size );
    } else {
        // so we don't have to check for NULL structures everywhere
        sd = STStruct( NULL, 0 );
    }
    AdvanceITPtr();
    ReqDiv();
    return( sd );
}


void    CpStructure( void ) {
//=====================

// Process STRUCTURE statement.
//      STRUCTURE /structure_name/

    CSExtn();
    CurrStruct = StructName();
    if( CurrStruct != NULL ) {
        if( CurrStruct->u.sd.fl.fields != NULL ) {
            StructErr( SP_STRUCT_DEFINED, CurrStruct );
            // consider:
            //      STRUCTURE /FOO/
            //          INTEGER K
            //      END STRUCTURE
            //      STRUCTURE /FOO/
            //          INTEGER K
            //      END STRUCTURE
            // we don't want to issue SP_DUPLICATE_FIELD errors.
            CurrStruct = STStruct( NULL, 0 );
        }
        SgmtSw |= SG_DEFINING_STRUCTURE;
    }
    EndOfStatement();
}


void    CpEndStructure( void ) {
//========================

// Process ENDSTRUCTURE statement.
//      ENDSTRUCTURE

    CSExtn();
    if( SgmtSw & SG_DEFINING_STRUCTURE ) {
        if( CurrStruct->u.sd.fl.fields == NULL ) {
            StructErr( SP_STRUCT_NEEDS_FIELD, CurrStruct );
            // consider:
            //  STRUCTURE /FOO/
            //  END STRUCTURE
            //  STRUCTURE /FOO/
            //      INTEGER K
            //  END STRUCTURE
            // we'd like to issue SP_STRUCT_DEFINED.  On the other hand
            // if a second definition of FOO isn't found then we would
            // issue an extra SP_UNDEF_STRUCT.
            STField( NULL, 0 );
        }
        SgmtSw &= ~SG_DEFINING_STRUCTURE;
    } else {
        StmtPtrErr( SP_UNMATCHED, StmtKeywords[ PR_STRUCTURE ] );
    }
    EndOfStatement();
}


void    CpRecord( void ) {
//==================

// Process RECORD statement.
//      RECORD /structure_name/ var_name [, var_name]

    sym_id      sym;
    itnode      *var_node;
    sym_id      sd;

    CSExtn();
    sd = StructName();
    if( IsFunctionDefn() ) {
        Function( FT_STRUCTURE, 0, TRUE );
        SubProgId->u.ns.xt.sym_record = sd;
    } else {
        MustBeTypeDecl();
        for(;;) {
            if( ReqName( NAME_VAR_OR_ARR ) ) {
                var_node = CITNode;
                if( SgmtSw & SG_DEFINING_STRUCTURE ) {
                    sym = FieldDecl();
                    sym->u.fd.typ = FT_STRUCTURE;
                    sym->u.fd.xt.sym_record = sd;
                } else {
                    sym = VarDecl( FT_STRUCTURE );
                    sym->u.ns.xt.sym_record = sd;
                }
                if( RecOpenParen() ) {
                    ArrayDecl( sym );
                }
                if( ( SgmtSw & SG_DEFINING_STRUCTURE ) == 0 ) {
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


void    StructResolve( void ) {
//=======================

// Make sure all structures have been defined.

    sym_id      sd;

    for( sd = RList; sd != NULL; sd = sd->u.sd.link ) {
        if( sd->u.sd.name_len == 0 ) continue; // NULL structure
        if( sd->u.sd.fl.fields != NULL ) {
            if( CalcStructSize( sd ) ) {
                StructErr( SP_STRUCT_RECURSION, sd );
            }
        } else {
            StructErr( SP_UNDEF_STRUCT, sd );
        }
    }
}

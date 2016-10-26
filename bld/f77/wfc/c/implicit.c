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
// IMPLICIT  : IMPLICIT statement processor
//

#include "ftnstd.h"
#include "opr.h"
#include "errcod.h"
#include "global.h"
#include "segsw.h"
#include "csetinfo.h"
#include "recog.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "charset.h"
#include "impltab.h"
#include "declare.h"
#include "proctbl.h"
#include "implicit.h"
#include "fmtcnvt.h"

#include <string.h>
#include <limits.h>


static  bool    ReqChar( void ) {
//=========================

// Recognize one character operand.

    if( RecName() && (CITNode->opnd_size == 1) && !CharSetInfo.is_foreign( *CITNode->opnd ) )
        return( true );
    Error( IM_ILLEGAL_RANGE );
    return( false );
}


static  bool            StarStar( TYPE typ ) {
//============================================

    if( typ != FT_CHAR )
        return( false );
    if( !RecNOpn() )
        return( false );
    if( !RecNextOpr( OPR_LBR ) )
        return( false );
    AdvanceITPtr();
    if( !RecNOpn() )
        return( false );
    if( !RecNextOpr( OPR_MUL ) )
        return( false );
    AdvanceITPtr();
    if( !RecNOpn() )
        return( false );
    if( !RecNextOpr( OPR_RBR ) )
        return( false );
    return( true );
}


static  bool    CheckSize( TYPE typ, intstar4 size, itnode *start ) {
//===================================================================

// Ensure that the length specification (SIZE) is valid for the
// given data type (TYP).

    itnode      *temp;

    if( (typ == FT_DOUBLE) || (typ == FT_DCOMPLEX) || (typ == FT_EXTENDED) || (typ == FT_XCOMPLEX) ) {
        temp = CITNode;
        CITNode = start; // get the caret in proper place
        TypeErr( TY_NOT_DBL_PREC, typ );
        CITNode = temp;
        return( false );
    }
    if( typ == FT_LOGICAL ) {
        if( size == sizeof( logstar1 ) )
            return( true );
        if( size == sizeof( logstar4 ) ) {
            return( true );
        }
    } else if( typ == FT_INTEGER ) {
        if( size == sizeof( intstar1 ) )
            return( true );
        if( size == sizeof( intstar2 ) )
            return( true );
        if( size == sizeof( intstar4 ) ) {
            return( true );
        }
    } else if( typ == FT_REAL ) {
        if( size == sizeof( single ) )
            return( true );
        if( size == sizeof( double ) )
            return( true );
        if( size == sizeof( extended ) ) {
            return( true );
        }
    } else if( typ == FT_COMPLEX ) {
        if( size == sizeof( scomplex ) )
            return( true );
        if( size == sizeof( dcomplex ) )
            return( true );
        if( size == sizeof( xcomplex ) ) {
            return( true );
        }
    } else if( typ == FT_CHAR ) {
#if _CPU == 8086
        if( (size > 0) && (size <= USHRT_MAX) ) {
            return( true );
        }
#else
        if( size > 0 ) {
            return( true );
        }
#endif
    }
    temp = CITNode;
    CITNode = start; // get the caret in proper place
    Error( TY_ILL_TYP_SIZE, size, TypeKW( typ ) );
    CITNode = temp;
    return( false );
}


bool    LenSpec( TYPE typ, uint *size_ptr ) {
//==========================================

// Process a length specification.

    itnode      *save_itptr;
    bool        len_spec;
    itnode      *temp;
    intstar4    ivalue;

    len_spec = false;
    if( RecMul() ) {
        save_itptr = CITNode;
        if( StarStar( typ ) ) {
            AdvanceITPtr();
            if( RecNOpn() ) {
                AdvanceITPtr();
            }
            *size_ptr = 0;
            if( StmtProc == PR_IMP ) {
                temp = CITNode;
                CITNode = save_itptr; // get the caret in proper place
                Extension( IM_CHAR_STAR_STAR );
                CITNode = temp;
            }
            len_spec = true;
        } else {
            CITNode = save_itptr;
            if( RecNOpn() && RecNextOpr( OPR_LBR ) ) {
                AdvanceITPtr();
                CIntExpr();
                ivalue = ITIntValue( CITNode );
                AdvanceITPtr();
                ReqCloseParen();
                if( RecNOpn() ) {
                    AdvanceITPtr();
                }
                len_spec = !AError;
            } else if( RecNumber() ) {
                FmtS2I( CITNode->opnd, CITNode->opnd_size, false, &ivalue, false, NULL );
                AdvanceITPtr();
                len_spec = true;
            } else {
                Error( SX_EXPECT_INT );
                AdvanceITPtr();
                len_spec = false;
            }
            if( len_spec ) {
                len_spec = CheckSize( typ, ivalue, save_itptr );
                if( len_spec ) {
                    *size_ptr = (uint)ivalue;
                }
            }
        }
    }
    if( len_spec && ( typ != FT_CHAR ) ) {
        temp = CITNode;
        CITNode = save_itptr;
        StmtExtension( TY_LEN_SPEC );
        CITNode = temp;
    }
    return( len_spec );
}


static  TYPE    RecTypeKW( void ) {
//=================================

// Recognize a type keyword (INTEGER, REAL, etc.).
// Assumptions: Types are consecutive starting at 0

    TYPE    typ;

    if( RecName() ) {
        for( typ = FT_LOGICAL_1; typ <= FT_CHAR; typ++ ) {
            if( CmpNode2Str( CITNode, TypeKW( typ ) ) ) {
                return( typ );
            }
        }
    }
    Error( IM_UNRECOG_TYPE );
    return( FT_NO_TYPE );
}


void    CpImplicit( void ) {
//====================

// Process the IMPLICIT statement.

    byte        chr1;
    byte        chr2;
    TYPE        typ;
    bool        valid_range;
    uint        size;

    if( (CITNode->opnd_size == 4) && (memcmp( CITNode->opnd, "NONE", 4 ) == 0) ) {
        AdvanceITPtr();
        Extension( SP_STRUCTURED_EXT, "IMPLICIT NONE" );
        if( SgmtSw & ( SG_IMPLICIT_STMT | SG_IMPLICIT_NONE ) ) {
            Error( IM_NONE_USED );
        }
        SgmtSw |= SG_IMPLICIT_NONE;
    } else {
        for( ;; ) {
            typ = RecTypeKW();
            AdvanceITPtr();
            if( ( typ != FT_NO_TYPE ) && !LenSpec( typ, &size ) ) {
                size = StorageSize( typ );
            }
            ReqOpenParen();
            for( ;; ) {
                valid_range = ReqChar();
                chr1 = *CITNode->opnd;
                AdvanceITPtr();
                if( RecMin() ) {
                    valid_range = valid_range && ReqChar();
                    chr2 = *CITNode->opnd;
                    if( valid_range ) {
                        if( chr1 >= chr2 ) {
                            Error( IM_ILLEGAL_RANGE );
                        }
                        AdvanceITPtr();
                    }
                } else {
                    chr2 = chr1;
                }
                if( valid_range ) {
                    typ = MapTypes( typ, size );
                    if( !SetImplType( chr1, chr2, typ, size ) ) {
                        Error( IM_PREV_IMPLICIT );
                    }
                }
                if( !RecComma() ) {
                    break;
                }
            }
            ReqCloseParen();
            ReqNOpn();
            AdvanceITPtr();
            if( !RecComma() ) {
                break;
            }
        }
        if( SgmtSw & SG_IMPLICIT_NONE ) {
            Error( IM_NONE_USED );
        }
        SgmtSw |= SG_IMPLICIT_STMT;
    }
    ReqEOS();
}

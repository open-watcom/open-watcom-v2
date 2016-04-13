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
* Description:  utilities for compiling
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "opr.h"
#include "opn.h"
#include "astype.h"
#include "cpopt.h"
#include "global.h"
#include "fmemmgr.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "frl.h"
#include "utility.h"
#include "aentry.h"
#include "downscan.h"

#include <string.h>


void    AdvanceITPtr( void ) {
//======================

    if( CITNode->link != NULL ) {
        CITNode = CITNode->link;
    }
}


void    FreeITNodes( itnode *head ) {
//===================================

// Free the internal text list until null link reached.

    itnode      *next;

    while( head != NULL ) {
        if( ( ( head->opn.us & USOPN_WHAT ) == USOPN_NWL ) ||
            ( ( head->opn.us & USOPN_WHAT ) == USOPN_ASS ) ) {
            if( head->list != NULL ) {
                FreeITNodes( head->list );
            }
        }
        next = head->link;
        FrlFree( &ITPool, head );
        head = next;
    }
}


void    ITPurge( void ) {
//=================

// Release all the internal text node structures.

    FreeITNodes( ITHead );
    ITHead = NULL;
    FrlFini( &ITPool );
}


void    FreeOneNode( itnode *itptr ) {
//====================================

// Free one itnode.

    itptr->link = NULL;
    FreeITNodes( itptr );
}


char    *MkNodeStr( itnode *itptr ) {
//===================================

// Put a NULLCHAR at the end of the "opnd" string of an itnode.

    int         opnd_size;
    char        *str;

    opnd_size = itptr->opnd_size;
    str = FMemAlloc( opnd_size + sizeof( char ) );
    memcpy( str, itptr->opnd, opnd_size );
    str[ opnd_size ] = NULLCHAR;
    return( str );
}


void    FrNodeStr( char *str ) {
//==============================

// Free memory allocated by MkNodeStr().

    FMemFree( str );
}


bool    CmpNode2Str( itnode *itptr, char *str ) {
//===============================================

// Compare the "opnd" field of an itnode to a string.

    int         str_len;

    str_len = strlen( str );
    if( itptr->opnd_size != str_len ) return( false );
    return( memcmp( itptr->opnd, str, str_len ) == 0 );
}


void    ProcExpr( void ) {
//==================

// Process an expression.

    ASType = AST_OFF;
    EatExpr();
}


bool TypeIs( TYPE typ ) {
//======================

// Return true if current itnode has specified variable type.

    switch( CITNode->typ ) {
    case( FT_INTEGER ):
    case( FT_INTEGER_2 ):
    case( FT_INTEGER_1 ):
        return( ( typ == FT_INTEGER ) ||
                ( typ == FT_INTEGER_2 ) ||
                ( typ == FT_INTEGER_1 ) );
    case( FT_LOGICAL ):
    case( FT_LOGICAL_1 ):
        return( ( typ == FT_LOGICAL ) ||
                ( typ == FT_LOGICAL_1 ) );
    default:
        return( CITNode->typ == typ );
    }
}


bool    ConstExpr( TYPE typ ) {
//============================

// Process a constant expression.

    ASType = AST_CEX;
    EatExpr();
    if( AError )
        return( false );
    if( CITNode->opn.us != USOPN_CON ) {
        Error( SX_NOT_CONST_EXPR );
        return( false );
    } else if( !TypeIs( typ ) && (typ != FT_NO_TYPE) ) {
        TypeErr( SX_WRONG_TYPE, typ );
        return( false );
    }
    return( true );
}


void    ChkType( TYPE typ ) {
//==========================

    if( !TypeIs( typ ) ) {
        TypeErr( SX_WRONG_TYPE, typ );
    }
}


void    DimExpr( void ) {
//=================

// Process a dimension expression.

    ASType = AST_DIM;
    EatExpr();
    ChkType( FT_INTEGER );
}


void    ProcSubExpr( void ) {
//=====================

// Process a subexpression.

    ASType = AST_SUB;
    EatExpr();
}


void    ProcDataExpr( void ) {
//======================

// Process an expression to be initialized in a DATA statement.

    ASType = AST_IO | AST_DEXP;
    EatExpr();
}


void    ProcDataRepExpr( void ) {
//=========================

// Process an expression used as a repeat specifier in a DATA statement.

    ASType = AST_DIEXP;
    GetIntConst();
}


void    ProcDataIExpr( void ) {
//=======================

// Process an expression used to initialize data in a DATA statement.

    ASType = AST_DIEXP;
    GetConst();
}


void    ProcIOExpr( void ) {
//====================

// Process an expression from an i/o list.

    ASType = AST_IO;
    EatExpr();
}


bool    CLogicExpr( void ) {
//====================

// Process a constant logical expression.

    return( ConstExpr( FT_LOGICAL ) );
}


bool    CCharExpr( void ) {
//===================

// Process a constant character expression.

    return( ConstExpr( FT_CHAR ) );
}


bool    CIntExpr( void ) {
//==================

// Process a constant integer expression.

    return( ConstExpr( FT_INTEGER ) );
}


void    CArithExpr( void ) {
//====================

    if( ConstExpr( FT_NO_TYPE ) &&
        ( ( CITNode->typ < FT_INTEGER_1 ) || ( CITNode->typ > FT_XCOMPLEX ) ) ) {
        Error( SX_NOT_SIMPLE_NUMBER );
    }
}


void    BoolExpr( void ) {
//==================

// Process an expression and check that it is boolean.

    ASType = AST_CCR;                   // flag to set condition codes
    EatExpr();
    if( _IsTypeInteger( CITNode->typ ) ) {
        Extension( SP_INTEGER_CONDITION );
    } else if( !_IsTypeLogical( CITNode->typ ) ) {
        TypeErr( SX_WRONG_TYPE, FT_LOGICAL );
    }
}


void    BoolSubExpr( void ) {
//=====================

// Process a subexpression and check that it is boolean.

    ASType = ( AST_CCR | AST_SUB );       // flag to set condition codes
    EatExpr();
    ChkType( FT_LOGICAL );
}


void    SelectExpr( void ) {
//====================

// Process an expression and check that it is LOGICAL, INTEGER or CHARACTER.

    ProcExpr();
    if( ( !_IsTypeLogical( CITNode->typ ) ) &&
        ( !_IsTypeInteger( CITNode->typ ) ) &&
        ( CITNode->typ != FT_CHAR ) ) {
        TypeErr( SX_WRONG_TYPE, FT_INTEGER );
        CITNode->typ = FT_NO_TYPE;
    }
}


void    IntegerExpr( void ) {
//=====================

// Process an expression and check that it is integer.

    ProcExpr();
    ChkType( FT_INTEGER );
}


void    IntSubExpr( void ) {
//====================

// Process a subexpression and check that it is integer.

    ProcSubExpr();
    ChkType( FT_INTEGER );
}


static  void    EatNumb( int ast ) {
//==================================

    ASType = ast;
    EatExpr();
    if( AError ) return;
    if( ( CITNode->typ < FT_INTEGER_1 ) || ( CITNode->typ > FT_EXTENDED ) ) {
        Error( SX_NOT_SIMPLE_NUMBER );
    }
}


void    EatDoParm( void ) {
//===================

// Process an expression and check that it is integer, real, or
// double precision.

    EatNumb( AST_SUB );
}


void    IfExpr( void ) {
//================

// Process an expression and check that it is integer, real, or double precision
// condition codes must be set for the arithmetic IF.

    EatNumb( AST_CCR );
}


void    CharSubExpr( void ) {
//=====================

// Process a character subexpression.

    ProcSubExpr();
    ChkType( FT_CHAR );
}


bool    BitOn( unsigned_16 bits ) {
//=================================

// Check if "flags" field of CITNode has ANY of specified BITS on.

    return( ( CITNode->flags & bits ) != 0 );
}


TYPE   Map2BaseType( TYPE typ ) {
//==============================

// Due to the three different integer sizes we use this function
// we use the equivalent function

    switch( typ ) {
    case( FT_INTEGER ):
    case( FT_INTEGER_2 ):
    case( FT_INTEGER_1 ):
        return( FT_INTEGER );
    case( FT_LOGICAL ):
    case( FT_LOGICAL_1 ):
        return( FT_LOGICAL );
    default:
        return( typ );
    }
}


bool    ClassIs( unsigned_16 class ) {
//====================================

// Return true if current itnode has specified variable class.
// ( SY_COMMON, SY_SUBPROGRAM, ... )

    return( ( CITNode->flags & SY_CLASS ) == class );
}


bool    Subscripted( void ) {
//=====================

// Return true if current itnode is a subscripted variable.

    return( ClassIs( SY_VARIABLE ) && (CITNode->flags & SY_SUBSCRIPTED) != 0 );
}

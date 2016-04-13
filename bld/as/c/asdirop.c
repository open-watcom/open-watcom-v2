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


#include "as.h"

static dir_operand *dirOpAlloc( void ) {
//**************************************

    return( MemAlloc( sizeof( dir_operand ) ) );
}

static void dirOpFree( dir_operand *dirop ) {
//*******************************************

    MemFree( dirop );
}

static bool dirOpGetNumber( dir_operand *dirop, expr_tree *expr ) {
//*****************************************************************
// Fills out the content and type fields of the operand.
// Can be used to fill out the operand or just to evaluate the expression.

    bool    status = true;

    assert( expr != NULL );
    assert( dirop != NULL );
    expr = ETBurn( expr );
    switch( expr->type ) {
    case ET_CONSTANT:
        NUMBER_INTEGER( dirop ) = CONST_VALUE( expr );
        dirop->type = DIROP_INTEGER;
        break;
    case ET_FP_CONSTANT:
        NUMBER_FLOAT( dirop ) = FP_CONST_VALUE( expr );
        dirop->type = DIROP_FLOATING;
        break;
    default:
        Error( IMPROPERLY_FORMED_DIROP );
        status = false;
    }
    ETFree( expr );
    return( status );
}

extern dir_operand *DirOpLine( const char *string ) {
//***************************************************

    dir_operand *dirop;

    dirop = dirOpAlloc();
    dirop->next = NULL;
    dirop->type = DIROP_LINE;
    STRING_CONTENT( dirop ) = AsStrdup( string );
    return( dirop );
}

extern dir_operand *DirOpString( const char *string ) {
//*****************************************************

    dir_operand *dirop;

    dirop = dirOpAlloc();
    dirop->next = NULL;
    dirop->type = DIROP_STRING;
    STRING_CONTENT( dirop ) = AsStrdup( string );
    return( dirop );
}

extern dir_operand *DirOpNumber( expr_tree *expr ) {
//**************************************************

    dir_operand *dirop;

    assert( expr != NULL );
    dirop = dirOpAlloc();
    dirop->next = NULL;
    if( !dirOpGetNumber( dirop, expr ) ) {
        dirop->type = DIROP_ERROR;
    }
    return( dirop );
}

static dir_operand *doDirOpSymbol( asm_reloc_type rtype, void *target, expr_tree *expr, int sign, bool is_named ) {
//*****************************************************************************************************************
// Does both named and unnamed symbols

    dir_operand *dirop, *tmp;

    dirop = dirOpAlloc();
    dirop->next = NULL;
    if( is_named ) {
        dirop->type = DIROP_SYMBOL;
        SYMBOL_HANDLE( dirop ) = target;
    } else {
        dirop->type = DIROP_NUMLABEL_REF;
        SYMBOL_LABEL_REF( dirop ) = *(int_32 *)target;
    }
    SYMBOL_RELOC_TYPE( dirop ) = rtype;
    if( expr ) {
        tmp = dirOpAlloc();
        if( !dirOpGetNumber( tmp, expr ) || tmp->type == DIROP_FLOATING ) {
            dirop->type = DIROP_ERROR;
        } else {
            SYMBOL_OFFSET( dirop ) = sign * NUMBER_INTEGER( tmp );
        }
        dirOpFree( tmp );
    } else {
        SYMBOL_OFFSET( dirop ) = 0;
    }
    return( dirop );
}

extern dir_operand *DirOpIdentifier( asm_reloc_type rtype, sym_handle symbol, expr_tree *expr, int sign ) {
//*********************************************************************************************************

    return( doDirOpSymbol( rtype, symbol, expr, sign, true ) );
}

extern dir_operand *DirOpNumLabelRef( asm_reloc_type rtype, int_32 label_ref, expr_tree *expr, int sign ) {
//*********************************************************************************************************

    return( doDirOpSymbol( rtype, &label_ref, expr, sign, false ) );
}

extern dir_operand *DirOpRepeat( expr_tree *number, expr_tree *repeat ) {
//***********************************************************************

    dir_operand *dirop, *tmp;

    assert( number != NULL && repeat != NULL );
    dirop = dirOpAlloc();
    dirop->next = NULL;
    tmp = dirOpAlloc();     // use it to transfer type and content data
    if( dirOpGetNumber( tmp, number ) ) {
        REPEAT_NUMBER( dirop ) = tmp->content.number;
        switch( tmp->type ) {
        case DIROP_INTEGER:
            dirop->type = DIROP_REP_INT;
            break;
        case DIROP_FLOATING:
            dirop->type = DIROP_REP_FLT;
            break;
        default:
            assert( false );
        }
        if( dirOpGetNumber( tmp, repeat ) ) {
            if( tmp->type != DIROP_INTEGER ) {
                // Error: invalid repeat constant
                dirop->type = DIROP_ERROR;
            } else if( NUMBER_INTEGER( tmp ) < 0 ) {
                dirop->type = DIROP_ERROR;
            } else {
                REPEAT_COUNT( dirop ) = NUMBER_INTEGER( tmp );
            }
        } else {
            dirop->type = DIROP_ERROR;
        }
    } else {
        dirop->type = DIROP_ERROR;
    }
    dirOpFree( tmp );
    return( dirop );
}

extern void DirOpDestroy( dir_operand *dirop ) {
//**********************************************

    if( !dirop ) return;
    if( dirop->type == DIROP_LINE || dirop->type == DIROP_STRING ) {
        MemFree( STRING_CONTENT( dirop ) );
    }
    dirOpFree( dirop );
}

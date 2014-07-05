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
* Description:  Miscellaneous C front end utilities.
*
****************************************************************************/


#include "cvars.h"


void MustRecog( TOKEN this_token )
{
    if( ExpectingToken( this_token ) ) {
        NextToken();
    } else {
/*      if( CurToken != T_LEFT_BRACE && CurToken != T_RIGHT_BRACE */
        if( CurToken != T_EOF ) {
            if( CurToken == T_SEMI_COLON ) {
                switch( this_token ) {
                case T_RIGHT_BRACKET:
                case T_RIGHT_PAREN:
                    break;
                default:
                    NextToken();
                    break;
                }
            } else if( this_token != T_SEMI_COLON ) {
                NextToken();
            }
        }
    }
}

static char *NameOfCurToken( void )
{
    char        *token;

    switch( CurToken ) {
    case T_BAD_CHAR:
        Buffer[1] = '\0';
    case T_ID:
    case T_STRING:
    case T_CONSTANT:
    case T_BAD_TOKEN:
        token = Buffer;
        break;
    default:
        token = Tokens[CurToken];
    }
    return( token );
}

void Expecting( const char *a_token )
{
    CErr3p( ERR_EXPECTING_BUT_FOUND, a_token, NameOfCurToken() );
}

bool ExpectingToken( TOKEN token )
{
    if( CurToken == token ) {
        return( TRUE );
    }
    CErr3p( ERR_EXPECTING_BUT_FOUND, Tokens[token], NameOfCurToken() );
    return( FALSE );
}

void ExpectingAfter( TOKEN token, TOKEN after_token )
{
    CErr4p( ERR_EXPECTING_AFTER_BUT_FOUND, Tokens[token], Tokens[after_token], NameOfCurToken() );
}


void ExpectEndOfLine( void )
{
    CErr2p( ERR_EXPECTING_END_OF_LINE_BUT_FOUND, NameOfCurToken() );
}

void ExpectIdentifier( void )
{
    CErr2p( ERR_EXPECTING_IDENTIFIER_BUT_FOUND, NameOfCurToken() );
}

bool ExpectingConstant( void )
{
    if( CurToken == T_CONSTANT ) {
        return( TRUE );
    }
    CErr2p( ERR_EXPECTING_CONSTANT_BUT_FOUND, NameOfCurToken() );
    return( FALSE );
}

void ExpectString( void )
{
    CErr2p( ERR_EXPECTING_STRING_BUT_FOUND, NameOfCurToken() );
}

void ExpectStructUnionTag( void )
{
    CErr2p( ERR_EXPECTING_STRUCT_UNION_TAG_BUT_FOUND, NameOfCurToken() );
}


SYM_NAMEPTR SymName( SYMPTR sym, SYM_HANDLE sym_handle )
{
    SYM_HASHPTR hsym;

    if( sym_handle == CharSymHandle )
        return( "char" );
    if( sym->name != NULL )
        return( sym->name );
    hsym = HashTab[sym->info.hash];
    while( hsym->handle != sym_handle )
        hsym = hsym->next_sym;
    return( hsym->name );
}


void CErrSymName( int err, SYMPTR sym, SYM_HANDLE sym_handle )
{
    CErr2p( err, SymName( sym, sym_handle ) );
}

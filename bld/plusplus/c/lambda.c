/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description: Create c++11 Lambda closures.
*
****************************************************************************/

#include "plusplus.h"
#include "lambda.h"
#include "class.h"
#include "gstack.h"
#include "decl.h"
#include "ptree.h"
#include "name.h"
#include "decl.h"
#include "symtype.h"

void LambdaStartClosure(PARSE_STACK *state)
{
	/*
	1 The type of a lambda-expression (which is also the type of the closure object) is
	a unique, unnamed non-union class type, called the closure type,
	 */

	ClassInitState( TF1_NULL, CLINIT_NULL, NULL );

	ClassName( NULL, CLASS_DEFINITION );

	ClassStart();

}


void LambdaAttachBodyToCallOp(DECL_INFO *lambda_function)
{

    {
        if( lambda_function->body != NULL ) {
            if( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_DECL ) ) {
                assert(false);
            } else if( ( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_INST )
                    && ! lambda_function->friend_fn ) ) {
                //TemplateMemberAttachDefn( $1, false );
                assert(false);
            } else if( ( ( ScopeType( GetCurrScope(), SCOPE_CLASS )
                        && ScopeType( GetCurrScope()->enclosing,
                                      SCOPE_TEMPLATE_INST ) )
                    && ! lambda_function->friend_fn ) ) {
                if( GetCurrScope()->owner.type->flag & TF1_SPECIFIC ) {
                    ClassStoreInlineFunc( lambda_function );
                } else {
                    TemplateMemberAttachDefn( lambda_function, true );
                }
            } else {
                ClassStoreInlineFunc( lambda_function );
            }
        } else {
            FreeDeclInfo( lambda_function );
        }
    }
}

/**
 * Creates a declaration for the call operator of the lambda closure object.
 *
 * \param parameter_declaration_clause The parameter-declaration-clause of the lambda.
 * \param trailing_return_type The (trailing) return type of the lambda.
 **/
DECL_INFO *LambdaMakeClosureCallOpDeclaration(DECL_INFO *parameter_declaration_clause, DECL_SPEC *trailing_return_type, TOKEN_LOCN *locn)
{
	// The member function must be public.
	ClassPermission( SF_NULL );

	// First, make a declarator for 'operator ()'.
	DECL_INFO *call_operator = MakeDeclarator( trailing_return_type, PTreeSetLocn( MakeOperatorId( CO_CALL ), locn ) );
	DECL_INFO *call_op_declarator = AddDeclarator( call_operator, MakeFnType( &parameter_declaration_clause, STY_CONST, ThrowsAnything() ) );
    AddExplicitParms( call_op_declarator, parameter_declaration_clause );
    call_op_declarator = FinishDeclarator( trailing_return_type, call_op_declarator );

    // Then declare the 'operator ()' function with the given return type.
    DECL_INFO *call_op_declaration = DeclFunction( trailing_return_type, call_op_declarator );

    return call_op_declaration;
}

/**
 * Complete the closure object and return an instance of the underlying closure type.
 **/
PTREE LambdaFinishClosure()
{
	DECL_SPEC * complete_closure_type = ClassEnd();
	complete_closure_type->stg_class = STG_AUTO; //FIXME

	NAME name;
	TYPE lambda_type = StructType( complete_closure_type->partial );

	SYMBOL closure = SymMakeDummy( lambda_type, &name );
	SymbolLocnDefine( NULL, closure );
	InsertSymbol( GetCurrScope(), closure, name );

	PTypeRelease( complete_closure_type );

	return PTreeIdSym( closure );
}



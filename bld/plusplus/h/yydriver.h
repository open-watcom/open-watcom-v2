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


#ifndef _YYDRIVER_H

typedef struct parse_stack PARSE_STACK;

extern void ParseFlush( void );
extern REWRITE *ParseGetRecordingInProgress( TOKEN_LOCN ** );
extern PTREE ParseExpr( int end_token );
extern void ParseDecls( void );
extern PTREE ParseExprDecl( void );
extern PTREE ParseMemInit( void );
extern PTREE ParseDefArg( void );
extern PTREE ParseTemplateIntDefArg( void );
extern PTREE ParseTemplateTypeDefArg( void );
extern DECL_INFO *ParseException( void );
extern DECL_SPEC *ParseClassInstantiation( REWRITE * );
extern DECL_INFO *ReparseFunctionDeclaration( REWRITE * );
extern void ParseClassMemberInstantiation( REWRITE * );
extern void ParseFunctionInstantiation( REWRITE * );
extern void ParsePushQualification( void * );
extern void *ParsePopQualification( void );
extern void *ParseCurrQualification( void );
extern SYMBOL ParseCurrFunction( void );
static void recordTemplateCtorInitializer( PARSE_STACK * );

#define _YYDRIVER_H
#endif

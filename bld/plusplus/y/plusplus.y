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
* Description:  PLUSPLUS.Y : description of C++ syntax

Modified        By              Reason
--------        --              ------
92/10/02        Greg Bentz      Initial version, shipped as "alpha"
                Jim Randall
                Anthony Scian
                Jim Welch

92/10/02        A.F.Scian       changed cast-expression to use MakeNormalCast so
                                we can distinguish programmer specified casts
92/10/15        A.F.Scian       added new keywords __stdcall, _Syscall, _Packed,
                                and _Seg16
92/10/16        A.F.Scian       - added _Packed semantic actions for OS/2
                                - added _Seg16 semantic actions for OS/2
                                - made far16 equivalent to far for 16-bit compiler
                                - made syscall and stdcall equivalent to cdecl
                                  for 16-bit compiler
92/10/22        A.F.Scian       instead of parsing one global declaration at a
                                time and accepting the tokens, parse all of the
                                global declarations before accepting
92/10/23        A.F.Scian       made interrupt a flag instead of a calling convention
92/10/28        A.F.Scian       used direct access to standard calling conventions
92/10/29        A.F.Scian       - treat exception decls just like arguments
                                - corrected throw to have an optional expr
92/12/01        A.F.Scian       added better error messages for missing ';'
92/12/04        A.F.Scian       added optional size expression for delete []
93/01/07        A.F.Scian       changed precedence of :> operator to just less
                                than a cast
93/01/27        A.F.Scian       added support for '::'<template-name>
93/01/31        A.F.Scian       removed kludge to define class template symbols
93/02/08        A.F.Scian       fixed problem with "struct T<x>" followed by id
93/02/09        A.F.Scian       added calls to zapTemplateClassDeclSpec
                                (see YYDRIVER.C for explanation)
93/03/11        J.W.Welch       New messaging
93/03/16        A.F.Scian       streamlined member-declarator to directly call
                                DeclNoInit instead of going through DataInitNoInit
93/03/29        A.F.Scian       added semantics for "new (int [x+1])" 5.3.3
                                "The first array dimension can be a general
                                 integral expression even when <type-id> is used"
93/04/20        A.F.Scian       added rules for y_template_scoped_typename to
                                handle nested types inside of templates
93/04/21        A.F.Scian       fixed actions for function-declaration to not
                                pop the GS_DECL_SPEC stack until the function
                                body is processed (this allows any linkage on
                                the function symbol to be retained for the
                                entire processing of the function body)
93/07/13        A.F.Scian       - added more than one pointer declarator to
                                  conversion function ids
                                - fixed concatenation of strings so that the
                                  type of the result is correct
93/07/29        A.F.Scian       it was impossible to tell the difference between
                                no parms and ( void ) (both had parms == NULL)
                                so code now calls AddExplicitParms which handles
                                the problem by setting a bit in the DECL_INFO
93/08/11        A.F.Scian       allow multiple ;'s in member decl lists
                                e.g., struct S { int a;;;; void foo(){};;; };
93/12/01        A.F.Scian       added %ambig directives supported by new YACC
93/12/15        A.F.Scian       added operator new []/operator delete [] support
94/03/23        A.F.Scian       set up so that class S<int>; doesn't define the
                                class
94/04/18        A.F.Scian       added support for "class __export __stdcall X"
94/04/19        A.F.Scian       push some calls to FreeDeclInfo into functions
                                that are called before
94/06/08        A.F.Scian       improved error message location for missing ';'
                                error messages
94/10/14        A.F.Scian       added __declspec( <id> )
94/10/18        A.F.Scian       added kludge to support:
                                typedef struct S { } S, const *CPS;
                                                        ^^^^^- accept this
94/10/25        A.F.Scian       upgrade LHS of assignment-expr so that it
                                accepts casts and pm-exprs
94/11/02        A.F.Scian       added support for "class __declspec(dllimport) X"
94/12/07        A.F.Scian       added Y_GLOBAL_ID as a valid declarator-id so
                                struct S { friend int ::foo( int ); }; works
95/02/23        A.F.Scian       added __builtin_isfloat( <type-id> ) construct
95/04/28        A.F.Scian       added __unaligned modifier
95/05/16        A.F.Scian       added class template directives
95/06/28        A.F.Scian       added more general offsetof field expressions
96/01/10        A.F.Scian       adjusted syntax of __unaligned to match 'const'
                                rather than 'far'
96/02/12        A.F.Scian       added support for __declspec( pragma-modifier )
96/02/26        A.F.Scian       added some namespace syntax
96/03/19        A.F.Scian       adjusted ++@/--@ grammar as per ISO C++ mtg in
                                Santa Cruz, CA resolutions
96/05/07        A.F.Scian       added "using namespace X;"
96/06/07        A.F.Scian       added support for multiple ids in __declspec()
96/07/02        A.F.Scian       added sizeof <type-name>
96/07/25        A.F.Scian       accept unsigned <type-name> : 1; as a bitfield
96/10/28        A.F.Scian       fixed 96/07/25 so that <type-name> : 1; still works
*/
%token Y_IMPOSSIBLE

/*** declaration keywords ***/
%token Y_ASM
%token Y_AUTO
%token Y_BOOL
%token Y_CHAR
%token Y_CLASS
%token Y_CONST
%token Y_CONST_CAST
%token Y_DELETE
%token Y_DOUBLE
%token Y_DYNAMIC_CAST
%token Y_ELSE
%token Y_ENUM
%token Y_EXPLICIT
%token Y_EXPORT
%token Y_EXTERN
%token Y_FALSE
%token Y_FLOAT
%token Y_FOR
%token Y_FRIEND
%token Y_INLINE
%token Y_INT
%token Y_LONG
%token Y_MUTABLE
%token Y_NAMESPACE
%token Y_NEW
%token Y_OPERATOR
%token Y_PRIVATE
%token Y_PROTECTED
%token Y_PUBLIC
%token Y_REGISTER
%token Y_REINTERPRET_CAST
%token Y_SHORT
%token Y_SIGNED
%token Y_SIZEOF
%token Y_STATIC
%token Y_STATIC_ASSERT
%token Y_STATIC_CAST
%token Y_STRUCT
%token Y_TEMPLATE
%token Y_THIS
%token Y_THROW
%token Y_TRUE
%token Y_TYPEDEF
%token Y_TYPEID
%token Y_TYPENAME
%token Y_UNION
%token Y_UNSIGNED
%token Y_USING
%token Y_VIRTUAL
%token Y_VOID
%token Y_VOLATILE
%token Y_WCHAR_T

/*** punctuation and operators ***/
%token Y_COMMA
%token Y_QUESTION
%token Y_COLON
%token Y_SEMI_COLON
%token Y_LEFT_PAREN
%token Y_RIGHT_PAREN
%token Y_LEFT_BRACKET
%token Y_RIGHT_BRACKET
%token Y_LEFT_BRACE
%token Y_RIGHT_BRACE
%token Y_TILDE
%token Y_EQUAL
%token Y_EQ
%token Y_EXCLAMATION
%token Y_NE
%token Y_OR
%token Y_OR_EQUAL
%token Y_OR_OR
%token Y_AND
%token Y_AND_EQUAL
%token Y_AND_AND
%token Y_XOR
%token Y_XOR_EQUAL
%token Y_GT
%token Y_GE
%token Y_LT
%token Y_LE
%token Y_LSHIFT
%token Y_LSHIFT_EQUAL
%token Y_RSHIFT
%token Y_RSHIFT_EQUAL
%token Y_PLUS
%token Y_PLUS_EQUAL
%token Y_PLUS_PLUS
%token Y_MINUS
%token Y_MINUS_EQUAL
%token Y_MINUS_MINUS
%token Y_TIMES
%token Y_TIMES_EQUAL
%token Y_DIV
%token Y_DIV_EQUAL
%token Y_PERCENT
%token Y_PERCENT_EQUAL
%token Y_DOT
%token Y_DOT_DOT_DOT
%token Y_DOT_STAR
%token Y_ARROW
%token Y_ARROW_STAR
%token Y_SEG_OP

/*** special lexical tokens ***/
%token <tree> Y_ID
%token <tree> Y_UNKNOWN_ID
%token <tree> Y_TEMPLATE_ID
%token <tree> Y_TYPE_NAME
%token <tree> Y_TEMPLATE_NAME
%token <tree> Y_NAMESPACE_NAME
%token <tree> Y_CONSTANT
%token <tree> Y_TRUE
%token <tree> Y_FALSE
%token <tree> Y_STRING

%token <tree> Y_GLOBAL_ID               /* ::<id> */
%token <tree> Y_GLOBAL_UNKNOWN_ID       /* ::<id> */
%token <tree> Y_GLOBAL_TEMPLATE_ID      /* ::<id> */
%token <tree> Y_GLOBAL_TYPE_NAME        /* ::<type-name> */
%token <tree> Y_GLOBAL_TEMPLATE_NAME    /* ::<template-name> */
%token <tree> Y_GLOBAL_NAMESPACE_NAME   /* ::<namespace-name> */
%token <tree> Y_GLOBAL_OPERATOR         /* ::operator */
%token <tree> Y_GLOBAL_TILDE            /* ::~ */
%token <tree> Y_GLOBAL_NEW              /* ::new */
%token <tree> Y_GLOBAL_DELETE           /* ::delete */

/* Y_SCOPED_* tokens must stay in line with Y_TEMPLATE_SCOPED_* tokens */
%token <tree> Y_SCOPED_ID               /* C::<id> */
%token <tree> Y_SCOPED_UNKNOWN_ID       /* C::<id> */
%token <tree> Y_SCOPED_TEMPLATE_ID      /* C::<id> */
%token <tree> Y_SCOPED_TYPE_NAME        /* C::<type-name> */
%token <tree> Y_SCOPED_TEMPLATE_NAME    /* C::<template-name> */
%token <tree> Y_SCOPED_NAMESPACE_NAME   /* C::<namespace-name> */
%token <tree> Y_SCOPED_OPERATOR         /* C::operator */
%token <tree> Y_SCOPED_TILDE            /* C::~ */
%token <tree> Y_SCOPED_TIMES            /* C::* */

%token <tree> Y_TEMPLATE_SCOPED_ID      /* T<>::<id> */
%token <tree> Y_TEMPLATE_SCOPED_UNKNOWN_ID/* T<>::<id> */
%token <tree> Y_TEMPLATE_SCOPED_TEMPLATE_ID/* T<>::<id> */
%token <tree> Y_TEMPLATE_SCOPED_TYPE_NAME/* T<>::<type-name> */
%token <tree> Y_TEMPLATE_SCOPED_TEMPLATE_NAME/* T<>::<template-name> */
%token <tree> Y_TEMPLATE_SCOPED_NAMESPACE_NAME/* T<>::<namespace-name> */
%token <tree> Y_TEMPLATE_SCOPED_OPERATOR/* T<>::operator */
%token <tree> Y_TEMPLATE_SCOPED_TILDE   /* T<>::~ */
%token <tree> Y_TEMPLATE_SCOPED_TIMES   /* T<>::* */

/*** leader token for "special" parsing ***/
%token Y_EXPRESSION_SPECIAL
%token Y_EXPR_DECL_SPECIAL
%token Y_FUNCTION_DECL_SPECIAL
%token Y_EXCEPTION_SPECIAL
%token Y_MEM_INIT_SPECIAL
%token Y_DEFARG_SPECIAL
%token Y_TEMPLATE_INT_DEFARG_SPECIAL
%token Y_TEMPLATE_TYPE_DEFARG_SPECIAL
%token Y_CLASS_INST_SPECIAL

/*** terminator tokens for "special" parsing ***/
%token Y_DEFARG_END
%token Y_GT_SPECIAL

/*** tokens that are never generated by yylex() ***/
%token Y_REDUCE_SPECIAL
%token Y_SHIFT_SPECIAL
%token Y_DEFARG_GONE_SPECIAL
%token Y_TEMPLATE_DEFARG_GONE_SPECIAL
%token Y_PURE_FUNCTION_SPECIAL

/*** special function names ***/
%token Y_DECLTYPE
%token Y___OFFSETOF
%token Y___TYPEOF
%token Y___BUILTIN_ISFLOAT

/*** PC-specific keywords ***/
%token Y___ASM
%token Y___BASED
%token Y__CDECL
%token Y___CDECL
%token Y___DECLSPEC
%token Y__EXPORT
%token Y___EXPORT
%token Y___FAR
%token Y__FAR16
%token Y___FAR16
%token Y__FASTCALL
%token Y___FASTCALL
%token Y___FORTRAN
%token Y___HUGE
%token Y___INLINE
%token Y___INT8
%token Y___INT16
%token Y___INT32
%token Y___INT64
%token Y___INTERRUPT
%token Y___LOADDS
%token Y___NEAR
%token Y__OPTLINK
%token Y__PACKED
%token Y__PASCAL
%token Y___PASCAL
%token Y___PRAGMA
%token Y___SAVEREGS
%token Y__SEG16
%token Y___SEGMENT
%token Y___SEGNAME
%token Y___SELF
%token Y___STDCALL
%token Y__SYSCALL
%token Y___SYSCALL
%token Y__SYSTEM
%token Y___WATCALL
%token Y___UNALIGNED

%type <flags> modifier
%type <flags> access-specifier
%type <flags> cv-qualifier-seq-opt
%type <flags> base-qualifiers-opt
%type <flags> segment-cast-opt
%type <flags> packed-class-opt

%type <token> class-key
%type <token> template-typename-key
%type <token> operator

%type <type> class-mod-opt
%type <type> class-mod-seq
%type <type> class-mod

%type <type> ptr-operator
%type <type> pragma-modifier
%type <type> based-expression
%type <type> ptr-mod

%type <base> base-specifier base-specifier-list

%type <rewrite> ctor-initializer
%type <rewrite> defarg-check

%type <dspec> type-specifier-seq
%type <dspec> type-specifier
%type <dspec> typeof-specifier
%type <dspec> arg-decl-specifier-seq
%type <dspec> decl-specifier-seq
%type <dspec> non-type-decl-specifier-seq
%type <dspec> non-type-decl-specifier
%type <dspec> no-declarator-declaration
%type <dspec> basic-type-specifier
%type <dspec> simple-type-specifier
%type <dspec> maybe-type-decl-specifier-seq
%type <dspec> storage-class-specifier
%type <dspec> ms-specific-declspec
%type <dspec> ms-declspec-seq
%type <dspec> function-specifier
%type <dspec> cv-qualifier
%type <dspec> class-specifier
%type <dspec> enum-specifier
%type <dspec> elaborated-type-specifier
%type <dspec> class-substance
%type <dspec> class-body
%type <dspec> qualified-type-specifier
%type <dspec> qualified-class-specifier
%type <dspec> qualified-class-type
%type <dspec> typename-specifier

%type <dinfo> function-declaration
%type <dinfo> declarator
%type <dinfo> comma-declarator
%type <dinfo> init-declarator
%type <dinfo> comma-init-declarator
%type <dinfo> ctor-declarator
%type <dinfo> declaring-declarator
%type <dinfo> comma-declaring-declarator
%type <dinfo> direct-declarator
%type <dinfo> ptr-mod-init-declarator
%type <dinfo> ptr-mod-declarator
%type <dinfo> conversion-declarator
%type <dinfo> special-new-abstract-ptr-mod-declarator
%type <dinfo> abstract-ptr-mod-declarator
%type <dinfo> special-new-abstract-declarator
%type <dinfo> abstract-declarator
%type <dinfo> special-new-direct-abstract-declarator
%type <dinfo> direct-abstract-declarator
%type <dinfo> parameter-declaration-clause
%type <dinfo> simple-parameter-declaration
%type <dinfo> parameter-declaration
%type <dinfo> template-parameter
%type <dinfo> type-parameter
%type <dinfo> type-parameter-no-defarg
%type <dinfo> parameter-declaration-list
%type <dinfo> actual-exception-declaration
%type <dinfo> exception-declaration
%type <dinfo> member-declaring-declarator
%type <dinfo> member-declarator
%type <dinfo> direct-new-declarator
%type <dinfo> partial-ptr-declarator
%type <dinfo> new-declarator
%type <dinfo> dynamic-type-id
%type <dinfo> new-type-id
%type <dinfo> special-new-type-id
%type <dinfo> simple-arg-no-id

%type <tree> identifier
%type <tree> boolean-literal
%type <tree> new-keyword
%type <tree> delete-keyword
%type <tree> declarator-id
%type <tree> conversion-type-id
%type <tree> operator-function-id
%type <tree> scoped-operator-function-id
%type <tree> template-scoped-operator-function-id
%type <tree> conversion-function-id
%type <tree> scoped-conversion-function-id
%type <tree> template-scoped-conversion-function-id
%type <tree> pseudo-destructor-name
%type <tree> exception-specification-opt
%type <tree> exception-specification
%type <tree> expression
%type <tree> expression-before-semicolon
%type <tree> expression-list
%type <tree> expression-list-opt
%type <tree> assignment-operator
%type <tree> assignment-expression
%type <tree> assignment-expression-opt
%type <tree> conditional-expression
%type <tree> logical-or-expression
%type <tree> logical-and-expression
%type <tree> inclusive-or-expression
%type <tree> exclusive-or-expression
%type <tree> and-expression
%type <tree> equality-expression
%type <tree> relational-expression
%type <tree> shift-expression
%type <tree> additive-expression
%type <tree> multiplicative-expression
%type <tree> pm-expression
%type <tree> cast-expression
%type <tree> unary-operator
%type <tree> unary-expression
%type <tree> offsetof-field
%type <tree> offsetof-index
%type <tree> new-expression
%type <tree> delete-expression
%type <tree> postfix-expression
%type <tree> primary-expression
%type <tree> id-expression
%type <tree> unqualified-id
%type <tree> template-scoped-unqualified-id
%type <tree> qualified-id
%type <tree> nested-name-specifier
%type <tree> scoped-nested-name-specifier
%type <tree> segment-expression
%type <tree> postfix-expression-before-dot
%type <tree> postfix-expression-before-arrow
%type <tree> new-placement
%type <tree> make-id
%type <tree> class-name-id
%type <tree> invalid-class-name-id
%type <tree> enumerator
%type <tree> elaborated-type-name
%type <tree> literal
%type <tree> string-literal pragma-id
%type <tree> type-id-list
%type <tree> type-id
%type <tree> new-initializer
%type <tree> new-initializer-opt
%type <tree> constant-expression
%type <tree> constant-initializer
%type <tree> mem-initializer-list
%type <tree> mem-initializer
%type <tree> template-id
%type <tree> scoped-template-id
%type <tree> template-type
%type <tree> template-type-instantiation
%type <tree> scoped-template-type
%type <tree> scoped-template-type-instantiation
%type <tree> template-scoped-template-type
%type <tree> template-scoped-template-type-instantiation
%type <tree> template-argument
%type <tree> template-argument-list-opt
%type <tree> template-argument-list
%type <tree> qualified-namespace-specifier

%type <tree> expr-decl-stmt
%type <tree> goal-symbol

%nonassoc Y_LEFT_PAREN
%nonassoc Y_FAVOUR_REDUCE_SPECIAL

%start goal-symbol

%%

/* these actions change the token to EOF to force acceptance */
goal-symbol
    : Y_EXPRESSION_SPECIAL expression
    {
        $$ = $2;
        t = YYEOFTOKEN;
    }
    | Y_EXPRESSION_SPECIAL type-specifier-seq declaring-declarator initializer
    {
        CheckDeclarationDSpec( state->gstack->u.dspec, GetCurrScope() );
        GStackPop( &(state->gstack) );
        $$ = $3->id;
        $3->id = NULL;
        FreeDeclInfo( $3 );
        t = YYEOFTOKEN;
    }
    | Y_EXPR_DECL_SPECIAL expr-decl-stmt
    {
        $$ = $2;
        t = YYEOFTOKEN;
    }
    | Y_FUNCTION_DECL_SPECIAL decl-specifier-seq declarator
    {
        GStackPop( &(state->gstack) );      /* decl-spec */
        $$ = (PTREE) $3;
        t = YYEOFTOKEN;
    }
    | Y_FUNCTION_DECL_SPECIAL                    declarator
    {
        $$ = (PTREE) $2;
        t = YYEOFTOKEN;
    }
    | Y_FUNCTION_DECL_SPECIAL decl-specifier-seq ctor-declarator
    {
        GStackPop( &(state->gstack) );      /* decl-spec */
        $$ = (PTREE) $3;
        t = YYEOFTOKEN;
    }
    | might-restart-declarations
    {
        t = YYEOFTOKEN;
    }
    | Y_EXCEPTION_SPECIAL exception-declaration
    {
        /* treat exception decl like an argument */
        $$ = (PTREE) AddArgument( NULL, $2 );
        t = YYEOFTOKEN;
    }
    | Y_MEM_INIT_SPECIAL mem-initializer-list Y_LEFT_BRACE
    {
        $$ = $2;
        t = YYEOFTOKEN;
    }
    | Y_DEFARG_SPECIAL assignment-expression Y_DEFARG_END
    {
        $$ = $2;
        t = YYEOFTOKEN;
    }
    | Y_TEMPLATE_INT_DEFARG_SPECIAL logical-or-expression Y_DEFARG_END
    {
        $$ = $2;
        t = YYEOFTOKEN;
    }
    | Y_TEMPLATE_TYPE_DEFARG_SPECIAL expect-type-id type-id Y_DEFARG_END
    {
        $$ = $3;
        t = YYEOFTOKEN;
    }
    | Y_CLASS_INST_SPECIAL class-specifier
    {
        $$ = (PTREE) $2;
        t = YYEOFTOKEN;
    }
    /* I have included this as a stack reset leaves us open to abuse
     * now we fixed bug 218. All linkage gets reset when we have a
     * syntax error earlier in the file which screws up closing of the
     * parser. We only issue an error if we have not reported any
     * earlier errors as this error comes out badly at the end of a
     * file
     */
    | Y_RIGHT_BRACE
    {
        error_state_t save;
        CErrCheckpoint( &save );
        if( 0 == save ) {
            SetErrLoc( &yylp[1] );
            CErr1( ERR_MISPLACED_RIGHT_BRACE );
            what = P_DIAGNOSED;
        }
    }
    | /* nothing */
    {
        if( CurToken != T_EOF ) {
            CErr1( ERR_SYNTAX );
        }
        what = P_DIAGNOSED;
    }
    ;

/* error reporting hints */
expect-string-literal
    : /* nothing */
    { state->expect = "string-literal"; }
    ;

expect-identifier
    : /* nothing */
    { state->expect = "identifier"; }
    ;

expect-id-expression
    : /* nothing */
    { state->expect = "id-expression"; }
    ;

expect-type-name
    : /* nothing */
    { state->expect = "type-name"; }
    ;

expect-type-id
    : /* nothing */
    { state->expect = "type-id"; }
    ;

expect-qualified-namespace-specifier
    : /* nothing */
    { state->expect = "qualified-namespace-specifier"; }
    ;

lt-special-init
    : /* nothing */
    {
        angle_bracket_stack *angle_state;

        angle_state = VstkPush( &(state->angle_stack) );
        angle_state->paren_depth = 0;
    }
    ;

lt-special
    : lt-special-init Y_LT
    ;

expr-decl-stmt
    : expression-before-semicolon Y_SEMI_COLON
    { $$ = $1; }
    | local-declaration
    { $$ = NULL; }
    ;

expression-before-semicolon
    : expression
    {
        $$ = $1;
        if( t != Y_SEMI_COLON ) {
            SetErrLoc( &yylocation );
            CErr1( ERR_SYNTAX_MISSING_SEMICOLON );
            what = P_DIAGNOSED;
        }
    }
    ;

/* r/r conflict: 
 *   id-expression <- qualified-id (unit production)
 *   access-declaration <- qualified-id
 */
access-declaration
    : qualified-id
    { ClassAccessDeclaration( $1, &yylocation ); }
    | qualified-type-specifier
    { ClassAccessTypeDeclaration( $1, &yylocation ); }
    | Y_SCOPED_UNKNOWN_ID
    { ClassAccessDeclaration( MakeScopedId( $1 ), &yylocation ); }
    | nested-name-specifier Y_TEMPLATE_SCOPED_UNKNOWN_ID
    {
        PTreeFreeSubtrees( $1 );
        ClassAccessDeclaration( MakeScopedId( $2 ), &yylocation );
    }
    ;



/* A.2 Lexical conventions [gram.lex] */

identifier
    : Y_ID
    | Y_UNKNOWN_ID
    | Y_TEMPLATE_ID
    ;

literal
    : Y_CONSTANT /* integer-literal character-literal floating-literal */
    | string-literal
    | boolean-literal
    ;

string-literal
    : Y_STRING
    | string-literal Y_STRING
    { $$ = PTreeStringLiteralConcat( $1, $2 ); }
    ;

boolean-literal
    : Y_TRUE
    | Y_FALSE
    ;


/* A.4 Expressions [gram.expr] */

primary-expression
    : literal
    | Y_THIS
    { $$ = setLocation( PTreeThis(), &yylp[1] ); }
    | Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = $2; }
    | id-expression
    ;

id-expression
    : unqualified-id
    | qualified-id
    ;

unqualified-id
    : identifier
    | operator-function-id
    | conversion-function-id
    | Y_TILDE expect-type-name Y_TYPE_NAME /* ~ class-name */
    { $$ = setLocation( MakeDestructorId( $3 ), &yylp[1] ); }
    | Y_TILDE expect-type-name Y_UNKNOWN_ID /* ~ id (non-stanard, diagnostics only) */
    { $$ = setLocation( MakeDestructorId( $3 ), &yylp[1] ); }
    | Y_TILDE expect-type-name template-type /* ~ class-name */
    { $$ = setLocation( MakeDestructorIdFromType( PTypeClassInstantiation( state->class_colon, $3 ) ), &yylp[1] ); }
    | template-id
    ;

qualified-id
    : nested-name-specifier template-scoped-unqualified-id
    { PTreeFreeSubtrees( $1 ); $$ = $2; }
    | Y_GLOBAL_ID /* :: identifier */
    { $$ = MakeGlobalId( $1 ); }
    | Y_GLOBAL_TEMPLATE_ID /* :: identifier */
    { $$ = MakeGlobalId( $1 ); }
    | Y_SCOPED_ID
    { $$ = MakeScopedId( $1 ); }
    | Y_SCOPED_TEMPLATE_ID
    { $$ = MakeScopedId( $1 ); }
    | scoped-operator-function-id
    | scoped-conversion-function-id
    | scoped-template-id
    | pseudo-destructor-name
    ;

/* differs from standard */
nested-name-specifier
    : template-type
    | scoped-template-type
    | template-type scoped-nested-name-specifier
    { PTreeFreeSubtrees( $1 ); $$ = $2; }
    | scoped-template-type scoped-nested-name-specifier
    { PTreeFreeSubtrees( $1 ); $$ = $2; }
    ;

/* non-standard */
scoped-nested-name-specifier
    : template-scoped-template-type
    | template-scoped-template-type scoped-nested-name-specifier
    {
        PTreeFreeSubtrees( $1 );
        $$ = $2;
    }
    ;

/* non-standard */
template-scoped-unqualified-id
    : Y_TEMPLATE_SCOPED_ID
    { $$ = MakeScopedId( $1 ); }
    | Y_TEMPLATE_SCOPED_TEMPLATE_ID
    { $$ = MakeScopedId( $1 ); }
    | Y_TEMPLATE_SCOPED_TEMPLATE_ID lt-special template-argument-list-opt Y_GT_SPECIAL
    | Y_TEMPLATE_SCOPED_TEMPLATE_NAME lt-special template-argument-list-opt Y_GT_SPECIAL
    | template-scoped-operator-function-id
    | template-scoped-conversion-function-id
    ;

postfix-expression
    : primary-expression
    | postfix-expression Y_LEFT_BRACKET expression Y_RIGHT_BRACKET
    { $$ = setLocation( PTreeBinary( CO_INDEX, $1, $3 ), &yylp[2] ); }
    | postfix-expression Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_CALL, $1, $3 ), &yylp[2] ); }
    | simple-type-specifier Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN %ambig 0 Y_LEFT_PAREN
    { $$ = setLocation( MakeFunctionLikeCast( $1, $3 ), &yylp[2] ); }
    | typename-specifier Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
    {
        $$ = setLocation( MakeFunctionLikeCast( $1, $3 ), &yylp[2] );
    }
    | postfix-expression-before-dot Y_DOT expect-id-expression id-expression
    { $$ = PTreeReplaceRight( setLocation( $1, &yylp[3] ), $4 ); }
    | postfix-expression-before-arrow Y_ARROW expect-id-expression id-expression
    { $$ = PTreeReplaceRight( setLocation( $1, &yylp[3] ), $4 ); }
/* id-expression includes pseudo-destructor-name
    | postfix-expression-before-dot Y_DOT pseudo-destructor-name
    | postfix-expression-before-arrow Y_ARROW pseudo-destructor-name
*/
    | postfix-expression Y_PLUS_PLUS
    { $$ = setLocation( PTreeUnary( CO_POST_PLUS_PLUS, $1 ), &yylp[2] ); }
    | postfix-expression Y_MINUS_MINUS
    { $$ = setLocation( PTreeUnary( CO_POST_MINUS_MINUS, $1 ), &yylp[2] ); }
    | Y_DYNAMIC_CAST lt-special expect-type-id type-id Y_GT_SPECIAL Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_DYNAMIC_CAST, $4, $7 ), &yylp[1] ); }
    | Y_STATIC_CAST lt-special expect-type-id type-id Y_GT_SPECIAL Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_STATIC_CAST, $4, $7 ), &yylp[1] ); }
    | Y_REINTERPRET_CAST lt-special expect-type-id type-id Y_GT_SPECIAL Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_REINTERPRET_CAST, $4, $7 ), &yylp[1] ); }
    | Y_CONST_CAST lt-special expect-type-id type-id Y_GT_SPECIAL Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_CONST_CAST, $4, $7 ), &yylp[1] ); }
    | Y_TYPEID Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeUnary( CO_TYPEID_EXPR, $3 ), &yylp[1] ); }
    | Y_TYPEID Y_LEFT_PAREN type-id Y_RIGHT_PAREN
    { $$ = setLocation( PTreeUnary( CO_TYPEID_TYPE, $3 ), &yylp[1] ); }
    /* extension */
    | Y___SEGNAME Y_LEFT_PAREN expect-string-literal string-literal Y_RIGHT_PAREN
    { $$ = setLocation( PTreeUnary( CO_SEGNAME, $4 ), &yylp[1] ); }
    ;

/* non-standard */
postfix-expression-before-dot
    : postfix-expression
    {
        PTREE AnalyseNode( PTREE );
        bool AnalyseLvalue( PTREE * );

        $1 = PTreeTraversePostfix( $1, &AnalyseNode );
        if( ! ( $1->flags & PTF_LV_CHECKED ) ) {
            AnalyseLvalue( &$1 );
        }
        $1 = PTreeTraversePostfix( $1, &setAnalysedFlag );
        if( $1->type ) {
            TYPE cls;

            cls = TypedefModifierRemoveOnly( $1->type );
            if( ( cls != NULL )
             && ( cls->id == TYP_POINTER )
             && ( cls->flag & TF1_REFERENCE ) ) {
                cls = cls->of;
            }

            if( cls != NULL ) {
                cls = BindTemplateClass( cls, &$1->locn, FALSE );
                $1->type = BoundTemplateClass( $1->type );
                cls = TypedefModifierRemoveOnly( cls );

                if( cls->id == TYP_CLASS ) {
                    setTypeMember( state, cls->u.c.scope );
                }
            }
        }
        $$ = PTreeBinary( CO_DOT, $1, NULL );
    }
    ;

/* non-standard */
postfix-expression-before-arrow
    : postfix-expression
    {
        PTREE AnalyseNode( PTREE );
        bool AnalyseLvalue( PTREE * );
        PTREE OverloadOperator( PTREE );

        $1 = PTreeTraversePostfix( $1, &AnalyseNode );
        if( ! ( $1->flags & PTF_LV_CHECKED ) ) {
            AnalyseLvalue( &$1 );
        }
        $$ = PTreeBinary( CO_ARROW, $1, NULL );
        $$ = OverloadOperator( $$ );
        $$->u.subtree[0] = PTreeTraversePostfix( $$->u.subtree[0],
                                                 &setAnalysedFlag );
        if( $$->u.subtree[0] ) {
            TYPE cls;

            cls = TypedefModifierRemoveOnly( $$->u.subtree[0]->type );
            if( ( cls != NULL )
             && ( cls->id == TYP_POINTER )
             && ( cls->flag & TF1_REFERENCE ) ) {
                cls = TypedefModifierRemoveOnly( cls->of );
            }

            if( ( cls != NULL ) && cls->id == TYP_POINTER ) {
                cls = BindTemplateClass( cls->of, &$1->locn, FALSE );
                $1->type = BoundTemplateClass( $1->type );
                cls = TypedefModifierRemoveOnly( cls );

                if( cls->id == TYP_CLASS ) {
                    setTypeMember( state, cls->u.c.scope );
                }
            }
        }
    }
    ;

expression-list-opt
    : /* nothing */
    { $$ = NULL; }
    | expression-list
    ;

expression-list
    : assignment-expression
    { $$ = PTreeBinary( CO_LIST, NULL, $1 ); }
    | expression-list Y_COMMA assignment-expression
    { $$ = setLocation( PTreeBinary( CO_LIST,   $1, $3 ), &yylp[2] ); }
    ;

pseudo-destructor-name
    : Y_SCOPED_TILDE expect-type-name identifier
    { $$ = setLocation( MakeScopedDestructorId( $1, $3 ), &yylp[1] ); }
    | Y_SCOPED_TILDE expect-type-name Y_TYPE_NAME
    { $$ = setLocation( MakeScopedDestructorId( $1, $3 ), &yylp[1] ); }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TILDE expect-type-name Y_ID
    {
        PTreeFreeSubtrees( $1 );
        $$ = setLocation( MakeScopedDestructorId( $2, $4 ), &yylp[2] );
    }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TILDE expect-type-name Y_UNKNOWN_ID
    {
        PTreeFreeSubtrees( $1 );
        $$ = setLocation( MakeScopedDestructorId( $2, $4 ), &yylp[2] );
    }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TILDE expect-type-name Y_TEMPLATE_NAME
    {
        PTreeFreeSubtrees( $1 );
        $$ = setLocation( MakeScopedDestructorId( $2, $4 ), &yylp[2] );
    }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TILDE expect-type-name Y_TYPE_NAME
    { $$ = setLocation( MakeScopedDestructorId( $2, $4 ), &yylp[1] ); }
    ;

unary-expression
    : postfix-expression
    | Y_PLUS_PLUS cast-expression
    { $$ = setLocation( PTreeUnary( CO_PRE_PLUS_PLUS, $2 ), &yylp[1] ); }
    | Y_MINUS_MINUS cast-expression
    { $$ = setLocation( PTreeUnary( CO_PRE_MINUS_MINUS, $2 ), &yylp[1] ); }
    | unary-operator cast-expression
    { $$ = PTreeReplaceLeft( $1, $2 ); }
    | Y_SIZEOF unary-expression
    { $$ = setLocation( PTreeUnary( CO_SIZEOF_EXPR, $2 ), &yylp[1] ); }
    | Y_SIZEOF Y_LEFT_PAREN type-id Y_RIGHT_PAREN
    { $$ = setLocation( PTreeUnary( CO_SIZEOF_TYPE, $3 ), &yylp[1] ); }
    | new-expression
    | delete-expression
    /* extension */
    | Y_SIZEOF Y_TYPE_NAME
    { $$ = setLocation( PTreeUnary( CO_SIZEOF_TYPE, PTreeMSSizeofKludge( $2 ) ), &yylp[1] ); }
    | Y___BUILTIN_ISFLOAT Y_LEFT_PAREN expect-type-id type-id Y_RIGHT_PAREN
    { $$ = setLocation( MakeBuiltinIsFloat( $4 ), &yylp[1] ); }
    | Y___OFFSETOF Y_LEFT_PAREN expect-type-id type-id Y_COMMA offsetof-field Y_RIGHT_PAREN
    { $$ = setLocation( PTreeOffsetof( $4, $6 ), &yylp[5] ); }
    ;

unary-operator
    : Y_TIMES
    { $$ = setLocation( PTreeUnary( CO_INDIRECT, NULL ), &yylp[1] ); }
    | Y_AND
    { $$ = setLocation( PTreeUnary( CO_ADDR_OF, NULL ), &yylp[1] ); }
    | Y_PLUS
    { $$ = setLocation( PTreeUnary( CO_UPLUS, NULL ), &yylp[1] ); }
    | Y_MINUS
    { $$ = setLocation( PTreeUnary( CO_UMINUS, NULL ), &yylp[1] ); }
    | Y_EXCLAMATION
    { $$ = setLocation( PTreeUnary( CO_EXCLAMATION, NULL ), &yylp[1] ); }
    | Y_TILDE
    { $$ = setLocation( PTreeUnary( CO_TILDE, NULL ), &yylp[1] ); }
    ;

new-expression
    : new-keyword new-modifier-opt new-placement new-type-id new-initializer-opt
    {
        $$ = setLocation( MakeNewExpr( $1, $3, $4, $5 ), &$1->locn );
        PTreeFreeSubtrees( $1 );
    }
    | new-keyword new-modifier-opt               new-type-id new-initializer-opt
    {
        $$ = setLocation( MakeNewExpr( $1, NULL, $3, $4 ), &$1->locn );
        PTreeFreeSubtrees( $1 );
    }
    | new-keyword new-modifier-opt new-placement Y_LEFT_PAREN special-new-type-id Y_RIGHT_PAREN new-initializer-opt /* TODO */
    {
        $$ = setLocation( MakeNewExpr( $1, $3, $5, $7 ), &$1->locn );
        PTreeFreeSubtrees( $1 );
    }
    | new-keyword new-modifier-opt               Y_LEFT_PAREN special-new-type-id Y_RIGHT_PAREN new-initializer-opt /* TODO */
    {
        $$ = setLocation( MakeNewExpr( $1, NULL, $4, $6 ), &$1->locn );
        PTreeFreeSubtrees( $1 );
    }
    ;

/* non-standard */
new-keyword
    : Y_NEW
    { $$ = makeUnary( CO_NEW, NULL); }
    | Y_GLOBAL_NEW
    ;

new-modifier-opt
    : /* nothing */
    | modifier
    { CheckNewModifier( $1 ); }
    ;

new-placement
    : Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    { $$ = $2; }
    ;

new-type-id
    : dynamic-type-id
    | Y_LEFT_PAREN special-new-type-id Y_RIGHT_PAREN
    { $$ = $2; }
    ;

new-initializer-opt
    : /* nothing */
    { $$ = NULL; }
    | new-initializer
    ;

new-initializer
    : Y_LEFT_PAREN Y_RIGHT_PAREN
    { $$ = NULL; } /* ARM p.61 '()' is syntactic convenience */
    | Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    { $$ = $2; }
    ;

dynamic-type-id
    : type-specifier-seq
    {
        $$ = MakeNewTypeId( DoDeclSpec( state->gstack->u.dspec ) );
        GStackPop( &(state->gstack) );
    }
    | type-specifier-seq new-declarator
    {
        $$ = $2;
        GStackPop( &(state->gstack) );
    }
    ;

/* TODO */
new-declarator
    : partial-ptr-declarator direct-new-declarator
    { $$ = MakeNewDeclarator( state->gstack->u.dspec, $1, $2 ); }
    | partial-ptr-declarator
    { $$ = MakeNewDeclarator( state->gstack->u.dspec, $1, NULL ); }
    |                        direct-new-declarator
    { $$ = MakeNewDeclarator( state->gstack->u.dspec, NULL, $1 ); }
    ;

delete-expression
    : delete-keyword cast-expression
    { $$ = setLocation( MakeDeleteExpr( $1, ( $1 == NULL ) ? CO_DELETE : CO_DELETE_G, $2 ), &yylp[1] ); }
    | delete-keyword Y_LEFT_BRACKET delete-size-expression-opt Y_RIGHT_BRACKET cast-expression
    { $$ = setLocation( MakeDeleteExpr( $1, ( $1 == NULL ) ? CO_DELETE_ARRAY : CO_DELETE_G_ARRAY, $5 ), &yylp[1] ); }
    ;

/* non-standard */
delete-keyword
    : Y_DELETE
    { $$ = NULL; }
    | Y_GLOBAL_DELETE
    ;

/* non-standard */
delete-size-expression-opt
    : /* nothing */
    | expression
    {
        PTreeDeleteSizeExpr( $1 );
    }
    ;

cast-expression
    : unary-expression
    | Y_LEFT_PAREN type-id Y_RIGHT_PAREN cast-expression
    { $$ = setLocation( MakeNormalCast( $2, $4 ), &yylp[3] ); }
    ;

/* extension */
segment-expression
    : cast-expression
    | segment-expression Y_SEG_OP cast-expression
    { $$ = setLocation( PTreeBinary( CO_SEG_OP, $1, $3 ), &yylp[2] ); }
    ;

pm-expression
    : segment-expression
    | pm-expression Y_DOT_STAR segment-expression
    { $$ = setLocation( PTreeBinary( CO_DOT_STAR, $1, $3 ), &yylp[2] ); }
    | pm-expression Y_ARROW_STAR segment-expression
    { $$ = setLocation( PTreeBinary( CO_ARROW_STAR, $1, $3 ), &yylp[2] ); }
    ;

multiplicative-expression
    : pm-expression
    | multiplicative-expression Y_TIMES pm-expression
    { $$ = setLocation( PTreeBinary( CO_TIMES, $1, $3 ), &yylp[2] ); }
    | multiplicative-expression Y_DIV pm-expression
    { $$ = setLocation( PTreeBinary( CO_DIVIDE, $1, $3 ), &yylp[2] ); }
    | multiplicative-expression Y_PERCENT pm-expression
    { $$ = setLocation( PTreeBinary( CO_PERCENT, $1, $3 ), &yylp[2] ); }
    ;

additive-expression
    : multiplicative-expression
    | additive-expression Y_PLUS multiplicative-expression
    { $$ = setLocation( PTreeBinary( CO_PLUS, $1, $3 ), &yylp[2] ); }
    | additive-expression Y_MINUS multiplicative-expression
    { $$ = setLocation( PTreeBinary( CO_MINUS, $1, $3 ), &yylp[2] ); }
    ;

shift-expression
    : additive-expression
    | shift-expression Y_RSHIFT additive-expression
    { $$ = setLocation( PTreeBinary( CO_RSHIFT, $1, $3 ), &yylp[2] ); }
    | shift-expression Y_LSHIFT additive-expression
    { $$ = setLocation( PTreeBinary( CO_LSHIFT, $1, $3 ), &yylp[2] ); }
    ;

relational-expression
    : shift-expression
    | relational-expression Y_LT shift-expression
    { $$ = setLocation( PTreeBinary( CO_LT, $1, $3 ), &yylp[2] ); }
    | relational-expression Y_LE shift-expression
    { $$ = setLocation( PTreeBinary( CO_LE, $1, $3 ), &yylp[2] ); }
    | relational-expression Y_GT shift-expression
    { $$ = setLocation( PTreeBinary( CO_GT, $1, $3 ), &yylp[2] ); }
    | relational-expression Y_GE shift-expression
    { $$ = setLocation( PTreeBinary( CO_GE, $1, $3 ), &yylp[2] ); }
    ;

equality-expression
    : relational-expression
    | equality-expression Y_EQ relational-expression
    { $$ = setLocation( PTreeBinary( CO_EQ, $1, $3 ), &yylp[2] ); }
    | equality-expression Y_NE relational-expression
    { $$ = setLocation( PTreeBinary( CO_NE, $1, $3 ), &yylp[2] ); }
    ;

and-expression
    : equality-expression
    | and-expression Y_AND equality-expression
    { $$ = setLocation( PTreeBinary( CO_AND, $1, $3 ), &yylp[2] ); }
    ;

exclusive-or-expression
    : and-expression
    | exclusive-or-expression Y_XOR and-expression
    { $$ = setLocation( PTreeBinary( CO_XOR, $1, $3 ), &yylp[2] ); }
    ;

inclusive-or-expression
    : exclusive-or-expression
    | inclusive-or-expression Y_OR exclusive-or-expression
    { $$ = setLocation( PTreeBinary( CO_OR, $1, $3 ), &yylp[2] ); }
    ;

logical-and-expression
    : inclusive-or-expression
    | logical-and-expression Y_AND_AND inclusive-or-expression
    { $$ = setLocation( PTreeBinary( CO_AND_AND, $1, $3 ), &yylp[2] ); }
    ;

logical-or-expression
    : logical-and-expression
    | logical-or-expression Y_OR_OR logical-and-expression
    { $$ = setLocation( PTreeBinary( CO_OR_OR, $1, $3 ), &yylp[2] ); }
    ;

conditional-expression
    : logical-or-expression
    | logical-or-expression Y_QUESTION expression Y_COLON assignment-expression
    {
        $3 = setLocation( PTreeBinary( CO_COLON, $3, $5 ), &yylp[4] );
        $$ = setLocation( PTreeBinary( CO_QUESTION, $1, $3 ), &yylp[2] );
    }
    ;

assignment-expression
    : conditional-expression
    | logical-or-expression assignment-operator assignment-expression
    {
        $$ = PTreeReplaceLeft( $2, $1 );
        $$ = PTreeReplaceRight( $$, $3 );
    }
    | Y_THROW assignment-expression-opt
    { $$ = setLocation( PTreeUnary( CO_THROW, $2 ), &yylp[1] ); }
    ;

assignment-expression-opt
    : /* nothing */
    { $$ = NULL; }
    | assignment-expression
    ;

assignment-operator
    : Y_EQUAL
    { $$ = setLocation( PTreeBinary( CO_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_TIMES_EQUAL
    { $$ = setLocation( PTreeBinary( CO_TIMES_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_DIV_EQUAL
    { $$ = setLocation( PTreeBinary( CO_DIVIDE_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_PERCENT_EQUAL
    { $$ = setLocation( PTreeBinary( CO_PERCENT_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_PLUS_EQUAL
    { $$ = setLocation( PTreeBinary( CO_PLUS_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_MINUS_EQUAL
    { $$ = setLocation( PTreeBinary( CO_MINUS_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_LSHIFT_EQUAL
    { $$ = setLocation( PTreeBinary( CO_LSHIFT_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_RSHIFT_EQUAL
    { $$ = setLocation( PTreeBinary( CO_RSHIFT_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_OR_EQUAL
    { $$ = setLocation( PTreeBinary( CO_OR_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_AND_EQUAL
    { $$ = setLocation( PTreeBinary( CO_AND_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_XOR_EQUAL
    { $$ = setLocation( PTreeBinary( CO_XOR_EQUAL, NULL, NULL ), &yylp[1] ); }
    ;

expression
    : assignment-expression
    | expression Y_COMMA assignment-expression
    { $$ = setLocation( PTreeBinary( CO_COMMA, $1, $3 ), &yylp[2] ); }
    ;

constant-expression
    : conditional-expression
    { $$ = PTreeNonZeroConstantExpr( $1 ); }
    ;

offsetof-field
    : make-id
    {
        $$ = PTreeBinary( CO_DOT, $1, NULL );
        $$ = PTreeBinary( CO_DOT, NULL, $$ );
    }
    | make-id offsetof-index
    {
        $$ = PTreeBinary( CO_DOT, $1, $2 );
        $$ = PTreeBinary( CO_DOT, NULL, $$ );
    }
    | offsetof-field Y_DOT make-id
    {
        $$ = setLocation( PTreeBinary( CO_DOT, $3, NULL ), &yylp[2] );
        $$ = setLocation( PTreeBinary( CO_DOT, $1, $$ ), &yylp[2] );
    }
    | offsetof-field Y_DOT make-id offsetof-index
    {
        $$ = setLocation( PTreeBinary( CO_DOT, $3, $4 ), &yylp[2] );
        $$ = setLocation( PTreeBinary( CO_DOT, $1, $$ ), &yylp[2] );
    }
    ;

offsetof-index
    : Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = setLocation( PTreeBinary( CO_INDEX, NULL, $2 ), &yylp[1] ); }
    | offsetof-index Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = setLocation( PTreeBinary( CO_INDEX, $1, $3 ), &yylp[2] ); }
    ;

partial-ptr-declarator
    : Y_TIMES cv-qualifier-seq-opt partial-ptr-declarator
    { $$ = MakeNewPointer( $2, $3, NULL ); }
    | Y_TIMES cv-qualifier-seq-opt
    { $$ = MakeNewPointer( $2, NULL, NULL ); }
    | Y_SCOPED_TIMES cv-qualifier-seq-opt partial-ptr-declarator
    { $$ = MakeNewPointer( $2, $3, $1 ); }
    | Y_SCOPED_TIMES cv-qualifier-seq-opt
    { $$ = MakeNewPointer( $2, NULL, $1 ); }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TIMES cv-qualifier-seq-opt partial-ptr-declarator
    { $$ = MakeNewPointer( $3, $4, $2 ); PTreeFreeSubtrees( $1 ); }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TIMES cv-qualifier-seq-opt
    { $$ = MakeNewPointer( $3, NULL, $2 ); PTreeFreeSubtrees( $1 ); }
    ;

direct-new-declarator
    : Y_LEFT_BRACKET expression Y_RIGHT_BRACKET
    { $$ = MakeNewDynamicArray( $2 ); }
    | direct-new-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    ;

make-id
    : Y_ID
    | Y_UNKNOWN_ID
    | Y_TEMPLATE_ID
    | Y_TYPE_NAME
    | Y_TEMPLATE_NAME
    | Y_NAMESPACE_NAME
    ;


/* A.5 Statements [gram.stmt] */


/* A.6 Declarations [gram.dcl] */

/* should only be referenced by might-restart-declarations */
declaration-seq
    : declaration
    | declaration-seq declaration
    ;

/* slightly differs from standard */
declaration
    : block-declaration-before-semicolon Y_SEMI_COLON
    | function-definition
    | template-declaration /* | explicit-specialization */
    | explicit-instantiation
    | linkage-specification
    | namespace-definition
    | Y_SEMI_COLON /* extension */
    ;

block-declaration-before-semicolon
    : block-declaration
    {
        if( t != Y_SEMI_COLON ) {
            SetErrLoc( &yylocation );
            CErr1( ERR_SYNTAX_MISSING_SEMICOLON );
            what = P_DIAGNOSED;
        }
    }
    ;

block-declaration
    : simple-declaration
    | asm-definition
    | namespace-alias-definition
    | using-declaration
    | using-directive
    | static_assert-declaration
    ;

simple-declaration
    : decl-specifier-seq init-declarator-list
    {
        CheckDeclarationDSpec( state->gstack->u.dspec, GetCurrScope() );
        GStackPop( &(state->gstack) );
    }
    | no-declarator-declaration
    {
        CheckDeclarationDSpec( state->gstack->u.dspec, GetCurrScope() );
        GStackPop( &(state->gstack) );
    }
    ;

static_assert-declaration
    : Y_STATIC_ASSERT Y_LEFT_PAREN constant-expression Y_COMMA expect-string-literal string-literal Y_RIGHT_PAREN
    {
        /* see N1720 -- Proposal to Add Static Assertions to the Core
         * Language (Revision 3) */
        DbgAssert( $3->op == PT_INT_CONSTANT );
        DbgAssert( $6->op == PT_STRING_CONSTANT );

        if( $3->u.int_constant == 0 ) {
            CErr2p( ERR_STATIC_ASSERTION_FAILURE,
                    StringBytes( $6->u.string ) );
        }

        PTreeFreeSubtrees( $3 );
        PTreeFreeSubtrees( $6 );
    }
    ;

/* TODO */
decl-specifier-seq
    : non-type-decl-specifier-seq type-specifier maybe-type-decl-specifier-seq
    {
        $1 = PTypeCombine( $1, $2 );
        $$ = PTypeCombine( $1, $3 );
        $$ = PTypeDone( $$, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    | non-type-decl-specifier-seq type-specifier
    {
        $$ = PTypeCombine( $1, $2 );
        $$ = PTypeDone( $$, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    |                            type-specifier maybe-type-decl-specifier-seq
    {
        $$ = PTypeCombine( $1, $2 );
        $$ = PTypeDone( $$, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    |                            type-specifier
    {
        $$ = PTypeDone( $1, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    | non-type-decl-specifier-seq
    {
        $$ = PTypeDone( $1, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    ;

/* non-standard */
non-type-decl-specifier-seq
    : non-type-decl-specifier
    | non-type-decl-specifier-seq non-type-decl-specifier
    { $$ = PTypeCombine( $1, $2 ); }
    ;

/* non-standard */
maybe-type-decl-specifier-seq
    : non-type-decl-specifier
    | basic-type-specifier
    | maybe-type-decl-specifier-seq non-type-decl-specifier
    { $$ = PTypeCombine( $1, $2 ); }
    | maybe-type-decl-specifier-seq basic-type-specifier
    { $$ = PTypeCombine( $1, $2 ); }
    ;

/* non-standard */
non-type-decl-specifier
    : storage-class-specifier
    | function-specifier
    | cv-qualifier
    | ms-specific-declspec
    | Y_FRIEND
    { $$ = PTypeSpecifier( STY_FRIEND ); }
    ;

/* non-standard */
ms-specific-declspec
    : Y___DECLSPEC Y_LEFT_PAREN                 Y_RIGHT_PAREN
    { $$ = PTypeMSDeclSpec( NULL, NULL ); }
    | Y___DECLSPEC Y_LEFT_PAREN ms-declspec-seq Y_RIGHT_PAREN
    { $$ = $3; }
    ;

/* non-standard */
ms-declspec-seq
    : make-id
    { $$ = PTypeMSDeclSpec( NULL, $1 ); }
    | pragma-modifier
    { $$ = PTypeMSDeclSpecModifier( NULL, $1 ); }
    | ms-declspec-seq make-id
    { $$ = PTypeMSDeclSpec( $1, $2 ); }
    | ms-declspec-seq pragma-modifier
    { $$ = PTypeMSDeclSpecModifier( $1, $2 ); }
    ;

storage-class-specifier
    : Y_AUTO
    { $$ = PTypeStgClass( STG_AUTO ); }
    | Y_REGISTER
    { $$ = PTypeStgClass( STG_REGISTER ); }
    | Y_STATIC
    { $$ = PTypeStgClass( STG_STATIC ); }
    | Y_EXTERN
    { $$ = PTypeStgClass( STG_EXTERN ); }
    | Y_MUTABLE
    { $$ = PTypeStgClass( STG_MUTABLE ); }
    /* extension */
    | Y_EXTERN linkage-id
    { $$ = PTypeLinkage(); }
    | Y_TYPEDEF
    { $$ = PTypeStgClass( STG_TYPEDEF ); }
    ;

function-specifier
    : Y_INLINE
    { $$ = PTypeSpecifier( STY_INLINE ); }
    | Y_VIRTUAL
    { $$ = PTypeSpecifier( STY_VIRTUAL ); }
    | Y_EXPLICIT
    { $$ = PTypeSpecifier( STY_EXPLICIT ); }
    /* extension */
    | Y___INLINE
    { $$ = PTypeSpecifier( STY_INLINE ); }
    ;

type-specifier
    : simple-type-specifier %prec Y_FAVOUR_REDUCE_SPECIAL %ambig 0 Y_LEFT_PAREN
    | class-specifier
    | enum-specifier
    | elaborated-type-specifier
    | typename-specifier
    /* cv-qualifier */
    ;

simple-type-specifier
    : Y_TYPE_NAME
    { $$ = sendType( $1 ); }
    | qualified-type-specifier
    | basic-type-specifier
    /* see N1978 -- Decltype (Revision 5) */
    | Y_DECLTYPE Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = PTypeExpr( $3 ); }
    /* extension */
    | typeof-specifier
    ;

/* non-standard */
qualified-type-specifier
    : Y_GLOBAL_TYPE_NAME
    { $$ = sendType( $1 ); }
    | Y_SCOPED_TYPE_NAME
    { $$ = sendType( $1 ); }
    | nested-name-specifier
    {
        $$ = PTypeClassInstantiation( state->class_colon, $1 );
    }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TYPE_NAME
    {
        PTreeFreeSubtrees( $1 );
        $$ = sendType( $2 );
    }
    ;

enum-specifier
    : enum-key enum-start enumerator-list Y_COMMA Y_RIGHT_BRACE
    {
        $$ = MakeEnumType( &(state->gstack->u.enumdata) );
        GStackPop( &(state->gstack) );
        // TODO: warning non-standard
    }
    | enum-key enum-start enumerator-list         Y_RIGHT_BRACE
    {
        $$ = MakeEnumType( &(state->gstack->u.enumdata) );
        GStackPop( &(state->gstack) );
    }
    | enum-key enum-start                         Y_RIGHT_BRACE
    {
        $$ = MakeEnumType( &(state->gstack->u.enumdata) );
        GStackPop( &(state->gstack) );
    }
    ;

/* non-standard */
enum-start
    : Y_LEFT_BRACE
    { EnumDefine( &(state->gstack->u.enumdata) ); }
    ;

/* non-standard */
enum-key
    : Y_ENUM make-id
    {
        GStackPush( &(state->gstack), GS_ENUM_DATA );
        InitEnumState( &(state->gstack->u.enumdata), $2 );
    }
    | Y_ENUM
    {
        GStackPush( &(state->gstack), GS_ENUM_DATA );
        InitEnumState( &(state->gstack->u.enumdata), NULL );
    }
    ;

enumerator-list
    : enumerator-definition
    | enumerator-list Y_COMMA enumerator-definition
    ;

/* TODO */
enumerator-definition
    : enumerator
    {
        MakeEnumMember( &(state->gstack->u.enumdata), $1, NULL );
    }
    | enumerator Y_EQUAL constant-expression
    {
        MakeEnumMember( &(state->gstack->u.enumdata), $1, $3 );
    }
    ;

enumerator
    : make-id /* identifier */
    ;

elaborated-type-specifier
    : Y_ENUM make-id
    {
        ENUM_DATA edata;
        InitEnumState( &edata, $2 );
        $$ = EnumReference( &edata );
    }
    | Y_ENUM elaborated-type-name
    {
        ENUM_DATA edata;
        InitEnumState( &edata, $2 );
        $$ = EnumReference( &edata );
    }
    ;

namespace-definition
    /* named-namespace-definition | unnamed-namespace-definition */
    : namespace-key Y_LEFT_BRACE namespace-body Y_RIGHT_BRACE
    ;

/* non-standard */
namespace-key
    : Y_NAMESPACE
    { NameSpaceUnnamed( &yylp[1] ); }
    | Y_NAMESPACE expect-identifier make-id
    { NameSpaceNamed( $3 ); }
    ;

namespace-body
    : /* nothing */
    { NameSpaceClose(); }
    | might-restart-declarations
    { NameSpaceClose(); }
    ;

namespace-alias-definition
    : Y_NAMESPACE expect-identifier make-id Y_EQUAL expect-qualified-namespace-specifier qualified-namespace-specifier
    { NameSpaceAlias( $3, $6 ); }
    ;

qualified-namespace-specifier
    : Y_NAMESPACE_NAME
    | Y_GLOBAL_NAMESPACE_NAME
    | Y_SCOPED_NAMESPACE_NAME
    ;

using-declaration
    : Y_USING qualified-id
    { NameSpaceUsingDeclId( $2 ); }
    | Y_USING qualified-type-specifier
    { NameSpaceUsingDeclType( $2 ); }
    | Y_USING Y_GLOBAL_TEMPLATE_NAME
    { NameSpaceUsingDeclTemplateName( $2 ); }
    | Y_USING Y_SCOPED_TEMPLATE_NAME
    { NameSpaceUsingDeclTemplateName( $2 ); }
    | Y_USING qualified-namespace-specifier
    {
        CErr2p( ERR_NAMESPACE_NOT_ALLOWED_IN_USING_DECL, $2 );
        PTreeFreeSubtrees( $2 );
    }
    ;

using-directive
    : Y_USING Y_NAMESPACE expect-qualified-namespace-specifier qualified-namespace-specifier
    { NameSpaceUsingDirective( $4 ); }
    ;

asm-definition
    : Y_ASM Y_LEFT_PAREN expect-string-literal string-literal Y_RIGHT_PAREN
    {
        StringTrash( $4->u.string );
        PTreeFree( $4 );
        CErr1( WARN_ASM_IGNORED );
    }
    ;

linkage-specification
    : Y_EXTERN linkage-id start-linkage-block linkage-body Y_RIGHT_BRACE
    { LinkagePop(); }
    ;

linkage-id
    : string-literal
    {
        LinkagePush( $1->u.string->string );
        StringTrash( $1->u.string );
        PTreeFree( $1 );
        if( ! ScopeType( GetCurrScope(), SCOPE_FILE ) ) {
            CErr1( ERR_ONLY_GLOBAL_LINKAGES );
        }
    }
    ;

start-linkage-block
    : Y_LEFT_BRACE
    { LinkageBlock(); }
    ;

linkage-body
    : /* nothing */
    | might-restart-declarations
    ;


/* A.7 Declarators [gram.decl] */

init-declarator-list
    : init-declarator
    {
        tryCtorStyleInit( state, $1 );
    }
    | init-declarator-list Y_COMMA comma-init-declarator
    {
        tryCtorStyleInit( state, $3 );
    }
    ;

init-declarator
    : declarator
    {
        $$ = InsertDeclInfo( GetCurrScope(), $1 );
        GStackPush( &(state->gstack), GS_INIT_DATA );
        $$ = DataInitNoInit( &(state->gstack->u.initdata), $$ );
        GStackPop( &(state->gstack) );
    }
    | declaring-declarator initializer
    { $$ = $1; }
    | ptr-mod-init-declarator
    {
        $1 = FinishDeclarator( state->gstack->u.dspec, $1 );
        $$ = InsertDeclInfo( GetCurrScope(), $1 );
    }
    | direct-declarator Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    {
        $1 = FinishDeclarator( state->gstack->u.dspec, $1 );
        $$ = InsertDeclInfo( GetCurrScope(), $1 );
        setInitWithLocn( $$, $3, &yylp[2] );
    }
    ;

declarator
    : ptr-mod-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    | direct-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    ;

comma-init-declarator
    : comma-declarator
    {
        $$ = InsertDeclInfo( GetCurrScope(), $1 );
        GStackPush( &(state->gstack), GS_INIT_DATA );
        $$ = DataInitNoInit( &(state->gstack->u.initdata), $$ );
        GStackPop( &(state->gstack) );
    }
    | comma-declaring-declarator initializer
    { $$ = $1; }
    | cv-qualifier-seq-opt ptr-mod-init-declarator
    {
        $2 = AddMSCVQualifierKludge( $1, $2 );
        $2 = FinishDeclarator( state->gstack->u.dspec, $2 );
        $$ = InsertDeclInfo( GetCurrScope(), $2 );
    }
    | cv-qualifier-seq-opt direct-declarator Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    {
        $2 = AddMSCVQualifierKludge( $1, $2 );
        $2 = FinishDeclarator( state->gstack->u.dspec, $2 );
        $$ = InsertDeclInfo( GetCurrScope(), $2 );
        setInitWithLocn( $$, $4, &yylp[3] );
    }
    ;

declaring-declarator
    : declarator
    {
        $$ = InsertDeclInfo( GetCurrScope(), $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    ;

comma-declaring-declarator
    : comma-declarator
    {
        $$ = InsertDeclInfo( GetCurrScope(), $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    ;

comma-declarator
    : cv-qualifier-seq-opt ptr-mod-declarator
    {
        $2 = AddMSCVQualifierKludge( $1, $2 );
        $$ = FinishDeclarator( state->gstack->u.dspec, $2 );
    }
    | cv-qualifier-seq-opt direct-declarator
    {
        $2 = AddMSCVQualifierKludge( $1, $2 );
        $$ = FinishDeclarator( state->gstack->u.dspec, $2 );
    }
    ;

ptr-mod
    : modifier
    { $$ = MakeFlagModifier( $1 ); }
    | Y___BASED Y_LEFT_PAREN based-expression Y_RIGHT_PAREN
    { $$ = $3; }
    | pragma-modifier
    | ptr-operator
    ;

ptr-mod-init-declarator
    : ptr-mod ptr-mod-init-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod direct-declarator Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    {
        $$ = AddDeclarator( $2, $1 );
        setInitWithLocn( $$, $4, &yylp[3] );
    }
    ;

ptr-mod-declarator
    : ptr-mod ptr-mod-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod direct-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    ;

direct-declarator
    : declarator-id
    {
        $$ = MakeDeclarator( state->gstack->u.dspec, $1 );
        if( $$ == NULL ) {
            what = P_SYNTAX;
        } else {
            if( $$->template_member ) {
                what = P_CLASS_TEMPLATE;
            }
        }
    }
    | direct-declarator Y_LEFT_PAREN parameter-declaration-clause Y_RIGHT_PAREN cv-qualifier-seq-opt exception-specification-opt
    {
        $$ = AddDeclarator( $1, MakeFnType( &($3), $5, $6 ) );
        $$ = AddExplicitParms( $$, $3 );
    }
    | direct-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    | direct-declarator Y_LEFT_BRACKET                     Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, NULL ); }
    | Y_LEFT_PAREN ptr-mod-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    | Y_LEFT_PAREN direct-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    ;

modifier
    : Y___NEAR
    { $$ = TF1_NEAR; }
    | Y___FAR
    { $$ = TF1_FAR; }
    | Y__FAR16
    { $$ = TF1_SET_FAR16;       /* equals TF1_FAR on the 8086 */ }
    | Y___FAR16
    { $$ = TF1_SET_FAR16;       /* equals TF1_FAR on the 8086 */ }
    | Y___HUGE
    { $$ = TF1_SET_HUGE;        /* equals TF1_FAR on the 386 */ }
    | Y__EXPORT
    { $$ = TF1_DLLEXPORT | TF1_HUG_FUNCTION; }
    | Y___EXPORT
    { $$ = TF1_DLLEXPORT | TF1_HUG_FUNCTION; }
    | Y___LOADDS
    { $$ = TF1_LOADDS | TF1_TYP_FUNCTION; }
    | Y___SAVEREGS
    { $$ = TF1_SAVEREGS | TF1_TYP_FUNCTION; }
    | Y___INTERRUPT
    { $$ = TF1_INTERRUPT | TF1_TYP_FUNCTION; }
    ;

based-expression
    : segment-cast-opt Y___SEGNAME Y_LEFT_PAREN expect-string-literal string-literal Y_RIGHT_PAREN
    { $$ = MakeBasedModifier( TF1_BASED_STRING, $1, $5 ); }
    | segment-cast-opt identifier
    { $$ = MakeBasedModifier( TF1_NULL, $1, $2 ); }
    | segment-cast-opt Y_VOID
    { $$ = MakeBasedModifier( TF1_BASED_VOID, $1, NULL ); }
    | segment-cast-opt Y___SELF
    { $$ = MakeBasedModifier( TF1_BASED_SELF, $1, NULL ); }
    ;

segment-cast-opt
    : /* nothing */
    { $$ = FALSE; }
    | Y_LEFT_PAREN Y___SEGMENT Y_RIGHT_PAREN
    { $$ = TRUE; }
    ;

pragma-modifier
    : Y___PRAGMA Y_LEFT_PAREN pragma-id Y_RIGHT_PAREN
    {
        $$ = MakePragma( $3->u.string->string );
        StringTrash( $3->u.string );
        PTreeFree( $3 );
    }
    | Y__CDECL
    { $$ = MakeIndexPragma( M_CDECL ); }
    | Y___CDECL
    { $$ = MakeIndexPragma( M_CDECL ); }
    | Y__FASTCALL
    { $$ = MakeIndexPragma( M_FASTCALL ); }
    | Y___FASTCALL
    { $$ = MakeIndexPragma( M_FASTCALL ); }
    | Y___FORTRAN
    { $$ = MakeIndexPragma( M_FORTRAN ); }
    | Y__OPTLINK
    { $$ = MakeIndexPragma( M_OPTLINK ); }
    | Y__PASCAL
    { $$ = MakeIndexPragma( M_PASCAL ); }
    | Y___PASCAL
    { $$ = MakeIndexPragma( M_PASCAL ); }
    | Y___STDCALL
    { $$ = MakeIndexPragma( M_STDCALL ); }
    | Y__SYSCALL
    { $$ = MakeIndexPragma( M_SYSCALL ); }
    | Y___SYSCALL
    { $$ = MakeIndexPragma( M_SYSCALL ); }
    | Y__SYSTEM
    { $$ = MakeIndexPragma( M_SYSTEM ); }
    | Y___WATCALL
    { $$ = MakeIndexPragma( M_WATCALL ); }
    ;

pragma-id
    : string-literal
    ;

ptr-operator
    : Y_TIMES cv-qualifier-seq-opt
    { $$ = MakePointerType( TF1_NULL, $2 ); }
    | Y_AND cv-qualifier-seq-opt
    { $$ = MakePointerType( TF1_REFERENCE, $2 ); }
    | Y_SCOPED_TIMES cv-qualifier-seq-opt
    { $$ = MakeMemberPointer( $1, $2 ); }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TIMES cv-qualifier-seq-opt
    { $$ = MakeMemberPointer( $2, $3 ); PTreeFreeSubtrees( $1 ); }
    /* extension */
    | Y_TIMES Y__SEG16 cv-qualifier-seq-opt
    { $$ = MakeSeg16Pointer( $3 ); }
    ;

cv-qualifier-seq-opt
    : /* nothing */
    { $$ = STY_NULL; }
    | Y_CONST
    { $$ = STY_CONST; }
    | Y_VOLATILE
    { $$ = STY_VOLATILE; }
    | Y___UNALIGNED
    { $$ = STY_UNALIGNED; }
    | Y_CONST Y_VOLATILE
    { $$ = STY_CONST | STY_VOLATILE; }
    | Y_VOLATILE Y_CONST
    { $$ = STY_CONST | STY_VOLATILE; }
    | Y_CONST Y___UNALIGNED
    { $$ = STY_CONST | STY_UNALIGNED; }
    | Y___UNALIGNED Y_CONST
    { $$ = STY_CONST | STY_UNALIGNED; }
    | Y_VOLATILE Y___UNALIGNED
    { $$ = STY_VOLATILE | STY_UNALIGNED; }
    | Y___UNALIGNED Y_VOLATILE
    { $$ = STY_VOLATILE | STY_UNALIGNED; }
    | Y___UNALIGNED Y_CONST Y_VOLATILE
    { $$ = STY_CONST | STY_VOLATILE | STY_UNALIGNED; }
    | Y___UNALIGNED Y_VOLATILE Y_CONST
    { $$ = STY_CONST | STY_VOLATILE | STY_UNALIGNED; }
    | Y_VOLATILE Y_CONST Y___UNALIGNED
    { $$ = STY_CONST | STY_VOLATILE | STY_UNALIGNED; }
    | Y_VOLATILE Y___UNALIGNED Y_CONST
    { $$ = STY_CONST | STY_VOLATILE | STY_UNALIGNED; }
    | Y_CONST Y_VOLATILE Y___UNALIGNED
    { $$ = STY_CONST | STY_VOLATILE | STY_UNALIGNED; }
    | Y_CONST Y___UNALIGNED Y_VOLATILE
    { $$ = STY_CONST | STY_VOLATILE | STY_UNALIGNED; }
    ;

declarator-id
    : id-expression
    | Y_TEMPLATE_NAME /* non-standard */
    | Y_NAMESPACE_NAME /* non-standard */
    | Y_TYPE_NAME /* non-standard */
    | Y_GLOBAL_UNKNOWN_ID /* :: identifier */
    { $$ = MakeGlobalId( $1 ); }
    | Y_SCOPED_UNKNOWN_ID
    { $$ = MakeScopedId( $1 ); }
    | nested-name-specifier Y_TEMPLATE_SCOPED_UNKNOWN_ID
    { PTreeFreeSubtrees( $1 ); $$ = MakeScopedId( $2 ); }
    /*
     * these Y_TEMPLATE_SCOPED_ cases are needed because of the odd way
     * template class constructors are parsed. Essentially, a constructor
     * of the form A<T>::A() gets parsed as a function with A<T> being the
     * return type and ::A being the declarator-id. zapTemplateClassDeclSpec
     * then cleans things up by converting it into a constructor.
     */
    | Y_TEMPLATE_SCOPED_ID
    { $$ = MakeScopedId( $1 ); zapTemplateClassDeclSpec( state ); }
    | Y_TEMPLATE_SCOPED_UNKNOWN_ID
    { $$ = MakeScopedId( $1 ); zapTemplateClassDeclSpec( state ); }
    | Y_TEMPLATE_SCOPED_TILDE make-id
    { $$ = MakeScopedDestructorId( $1, $2 ); zapTemplateClassDeclSpec( state ); }
    | template-scoped-conversion-function-id
    {
        zapTemplateClassDeclSpec( state );
    }
    | template-scoped-operator-function-id
    {
        zapTemplateClassDeclSpec( state );
    }
    ;

type-id
    : type-specifier-seq abstract-declarator
    {
        $$ = TypeDeclarator( $2 );
        GStackPop( &(state->gstack) );
    }
    | type-specifier-seq
    {
        $$ = DoDeclSpec( state->gstack->u.dspec );
        GStackPop( &(state->gstack) );
    }
    ;

type-specifier-seq
    : decl-specifier-seq
    {
        $$ = CheckTypeSpecifier( $1 );
        if( $$ == NULL ) {
            what = P_SYNTAX;
        }
    }
    ;

abstract-declarator
    : abstract-ptr-mod-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    | direct-abstract-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    ;

abstract-ptr-mod-declarator
    : ptr-mod abstract-ptr-mod-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod direct-abstract-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod
    { $$ = MakeAbstractDeclarator( $1 ); }
    ;

direct-abstract-declarator
    : direct-abstract-declarator Y_LEFT_PAREN parameter-declaration-clause Y_RIGHT_PAREN cv-qualifier-seq-opt exception-specification-opt
    {
        $$ = AddDeclarator( $1, MakeFnType( &($3), $5, $6 ) );
        FreeArgs( $3 );
    }
    | Y_LEFT_PAREN parameter-declaration-clause Y_RIGHT_PAREN cv-qualifier-seq-opt exception-specification-opt
    {
        $$ = MakeAbstractDeclarator( MakeFnType( &($2), $4, $5 ) );
        FreeArgs( $2 );
    }
    | direct-abstract-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    |                            Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    {
        $$ = MakeAbstractDeclarator( NULL );
        $$ = AddArrayDeclarator( $$, $2 );
    }
    | direct-abstract-declarator Y_LEFT_BRACKET Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, NULL ); }
    |                            Y_LEFT_BRACKET Y_RIGHT_BRACKET
    {
        $$ = MakeAbstractDeclarator( NULL );
        $$ = AddArrayDeclarator( $$, NULL );
    }
    | Y_LEFT_PAREN direct-abstract-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    | Y_LEFT_PAREN abstract-ptr-mod-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    ;

parameter-declaration-clause
    : /* nothing */
    { $$ = NULL; }
    | parameter-declaration-list
    | parameter-declaration-list Y_COMMA Y_DOT_DOT_DOT
    { $$ = AddEllipseArg( $1 ); }
    | parameter-declaration-list         Y_DOT_DOT_DOT
    { $$ = AddEllipseArg( $1 ); }
    | Y_DOT_DOT_DOT
    { $$ = AddEllipseArg( NULL ); }
    ;

parameter-declaration-list
    : parameter-declaration
    { $$ = AddArgument( NULL, $1 ); }
    | parameter-declaration-list Y_COMMA parameter-declaration
    { $$ = AddArgument( $1, $3 ); }
    ;

arg-decl-specifier-seq
    : decl-specifier-seq
    {
        $$ = CheckArgDSpec( $1 );
        if( $$ == NULL ) {
            what = P_SYNTAX;
        }
    }
    ;

parameter-declaration
    : simple-parameter-declaration
    { GStackPop( &(state->gstack) ); }
    ;

/* non standard */
simple-parameter-declaration
    : arg-decl-specifier-seq
    { $$ = DeclSpecDeclarator( $1 ); }
    | arg-decl-specifier-seq simple-arg-no-id defarg-parse-or-copy
    { $$ = $2; }
    | arg-decl-specifier-seq declarator
    { $$ = $2; }
    | arg-decl-specifier-seq declarator defarg-parse-or-copy
    { $$ = $2; }
    | arg-decl-specifier-seq abstract-declarator
    { $$ = $2; }
    | arg-decl-specifier-seq abstract-declarator defarg-parse-or-copy
    { $$ = $2; }
    ;

simple-arg-no-id
    : /* nothing */
    { $$ = DeclSpecDeclarator( $<dspec>0 ); }
    ;

defarg-parse-or-copy
    : defarg-check Y_EQUAL assignment-expression
    {
        DECL_INFO *dinfo;

        DbgAssert( $1 == NULL );
        dinfo = $<dinfo>0;
        dinfo->defarg_expr = $3;
        dinfo->has_defarg = TRUE;
        TokenLocnAssign( dinfo->init_locn, yylp[2] );
    }
    | defarg-check Y_DEFARG_GONE_SPECIAL
    {
        DECL_INFO *dinfo;

        dinfo = $<dinfo>0;
        dinfo->defarg_rewrite = $1;
        dinfo->has_defarg = TRUE;
        TokenLocnAssign( dinfo->init_locn, yylp[2] );
    }
    ;

type-defarg-parse-or-copy
    : defarg-check Y_EQUAL type-id
    {
        DECL_INFO *dinfo;

        DbgAssert( $1 == NULL );
        dinfo = $<dinfo>0;
        dinfo->defarg_expr = $3;
        dinfo->has_defarg = TRUE;
        TokenLocnAssign( dinfo->init_locn, yylp[2] );
    }
    | defarg-check Y_DEFARG_GONE_SPECIAL
    {
        DECL_INFO *dinfo;

        dinfo = $<dinfo>0;
        dinfo->defarg_rewrite = $1;
        dinfo->has_defarg = TRUE;
        TokenLocnAssign( dinfo->init_locn, yylp[2] );
    }
    ;

defarg-check
    : /* nothing */
    {
        if( t != Y_EQUAL ) {
            what = P_SYNTAX;
            $$ = NULL;
        } else {
            angle_bracket_stack *angle_state;

            angle_state = VstkTop( &(state->angle_stack) );
            if( state->template_decl && ( angle_state != NULL )
             && ( angle_state->paren_depth == 0 ) ) {
                $$ = RewritePackageTemplateDefArg();
                t = Y_DEFARG_GONE_SPECIAL;
            } else if( state->template_decl ) {
                $$ = RewritePackagePassThrough( currParseStack->template_record_tokens );
                t = Y_DEFARG_GONE_SPECIAL;
            } else if( ClassOKToRewrite() ) {
                $$ = RewritePackageDefArg( NULL );
                t = Y_DEFARG_GONE_SPECIAL;
            } else {
                $$ = NULL;
            }
        }
    }
    ;

ctor-declarator
    : Y_LEFT_PAREN parameter-declaration-clause Y_RIGHT_PAREN cv-qualifier-seq-opt exception-specification-opt
    {
        DECL_SPEC *dspec;
        PTREE id;

        dspec = state->gstack->u.dspec;
        id = MakeConstructorId( dspec );
        if( id == NULL ) {
            what = P_SYNTAX;
        }
        $$ = MakeDeclarator( dspec, id );
        AddDeclarator( $$, MakeFnType( &($2), $4, $5 ) );
        $$ = AddExplicitParms( $$, $2 );
        if( $$->template_member ) {
            what = P_CLASS_TEMPLATE;
        } else {
            $$ = FinishDeclarator( dspec, $$ );
        }
    }
    ;

function-declaration
    : decl-specifier-seq declarator ctor-initializer
    {
        $2->mem_init = $3;
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    | decl-specifier-seq declarator
    {
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    |                    declarator ctor-initializer
    {
        $1->mem_init = $2;
        $$ = DeclFunction( NULL, $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    |                    declarator
    {
        $$ = DeclFunction( NULL, $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    | decl-specifier-seq ctor-declarator ctor-initializer
    {
        $2->mem_init = $3;
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    | decl-specifier-seq ctor-declarator
    {
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    ;

function-definition
    : function-declaration function-body
    {
        GStackPop( &(state->gstack) );  /* decl-info */
        if( $1->has_dspec ) {
            GStackPop( &(state->gstack) );      /* decl-spec */
        }
        if( $1->body != NULL ) {
            if( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_DECL ) ) {
                TemplateFunctionAttachDefn( $1 );
            } else if( ( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_INST )
                    && ! $1->friend_fn ) ) {
                TemplateMemberAttachDefn( $1, FALSE );
            } else if( ( ( ScopeType( GetCurrScope(), SCOPE_CLASS )
                        && ScopeType( GetCurrScope()->enclosing,
                                      SCOPE_TEMPLATE_INST ) )
                    && ! $1->friend_fn ) ) {
                if( GetCurrScope()->owner.type->flag & TF1_SPECIFIC ) {
                    ClassStoreInlineFunc( $1 );
                } else {
                    TemplateMemberAttachDefn( $1, TRUE );
                }
            } else {
                ClassStoreInlineFunc( $1 );
            }
        } else {
            FreeDeclInfo( $1 );
        }
    }
    ;

function-body
    : check-for-rewrite Y_LEFT_BRACE absorb-body Y_RIGHT_BRACE
    ;

check-for-rewrite
    : /* nothing */
    {
        /* guards against errors from Q during rewriting:
            void foo( void )
            {
                Q::blah
            }
        */
        setNoSuperTokens( state );
    }
    ;

absorb-body
    : /* nothing */
    {
        /* token will be thrown away (see check-for-rewrite) */
        ParseFlush();
        if( doFnbodyRewrite() ) {
            REWRITE *rewrite;
            rewrite = RewritePackageFunction( getMultiToken() );
            state->gstack->u.dinfo->body = rewrite;
        } else {
            FunctionBody( state->gstack->u.dinfo );
        }
        what = P_RELEX;
    }
    ;

initializer
    : init-start assignment-expression
    {
        DataInitSimple( $2 );
        GStackPop( &(state->gstack) );
    }
    | init-start braced-initializer
    {
        GStackPop( &(state->gstack) );
    }
    ;

init-start
    : Y_EQUAL
    {
        DECL_INFO *dinfo;

        dinfo = state->gstack->u.dinfo;
        reuseGStack( state, GS_INIT_DATA );
        DataInitStart( &(state->gstack->u.initdata), dinfo );
        DataInitSimpleLocn( &yylp[1] );
    }
    ;

initializer-list
    : assignment-expression
    { DataInitExpr( $1 ); }
    | initializer-list Y_COMMA assignment-expression
    { DataInitExpr( $3 ); }
    | braced-initializer
    | initializer-list Y_COMMA braced-initializer
    ;


/* A.8 Classes [gram.class] */

/* r/r conflict:
 *   class-specifier <- class-key class-name
 *   start-class <-
 */
start-class
    : /* nothing */
    {
        ClassStart();
        pushDefaultDeclSpec( state );
    }
    ;

invalid-class-name-id
    : Y_GLOBAL_UNKNOWN_ID
    | Y_SCOPED_UNKNOWN_ID
    ;

class-name-id
    : Y_ID
    | Y_UNKNOWN_ID
    | Y_TEMPLATE_ID
    | Y_TYPE_NAME
    | Y_TEMPLATE_NAME
    | Y_NAMESPACE_NAME
    | Y_GLOBAL_ID
    | Y_GLOBAL_TYPE_NAME
    | Y_SCOPED_ID
    | Y_SCOPED_TYPE_NAME
    | invalid-class-name-id
    {
        CErr2p( ERR_QUALIFIED_NAME_NOT_CLASS, $1 );
        $$ = PTreeErrorNode( $1 );
    }
    ;

class-name
    : class-name-id
    {
        CLASS_DECL decl_type;
        CLNAME_STATE after_name;

        decl_type = CLASS_REFERENCE;
        switch( t ) {
        case Y_LEFT_BRACE:
        case Y_COLON:
            decl_type = CLASS_DEFINITION;
            break;
        case Y_SEMI_COLON:
            decl_type = CLASS_DECLARATION;
            break;
        }
        after_name = ClassName( $1, decl_type );
        switch( after_name ) {
        case CLNAME_CONTINUE:
            what = P_RELEX;
            break;
        case CLNAME_PROBLEM:
            what = P_DIAGNOSED;
            break;
        }
    }
    | nested-name-specifier
    {
        CLASS_DECL decl_type;
        tc_directive tcd_control;

        tcd_control = TCD_NULL;
        decl_type = CLASS_REFERENCE;

        switch( t ) {
        case Y_LEFT_BRACE:
        case Y_COLON:
            decl_type = CLASS_DEFINITION;
            break;
        case Y_SEMI_COLON:
            if( state->template_extern ) {
                tcd_control |= TCD_EXTERN;
            } else if( state->template_instantiate ) {
                tcd_control |= TCD_INSTANTIATE;
            }
            decl_type = CLASS_DECLARATION;
            break;
        }
        ClassSpecificInstantiation( $1, decl_type, tcd_control );
        what = P_RELEX;
    }
    ;

class-specifier
    : class-key class-name
    {
        $$ = ClassRefDef();
        GStackPop( &(state->gstack) );
    }
    | class-key class-name base-clause class-body
    { $$ = $4; }
    | class-key class-name             class-body
    { $$ = $3; }
    | class-key no-class-name base-clause class-body
    { $$ = $4; }
    | class-key no-class-name             class-body
    { $$ = $3; }
    ;

class-key
    : packed-class-opt Y_CLASS class-mod-opt
    {
        pushClassData( state, TF1_NULL, $1, $3 );
    }
    | packed-class-opt Y_STRUCT class-mod-opt
    {
        pushClassData( state, TF1_STRUCT, $1, $3 );
    }
    | packed-class-opt Y_UNION class-mod-opt
    {
        pushClassData( state, TF1_UNION, $1, $3 );
    }
    ;

class-body
    : class-substance Y_RIGHT_BRACE
    {
        GStackPop( &(state->gstack) );
        GStackPop( &(state->gstack) );
        $$ = $1;
    }
    ;

class-substance
    : start-class Y_LEFT_BRACE member-specification-opt
    { $$ = ClassEnd(); what = P_RELEX; }
    ;

no-class-name
    : /* nothing */
    { ClassName( NULL, CLASS_DEFINITION ); }
    ;

packed-class-opt
    : /* nothing */
    { $$ = CLINIT_NULL; }
    | Y__PACKED
    { $$ = CLINIT_PACKED; }
    ;

class-mod-opt
    : /* nothing */
    { $$ = NULL; }
    | class-mod-seq
    ;

class-mod
    : pragma-modifier
    | modifier
    { $$ = MakeFlagModifier( $1 ); }
    | ms-specific-declspec
    { $$ = MakeClassModDeclSpec( $1 ); }
    ;

class-mod-seq
    : class-mod
    | class-mod-seq class-mod
    { $$ = TypeClassModCombine( $1, $2 ); }
    ;

member-specification-opt
    : /* nothing */
    | member-specification
    ;

member-specification
    : member-specification-opt member-declaration
    | member-specification-opt access-specifier Y_COLON
    { ClassPermission( $2 ); }
    ;

member-declaration
    : simple-member-declaration-before-semicolon Y_SEMI_COLON
    | function-definition
    | access-declaration Y_SEMI_COLON
    | static_assert-declaration Y_SEMI_COLON
    | template-declaration
    | Y_SEMI_COLON
    {
        SetErrLoc( &yylp[1] );
        CErr1( WARN_EMPTY_MEMBER_DECL );
    }
    ;

simple-member-declaration-before-semicolon
    : simple-member-declaration
    {
        if( t != Y_SEMI_COLON ) {
            SetErrLoc( &yylocation );
            CErr1( ERR_SYNTAX_MISSING_SEMICOLON );
            what = P_DIAGNOSED;
        }
    }
    ;

simple-member-declaration
    : decl-specifier-seq member-declarator-list
    {
        GStackPop( &(state->gstack) );
    }
    |                 member-declarator-list
    {
    }
    | decl-specifier-seq
    {
        $1 = CheckNoDeclarator( $1 );
        if( $1 == NULL ) {
            what = P_SYNTAX;
        }
        GStackPop( &(state->gstack) );
    }
    | decl-specifier-seq ctor-declarator
    {
        InsertDeclInfo( GetCurrScope(), $2 );
        FreeDeclInfo( $2 );
        GStackPop( &(state->gstack) );
    }
    | using-declaration
    ;

member-declarator-list
    : member-declarator
    {
        VerifyMemberFunction( state->gstack->u.dspec, $1 );
    }
    | member-declarator-list Y_COMMA member-declarator
    {
        VerifyMemberFunction( state->gstack->u.dspec, $3 );
    }
    ;

member-declarator
    : member-declaring-declarator constant-initializer
    {
        $$ = $1;
        $$->sym->flag |= SF_IN_CLASS_INIT;
        DataInitSimple( $2 );
        GStackPop( &(state->gstack) );
    }
    | member-declaring-declarator Y_PURE_FUNCTION_SPECIAL Y_CONSTANT
    {
        if( $3->op != PT_INT_CONSTANT || $3->u.int_constant != 0 ) {
            CErr1( ERR_MUST_BE_ZERO );
        }
        PTreeFree( $3 );
        $$ = $1;
    }
    | member-declaring-declarator
    {
        $$ = $1;
        DeclNoInit( $$ );
    }
    |         Y_COLON constant-expression
    {
        ClassBitfield( state->gstack->u.dspec, NULL, $2 );
        $$ = NULL;
    }
    | declarator-id Y_COLON constant-expression
    {
        ClassBitfield( state->gstack->u.dspec, $1, $3 );
        $$ = NULL;
    }
    ;

member-declaring-declarator
    : declarator
    {
        if( t == Y_EQUAL ) {
            if( VerifyPureFunction( $1 ) ) {
                t = Y_PURE_FUNCTION_SPECIAL;
            }
        }

        $$ = InsertDeclInfo( GetCurrScope(), $1 );

        if( t == Y_EQUAL ) {
            if( ! SymIsStaticMember( $$->sym ) || ! SymIsConstant( $$->sym ) ) {
                CErr1( ERR_MUST_BE_CONST_STATIC_INTEGRAL );
            }

            GStackPush( &(state->gstack), GS_DECL_INFO );
            state->gstack->u.dinfo = $$;

            reuseGStack( state, GS_INIT_DATA );
            DataInitStart( &(state->gstack->u.initdata), $$ );
            DataInitSimpleLocn( &yylp[1] );
        }
    }
    ;

constant-initializer
    : Y_EQUAL constant-expression
    { $$ = $2; }
    ;


/* A.9 Derived classes [gram.derived] */

base-clause
    : Y_COLON base-specifier-list
    { ClassBaseClause( $2 ); }
    ;

base-specifier-list
    : base-specifier
    { $$ = ClassBaseList( NULL, $1 ); }
    | base-specifier-list Y_COMMA base-specifier
    { $$ = ClassBaseList( $1, $3 ); }
    ;

base-specifier
    : base-qualifiers-opt qualified-class-specifier
    { $$ = ClassBaseSpecifier( $1, $2 ); }
    ;

base-qualifiers-opt
    : /* nothing */
    { $$ = IN_NULL; }
    | base-qualifiers-opt Y_PRIVATE
    { $$ = ClassBaseQualifiers( $1, IN_PRIVATE ); }
    | base-qualifiers-opt Y_PUBLIC
    { $$ = ClassBaseQualifiers( $1, IN_PUBLIC ); }
    | base-qualifiers-opt Y_PROTECTED
    { $$ = ClassBaseQualifiers( $1, IN_PROTECTED ); }
    | base-qualifiers-opt Y_VIRTUAL
    { $$ = ClassBaseQualifiers( $1, IN_VIRTUAL ); }
    ;

access-specifier
    : Y_PRIVATE
    { $$ = SF_PRIVATE; }
    | Y_PROTECTED
    { $$ = SF_PROTECTED; }
    | Y_PUBLIC
    { $$ = SF_NULL; }
    ;


/* A.10 Special member functions [gram.special] */

conversion-function-id
    : Y_OPERATOR conversion-type-id
    { $$ = setLocation( $2, &yylp[1] ); }
    ;

scoped-conversion-function-id
    : Y_SCOPED_OPERATOR conversion-type-id
    {
        $$ = setLocation( MakeScopedUserConversionId( $1, $2 ), &yylp[1] );
        ScopeQualifyPop();
    }
    | Y_GLOBAL_OPERATOR conversion-type-id
    { $$ = setLocation( $2, &yylp[1] ); }
    ;

template-scoped-conversion-function-id
    : Y_TEMPLATE_SCOPED_OPERATOR conversion-type-id
    {
        $$ = setLocation( MakeScopedUserConversionId( $1, $2 ), &yylp[1] );
        ScopeQualifyPop();
    }
    ;

conversion-type-id
    : type-specifier-seq
    {
        $$ = MakeUserConversionId( $1, NULL );
        GStackPop( &(state->gstack) );
    }
    | type-specifier-seq conversion-declarator
    {
        $$ = MakeUserConversionId( $1, $2 );
        GStackPop( &(state->gstack) );
    }
    ;

conversion-declarator
    : ptr-operator
    { $$ = MakeAbstractDeclarator( $1 ); }
    | ptr-operator conversion-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    ;

ctor-initializer
    : Y_COLON
    {
        if( state->template_record_tokens != NULL ) {
            PTreeFreeSubtrees( getMultiToken() );
            recordTemplateCtorInitializer( state );
            $$ = NULL;
        } else {
            $$ = RewritePackageMemInit( getMultiToken() );
        }
        what = P_RELEX;
    }
    ;

mem-initializer-list
    : mem-initializer
    { $$ = PTreeBinary( CO_LIST, NULL, $1 ); }
    | mem-initializer-list Y_COMMA mem-initializer
    { $$ = PTreeBinary( CO_LIST,   $1, $3 ); }
    ;

mem-initializer
    : qualified-class-specifier Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
    { $$ = MakeMemInitItem( $1, NULL, $3, &yylp[2] ); }
    | identifier Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
    { $$ = MakeMemInitItem( NULL, $1, $3, &yylp[2] ); }
    ;

/* non-standard */
qualified-class-specifier
    : qualified-class-type
    {
        if( $$ == NULL ) {
            what = P_DIAGNOSED;
        }
    }
    ;

/* non-standard */
qualified-class-type
    : Y_TYPE_NAME
    { $$ = sendClass( $1 ); }
    | Y_GLOBAL_TYPE_NAME
    { $$ = sendClass( $1 ); }
    | Y_SCOPED_TYPE_NAME
    { $$ = sendClass( $1 ); }
    | nested-name-specifier
    {
        $$ = PTypeClassInstantiation( state->class_colon, $1 );
        if( StructType( $$->partial ) == NULL ) {
            CErr2p( ERR_EXPECTED_CLASS_TYPE, $$->partial );
            PTypeRelease( $$ );
            $$ = NULL;
        }
    }
    | nested-name-specifier Y_TEMPLATE_SCOPED_TYPE_NAME
    {
        PTreeFreeSubtrees( $1 );
        $$ = sendClass( $2 );
    }
    ;


/* A.11 Overloading [gram.over] */

/* TODO */
operator-function-id
    : Y_OPERATOR operator
    { $$ = setLocation( MakeOperatorId( $2 ), &yylp[1] ); }
    ;

scoped-operator-function-id
    : Y_SCOPED_OPERATOR operator
    {
        $$ = setLocation( MakeScopedOperatorId( $1, $2 ), &yylp[1] );
        ScopeQualifyPop();
    }
    | Y_GLOBAL_OPERATOR operator
    {
        $$ = setLocation( MakeGlobalOperatorId( $1, $2 ), &yylp[1] );
        ScopeQualifyPop();
    }
    ;

template-scoped-operator-function-id
    : Y_TEMPLATE_SCOPED_OPERATOR operator
    {
        $$ = setLocation( MakeScopedOperatorId( $1, $2 ), &yylp[1] );
        ScopeQualifyPop();
    }
    ;

operator
    : Y_NEW
    { $$ = CO_NEW; }
    | Y_DELETE
    { $$ = CO_DELETE; }
    | Y_NEW Y_LEFT_BRACKET Y_RIGHT_BRACKET
    { $$ = CO_NEW_ARRAY; }
    | Y_DELETE Y_LEFT_BRACKET Y_RIGHT_BRACKET
    { $$ = CO_DELETE_ARRAY; }
    | Y_PLUS
    { $$ = CO_PLUS; }
    | Y_MINUS
    { $$ = CO_MINUS; }
    | Y_TIMES
    { $$ = CO_TIMES; }
    | Y_DIV
    { $$ = CO_DIVIDE; }
    | Y_PERCENT
    { $$ = CO_PERCENT; }
    | Y_XOR
    { $$ = CO_XOR; }
    | Y_AND
    { $$ = CO_AND; }
    | Y_OR
    { $$ = CO_OR; }
    | Y_TILDE
    { $$ = CO_TILDE; }
    | Y_EXCLAMATION
    { $$ = CO_EXCLAMATION; }
    | Y_EQUAL
    { $$ = CO_EQUAL; }
    | Y_TIMES_EQUAL
    { $$ = CO_TIMES_EQUAL; }
    | Y_DIV_EQUAL
    { $$ = CO_DIVIDE_EQUAL; }
    | Y_PERCENT_EQUAL
    { $$ = CO_PERCENT_EQUAL; }
    | Y_PLUS_EQUAL
    { $$ = CO_PLUS_EQUAL; }
    | Y_MINUS_EQUAL
    { $$ = CO_MINUS_EQUAL; }
    | Y_LSHIFT_EQUAL
    { $$ = CO_LSHIFT_EQUAL; }
    | Y_RSHIFT_EQUAL
    { $$ = CO_RSHIFT_EQUAL; }
    | Y_OR_EQUAL
    { $$ = CO_OR_EQUAL; }
    | Y_AND_EQUAL
    { $$ = CO_AND_EQUAL; }
    | Y_XOR_EQUAL
    { $$ = CO_XOR_EQUAL; }
    | Y_LSHIFT
    { $$ = CO_LSHIFT; }
    | Y_RSHIFT
    { $$ = CO_RSHIFT; }
    | Y_EQ
    { $$ = CO_EQ; }
    | Y_NE
    { $$ = CO_NE; }
    | Y_LE
    { $$ = CO_LE; }
    | Y_GE
    { $$ = CO_GE; }
    | Y_LT
    { $$ = CO_LT; }
    | Y_GT
    { $$ = CO_GT; }
    | Y_AND_AND
    { $$ = CO_AND_AND; }
    | Y_OR_OR
    { $$ = CO_OR_OR; }
    | Y_PLUS_PLUS
    { $$ = CO_PRE_PLUS_PLUS; }
    | Y_MINUS_MINUS
    { $$ = CO_PRE_MINUS_MINUS; }
    | Y_COMMA
    { $$ = CO_COMMA; }
    | Y_ARROW_STAR
    { $$ = CO_ARROW_STAR; }
    | Y_ARROW
    { $$ = CO_ARROW; }
    | Y_LEFT_PAREN Y_RIGHT_PAREN
    { $$ = CO_CALL; }
    | Y_LEFT_BRACKET Y_RIGHT_BRACKET
    { $$ = CO_INDEX; }
    | Y_SEG_OP
    { $$ = CO_SEG_OP; }
    ;


/* A.12 Templates [gram.temp] */

/* differs from standard */
template-declaration
    : template-declaration-before-semicolon Y_SEMI_COLON
    | template-function-declaration
    ;

/* differs from standard */
template-declaration-start
    : template-key template-declaration-init lt-special template-opt-parameter-list Y_GT_SPECIAL
    ;

/* differs from standard */
simple-template-declaration-before-semicolon
    : block-declaration-before-semicolon
    | simple-member-declaration-before-semicolon
    ;

template-declaration-before-semicolon
    : template-declaration-start simple-template-declaration-before-semicolon
    {
        RewriteFree( ParseGetRecordingInProgress( NULL ) );
        state->template_decl = FALSE;
        GStackPop( &(state->gstack) ); /* GS_DECL_SPEC */
        GStackPop( &(state->gstack) ); /* GS_TEMPLATE_DATA */
    }
    | Y_EXPORT template-declaration-start simple-template-declaration-before-semicolon
    {
        CErr1( WARN_UNSUPPORTED_TEMPLATE_EXPORT );
        RewriteFree( ParseGetRecordingInProgress( NULL ) );
        state->template_decl = FALSE;
        GStackPop( &(state->gstack) ); /* GS_DECL_SPEC */
        GStackPop( &(state->gstack) ); /* GS_TEMPLATE_DATA */
    }
    ;

template-function-declaration
    : template-declaration-start function-definition
    {
        RewriteFree( ParseGetRecordingInProgress( NULL ) );
        state->template_decl = FALSE;
        GStackPop( &(state->gstack) ); /* GS_DECL_SPEC */
        GStackPop( &(state->gstack) ); /* GS_TEMPLATE_DATA */
    }
    | Y_EXPORT template-declaration-start function-definition
    {
        CErr1( WARN_UNSUPPORTED_TEMPLATE_EXPORT );
        RewriteFree( ParseGetRecordingInProgress( NULL ) );
        state->template_decl = FALSE;
        GStackPop( &(state->gstack) ); /* GS_DECL_SPEC */
        GStackPop( &(state->gstack) ); /* GS_TEMPLATE_DATA */
    }
    ;


/* non standard */
template-declaration-init
    : /* nothing */
    {
        GStackPush( &(state->gstack), GS_TEMPLATE_DATA );
        TemplateDeclInit( &(state->gstack->u.templatedata) );
    }
    ;

/* non standard */
template-opt-parameter-list
    : /* nothing */
    {
        pushDefaultDeclSpec( state );
        state->template_record_tokens =
            RewriteRecordInit( &(state->template_record_locn) );
    }
    | template-parameter-list
    {
        pushDefaultDeclSpec( state );
        state->template_record_tokens =
            RewriteRecordInit( &(state->template_record_locn) );
    }
    ;


template-parameter-list
    : template-parameter
    {
        TemplateDeclAddArgument( $1 );
    }
    | template-parameter-list Y_COMMA template-parameter
    {
        TemplateDeclAddArgument( $3 );
    }
    ;

template-parameter
    : type-parameter
    | parameter-declaration
    ;

type-parameter
    : type-parameter-no-defarg
    | type-parameter-no-defarg type-defarg-parse-or-copy
    ;

/* non standard */
type-parameter-no-defarg
    : template-typename-key
    {
        DECL_SPEC *dspec;

        pushClassData( state, TF1_NULL, CLINIT_NULL, NULL);
        ClassName( NULL, CLASS_GENERIC );
        dspec = ClassRefDef();
        GStackPop( &(state->gstack) );
        $$ = DeclSpecDeclarator( dspec );
        PTypeRelease( dspec );
    }
    | template-typename-key make-id
    {
        DECL_SPEC *dspec;

        pushClassData( state, TF1_NULL, CLINIT_NULL, NULL);
        ClassName( $2, CLASS_GENERIC );
        dspec = ClassRefDef();
        GStackPop( &(state->gstack) );
        $$ = DeclSpecDeclarator( dspec );
        PTypeRelease( dspec );
    }
    ;

/* non standard */
template-typename-key
    : Y_TYPENAME
    { }
    | Y_CLASS
    { }
    ;

template-id
    : Y_TEMPLATE_ID lt-special template-argument-list-opt Y_GT_SPECIAL
    {
        $3 = NodeReverseArgs( NULL, $3 );
        $3->flags |= PTF_ALREADY_ANALYSED;
        $$ = PTreeBinary( CO_TEMPLATE, $1, $3 );
    }
    ;

scoped-template-id
    : Y_SCOPED_TEMPLATE_ID lt-special template-argument-list-opt Y_GT_SPECIAL
    {
        $3 = NodeReverseArgs( NULL, $3 );
        $3->flags |= PTF_ALREADY_ANALYSED;
        $$ = PTreeBinary( CO_TEMPLATE, MakeScopedId( $1 ), $3 );
    }
    | Y_GLOBAL_TEMPLATE_ID lt-special template-argument-list-opt Y_GT_SPECIAL
    {
        $3 = NodeReverseArgs( NULL, $3 );
        $3->flags |= PTF_ALREADY_ANALYSED;
        $$ = PTreeBinary( CO_TEMPLATE, MakeScopedId( $1 ), $3 );
    }
    ;

template-argument-list
    : template-argument
    { $$ = PTreeBinary( CO_LIST, NULL, $1 ); }
    | template-argument-list Y_COMMA template-argument
    { $$ = setLocation( PTreeBinary( CO_LIST, $1, $3 ), &yylp[2] ); }
    ;

template-argument
    : assignment-expression
    {
        $1 = AnalyseRawExpr( $1 );
        if( $1->op == PT_SYMBOL ) {
            SYMBOL sym = $1->u.symcg.symbol;
            if( SymIsConstantInt( sym ) ) {
                PTreeFreeSubtrees( $1 );
                $1 = PTreeIntConstant( sym->u.sval, TYP_SINT );
            }
        }
        $$ = $1;
    }
    | type-id
    ;

/* TODO */
typename-specifier
    : typename-special nested-name-specifier Y_TEMPLATE_SCOPED_TYPE_NAME
    {
        $$ = sendType( $3 );
        PTreeFreeSubtrees( $2 );
    }
    | typename-special nested-name-specifier
    {
        $$ = sendType( $2 );
    }
    | typename-special Y_SCOPED_TYPE_NAME
    {
        $$ = sendType( $2 );
    }
    ;

/* non-standard */
explicit-instantiation-special
    : Y_TEMPLATE
    {
        state->template_instantiate = TRUE;
    }
    | Y_EXTERN Y_TEMPLATE
    {
        state->template_extern = TRUE;
    }
    ;

explicit-instantiation
    : Y_EXTERN Y_TEMPLATE template-class-directive-extern Y_SEMI_COLON
    {
        CErr1( WARN_MISSING_KEYWORD_IN_EXPLICT_INSTANTIATION );
    }
    | Y_TEMPLATE template-class-directive-instantiate Y_SEMI_COLON
    {
        CErr1( WARN_MISSING_KEYWORD_IN_EXPLICT_INSTANTIATION );
    }
    | explicit-instantiation-special block-declaration-before-semicolon Y_SEMI_COLON
    {
        state->template_instantiate = FALSE;
        state->template_extern = FALSE;
    }
    ;

template-key
    : Y_TEMPLATE
    {
        if( state->template_decl ) {
            CErr1( ERR_NO_NESTED_TEMPLATES );
        } else if( ! ScopeType( GetCurrScope(), SCOPE_FILE )
                && ! ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
            CErr1( ERR_ONLY_GLOBAL_TEMPLATES );
        }
        state->template_decl = TRUE;
    }
    ;

template-argument-list-opt
    : /* nothing */
    { $$ = PTreeBinary( CO_LIST, NULL, NULL ); }
    | template-argument-list
    { $$ = $1; }
    ;

template-class-directive-extern
    : nested-name-specifier
    {
        TYPE type = NodeIsBinaryOp( $1, CO_STORAGE ) ?
            $1->u.subtree[1]->type : $1->type;

        TemplateClassDirective( type, &($1->locn), TCD_EXTERN ); 
        NodeFreeDupedExpr( $1 );
    }
    ;

template-class-directive-instantiate
    : nested-name-specifier
    {
        TYPE type = NodeIsBinaryOp( $1, CO_STORAGE ) ?
            $1->u.subtree[1]->type : $1->type;

        TemplateClassDirective( type, &($1->locn), TCD_INSTANTIATE ); 
        NodeFreeDupedExpr( $1 );
    }
    ;

template-type
    : template-type-instantiation Y_GT_SPECIAL
    ;

template-type-instantiation
    : Y_TEMPLATE_NAME lt-special template-argument-list-opt
    {
        TYPE inst_type;

        inst_type = TemplateClassReference( $1, $3 );
        setWatchColonColon( state, $1, inst_type );
        $$ = $1;

        if( inst_type != NULL ) {
            $$->type = inst_type;
        }
    }
    ;

scoped-template-type
    : scoped-template-type-instantiation Y_GT_SPECIAL
    ;

scoped-template-type-instantiation
    : Y_SCOPED_TEMPLATE_NAME lt-special template-argument-list-opt
    {
        TYPE inst_type;

        inst_type = TemplateClassReference( $1, $3 );
        setWatchColonColon( state, $1, inst_type );
        $$ = $1;

        if( inst_type == NULL ) {
            DbgAssert( ( $$->op == PT_BINARY )
                    && ( $$->cgop == CO_STORAGE ) );
            $$->u.subtree[1] =
                PTreeBinary( CO_TEMPLATE, $$->u.subtree[1], $3 );
        } else {
            $$->u.subtree[1]->type = inst_type;
        }
    }
    | Y_GLOBAL_TEMPLATE_NAME lt-special template-argument-list-opt
    {
        TYPE inst_type;

        inst_type = TemplateClassReference( $1, $3 );
        setWatchColonColon( state, $1, inst_type );
        $$ = $1;

        if( inst_type == NULL ) {
            DbgAssert( ( $$->op == PT_BINARY )
                    && ( $$->cgop == CO_STORAGE ) );
            $$->u.subtree[1] =
                PTreeBinary( CO_TEMPLATE, $$->u.subtree[1], $3 );
        } else {
            $$->u.subtree[1]->type = inst_type;
        }
    }
    ;

template-scoped-template-type
    : template-scoped-template-type-instantiation Y_GT_SPECIAL
    ;

template-scoped-template-type-instantiation
    : Y_TEMPLATE_SCOPED_TEMPLATE_NAME lt-special template-argument-list-opt
    {
        TYPE inst_type;

        inst_type = TemplateClassReference( $1, $3 );
        setWatchColonColon( state, $1, inst_type );
        $$ = $1;

        if( inst_type == NULL ) {
            DbgAssert( ( $$->op == PT_BINARY )
                    && ( $$->cgop == CO_STORAGE ) );
            $$->u.subtree[1] =
                PTreeBinary( CO_TEMPLATE, $$->u.subtree[1], $3 );
        } else {
            $$->u.subtree[1]->type = inst_type;
        }
    }
    ;


/* A.13 Exception handling [gram.except] */

exception-declaration
    : actual-exception-declaration
    {
        $$ = $1;
        GStackPop( &(state->gstack) );
    }
    ;

actual-exception-declaration
    : type-specifier-seq declarator
    { $$ = $2; }
    | type-specifier-seq abstract-declarator
    { $$ = $2; }
    | type-specifier-seq
    { $$ = DeclSpecDeclarator( state->gstack->u.dspec ); }
    ;

exception-specification-opt
    : /* nothing */
    { $$ = ThrowsAnything(); }
    | exception-specification
    ;

exception-specification
    : Y_THROW Y_LEFT_PAREN type-id-list Y_RIGHT_PAREN
    { $$ = $3; }
    | Y_THROW Y_LEFT_PAREN              Y_RIGHT_PAREN
    { $$ = NULL; }
    ;

type-id-list
    : expect-type-id type-id
    { $$ = $2; }
    | type-id-list Y_COMMA expect-type-id type-id
    { $$ = PTreeTListAppend( $1, $4 ); }
    ;



/*** declaration syntax ***/
might-restart-declarations
    : start-restart-declarations
    { popRestartDecl( state ); }
    | start-restart-declarations declaration-seq
    { popRestartDecl( state ); }
    ;

start-restart-declarations
    : /* nothing */
    { pushRestartDecl( state ); }
    ;

local-declaration
    : block-declaration-before-semicolon Y_SEMI_COLON
    ;

no-declarator-declaration
    : decl-specifier-seq
    {
        $$ = CheckNoDeclarator( $1 );
        if( $$ == NULL ) {
            what = P_SYNTAX;
        }
    }
    ;

/* extension */
typeof-specifier
    : Y___TYPEOF Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = PTypeExpr( $3 ); }
    | Y___TYPEOF Y_LEFT_PAREN type-id Y_RIGHT_PAREN
    { $$ = PTypeTypeid( $3 ); }
    ;

cv-qualifier
    : Y_CONST
    { $$ = PTypeSpecifier( STY_CONST ); }
    | Y_VOLATILE
    { $$ = PTypeSpecifier( STY_VOLATILE ); }
    | Y___UNALIGNED
    { $$ = PTypeSpecifier( STY_UNALIGNED ); }
    ;

/* non-standard */
basic-type-specifier
    : Y_CHAR
    { $$ = PTypeScalar( STM_CHAR ); }
    | Y_WCHAR_T
    { $$ = PTypeScalar( STM_WCHAR ); }
    | Y_BOOL
    { $$ = PTypeScalar( STM_BOOL ); }
    | Y_SHORT
    { $$ = PTypeScalar( STM_SHORT ); }
    | Y_INT
    { $$ = PTypeScalar( STM_INT ); }
    | Y_LONG
    { $$ = PTypeScalar( STM_LONG ); }
    | Y_SIGNED
    { $$ = PTypeScalar( STM_SIGNED ); }
    | Y_UNSIGNED
    { $$ = PTypeScalar( STM_UNSIGNED ); }
    | Y_FLOAT
    { $$ = PTypeScalar( STM_FLOAT ); }
    | Y_DOUBLE
    { $$ = PTypeScalar( STM_DOUBLE ); }
    | Y_VOID
    { $$ = PTypeScalar( STM_VOID ); }
    /* extension */
    | Y___SEGMENT
    { $$ = PTypeScalar( STM_SEGMENT ); }
    | Y___INT8
    { $$ = PTypeScalar( STM_CHAR ); }
    | Y___INT16
    { $$ = PTypeScalar( STM_SHORT ); }
    | Y___INT32
    { $$ = PTypeScalar( STM_INT32 ); }
    | Y___INT64
    { $$ = PTypeScalar( STM_INT64 ); }
    ;

elaborated-type-name
    : Y_GLOBAL_ID
    | Y_GLOBAL_TEMPLATE_ID
    | Y_GLOBAL_TYPE_NAME
    | Y_SCOPED_ID
    | Y_SCOPED_TEMPLATE_ID
    | Y_SCOPED_TYPE_NAME
    ;


braced-initializer
    : brace-start initializer-list Y_COMMA Y_RIGHT_BRACE
    { DataInitPop(); }
    | brace-start initializer-list         Y_RIGHT_BRACE
    { DataInitPop(); }
    ;

brace-start
    : Y_LEFT_BRACE
    { DataInitPush(); }
    ;




typename-special
    : typename-special-init Y_TYPENAME
    {
        if( ! ScopeType( GetCurrScope(), SCOPE_TEMPLATE_DECL )
         && ! IsTemplateInstantiationActive() ) {
            SetErrLoc( &yylp[2] );
            CErr1( ERR_TYPENAME_OUTSIDE_TEMPLATE );
        }
    }
    ;

typename-special-init
    : /* nothing */
    {
        state->special_typename = TRUE;
    }
    ;

special-new-type-id
    : type-specifier-seq special-new-abstract-declarator
    {
        $$ = $2;
        GStackPop( &(state->gstack) );
    }
    | type-specifier-seq
    {
        $$ = MakeNewTypeId( DoDeclSpec( state->gstack->u.dspec ) );
        GStackPop( &(state->gstack) );
    }
    ;

special-new-abstract-declarator
    : special-new-abstract-ptr-mod-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    | special-new-direct-abstract-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    ;

special-new-abstract-ptr-mod-declarator
    : ptr-mod special-new-abstract-ptr-mod-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod special-new-direct-abstract-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod
    { $$ = MakeAbstractDeclarator( $1 ); }
    ;

special-new-direct-abstract-declarator
    : special-new-direct-abstract-declarator Y_LEFT_PAREN parameter-declaration-clause Y_RIGHT_PAREN cv-qualifier-seq-opt exception-specification-opt
    {
        $$ = AddDeclarator( $1, MakeFnType( &($3), $5, $6 ) );
        FreeArgs( $3 );
    }
    | Y_LEFT_PAREN parameter-declaration-clause Y_RIGHT_PAREN cv-qualifier-seq-opt exception-specification-opt
    {
        $$ = MakeAbstractDeclarator( MakeFnType( &($2), $4, $5 ) );
        FreeArgs( $2 );
    }
    | special-new-direct-abstract-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    |                            Y_LEFT_BRACKET expression Y_RIGHT_BRACKET
    { $$ = MakeNewDynamicArray( $2 ); }
    | special-new-direct-abstract-declarator Y_LEFT_BRACKET Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, NULL ); }
    |                            Y_LEFT_BRACKET Y_RIGHT_BRACKET
    {
        $$ = MakeAbstractDeclarator( NULL );
        $$ = AddArrayDeclarator( $$, NULL );
    }
    | Y_LEFT_PAREN special-new-direct-abstract-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    | Y_LEFT_PAREN special-new-abstract-ptr-mod-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    ;


%%

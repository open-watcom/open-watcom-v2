/*
**********************************************************************
*       Copyright by WATCOM International Corporation, 1987, 1992.   *
*       All rights reserved. No part of this software may be         *
*       reproduced in any form or by any means - graphic, electronic,*
*       mechanical or otherwise, including, without limitation,      *
*       photocopying, recording, taping or information storage and   *
*       retrieval systems - except with the written permission of    *
*       WATCOM International Corporation.                            *
**********************************************************************

PLUSPLUS.Y : description of C++ syntax

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
93/03/16	A.F.Scian	streamlined member-declarator to directly call
				DeclNoInit instead of going through DataInitNoInit
93/03/29	A.F.Scian	added semantics for "new (int [x+1])" 5.3.3
				"The first array dimension can be a general
				 integral expression even when <type-id> is used"
93/04/20	A.F.Scian	added rules for y_template_scoped_typename to
				handle nested types inside of templates
93/04/21	A.F.Scian	fixed actions for function-declaration to not
				pop the GS_DECL_SPEC stack until the function
				body is processed (this allows any linkage on
				the function symbol to be retained for the
				entire processing of the function body)
93/07/13	A.F.Scian	- added more than one pointer declarator to
				  conversion function ids
				- fixed concatenation of strings so that the
				  type of the result is correct
93/07/29	A.F.Scian	it was impossible to tell the difference between
				no parms and ( void ) (both had parms == NULL)
				so code now calls AddExplicitParms which handles
				the problem by setting a bit in the DECL_INFO
93/08/11	A.F.Scian	allow multiple ;'s in member decl lists
				e.g., struct S { int a;;;; void foo(){};;; };
93/12/01	A.F.Scian	added %ambig directives supported by new YACC
93/12/15	A.F.Scian	added operator new []/operator delete [] support
94/03/23	A.F.Scian	set up so that class S<int>; doesn't define the
				class
94/04/18	A.F.Scian	added support for "class __export __stdcall X"
94/04/19	A.F.Scian	push some calls to FreeDeclInfo into functions
				that are called before
94/06/08	A.F.Scian	improved error message location for missing ';'
				error messages
94/10/14	A.F.Scian	added __declspec( <id> )
94/10/18	A.F.Scian	added kludge to support:
				typedef struct S { } S, const *CPS;
							^^^^^- accept this
94/10/25	A.F.Scian	upgrade LHS of assignment-expr so that it
				accepts casts and pm-exprs
94/11/02	A.F.Scian	added support for "class __declspec(dllimport) X"
94/12/07	A.F.Scian	added Y_GLOBAL_ID as a valid declarator-id so
				struct S { friend int ::foo( int ); }; works
95/02/23	A.F.Scian	added __builtin_isfloat( <type-id> ) construct
95/04/28	A.F.Scian	added __unaligned modifier
95/05/16	A.F.Scian	added class template directives
95/06/28	A.F.Scian	added more general offsetof field expressions
96/01/10	A.F.Scian	adjusted syntax of __unaligned to match 'const'
				rather than 'far'
96/02/12	A.F.Scian	added support for __declspec( pragma-modifier )
96/02/26	A.F.Scian	added some namespace syntax
96/03/19	A.F.Scian	adjusted ++@/--@ grammar as per ISO C++ mtg in
				Santa Cruz, CA resolutions
96/05/07	A.F.Scian	added "using namespace X;"
96/06/07	A.F.Scian	added support for multiple ids in __declspec()
96/07/02	A.F.Scian	added sizeof <type-name>
96/07/25	A.F.Scian	accept unsigned <type-name> : 1; as a bitfield
96/10/28	A.F.Scian	fixed 96/07/25 so that <type-name> : 1; still works
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
%token Y_STATIC_CAST
%token Y_STRUCT
%token Y_TEMPLATE
%token Y_THIS
%token Y_THROW
%token Y_TRUE
%token Y_TYPEDEF
%token Y_TYPEID
%token Y_UNION
%token Y_UNSIGNED
%token Y_USING
%token Y_VIRTUAL
%token Y_VOID
%token Y_VOLATILE

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
%token Y_DIVIDE
%token Y_DIVIDE_EQUAL
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
%token <tree> Y_TYPE_NAME
%token <tree> Y_TEMPLATE_NAME
%token <tree> Y_NAMESPACE_NAME
%token <tree> Y_CONSTANT
%token <tree> Y_TRUE
%token <tree> Y_FALSE
%token <tree> Y_STRING

%token <tree> Y_GLOBAL_ID               /* ::<id> */
%token <tree> Y_GLOBAL_TYPE_NAME        /* ::<type-name> */
%token <tree> Y_GLOBAL_TEMPLATE_NAME    /* ::<template-name> */
%token <tree> Y_GLOBAL_NAMESPACE_NAME   /* ::<namespace-name> */
%token <tree> Y_GLOBAL_OPERATOR         /* ::operator */
%token <tree> Y_GLOBAL_TILDE            /* ::~ */
%token <tree> Y_GLOBAL_NEW              /* ::new */
%token <tree> Y_GLOBAL_DELETE           /* ::delete */

/* Y_SCOPED_* tokens must stay in line with Y_TEMPLATE_SCOPED_* tokens */
%token <tree> Y_SCOPED_ID               /* C::<id> */
%token <tree> Y_SCOPED_TYPE_NAME        /* C::<type-name> */
%token <tree> Y_SCOPED_TEMPLATE_NAME    /* C::<template-name> */
%token <tree> Y_SCOPED_NAMESPACE_NAME   /* C::<namespace-name> */
%token <tree> Y_SCOPED_OPERATOR         /* C::operator */
%token <tree> Y_SCOPED_TILDE            /* C::~ */
%token <tree> Y_SCOPED_TIMES            /* C::* */

%token <tree> Y_TEMPLATE_SCOPED_ID      /* T<>::<id> */
%token <tree> Y_TEMPLATE_SCOPED_TYPE_NAME/* T<>::<type-name> */
%token <tree> Y_TEMPLATE_SCOPED_TEMPLATE_NAME/* T<>::<template-name> */
%token <tree> Y_TEMPLATE_SCOPED_NAMESPACE_NAME/* T<>::<namespace-name> */
%token <tree> Y_TEMPLATE_SCOPED_OPERATOR/* T<>::operator */
%token <tree> Y_TEMPLATE_SCOPED_TILDE   /* T<>::~ */
%token <tree> Y_TEMPLATE_SCOPED_TIMES   /* T<>::* */

/*** leader token for "special" parsing ***/
%token Y_EXPRESSION_SPECIAL
%token Y_EXPR_DECL_SPECIAL
%token Y_EXCEPTION_SPECIAL
%token Y_MEM_INIT_SPECIAL
%token Y_DEFARG_SPECIAL
%token Y_CLASS_INST_SPECIAL

/*** terminator tokens for "special" parsing ***/
%token Y_DEFARG_END

/*** tokens that are never generated by yylex() ***/
%token Y_REDUCE_SPECIAL
%token Y_SHIFT_SPECIAL
%token Y_DEFARG_GONE_SPECIAL

/*** special function names ***/
%token Y___OFFSETOF
%token Y___TYPEOF
%token Y___BUILTIN_ISFLOAT

/*** PC-specific keywords ***/
%token Y___ASM
%token Y___BASED
%token Y___CDECL
%token Y___DECLSPEC
%token Y___EXPORT
%token Y___FAR
%token Y___FAR16
%token Y___FORTRAN
%token Y___HUGE
%token Y___INT64
%token Y___INTERRUPT
%token Y___LOADDS
%token Y___NEAR
%token Y__OPTLINK
%token Y__PACKED
%token Y___PASCAL
%token Y___PRAGMA
%token Y___SAVEREGS
%token Y__SEG16
%token Y___SEGMENT
%token Y___SEGNAME
%token Y___SELF
%token Y___STDCALL
%token Y__SYSCALL
%token Y___UNALIGNED

%type <flags> modifier
%type <flags> access-specifier
%type <flags> cv-qualifiers-opt
%type <flags> base-qualifiers-opt
%type <flags> segment-cast-opt
%type <flags> packed-class-opt

%type <token> class-key
%type <token> operator-function-type

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

%type <dspec> type-specifiers
%type <dspec> type-specifier
%type <dspec> typeof-specifier
%type <dspec> arg-decl-specifiers
%type <dspec> decl-specifiers
%type <dspec> non-type-decl-specifiers
%type <dspec> non-type-decl-specifier
%type <dspec> no-declarator-declaration
%type <dspec> basic-type-specifier
%type <dspec> simple-type-specifier
%type <dspec> maybe-type-decl-specifiers
%type <dspec> storage-class-specifier
%type <dspec> ms-specific-declspec
%type <dspec> ms-declspec-seq
%type <dspec> function-specifier
%type <dspec> cv-qualifier
%type <dspec> class-specifier
%type <dspec> enum-specifier
%type <dspec> class-substance
%type <dspec> class-body
%type <dspec> qualified-type-specifier
%type <dspec> qualified-class-specifier
%type <dspec> qualified-class-type
%type <dspec> template-class-id
%type <dspec> template-class-instantiation

%type <dinfo> function-declaration
%type <dinfo> declarator
%type <dinfo> comma-declarator
%type <dinfo> init-declarator
%type <dinfo> comma-init-declarator
%type <dinfo> ctor-declarator
%type <dinfo> declaring-declarator
%type <dinfo> comma-declaring-declarator
%type <dinfo> actual-declarator
%type <dinfo> ptr-mod-init-declarator
%type <dinfo> ptr-mod-declarator
%type <dinfo> conversion-function-ptr-declarator
%type <dinfo> special-new-abstract-ptr-mod-declarator
%type <dinfo> abstract-ptr-mod-declarator
%type <dinfo> special-new-abstract-declarator
%type <dinfo> abstract-declarator
%type <dinfo> special-new-actual-abstract-declarator
%type <dinfo> actual-abstract-declarator
%type <dinfo> abstract-args
%type <dinfo> template-abstract-args
%type <dinfo> simple-arg-declaration
%type <dinfo> simple-template-arg-declaration
%type <dinfo> arg-declaration
%type <dinfo> template-arg-declaration
%type <dinfo> arg-declaration-list
%type <dinfo> template-arg-declaration-list
%type <dinfo> actual-exception-declaration
%type <dinfo> exception-declaration
%type <dinfo> member-declarator
%type <dinfo> actual-new-declarator
%type <dinfo> partial-ptr-declarator
%type <dinfo> new-declarator
%type <dinfo> new-type-id
%type <dinfo> special-new-type-id
%type <dinfo> dynamic-type-id
%type <dinfo> simple-arg-no-id

%type <tree> declarator-id
%type <tree> conversion-function-type
%type <tree> operator-name
%type <tree> destructor-name
%type <tree> except-spec-opt
%type <tree> expression
%type <tree> expression-before-semicolon
%type <tree> expression-list
%type <tree> expression-list-opt
%type <tree> template-assignment-expression
%type <tree> template-assignment-expression-opt
%type <tree> template-conditional-expression
%type <tree> template-logical-or-expression
%type <tree> template-logical-and-expression
%type <tree> template-inclusive-or-expression
%type <tree> template-exclusive-or-expression
%type <tree> template-and-expression
%type <tree> template-equality-expression
%type <tree> template-relational-expression
%type <tree> assignment-tree
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
%type <tree> segment-expression
%type <tree> function-like-cast-expression
%type <tree> qualified-id-expression
%type <tree> member-selection-expression
%type <tree> field-expression
%type <tree> field-name
%type <tree> new-placement
%type <tree> make-id
%type <tree> literal
%type <tree> strings pragma-id
%type <tree> type-id-list
%type <tree> type-id
%type <tree> new-initializer-opt
%type <tree> constant-expression
%type <tree> mem-initializer-list
%type <tree> mem-initializer-item
%type <tree> template-parameter-expression
%type <tree> template-arg-list
%type <tree> template-class-pre-instantiation
%type <tree> template-class-pre-id
%type <tree> template-directive-class
%type <tree> raw-qualified-namespace-specifier

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
    | Y_EXPR_DECL_SPECIAL expr-decl-stmt
    {
        $$ = $2;
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
    | Y_CLASS_INST_SPECIAL class-specifier
    {
        $$ = (PTREE) $2;
        t = YYEOFTOKEN;
    }
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

/*** expression syntax ***/
constant-expression
    : conditional-expression
    { $$ = PTreeNonZeroConstantExpr( $1 ); }
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

expression
    : assignment-expression
    | expression Y_COMMA assignment-expression
    { $$ = setLocation( PTreeBinary( CO_COMMA, $1, $3 ), &yylp[2] ); }
    ;

/*
%type <tree> template-expression
template-expression
    : template-assignment-expression
    | template-expression Y_COMMA template-assignment-expression
    { $$ = setLocation( PTreeBinary( CO_COMMA, $1, $3 ), &yylp[2] ); }
    ;
*/

assignment-tree
    : Y_EQUAL
    { $$ = setLocation( PTreeBinary( CO_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_TIMES_EQUAL
    { $$ = setLocation( PTreeBinary( CO_TIMES_EQUAL, NULL, NULL ), &yylp[1] ); }
    | Y_DIVIDE_EQUAL
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

template-assignment-expression
    : template-conditional-expression
    | template-logical-or-expression assignment-tree template-assignment-expression
    {
        $$ = PTreeReplaceLeft( $2, $1 );
        $$ = PTreeReplaceRight( $$, $3 );
    }
    | Y_THROW template-assignment-expression-opt
    { $$ = setLocation( PTreeUnary( CO_THROW, $2 ), &yylp[1] ); }
    ;

template-assignment-expression-opt
    : /* nothing */
    { $$ = NULL; }
    | template-assignment-expression
    ;

assignment-expression
    : conditional-expression
    | logical-or-expression assignment-tree assignment-expression
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

template-conditional-expression
    : template-logical-or-expression
    | template-logical-or-expression Y_QUESTION expression Y_COLON template-assignment-expression
    {
        $3 = setLocation( PTreeBinary( CO_COLON, $3, $5 ), &yylp[4] );
        $$ = setLocation( PTreeBinary( CO_QUESTION, $1, $3 ), &yylp[2] );
    }
    ;

conditional-expression
    : logical-or-expression
    | logical-or-expression Y_QUESTION expression Y_COLON assignment-expression
    {
        $3 = setLocation( PTreeBinary( CO_COLON, $3, $5 ), &yylp[4] );
        $$ = setLocation( PTreeBinary( CO_QUESTION, $1, $3 ), &yylp[2] );
    }
    ;

template-logical-or-expression
    : template-logical-and-expression
    | template-logical-or-expression Y_OR_OR template-logical-and-expression
    { $$ = setLocation( PTreeBinary( CO_OR_OR, $1, $3 ), &yylp[2] ); }
    ;

logical-or-expression
    : logical-and-expression
    | logical-or-expression Y_OR_OR logical-and-expression
    { $$ = setLocation( PTreeBinary( CO_OR_OR, $1, $3 ), &yylp[2] ); }
    ;

template-logical-and-expression
    : template-inclusive-or-expression
    | template-logical-and-expression Y_AND_AND template-inclusive-or-expression
    { $$ = setLocation( PTreeBinary( CO_AND_AND, $1, $3 ), &yylp[2] ); }
    ;

logical-and-expression
    : inclusive-or-expression
    | logical-and-expression Y_AND_AND inclusive-or-expression
    { $$ = setLocation( PTreeBinary( CO_AND_AND, $1, $3 ), &yylp[2] ); }
    ;

template-inclusive-or-expression
    : template-exclusive-or-expression
    | template-inclusive-or-expression Y_OR template-exclusive-or-expression
    { $$ = setLocation( PTreeBinary( CO_OR, $1, $3 ), &yylp[2] ); }
    ;

inclusive-or-expression
    : exclusive-or-expression
    | inclusive-or-expression Y_OR exclusive-or-expression
    { $$ = setLocation( PTreeBinary( CO_OR, $1, $3 ), &yylp[2] ); }
    ;

template-exclusive-or-expression
    : template-and-expression
    | template-exclusive-or-expression Y_XOR template-and-expression
    { $$ = setLocation( PTreeBinary( CO_XOR, $1, $3 ), &yylp[2] ); }
    ;

exclusive-or-expression
    : and-expression
    | exclusive-or-expression Y_XOR and-expression
    { $$ = setLocation( PTreeBinary( CO_XOR, $1, $3 ), &yylp[2] ); }
    ;

template-and-expression
    : template-equality-expression
    | template-and-expression Y_AND template-equality-expression
    { $$ = setLocation( PTreeBinary( CO_AND, $1, $3 ), &yylp[2] ); }
    ;

and-expression
    : equality-expression
    | and-expression Y_AND equality-expression
    { $$ = setLocation( PTreeBinary( CO_AND, $1, $3 ), &yylp[2] ); }
    ;

template-equality-expression
    : template-relational-expression
    | template-equality-expression Y_EQ template-relational-expression
    { $$ = setLocation( PTreeBinary( CO_EQ, $1, $3 ), &yylp[2] ); }
    | template-equality-expression Y_NE template-relational-expression
    { $$ = setLocation( PTreeBinary( CO_NE, $1, $3 ), &yylp[2] ); }
    ;

equality-expression
    : relational-expression
    | equality-expression Y_EQ relational-expression
    { $$ = setLocation( PTreeBinary( CO_EQ, $1, $3 ), &yylp[2] ); }
    | equality-expression Y_NE relational-expression
    { $$ = setLocation( PTreeBinary( CO_NE, $1, $3 ), &yylp[2] ); }
    ;

template-relational-expression
    : shift-expression
    | template-relational-expression Y_LT shift-expression
    { $$ = setLocation( PTreeBinary( CO_LT, $1, $3 ), &yylp[2] ); }
    | template-relational-expression Y_LE shift-expression
    { $$ = setLocation( PTreeBinary( CO_LE, $1, $3 ), &yylp[2] ); }
    /* A '>' is the end of a template argument list
    | template-relational-expression Y_GT shift-expression
    { $$ = setLocation( PTreeBinary( CO_GT, $1, $3 ), &yylp[2] ); }
    */
    | template-relational-expression Y_GE shift-expression
    { $$ = setLocation( PTreeBinary( CO_GE, $1, $3 ), &yylp[2] ); }
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

shift-expression
    : additive-expression
    | shift-expression Y_RSHIFT additive-expression
    { $$ = setLocation( PTreeBinary( CO_RSHIFT, $1, $3 ), &yylp[2] ); }
    | shift-expression Y_LSHIFT additive-expression
    { $$ = setLocation( PTreeBinary( CO_LSHIFT, $1, $3 ), &yylp[2] ); }
    ;

additive-expression
    : multiplicative-expression
    | additive-expression Y_PLUS multiplicative-expression
    { $$ = setLocation( PTreeBinary( CO_PLUS, $1, $3 ), &yylp[2] ); }
    | additive-expression Y_MINUS multiplicative-expression
    { $$ = setLocation( PTreeBinary( CO_MINUS, $1, $3 ), &yylp[2] ); }
    ;

multiplicative-expression
    : pm-expression
    | multiplicative-expression Y_TIMES pm-expression
    { $$ = setLocation( PTreeBinary( CO_TIMES, $1, $3 ), &yylp[2] ); }
    | multiplicative-expression Y_DIVIDE pm-expression
    { $$ = setLocation( PTreeBinary( CO_DIVIDE, $1, $3 ), &yylp[2] ); }
    | multiplicative-expression Y_PERCENT pm-expression
    { $$ = setLocation( PTreeBinary( CO_PERCENT, $1, $3 ), &yylp[2] ); }
    ;

pm-expression
    : segment-expression
    | pm-expression Y_DOT_STAR segment-expression
    { $$ = setLocation( PTreeBinary( CO_DOT_STAR, $1, $3 ), &yylp[2] ); }
    | pm-expression Y_ARROW_STAR segment-expression
    { $$ = setLocation( PTreeBinary( CO_ARROW_STAR, $1, $3 ), &yylp[2] ); }
    ;

segment-expression
    : cast-expression
    | segment-expression Y_SEG_OP cast-expression
    { $$ = setLocation( PTreeBinary( CO_SEG_OP, $1, $3 ), &yylp[2] ); }
    ;

cast-expression
    : unary-expression
    | Y_LEFT_PAREN type-id Y_RIGHT_PAREN cast-expression
    { $$ = setLocation( MakeNormalCast( $2, $4 ), &yylp[3] ); }
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
    | Y_SIZEOF Y_TYPE_NAME
    { $$ = setLocation( PTreeUnary( CO_SIZEOF_TYPE, PTreeMSSizeofKludge( $2 ) ), &yylp[1] ); }
    | Y___BUILTIN_ISFLOAT Y_LEFT_PAREN type-id Y_RIGHT_PAREN
    { $$ = setLocation( MakeBuiltinIsFloat( $3 ), &yylp[1] ); }
    | Y___OFFSETOF Y_LEFT_PAREN type-id Y_COMMA offsetof-field Y_RIGHT_PAREN
    { $$ = setLocation( PTreeOffsetof( $3, $5 ), &yylp[4] ); }
    | new-expression
    | delete-expression
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

new-expression
    : Y_NEW new-modifier-opt new-placement new-type-id new-initializer-opt
    { $$ = setLocation( MakeNewExpr( NULL, $3, $4, $5 ), &yylp[1] ); }
    | Y_NEW new-modifier-opt               new-type-id new-initializer-opt
    { $$ = setLocation( MakeNewExpr( NULL, NULL, $3, $4 ), &yylp[1] ); }
    | Y_GLOBAL_NEW new-modifier-opt new-placement new-type-id new-initializer-opt
    { $$ = setLocation( MakeNewExpr( $1, $3, $4, $5 ), &yylp[1] ); }
    | Y_GLOBAL_NEW new-modifier-opt               new-type-id new-initializer-opt
    { $$ = setLocation( MakeNewExpr( $1, NULL, $3, $4 ), &yylp[1] ); }
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
    | Y_LEFT_PAREN Y_RIGHT_PAREN
    { $$ = NULL; } /* ARM p.61 '()' is syntactic convenience */
    | Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    { $$ = $2; }
    ;

dynamic-type-id
    : type-specifiers
    {
        $$ = MakeNewTypeId( DoDeclSpec( state->gstack->u.dspec ) );
        GStackPop( &(state->gstack) );
    }
    | type-specifiers new-declarator
    {
        $$ = $2;
        GStackPop( &(state->gstack) );
    }
    ;

new-declarator
    : partial-ptr-declarator actual-new-declarator
    { $$ = MakeNewDeclarator( state->gstack->u.dspec, $1, $2 ); }
    | partial-ptr-declarator
    { $$ = MakeNewDeclarator( state->gstack->u.dspec, $1, NULL ); }
    |                        actual-new-declarator
    { $$ = MakeNewDeclarator( state->gstack->u.dspec, NULL, $1 ); }
    ;

partial-ptr-declarator
    : Y_TIMES cv-qualifiers-opt partial-ptr-declarator
    { $$ = MakeNewPointer( $2, $3, NULL ); }
    | Y_TIMES cv-qualifiers-opt
    { $$ = MakeNewPointer( $2, NULL, NULL ); }
    | Y_SCOPED_TIMES cv-qualifiers-opt partial-ptr-declarator
    { $$ = MakeNewPointer( $2, $3, $1 ); }
    | Y_SCOPED_TIMES cv-qualifiers-opt
    { $$ = MakeNewPointer( $2, NULL, $1 ); }
    | template-class-id Y_TEMPLATE_SCOPED_TIMES cv-qualifiers-opt partial-ptr-declarator
    { $$ = MakeNewPointer( $3, $4, $2 ); PTypeRelease( $1 ); }
    | template-class-id Y_TEMPLATE_SCOPED_TIMES cv-qualifiers-opt
    { $$ = MakeNewPointer( $3, NULL, $2 ); PTypeRelease( $1 ); }
    | Y_TEMPLATE_SCOPED_TIMES cv-qualifiers-opt partial-ptr-declarator
    { $$ = MakeNewPointer( $2, $3, $1 ); zapTemplateClassDeclSpec( state ); }
    | Y_TEMPLATE_SCOPED_TIMES cv-qualifiers-opt
    { $$ = MakeNewPointer( $2, NULL, $1 ); zapTemplateClassDeclSpec( state ); }
    ;

actual-new-declarator
    : Y_LEFT_BRACKET expression Y_RIGHT_BRACKET
    { $$ = MakeNewDynamicArray( $2 ); }
    | actual-new-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    ;

delete-expression
    : Y_DELETE cast-expression
    { $$ = setLocation( MakeDeleteExpr( NULL, CO_DELETE, $2 ), &yylp[1] ); }
    | Y_DELETE Y_LEFT_BRACKET delete-size-expression-opt Y_RIGHT_BRACKET cast-expression
    { $$ = setLocation( MakeDeleteExpr( NULL, CO_DELETE_ARRAY, $5 ), &yylp[1] ); }
    | Y_GLOBAL_DELETE cast-expression
    { $$ = setLocation( MakeDeleteExpr( $1, CO_DELETE_G, $2 ), &yylp[1] ); }
    | Y_GLOBAL_DELETE Y_LEFT_BRACKET delete-size-expression-opt Y_RIGHT_BRACKET cast-expression
    { $$ = setLocation( MakeDeleteExpr( $1, CO_DELETE_G_ARRAY, $5 ), &yylp[1] ); }
    ;

delete-size-expression-opt
    : /* nothing */
    | expression
    {
        PTreeDeleteSizeExpr( $1 );
    }
    ;

postfix-expression
    : primary-expression
    | function-like-cast-expression
    | postfix-expression Y_LEFT_BRACKET expression Y_RIGHT_BRACKET
    { $$ = setLocation( PTreeBinary( CO_INDEX, $1, $3 ), &yylp[2] ); }
    | postfix-expression Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_CALL, $1, $3 ), &yylp[2] ); }
    | member-selection-expression field-expression
    { $$ = PTreeReplaceRight( $1, $2 ); }
    | postfix-expression Y_PLUS_PLUS
    { $$ = setLocation( PTreeUnary( CO_POST_PLUS_PLUS, $1 ), &yylp[2] ); }
    | postfix-expression Y_MINUS_MINUS
    { $$ = setLocation( PTreeUnary( CO_POST_MINUS_MINUS, $1 ), &yylp[2] ); }
    | Y___SEGNAME Y_LEFT_PAREN strings Y_RIGHT_PAREN
    { $$ = setLocation( PTreeUnary( CO_SEGNAME, $3 ), &yylp[1] ); }
    | Y_DYNAMIC_CAST Y_LT type-id Y_GT Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_DYNAMIC_CAST, $3, $6 ), &yylp[1] ); }
    | Y_STATIC_CAST Y_LT type-id Y_GT Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_STATIC_CAST, $3, $6 ), &yylp[1] ); }
    | Y_REINTERPRET_CAST Y_LT type-id Y_GT Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_REINTERPRET_CAST, $3, $6 ), &yylp[1] ); }
    | Y_CONST_CAST Y_LT type-id Y_GT Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeBinary( CO_CONST_CAST, $3, $6 ), &yylp[1] ); }
    | Y_TYPEID Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = setLocation( PTreeUnary( CO_TYPEID_EXPR, $3 ), &yylp[1] ); }
    | Y_TYPEID Y_LEFT_PAREN type-id Y_RIGHT_PAREN
    { $$ = setLocation( PTreeUnary( CO_TYPEID_TYPE, $3 ), &yylp[1] ); }
    ;

member-selection-expression
    : postfix-expression Y_DOT
    { $$ = setLocation( PTreeBinary( CO_DOT, $1, NULL ), &yylp[2] ); }
    | postfix-expression Y_ARROW
    { $$ = setLocation( PTreeBinary( CO_ARROW, $1, NULL ), &yylp[2] ); }
    ;

function-like-cast-expression
    : simple-type-specifier Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN %ambig 0 Y_LEFT_PAREN
    { $$ = setLocation( MakeFunctionLikeCast( $1, $3 ), &yylp[2] ); }
    ;

primary-expression
    : literal
    | Y_THIS
    { $$ = setLocation( PTreeThis(), &yylp[1] ); }
    | Y_GLOBAL_ID
    { $$ = MakeGlobalId( $1 ); }
    | Y_GLOBAL_OPERATOR operator-function-type
    { $$ = MakeGlobalOperatorId( $1, $2 ); }
    | Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = $2; }
    | id-expression
    ;

id-expression
    : Y_ID
    | operator-name
    | qualified-id-expression
    ;

make-id
    : Y_ID
    | Y_TYPE_NAME
    | Y_TEMPLATE_NAME
    | Y_NAMESPACE_NAME
    ;

destructor-name
    : Y_TILDE make-id
    { $$ = setLocation( MakeDestructorId( $2 ), &yylp[1] ); }
    | Y_TILDE template-class-id
    { $$ = setLocation( MakeDestructorIdFromType( $2 ), &yylp[1] ); }
    ;

field-name
    : make-id
    | destructor-name
    | operator-name
    ;

field-expression
    : field-name
    | qualified-id-expression
    /* handles 'int::~int' cases (p. 280 ARM) */
    | basic-type-specifier Y_GLOBAL_TILDE basic-type-specifier
    { $$ = MakeScalarDestructor( $1, $2, $3 ); }
    ;

qualified-id-expression
    : Y_SCOPED_ID
    { $$ = MakeScopedId( $1 ); }
    | Y_SCOPED_TILDE make-id
    { $$ = MakeScopedDestructorId( $1, $2 ); }
    | Y_SCOPED_OPERATOR conversion-function-type
    {
        $$ = MakeScopedUserConversionId( $1, $2 );
        ScopeQualifyPop();
    }
    | Y_SCOPED_OPERATOR operator-function-type
    {
        $$ = MakeScopedOperatorId( $1, $2 );
        ScopeQualifyPop();
    }
    | template-class-id Y_TEMPLATE_SCOPED_ID
    { $$ = MakeScopedId( $2 ); PTypeRelease( $1 ); }
    | template-class-id Y_TEMPLATE_SCOPED_TILDE make-id
    { $$ = MakeScopedDestructorId( $2, $3 ); PTypeRelease( $1 ); }
    | template-class-id Y_TEMPLATE_SCOPED_OPERATOR conversion-function-type
    {
        $$ = MakeScopedUserConversionId( $2, $3 );
        ScopeQualifyPop();
        PTypeRelease( $1 );
    }
    | template-class-id Y_TEMPLATE_SCOPED_OPERATOR operator-function-type
    {
        $$ = MakeScopedOperatorId( $2, $3 );
        ScopeQualifyPop();
        PTypeRelease( $1 );
    }
    | Y_TEMPLATE_SCOPED_ID
    { $$ = MakeScopedId( $1 ); zapTemplateClassDeclSpec( state ); }
    | Y_TEMPLATE_SCOPED_TILDE make-id
    { $$ = MakeScopedDestructorId( $1, $2 ); zapTemplateClassDeclSpec( state ); }
    | Y_TEMPLATE_SCOPED_OPERATOR conversion-function-type
    {
        $$ = MakeScopedUserConversionId( $1, $2 );
        ScopeQualifyPop();
        zapTemplateClassDeclSpec( state );
    }
    | Y_TEMPLATE_SCOPED_OPERATOR operator-function-type
    {
        $$ = MakeScopedOperatorId( $1, $2 );
        ScopeQualifyPop();
        zapTemplateClassDeclSpec( state );
    }
    ;

operator-name
    : Y_OPERATOR conversion-function-type
    { $$ = setLocation( $2, &yylp[1] ); }
    | Y_OPERATOR operator-function-type
    { $$ = setLocation( MakeOperatorId( $2 ), &yylp[1] ); }
    ;

conversion-function-type
    : type-specifiers
    {
        $$ = MakeUserConversionId( $1, NULL );
        GStackPop( &(state->gstack) );
    }
    | type-specifiers conversion-function-ptr-declarator
    {
        $$ = MakeUserConversionId( $1, $2 );
        GStackPop( &(state->gstack) );
    }
    ;
    
conversion-function-ptr-declarator
    : ptr-operator
    { $$ = MakeAbstractDeclarator( $1 ); }
    | ptr-operator conversion-function-ptr-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    ;

operator-function-type
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
    | Y_DIVIDE
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
    | Y_DIVIDE_EQUAL
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

literal
    : Y_CONSTANT
    | Y_TRUE
    | Y_FALSE
    | strings
    ;

strings
    : Y_STRING
    | strings Y_STRING
    { $$ = PTreeStringLiteralConcat( $1, $2 ); }
    ;

/*** declaration syntax ***/
might-restart-declarations
    : start-restart-declarations declarations
    { popRestartDecl( state ); }
    ;
    
start-restart-declarations
    : /* nothing */
    { pushRestartDecl( state ); }
    ;
    
/* should only be referenced by might-restart-declarations */
declarations
    : declaration
    | declarations declaration
    ;

declaration
    : simple-declaration-before-semicolon Y_SEMI_COLON
    | function-definition
    | linkage-specification
    | template-declaration
    | template-directive
    | namespace-definition
    | Y_SEMI_COLON
    ;
    
namespace-alias-definition
    : Y_NAMESPACE make-id Y_EQUAL raw-qualified-namespace-specifier
    { NameSpaceAlias( $2, $4 ); }
    ;
    
namespace-using-directive
    : Y_USING Y_NAMESPACE raw-qualified-namespace-specifier
    { NameSpaceUsingDirective( $3 ); }
    ;
    
namespace-using-declaration
    : Y_USING qualified-type-specifier
    { NameSpaceUsingDeclType( $2 ); }
    | Y_USING qualified-id-expression
    { NameSpaceUsingDeclId( $2 ); }
    | Y_USING Y_SCOPED_NAMESPACE_NAME
    { NameSpaceUsingDeclId( MakeScopedId( $2 ) ); }
    | Y_USING Y_GLOBAL_ID
    { NameSpaceUsingDeclId( MakeGlobalId( $2 ) ); }
    | Y_USING Y_GLOBAL_OPERATOR operator-function-type
    { NameSpaceUsingDeclId( MakeGlobalOperatorId( $2, $3 ) ); }
    ;
    
raw-qualified-namespace-specifier
    : Y_SCOPED_ID
    | Y_SCOPED_TYPE_NAME
    | Y_SCOPED_TEMPLATE_NAME
    | Y_SCOPED_NAMESPACE_NAME
    | Y_GLOBAL_ID
    | Y_GLOBAL_TYPE_NAME
    | Y_GLOBAL_TEMPLATE_NAME
    | Y_GLOBAL_NAMESPACE_NAME
    | Y_ID
    | Y_TYPE_NAME
    | Y_TEMPLATE_NAME
    | Y_NAMESPACE_NAME
    ;
    
namespace-definition
    : namespace-key Y_LEFT_BRACE namespace-body Y_RIGHT_BRACE
    ;
    
namespace-key
    : Y_NAMESPACE
    { NameSpaceUnnamed( &yylp[1] ); }
    | Y_NAMESPACE make-id
    { NameSpaceNamed( $2 ); }
    ;
    
namespace-body
    : /* nothing */
    { NameSpaceClose(); }
    | might-restart-declarations
    { NameSpaceClose(); }
    ;

local-declaration
    : simple-declaration-before-semicolon Y_SEMI_COLON
    ;

access-declaration
    : qualified-id-expression
    { ClassAccessDeclaration( $1, &yylocation ); }
    | qualified-type-specifier
    { ClassAccessTypeDeclaration( $1, &yylocation ); }
    ;

simple-declaration-before-semicolon
    : simple-declaration
    {
        if( t != Y_SEMI_COLON ) {
	    SetErrLoc( &yylocation );
            CErr1( ERR_SYNTAX_MISSING_SEMICOLON );
            what = P_DIAGNOSED;
        }
    }
    ;

simple-declaration
    : decl-specifiers declarator-list
    {
        CheckDeclarationDSpec( state->gstack->u.dspec, CurrScope );
        GStackPop( &(state->gstack) );
    }
    | no-declarator-declaration
    {
        CheckDeclarationDSpec( state->gstack->u.dspec, CurrScope );
        GStackPop( &(state->gstack) );
    }
    | asm-declaration
    | namespace-alias-definition
    | namespace-using-directive
    | namespace-using-declaration
    ;

no-declarator-declaration
    : decl-specifiers
    {
        $$ = CheckNoDeclarator( $1 );
        if( $$ == NULL ) {
            what = P_SYNTAX;
        }
    }
    ;

decl-specifiers
    : non-type-decl-specifiers type-specifier maybe-type-decl-specifiers
    {
        $1 = PTypeCombine( $1, $2 );
        $$ = PTypeCombine( $1, $3 );
        $$ = PTypeDone( $$, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    | non-type-decl-specifiers type-specifier
    {
        $$ = PTypeCombine( $1, $2 );
        $$ = PTypeDone( $$, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    |                          type-specifier maybe-type-decl-specifiers
    {
        $$ = PTypeCombine( $1, $2 );
        $$ = PTypeDone( $$, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    |                          type-specifier
    {
        $$ = PTypeDone( $1, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    | non-type-decl-specifiers
    {
        $$ = PTypeDone( $1, t == Y_SEMI_COLON );
        pushUserDeclSpec( state, $$ );
    }
    ;

non-type-decl-specifiers
    : non-type-decl-specifier
    | non-type-decl-specifiers non-type-decl-specifier
    { $$ = PTypeCombine( $1, $2 ); }
    ;

maybe-type-decl-specifiers
    : non-type-decl-specifier
    | basic-type-specifier
    | maybe-type-decl-specifiers non-type-decl-specifier
    { $$ = PTypeCombine( $1, $2 ); }
    | maybe-type-decl-specifiers basic-type-specifier
    { $$ = PTypeCombine( $1, $2 ); }
    ;

non-type-decl-specifier
    : storage-class-specifier
    | function-specifier
    | cv-qualifier
    | ms-specific-declspec
    ;
    
ms-specific-declspec
    : Y___DECLSPEC Y_LEFT_PAREN                 Y_RIGHT_PAREN
    { $$ = PTypeMSDeclSpec( NULL, NULL ); }
    | Y___DECLSPEC Y_LEFT_PAREN ms-declspec-seq Y_RIGHT_PAREN
    { $$ = $3; }
    ;
    
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
    | Y_EXTERN
    { $$ = PTypeStgClass( STG_EXTERN ); }
    | Y_STATIC
    { $$ = PTypeStgClass( STG_STATIC ); }
    | Y_TYPEDEF
    { $$ = PTypeStgClass( STG_TYPEDEF ); }
    | Y_MUTABLE
    { $$ = PTypeStgClass( STG_MUTABLE ); }
    | Y_EXTERN linkage-id
    { $$ = PTypeLinkage(); }
    ;

function-specifier
    : Y_INLINE
    { $$ = PTypeSpecifier( STY_INLINE ); }
    | Y_VIRTUAL
    { $$ = PTypeSpecifier( STY_VIRTUAL ); }
    | Y_FRIEND
    { $$ = PTypeSpecifier( STY_FRIEND ); }
    | Y_EXPLICIT
    { $$ = PTypeSpecifier( STY_EXPLICIT ); }
    ;

type-specifier
    : simple-type-specifier %prec Y_FAVOUR_REDUCE_SPECIAL %ambig 0 Y_LEFT_PAREN
    | class-specifier
    | enum-specifier
    | typeof-specifier
    ;

typeof-specifier
    : Y___TYPEOF Y_LEFT_PAREN expression Y_RIGHT_PAREN
    { $$ = PTypeExpr( $3 ); }
    | Y___TYPEOF Y_LEFT_PAREN type-id Y_RIGHT_PAREN
    { $$ = PTypeTypeid( $3 ); }
    ;

type-specifiers
    : decl-specifiers
    {
        $$ = CheckTypeSpecifier( $1 );
        if( $$ == NULL ) {
            what = P_SYNTAX;
        }
    }
    ;

cv-qualifier
    : Y_CONST
    { $$ = PTypeSpecifier( STY_CONST ); }
    | Y_VOLATILE
    { $$ = PTypeSpecifier( STY_VOLATILE ); }
    | Y___UNALIGNED
    { $$ = PTypeSpecifier( STY_UNALIGNED ); }
    ;

simple-type-specifier
    : qualified-type-specifier
    | basic-type-specifier
    ;

basic-type-specifier
    : Y_CHAR
    { $$ = PTypeScalar( STM_CHAR ); }
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
    | Y___SEGMENT
    { $$ = PTypeScalar( STM_SEGMENT ); }
    | Y___INT64
    { $$ = PTypeScalar( STM_INT64 ); }
    ;

qualified-type-specifier
    : Y_TYPE_NAME
    { $$ = sendType( $1 ); }
    | Y_SCOPED_TYPE_NAME
    { $$ = sendType( $1 ); }
    | template-class-id Y_TEMPLATE_SCOPED_TYPE_NAME
    { $$ = sendType( $2 ); PTypeRelease( $1 ); }
    | Y_GLOBAL_TYPE_NAME
    { $$ = sendType( $1 ); }
    | template-class-id
    ;

qualified-class-specifier
    : qualified-class-type
    {
        if( $$ == NULL ) {
            what = P_SYNTAX;
        }
    }
    ;

qualified-class-type
    : Y_TYPE_NAME
    { $$ = sendClass( $1 ); }
    | Y_GLOBAL_TYPE_NAME
    { $$ = sendClass( $1 ); }
    | Y_SCOPED_TYPE_NAME
    { $$ = sendClass( $1 ); }
    | template-class-id Y_TEMPLATE_SCOPED_TYPE_NAME
    { $$ = sendClass( $2 ); PTypeRelease( $1 ); }
    | template-class-id
    ;

enum-specifier
    : enum-key enum-start enumerator-list Y_COMMA Y_RIGHT_BRACE
    {
        $$ = MakeEnumType( &(state->gstack->u.enumdata) );
        GStackPop( &(state->gstack) );
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
    | enum-key
    {
        $$ = EnumReference( &(state->gstack->u.enumdata) );
        GStackPop( &(state->gstack) );
    }
    ;

enum-start
    : Y_LEFT_BRACE
    { EnumDefine( &(state->gstack->u.enumdata) ); }
    ;

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
    : enumerator
    | enumerator-list Y_COMMA enumerator
    ;

enumerator
    : make-id
    {
        MakeEnumMember( &(state->gstack->u.enumdata), $1, NULL );
    }
    | make-id Y_EQUAL constant-expression
    {
        MakeEnumMember( &(state->gstack->u.enumdata), $1, $3 );
    }
    ;

asm-declaration
    : Y_ASM Y_LEFT_PAREN asm-id Y_RIGHT_PAREN
    ;

asm-id
    : strings
    {
        StringTrash( $1->u.string );
        PTreeFree( $1 );
        CErr1( WARN_ASM_IGNORED );
    }
    ;

linkage-specification
    : Y_EXTERN linkage-id start-linkage-block linkage-body Y_RIGHT_BRACE
    { LinkagePop(); }
    ;
    
linkage-body
    : /* nothing */
    | might-restart-declarations
    ;

start-linkage-block
    : Y_LEFT_BRACE
    { LinkageBlock(); }
    ;

linkage-id
    : strings
    {
        LinkagePush( $1->u.string->string );
        StringTrash( $1->u.string );
        PTreeFree( $1 );
        if( ! ScopeType( CurrScope, SCOPE_FILE ) ) {
            CErr1( ERR_ONLY_GLOBAL_LINKAGES );
        }
    }
    ;

declarator-list
    : init-declarator
    {
        tryCtorStyleInit( state, $1 );
    }
    | declarator-list Y_COMMA comma-init-declarator
    {
        tryCtorStyleInit( state, $3 );
    }
    ;

init-declarator
    : declarator
    {
        $$ = InsertDeclInfo( CurrScope, $1 );
        GStackPush( &(state->gstack), GS_INIT_DATA );
        $$ = DataInitNoInit( &(state->gstack->u.initdata), $$ );
        GStackPop( &(state->gstack) );
    }
    | declaring-declarator initializer
    { $$ = $1; }
    | ptr-mod-init-declarator
    {
        $1 = FinishDeclarator( state->gstack->u.dspec, $1 );
        $$ = InsertDeclInfo( CurrScope, $1 );
    }
    | actual-declarator Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    {
        $1 = FinishDeclarator( state->gstack->u.dspec, $1 );
        $$ = InsertDeclInfo( CurrScope, $1 );
	setInitWithLocn( $$, $3, &yylp[2] );
    }
    ;

comma-init-declarator
    : comma-declarator
    {
        $$ = InsertDeclInfo( CurrScope, $1 );
        GStackPush( &(state->gstack), GS_INIT_DATA );
        $$ = DataInitNoInit( &(state->gstack->u.initdata), $$ );
        GStackPop( &(state->gstack) );
    }
    | comma-declaring-declarator initializer
    { $$ = $1; }
    | cv-qualifiers-opt ptr-mod-init-declarator
    {
	$2 = AddMSCVQualifierKludge( $1, $2 );
        $2 = FinishDeclarator( state->gstack->u.dspec, $2 );
        $$ = InsertDeclInfo( CurrScope, $2 );
    }
    | cv-qualifiers-opt actual-declarator Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    {
	$2 = AddMSCVQualifierKludge( $1, $2 );
        $2 = FinishDeclarator( state->gstack->u.dspec, $2 );
        $$ = InsertDeclInfo( CurrScope, $2 );
	setInitWithLocn( $$, $4, &yylp[3] );
    }
    ;

declaring-declarator
    : declarator
    {
        $$ = InsertDeclInfo( CurrScope, $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    ;

comma-declaring-declarator
    : comma-declarator
    {
        $$ = InsertDeclInfo( CurrScope, $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    ;

declarator
    : ptr-mod-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    | actual-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    ;

comma-declarator
    : cv-qualifiers-opt ptr-mod-declarator
    {
	$2 = AddMSCVQualifierKludge( $1, $2 );
        $$ = FinishDeclarator( state->gstack->u.dspec, $2 );
    }
    | cv-qualifiers-opt actual-declarator
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
    | ptr-mod actual-declarator Y_LEFT_PAREN expression-list Y_RIGHT_PAREN
    {
	$$ = AddDeclarator( $2, $1 );
	setInitWithLocn( $$, $4, &yylp[3] );
    }
    ;

ptr-mod-declarator
    : ptr-mod ptr-mod-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod actual-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    ;

actual-declarator
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
    | actual-declarator Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN cv-qualifiers-opt except-spec-opt
    {
        $$ = AddDeclarator( $1, MakeFnType( &($3), $5, $6 ) );
	$$ = AddExplicitParms( $$, $3 );
    }
    | actual-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    | actual-declarator Y_LEFT_BRACKET                     Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, NULL ); }
    | Y_LEFT_PAREN ptr-mod-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    | Y_LEFT_PAREN actual-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    ;

modifier
    : Y___NEAR
    { $$ = TF1_NEAR; }
    | Y___FAR
    { $$ = TF1_FAR; }
    | Y___FAR16
    { $$ = TF1_SET_FAR16;       /* equals TF1_FAR on the 8086 */ }
    | Y___HUGE
    { $$ = TF1_SET_HUGE;        /* equals TF1_FAR on the 386 */ }
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
    : segment-cast-opt Y___SEGNAME Y_LEFT_PAREN strings Y_RIGHT_PAREN
    { $$ = MakeBasedModifier( TF1_BASED_STRING, $1, $4 ); }
    | segment-cast-opt Y_ID
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
    | Y___CDECL
    { $$ = MakeIndexPragma( M_CDECL ); }
    | Y___PASCAL
    { $$ = MakeIndexPragma( M_PASCAL ); }
    | Y___FORTRAN
    { $$ = MakeIndexPragma( M_FORTRAN ); }
    | Y__SYSCALL
    { $$ = MakeIndexPragma( M_SYSCALL ); }
    | Y__OPTLINK
    { $$ = MakeIndexPragma( M_OPTLINK ); }
    | Y___STDCALL
    { $$ = MakeIndexPragma( M_STDCALL ); }
    ;

pragma-id
    : strings
    ;

ptr-operator
    : Y_TIMES cv-qualifiers-opt
    { $$ = MakePointerType( TF1_NULL, $2 ); }
    | Y_AND cv-qualifiers-opt
    { $$ = MakePointerType( TF1_REFERENCE, $2 ); }
    | Y_SCOPED_TIMES cv-qualifiers-opt
    { $$ = MakeMemberPointer( $1, $2 ); }
    | template-class-id Y_TEMPLATE_SCOPED_TIMES cv-qualifiers-opt
    { $$ = MakeMemberPointer( $2, $3 ); PTypeRelease( $1 ); }
    | Y_TEMPLATE_SCOPED_TIMES cv-qualifiers-opt
    { $$ = MakeMemberPointer( $1, $2 ); zapTemplateClassDeclSpec( state ); }
    | Y_TIMES Y__SEG16 cv-qualifiers-opt
    { $$ = MakeSeg16Pointer( $3 ); }
    ;

cv-qualifiers-opt
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
    : Y_ID
    | Y_GLOBAL_ID
    | Y_TEMPLATE_NAME
    | Y_NAMESPACE_NAME
    | qualified-type-specifier
    { $$ = MakeIdFromType( $1 ); }
    | destructor-name
    | operator-name
    | qualified-id-expression
    ;
    
special-new-type-id
    : type-specifiers special-new-abstract-declarator
    {
        $$ = $2;
        GStackPop( &(state->gstack) );
    }
    | type-specifiers
    {
        $$ = MakeNewTypeId( DoDeclSpec( state->gstack->u.dspec ) );
        GStackPop( &(state->gstack) );
    }
    ;

type-id
    : type-specifiers abstract-declarator
    {
        $$ = TypeDeclarator( $2 );
        GStackPop( &(state->gstack) );
    }
    | type-specifiers
    {
        $$ = DoDeclSpec( state->gstack->u.dspec );
        GStackPop( &(state->gstack) );
    }
    ;

type-id-list
    : type-id
    | type-id-list Y_COMMA type-id
    { $$ = PTreeTListAppend( $1, $3 ); }
    ;

special-new-abstract-declarator
    : special-new-abstract-ptr-mod-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    | special-new-actual-abstract-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    ;

abstract-declarator
    : abstract-ptr-mod-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    | actual-abstract-declarator
    {
        $$ = FinishDeclarator( state->gstack->u.dspec, $1 );
    }
    ;

special-new-abstract-ptr-mod-declarator
    : ptr-mod special-new-abstract-ptr-mod-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod special-new-actual-abstract-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod
    { $$ = MakeAbstractDeclarator( $1 ); }
    ;

abstract-ptr-mod-declarator
    : ptr-mod abstract-ptr-mod-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod actual-abstract-declarator
    { $$ = AddDeclarator( $2, $1 ); }
    | ptr-mod
    { $$ = MakeAbstractDeclarator( $1 ); }
    ;

special-new-actual-abstract-declarator
    : special-new-actual-abstract-declarator Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN cv-qualifiers-opt except-spec-opt
    {
        $$ = AddDeclarator( $1, MakeFnType( &($3), $5, $6 ) );
        FreeArgs( $3 );
    }
    | Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN cv-qualifiers-opt except-spec-opt
    {
        $$ = MakeAbstractDeclarator( MakeFnType( &($2), $4, $5 ) );
        FreeArgs( $2 );
    }
    | special-new-actual-abstract-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    |                            Y_LEFT_BRACKET expression Y_RIGHT_BRACKET
    { $$ = MakeNewDynamicArray( $2 ); }
    | special-new-actual-abstract-declarator Y_LEFT_BRACKET Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, NULL ); }
    |                            Y_LEFT_BRACKET Y_RIGHT_BRACKET
    {
        $$ = MakeAbstractDeclarator( NULL );
        $$ = AddArrayDeclarator( $$, NULL );
    }
    | Y_LEFT_PAREN special-new-actual-abstract-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    | Y_LEFT_PAREN special-new-abstract-ptr-mod-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    ;

actual-abstract-declarator
    : actual-abstract-declarator Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN cv-qualifiers-opt except-spec-opt
    {
        $$ = AddDeclarator( $1, MakeFnType( &($3), $5, $6 ) );
        FreeArgs( $3 );
    }
    | Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN cv-qualifiers-opt except-spec-opt
    {
        $$ = MakeAbstractDeclarator( MakeFnType( &($2), $4, $5 ) );
        FreeArgs( $2 );
    }
    | actual-abstract-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, $3 ); }
    |                            Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
    {
        $$ = MakeAbstractDeclarator( NULL );
        $$ = AddArrayDeclarator( $$, $2 );
    }
    | actual-abstract-declarator Y_LEFT_BRACKET Y_RIGHT_BRACKET
    { $$ = AddArrayDeclarator( $1, NULL ); }
    |                            Y_LEFT_BRACKET Y_RIGHT_BRACKET
    {
        $$ = MakeAbstractDeclarator( NULL );
        $$ = AddArrayDeclarator( $$, NULL );
    }
    | Y_LEFT_PAREN actual-abstract-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    | Y_LEFT_PAREN abstract-ptr-mod-declarator Y_RIGHT_PAREN
    { $$ = $2; }
    ;

abstract-args
    : /* nothing */
    { $$ = NULL; }
    | arg-declaration-list
    | arg-declaration-list Y_COMMA Y_DOT_DOT_DOT
    { $$ = AddEllipseArg( $1 ); }
    | arg-declaration-list         Y_DOT_DOT_DOT
    { $$ = AddEllipseArg( $1 ); }
    | Y_DOT_DOT_DOT
    { $$ = AddEllipseArg( NULL ); }
    ;

template-abstract-args
    : /* nothing */
    { $$ = NULL; }
    | template-arg-declaration-list
    ;

arg-declaration-list
    : arg-declaration
    { $$ = AddArgument( NULL, $1 ); }
    | arg-declaration-list Y_COMMA arg-declaration
    { $$ = AddArgument( $1, $3 ); }
    ;

template-arg-declaration-list
    : template-arg-declaration
    { $$ = AddArgument( NULL, $1 ); }
    | template-arg-declaration-list Y_COMMA template-arg-declaration
    { $$ = AddArgument( $1, $3 ); }
    ;

arg-decl-specifiers
    : decl-specifiers
    {
        $$ = CheckArgDSpec( $1 );
        if( $$ == NULL ) {
            what = P_SYNTAX;
        }
    }
    ;

arg-declaration
    : simple-arg-declaration
    { GStackPop( &(state->gstack) ); }
    ;

template-arg-declaration
    : simple-template-arg-declaration
    { GStackPop( &(state->gstack) ); }
    ;

simple-arg-declaration
    : arg-decl-specifiers
    { $$ = DeclSpecDeclarator( $1 ); }
    | arg-decl-specifiers simple-arg-no-id defarg-parse-or-copy
    { $$ = $2; }
    | arg-decl-specifiers declarator
    { $$ = $2; }
    | arg-decl-specifiers declarator defarg-parse-or-copy
    { $$ = $2; }
    | arg-decl-specifiers abstract-declarator
    { $$ = $2; }
    | arg-decl-specifiers abstract-declarator defarg-parse-or-copy
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
    
defarg-check
    : /* nothing */
    {
	if( ClassOKToRewrite() ) {
	    if( t != Y_EQUAL ) {
		what = P_SYNTAX;
		$$ = NULL;
	    } else {
		$$ = RewritePackageDefArg( NULL );
		t = Y_DEFARG_GONE_SPECIAL;
	    }
	} else {
	    $$ = NULL;
	}
    }
    ;

simple-template-arg-declaration
    : arg-decl-specifiers
    { $$ = DeclSpecDeclarator( $1 ); }
    | arg-decl-specifiers Y_EQUAL template-assignment-expression
    { $$ = DeclSpecDeclarator( $1 ); $$->defarg_expr = $3; }
    | arg-decl-specifiers declarator
    { $$ = $2; }
    | arg-decl-specifiers declarator Y_EQUAL template-assignment-expression
    { $$ = $2; $$->defarg_expr = $4; }
    | arg-decl-specifiers abstract-declarator
    { $$ = $2; }
    | arg-decl-specifiers abstract-declarator Y_EQUAL template-assignment-expression
    { $$ = $2; $$->defarg_expr = $4; }
    ;

ctor-declarator
    : Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN cv-qualifiers-opt except-spec-opt
    {
        DECL_SPEC *dspec;
        PTREE id;

        dspec = state->gstack->u.dspec;
        id = MakeConstructorId( dspec );
        if( id == NULL ) {
            what = P_SYNTAX;
        }
        $$ = MakeDeclarator( dspec, id );
        if( $$->template_member ) {
            what = P_CLASS_TEMPLATE;
        } else {
            AddDeclarator( $$, MakeFnType( &($2), $4, $5 ) );
	    $$ = AddExplicitParms( $$, $2 );
            $$ = FinishDeclarator( dspec, $$ );
        }
    }
    ;

function-declaration
    : decl-specifiers declarator ctor-initializer
    {
        $2->mem_init = $3;
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    | decl-specifiers declarator
    {
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    |                 declarator ctor-initializer
    {
        $1->mem_init = $2;
        $$ = DeclFunction( NULL, $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    |                 declarator
    {
        $$ = DeclFunction( NULL, $1 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    | decl-specifiers ctor-declarator ctor-initializer
    {
        $2->mem_init = $3;
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    | decl-specifiers ctor-declarator
    {
        $$ = DeclFunction( state->gstack->u.dspec, $2 );
        GStackPush( &(state->gstack), GS_DECL_INFO );
        state->gstack->u.dinfo = $$;
    }
    ;

function-definition
    : function-declaration function-body
    {
        GStackPop( &(state->gstack) );	/* decl-info */
	if( $1->has_dspec ) {
	    GStackPop( &(state->gstack) );	/* decl-spec */
	}
        if( $1->body != NULL ) {
            if( ScopeType( CurrScope, SCOPE_TEMPLATE_DECL ) ) {
                TemplateFunctionAttachDefn( $1 );
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

/* must be before 'class-specifier' to resolve r/r conflict properly */
start-class
    : /* nothing */
    {
        ClassStart();
        pushDefaultDeclSpec( state );
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

no-class-name
    : /* nothing */
    { ClassName( NULL, CLASS_DEFINITION ); }
    ;

class-name
    : make-id
    {
        CLASS_DECL decl_type;
        CLNAME_STATE after_name;

        decl_type = CLASS_REFERENCE;
        switch( t ) {
        case Y_LEFT_BRACE:
        case Y_COLON:
	    if( state->template_class_inst_defer ) {
		what = P_DEFER_DEFN;
		decl_type = CLASS_DECLARATION;
	    } else {
		decl_type = CLASS_DEFINITION;
	    }
            break;
        case Y_SEMI_COLON:
            decl_type = CLASS_DECLARATION;
            break;
        case Y_COMMA:
        case Y_GT:
        case Y_EQUAL:
            if( state->template_args ) {
                decl_type = CLASS_GENERIC;
            }
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
    | template-class-pre-id
    {
        CLASS_DECL decl_type;

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
        ClassSpecificInstantiation( $1, decl_type );
        what = P_RELEX;
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
    : start-class Y_LEFT_BRACE
    { $$ = ClassEnd(); what = P_RELEX; }
    | start-class Y_LEFT_BRACE member-list
    { $$ = ClassEnd(); what = P_RELEX; }
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

member-list
    : member-declaration
    | member-list member-declaration
    | access-specifier Y_COLON
    { ClassPermission( $1 ); }
    | member-list access-specifier Y_COLON
    { ClassPermission( $2 ); }
    ;

access-specifier
    : Y_PRIVATE
    { $$ = SF_PRIVATE; }
    | Y_PROTECTED
    { $$ = SF_PROTECTED; }
    | Y_PUBLIC
    { $$ = SF_NULL; }
    ;

member-declaration
    : simple-member-declaration-before-semicolon Y_SEMI_COLON
    | function-definition
    | access-declaration Y_SEMI_COLON
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
    : decl-specifiers member-declarator-list
    {
        GStackPop( &(state->gstack) );
    }
    |                 member-declarator-list
    {
    }
    | decl-specifiers
    {
        $1 = CheckNoDeclarator( $1 );
        if( $1 == NULL ) {
            what = P_SYNTAX;
        }
        GStackPop( &(state->gstack) );
    }
    | decl-specifiers ctor-declarator
    {
        InsertDeclInfo( CurrScope, $2 );
        FreeDeclInfo( $2 );
        GStackPop( &(state->gstack) );
    }
    | namespace-using-declaration
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
    : declarator Y_EQUAL Y_CONSTANT
    {
        VerifyPureFunction( $1, $3 );
        $$ = InsertDeclInfo( CurrScope, $1 );
    }
    | declarator
    {
        $$ = InsertDeclInfo( CurrScope, $1 );
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

ctor-initializer
    : Y_COLON
    {
        $$ = RewritePackageMemInit( getMultiToken() );
        what = P_RELEX;
    }
    ;

mem-initializer-list
    : mem-initializer-item
    { $$ = PTreeBinary( CO_LIST, NULL, $1 ); }
    | mem-initializer-list Y_COMMA mem-initializer-item
    { $$ = PTreeBinary( CO_LIST,   $1, $3 ); }
    ;

mem-initializer-item
    : qualified-class-specifier Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
    { $$ = MakeMemInitItem( $1, NULL, $3, &yylp[2] ); }
    | Y_ID Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
    { $$ = MakeMemInitItem( NULL, $1, $3, &yylp[2] ); }
    ;

/*** template syntax ***/
template-declaration
    : template-key Y_LT template-argument-decl Y_GT template-def
    {
        RewriteFree( ParseGetRecordingInProgress( NULL ) );
        state->template_decl = FALSE;
    }
    ;

template-key
    : Y_TEMPLATE
    {
        if( state->template_decl ) {
            CErr1( ERR_NO_NESTED_TEMPLATES );
        } else if( ! ScopeType( CurrScope, SCOPE_FILE ) ) {
            CErr1( ERR_ONLY_GLOBAL_TEMPLATES );
        }
        state->template_decl = TRUE;
        state->template_args = TRUE;
    }
    ;

template-argument-decl
    : template-abstract-args
    {
        state->template_args = FALSE;
        GStackPush( &(state->gstack), GS_TEMPLATE_DATA );
        TemplateDeclInit( &(state->gstack->u.templatedata), $1 );
        pushDefaultDeclSpec( state );
        state->template_record_tokens = RewriteRecordInit( &(state->template_record_locn) );
    }
    ;

template-def
    : class-template Y_SEMI_COLON
    | function-definition
    {
        GStackPop( &(state->gstack) );
        GStackPop( &(state->gstack) );
    }
    ;

class-template
    : simple-declaration-before-semicolon
    {
        GStackPop( &(state->gstack) );
        GStackPop( &(state->gstack) );
    }
    ;

template-class-id
    : template-class-instantiation Y_GT
    ;

template-class-instantiation
    : Y_TEMPLATE_NAME Y_LT template-arg-list
    {
        $$ = TemplateClassInstantiation( $1, $3, TCI_NULL );
        setWatchColonColon( state, $$ );
    }
    | Y_GLOBAL_TEMPLATE_NAME Y_LT template-arg-list
    {
        $$ = TemplateClassInstantiation( MakeTemplateId( $1 ), $3, TCI_NULL );
        setWatchColonColon( state, $$ );
    }
    ;

template-class-pre-id
    : template-class-pre-instantiation Y_GT
    ;

template-class-pre-instantiation
    : Y_TEMPLATE_NAME Y_LT template-arg-list
    { $$ = setLocation( PTreeBinary( CO_STORAGE, $1, $3 ), &yylp[2] ); }
    | Y_GLOBAL_TEMPLATE_NAME Y_LT template-arg-list
    {
        $1 = MakeTemplateId( $1 );
        $$ = setLocation( PTreeBinary( CO_STORAGE, $1, $3 ), &yylp[2] );
    }
    ;

template-arg-list
    : template-parameter-expression
    { $$ = PTreeBinary( CO_LIST, NULL, $1 ); }
    | template-arg-list Y_COMMA template-parameter-expression
    { $$ = setLocation( PTreeBinary( CO_LIST,   $1, $3 ), &yylp[2] ); }
    ;

template-parameter-expression
    : type-id
    | template-assignment-expression
    ;
    
template-directive
    : Y_EXTERN Y_TEMPLATE template-class-directive-extern Y_SEMI_COLON
    | Y_TEMPLATE template-class-directive-instantiate Y_SEMI_COLON
    ;
    
template-class-directive-extern
    : template-directive-class
    { TemplateClassDirective( $1, TCD_EXTERN ); }
    ;
    
template-class-directive-instantiate
    : template-directive-class
    { TemplateClassDirective( $1, TCD_INSTANTIATE ); }
    ;
    
template-directive-class
    : Y_TEMPLATE_NAME Y_LT template-arg-list Y_GT
    { $$ = PTreeBinary( CO_LIST, $1, $3 ); }
    | Y_GLOBAL_TEMPLATE_NAME Y_LT template-arg-list Y_GT
    { $$ = PTreeBinary( CO_LIST, MakeTemplateId( $1 ), $3 ); }
    ;

/*** exception syntax ***/
exception-declaration
    : actual-exception-declaration
    {
        $$ = $1;
        GStackPop( &(state->gstack) );
    }
    ;

actual-exception-declaration
    : type-specifiers
    { $$ = DeclSpecDeclarator( state->gstack->u.dspec ); }
    | type-specifiers declarator
    { $$ = $2; }
    | type-specifiers abstract-declarator
    { $$ = $2; }
    ;

except-spec-opt
    : /* nothing */
    { $$ = ThrowsAnything(); }
    | Y_THROW Y_LEFT_PAREN type-id-list Y_RIGHT_PAREN
    { $$ = $3; }
    | Y_THROW Y_LEFT_PAREN              Y_RIGHT_PAREN
    { $$ = NULL; }
    ;

%%

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %     Copyright (C) 1994, by WATCOM International Inc.  All rights    %
   %     reserved.  No part of this software may be reproduced or        %
   %     used in any form or by any means - graphic, electronic or       %
   %     mechanical, including photocopying, recording, taping or        %
   %     information storage and retrieval systems - except with the     %
   %     written permission of WATCOM International Inc.                 %
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    Date	By		Reason
    ----	--		------
    15-Jun-94	D.Wellstood	Initial Implementation
*/

%token <String>		T_NUM
%token <String>		T_NAME
%token <String>		T_FNAME
%token <String>		T_STRING

/* reserved words */
%token <String>		T_CHAR
%token <String>		T_SHORT
%token <String>		T_INT
%token <String>		T_LONG
%token <String>		T_SIGNED
%token <String>		T_UNSIGNED
%token <String>		T_FLOAT
%token <String>		T_DOUBLE
%token <String>		T_VOID
%token <String>		T_CONST
%token <String>		T_STRUCT
%token <String>		T_UNION
%token <String>		T_NEAR
%token <String>		T_FAR
%token <String>		T_HUGE
%token <String>		T_CDECL
%token <String>		T_PASCAL
%token <String>		T_FORTRAN
%token <String>		T_SYSCALL
%token <String>		T_EXPORT     
%token <String>		T_LPAREN
%token <String>		T_RPAREN
%token <String>		T_EXTERN
%token <String>		T_LINE

/* punctuation */
%token T_LSQ_BRACKET
%token T_PERIOD
%token T_RSQ_BRACKET
%token T_SEMICOLON
%token T_STAR
%token T_COMMA
%token T_POUND
%token T_SLASH

%type <Parm>		Y_PARAMETERS
%type <Parm>		Y_PARAM_LIST
%type <Type>		Y_TYPENAME
%type <String>		Y_STRUCT_OR_UNION
%type <String>		Y_STRUCT_OR_UNION_KEYWORD
%type <String>		Y_PREDEFINED_TYPE
%type <String>		Y_FNAME 
%type <String>		Y_NAME
%type <String>		Y_TYPENAME1
%type <Token>		Y_SPECIFIER
%type <Parm>		Y_DECLARATION
%type <Parm>		Y_DECLARATOR
%type <Parm>		Y_DIRECT_DECLARATOR
%type <Parm>		Y_ABS_DIRECT_DECLARATOR
%type <Parm>		Y_ABS_DECLARATOR


%start Y_FILE
%%

Y_FILE
	: Y_LINE
	| Y_FILE Y_LINE
	;

Y_LINE
	: Y_FUNCTION_STMNT
	| Y_LINE_STMNT
	| Y_STRUCT_STMNT
	| Y_UNION_STMNT
	;
	
Y_LINE_STMNT
	: T_SLASH T_SLASH T_POUND T_LINE Y_FNAME T_NUM
	    {
	        SemLine( $5, $6 ); 
	    }	
	;
	
Y_STRUCT_STMNT
	: T_STRUCT Y_NAME T_SEMICOLON
	    {}
	;

Y_UNION_STMNT
	: T_UNION Y_NAME T_SEMICOLON
	    {}
	;
	
Y_FUNCTION_STMNT
	: T_EXTERN Y_DECLARATION T_SEMICOLON
	    {
	        SemFunction( $2 );
	    }
	;

Y_DECLARATION
	: Y_TYPENAME Y_DECLARATOR
	    {
	        $$ = $2;
		$$->type = $1;
	    }
	| Y_TYPENAME Y_ABS_DECLARATOR
	    {
	        $$ = $2;
		$$->type = $1;
	    }
	| Y_TYPENAME
	    {
	        $$ = SemCreateParam( $1, NULL, 0, 0 );
	    }
	;
	
Y_ABS_DECLARATOR
	: T_STAR 
	    {
	        $$ = SemCreateParam( NULL, NULL, 1, 0 );
	    }
	| Y_SPECIFIER 
	    {
	        $$ = SemCreateParam( NULL, NULL, 0, 0 );
		$$->modifiers = SemAddSpecifier( $$->modifiers, $1 );
	    }
	| Y_ABS_DIRECT_DECLARATOR
	| T_STAR Y_ABS_DECLARATOR
	    {
	        $$ = $2;
		$$->ptrcnt++;
	    }
	| Y_SPECIFIER Y_ABS_DECLARATOR
	    {
	    	$$ = $2;
		$$->modifiers = SemAddSpecifier( $$->modifiers, $1 );
	    }
	;

Y_ABS_DIRECT_DECLARATOR
    	: T_LPAREN Y_ABS_DECLARATOR T_RPAREN
	    { $$ = $2; }
	| Y_ARRAY
	    {
	        $$ = SemCreateParam( NULL, NULL, 0, 1 );
	    }
	| Y_ABS_DIRECT_DECLARATOR Y_ARRAY
	    {
	        $$ = $1;
		$$->arraycnt ++;
	    }
	;
	
Y_DECLARATOR
	: T_STAR Y_DECLARATOR
	    {
	        $$ = $2;
		$$->ptrcnt++;
	    }
	| Y_SPECIFIER Y_DECLARATOR
	    {
	        $$ = $2;
		$$->modifiers = SemAddSpecifier( $$->modifiers, $1 );
	    }
	| Y_DIRECT_DECLARATOR
	;
	
Y_DIRECT_DECLARATOR
	: Y_NAME
	    {
	        $$ = SemCreateParam( NULL, $1, 0, 0 );
	    }
	| T_LPAREN Y_DECLARATOR T_RPAREN
	    { $$ = $2; }
	| Y_DIRECT_DECLARATOR Y_ARRAY
	    {
	        $$ = $1;
		$$->arraycnt ++;
	    }
	| Y_DIRECT_DECLARATOR Y_PARAMETERS
	    {
	        $$ = $1;
	        if( $$->parameters == NULL ) {
		    $$->parameters = $2;
		}
	    }
	;
	
Y_SPECIFIER
	: T_NEAR
	    { $$ = T_NEAR; }
	| T_CONST
	    { $$ = T_CONST; }
	| T_FAR
	    { $$ = T_FAR; }
	| T_HUGE
	    { $$ = T_HUGE; }
	| T_CDECL
	    { $$ = T_CDECL; }
	| T_PASCAL
	    { $$ = T_PASCAL; }
	| T_FORTRAN
	    { $$ = T_FORTRAN; }
	| T_SYSCALL
	    { $$ = T_SYSCALL; }
	| T_EXPORT     
	    { $$ = T_EXPORT; }
	;
	
Y_PARAMETERS
	: T_LPAREN Y_PARAM_LIST T_RPAREN
	    { $$ = $2; }
	| T_LPAREN Y_PARAM_LIST T_COMMA T_PERIOD T_PERIOD T_PERIOD T_RPAREN
	    {
	        ReportWarning( "Ellipsis (...) parmeters are not supported" );
		$$ = $2;
		$$->err = TRUE;
	    }
	;
	
Y_PARAM_LIST
	: Y_DECLARATION
	    { $$ = $1; }
	| Y_PARAM_LIST T_COMMA Y_DECLARATION
	    {
		ParamInfo	*cur;
		
	        if( $1 == NULL ) {
		    $$ = $3;
		} else {
		    cur = $1;
		    while( cur->next != NULL ) cur = cur->next;
		    cur->next = $3;
		    $3->next = NULL;
		    $$ = $1;
		}
	    }
	| /* nothing */
	    { $$ = NULL; }
	;
	
Y_ARRAY
	: T_LSQ_BRACKET T_RSQ_BRACKET 
	| T_LSQ_BRACKET T_NUM T_RSQ_BRACKET 
	;
	
Y_TYPENAME
	: Y_TYPENAME1
	    {
	    	$$ = malloc( sizeof( TypeInfo ) );
	        $$->typename = $1;
	    }
	| T_CONST Y_TYPENAME1
	    {
	    	$$ = malloc( sizeof( TypeInfo ) );
	        $$->typename = $2;
	    }
	;
	
Y_TYPENAME1
	: Y_NAME
	| Y_PREDEFINED_TYPE 
	| Y_STRUCT_OR_UNION 
	;
	
Y_STRUCT_OR_UNION
	: Y_STRUCT_OR_UNION_KEYWORD Y_NAME
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	;

Y_STRUCT_OR_UNION_KEYWORD
	: T_UNION
	| T_STRUCT
	;

Y_PREDEFINED_TYPE 
	: T_SIGNED
	| T_UNSIGNED
	| T_CHAR
	| T_SHORT
	| T_LONG
	| T_INT
	| T_FLOAT
	| T_DOUBLE
	| T_VOID
	| T_SIGNED T_CHAR
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	| T_UNSIGNED T_CHAR
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	| T_SIGNED T_SHORT
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	| T_UNSIGNED T_SHORT
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	| T_SIGNED T_INT
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	| T_UNSIGNED T_INT 
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	| T_SIGNED T_LONG
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	| T_UNSIGNED T_LONG
	    {
	        $$ = malloc( strlen( $1 ) + strlen( $2 ) + 2 );
		sprintf( $$, "%s %s", $1, $2 );
		free( $1 );
		free( $2 );
	    }
	;

Y_FNAME 
	: T_NAME
	| T_FNAME
	| T_STRING
	| T_CHAR
	| T_SHORT
	| T_INT
	| T_LONG
	| T_SIGNED
	| T_UNSIGNED
	| T_FLOAT
	| T_DOUBLE
	| T_VOID
	| T_CONST
	| T_STRUCT
	| T_UNION
	| T_NEAR
	| T_FAR
	| T_HUGE
	| T_CDECL
	| T_PASCAL
	| T_FORTRAN
	| T_SYSCALL
	| T_EXPORT     
	| T_LPAREN
	| T_RPAREN
	| T_EXTERN
	| T_LINE
	;	
	
Y_NAME
	: T_NAME
	| T_LINE
	;

%{

/*
**********************************************************************
*       Copyright by WATCOM International Corporation, 1987, 1993.   *
*       All rights reserved. No part of this software may be         *
*       reproduced in any form or by any means - graphic, electronic,*
*       mechanical or otherwise, including, without limitation,      *
*       photocopying, recording, taping or information storage and   *
*       retrieval systems - except with the written permission of    *
*       WATCOM International Corporation.                            *
**********************************************************************

cparse.y: A Grammar for PB generated sru scripts

	Modified	By              Reason
	--------	--		------
	02 Aug 94	Alex Brodsky	Created
	07 Sep 94	D. Wellstood	handle access specifiers, reformatted,
					handle shared variables
	07 Sep 94	D. Wellstood	support access headers
	09 Sep 94	D. Wellstood	support multiple variables on a line
					and single dimensional arrays
	13 Sep 94 	D. Wellstood	modifications for new scanner
	22 Sep 94	D. Wellstood	store REF info with the typename
					instead of setting a global
	03 Oct 94 	D. Wellstood	handle all kinds or array declarations
*/

#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "srusuprt.h"
#include "lexxer.h"
#include "types.h"
#include "mem.h"
#include "sruinter.h"
#include "list.h"
#include "error.h"

//#define DEBUGOUT( x )  printf( x );
#define DEBUGOUT( x )
%}


%union {
	char		*o_string;
	id_type		o_type;
	long		o_value;
	VarInfo		o_var;
	TypeInfo	o_typeinfo;
	ArrayInfo	o_array;
	List		*o_list;
}

%token	<o_type>	ST_FUNCTION
%token	<o_type>	ST_SUBROUTINE
%token	<o_type>	ST_ON
%token	<o_type>	ST_END
%token	<o_type>	ST_FROM
%token	<o_type>	ST_PUBLIC
%token	<o_type>	ST_PROTECTED
%token	<o_type>	ST_PRIVATE
%token	<o_type>	ST_COMMENT
%token  <o_type>	ST_PERIOD
%token  <o_type>	ST_COLON
%token  <o_type>	ST_COMMA
%token  <o_type>	ST_LSQ_BRACKET
%token  <o_type>	ST_RSQ_BRACKET
%token  <o_type>	ST_LBRACKET
%token  <o_type>	ST_RBRACKET
%token  <o_type>	ST_EQ
%token  <o_type>	ST_EXCLAM
%token  <o_type>	ST_DOLLAR

%token	<o_type>	ST_FORWARD
%token	<o_type>	ST_TYPE
%token	<o_type>	ST_GLOBAL
%token	<o_type>	ST_SHARED
%token	<o_type>	ST_PROTOTYPES
%token	<o_type>	ST_VARIABLES
%token	<o_type>	ST_NOTYPE
%token	<o_type>	ST_REF
%token	<o_type>	ST_LIBRARY
%token	<o_type>	ST_RETURN
%token	<o_type>	ST_TO

%token	<o_string>	ID_IDENTIFIER

%token 	<o_value> 	CT_INTEGER
%token 	<o_string> 	CT_STRING

%token 	<o_type> 	FI_EOF
%token 	<o_type> 	FI_EOL
%token 	<o_string> 	FI_COMMENT

%type	<o_type>	global_type
%type	<o_type>	sec_type
%type	<o_type>	access_specifier
%type	<o_typeinfo>	type
%type 	<o_var>		var_dec
%type 	<o_list>	variable_list
%type 	<o_string>	variable
%type	<o_array>	array_spec
%type	<o_array>	array_expr_list
%type	<o_array>	array_expr

%%

sru_statement
	: sru_stmt				
	    { ProcessStatement(); }
	| ST_COMMENT
	    { SetComment(); } 
	;
	
sru_stmt 
	: header				
	    { DEBUGOUT( "header\n" ); }
	| start_section 			
	    { DEBUGOUT( "start_section\n" ); }
	| end_section 				
	    { DEBUGOUT( "end_section\n" ); }
	| body	 				
	    { DEBUGOUT( "body\n" ); };
	| global_declare			
	    { DEBUGOUT( "global_declare\n" ); }
	| subprogram				
	    { 
	        DEBUGOUT( "subprogram\n" );
		UserCode(); 
	    };
	| end_subprogram			
	    { DEBUGOUT( "end_subprogram\n" ); }
	| end 					
	    { 
		DEBUGOUT( "end\n" );
	        YYABORT; /* finished */ 
	    }
	| error					
	    { 
	        DEBUGOUT( "error\n" );
		UserCode(); 
	    }
	| /* nothing */					
	    { DEBUGOUT( "nothing\n" ); }
	;

header
	: ST_DOLLAR ID_IDENTIFIER ST_PERIOD ID_IDENTIFIER	
	    { SetHeader( $2, $4 ); }
	| ST_DOLLAR ID_IDENTIFIER ST_PERIOD CT_INTEGER		
	    { 
	        char	buff[5];
	        itoa( $4 % 1000, buff, 10 );
	        SetHeader( $2, buff );
	    }
        
	| ST_DOLLAR ID_IDENTIFIER ST_PERIOD			
	    { SetHeader( $2, NULL ); }
	| ST_DOLLAR ID_IDENTIFIER				
	    { SetHeader( $2, NULL ); }
	;
	
start_section
	: ST_FORWARD sec_type 	 		
	    { StartSection( $1, $2 ); }
	| ST_GLOBAL global_type			
	    { StartSection( $1, $2 ); }
	| ST_TYPE sec_type 			
	    { StartSection( $1, $2 ); }
	| ST_SHARED ST_VARIABLES		
	    { StartSection( $1, $2 ); }
	;

global_type
	: ST_TYPE global_object			
	    { $$ = $1; }
	| /* nothing */ 	
	;
	
global_object
	: ID_IDENTIFIER ST_FROM ID_IDENTIFIER	
	    {}
	| /* nothing */					
	    {}
	;
	
end_section
	: ST_END ST_FORWARD 	 		
	    { EndSection(); }
	| ST_END ST_GLOBAL 			
	    { EndSection(); }
	| ST_END ST_TYPE 	 		
	    { EndSection(); }
	| ST_END ST_PROTOTYPES 	 		
	    { EndSection(); }
	| ST_END ST_VARIABLES 	 		
	    { EndSection(); }
	;
	 
body 
	: sp_prototype				
	    { StartSubProgram(); }
	| access_specifier ST_COLON
	    { SetDefaultAccess( $1 ); }
	| obj_variable		
	    {}
	| event_header 	
	    {}
	;
	
sp_prototype
	: function_header
	    {}
	| subroutine_header
	    {}
	;

obj_variable
	: access_specifier type variable_list
	    { AddDataMethod( $1, &($2), $3 ); }
	| type variable_list
	    { AddDataMethod2( &($1), $2 ); }
	;
	
variable_list
	: var_dec
	    { 
	        $$ = NewList( sizeof( VarInfo ) ); 
		AddToList( $$, &($1) );
	    }
	| variable_list ST_COMMA var_dec
	    { 
	        AddToList( $1, &($3) ); 
		$$ = $1;
	    }
	;
	
var_dec
  	: variable
	    { 
	        $$.name = MemStrDup( $1 );
		$$.flags = 0;
		$$.fake = FALSE;
	    }
	| variable array_spec
	    { 
	        $$.name = MemStrDup( $1 );
		$$.flags = VAR_ARRAY;
		$$.array = $2;
		$$.fake = TRUE;
		if( $$.array.flags & ARRAY_SIMPLE ) {
		    $$.fake = FALSE;
		} else if( $$.array.flags & ARRAY_MULTI_DIM ) {
		    Warning( ERR_MULTI_DIM_ARRAY, $1 );
		} else if( $$.array.flags & ARRAY_RANGE ) {
		    Warning( ERR_INDEX_DEFN, $1 );
		} else if( $$.array.flags & ARRAY_DYNAMIC ) {
		    Warning( ERR_DYNAMIC_ARRAY, $1 );
		}
	    }
	;

array_spec
	: ST_LSQ_BRACKET array_expr ST_RSQ_BRACKET
	    { $$ = $2; }
	| ST_LSQ_BRACKET array_expr_list ST_RSQ_BRACKET
	    { $$ = $2; }
	| ST_LSQ_BRACKET ST_RSQ_BRACKET
	    { $$.flags = ARRAY_DYNAMIC; }
	;
	
array_expr_list
	: array_expr ST_COMMA array_expr
	    { $$.flags = ARRAY_MULTI_DIM; }
	| array_expr_list ST_COMMA array_expr
	    { $$.flags = ARRAY_MULTI_DIM; }
	;
	
array_expr
	: CT_INTEGER
	    {
	        $$.flags = ARRAY_SIMPLE;
		$$.elemcnt = $1;
	    }
	| CT_INTEGER ST_TO CT_INTEGER
	    {
	        $$.flags = ARRAY_RANGE;
	    }
	;
	
access_specifier
	: ST_PROTECTED
	| ST_PRIVATE
	| ST_PUBLIC
	;

global_declare
	: ST_GLOBAL ID_IDENTIFIER ID_IDENTIFIER
	    {}
	;
	
end_subprogram
	: ST_END ST_FUNCTION 	 		
	    { EndSubProgram(); }
	| ST_END ST_SUBROUTINE 			
	    { EndSubProgram(); }
	| ST_END ST_ON 			
	    {}
	;

sec_type 
	: ST_PROTOTYPES				
	    { $$ = $1; }
	| ST_TYPE				
	    { $$ = $1; }
	| ST_VARIABLES				
	    { $$ = $1; }
	| /* nothing */					
	    { $$ = 0; }
	;

function_header
	: access_specifier ST_FUNCTION type ID_IDENTIFIER parm_list sp_modifier 	
	    { SetFunction( &($3), $4 ); }
	| ST_FUNCTION type ID_IDENTIFIER parm_list sp_modifier 	
	    { SetFunction( &($2), $3 ); }
	;
						
subroutine_header
	: access_specifier ST_SUBROUTINE ID_IDENTIFIER parm_list sp_modifier 	
	    { SetSubroutine( $3 ); }
	| ST_SUBROUTINE ID_IDENTIFIER parm_list sp_modifier 	
	    { SetSubroutine( $2 ); }
	;

event_header
	: ST_ON	ID_IDENTIFIER ST_PERIOD ID_IDENTIFIER 
	    {}
	| ST_ON	ID_IDENTIFIER 			
	    { RegisterEvent( $2 ); }
	;
	

sp_modifier
	: ST_LIBRARY CT_STRING	
	    {}
	| /* nothing */			
	    {}
	;
	
parm_list 
	: ST_LBRACKET parameters ST_RBRACKET			
	    { FiniParmList(); }
	;
	
parameters 
	: parms		
	    {}
	| /* nothing */	
	    {}
	;
	
parms
	: parm ST_COMMA parms	
	    {}
	| parm		
	    {}
	;
	
parm	
	: type ID_IDENTIFIER			
	    { AddParm( &($1), $2, NULL ); }
	| type ID_IDENTIFIER array_spec			
	    { AddParm( &($1), $2, &($3) ); }
	;
	
subprogram
	: call 					
	    {}
	| ID_IDENTIFIER ST_EQ expr 		
	    {}
	| ST_RETURN expr			
	     { SetReturn(); }
	;
	
call
	: ID_IDENTIFIER call_list
	    {}
	;
	
call_list
	: ST_LBRACKET call_args ST_RBRACKET 
	    {}
	;
	
call_args
	: args		
	    {}
	| /* nothing */	
	    {}
	;
	
args	
	: arg ST_COMMA args
	    {}
	| arg	
	    {}
	;
	
arg	
	: expr
	    {}
	;
	
expr
	: ST_LBRACKET expr ST_RBRACKET	
	    {}
	| variable	
	    {}
	| constant	
	    {}
	| ID_IDENTIFIER ST_EXCLAM 
	    {}
	| call		
	    {}
	;
	
constant
	: CT_STRING
	    {}
	| CT_INTEGER ST_PERIOD CT_INTEGER
	    {}
	| CT_INTEGER		
	    {}
	; 	

variable 
	: ID_IDENTIFIER	
	;
		
type
	: ST_REF ID_IDENTIFIER			
	    { 
	        $$.name = $2; 
	        $$.isref = TRUE; 
	    }
	| ID_IDENTIFIER				
	    { 
	        $$.name = $1; 
		$$.isref = FALSE;
	    }
	;
	
end 
	: FI_EOF
	    {}
	;
%%

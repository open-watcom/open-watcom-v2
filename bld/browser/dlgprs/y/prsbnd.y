%{  /* include files */
    #include <stdio.h>	// printf debugging
    #include <assert.h>
    
    #include "bind.h"

#ifdef __WATCOMC__    
    #pragma warning 17 5	// shut off the unreferenced goto warning
#endif
    
    extern Binding *	CurrBinding;
    
    #define YYPARSER BindingParser
%}

%token T_String
%token T_Number
%token T_Ident

%token T_Class

%token ';'
%token ','
%token '{'
%token '}'
%token '('
%token ')'
%token '@'

%%

goal
	: class_list
	;

class_list
	: class_def
	| class_def class_list
	;
	
class_def
	: class_line bind_block
	  {
	    /* if bindings were placed in a list, it would be done here */
	  }
	;
	
	
class_line
	: T_Class T_Ident 
	  { 
	    assert( CurrBinding == NULL );
	    
	    CurrBinding = new Binding( _scanner->getIdent( $2 ) );
	  }
	;
	
bind_block
	: '{' bind_list '}' ';'
	;
	
bind_list
	: binding
	| binding bind_list
	;
	
binding
	: T_Ident '(' T_Ident ',' absrel_rect ')' ';'
	  {
	    CurrBinding->addControl( _scanner->getIdent( $1 ), _scanner->getIdent( $3 ), $5 );
	  }
	;
	
absrel_rect
	: '(' absrel ',' absrel ',' absrel ',' absrel ')'
	  {
	    $$ = (YYSTYPE)CurrBinding->addAbsRelRect( Rect( $2, $4, $6, $8 ) );
	  }
	;
	
absrel
	: '@'
	  { $$ = Absolute; }
	| '#'
	  { $$ = Relative; }
	;
	
%%


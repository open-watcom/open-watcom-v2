%{
#include <stdio.h>
%}

%token	'+' '-' '*' '/' 'd'

%start	expr

%%

expr	:	term
		{ printf( "expr\t::= term\n" ); }
	;

term	:	term '+' factor
		{ printf( "term\t::= term '+' factor\n" ); }
	|	term '-' factor
		{ printf( "term\t::= term '-' factor\n" ); }
	|	factor
		{ printf( "term\t::= factor\n" ); }
	;

factor	:	factor '*' primary
		{ printf( "factor\t::= factor '*' primary\n" ); }
	|	factor '/' primary
		{ printf( "factor\t::= factor '/' primary\n" ); }
	|	primary
		{ printf( "factor\t::= primary\n" ); }
	;

primary	:	'd'
		{ printf( "primary\t::= 'd'\n" ); }
	|	'(' expr ')'
		{ printf( "primary\t::= '(' expr ')'\n" ); }
	;
%%
main()
{
  return( yyparse() );
}

yyerror(s)
  char *s;
{
  fprintf( stderr, "%s\n", s );
  exit( 1 );
}

int READ( fd, p, n )
  unsigned fd, n;
  char *p;
{
  return( read( fd, p, n ) );
}

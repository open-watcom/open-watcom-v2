%left '+'
%left '*'
%right '^'
%left ','

%%

expr	:	expr '+' expr
	|	expr '*' expr
	|	expr '^' expr
	|	expr ',' expr
	|	'-' expr	%prec '^'
	|	'(' expr ')'
	|	'a'
	;

%token	IF cond THEN ELSE OTHER

%start	stat

%%

stat	:	IF cond THEN stat ELSE stat
	|	IF cond THEN stat
	|	OTHER
	;

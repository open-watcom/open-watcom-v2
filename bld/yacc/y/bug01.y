/*
Found in COMP.COMPILERS (AFS 13-feb-92)

Do a "yacc -vd botch.y", and look at the y.output file.  If you see a
state which can shift "error", but has a default reduction, you have a
buggy yacc.

EXAMPLE of buggy y.output:

    state 2
        s :  oseq_    (1)
        oseq :  oseq_a 
        oseq :  oseq_error 

        error  shift 4
        a  shift 3
        .  reduce 1

It seems that it can shift to state 4 on a (synthesized) error token,
but in fact it will do the default reduction (reduction 1) before the
error-token can be synthesized by the parser.

A properly working yacc would produce the following:

    state 2
        s :  oseq_    (1)
        oseq :  oseq_a 
        oseq :  oseq_error 

        $end  reduce 1
        error  shift 4
        a  shift 3
        .  error

Notice that the default action is "error", not a reduction. The
error-action synthesizes the error-token, which can then be shifted.
*/
%start start
%token a

%%

start
	: oseq
	;
	
oseq
	: /* empty */
	| oseq a
	| oseq error
	;
	
%%

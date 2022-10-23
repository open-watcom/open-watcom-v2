%start  spec

%union {
    Symbol      *symbol;
    RegExp      *regexp;
    Token       *token;
    uchar       op;
    ExtOp       extop;
}

%token          CLOSESIZE   CLOSE       ID      CODE    RANGE   STRING

%type   <op>            CLOSE
%type   <op>            close
%type   <extop>         CLOSESIZE
%type   <symbol>        ID
%type   <token>         CODE
%type   <regexp>        RANGE   STRING
%type   <regexp>        rule    look    expr    diff    term    factor  primary

%%

spec    :
                { accept = 0;
                  spec = NULL; }
        |       spec rule
                { spec = spec? mkAlt(spec, $2) : $2; }
        |       spec decl
        ;

decl    :       ID '=' expr ';'
                { if($1->re)
                      Scanner_fatal(in, "sym already defined");
                  $1->re = $3; }
        ;

rule    :       expr look CODE
                { $$ = RegExp_new_RuleOp($1, $2, $3, accept++); }
        ;

look    :
                { $$ = RegExp_new_NullOp(); }
        |       '/' expr
                { $$ = $2; }
        ;

expr    :       diff
                { $$ = $1; }
        |       expr '|' diff
                { $$ =  mkAlt($1, $3); }
        ;

diff    :       term
                { $$ = $1; }
        |       diff '\\' term
                { $$ =  mkDiff($1, $3);
                  if(!$$)
                       Scanner_fatal(in, "can only difference char sets");
                }
        ;

term    :       factor
                { $$ = $1; }
        |       term factor
                { $$ = RegExp_new_CatOp($1, $2); }
        ;

factor  :       primary
                { $$ = $1; }
        |       primary close
                {
                    switch($2){
                    case '*':
                        $$ = mkAlt(RegExp_new_CloseOp($1), RegExp_new_NullOp());
                        break;
                    case '+':
                        $$ = RegExp_new_CloseOp($1);
                        break;
                    case '?':
                        $$ = mkAlt($1, RegExp_new_NullOp());
                        break;
                    }
                }
        |       primary CLOSESIZE
                {
                        $$ = RegExp_new_CloseVOp($1, $2.minsize, $2.maxsize);
                }
        ;

close   :       CLOSE
                { $$ = $1; }
        |       close CLOSE
                { $$ = ($1 == $2) ? $1 : '*'; }
        ;

primary :       ID
                { if(!$1->re)
                      Scanner_fatal(in, "can't find symbol");
                  $$ = $1->re; }
        |       RANGE
                { $$ = $1; }
        |       STRING
                { $$ = $1; }
        |       '(' expr ')'
                { $$ = $2; }
        ;

%%

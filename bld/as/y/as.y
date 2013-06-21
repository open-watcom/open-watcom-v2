%{
    #include "as.h"
    #include "lexyacc.h"
    #include "ytab.h"
    
    typedef uint_16         YYACTIONBASETYPE;
    typedef uint_16         YYACTIONTYPE;
    typedef uint_16         YYBITBASETYPE;
    typedef uint_8          YYBITTYPE;
    typedef uint_8          YYPLENTYPE;
    typedef uint_16         YYPLHSTYPE;
    
    typedef unsigned        YYTOKENTYPE;

#ifdef __WATCOMC__    
#pragma disable_message( 118 ); // no more unreferenced label warning
#pragma warning 17 5    // shut off the unreferenced goto warning
#pragma warning 389 5   // shut off the integral truncated warning
#pragma warning 373 5   // get rid of defined but not referenced warning
#endif

    instruction             *AsCurrIns;
    directive_t             *AsCurrDir;
%}

%union {
    char            *str;
    void            *null;
    int_32          val;
    double          fval;
    sym_handle      sym;
    reg             reg;
    expr_tree       *tree;      /* node in an expression tree */
    instruction     *ins;
    ins_operand     *op;
    asm_reloc_type  rtype;
    directive_t     *dir;
:segment _STANDALONE_
    fileinfo        *file;
:endsegment
}

%token  <null>  T_ERROR
:segment _STANDALONE_
%token  <file>  T_FILE_SWITCH   /* indicating a #line; info in <file> */
:endsegment

%token  <sym>   T_DIRECTIVE
%token  <str>   T_DIRECTIVE_OPERAND
%token  <sym>   T_OPCODE
%token  <sym>   T_IDENTIFIER

/* registers */
%token  <reg>   T_REGISTER

/* manifest constants */

%token  <val>   T_INTEGER_CONST
%token  <str>   T_STRING_CONST
%token  <fval>  T_FLOAT_CONST
:segment AS_PPC
%token  <val>   T_BI_OFFSET
:endsegment

/* punctuation and special symbols */

:segment _STANDALONE_
%token          T_NEWLINE
:endsegment
%token          T_COMMA
%token          T_COLON
%token          T_SEMICOLON
%token          T_LEFT_PAREN
%token          T_RIGHT_PAREN
%token  <rtype> T_RELOC_MODIFIER
%token  <val>   T_NUMERIC_LABEL
%token  <val>   T_NUMLABEL_REF

/* arithmetic stuff */

%left   <tree>  T_OR
%left   <tree>  T_XOR
%left   <tree>  T_AND
%left   <tree>  T_SHIFT_LEFT T_SHIFT_RIGHT
%left   <tree>  T_PLUS T_MINUS
%left   <tree>  T_TIMES T_DIVIDE T_MOD
%nonassoc <tree> T_UNARY_MINUS T_NOT T_UNARY_PLUS

:segment _STANDALONE_
%type   <null>  asm_file
:endsegment
%type   <null>  asm_line
%type   <sym>   label
%type   <null>  statement
%type   <null>  directive
%type   <dir>   dir_name
%type   <null>  dir_operands
%type   <null>  dir_op_tail
%type   <null>  dir_op
%type   <null>  dir_ident
%type   <tree>  plus_minus_num_expr
%type   <tree>  num_expr_plus
%type   <null>  dir_ident
%type   <tree>  num_expr
%type   <ins>   instruction
%type   <null>  operand_list
%type   <null>  operand_tail
%type   <op>    operand
%type   <tree>  expression
%type   <reg>   reg_indirect
%type   <op>    mem_reference
:segment AS_PPC
%type   <op>    field_BI
%type   <val>   bi_crf
:endsegment
%type   <ins>   opcode
%type   <op>    ins_operand
%type   <sym>   identifier

%%

:segment _STANDALONE_
asm_file        : asm_line                                      { }
                | asm_file asm_line                             { }
                ;

asm_line        : label statement more_stmts T_NEWLINE          { CurrLineno++; }
                | error T_NEWLINE                               { CurrLineno++; yyerrok; }
                | T_FILE_SWITCH                                 {   /* #line directive */
                                                                    CurrLineno = $1->line;
                                                                    MemFree( CurrFilename );
                                                                    CurrFilename = AsStrdup( $1->name );
                                                                    MemFree( $1 );
                                                                }
:elsesegment
asm_line        : label statement more_stmts                    { }
:endsegment
                ;

label           : /* nothing */                                 { }
                | T_IDENTIFIER T_COLON label                    { ObjEmitLabel( $1 ); }
                | T_NUMERIC_LABEL label                         { ObjEmitNumericLabel( $1 ); }
                ;

statement       : /* nothing */                                 { }
                | directive                                     { }
                | instruction                                   { }
                ;
                
more_stmts      : /* nothing */                                 { }
                | T_SEMICOLON label statement more_stmts        { }
                ;

directive       : dir_name dir_operands                         {
                                                                    bool okay;
                                                                    
                                                                    okay = DirParse( $1 );
                                                                    DirDestroy( $1 );
                                                                    AsCurrDir = NULL;
                                                                    if( !okay ) YYABORT;
                                                                }
                ;
 
dir_name        : T_DIRECTIVE                                   { $$ = DirCreate( $1 ); AsCurrDir = $$; }
                ;
                
dir_operands    : /* nothing */                                 { }
                | dir_op dir_op_tail                            { }
                ;

dir_op_tail     : /* nothing */                                 { }
                | T_COMMA dir_op dir_op_tail                    { }
                ;

num_expr_plus   : /* nothing */                                 { $$ = ETConst( 0 ); }
                | num_expr T_PLUS                               { $$ = $1; }
                ;

plus_minus_num_expr : /* nothing */                             { $$ = ETConst( 0 ); }
                | T_PLUS num_expr                               { $$ = $2; }
                | T_MINUS num_expr                              { $$ = ETUnary( ET_UNARY_MINUS, $2 ); }
                ;

dir_ident       : num_expr_plus identifier plus_minus_num_expr  { DirAddOperand( AsCurrDir, DirOpIdentifier( ASM_RELOC_UNSPECIFIED, $2, ETBinary( ET_PLUS, $1, $3 ), 1 ) ); }
                | num_expr_plus T_RELOC_MODIFIER identifier plus_minus_num_expr
                                                                { DirAddOperand( AsCurrDir, DirOpIdentifier( $2, $3, ETBinary( ET_PLUS, $1, $4 ), 1 ) ); }
                | num_expr_plus T_NUMLABEL_REF plus_minus_num_expr
                                                                { DirAddOperand( AsCurrDir, DirOpNumLabelRef( ASM_RELOC_UNSPECIFIED, $2, ETBinary( ET_PLUS, $1, $3 ), 1 ) ); }
                | num_expr_plus T_RELOC_MODIFIER T_NUMLABEL_REF plus_minus_num_expr
                                                                { DirAddOperand( AsCurrDir, DirOpNumLabelRef( $2, $3, ETBinary( ET_PLUS, $1, $4 ), 1 ) ); }
                ;

dir_op          : num_expr                                      { DirAddOperand( AsCurrDir, DirOpNumber( $1 ) ); }
                | T_DIRECTIVE_OPERAND                           { DirAddOperand( AsCurrDir, DirOpLine( $1 ) ); }
                | dir_ident                                     { }
                | T_STRING_CONST                                { DirAddOperand( AsCurrDir, DirOpString( $1 ) ); }
                | num_expr T_COLON num_expr                     { DirAddOperand( AsCurrDir, DirOpRepeat( $1, $3 ) ); }
                ;

num_expr        : num_expr T_TIMES num_expr                     { $$ = ETBinary( ET_TIMES, $1, $3 ); }
                | num_expr T_DIVIDE num_expr                    { $$ = ETBinary( ET_DIVIDE, $1, $3 ); }
                | num_expr T_MOD num_expr                       { $$ = ETBinary( ET_MOD, $1, $3 ); }
                | num_expr T_PLUS num_expr                      { $$ = ETBinary( ET_PLUS, $1, $3 ); }
                | num_expr T_MINUS num_expr                     { $$ = ETBinary( ET_MINUS, $1, $3 ); }
                | num_expr T_SHIFT_LEFT num_expr                { $$ = ETBinary( ET_SHIFT_L, $1, $3 ); }
                | num_expr T_SHIFT_RIGHT num_expr               { $$ = ETBinary( ET_SHIFT_R, $1, $3 ); }
                | num_expr T_AND num_expr                       { $$ = ETBinary( ET_AND, $1, $3 ); }
                | num_expr T_OR num_expr                        { $$ = ETBinary( ET_OR, $1, $3 ); }
                | num_expr T_XOR num_expr                       { $$ = ETBinary( ET_XOR, $1, $3 ); }
                | num_expr error                                { /* Pass this on so that DirOpXXX will free the memory */ }
                | T_MINUS %prec T_UNARY_MINUS num_expr          { $$ = ETUnary( ET_UNARY_MINUS, $2 ); }
                | T_PLUS %prec T_UNARY_PLUS num_expr            { $$ = $2; }
                | T_NOT num_expr                                { $$ = ETUnary( ET_NOT, $2 ); }
                | T_LEFT_PAREN num_expr T_RIGHT_PAREN           { $$ = ETUnary( ET_PARENS, $2 ); }
                | T_INTEGER_CONST                               { $$ = ETConst( $1 ); }
                | T_FLOAT_CONST                                 { $$ = ETFPConst( $1 ); }
                ;

instruction     : opcode operand_list                           { InsEmit( $1 ); InsDestroy( $1 ); AsCurrIns = NULL; }
                ;
                
opcode          : T_OPCODE                                      {
                                                                    $$ = InsCreate( $1 );
                                                                    if( AsCurrIns ) InsDestroy( AsCurrIns );
                                                                    AsCurrIns = $$;
                                                                }
                ;

operand_list    : /* nothing */                                 { }
                | ins_operand operand_tail                      { }
                ;
                
operand_tail    : /* nothing */                                 { }
                | T_COMMA ins_operand operand_tail              { }
                ;
                
ins_operand     : operand                                       { InsAddOperand( AsCurrIns, $1 ); }
                ;
                
operand         : expression                                    { $$ = OpImmed( $1 ); }
                | T_REGISTER                                    { $$ = OpRegister( $1 ); }
                | mem_reference                                 { $$ = $1; }
:segment AS_PPC
                | field_BI                                      { $$ = $1; }
:endsegment
                ;

mem_reference   : expression reg_indirect                       { $$ = OpRegIndirect( $2, $1 ); }
                | reg_indirect                                  { $$ = OpRegIndirect( $1, NULL ); }
                ;

:segment AS_PPC
field_BI        : bi_crf T_PLUS T_BI_OFFSET                     { $$ = OpBI( $1, $3 ); }
                | T_BI_OFFSET                                   { $$ = OpBI( 0, $1 ); }
                ;

bi_crf          : T_REGISTER                                    { $$ = RegCrfToBI( $1 ); }
                ;
:endsegment

reg_indirect    : T_LEFT_PAREN T_REGISTER T_RIGHT_PAREN         { $$ = $2; }
                ;
                
expression      : expression T_TIMES expression                 { $$ = ETBinary( ET_TIMES, $1, $3 ); }
                | expression T_DIVIDE expression                { $$ = ETBinary( ET_DIVIDE, $1, $3 ); }
                | expression T_MOD expression                   { $$ = ETBinary( ET_MOD, $1, $3 ); }
                | expression T_PLUS expression                  { $$ = ETBinary( ET_PLUS, $1, $3 ); }
                | expression T_MINUS expression                 { $$ = ETBinary( ET_MINUS, $1, $3 ); }
                | expression T_SHIFT_LEFT expression            { $$ = ETBinary( ET_SHIFT_L, $1, $3 ); }
                | expression T_SHIFT_RIGHT expression           { $$ = ETBinary( ET_SHIFT_R, $1, $3 ); }
                | expression T_AND expression                   { $$ = ETBinary( ET_AND, $1, $3 ); }
                | expression T_OR expression                    { $$ = ETBinary( ET_OR, $1, $3 ); }
                | expression T_XOR expression                   { $$ = ETBinary( ET_XOR, $1, $3 ); }
                | expression error                              { /* Pass this on so that OpXXX will free the memory */ }
                | T_MINUS %prec T_UNARY_MINUS expression        { $$ = ETUnary( ET_UNARY_MINUS, $2 ); }
                | T_PLUS %prec T_UNARY_PLUS expression          { $$ = $2; }
                | T_NOT expression                              { $$ = ETUnary( ET_NOT, $2 ); }
                | T_LEFT_PAREN expression T_RIGHT_PAREN         { $$ = ETUnary( ET_PARENS, $2 ); }
                | T_INTEGER_CONST                               { $$ = ETConst( $1 ); }
                | T_RELOC_MODIFIER identifier                   { $$ = ETReloc( $1, $2 ); }
                | identifier                                    { $$ = ETReloc( ASM_RELOC_UNSPECIFIED, $1 ); }
                | T_RELOC_MODIFIER T_NUMLABEL_REF               { $$ = ETNumLabelReloc( $1, $2 ); };
                | T_NUMLABEL_REF                                { $$ = ETNumLabelReloc( ASM_RELOC_UNSPECIFIED, $1 ); };
                ;
                
identifier      : T_IDENTIFIER                                  { $$ = $1; }
:segment _STANDALONE_
                | T_DIRECTIVE                                   {   if( !SymIsSectionSymbol( $1 ) ) {
                                                                        Error( INVALID_RELOC_SYM, SymName( $1 ) );
                                                                        YYERROR;
                                                                    }
                                                                    // otherwise
                                                                    $$ = $1;
                                                                }
:endsegment
                ;

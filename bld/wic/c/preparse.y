%{
// File generated from preparse.y using yaccpre.bat

/*
preparse.y: A Grammar for preprocessor parsing
*/

#include <stdlib.h>
#include <stdio.h>
#include "wic.h"

static pToken _lastDefineTok;

#define PRESTYPE ParseUnion

#ifndef __SMALL__
#define __SMALL__ 0            // To avoid warnings in MKS yacc
#endif

#pragma disable_message(118)
%}

/* Tokens for used by parsers.  NOTE: If any values are modified or added,
   make sure the same is done for cparse.y and token.h */

%token <token>  Y_EOF                   300  /* MKS yacc will not allow it to be 0*/
%token <token>  Y_EXCLAMATION           301
%token <token>  Y_NE                    302
%token <token>  Y_POUND                 303
%token <token>  Y_POUND_POUND           304
%token <token>  Y_AND                   305
%token <token>  Y_AND_AND               306
%token <token>  Y_AND_EQUAL             307
%token <token>  Y_LEFT_PAREN            308
%token <token>  Y_RIGHT_PAREN           309
%token <token>  Y_TIMES                 310
%token <token>  Y_TIMES_EQUAL           311
%token <token>  Y_PLUS                  312
%token <token>  Y_PLUS_PLUS             313
%token <token>  Y_PLUS_EQUAL            314
%token <token>  Y_COMMA                 315
%token <token>  Y_MINUS                 316
%token <token>  Y_MINUS_MINUS           317
%token <token>  Y_MINUS_EQUAL           318
%token <token>  Y_ARROW                 319
%token <token>  Y_DOT                   320
%token <token>  Y_DOT_DOT_DOT           321
%token <token>  Y_DIVIDE                322
%token <token>  Y_DIVIDE_EQUAL          323
%token <token>  Y_COLON                 324
%token <token>  Y_SEG_OP                325
%token <token>  Y_SEMICOLON             326
%token <token>  Y_LT                    327
%token <token>  Y_LSHIFT                328
%token <token>  Y_LSHIFT_EQUAL          329
%token <token>  Y_LE                    330
%token <token>  Y_EQUAL                 331
%token <token>  Y_EQ                    332
%token <token>  Y_GT                    333
%token <token>  Y_GE                    334
%token <token>  Y_RSHIFT                335
%token <token>  Y_RSHIFT_EQUAL          336
%token <token>  Y_QUESTION              337
%token <token>  Y_LEFT_BRACKET          338
%token <token>  Y_RIGHT_BRACKET         339
%token <token>  Y_XOR                   340
%token <token>  Y_XOR_EQUAL             341
%token <token>  Y___BASED               342
%token <token>  Y___CDECL               343
%token <token>  Y___EXPORT              344
%token <token>  Y___FAR                 345
%token <token>  Y___FAR16               346
%token <token>  Y___FORTRAN             347
%token <token>  Y___HUGE                348
%token <token>  Y___INTERRUPT           349
%token <token>  Y___LOADDS              350
%token <token>  Y___NEAR                351
%token <token>  Y___PASCAL              352
%token <token>  Y___PRAGMA              353
%token <token>  Y___SAVEREGS            354
%token <token>  Y___SEGMENT             355
%token <token>  Y___SEGNAME             356
%token <token>  Y___SELF                357
%token <token>  Y___STDCALL             358
%token <token>  Y__PACKED               359
%token <token>  Y__SEG16                360
%token <token>  Y__SYSCALL              361
%token <token>  Y_AUTO                  362
%token <token>  Y_CHAR                  363
%token <token>  Y_CONST                 364
%token <token>  Y_DOUBLE                365
%token <token>  Y_ELSE                  366
%token <token>  Y_ENUM                  367
%token <token>  Y_EXTERN                368
%token <token>  Y_FLOAT                 369
%token <token>  Y_INT                   370
%token <token>  Y_LONG                  371
%token <token>  Y_REGISTER              372
%token <token>  Y_SHORT                 373
%token <token>  Y_SIGNED                374
%token <token>  Y_SIZEOF                375
%token <token>  Y_STATIC                376
%token <token>  Y_STRUCT                377
%token <token>  Y_TYPEDEF               378
%token <token>  Y_UNION                 379
%token <token>  Y_UNSIGNED              380
%token <token>  Y_VOID                  381
%token <token>  Y_VOLATILE              382
%token <token>  Y_LEFT_BRACE            383
%token <token>  Y_OR                    384
%token <token>  Y_OR_EQUAL              385
%token <token>  Y_OR_OR                 386
%token <token>  Y_RIGHT_BRACE           387
%token <token>  Y_TILDE                 388
%token <token>  Y_ID                    389
%token <token>  Y_STRING                390
%token <token>  Y_INCLUDE_FILE_NAME     391
%token <token>  Y_TYPEDEF_NAME          392
%token <token>  Y_NUMBER                393
%token <token>  Y_PERCENT               394
%token <token>  Y_PERCENT_EQUAL         395
%token <token>  Y_DEFINED               396

%token <token>  Y_PRE_COMMENT           500
%token <token>  Y_PRE_NULL              501  /* # followed by newline */
%token <token>  Y_PRE_NEWLINE           502
%token <token>  Y_PRE_DEFINE            503
%token <token>  Y_PRE_ELIF              504
%token <token>  Y_PRE_ELSE              505
%token <token>  Y_PRE_ENDIF             506
%token <token>  Y_PRE_ERROR             507
%token <token>  Y_PRE_IF                508
%token <token>  Y_PRE_IFDEF             509
%token <token>  Y_PRE_IFNDEF            510
%token <token>  Y_PRE_INCLUDE           511
%token <token>  Y_PRE_LINE              512
%token <token>  Y_PRE_PRAGMA            513
%token <token>  Y_PRE_UNDEF             514
%token <token>  Y_PRE_SPECIAL_LEFT_PAREN 515


/*======================== Local Symbols =================================*/
/* Symbols specific to preparse.y                                         */
/*========================================================================*/
%type <tree> control-line
%type <tree> define-directive
%type <tree> directive
%type <token> file-name
%type <tree> pre-define-keyword
%type <tree> define-directive
%type <token> line-end
%type <data> no-expand-next-token-action
/*========================================================================*/

/*======================== EXPRESSION Symbols ============================*/
/* This part is common to both preparse.y and cparse.y                    */
/*========================================================================*/
%type <tree> primary-expression
%type <tree> postfix-expression
%type <tree> expression-list-opt
%type <tree> expression-list
%type <tree> unary-expression
%type <label> unary-operator
%type <tree> cast-expression
%type <tree> multiplicative-expression
%type <tree> additive-expression
%type <tree> shift-expression
%type <tree> relational-expression
%type <tree> equality-expression
%type <tree> and-expression
%type <tree> exclusive-or-expression
%type <tree> inclusive-or-expression
%type <tree> logical-and-expression
%type <tree> logical-or-expression
%type <tree> conditional-expression
%type <tree> assignment-expression
%type <tree> expression
%type <tree> constant-expression
%type <token> declarator-id
%type <dinfo> decl-specs
%type <dinfo> non-type-decl-specs
%type <dinfo> non-type-decl-spec
%type <dinfo> type-spec
%type <dinfo> typedef-name
%type <dinfo> maybe-type-decl-specs
%type <dinfo> storage-class-spec
%type <dinfo> scalar-type-spec
%type <dsinfo> struct-or-union-spec
%type <token> struct-id
%type <token> struct-or-union
%type <dsbody> struct-or-union-body
%type <declList> struct-decl-list
%type <dinfo> struct-decl
%type <dclrList> struct-declarator-list
%type <dclr> struct-declarator
%type <declEnum> enum-spec
%type <enumList> enum-list
%type <enumList> enum-list-collect
%type <enumElem> comma-and-enumerator
%type <enumElem> enumerator
%type <dinfo> cv-qualifier
%type <tree> type-name
%type <tree> literal
%type <tree> strings
%type <tree> single-string
%type <dclr> declarator
%type <dclr> actual-declarator
%type <dclr> declarator-no-id
%type <dclr> actual-declarator-no-id
%type <dclrPtr> ptr-modifier
%type <token> mem-modifier
%type <token> pragma-modifier
%type <flag> cv-qualifiers-opt
%type <declList> abstract-args
%type <declList> arg-decl-list
%type <dinfo> arg-decl-elem
%type <dinfo> dot-dot-dot-decl
%type <token> identifier

/*==========================================================================*/
%%

directive
    : control-line
    {
        if ($1 != NULL) {
            addSLListElem(g_dirList, $1);
        }
        PREACCEPT;
    }
    | error
        { recoverError(); PREACCEPT; }
    ;

control-line
    : Y_PRE_NULL line-end
        { $$ = NULL; }

    | Y_PRE_ELSE line-end
        { $$ = createCTreeRoot(createConstr1Label(LABCT_PRE_ELSE, $1)); }

    | Y_PRE_ENDIF line-end
        { $$ = createCTreeRoot(createConstr1Label(LABCT_PRE_ENDIF, $1)); }

    | Y_PRE_IF constant-expression line-end
        {
            if (g_opt.targetLang == TLT_FORTRAN) {
                reportError(CERR_PREDIR_NOT_SUPPORTED, "#if");
            }
            $$ = createCTree1(createConstr1Label(LABCT_PRE_IF, $1), $2);
        }

    | Y_PRE_ELIF constant-expression line-end
        {
            if (g_opt.targetLang == TLT_FORTRAN) {
                reportError(CERR_PREDIR_NOT_SUPPORTED, "#elif");
            }
            $$ = createCTree1(createConstr1Label(LABCT_PRE_ELIF, $1), $2);
        }

    | Y_PRE_INCLUDE file-name line-end
        {
            $$ = createCTreeRoot(createConstr2Label(LABCT_PRE_INCLUDE, $1, $2));
            pushTokFile(getTokenIdName($2));
        }
    | pre-define-keyword define-directive
      { $$ = $2; }

    | Y_PRE_IFDEF no-expand-next-token-action Y_ID line-end
        { $$ = createCTreeRoot(createConstr2Label(LABCT_PRE_IFDEF, $1, $3)); }

    | Y_PRE_IFNDEF no-expand-next-token-action Y_ID line-end
        { $$ = createCTreeRoot(createConstr2Label(LABCT_PRE_IFNDEF, $1, $3)); }

    | Y_PRE_PRAGMA
        {
            reportError(CERR_PREDIR_NOT_SUPPORTED, "#pragma");
            $$ = eatPreDirList($1);
        }

    | Y_PRE_ERROR
        {
            if (g_opt.targetLang == TLT_FORTRAN) {
                reportError(CERR_PREDIR_NOT_SUPPORTED, "#error");
            }
            $$ = eatPreDirList($1);
        }

    | Y_PRE_LINE
        {
            if (g_opt.targetLang == TLT_FORTRAN) {
                reportError(CERR_PREDIR_NOT_SUPPORTED, "#line");
            }
            $$ = eatPreDirList($1);
        }

    | Y_PRE_UNDEF no-expand-next-token-action
        { $$ = eatPreDirList($1); }

    | Y_PRE_NEWLINE
        { $$ = NULL; }
    ;

file-name
    : Y_STRING
        { $$ = $1; }
    | Y_INCLUDE_FILE_NAME
        { $$ = $1; }
    ;

pre-define-keyword
    : Y_PRE_DEFINE
        {
            expandNextToken = 0;
            expandThisLineHideErrors = 1;
            _lastDefineTok = $1;
        }
    ;

define-directive
    : identifier constant-expression line-end
        { $$ = createEnumFromDefine(_lastDefineTok, $1, $2); }
    | identifier Y_PRE_SPECIAL_LEFT_PAREN
        { expandThisLine = 0; $$ = createDefineMacroCTree(); }
    | identifier error
        { $$ = createDefineMacroCTree(); }
    | error
        { recoverError(); $$ = NULL; }
    ;

line-end
    : Y_PRE_NEWLINE
        {
            $$ = $1;
        }
    ;

no-expand-next-token-action
    :  {  expandNextToken = 0; }
    ;

postfix-expression
    : Y_DEFINED Y_LEFT_PAREN no-expand-next-token-action identifier Y_RIGHT_PAREN
        { $$ = createCTreeRoot(
                createConstr4Label(LABCT_DEFINED, $1, $2, $4, $5)); }
    ;

/*======================== EXPRESSIONS ===================================*/
/* This part is common to both preparse.y and cparse.y                    */
/*========================================================================*/

primary-expression
    : literal
    | Y_LEFT_PAREN expression Y_RIGHT_PAREN
        { $$ = createCTree1(createConstr2Label(LABCT_PAREN_EXPR, $1, $3), $2); }
    | identifier
        { $$ = createCTreeRoot(createTokenLabel($1)); }
    ;

postfix-expression
    : primary-expression
    | postfix-expression Y_LEFT_BRACKET expression Y_RIGHT_BRACKET
        { $$ = createCTree2(createConstr2Label(LABCT_INDEX, $2, $4), $1,  $3); }
    | postfix-expression Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN
        { $$ = createCTree2(createConstr2Label(LABCT_CALL, $2, $4), $1,  $3); }
    | postfix-expression Y_DOT identifier
        { $$ = createCTree2(createConstr1Label(LABCT_DOT, $2), $1,
                            createCTreeRoot(createTokenLabel($3))); }
    | postfix-expression Y_ARROW identifier
        { $$ = createCTree2(createConstr1Label(LABCT_ARROW, $2), $1,
                            createCTreeRoot(createTokenLabel($3))); }
    ;

expression-list-opt
    : /* Nothing */
        { $$ = createNULLCTree(); }
    | expression-list
    ;

expression-list
    : assignment-expression
    | expression-list Y_COMMA assignment-expression
        { $$ = createCTree2(createConstr1Label(LABCT_EXPR_LIST, $2), $1, $3); }
    ;

unary-expression
    : postfix-expression
    | unary-operator cast-expression
        { $$ = createCTree1( $1, $2 ); }
    | Y_SIZEOF unary-expression
        { $$ = createCTree1(createConstr1Label(LABCT_SIZEOF_EXPR, $1), $2); }
    | Y_SIZEOF Y_LEFT_PAREN type-name Y_RIGHT_PAREN
        { $$ = createCTree1(createConstr3Label(LABCT_SIZEOF_TYPE, $1, $2, $4), $3); }
    ;

unary-operator
    : Y_TIMES
        { $$ = createConstr1Label(LABCT_VALUE_AT_ADDR, $1); }
    | Y_AND
        { $$ = createConstr1Label(LABCT_ADDR_OF_VALUE, $1); }
    | Y_PLUS
        { $$ = createConstr1Label(LABCT_UNARY_PLUS, $1); }
    | Y_MINUS
        { $$ = createConstr1Label(LABCT_UNARY_MINUS, $1); }
    | Y_EXCLAMATION
        { $$ = createConstr1Label(LABCT_EXCLAMATION, $1); }
    | Y_TILDE
        { $$ = createConstr1Label(LABCT_TILDE, $1); }
    ;

cast-expression
    : unary-expression
    | Y_LEFT_PAREN type-name Y_RIGHT_PAREN cast-expression
        { $$ = createCTree2(createConstr2Label(LABCT_CAST_EXPR, $1, $3), $2, $4); }
    ;

multiplicative-expression
    : cast-expression
    | multiplicative-expression Y_TIMES cast-expression
        { $$ = createCTree2(createConstr1Label(LABCT_TIMES, $2), $1, $3); }
    | multiplicative-expression Y_DIVIDE cast-expression
        { $$ = createCTree2(createConstr1Label(LABCT_DIVIDE, $2), $1, $3); }
    | multiplicative-expression Y_PERCENT cast-expression
        { $$ = createCTree2(createConstr1Label(LABCT_PERCENT, $2), $1, $3); }
    ;

additive-expression
    : multiplicative-expression
    | additive-expression Y_PLUS multiplicative-expression
        { $$ = createCTree2(createConstr1Label(LABCT_PLUS, $2), $1, $3); }
    | additive-expression Y_MINUS multiplicative-expression
        { $$ = createCTree2(createConstr1Label(LABCT_MINUS, $2), $1, $3); }
    ;

shift-expression
    : additive-expression
    | shift-expression Y_RSHIFT additive-expression
        { $$ = createCTree2(createConstr1Label(LABCT_RSHIFT, $2), $1, $3); }
    | shift-expression Y_LSHIFT additive-expression
        { $$ = createCTree2(createConstr1Label(LABCT_LSHIFT, $2), $1, $3); }
    ;

relational-expression
    : shift-expression
    | relational-expression Y_LT shift-expression
        { $$ = createCTree2(createConstr1Label(LABCT_LT, $2), $1, $3); }
    | relational-expression Y_LE shift-expression
        { $$ = createCTree2(createConstr1Label(LABCT_LE, $2), $1, $3); }
    | relational-expression Y_GT shift-expression
        { $$ = createCTree2(createConstr1Label(LABCT_GT, $2), $1, $3); }
    | relational-expression Y_GE shift-expression
        { $$ = createCTree2(createConstr1Label(LABCT_GE, $2), $1, $3); }
    ;

equality-expression
    : relational-expression
    | equality-expression Y_EQ relational-expression
        { $$ = createCTree2(createConstr1Label(LABCT_EQ, $2), $1, $3); }
    | equality-expression Y_NE relational-expression
        { $$ = createCTree2(createConstr1Label(LABCT_NE, $2), $1, $3); }
    ;

and-expression
    : equality-expression
    | and-expression Y_AND equality-expression
        { $$ = createCTree2(createConstr1Label(LABCT_AND, $2), $1, $3); }
    ;

exclusive-or-expression
    : and-expression
    | exclusive-or-expression Y_XOR and-expression
        { $$ = createCTree2(createConstr1Label(LABCT_XOR, $2), $1, $3); }
    ;

inclusive-or-expression
    : exclusive-or-expression
    | inclusive-or-expression Y_OR exclusive-or-expression
        { $$ = createCTree2(createConstr1Label(LABCT_OR, $2), $1, $3); }
    ;

logical-and-expression
    : inclusive-or-expression
    | logical-and-expression Y_AND_AND inclusive-or-expression
        { $$ = createCTree2(createConstr1Label(LABCT_AND_AND, $2), $1, $3); }
    ;

logical-or-expression
    : logical-and-expression
    | logical-or-expression Y_OR_OR logical-and-expression
        { $$ = createCTree2(createConstr1Label(LABCT_OR_OR, $2), $1, $3); }
    ;

conditional-expression
    : logical-or-expression
    | logical-or-expression Y_QUESTION expression Y_COLON assignment-expression
        {
            $$ = createCTree2(createConstr1Label(LABCT_QUESTION, $2), $1,
                        createCTree2(createConstr1Label(LABCT_COLON, $4), $3, $5));
        }
    ;

assignment-expression
    : conditional-expression
    ;

expression
    : assignment-expression
    | expression Y_COMMA assignment-expression
        { $$ = createCTree2(createConstr1Label(LABCT_EXPR_SEPARATOR, $2), $1, $3); }
    ;

constant-expression
    : conditional-expression
    ;

decl-specs
    : non-type-decl-specs type-spec maybe-type-decl-specs
        { $$ = combine2DeclInfo(combine2DeclInfo($1, $2), $3); }
    | non-type-decl-specs type-spec
        { $$ = combine2DeclInfo($1, $2); }
    |                     type-spec maybe-type-decl-specs
        { $$ = combine2DeclInfo($1, $2); }
    |                     type-spec
    | non-type-decl-specs
    ;

non-type-decl-specs
    : non-type-decl-spec
    | non-type-decl-specs non-type-decl-spec
        { $$ = combine2DeclInfo($1, $2); }
    ;

non-type-decl-spec
    : storage-class-spec
    | cv-qualifier
    ;

type-spec
    : scalar-type-spec
    | typedef-name
    | struct-or-union-spec
        { $$ = createDeclInfoSTRUCT($1); }
    | enum-spec
        { $$ = createDeclInfoENUM($1); }
    ;

typedef-name
    : Y_TYPEDEF_NAME
        { $$ = dupDeclInfo($1->data->repr.pTypeDecl, $1->pos); zapToken($1); }
    ;


maybe-type-decl-specs
    : non-type-decl-spec
    | scalar-type-spec
    | maybe-type-decl-specs non-type-decl-spec
        { $$ = combine2DeclInfo($1, $2); }
    | maybe-type-decl-specs scalar-type-spec
        { $$ = combine2DeclInfo($1, $2); }
    ;

storage-class-spec
    : Y_AUTO
        { $$ = createStgClassDeclInfo(STG_AUTO, $1); }
    | Y_REGISTER
        { $$ = createStgClassDeclInfo(STG_REGISTER, $1); }
    | Y_EXTERN
        { $$ = createStgClassDeclInfo(STG_EXTERN, $1); }
    | Y_STATIC
        { $$ = createStgClassDeclInfo(STG_STATIC, $1); }
    | Y_TYPEDEF
        { $$ = createStgClassDeclInfo(STG_TYPEDEF, $1); }
    ;

scalar-type-spec
    : Y_VOID
        { $$ = createDeclInfoSCALAR(STM_VOID, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_CHAR
        { $$ = createDeclInfoSCALAR(STM_CHAR, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_SHORT
        { $$ = createDeclInfoSCALAR(STM_SHORT, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_INT
        { $$ = createDeclInfoSCALAR(STM_INT, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_LONG
        { $$ = createDeclInfoSCALAR(STM_LONG, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_FLOAT
        { $$ = createDeclInfoSCALAR(STM_FLOAT, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_DOUBLE
        { $$ = createDeclInfoSCALAR(STM_DOUBLE, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_SIGNED
        { $$ = createDeclInfoSCALAR(STM_SIGNED, $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_UNSIGNED
        { $$ = createDeclInfoSCALAR(STM_UNSIGNED, $1->pos);
          $1->pos = NULL; zapToken($1); }
    ;

struct-or-union-spec
    : struct-or-union struct-id struct-or-union-body
        { $$ = createDeclStructInfo($1, $2, $3); }
    | struct-or-union            struct-or-union-body
        { $$ = createDeclStructInfo($1, NULL, $2); }
    | struct-or-union struct-id
        { $$ = createDeclStructInfo($1, $2, NULL); }
    ;

struct-id
    : Y_ID
    | Y_TYPEDEF_NAME
    ;

struct-or-union
    : Y_STRUCT
    | Y_UNION
    ;

struct-or-union-body
    : Y_LEFT_BRACE struct-decl-list Y_RIGHT_BRACE
        {
            $$ = createDeclStructBody($2, $3->pos);
            $3->pos = NULL; zapTokens2($1, $3);
        }
    ;

struct-decl-list
    : struct-decl
        { $$ = createDeclList($1); }
    | struct-decl-list struct-decl
        { $$ = addDeclList($1, $2); }
    | struct-decl-list error
        { recoverError(); $$ = $1; }
    ;

struct-decl
    : decl-specs Y_SEMICOLON
        { $$ = addDeclInfoDclrList($1, NULL); zapToken($2); }
    | decl-specs struct-declarator-list Y_SEMICOLON
        { $$ = addDeclInfoDclrList($1, $2); zapToken($3); }
    ;

struct-declarator-list
    : struct-declarator
        { $$ = createDclrList($1); }
    | struct-declarator-list Y_COMMA struct-declarator
        { $$ = addDclrList($1, $3); zapToken($2); }
    ;

struct-declarator
    : declarator
    | declarator Y_COLON constant-expression
        {
            reportError(CERR_CANT_CONVERT_BIT_FIELDS);
            $$ = $1;
            zapToken($2);  zapCTree($3);
        }
    ;

enum-spec
    : Y_ENUM identifier Y_LEFT_BRACE enum-list Y_RIGHT_BRACE
        { $$ = createDeclEnum($1, $4); zapTokens3($2, $3, $5); }
    | Y_ENUM identifier
        { $$ = createDeclEnum($1, NULL); zapToken($2); }
    | Y_ENUM            Y_LEFT_BRACE enum-list Y_RIGHT_BRACE
        { $$ = createDeclEnum($1, $3); zapTokens2($2, $4); }
    ;

enum-list
    : enum-list-collect
        { $$ = finishEnumListCreation($1); }
    | enum-list-collect Y_COMMA              /* Watcom C extension */
        { $$ = finishEnumListCreation($1); }
    ;

enum-list-collect
    : enumerator
        { $$ = createEnumList($1); }
    | enum-list-collect comma-and-enumerator
        { $$ = addEnumList($1,  $2); }
    | enum-list-collect error
        { recoverError();  $$ = $1; }
    ;

comma-and-enumerator
    : Y_COMMA enumerator
        { $$ = addEnumElemBegPunct($2); zapToken($1); }
    ;

enumerator
    : identifier
        { $$ = createEnumElem($1, NULL, NULL); }
    | identifier Y_EQUAL constant-expression
        { $$ = createEnumElem($1, $2, $3); }
    ;

cv-qualifier
    : Y_CONST
        { reportError(CERR_CANT_CONVERT_QUALIFIER);
          $$ = createQualifierDeclInfo(STY_CONST,  $1->pos);
          $1->pos = NULL; zapToken($1); }
    | Y_VOLATILE
        { reportError(CERR_CANT_CONVERT_QUALIFIER);
          $$ = createQualifierDeclInfo(STY_VOLATILE,  $1->pos);
          $1->pos = NULL; zapToken($1); }
    ;

type-name
    : decl-specs
        { $$ = createCTreeRoot(createDeclInfoLabel($1)); }
    | decl-specs declarator-no-id
        { $$ = createCTreeRoot(createDeclInfoLabel(addDeclDclr($1, $2))); }
    ;

literal
    : strings
    | Y_NUMBER
        { $$ = createCTreeRoot(createTokenLabel($1)); }
    ;

strings
    : strings single-string
        {
            $$ = createCTree2(createConstr0Label(LABCT_STRINGS), $1, $2);
        }
    | single-string
        { $$ = $1; }
    ;

single-string
    : Y_STRING
        { $$ = createCTreeRoot(createTokenLabel($1)); }
    ;

declarator-id
    : identifier
    | Y_TYPEDEF_NAME
    ;

declarator
    : pragma-modifier declarator
        { $$ = addDclrPragmaModifier($2, $1); }
    | mem-modifier declarator
        { $$ = addDclrMemModifier($2, $1); }
    | ptr-modifier declarator
        { $$ = addDclrPtrModifier($2, $1); }
    | actual-declarator
        { $$ = $1; }
    ;

actual-declarator
    : declarator-id
        { $$ = createDclr($1); }
    | Y_LEFT_PAREN declarator Y_RIGHT_PAREN
        { $$ = $2; zapTokens2($1, $3); }
    | actual-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
        { $$ = addDclrArray($1, $2, $3, $4); }
    | actual-declarator Y_LEFT_BRACKET                     Y_RIGHT_BRACKET
        { $$ = addDclrArray($1, $2, NULL, $3); }
    | actual-declarator Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN
        { $$ = addDclrFuncArgs($1, $2, $3, $4); }
    ;

declarator-no-id
    : pragma-modifier declarator-no-id
        { $$ = addDclrPragmaModifier($2, $1); }
    | pragma-modifier
        { $$ = addDclrPragmaModifier(createDclr(NULL), $1); }
    | mem-modifier declarator-no-id
        { $$ = addDclrMemModifier($2, $1); }
    | mem-modifier
        { $$ = addDclrMemModifier(createDclr(NULL), $1); }
    | ptr-modifier declarator-no-id
        { $$ = addDclrPtrModifier($2, $1); }
    | ptr-modifier
        { $$ = addDclrPtrModifier(createDclr(NULL), $1); }
    | actual-declarator-no-id
        { $$ = $1; }
    ;

actual-declarator-no-id
    : Y_LEFT_PAREN declarator-no-id Y_RIGHT_PAREN
        { $$ = $2; zapTokens2($1, $3); }
    | actual-declarator-no-id Y_LEFT_BRACKET                     Y_RIGHT_BRACKET
        { $$ = addDclrArray($1, $2, NULL, $3); }
    | actual-declarator-no-id Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
        { $$ = addDclrArray($1, $2, $3, $4); }
    |                         Y_LEFT_BRACKET                     Y_RIGHT_BRACKET
        { $$ = addDclrArray(createDclr(NULL), $1, NULL, $2); }
    |                         Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET
        { $$ = addDclrArray(createDclr(NULL), $1, $2, $3); }
    | actual-declarator-no-id Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN
        { $$ = addDclrFuncArgs($1, $2, $3, $4); }
    |                         Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN
        { $$ = addDclrFuncArgs(createDclr(NULL), $1, $2, $3); }
    ;

ptr-modifier
    : Y_TIMES cv-qualifiers-opt
        { $$ = createDclrPtr($1, $2); }
    ;

mem-modifier
    : Y___NEAR
    | Y___FAR
    | Y___FAR16
    | Y___HUGE
    ;

pragma-modifier
    : Y___CDECL
    | Y___PASCAL
    | Y___FORTRAN
    | Y__SYSCALL
    | Y___STDCALL
    ;

cv-qualifiers-opt
    : /* nothing */
        { $$ = STY_NULL; }
    | Y_CONST
        { reportError(CERR_CANT_CONVERT_QUALIFIER);
        $$ = STY_CONST; zapToken($1); }
    | Y_CONST Y_VOLATILE
        { reportError(CERR_CANT_CONVERT_QUALIFIER);
        $$ = STY_CONST | STY_VOLATILE; zapTokens2($1, $2); }
    | Y_VOLATILE
        { reportError(CERR_CANT_CONVERT_QUALIFIER);
        $$ = STY_VOLATILE; zapToken($1); }
    | Y_VOLATILE Y_CONST
        { reportError(CERR_CANT_CONVERT_QUALIFIER);
        $$ = STY_CONST | STY_VOLATILE; zapTokens2($1, $2); }
    ;

abstract-args
    : /* nothing */
        { $$ = createSLList(); }
    | arg-decl-list
    | arg-decl-list Y_COMMA dot-dot-dot-decl
        { addDeclPunct($3, $2); $$ = addDeclList($1, $3 ); }
    | dot-dot-dot-decl
        { $$ = createDeclList($1); }
    ;

arg-decl-list
    : arg-decl-elem
        { $$ = createDeclList($1); }
    | arg-decl-list Y_COMMA arg-decl-elem
        { addDeclPunct($3, $2); $$ = addDeclList($1, $3 ); }
    ;

arg-decl-elem
    : decl-specs
        { $$ = $1; }
    | decl-specs declarator
        { $$ = addDeclDclr($1, $2); }
    | decl-specs declarator-no-id
        { $$ = addDeclDclr($1, $2); }
    ;

dot-dot-dot-decl
    : Y_DOT_DOT_DOT
        { $$ = createDeclInfoSCALAR(STM_DOT_DOT_DOT, $1->pos);
          $1->pos = NULL; zapToken($1); }
    ;

identifier
    : Y_ID
        { $$ = $1; }
    ;

/* NOTE: There is an ambiguity in C:
   typedef int F, INT;         static F(INT);
   This is resolved by redefining F as a function, not by redefining INT
   to be of type "static F"
*/

/*==========================================================================*/

%%

static pToken firstToken;
static int expandNextToken;
static int expandThisLine;
int expandThisLineHideErrors;
int successfulExpand;

int preerror(char *str) {
    str = str;
    return 0;
}

void preparseInterface(pToken t) {
    firstToken = t;
    expandNextToken = 1;
    expandThisLine = 1;
    expandThisLineHideErrors = 0;
    successfulExpand = 1;
    preparse();
}

static int prelex(void) {
    int retval;

    successfulExpand = 1;
    if (firstToken != NULL) {
        prelval.token = firstToken;
        firstToken = NULL;
    } else {
        prelval.token = getExpandToken(
                ((expandNextToken && expandThisLine) ? EXP_OP_EXPAND : 0)  |
                EXP_OP_EMIT_EOL |
                (expandThisLineHideErrors ? EXP_OP_HIDE_ERRORS : 0),
                &successfulExpand);

    }
    if (!successfulExpand) {
        expandThisLineHideErrors = 0;
        retval = 0;
    } else {
        retval = prelval.token->data->code;
    }
    expandNextToken = 1;
    if (retval == Y_PRE_NEWLINE) {
        expandThisLine = 1;
        expandThisLineHideErrors = 0;
        zapToken(prelval.token);
    }
    return retval;
}

static pToken _dupTokenKeepPos(void * _tok) {
    pToken tok = _tok;
    return dupToken(tok, NULL);
}

pCTree createDefineMacroCTree(void) {
    pCTree tree;
    pSLList list;
    pToken tempTok;
    pToken idToken;
    pSymTabEntry newSymbol;
    int assertVal;

    /* Read tokens until end-of-line */
    expandThisLine = 0;
    if (prechar != Y_PRE_NEWLINE) {
        while (prelex() != Y_PRE_NEWLINE);
        if (g_opt.targetLang == TLT_FORTRAN) {
            reportError(CERR_PREDIR_NOT_SUPPORTED, "#define");
        }
    }

    rewindCurrSLListPos(g_currLineCode);
    incCurrSLListPos(g_currLineCode);  /* Skip #define */
    assertVal = getCurrSLListPosElem(g_currLineCode, &idToken);
    assert(assertVal);
    incCurrSLListPos(g_currLineCode);  /* Skip id */

    list = createSLList();
    for (;;) {
        if (!getCurrSLListPosElem(g_currLineCode, &tempTok)) {
            break;
        }
        addSLListElem(list, tempTok);
        incCurrSLListPos(g_currLineCode);
    }

    tree = createCTree2(
            createConstr1Label(LABCT_PRE_DEFINE_MACRO, _lastDefineTok),
            createCTreeRoot(createTokenLabel(idToken)),
            createCTreeRoot(createListLabel(list))
         );

    newSymbol = createTabEntry(getTokenIdName(idToken), SYMT_MACRO,
                               dupSLList(list, _dupTokenKeepPos));
    addSymbol(newSymbol);

    return tree;
}

pCTree eatPreDirList(pToken tok) {
    pSLList list;

    list = createSLList();
    prechar = prelex();
    while (prechar != Y_PRE_NEWLINE) {
        addSLListElem(list, prelval.token);
        prechar = prelex();
    }

    return  createCTree1(
            createConstr1Label(LABCT_PRE_DIR_LIST, tok),
            createCTreeRoot(createListLabel(list))
         );
}

#define preclearin      prechar = -1
                        // We have to copy yacc def'n of preclearin since
                        // yacc puts recoverError() function before it declares
                        // 'preclearin'.  If yacc's def'n ever changes,
                        // we will get a redefinition error.
#define CLEARIN preclearin
static void recoverError(void) {
    pToken tok;
    pSLList context = createSLList();
    int recordMaxTok = 15;
    char *s1;

    for (tok = prelval.token;
                prechar == Y_PRE_NEWLINE ? 0 : (tok->data->code != Y_EOF)  ;
                prechar = prelex(), tok = prelval.token)
    {
        if (recordMaxTok-- > 0) {
            addSLListElem(context, tok);
        }
    }

    s1 = getTokListString(context);
    reportError(RERR_CPARSE_WITH_CONTEXT, s1, "");
    zapSLList(context, zapToken);
    CLEARIN;
    firstToken = tok;
}

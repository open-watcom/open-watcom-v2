%{
#include <stdio.h>
#include <string.h>
#include "standard.h"
#include "ppsql.h"
#include "pplang.h"
#include "sqlca.h"
#include "rndbinfo.h"
#include "ppsymtab.h"
#include "rnparsdf.h"
#include "pppars.h"

#include "pppact.def"
#include "ppbuff.def"
#include "parsutil.def"

#include "rnlist.def"
#include "rnhostvr.def"
#include "rnpautil.def"
#include "rnstmt.def"
#include "rnfunc.def"

extern void 		LineInfo();

extern char *		actions[];
extern char		*ProgName;
extern a_statement	*Stmt;
extern bool		SQLStmt;

static a_host_type	hostvar_type = { 0 };
static char		buff[200] = { 0 };

#define PT_COLDEFN	1
#define PT_UNIQUE	2
#define PT_KEY		3

#ifdef __TURBOC__
    #define YYTABTYPE	short far
#else
    #define YYTABTYPE	short const far
#endif

#ifdef PARSTREE
    #define T(x)	x
#else
    #define T(x)
#endif

#define yylex		yygettoken

%}

%start sqlprogram


%token <kwd> T_LBRACE
%token <kwd> T_RBRACE
%token <kwd> T_SEMI_COLON
%token <kwd> T_LPAREN
%token <kwd> T_RPAREN
%token <kwd> T_DOT
%token <kwd> T_COMMA
%token <kwd> T_PLUS
%token <kwd> T_MINUS
%token <kwd> T_TIMES
%token <kwd> T_DIVIDE
%token <kwd> T_EXECSQL 
%token <kwd> T_CBLOCK
%token <kwd> T_INITIALIZER
%token <ptr> T_DSTRING
%token <kwd> T_STATVAR
%token <kwd> T_FIXCHAR
%token <kwd> T_LONGVARCHAR
%token <kwd> T_STATIC
%token <kwd> T_EXTERN
%token <kwd> T_REGISTER
%token <kwd> T_UNSIGNED
%token <kwd> T_SHORT
%token <kwd> T_SQL

%token <kwd> T_TOK

%token <kwd>  T_SELECT	256
%token <kwd>  T_AND	257
%token <kwd>  T_OR	258
%token <kwd>  T_COMMIT	259
%token <kwd>  T_ROLLBACK	260
%token <kwd>  T_WORK	261
%token <kwd>  T_INTO	262
%token <kwd>  T_FROM	263
%token <kwd>  T_WHERE	264
%token <kwd>  T_IN	265
%token <kwd>  T_BETWEEN	266
%token <kwd>  T_IS	267
%token <kwd>  T_NOT	268
%token <kwd>  T_NULL	269
%token <kwd>  T_SOME	270
%token <kwd>  T_ANY	271
%token <kwd>  T_EXISTS	272
%token <kwd>  T_LIKE	273
%token <kwd>  T_IF	274
%token <kwd>  T_THEN	275
%token <kwd>  T_ELSE	276
%token <kwd>  T_ENDIF	277
%token <kwd>  T_GROUP	278
%token <kwd>  T_HAVING	279
%token <kwd>  T_ORDER	280
%token <kwd>  T_BY	281
%token <kwd>  T_DELETE	282
%token <kwd>  T_CURRENT	283
%token <kwd>  T_ALTER	284
%token <kwd>  T_TABLE	285
%token <kwd>  T_RENAME	286
%token <kwd>  T_OF	287
%token <kwd>  T_UPDATE	288
%token <kwd>  T_SET	289
%token <kwd>  T_UNION	290
%token <kwd>  T_INSERT	291
%token <kwd>  T_VALUES	292
%token <kwd>  T_CREATE	293
%token <kwd>  T_VIEW	294
%token <kwd>  T_ADD	295
%token <kwd>  T_MODIFY	296
%token <kwd>  T_CHAR	297
%token <kwd>  T_INTEGER	298
%token <kwd>  T_SMALLINT	299
%token <kwd>  T_DECIMAL	300
%token <kwd>  T_NUMERIC	301
%token <kwd>  T_FLOAT	302
%token <kwd>  T_REAL	303
%token <kwd>  T_DOUBLE	304
%token <kwd>  T_PRECISION	305
%token <kwd>  T_VARCHAR	306
%token <kwd>  T_LONG	307
%token <kwd>  T_DATE	308
%token <kwd>  T_TIME	309
%token <kwd>  T_TIMESTAMP	310
%token <kwd>  T_DAYS	311
%token <kwd>  T_USER	312
%token <kwd>  T_PRIMARY	313
%token <kwd>  T_FOREIGN	314
%token <kwd>  T_REFERENCES	315
%token <kwd>  T_KEY	316
%token <kwd>  T_CHECK	317
%token <kwd>  T_FOR	318
%token <kwd>  T_UNIQUE	319
%token <kwd>  T_ON	320
%token <kwd>  T_ASC	321
%token <kwd>  T_DESC	322
%token <kwd>  T_AS	323
%token <kwd>  T_USING	324
%token <kwd>  T_DISTINCT	325
%token <kwd>  T_ALL	326
%token <kwd>  T_OPTION	327
%token <kwd>  T_DROP	328
%token <kwd>  T_INDEX	329
%token <kwd>  T_GRANT	330
%token <kwd>  T_REVOKE	331
%token <kwd>  T_TO	332
%token <kwd>  T_IDENTIFIED	333
%token <kwd>  T_PRIVILEGES	334
%token <kwd>  T_CONNECT	335
%token <kwd>  T_DBA	336
%token <kwd>  T_RESOURCE	337
%token <kwd>  T_SCHEDULE	338
%token <kwd>  T_WITH	339
%token <kwd>  T_LOCK	340
%token <kwd>  T_MODE	341
%token <kwd>  T_DBSPACE	342
%token <kwd>  T_SHARE	343
%token <kwd>  T_EXCLUSIVE	344
%token <kwd>  T_COLUMN	345
%token <kwd>  T_COMMENT	346
%token <kwd>  T_RELEASE	347
%token <kwd>  T_ACQUIRE	348
%token <kwd>  T_NAMED	349
%token <kwd>  T_PUBLIC	350
%token <kwd>  T_AVG	351
%token <kwd>  T_MAX	352
%token <kwd>  T_MIN	353
%token <kwd>  T_SUM	354
%token <kwd>  T_COUNT	355
%token <kwd>  T_SECOND	356
%token <kwd>  T_MINUTE	357
%token <kwd>  T_HOUR	358
%token <kwd>  T_DAY	359
%token <kwd>  T_MONTH	360
%token <kwd>  T_YEAR	361
%token <kwd>  T_MICROSECOND	362
%token <kwd>  T_LEFT	363
%token <kwd>  T_LENGTH	364
%token <kwd>  T_LOWER	365
%token <kwd>  T_LTRIM	366
%token <kwd>  T_RIGHT	367
%token <kwd>  T_RTRIM	368
%token <kwd>  T_SUBSTRING	369
%token <kwd>  T_SUBSTR	370
%token <kwd>  T_UPPER	371
%token <kwd>  T_MILLISECOND	372
%token <kwd>  T_WEEKDAY	373
%token <kwd>  T_ACCELERATED	374
%token <kwd>  T_AUTOINC	375
%token <kwd>  T_BEGINS	376
%token <kwd>  T_BFLOAT	377
%token <kwd>  T_BINARYNULL	378
%token <kwd>  T_BIT	379
%token <kwd>  T_BLANK	380
%token <kwd>  T_CASE	381
%token <kwd>  T_CONTAINS	382
%token <kwd>  T_CREATETAB	383
%token <kwd>  T_CURDATE	384
%token <kwd>  T_CURTIME	385
%token <kwd>  T_DATAPATH	386
%token <kwd>  T_DCOMPRESS	387
%token <kwd>  T_DDPATH	388
%token <kwd>  T_DECIMALNULL	389
%token <kwd>  T_DICTIONARY	390
%token <kwd>  T_EXEC	391
%token <kwd>  T_LOGICAL	392
%token <kwd>  T_LOGIN	393
%token <kwd>  T_LSTRING	394
%token <kwd>  T_LVAR	395
%token <kwd>  T_MASK	396
%token <kwd>  T_MOD	397
%token <kwd>  T_MONEY	398
%token <kwd>  T_NORMAL	399
%token <kwd>  T_NOTE	400
%token <kwd>  T_OPENMODE	401
%token <kwd>  T_OWNER	402
%token <kwd>  T_OWNERACCESS	403
%token <kwd>  T_PAGESIZE	404
%token <kwd>  T_PREALLOCATE	405
%token <kwd>  T_PROCEDURE	406
%token <kwd>  T_RANGE	407
%token <kwd>  T_READONLY	408
%token <kwd>  T_REPLACE	409
%token <kwd>  T_SECURITY	410
%token <kwd>  T_SEG	411
%token <kwd>  T_START	412
%token <kwd>  T_STRINGNULL	413
%token <kwd>  T_THRESHOLD	414
%token <kwd>  T_TRANSACTION	415
%token <kwd>  T_VERIFY	416
%token <kwd>  T_VIEWPATH	417
%token <kwd>  T_ZSTRING	418
%token <kwd>  T_BEGIN	419
%token <kwd>  T_END	420
%token <kwd>  T_GOTO	421
%token <kwd>  T_GO	422
%token <kwd>  T_INCLUDE	423
%token <kwd>  T_SECTION	424
%token <kwd>  T_PREPARE	425
%token <kwd>  T_DESCRIBE	426
%token <kwd>  T_SETTING	427
%token <kwd>  T_STATEMENT	428
%token <kwd>  T_NAMES	429
%token <kwd>  T_LABELS	430
%token <kwd>  T_BOTH	431
%token <kwd>  T_EXECUTE	432
%token <kwd>  T_IMMEDIATE	433
%token <kwd>  T_WHENEVER	434
%token <kwd>  T_SQLERROR	435
%token <kwd>  T_SQLWARNING	436
%token <kwd>  T_NOTFOUND	437
%token <kwd>  T_FOUND	438
%token <kwd>  T_CONTINUE	439
%token <kwd>  T_STOP	440
%token <kwd>  T_DECLARE	441
%token <kwd>  T_SCROLL	442
%token <kwd>  T_CURSOR	443
%token <kwd>  T_OPEN	444
%token <kwd>  T_REOPEN	445
%token <kwd>  T_DESCRIPTOR	446
%token <kwd>  T_FETCH	447
%token <kwd>  T_NEXT	448
%token <kwd>  T_PRIOR	449
%token <kwd>  T_FIRST	450
%token <kwd>  T_LAST	451
%token <kwd>  T_ABSOLUTE	452
%token <kwd>  T_RELATIVE	453
%token <kwd>  T_PUT	454
%token <kwd>  T_CLOSE	455
%token <kwd>  T_LIST	456
%token <kwd>  T_BIND	457
%token <kwd>  T_VARIABLES	458
%token <kwd>  T_GET	459
%token <kwd>  T_DATABASE	460
%token <kwd>  T_DEFAULT	461
%token <kwd>  T_INSENSITIVE	462
%token <kwd>  T_READ	463
%token <kwd>  T_ONLY	464

%type <ptr>  sqlprogram stmt execsql embsqlstmt 
%type <ptr>  open reopen id_spec id_indspec structure_spec
%type <ptr>  using_sqlda using_sqlda_ne 
%type <ptr>  fetch indhostlist intohostlist put putkwd 
%type <ptr>  close declare declare_tok scroll 
%type <ptr>  insert_stmt select_stmt describe
%type <ptr>  updatecursor delete onetabspec
%type <ptr>  offset
%type <ival> direction 
%type <info> position
%type <ptr>  statement_spec ppprogname prepare prepusing dropstmt
%type <ptr>  execute string_spec
%type <ptr>  indhostvar embhostvar intohostvar
%type <ptr>  whenever when_action when_action2
%type <ptr>  include declarations begin_decl declexecsql
%type <ptr>  end_decl var_decl_list var_decl type_specifier intlist intlist1
%type <ptr>  initializer
%type <ptr>  declarator_list declarator declarator2 connect 
%type <ptr>  defuid database dictionary
%type <ptr>  notfound goto
%type <ptr>  setoption getoption get_using
%type <ptr>  selinto2 

%type <uval> desc_using desc_using_element describe_type

%type <hosttype> pointer



 /************************************************************************
 *	 Copyright (C) 1989, by PACE Computing Solutions Inc. All rights *
 *	 reserved. No part of this software may be reproduced		 *
 *	 in any form or by any means - graphic, electronic or		 *
 *	 mechanical, including photocopying, recording, taping		 *
 *	 or information storage and retrieval systems - except		 *
 *	 with the written permission of PACE Computing Solutions Inc.	 *
 *************************************************************************/


%{
static void		*update_collist = NULL;
static void		*tabrefs = NULL;
extern bool		SelectStmt;
extern bool		StoredProcedure;
extern bool		GotHostVar;

#define	yyparse		paceparse
%}

%token <kwd>  T_BADTOK
%token <kwd>  T_EQ
%token <kwd>  T_NE
%token <kwd>  T_GT
%token <kwd>  T_LT
%token <kwd>  T_LE
%token <kwd>  T_GE
%token <kwd>  T_DOTSTAR
%token <ptr>  T_IDENTIFIER
%token <ptr>  T_NUMBER
%token <ptr>  T_STRING
%token <ptr>  T_HOSTVAR
%token <ptr>  T_SPPARM
%token <ptr>  T_LONGINT

%type <ptr>  hostvar selinto
%type <ptr>  sqlstmt 
%type <ptr>  select selectlist sellist selvalue expression
%type <ptr>  expression_nosub expression_notnull
%type <ptr>  query_expr query_term query_spec query_union query_unionall
%type <ptr>  order_by_clause orderbylistrev orderbylist orderbyitem
%type <ptr>  for_update_clause
%type <ptr>  builtin constant string special_register
%type <ptr>  from tabspec tabexprlist tabexprlistrev tabexpr onetabspec tabref
%type <ptr>  corrname tabname indexname colname colref colreflist colreflistrev
%type <ptr>  where searchcond
%type <ptr>  boolterm boolfactor boolprimary srchpred
%type <ptr>  vallist vallistrev valitem
%type <ptr>  subquery single_subquery quantified_subquery
%type <ptr>  groupby having
%type <ptr>  delete wherecurrentof
%type <ptr>  update sets setlist setitem
%type <ptr>  insert insertvalues 
%type <ptr>  ins_vallist ins_vallistrev ins_valitem
%type <ptr>  columns collist collistrev
%type <ptr>  commenton
%type <ptr>  createtable tblelmtlist rolename fgnkeycols fgnkeycollist fgnkeycol
%type <ptr>  inclause optcomma
%type <info> createclause
%type <ptr>  createindex indexlist indexlistrev indexcol 
%type <ival> indexattr indexattrlist indexattrlist1
/* 
%type <ptr>  createprogram create_options dropprogram
%type <ival> op_keep op_describe op_replace op_modify op_block
*/
%type <ptr>  createview
%type <ptr>  alter alter_delete alter_clause coldefn modcoldefn data_type
%type <ptr>  number integer
%type <ptr>  droptable dropindex dropview 
%type <ptr>  grant granttok revoketok 
%type <ptr>  userlist userlistrev userid
%type <ptr>  passwdlist passwdlistrev password priv_columns
%type <ptr>  revoke dbspacename
%type <ptr>  lock lockmode
%type <ptr>  commit rollback parameter parmlist
%type <ptr>  distinct runid_spec

%type <ival> not summarizer unique
%type <ival> comp_op quantifier duration
%type <ival> grantoption checkclause notnull 
%type <ival> tableprivileges tableprivlist tablepriv specialpriv

%type <uval> char_type integer_type exact_num_type float_type


%right T_CONCAT
%left T_PLUS T_MINUS
%left T_TIMES T_DIVIDE
%left T_JOIN

%%

sqlstmt:	alter
		    { T($$ = pa_make_stmt( STMT_ALTER, $1 ); )}
	    |	commenton
		    { T($$ = pa_make_stmt( STMT_COMMENTON, $1 ); )}
	    |	createtable
		    { T($$ = pa_make_stmt( STMT_CREATETABLE, $1 ); )}
	    |	createindex
		    { T($$ = pa_make_stmt( STMT_CREATEINDEX, $1 ); )}
	    |	createview
		    { T($$ = pa_make_stmt( STMT_CREATEVIEW, $1 ); )}
	    /*
	    |	createprogram
		    { T($$ = pa_make_stmt( STMT_CREATEPROGRAM, $1 ); )}
	    |	dropprogram
		    { T($$ = pa_make_stmt( STMT_DROPPROGRAM, $1 ); )}
            */
	    |	delete
		    { T($$ = pa_make_stmt( STMT_DELETE, $1 ); )}
	    |	droptable
		    { T($$ = pa_make_stmt( STMT_DROPTABLE, $1 ); )}
	    |	dropindex
		    { T($$ = pa_make_stmt( STMT_DROPINDEX, $1 ); )}
	    |	dropview
		    { T($$ = pa_make_stmt( STMT_DROPVIEW, $1 ); )}
	    |	grant
		    { T($$ = pa_make_stmt( STMT_GRANT, $1 ); )}
	    |	revoke
		    { T($$ = pa_make_stmt( STMT_REVOKE, $1 ); )}
	    |	insert
		    { T($$ = pa_make_stmt( STMT_INSERT, $1 ); )}
	    |	lock
		    { T($$ = pa_make_stmt( STMT_LOCK, $1 ); )}
	    |	{SelectStmt=TRUE;}  select
		    { T($$ = pa_make_stmt( STMT_SELECT, $2 ); )}
	    |	update
		    { T($$ = pa_make_stmt( STMT_UPDATE, $1 ); )}
	    |	commit
		    { T($$ = pa_make_stmt( STMT_COMMIT, $1 ); )}
	    |	rollback
		    { T($$ = pa_make_stmt( STMT_ROLLBACK, $1 ); )}
	    ;

select: 	query_expr  order_by_clause  for_update_clause
		    {T( $$ = pa_query( $1, $2 ); )}
	    ;

order_by_clause:    /* empty */
		    {T( $$ = NULL; )}
	    |	T_ORDER  T_BY  orderbylist
		    {T( $$ = $3; )}
	    ;

for_update_clause:  /* empty */
		    {T( $$ = NULL; )}
	    |	T_FOR  T_UPDATE  T_OF  colreflist
		    {  pa_specific( L_SAA );
		    T( $$ = $4; 
		    )}
	    ;

orderbylist:	orderbylistrev	optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

orderbylistrev: orderbyitem
		    {T( $$ = pa_iadd_plist( NULL, $1 ); )}
	    |	orderbylistrev	T_COMMA  orderbyitem
		    {T( $$ = pa_iadd_plist( $1, $3 ); )}
	    ;

orderbyitem:	expression
		    {T( $$ = pa_orderitem( $1, IA_ASC ); )}
	    |	expression  T_ASC
		    {T( $$ = pa_orderitem( $1, IA_ASC ); )}
	    |	expression  T_DESC
		    {T( $$ = pa_orderitem( $1, IA_DESC ); )}
	    ;

query_expr:	query_term
	    |	query_union
	    |	query_unionall
	    ;

query_unionall: query_unionall	T_UNION  T_ALL	query_term
		    {T( $$ = pa_binary_expr( E_UNIONALL, $1, $4 ); )}
	    |	query_term T_UNION  T_ALL  query_term
		    {T( $$ = pa_binary_expr( E_UNIONALL, $1, $4 ); )}
	    ;

query_union:	query_union  T_UNION  query_term
		    {T( $$ = pa_binary_expr( E_UNION, $1, $3 ); )}
	    |	query_term T_UNION  query_term
		    {T( $$ = pa_binary_expr( E_UNION, $1, $3 ); )}
	    ;

query_term:	query_spec
	    |	T_LPAREN  query_expr  T_RPAREN
		    {T( $$ = pa_unary_expr( E_PAREN, $2 ); )}
	    ;

query_spec:	T_SELECT  summarizer  selectlist  selinto  from  where
		groupby  having
		    {T( $$ = pa_query_spec( $2, $3, $5, $6, $7, $8 ); )}
	    ;

single_subquery:T_LPAREN  query_spec  T_RPAREN
		    {T( $$ = pa_subquery( $2 );
		      $$ = pa_unary_expr( E_PAREN, $$ );
		    )}
	    ;

subquery:	T_LPAREN  query_expr  T_RPAREN
		    {T( $$ = pa_subquery( $2 );
		      $$ = pa_unary_expr( E_PAREN, $$ );
		    )}
	    ;

summarizer:	 /* empty */
		    {T( $$ = FALSE; )}
	    |	T_ALL
		    {T( $$ = FALSE; )}
	    |	distinct
		    {T( $$ = TRUE; )}
	    ;

distinct:	T_DISTINCT
		    {T( $$ = pa_distinct(); )}
	    ;

selectlist:	T_TIMES
		    {T( $$ = NULL; )}
	    |	sellist  optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

sellist:	selvalue
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	sellist  T_COMMA  selvalue
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

selvalue:	expression
	    |	expression  T_IDENTIFIER
		    {T( $$ = pa_binary_expr( E_NAME, $1, $2 ); )}
            |   tabname  T_DOTSTAR
	    ;

expression:	expression_nosub
	    |	subquery
	    ;

expression_notnull: expression
		    {T( if( pa_isnull( $1 ) ){
		          sql_errors( SQLE_SYNTAX_ERROR, "NULL" );
		      }
		    )}
	    ;

expression_nosub: expression T_PLUS expression
		    {T( $$ = pa_binary_expr( E_ADD, $1, $3 ); )}
	    |	expression T_MINUS expression
		    {T( $$ = pa_binary_expr( E_SUB, $1, $3 ); )}
	    |	expression T_TIMES expression
		    {T( $$ = pa_binary_expr( E_MUL, $1, $3 ); )}
	    |	expression T_DIVIDE expression
		    {T( $$ = pa_binary_expr( E_DIV, $1, $3 ); )}
	    |	expression T_CONCAT expression
		    {T( $$ = pa_binary_expr( E_CONCAT, $1, $3 ); )}
	    |	T_MINUS expression	%prec T_TIMES
		    {T( $$ = pa_unary_expr( E_UMINUS, $2 ); )}
	    |	T_PLUS expression	%prec T_TIMES
		    {T( $$ = $2; )}
	    |	colref  duration
	    |	constant  duration
	    |	T_NULL
		    {T( $$ = pa_zero_expr( E_NULL ); )}
	    |	builtin T_LPAREN parmlist  T_RPAREN  duration
		    { $$ = pa_builtin( $1, pa_reverse_plist( $3 ) ); }
	    |	builtin T_LPAREN T_TIMES T_RPAREN  duration
		    { $$ = pa_builtin( $1, NULL ); }
	    |	T_LPAREN expression_nosub T_RPAREN  duration
		    {T( $$ = pa_unary_expr( E_PAREN, $2 ); )}
	    |	T_IF searchcond T_THEN	expression  T_ELSE  expression	T_ENDIF
		    {  pa_specific( L_PACE );
		    T( $$ = pa_ternary_expr( E_IF, $2, $4, $6 ); 
		    )}
	    |	T_IF searchcond T_THEN	expression  T_ENDIF
		    {  pa_specific( L_PACE );
		    T( $$ = pa_ternary_expr( E_IF, $2, $4, NULL ); 
		    )}
	    ;

duration:   	/* empty */
	    |	T_YEAR
	    |	T_MONTH
	    |	T_DAY
	    |	T_DAYS
	    |	T_HOUR
	    |	T_MINUTE
	    |	T_SECOND
	    |	T_MICROSECOND
	    ;

parmlist:	parameter
		    { $$ = pa_add_plist( NULL, $1 ); }
	    |	parmlist  T_COMMA  parameter
		    { $$ = pa_add_plist( $1, $3 ); }
	    ;

parameter:	expression
		    {T( $$ = $1; )}
	    |	T_ALL  expression
		    {T( $$ = $2; )}
	    |	distinct  colref
		    {T( $$ = pa_unary_expr( E_DISTINCT, $2 ); )}
	    ;

builtin:	T_IDENTIFIER
            |	T_MAX
            |	T_MIN
            |	T_AVG
            |	T_SUM
            |	T_COUNT
            |	T_DATE
            |	T_YEAR
	    |	T_MONTH
	    |	T_DAYS
	    |	T_DAY
	    |	T_HOUR
	    |	T_MINUTE
	    |	T_SECOND
	    |	T_MICROSECOND
	    |	T_MILLISECOND
	    |	T_TIME
	    |	T_TIMESTAMP
	    |	T_LOWER
	    |	T_UPPER
	    |	T_SUBSTRING
	    |	T_RTRIM
	    |	T_RIGHT
	    |	T_LTRIM
	    |	T_LEFT
	    |	T_LENGTH
	    |	T_CHAR
	    ;

constant:	number
	    |	hostvar
	    |	string
	    |	special_register
	    ;

string: 	T_STRING
		    {T( $$ = pa_unary_expr( E_STRING, $1 ); )}
	    ;

special_register: T_USER
		    {T( $$ = pa_zero_expr( E_USER ); )}
	    |	T_CURRENT  T_DATE
	            { pa_specific( L_SAA ); }
	    |	T_CURRENT  T_TIME
	            { pa_specific( L_SAA ); }
	    |	T_CURRENT  T_TIMESTAMP
	            { pa_specific( L_SAA ); }
	    ;

from:		T_FROM	tabspec
		    {T( $$ = $2; )}
	    ;

tabspec:	{T( tabrefs = NULL; )} tabexprlist
		    {T( $$ = pa_tabspec( $2, tabrefs ); )}
	    ;

tabexprlist:	tabexprlistrev	optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

tabexprlistrev: tabexpr
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	tabexprlistrev	T_COMMA  tabexpr
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

tabexpr:	tabref
		    {T( $$ = pa_unary_expr( E_TABLE, $1 ); )}
	    |	tabexpr  T_JOIN  tabexpr
		    {T( $$ = pa_binary_expr( E_JOIN, $1, $3 ); )}
	    |	T_LPAREN  tabexprlist  T_RPAREN
		    {T( $$ = pa_unary_expr( E_PAREN, $2 ); )}
	    ;


tabref: 	T_IDENTIFIER  T_DOT  T_IDENTIFIER  corrname
		    {T( $$ = pa_tabref( &tabrefs, $3, $1, $4 ); )}
	    |	T_IDENTIFIER  corrname
		    {T( $$ = pa_tabref( &tabrefs, $1, NULL, $2 ); )}
	    ;

corrname:	T_IDENTIFIER
	    |	/* empty */
		    {T( $$ = NULL; )}
	    ;

tabname:	T_IDENTIFIER  T_DOT  T_IDENTIFIER
		    {T( $$ = pa_table( $3, $1 ); )}
	    |	T_IDENTIFIER
		    {T( $$ = pa_table( $1, NULL ); )}
	    ;

indexname:	T_IDENTIFIER
	    ;

colref: 	T_IDENTIFIER  T_DOT  colname
		    {T( $$ = pa_column( $1, $3 ); )}
	    |	colname
		    {T( $$ = pa_column( NULL, $1 ); )}
	    |   T_IDENTIFIER  T_DOT  T_IDENTIFIER  T_DOT  colname
	            { pa_specific( L_SAA ); }
	    ;

colreflist:	colreflistrev  optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

colreflistrev:	colref
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	colreflistrev  T_COMMA  colref
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

colname:	T_IDENTIFIER
	    ;

where:		T_WHERE  searchcond
		    {T( $$ = $2; )}
	    |	/* empty */
		    {T( $$ = NULL; )}
	    ;

searchcond:	boolterm
	    |	searchcond  T_OR  boolterm
		    {T( $$ = pa_binary_expr( E_OR, $1, $3 ); )}
	    ;

boolterm:	boolfactor
	    |	boolterm  T_AND  boolfactor
		    {T( $$ = pa_binary_expr( E_AND, $1, $3 ); )}
	    ;

boolfactor:	boolprimary
	    |	T_NOT  boolprimary
		    {T( $$ = pa_unary_expr( E_NOT, $2 ); )}
	    ;

boolprimary:	srchpred
	    |	T_LPAREN  searchcond  T_RPAREN
		    {T( $$ = pa_unary_expr( E_PAREN, $2 ); )}
	    |	T_LPAREN  searchcond  T_COMMA  expression  T_RPAREN
	            {  pa_specific( L_PACE );
		    T( $$ = pa_binary_expr( E_ESTIMATE, $2, $4 ); 
		    )}
	    ;

srchpred:	expression_notnull  comp_op  expression_notnull
		    {T( $$ = pa_binary_expr( (an_expr_type)$2, $1, $3 ); )}
	    |	expression_notnull  comp_op  quantified_subquery
		    {T( $$ = pa_binary_expr( (an_expr_type)$2, $1, $3 ); )}
	    |	T_EXISTS  subquery
		    {T( $$ = pa_unary_expr( E_EXISTS, $2 ); )}
	    |	expression_notnull  not  T_IN  single_subquery
		    { /* no UNIONS in subquery - grammar conflicts */
		   T( $$ = pa_binary_expr( E_EQ,
					   $1,
					   pa_unary_expr( E_ANY, $4 ) );
		      if( $2 ){
			  $$ = pa_unary_expr( E_NOT, $$ );
		      }
		    )}
	    |	expression_notnull  not  T_IN  T_LPAREN  vallist  T_RPAREN
		    {T( $$ = pa_binary_expr( E_IN, $1, $5 );
		      if( $2 ){
			  $$ = pa_unary_expr( E_NOT, $$ );
		      }
		    )}
	    |	expression_notnull  not  T_BETWEEN  expression  T_AND expression
		    {T( $$ = pa_ternary_expr( E_BETWEEN, $1, $4, $6 ); 
		      if( $2 ){
		          $$ = pa_unary_expr( E_NOT, $$ );
		      }
		    )}
	    |	expression  T_IS  not  T_NULL
		    {T( $$ = pa_unary_expr( E_ISNULL, $1 );
		      if( $3 ){
			  $$ = pa_unary_expr( E_NOT, $$ );
		      }
		    )}
	    |	expression_notnull  not  T_EXISTS
		    /* NOTE: the above expression must be a colref but
		       it causes a conflict
		    */
	            {  pa_specific( L_PACE ); 
		    T( pa_must_be_column( $1 );
		       $$ = pa_unary_expr( E_FKEYEXISTS, $1 );
		       if( $2 ){
		 	   $$ = pa_unary_expr( E_NOT, $$ );
		       }
		    )}
	/*
	    |   T_USING T_PRIMARY T_KEY T_FOR corrname
		    {T( $$ = pa_unary_expr( E_PKEYUSED, $5 ); )}
	*/
	    |	expression_notnull  not  T_LIKE  expression_notnull
		    {T( $$ = pa_like( $1, $4 );
		      if( $2 ) {
			  $$ = pa_unary_expr( E_NOT, $$ );
		      }
		    )}
	    ;

comp_op:	T_EQ
		    {T( $$ = E_EQ; )}
	    |	T_NE
		    {T( $$ = E_NE; )}
	    |	T_GT
		    {T( $$ = E_GT; )}
	    |	T_GE
		    {T( $$ = E_GE; )}
	    |	T_LT
		    {T( $$ = E_LT; )}
	    |	T_LE
		    {T( $$ = E_LE; )}
	    ;

quantified_subquery:	quantifier  subquery
		    {T( $$ = pa_unary_expr( (an_expr_type)$1, $2 ); )}
	    ;

quantifier:	T_ALL
		    {T( $$ = E_ALL; )}
	    |	T_ANY
		    {T( $$ = E_ANY; )}
	    |	T_SOME
		    {T( $$ = E_ANY; )}
	    ;

vallist:	vallistrev  optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

vallistrev:	valitem
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	vallistrev  T_COMMA  valitem
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

valitem:	expression
	    ;

ins_vallist:	ins_vallistrev	optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

ins_vallistrev: ins_valitem
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	ins_vallistrev	T_COMMA  ins_valitem
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

ins_valitem:	expression
	    ;

not:		T_NOT
		    {T( $$ = TRUE; )}
	    |	/* empty */
		    {T( $$ = FALSE; )}
	    ;

groupby:	/* empty */
		    {T( $$ = NULL; )}
	    |	T_GROUP  T_BY  vallist
		    {T( $$ = $3; )}
	    ;

having: 	/* empty */
		    {T( $$ = NULL; )}
	    |	T_HAVING searchcond
		    {T( $$ = $2; )}
	    ;

/*	Moved to rnsql.y
delete: 	T_DELETE  T_FROM  tabname  where
		    {T( $$ = pa_delete( $3, $4, NULL ); )}
	    |	T_DELETE  T_FROM  tabname  wherecurrentof
		    {T( $$ = pa_delete( $3, NULL, $4 ); )}
	    ;
*/

/* Following is used by the delete rule */

runid_spec:	string
	    |   hostvar
	    |   T_IDENTIFIER
	            {T( $$ = pa_unary_expr( E_STRING, $1 ); )}
	    ;
	    
wherecurrentof: T_WHERE  T_CURRENT  {SQLStmt=FALSE;}  T_OF  T_IDENTIFIER
		    {T( $$ = $5; )}
	    ;

update: 	T_UPDATE  tabspec  sets  where
		    {T( if( !pa_onetable( $2 ) ){
		          pa_specific( L_PACE );
		      }
		      $$ = pa_update( $2, $3, $4, NULL ); 
		    )}
	    |	T_UPDATE  tabspec  sets  wherecurrentof
		    { pa_specific( L_PACE );
		    T( if( !pa_onetable( $2 ) ){
		          pa_specific( L_PACE );
		      }
		      $$ = pa_update( $2, $3, NULL, $4 ); 
		    )}
	    ;

sets:		T_SET  setlist	optcomma
		    {T( $$ = pa_reverse_plist( $2 ); )}
	    ;

setlist:	setitem
		    {T( $$ = pa_iadd_plist( NULL, $1 ); )}
	    |	setlist  T_COMMA  setitem
		    {T( $$ = pa_iadd_plist( $1, $3 ); )}
	    ;

setitem:	colref	T_EQ  expression
		    {T( $$ = pa_setitem( $1, $3 ); )}
	    ;

insert: 	T_INSERT  T_INTO  tabname  columns  insertvalues
		    {T( $$ = pa_insert( $3, $4, $5, NULL ); )}
	    |	T_INSERT  T_INTO  tabname  columns  query_expr
		    {T( $$ = pa_insert( $3, $4, NULL, $5 ); )}
	    ;

insertvalues:	T_VALUES T_LPAREN ins_vallist T_RPAREN
		    {T( $$ = $3; )}
	    ;

columns:	/* empty */
		    {T( $$ = NULL; )}
	    |	T_LPAREN  T_RPAREN
		    {T( $$ = NULL; )}
	    |	T_LPAREN collist  T_RPAREN
		    {T( $$ = $2; )}
	    ;

collist:	collistrev  optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

collistrev:	colname
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	collistrev  T_COMMA  colname
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

optcomma:	/* empty */
	    |	T_COMMA
	    ;

createtable:	T_CREATE  T_TABLE  tabname  
                T_LPAREN  tblelmtlist  optcomma  T_RPAREN inclause  
		    {T( $$ = pa_create_table( $5, $3, $8 ); )}
	    ;

inclause:	/* empty */
		    {T( $$ = NULL; )}
	    |	T_IN  dbspacename
		    {T( $$ = $2; )}
	    ;


tblelmtlist:	createclause
		    {T( $$ = pa_table_element( NULL, $1 ); )}
	    |	tblelmtlist  T_COMMA  createclause
		    {T( $$ = pa_table_element( $1, $3 ); )}
	    ;

createclause:	coldefn
		    {T( $$.ptr = $1;
		      $$.info = PI_COLDEFN;
		    )}
	    |	T_UNIQUE  T_LPAREN collist  T_RPAREN
		    {  pa_specific( L_PACE );
		    T( $$.info = PI_UNIQUE;
		       $$.ptr = $3;
		    )}
	    |	T_PRIMARY  T_KEY  T_LPAREN collist  T_RPAREN
		    {  pa_specific( L_PACE );
		    T( $$.info = PI_PRIMARYKEY;
		       $$.ptr = $4;
		    )}
	    |	rolename  notnull  T_FOREIGN  T_KEY  T_FOR
		tabname fgnkeycols checkclause
		    {  pa_specific( L_PACE );
		    T( $$.info = PI_FOREIGNKEY;
		       $$.ptr = pa_foreign_key( $1, $2, $8, $6, $7 );
		    )}
	    |	rolename  notnull  T_FOREIGN  T_KEY  rolename  notnull 
	        fgnkeycols  T_REFERENCES  tabname
		    {  pa_specific( L_PACE ); }
	    ;

coldefn:	colname  data_type  notnull  unique
		    {T( $$ = pa_coldefn( $1, $2, !$3, $4 ); )}
	    ;

notnull:	/* empty */
		    {T( $$ = FALSE; )}
	    |	T_NOT  T_NULL
		    {T( $$ = TRUE; )}
	    ;

checkclause:	T_CHECK  T_ON  T_COMMIT
		    {T( $$ = TRUE; )}
	    |	/* empty */
		    {T( $$ = FALSE; )}
	    ;

rolename:	/* empty */
		    {T( $$ = NULL; )}
	    |	colname
	    ;

fgnkeycols:	/* empty */
		    {T( $$ = NULL; )}
	    |	T_LPAREN  fgnkeycollist  optcomma  T_RPAREN
		    {T( $$ = $2; )}
	    ;

fgnkeycollist:	fgnkeycol
		    {T( $$ = pa_iadd_plist( NULL, $1 ); )}
	    |	fgnkeycollist  T_COMMA  fgnkeycol
		    {T( $$ = pa_iadd_plist( $1, $3 ); )}
	    ;

fgnkeycol:	colname  T_IS  colname
		    {T( $$ = pa_foreign_key_column( $1, $3 ); )}
	    ;

createindex:	T_CREATE  unique  T_INDEX  indexname  T_ON  tabname
		T_LPAREN indexlist  T_RPAREN
		    {T( $$ = pa_create_index( $2, $4, $6, $8 ); )}
	    ;

unique: 	/* empty */
		    {T( $$ = FALSE; )}
	    |	T_UNIQUE
		    {T( $$ = TRUE; )}
	    ;

indexlist:	indexlistrev  optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

indexlistrev:	indexcol
		    {T( $$ = pa_iadd_plist( NULL, $1 ); )}
	    |	indexlistrev  T_COMMA  indexcol
		    {T( $$ = pa_iadd_plist( $1, $3 ); )}
	    ;

indexcol:	colname  indexattrlist
		    {T( $$ = pa_index_column( $1, $2 ); )}
	    ;

indexattrlist:	/* empty */
	            { $$ = IA_ASC; }
            |	indexattrlist1
	            { $$ = $1; }
	    ;

indexattrlist1:	indexattr
	            { $$ = $1; }
	    |	indexattrlist1  indexattr
	    	    { $$ = ( $1 | $2 );
		      if( ($$ & IA_ASC)  &&  ($$ & IA_DESC)){
		          sql_errors( SQLE_SYNTAX_ERROR, "DESC" );
		      }
		    }
	    ;

indexattr:	T_ASC
	    	    { $$ = IA_ASC; }
            |   T_DESC
	            { $$ = IA_DESC; }
	    ;

createview:	T_CREATE  T_VIEW  tabname  columns  T_AS  query_expr
		    {T( $$ = pa_create_view( $3, $4, $6 ); )}
	    ;

alter:		T_ALTER  T_TABLE  tabname  alter_clause
		    {T( $$ = pa_alter_table( $3, $4 ); )}
	    ;

alter_clause:	T_ADD  createclause
		    {T( $$ = pa_alter_clause( AC_ADD, $2, NULL ); )}
	    |	T_MODIFY  modcoldefn
		    {  a_parser_info	i;
		       pa_specific( L_PACE | L_XQL );
		    T( i.info = PI_COLDEFN;
		       i.ptr = $2;
		       $$ = pa_alter_clause( AC_MODIFY, i, NULL );
		    )}
	    |	alter_delete  colname
		    {  a_parser_info	i;
		       pa_specific( L_XQL | L_PACE );
		    T( i.info = PI_COLDEFN;
		       i.ptr = pa_coldefn( $2, NULL, FALSE, FALSE );
		       $$ = pa_alter_clause( AC_DELETE, i, NULL );
		    )}
	    |	alter_delete  T_FOREIGN  T_KEY  T_IDENTIFIER
		    {  a_parser_info	i;
		       pa_specific( L_PACE );
		    T( i.info = PI_FOREIGNKEY;
		       i.ptr = $4;
		       $$ = pa_alter_clause( AC_DELETE_FKEY, i, NULL );
		    )}
	    |	alter_delete  T_PRIMARY  T_KEY
		    {  a_parser_info	i;
		       pa_specific( L_PACE );
		    T( i.info = PI_PRIMARYKEY;
		       $$ = pa_alter_clause( AC_DELETE_PKEY, i, NULL );
		    )}
	    |	T_RENAME  T_IDENTIFIER
		    {  a_parser_info	i;
		       pa_specific( L_PACE );
		    T( i.info = PI_NONE;
		       i.ptr = $2;
		       $$ = pa_alter_clause( AC_NEWNAME, i, NULL );
		    )}
	    |	T_RENAME  T_IDENTIFIER	T_TO  T_IDENTIFIER
		    {  a_parser_info	i;
		       pa_specific( L_PACE );
		    T( i.info = PI_NONE;
		       i.ptr = $2;
		       $$ = pa_alter_clause( AC_RENAME, i, $4 );
		    )}
	    ;

alter_delete:	T_DELETE
	    |	T_DROP
	    ;

modcoldefn:	coldefn
	    |	colname  T_NOT	T_NULL
		    {T( $$ = pa_coldefn( $1,
				       pa_data_type( DT_NOTYPE, 0L, 0 ),
				       FALSE, FALSE ); )}
	    |	colname  T_NULL
		    {T( $$ = pa_coldefn( $1,
				       pa_data_type( DT_NOTYPE, 0L, 0 ),
				       TRUE, FALSE ); )}
	    ;

data_type:	char_type  T_LPAREN  integer  T_RPAREN   
		    {T( $$ = pa_data_type( $1, ((an_integer *)$3)->value, 0 );)}
    	    |   char_type
		    {T( $$ = pa_data_type( $1, 1L, 0 ); )}
	    |	T_LONG	T_VARCHAR
		    {T( $$ = pa_data_type( DT_LONGVARCHAR, -1L, 0 ); )}
	    |	integer_type
		    {T( $$ = pa_data_type( $1, 0L, 0 ); )}
	    |	exact_num_type	T_LPAREN  integer  T_COMMA  integer  T_RPAREN
		    {T( $$ = pa_data_type( $1,
				 ((an_integer *)$3)->value,
				 (unsigned short)((an_integer *)$5)->value ); )}
	    |	exact_num_type	T_LPAREN  integer  T_RPAREN
		    {T( $$ = pa_data_type( $1, ((an_integer *)$3)->value, 0 );)}
	    |	exact_num_type
		    {T( $$ = pa_data_type( $1, -1L, 0 ); )}
	    |	float_type
		    {T( $$ = pa_data_type( $1, 0L, 0 ); )}
	    |	float_type  T_LPAREN  integer  T_RPAREN
		    {T( $$ = pa_data_type( $1, ((an_integer *)$3)->value, 0 );)}
	    |	T_DATE
		    {T( $$ = pa_data_type( DT_DATE, 0L, 0 ); )}
	    ;

char_type:	T_CHAR
		    {T( $$ = DT_FIXCHAR; )}
	    |	T_VARCHAR
		    {T( $$ = DT_VARCHAR; )}
	    ;

integer_type:	T_INTEGER
		    {T( $$ = DT_INT; )}
	    |	T_SMALLINT
		    {T( $$ = DT_SMALLINT; )}
	    ;

exact_num_type: T_DECIMAL
		    {T( $$ = DT_DECIMAL; )}
	    |	T_NUMERIC
		    {T( $$ = DT_DECIMAL; )}
	    ;

float_type: 	T_FLOAT
		    {T( $$ = DT_FLOAT; )}
	    |	T_REAL
		    {T( $$ = DT_FLOAT; )}
	    |	T_DOUBLE
		    {T( $$ = DT_DOUBLE; )}
	    |	T_DOUBLE  T_PRECISION
		    {T( $$ = DT_DOUBLE; )}
	    ;

number: 	T_NUMBER
		    {T( $$ = pa_number( $1 ); )}
	    |   T_LONGINT
		    {T( $$ = pa_integer( $1 ); )}
	    ;

integer:	T_LONGINT
		    { $$ = pa_integer( $1 ); }
	    ;

commenton:      T_COMMENT  T_ON  T_TABLE  tabname  T_IS  runid_spec
		    { pa_specific( L_SAA ); }
	    |	T_COMMENT  T_ON  T_COLUMN  colref  T_IS  runid_spec
		    { pa_specific( L_SAA ); }
	    ;

lock:		T_LOCK  T_TABLE  tabname  T_IN  lockmode 
		    { pa_specific( L_SAA ); }
	    ;

lockmode:	T_SHARE
	    |	T_EXCLUSIVE
	    ;

droptable:	T_DROP	T_TABLE  tabname  
		    {T( $$ = pa_droptable( $3 ); )}
	    ;

dropindex:	T_DROP	T_INDEX  indexname  
		    {T( $$ = pa_dropindex( $3 ); )}
	    ;

dropview:	T_DROP	T_VIEW	tabname
		    {T( $$ = pa_dropview( $3 ); )}
	    ;

grant:		granttok  tableprivileges  T_ON  tabname
			T_TO  userlist	grantoption
		    {T( $$ = pa_grant( PRIVT_TABLE, (a_privilege)$2, $4,
				     update_collist, $6, $7 ); )}
	    /*
	    |	granttok  T_RUN  T_ON  progname  T_TO  userlist  grantoption
		    {  pa_specific( L_PACE );
		    T( $$ = pa_grant( PRIVT_RUN, (a_privilege)0, $4,
				     NULL, $6, $7 ); 
		    )}
	    */
	    |	granttok  specialpriv  T_TO  userlist  passwdlist
		    {  pa_specific( L_PACE | L_XQL );
		    T( $$ = pa_grant( PRIVT_SPECIAL, (a_privilege)$2, NULL,
				     $5, $4, FALSE ); 
		    )}
	    ;

granttok:	T_GRANT
		    {T( update_collist = NULL; )}
	    ;


tableprivileges:T_ALL
		    {T( $$ = PRIV_ALL; )}
	    |	T_ALL  T_PRIVILEGES
		    {  pa_specific( L_PACE );
		    T( $$ = PRIV_ALL; 
		    )}
	    |	tableprivlist  optcomma
	    ;

tableprivlist:	tablepriv
	    |	tableprivlist  T_COMMA  tablepriv
		    {T( $$ = ( $1 | $3 ); )}
	    ;

tablepriv:	T_ALTER
		    {T( $$ = PRIV_ALTER; )}
	    |	T_DELETE
		    {T( $$ = PRIV_DELETE; )}
	    |	T_INDEX
		    {T( $$ = PRIV_INDEX; )}
	    |	T_INSERT
		    {T( $$ = PRIV_INSERT; )}
	    |	T_SELECT  priv_columns
		    {  if( $2 != NULL ){
		 	   pa_specific( L_PACE | L_XQL );
		       }
		    T( $$ = PRIV_SELECT; )}
	    |	T_UPDATE  priv_columns
		    {  if( $2 != NULL ){
		 	   pa_specific( L_PACE | L_XQL );
		       }
		    T( if( $2 != NULL ){
			   $$ = PRIV_UPDATECOL;
		       }else{
			   $$ = PRIV_UPDATE;
		       }
		       update_collist = $2;
		    )}
	    ;

priv_columns:   columns
	    |	collist 
	    ;


specialpriv:	T_CONNECT
		    {T( $$ = PRIV_CONNECT; )}
	    |	T_DBA
		    {T( $$ = PRIV_DBA; )}
	    |	T_RESOURCE
		    {T( $$ = PRIV_RESOURCE; )}
	    |	T_SCHEDULE
		    {T( $$ = PRIV_SCHEDULE; )}
	    ;

grantoption:	/* empty */
		    {T( $$ = FALSE; )}
	    |	T_WITH	T_GRANT  T_OPTION
		    {T( $$ = TRUE; )}
	    ;

userlist:	userlistrev  optcomma
		    {T( $$ = pa_reverse_plist( $1 ); )}
	    ;

userlistrev:	userid
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	userlistrev  T_COMMA  userid
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

userid: 	T_IDENTIFIER
	    |	T_PUBLIC
		    {T( $$ = NULL; )}
	    ;


passwdlist:	/* empty */
		    {T( $$ = NULL; )}
	    |	T_IDENTIFIED  T_BY  passwdlistrev  optcomma
		    {T( $$ = pa_reverse_plist( $3 ); )}
	    ;

passwdlistrev:	password
		    {T( $$ = pa_add_plist( NULL, $1 ); )}
	    |	passwdlistrev  T_COMMA  password
		    {T( $$ = pa_add_plist( $1, $3 ); )}
	    ;

password:	T_IDENTIFIER
	    ;

revoke: 	revoketok  tableprivileges  T_ON  tabname  T_FROM  userlist
		    {T( $$ = pa_revoke( PRIVT_TABLE, (a_privilege)$2, $4,
				      update_collist,$6 );)}
	    /*
	    |	revoketok  T_RUN  T_ON	progname  T_FROM  userlist
		    {  pa_specific( L_PACE );
		    T( $$ = pa_revoke( PRIVT_RUN, (a_privilege)0, $4,
				      NULL, $6 ); 
		    )}
	    */
	    |	revoketok  specialpriv	T_FROM	userlist
		    {  pa_specific( L_PACE | L_XQL );
		    T( $$ = pa_revoke( PRIVT_SPECIAL, (a_privilege)$2, NULL,
				      NULL, $4 ); 
		    )}
	    ;

revoketok:	T_REVOKE
		    {T( update_collist = NULL; )}
	    ;

dbspacename:	T_IDENTIFIER
		    {T( $$ = pa_dbspace( NULL, $1 ); )}
	    |	T_IDENTIFIER  T_DOT  T_IDENTIFIER
		    {T( $$ = pa_dbspace( $1, $3 ); )}
	    ;

commit: 	T_COMMIT  T_WORK  T_RELEASE
		    { $$ = pa_commit( TRUE ); }
	    |	T_COMMIT  T_WORK
		    { $$ = pa_commit( FALSE ); }
	    |	T_COMMIT  
		    { $$ = pa_commit( FALSE ); }
	    ;

rollback:	T_ROLLBACK  T_WORK  T_RELEASE
		    { $$ = pa_rollback( TRUE ); }
	    |	T_ROLLBACK  T_WORK
		    { $$ = pa_rollback( FALSE ); }
	    |	T_ROLLBACK  
		    { $$ = pa_rollback( FALSE ); }
	    ;


sqlprogram:     stmt
            |   sqlprogram stmt
            ;

stmt:		validstmt  T_SEMI_COLON
		    { pa_end_instr( TRUE ); }
	    |   error  errortoklist  T_SEMI_COLON
		    { pa_end_instr( TRUE ); 
	    	      yyerrok; }
	    ;

validstmt:      execsql  embstmt 
            |   execsql  runstmt
	    ;

embstmt:	embsqlstmt
		    /* taken out because of garbage at the end of statement */
		    /* { pa_end_instr( TRUE ); } */
            ;

runstmt:	sqlstmt
		    { if( SQLStmt ){
		          pa_sqlstmt();
		      }
		    /* taken out because of garbage at the end of statement */
		      /* pa_end_instr( TRUE ); */
		    }
	    ;

errortoklist:	toklist
		    { if( SQLStmt ){
		          pa_sqlstmt();
		      }
		    /* taken out because of garbage at the end of statement */
		      /* pa_end_instr( TRUE ); */
		    }
	    ;

toklist:	/* empty */                
	    |   toklist  token
	    ;

token:		T_TOK
            |   hostvar  
            |   T_COMMA  
	    |   selinto2  
	    ;

execsql:	T_EXECSQL
		    { $$ = pa_start_instr(); 
		      SQLStmt = TRUE; 
		    }
	    ;
	     
embsqlstmt:     { SQLStmt = FALSE; }  close
            |   { SQLStmt = FALSE; }  connect
	    |	{ SQLStmt = FALSE; }  declarations
            |   { SQLStmt = FALSE; }  declare
            |   { SQLStmt = FALSE; }  describe
            |                         dropstmt
            |   { SQLStmt = FALSE; }  execute
            |   { SQLStmt = FALSE; }  fetch
	    |   { SQLStmt = FALSE; }  getoption
            |   { SQLStmt = FALSE; }  include
            |   { SQLStmt = FALSE; }  open
            |   { SQLStmt = FALSE; }  prepare
            |                         put
            |   { SQLStmt = FALSE; }  reopen
	    |   { SQLStmt = FALSE; }  setoption
            |   		      updatecursor
            |   { SQLStmt = FALSE; }  whenever
            ;

selinto:        selinto2
            |   /* empty */
	    	    { $$ = NULL; }
            ;

selinto2:       T_INTO  { $<ptr>$ = pa_into_clause_start(); }  intohostlist  
		    { pa_into_clause_done();
		      $$ = $3;
		    }
	    ;

close:          T_CLOSE  id_spec
		    { $$ = pa_close( $2 ); }
            ;

defuid:		T_DEFAULT  T_USER  id_spec
 		    { $$ = $3; }
	    |   /* empty */
	            { $$ = NULL; }
	    ;

database:	T_DATABASE  id_spec
 		    { $$ = $2; }
	    |   /* empty */
	            { $$ = NULL; }
	    ;

dictionary:	T_DICTIONARY  id_spec
 		    { $$ = $2; }
	    |   /* empty */
	            { $$ = NULL; }
	    ;

connect:        T_CONNECT  id_spec  T_IDENTIFIED  T_BY  id_spec  
		defuid  dictionary  database  
		    { $$ = pa_connect( $2, $5, $6, $8, $7 ); }
            ;


open:           T_OPEN  id_spec  using_sqlda
		    { $$ = pa_open( $2, $3 ); }
            ;

reopen:         T_REOPEN  id_spec  
		    { $$ = pa_reopen( $2 ); }
            ;

using_sqlda:     /* empty */
		    { $$ = NULL; }
	    |	using_sqlda_ne
	    ;
	    
using_sqlda_ne: T_USING  T_DESCRIPTOR  structure_spec
                    { $$ = pa_using_info( UT_SQLDA, $3 ); }
            |   T_USING  indhostlist
                    { $$ = pa_using_info( UT_HOSTLIST, $2 ); }
            ;

structure_spec: T_IDENTIFIER
            ;

fetch:          T_FETCH  position  id_spec  T_INTO  intohostlist
                    { a_using_info		*ui;
		      ui = pa_using_info( UT_HOSTLIST, $5 );
		      $$ = pa_fetch( $3, $2, ui );
		    }
            |   T_FETCH  position  id_spec  using_sqlda
		    { $$ = pa_fetch( $3, $2, $4 ); }
            ;

offset:		T_HOSTVAR
                    { $$ = pa_runhostvar( $1, NULL ); }
	    |   integer
	    |	T_MINUS  integer
	            { an_integer		*i;
		      i = $2;
		      i->value = -(i->value);
		      $$ = i;
		    }
	    ;
position:    	/* empty */
	            { $$.info = D_RELATIVE;
		      $$.ptr = pa_makeint( 1L );
		    }
	    |	direction  offset  
	            { pa_ppspecific( L_SAAEXT );
		      $$.info = $1;
		      $$.ptr = $2;
		    }
	    |   T_NEXT
	            { pa_ppspecific( L_SAAEXT );
		      $$.info = D_RELATIVE;
		      $$.ptr = pa_makeint( 1L );
		    }
	    |   T_PRIOR
	            { pa_ppspecific( L_SAAEXT );
		      $$.info = D_RELATIVE;
		      $$.ptr = pa_makeint( -1L );
		    }
	    |   T_FIRST
	            { pa_ppspecific( L_SAAEXT );
		      $$.info = D_ABSOLUTE;
		      $$.ptr = pa_makeint( 1L );
		    }
	    |   T_LAST
	            { pa_ppspecific( L_SAAEXT );
		      $$.info = D_ABSOLUTE;
		      $$.ptr = pa_makeint( -1L );
		    }
	    ;
	    
direction:	T_ABSOLUTE
	    	    { $$ = D_ABSOLUTE; }
	    |   T_RELATIVE
	    	    { $$ = D_RELATIVE; }
	    ;

intohostlist:   intohostvar
                    { $$ = pa_add_list( NULL, $1 ); }
            |   intohostlist  T_COMMA  intohostvar
                    { $$ = pa_add_list_end( $1, $3 ); }
            ;

indhostlist:    indhostvar
                    { $$ = pa_add_list( NULL, $1 ); }
            |   indhostlist  T_COMMA  indhostvar
                    { $$ = pa_add_list_end( $1, $3 ); }
            ;

put:            putkwd  id_spec  using_sqlda
    		    { pa_ppspecific( L_SAAEXT );
		      $$ = pa_put( $2, $3 ); 
		    }
            |   putkwd  id_spec  T_FROM  indhostlist
    		    { pa_ppspecific( L_SAAEXT );
    		      $$ = pa_using_info( UT_HOSTLIST, $4 );
		      $$ = pa_put( $2, $$ ); 
		    }
            ;

putkwd:		{SQLStmt=TRUE;}  T_PUT
	    |	T_INSERT  T_INTO  {SQLStmt=TRUE;}  T_CURSOR 
	    ;

t_update:       T_UPDATE  {SQLStmt=FALSE;}  T_WHERE  T_CURRENT  T_OF
            ;

updatecursor:	t_update  id_spec  using_sqlda_ne
		    { $$ = pa_update_cursor( $2, $3 ); }
            |	t_update  id_spec  T_FROM  indhostlist
		    { $$ = pa_using_info( UT_HOSTLIST, $4 );
		      $$ = pa_update_cursor( $2, $$ ); 
		    }
	    ;

onetabspec:	T_IDENTIFIER  T_DOT  T_IDENTIFIER
		    { $$ = pa_table( $3, $1 );}
	    |	T_IDENTIFIER
		    { $$ = pa_table( $1, NULL );}
 	    ;

t_delete:	T_DELETE
 	    ;
	    
delete:		t_delete  {SQLStmt=FALSE;}  T_WHERE  T_CURRENT  T_OF  id_spec
		    { $$ = pa_delete_cursor( $6, NULL ); }
	    |   t_delete  T_FROM  onetabspec 
	        T_WHERE  T_CURRENT  {SQLStmt=FALSE;}  T_OF id_spec
		    { $$ = pa_delete_cursor( $8, $3 ); 
		      // {T( $$ = pa_delete( $3, NULL, $8 ); )}
		    }
	    |   t_delete  T_FROM  onetabspec  where
		    {T( $$ = pa_delete( $3, $4, NULL ); )}
	    ;

scroll:		T_SCROLL
                    { pa_ppspecific( L_SAAEXT | L_ANSI2 ); }
	    |	/* empty */
	    ;

updatability_clause: /* empty */
            |   T_FOR  T_READ  T_ONLY
	            { pa_ppspecific( L_ANSI2 ); }
	    |   T_FOR  T_UPDATE  
	            { pa_ppspecific( L_ANSI2 ); }
	    |   T_FOR  T_UPDATE  T_OF  collist
	            { pa_ppspecific( L_ANSI2 ); }
	    ;

declare_tok:	T_DECLARE
		    { New_Buffer();
		      Stop_Buffer();
		    }
	    ;
	    
declare:        declare_tok  id_spec  scroll  T_CURSOR  T_FOR  select_stmt 
                updatability_clause
		    { /* declare with order by */
		      $$ = pa_declare_select( $2, $6 ); 
		    }
            |   declare_tok  id_spec  scroll  T_CURSOR  T_FOR  insert_stmt
                updatability_clause
	    	    { $$ = pa_declare_insert( $2, $6 ); }
            |   declare_tok  id_spec  scroll  T_CURSOR  T_FOR  statement_spec
                updatability_clause
	    	    { $$ = pa_declare_stmt( $2, $6, 
		                ((a_statement_spec *)$6)->program, NULL ); }
            ;

select_stmt:	select
		    {T( $$ = pa_make_stmt( STMT_SELECT, $1 ); )}
	    ;
	    
insert_stmt:	insertvalues
                    {T( $$ = pa_make_stmt( STMT_INSERT, $1 ); )}
	    ;
	    
statement_spec:	T_IDENTIFIER
      		    { $$ = pa_statement_spec( NULL, $1, NULL ); }
	    |   embhostvar
	            { $$ = pa_statement_spec( NULL, NULL, $1 ); } 
	    |   embhostvar  T_IN  ppprogname
	            { $$ = pa_statement_spec( $3, NULL, $1 ); } 
	    ;
	    
describe:       T_DESCRIBE  describe_type  statement_spec
			T_INTO  structure_spec desc_using
		    { $$ = pa_describe( $3, $2, $5, $6 ); }
            ;

describe_type:	/* empty */
		    { $$ = DESCT_SELECTLIST; }
	    |   T_SELECT  T_LIST  T_FOR
		    { pa_ppspecific( L_SAAEXT );
		      $$ = DESCT_SELECTLIST; 
		    }
	    |   T_BIND  T_VARIABLES  T_FOR
		    { pa_ppspecific( L_SAAEXT );
		      $$ = DESCT_BINDVARIABLES; 
		    }
	    ;
	    
desc_using:     /* empty */
                    { $$ = DU_NAMES; }
            |   T_USING  desc_using_element
	            { pa_ppspecific( L_PACE );
		      $$ = $2; 
		    }
            ;

desc_using_element:     T_NAMES
		    { $$ = DU_NAMES; }
            |   T_LABELS
		    { $$ = DU_LABELS; }
            |   T_BOTH
		    { $$ = (DU_NAMES | DU_LABELS); }
            |   T_ANY
		    { $$ = DU_ANY; }
            ;

prepare:        T_PREPARE  statement_spec  T_FROM  string_spec  prepusing
                    { $$ = pa_prepare( $2, $4, $5 ); }
            |   T_PREPARE  T_FROM  string_spec  
	            T_SETTING  statement_spec  prepusing
                    { pa_ppspecific( L_SAAEXT );
		      $$ = pa_prepare( $5, $3, $6 ); 
		    }
            ;

dropstmt:	T_DROP  {SQLStmt=FALSE;}  T_STATEMENT  statement_spec 
		    { $$ = pa_dropstmt( $4 ); }
            ;

ppprogname:	id_spec
 		    { $$ = pa_ppprogname( NULL, $1 ); }
	    |   id_spec  T_DOT  id_spec
 		    { $$ = pa_ppprogname( $1, $3 ); }
	    ;

prepusing:	/* empty */
		    { $$ = NULL; }
	    |   T_USING  T_DESCRIPTOR  T_IDENTIFIER
	            { $$ = $3; }
	    ;

execute:        T_EXECUTE  statement_spec  using_sqlda
		    { $$ = pa_execute( $2, $3 ); }
            |   T_EXECUTE  T_IMMEDIATE  string_spec
	    	    { $$ = pa_execute_imm( $3 ); }
            ;

string_spec:    T_STRING
                    { $$ = pa_string_spec( ST_STRING, $1 ); }
            |   embhostvar
	            { $$ = pa_string_spec( ST_HOSTVAR, $1 ); }
            ;

id_spec:	T_STRING
                    { $$ = pa_string_spec( ST_STRING, $1 ); }
            |   embhostvar
	            { $$ = pa_string_spec( ST_HOSTVAR, $1 ); }
	    |	T_IDENTIFIER
	    	    { $$ = pa_string_spec( ST_STRING, $1 ); }
            ;

id_indspec:	T_STRING
                    { $$ = pa_string_spec( ST_STRING, $1 ); }
            |   indhostvar
	            { $$ = pa_string_spec( ST_HOSTVAR, $1 ); }
	    |	T_IDENTIFIER
	    	    { $$ = pa_string_spec( ST_STRING, $1 ); }
            ;

hostvar:        T_HOSTVAR
                    { $$ = pa_runhostvar( $1, NULL ); }
            |   T_HOSTVAR  T_HOSTVAR
	            { $$ = pa_runhostvar( $1, $2 ); }
            ;
	     
embhostvar:     T_HOSTVAR
                    { $$ = pa_embhostvar( $1, NULL, FALSE ); }
	    ;
	    
indhostvar:     T_HOSTVAR
                    { $$ = pa_embhostvar( $1, NULL, FALSE ); }
            |   T_HOSTVAR  T_HOSTVAR
	            { $$ = pa_embhostvar( $1, $2, FALSE ); }
            ;
	     
intohostvar:    T_HOSTVAR
                    { $$ = pa_embhostvar( $1, NULL, TRUE ); }
            |   T_HOSTVAR  T_HOSTVAR
	            { $$ = pa_embhostvar( $1, $2, TRUE ); }
            ;
	     
whenever:       T_WHENEVER  T_SQLERROR  when_action
                    { pa_set_action( A_ERROR, $3 ); }
            |   T_WHENEVER  T_SQLWARNING  when_action
                    { pa_set_action( A_WARNING, $3 ); }
            |   T_WHENEVER  notfound  when_action
                    { pa_set_action( A_NOTFOUND, $3 ); }
            ;

notfound:	T_NOTFOUND
                    { pa_ppspecific( L_SAAEXT ); }
	    |	T_NOT  T_FOUND
	    ;
	    
when_action:    when_action2
                    { $$ = $1; }
            |   T_STOP
                    { pa_ppspecific( L_SAAEXT );
		      strcpy( buff, "sqlstop( &sqlca )" );
                      $$ = buff;
                    }
	    |	T_CBLOCK
	            { pa_ppspecific( L_SAAEXT );
		      $$ = $1;
		    }
            ;

when_action2:   goto  T_IDENTIFIER
                    { sprintf( buff, "goto %s", $2 );
                      $$ = buff;
                    }
            |   T_CONTINUE
                    { $$ = NULL; }
            ;

goto:		T_GOTO
	    |	T_GO  T_TO
	    ;

include:	T_INCLUDE  T_IDENTIFIER
		    { $$ = pa_include( $2 ); }
            ;

/* declarations:   begin_decl  
            |	end_decl
            ;
*/

declexecsql:	T_EXECSQL
	    ;

declarations:   begin_decl  T_SEMI_COLON  var_decl_list  declexecsql  end_decl
            ;

begin_decl:     T_BEGIN  T_DECLARE  T_SECTION
            ;

end_decl:       T_END  T_DECLARE  T_SECTION
		    { LineInfo(); }
            ;

var_decl_list:  /* empty */
            |   var_decl_list  var_decl
	            { 
		      pa_end_instr( FALSE );
		      pa_start_instr();
		    }
            ;

var_decl:       { hostvar_type = 0; }  type_specifier_list  declarator_list  T_SEMI_COLON
            |
	        error T_SEMI_COLON
            ;

type_specifier_list:  type_specifier
	    |   type_specifier_list  type_specifier
	    ;
	    
type_specifier: T_CHAR           	{ hostvar_type |= HT_CHAR; }
            |   T_VARCHAR        	{ hostvar_type |= HT_VARCHAR; }
            |   T_DATE        		{ hostvar_type |= HT_DATE; }
            |   T_FIXCHAR        	{ hostvar_type |= HT_FIXCHAR; }
            |   T_INTEGER        	{ hostvar_type |= HT_INT; }
            |   T_SHORT  		{ hostvar_type |= (HT_SHORT|HT_INT); }
            |   T_LONG  		{ hostvar_type |= (HT_LONG|HT_INT); }
            |   T_UNSIGNED       	{ hostvar_type |=(HT_UNSIGNED|HT_INT); }
            |   T_FLOAT          	{ hostvar_type |= HT_FLOAT; }
            |   T_DOUBLE         	{ hostvar_type |= (HT_FLOAT|HT_LONG); }
            |   T_DECIMAL        	{ hostvar_type |= HT_DECIMAL; }
            |   T_STATVAR		{ hostvar_type |= HT_SQLSTAT; }
	    |	T_STATIC
	    |	T_EXTERN
	    |	T_REGISTER
            ;

declarator_list:    declarator
            |   declarator_list  T_COMMA  declarator
            ;

declarator:     pointer declarator2  initializer
	    	    { $$ = pa_decl( $2, hostvar_type|$1, NULL, 
		                    !SUBSCRIPTED, !PERMANENT, $3 ); }
            |   pointer declarator2  T_LBRACE  intlist  T_RBRACE  initializer
	    	    { $$ = pa_decl( $2, hostvar_type|$1, $4, 
		                    SUBSCRIPTED, !PERMANENT, $6 ); }
            ;

pointer:        T_TIMES
		    { $$ = HT_POINTER; }
	    |	/* empty */
	    	    { $$ = HT_NOTYPE; }
            ;

initializer:    T_INITIALIZER
	    |   /* empty */
	            { $$ = NULL; }
	    ;

intlist:	intlist1
	    |	/* empty */
	    	    { $$ = NULL; }
	    ;
	    
intlist1:	integer
		    { $$ = pa_add_list_end( NULL, $1 ); }
	    |   intlist1  T_COMMA  integer
	    	    { $$ = pa_add_list_end( $1, $3 ); }
	    ;
	     
declarator2:    T_IDENTIFIER
            ;


setoption:	T_SET  optionkwd  id_spec  T_EQ  id_indspec
		    { $$ = pa_setoption( FALSE, NULL, $3, $5 ); }
	    |	T_SET  optionkwd  id_spec  T_DOT  id_spec  T_EQ  id_indspec
		    { $$ = pa_setoption( TRUE, $3, $5, $7 ); }
	    ;

optionkwd:	T_OPTION
	    |   /* empty */
	    ;
	    
get_using:	T_INTO  intohostvar
                    { $$ = pa_add_list( NULL, $2 );
		      $$ = pa_using_info( UT_HOSTLIST, $$ );
		    }
            ;

getoption:	T_GET  T_OPTION  id_spec  get_using
		    { $$ = pa_getoption( FALSE, NULL, $3, $4 ); }
	    |	T_GET  T_OPTION  id_spec  T_DOT  id_spec  get_using
		    { $$ = pa_getoption( TRUE, $3, $5, $6 ); }
	    ;
%%

/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/




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
#ifndef __YYSTYPE_DEFINED
#define __YYSTYPE_DEFINED
typedef union {
        char            *o_string;
        id_type         o_type;
        long            o_value;
        VarInfo         o_var;
        TypeInfo        o_typeinfo;
        ArrayInfo       o_array;
        List            *o_list;
} YYSTYPE;
#endif
#ifndef YYTABTYPE
#define YYTABTYPE       short
#endif

#ifndef YYCHKTYPE
#define YYCHKTYPE       YYTABTYPE
#endif

#ifndef YYACTTYPE
#define YYACTTYPE       YYTABTYPE
#endif

#ifndef YYPLENTYPE
#define YYPLENTYPE      YYTABTYPE
#endif

#ifndef YYPLHSTYPE
#define YYPLHSTYPE      YYTABTYPE
#endif

#ifndef YYFAR
#define YYFAR
#endif

#define YYNOACTION              784
#define YYEOFTOKEN              0
#define YYERRTOKEN              2
#define YYETOKEN                2
#define YYPTOKEN                164
#define YYDTOKEN                163
#define YYSTART                 490
#define YYSTOP                  3
#define YYERR                   0
#define YYUSED                  687
static const YYCHKTYPE YYFAR yychktab[] = {
/*    0 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*   20 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*   40 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*   60 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*   80 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*  100 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*  120 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*  140 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,139,  1,  1,  1,  1,  1,  1,
/*  160 */   1,  1,  1,  1,163,163,164,163,164,163,164,163,164,163,164,163,164,163,164,163,
/*  180 */ 164,153,163,164,  1,157,172,163,164,163,164,163,164,154,163,164,168,163,164,158,
/*  200 */   1,158,163,164,163,164,163,164,158,163,164,154,157,163,164,142,138,174,163,164,
/*  220 */ 163,164,194,195,174,163,164,  1,159,163,164,192,163,164,163,164,163,164,163,164,
/*  240 */ 157,163,164,163,164,141,163,164,141,192,163,164,163,164,163,164,154,163,164,163,
/*  260 */ 164,157,158,159,164,163,164,163,164,156,163,164,  1,141,157,171,163,164,  1,154,
/*  280 */ 163,164,138,  1,140,138,  1,140,163,164,163,164,  1,131,192,163,164,197,191,  1,
/*  300 */ 196,197,163,164,200,201,202,163,164,153,163,164,142,157,  1,157,158,192,142,163,
/*  320 */ 164,163,164,191,168,163,164,163,164,138,158,163,164,163,164,163,164,163,164,163,
/*  340 */ 164,163,164,141,138,  1,157,163,164,193,194,195,163,164,163,164,140,157,136,141,
/*  360 */ 164,157,166,163,164,163,164,163,164,163,164,163,164,169,158,171,163,164,136,163,
/*  380 */ 164,163,164,163,164,163,164,163,164,173,174,163,164,191,163,164,163,164,163,164,
/*  400 */ 163,164,163,164,164,163,164,163,164,191,163,164,163,164,157,  1,157,163,164,136,
/*  420 */ 163,164,163,164,163,164,163,164,141,163,164,144,163,164,163,164,  0,197,163,164,
/*  440 */ 200,201,185,163,164,  1,163,164,163,164,163,164,163,164,153,157,163,164,157,163,
/*  460 */ 164,163,164,  1,163,164,157,163,164,168,163,164,163,164,163,164,139,  1,169,170,
/*  480 */ 171,163,164,153,157,198,164,157,199,200,163,164,  2,163,164,  1,157,  1,168,  1,
/*  500 */ 163,164,163,164,127,128,129,163,164,172,163,164,163,164,163,164,163,164,147,197,
/*  520 */   1,150,151,146,147,148,157,150,151,147,151,  1,163,164,163,164,  1,166,  1,157,
/*  540 */ 163,164,163,164,157,163,164,165,143,144,163,164,127,128,163,164,163,164,157,163,
/*  560 */ 164,  1,137,  1,163,164,163,164,163,164,169,170,171,163,164,163,164,  1,153,163,
/*  580 */ 164,  1,157,163,164,163,164,  1,163,164,163,164,  1,168,163,164,163,164,  1,163,
/*  600 */ 164,163,164,163,164,163,164,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,127,128,129,
/*  620 */ 130,  1,132,133,134,135,  1,  1,  1,  1,  1,  1,  1,  1,  1,145,146,147,148,149,
/*  640 */   1,  1,  1,153,  1,155,  1,157,  1,  1,160,  1,  1,163,164,  1,  1,167,168,  1,
/*  660 */   1,  1,  1,  1,  1,175,176,177,178,179,180,181,182,183,184,  1,186,187,188,189,
/*  680 */ 190,  1,  1,  1,  1,  1,196
};
static const YYACTTYPE YYFAR yyacttab[] = {
/*    0 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   10 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   20 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   30 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   40 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   50 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   60 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   70 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   80 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*   90 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*  100 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*  110 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*  120 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*  130 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*  140 */   490,  490,  490,  490,  490,  490,  490,  490,  490,  490,
/*  150 */   490,  490,  490,  216,  490,  490,  490,  490,  490,  490,
/*  160 */   490,  490,  490,  490,  760,  757,    1,  746,    1,  734,
/*  170 */     1,  732,    1,  731,    1,  747,    1,  759,    1,  754,
/*  180 */     1,  387,  752,    1,  490,  296,    1,  709,    1,  768,
/*  190 */     1,  784,    1,   69,  748,    1,   55,  729,    1,    6,
/*  200 */   490,  113,  753,    1,  784,    1,  784,    1,  113,  728,
/*  210 */     1,   69,   14,  784,    1,   16,   28,    8,  784,    1,
/*  220 */   753,    1,    2,   78,   10,  755,    1,  490,   19,  749,
/*  230 */     1,    4,  784,    1,  750,    1,  784,    1,  699,    1,
/*  240 */    24,  758,    1,  700,    1,  240,  784,    1,  156,   12,
/*  250 */   771,    1,  777,    1,  765,    1,   69,  725,    1,  730,
/*  260 */     1,  287,  283,  266,  240,  753,    1,  784,    1,   41,
/*  270 */   784,    1,  490,  156,   71,  269,  733,    1,  490,   69,
/*  280 */   784,    1,   43,  490,   34,   50,  490,   46,  753,    1,
/*  290 */   707,    1,  490,   83,   31,  784,    1,  178,   39,  490,
/*  300 */   271,  168,  738,    1,   26,  191,  280,  784,    1,  387,
/*  310 */   784,    1,   87,  296,  490,   75,   80,   66,   91,  756,
/*  320 */     1,  701,    1,   57,   55,  784,    1,  774,    1,  104,
/*  330 */    89,  770,    1,  784,    1,  784,    1,  766,    1,  784,
/*  340 */     1,  762,    1,  156,  204,  490,  107,  727,    1,   73,
/*  350 */    62,   78,  784,    1,  769,    1,   96,  132,  117,  156,
/*  360 */   371,  304,  249,  784,    1,  784,    1,  784,    1,  722,
/*  370 */     1,  739,    1,   94,  113,  337,  740,    1,  158,  784,
/*  380 */     1,  784,    1,  741,    1,  751,    1,  711,    1,  144,
/*  390 */   147,  713,    1,  102,  712,    1,  714,    1,  715,    1,
/*  400 */   703,    1,  743,    1,  104,  702,    1,  742,    1,  125,
/*  410 */   744,    1,  705,    1,  162,  490,  139,  704,    1,  172,
/*  420 */   710,    1,  784,    1,  706,    1,  780,    1,  289,  776,
/*  430 */     1,  164,  772,    1,  775,    1,    3,  170,  763,    1,
/*  440 */   784,  784,  127,  773,    1,  490,  778,    1,  764,    1,
/*  450 */   779,    1,  767,  104,  387,  202,  717,    1,  296,  781,
/*  460 */     1,  784,    1,  490,  784,    1,  304,  779,    1,  189,
/*  470 */   724,    1,  784,    1,  723,  206,  216,  490,  307,  206,
/*  480 */   337,  697,    1,  387,  218,  285,  240,  296,  176,  174,
/*  490 */   784,    1,  318,  784,    1,  490,  222,  490,  200,  490,
/*  500 */   726,    1,  784,    1,  208,  213,  220,  737,    1,  184,
/*  510 */   735,    1,  736,    1,  688,    1,  782,    1,  239,  275,
/*  520 */   490,  244,  247,  224,  228,  231,  242,  233,  235,  257,
/*  530 */   261,  490,  784,    1,  745,    1,  490,  237,  490,  259,
/*  540 */   784,    1,  784,    1,  263,  708,    1,  254,  100,  784,
/*  550 */   784,    1,  301,  298,  687,    1,  689,    1,  304,  690,
/*  560 */     1,  490,  293,  490,  784,    1,  691,    1,  781,  287,
/*  570 */   307,  311,  337,  692,    1,  693,    1,  490,  387,  694,
/*  580 */     1,  490,  296,  695,    1,  696,    1,  490,  784,    1,
/*  590 */   716,    1,  490,  309,  718,    1,  719,    1,  490,  784,
/*  600 */     1,  720,    1,  721,    1,  761,    1,  490,  490,  490,
/*  610 */   490,  490,  490,  490,  490,  490,  490,  330,  327,  339,
/*  620 */   377,  490,  344,  347,  349,  351,  490,  490,  490,  490,
/*  630 */   490,  490,  490,  490,  490,  369,  371,  196,  382,  379,
/*  640 */   490,  490,  490,  387,  490,  322,  490,  405,  490,  490,
/*  650 */   353,  490,  490,  698,    1,  490,  490,  425,  401,  490,
/*  660 */   490,  490,  490,  490,  490,  436,  391,  393,  396,  403,
/*  670 */   410,  412,  416,  420,  422,  490,  427,  431,  433,  438,
/*  680 */   440,  490,  490,  490,  490,  490,  442
};
static const YYPLENTYPE YYFAR yyplentab[] = {
/*    0 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  4,  4,  3,  2,  2,  2,  2,  2,
/*   20 */   2,  0,  3,  0,  2,  2,  2,  2,  2,  1,  2,  1,  1,  1,  1,  3,  2,  1,  3,  1,
/*   40 */   2,  3,  3,  2,  3,  3,  1,  3,  1,  1,  1,  3,  2,  2,  2,  1,  1,  1,  0,  6,
/*   60 */   5,  5,  4,  4,  2,  2,  0,  3,  1,  0,  3,  1,  2,  3,  1,  3,  2,  2,  3,  1,
/*   80 */   0,  3,  1,  1,  3,  1,  1,  2,  1,  1,  3,  1,  1,  2,  1,  1,  2
};
static const YYPLHSTYPE YYFAR yyplhstab[] = {
/*    0 */ 175,175,176,176,176,176,176,176,176,176,176,176,177,177,177,177,178,178,178,178,
/*   20 */ 165,165,185,185,179,179,179,179,179,180,180,180,180,186,186,187,187,170,170,169,
/*   40 */ 169,172,172,172,173,173,174,174,167,167,167,181,183,183,183,166,166,166,166,189,
/*   60 */ 189,190,190,188,188,192,192,191,193,193,194,194,195,195,182,182,182,196,198,199,
/*   80 */ 199,200,200,201,197,197,197,197,197,202,202,202,171,168,168,184,203
};


YYSTYPE *yyvp, yyval, yylval;

#define yyerrok         yyerrflag = 0
#define yyclearin       yytoken = yyscan()

#ifndef MAXDEPTH
#define MAXDEPTH        100
#endif

#define YYABORT         return(1)
#define YYACCEPT        return(0)
#define YYERROR         goto yyerrlab

yyparse()
{
  short yypnum;
  short yyi, yyk, yylhs, yyaction;
  short yytoken;
  short yys[MAXDEPTH], *yysp;
  YYSTYPE yyv[MAXDEPTH], *yyvp;
  short yyerrflag;

  yyerrflag = 0;
  yysp = yys;
  yyvp = yyv;
  *yysp = YYSTART;
  yytoken = yylex();
  for(;;){
yynewact:
      yyk = *yysp;
      while( (yyi = yyk + yytoken) < 0 || yyi >= YYUSED || yychktab[yyi] != yytoken )
          if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN )
              goto yycheck1;
          else
              yyk = yyacttab[yyi];
      yyaction = yyacttab[yyi];
      if( yyaction == YYNOACTION ){
yycheck1:
          yyk = *yysp;
          while( (yyi = yyk + YYDTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYDTOKEN )
              if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN )
                  goto yycheck2;
              else
                  yyk = yyacttab[yyi];
          yyaction = yyacttab[yyi];
          if( yyaction == YYNOACTION ){
yycheck2:
              switch( yyerrflag ){
                case 0:
                  yyerror( "syntax error" );
                  /*yyerrlab: ( avoid unreferenced warning ) */
                case 1:
                case 2:
                  yyerrflag = 3;
                  while( yysp >= yys ){
                      yyk = *yysp;
                      while( (yyi = yyk + YYETOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYETOKEN )
                          if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN )
                              goto continu;
                          else
                              yyk = yyacttab[yyi];
                      yyaction = yyacttab[yyi];
                      if( yyaction < YYUSED ){
                          *++yysp = yyaction;
                          ++yyvp;
                          goto yynewact;
                      };
                      continu:;
                      --yysp;
                      --yyvp;
                  };
                  YYABORT;
                case 3:
                  if( yytoken == 0 ) /* EOF token */
                      YYABORT;
                  yytoken = yylex();
                  goto yynewact;
              };
          };
      };
      if( yyaction < YYUSED ){
          if( yyaction == YYSTOP ){
              YYACCEPT;
          } else {
              *++yysp = yyaction;
              *++yyvp = yylval;
              if( yyerrflag )
                  --yyerrflag;
              yytoken = yylex();
          };
      } else {
          yypnum = yyaction - YYUSED;
          yyi = yyplentab[yypnum];
          yysp -= yyi;
          yyvp -= yyi;
          yylhs = yyplhstab[yypnum];
          if( yysp < yys ){
              printf( "stack underflow\n" );
              YYABORT;
          };
          yyk = *yysp;
          while( (yyi = yyk + yylhs) < 0 || yyi >= YYUSED || yychktab[yyi] != yylhs ){
              if( (yyi = yyk + YYPTOKEN) < 0 || yyi >= YYUSED || yychktab[yyi] != YYPTOKEN ){
                  printf( "missing nonterminal\n" );
                  YYABORT;
              };
              yyk = yyacttab[yyi];
          };
          *++yysp = yyacttab[yyi];
          ++yyvp;
          switch( yypnum ){
case 95:
/* end <- FI_EOF */
case 91:
/* constant <- CT_INTEGER */
case 90:
/* constant <- CT_INTEGER ST_PERIOD CT_INTEGER */
case 89:
/* constant <- CT_STRING */
case 88:
/* expr <- call */
case 87:
/* expr <- ID_IDENTIFIER ST_EXCLAM */
case 86:
/* expr <- constant */
case 85:
/* expr <- variable */
case 84:
/* expr <- ST_LBRACKET expr ST_RBRACKET */
case 83:
/* arg <- expr */
case 82:
/* args <- arg */
case 81:
/* args <- arg ST_COMMA args */
case 80:
/* call_args <- */
case 79:
/* call_args <- args */
case 78:
/* call_list <- ST_LBRACKET call_args ST_RBRACKET */
case 77:
/* call <- ID_IDENTIFIER call_list */
case 75:
/* subprogram <- ID_IDENTIFIER ST_EQ expr */
case 74:
/* subprogram <- call */
case 71:
/* parms <- parm */
case 70:
/* parms <- parm ST_COMMA parms */
case 69:
/* parameters <- */
case 68:
/* parameters <- parms */
case 66:
/* sp_modifier <- */
case 65:
/* sp_modifier <- ST_LIBRARY CT_STRING */
case 63:
/* event_header <- ST_ON ID_IDENTIFIER ST_PERIOD ID_IDENTIFIER */
case 54:
/* end_subprogram <- ST_END ST_ON */
case 51:
/* global_declare <- ST_GLOBAL ID_IDENTIFIER ID_IDENTIFIER */
case 34:
/* sp_prototype <- subroutine_header */
case 33:
/* sp_prototype <- function_header */
case 32:
/* body <- event_header */
case 31:
/* body <- obj_variable */
case 23:
/* global_object <- */
case 22:
/* global_object <- ID_IDENTIFIER ST_FROM ID_IDENTIFIER */
{}
break;
case 57:
/* sec_type <- ST_VARIABLES */
case 56:
/* sec_type <- ST_TYPE */
case 55:
/* sec_type <- ST_PROTOTYPES */
case 20:
/* global_type <- ST_TYPE global_object */
{ yyval.o_type = yyvp[0].o_type; }
break;
case 53:
/* end_subprogram <- ST_END ST_SUBROUTINE */
case 52:
/* end_subprogram <- ST_END ST_FUNCTION */
{ EndSubProgram(); }
break;
case 45:
/* array_expr_list <- array_expr_list ST_COMMA array_expr */
case 44:
/* array_expr_list <- array_expr ST_COMMA array_expr */
{ yyval.o_array.flags = ARRAY_MULTI_DIM; }
break;
case 42:
/* array_spec <- ST_LSQ_BRACKET array_expr_list ST_RSQ_BRACKET */
case 41:
/* array_spec <- ST_LSQ_BRACKET array_expr ST_RSQ_BRACKET */
{ yyval.o_array = yyvp[1].o_array; }
break;
case 28:
/* end_section <- ST_END ST_VARIABLES */
case 27:
/* end_section <- ST_END ST_PROTOTYPES */
case 26:
/* end_section <- ST_END ST_TYPE */
case 25:
/* end_section <- ST_END ST_GLOBAL */
case 24:
/* end_section <- ST_END ST_FORWARD */
{ EndSection(); }
break;
case 19:
/* start_section <- ST_SHARED ST_VARIABLES */
case 18:
/* start_section <- ST_TYPE sec_type */
case 17:
/* start_section <- ST_GLOBAL global_type */
case 16:
/* start_section <- ST_FORWARD sec_type */
{ StartSection( yyvp[0].o_type, yyvp[1].o_type ); }
break;
case 15:
/* header <- ST_DOLLAR ID_IDENTIFIER */
case 14:
/* header <- ST_DOLLAR ID_IDENTIFIER ST_PERIOD */
{ SetHeader( yyvp[1].o_string, NULL ); }
break;
case 0:
/* sru_statement <- sru_stmt */
{ ProcessStatement(); }
break;
case 1:
/* sru_statement <- ST_COMMENT */
{ SetComment(); }
break;
case 2:
/* sru_stmt <- header */
{ DEBUGOUT( "header\n" ); }
break;
case 3:
/* sru_stmt <- start_section */
{ DEBUGOUT( "start_section\n" ); }
break;
case 4:
/* sru_stmt <- end_section */
{ DEBUGOUT( "end_section\n" ); }
break;
case 5:
/* sru_stmt <- body */
{ DEBUGOUT( "body\n" ); }
break;
case 6:
/* sru_stmt <- global_declare */
{ DEBUGOUT( "global_declare\n" ); }
break;
case 7:
/* sru_stmt <- subprogram */
{
                DEBUGOUT( "subprogram\n" );
                UserCode();
            }
break;
case 8:
/* sru_stmt <- end_subprogram */
{ DEBUGOUT( "end_subprogram\n" ); }
break;
case 9:
/* sru_stmt <- end */
{
                DEBUGOUT( "end\n" );
                YYABORT; /* finished */
            }
break;
case 10:
/* sru_stmt <- error */
{
                DEBUGOUT( "error\n" );
                UserCode();
            }
break;
case 11:
/* sru_stmt <- */
{ DEBUGOUT( "nothing\n" ); }
break;
case 12:
/* header <- ST_DOLLAR ID_IDENTIFIER ST_PERIOD ID_IDENTIFIER */
{ SetHeader( yyvp[1].o_string, yyvp[3].o_string ); }
break;
case 13:
/* header <- ST_DOLLAR ID_IDENTIFIER ST_PERIOD CT_INTEGER */
{
                char    buff[5];
                itoa( yyvp[3].o_value % 1000, buff, 10 );
                SetHeader( yyvp[1].o_string, buff );
            }
break;
case 29:
/* body <- sp_prototype */
{ StartSubProgram(); }
break;
case 30:
/* body <- access_specifier ST_COLON */
{ SetDefaultAccess( yyvp[0].o_type ); }
break;
case 35:
/* obj_variable <- access_specifier type variable_list */
{ AddDataMethod( yyvp[0].o_type, &(yyvp[1].o_typeinfo), yyvp[2].o_list ); }
break;
case 36:
/* obj_variable <- type variable_list */
{ AddDataMethod2( &(yyvp[0].o_typeinfo), yyvp[1].o_list ); }
break;
case 37:
/* variable_list <- var_dec */
{
                yyval.o_list = NewList( sizeof( VarInfo ) );
                AddToList( yyval.o_list, &(yyvp[0].o_var) );
            }
break;
case 38:
/* variable_list <- variable_list ST_COMMA var_dec */
{
                AddToList( yyvp[0].o_list, &(yyvp[2].o_var) );
                yyval.o_list = yyvp[0].o_list;
            }
break;
case 39:
/* var_dec <- variable */
{
                yyval.o_var.name = MemStrDup( yyvp[0].o_string );
                yyval.o_var.flags = 0;
                yyval.o_var.fake = FALSE;
            }
break;
case 40:
/* var_dec <- variable array_spec */
{
                yyval.o_var.name = MemStrDup( yyvp[0].o_string );
                yyval.o_var.flags = VAR_ARRAY;
                yyval.o_var.array = yyvp[1].o_array;
                yyval.o_var.fake = TRUE;
                if( yyval.o_var.array.flags & ARRAY_SIMPLE ) {
                    yyval.o_var.fake = FALSE;
                } else if( yyval.o_var.array.flags & ARRAY_MULTI_DIM ) {
                    Warning( ERR_MULTI_DIM_ARRAY, yyvp[0].o_string );
                } else if( yyval.o_var.array.flags & ARRAY_RANGE ) {
                    Warning( ERR_INDEX_DEFN, yyvp[0].o_string );
                } else if( yyval.o_var.array.flags & ARRAY_DYNAMIC ) {
                    Warning( ERR_DYNAMIC_ARRAY, yyvp[0].o_string );
                }
            }
break;
case 43:
/* array_spec <- ST_LSQ_BRACKET ST_RSQ_BRACKET */
{ yyval.o_array.flags = ARRAY_DYNAMIC; }
break;
case 46:
/* array_expr <- CT_INTEGER */
{
                yyval.o_array.flags = ARRAY_SIMPLE;
                yyval.o_array.elemcnt = yyvp[0].o_value;
            }
break;
case 47:
/* array_expr <- CT_INTEGER ST_TO CT_INTEGER */
{
                yyval.o_array.flags = ARRAY_RANGE;
            }
break;
case 58:
/* sec_type <- */
{ yyval.o_type = 0; }
break;
case 59:
/* function_header <- access_specifier ST_FUNCTION type ID_IDENTIFIER parm_list sp_modifier */
{ SetFunction( &(yyvp[2].o_typeinfo), yyvp[3].o_string ); }
break;
case 60:
/* function_header <- ST_FUNCTION type ID_IDENTIFIER parm_list sp_modifier */
{ SetFunction( &(yyvp[1].o_typeinfo), yyvp[2].o_string ); }
break;
case 61:
/* subroutine_header <- access_specifier ST_SUBROUTINE ID_IDENTIFIER parm_list sp_modifier */
{ SetSubroutine( yyvp[2].o_string ); }
break;
case 62:
/* subroutine_header <- ST_SUBROUTINE ID_IDENTIFIER parm_list sp_modifier */
{ SetSubroutine( yyvp[1].o_string ); }
break;
case 64:
/* event_header <- ST_ON ID_IDENTIFIER */
{ RegisterEvent( yyvp[1].o_string ); }
break;
case 67:
/* parm_list <- ST_LBRACKET parameters ST_RBRACKET */
{ FiniParmList(); }
break;
case 72:
/* parm <- type ID_IDENTIFIER */
{ AddParm( &(yyvp[0].o_typeinfo), yyvp[1].o_string, NULL ); }
break;
case 73:
/* parm <- type ID_IDENTIFIER array_spec */
{ AddParm( &(yyvp[0].o_typeinfo), yyvp[1].o_string, &(yyvp[2].o_array) ); }
break;
case 76:
/* subprogram <- ST_RETURN expr */
{ SetReturn(); }
break;
case 93:
/* type <- ST_REF ID_IDENTIFIER */
{
                yyval.o_typeinfo.name = yyvp[1].o_string;
                yyval.o_typeinfo.isref = TRUE;
            }
break;
case 94:
/* type <- ID_IDENTIFIER */
{
                yyval.o_typeinfo.name = yyvp[0].o_string;
                yyval.o_typeinfo.isref = FALSE;
            }
break;

            default:
              yyval = yyvp[0];
          };
          *yyvp = yyval;
      };
  };
}


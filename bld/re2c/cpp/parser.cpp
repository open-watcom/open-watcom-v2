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


#define YYBYACC 1
#line 2 "parser.y"

#include <time.h>
#include <iostream>
#include "globals.h"
#include "parser.h"
int yyparse();
int yylex();
void yyerror(char*);

static uint accept;
static RegExp *spec;
static Scanner *in;

#line 19 "parser.y"
typedef union {
    Symbol      *symbol;
    RegExp      *regexp;
    Token       *token;
    char        op;
} YYSTYPE;
#line 27 "y.tab.c"
#define CLOSE 257
#define ID 258
#define CODE 259
#define RANGE 260
#define STRING 261
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    0,    9,    2,    3,    3,    4,    4,    5,
    5,    6,    6,    7,    7,    1,    1,    8,    8,    8,
    8,
};
short yylen[] = {                                         2,
    0,    2,    2,    4,    3,    0,    2,    1,    3,    1,
    3,    1,    2,    1,    2,    1,    2,    1,    1,    1,
    3,
};
short yydefred[] = {                                      1,
    0,    0,   19,   20,    0,    2,    0,    0,    0,   12,
    0,    3,    0,   18,    0,    0,    0,    0,    0,   13,
   16,    0,    0,   21,    0,    0,    5,    0,   17,    4,
};
short yydgoto[] = {                                       1,
   22,    6,   18,    7,    8,    9,   10,   11,   12,
};
short yysindex[] = {                                      0,
  -27,  -49,    0,    0,  -23,    0,  -44,  -84,  -23,    0,
 -243,    0,  -23,    0,  -39,  -23,  -23, -244,  -23,    0,
    0, -239,  -53,    0, -104,  -84,    0,  -23,    0,    0,
};
short yyrindex[] = {                                      0,
    0,  -31,    0,    0,    0,    0, -227,  -17,  -20,    0,
  -40,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -36,    0,    0, -226,  -16,    0,  -19,    0,    0,
};
short yygindex[] = {                                      0,
    0,    0,    0,   21,   18,   17,    1,    0,    0,
};
#define YYTABLESIZE 243
short yytable[] = {                                      14,
   14,   24,   16,   15,   15,   30,   14,   19,   18,   20,
   15,   13,    5,   21,   27,   18,    5,   29,   14,   17,
   10,   11,   15,    8,    9,   15,   10,   11,   20,    8,
    9,    6,    7,   23,   26,   28,   25,    0,   10,   11,
    0,    8,    9,    0,    0,    0,    0,    0,    0,    0,
    0,   14,    0,    0,    0,   15,    0,    0,    0,    0,
   18,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   17,   10,   11,    0,    0,    0,    0,    0,    0,   17,
    0,    0,    0,   14,   17,    0,    0,   15,    0,    0,
    0,    0,   18,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   10,   11,    0,    8,    9,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   14,   14,   14,
   14,   15,   15,   15,   15,   18,   18,   18,   18,   18,
    2,    0,    3,    4,   14,    0,    3,    4,   10,   11,
    0,    8,    9,
};
short yycheck[] = {                                      40,
   41,   41,   47,   40,   41,   59,   47,   92,   40,    9,
   47,   61,   40,  257,  259,   47,   40,  257,   59,  124,
   41,   41,   59,   41,   41,    5,   47,   47,   28,   47,
   47,  259,  259,   13,   17,   19,   16,   -1,   59,   59,
   -1,   59,   59,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   92,   -1,   -1,   -1,   92,   -1,   -1,   -1,   -1,
   92,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  124,   92,   92,   -1,   -1,   -1,   -1,   -1,   -1,  124,
   -1,   -1,   -1,  124,  124,   -1,   -1,  124,   -1,   -1,
   -1,   -1,  124,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  124,  124,   -1,  124,  124,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,  259,  260,
  261,  258,  259,  260,  261,  257,  258,  259,  260,  261,
  258,   -1,  260,  261,  258,   -1,  260,  261,  259,  259,
   -1,  259,  259,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 261
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,0,0,0,"'/'",0,0,0,0,0,0,0,0,0,0,0,"';'",0,"'='",0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'\\\\'",0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'|'",0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"CLOSE","ID","CODE","RANGE","STRING",
};
char *yyrule[] = {
"$accept : spec",
"spec :",
"spec : spec rule",
"spec : spec decl",
"decl : ID '=' expr ';'",
"rule : expr look CODE",
"look :",
"look : '/' expr",
"expr : diff",
"expr : expr '|' diff",
"diff : term",
"diff : diff '\\\\' term",
"term : factor",
"term : term factor",
"factor : primary",
"factor : primary close",
"close : CLOSE",
"close : close CLOSE",
"primary : ID",
"primary : RANGE",
"primary : STRING",
"primary : '(' expr ')'",
};
#endif
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#ifdef YYSTACKSIZE
#ifndef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#endif
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 119 "parser.y"

void yyerror(char* s){
    in->fatal(s);
}

int yylex(){
    return in->scan();
}

void parse(int i, std::ostream &o){
    o << "/* Generated by re2c on ";
    time_t now = time(&now);
    o.write(ctime(&now), 24);
    o << " */\n";

    in = new Scanner(i);
    o << "#line " << in->line() << " \"" << (fileName? fileName : "<stdin>") << "\"\n";

    while(in->echo(o)){
        yyparse();
        if(spec)
            genCode(o, spec);
        o << "#line " << in->line() << "\n";
    }
}
#line 212 "y.tab.c"
#define YYABORT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, reading %d (%s)\n", yystate,
                    yychar, yys);
        }
#endif
    }
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: state %d, shifting to state %d\n",
                    yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
    yyerror("syntax error");
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yyssp]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: state %d, error recovery shifting\
 to state %d\n", *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: error recovery discarding state %d\n",
                            *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, error recovery discards token %d (%s)\n",
                    yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("yydebug: state %d, reducing by rule %d (%s)\n",
                yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 38 "parser.y"
{ accept = 0;
                  spec = NULL; }
break;
case 2:
#line 41 "parser.y"
{ spec = spec? mkAlt(spec, yyvsp[0].regexp) : yyvsp[0].regexp; }
break;
case 4:
#line 46 "parser.y"
{ if(yyvsp[-3].symbol->re)
                      in->fatal("sym already defined");
                  yyvsp[-3].symbol->re = yyvsp[-1].regexp; }
break;
case 5:
#line 52 "parser.y"
{ yyval.regexp = new RuleOp(yyvsp[-2].regexp, yyvsp[-1].regexp, yyvsp[0].token, accept++); }
break;
case 6:
#line 56 "parser.y"
{ yyval.regexp = new NullOp; }
break;
case 7:
#line 58 "parser.y"
{ yyval.regexp = yyvsp[0].regexp; }
break;
case 8:
#line 62 "parser.y"
{ yyval.regexp = yyvsp[0].regexp; }
break;
case 9:
#line 64 "parser.y"
{ yyval.regexp =  mkAlt(yyvsp[-2].regexp, yyvsp[0].regexp); }
break;
case 10:
#line 68 "parser.y"
{ yyval.regexp = yyvsp[0].regexp; }
break;
case 11:
#line 70 "parser.y"
{ yyval.regexp =  mkDiff(yyvsp[-2].regexp, yyvsp[0].regexp);
                  if(!yyval.regexp)
                       in->fatal("can only difference char sets");
                }
break;
case 12:
#line 77 "parser.y"
{ yyval.regexp = yyvsp[0].regexp; }
break;
case 13:
#line 79 "parser.y"
{ yyval.regexp = new CatOp(yyvsp[-1].regexp, yyvsp[0].regexp); }
break;
case 14:
#line 83 "parser.y"
{ yyval.regexp = yyvsp[0].regexp; }
break;
case 15:
#line 85 "parser.y"
{
                    switch(yyvsp[0].op){
                    case '*':
                        yyval.regexp = mkAlt(new CloseOp(yyvsp[-1].regexp), new NullOp());
                        break;
                    case '+':
                        yyval.regexp = new CloseOp(yyvsp[-1].regexp);
                        break;
                    case '?':
                        yyval.regexp = mkAlt(yyvsp[-1].regexp, new NullOp());
                        break;
                    }
                }
break;
case 16:
#line 101 "parser.y"
{ yyval.op = yyvsp[0].op; }
break;
case 17:
#line 103 "parser.y"
{ yyval.op = ((yyvsp[-1].op == yyvsp[0].op) != 0) ? yyvsp[-1].op : '*'; }
break;
case 18:
#line 107 "parser.y"
{ if(!yyvsp[0].symbol->re)
                      in->fatal("can't find symbol");
                  yyval.regexp = yyvsp[0].symbol->re; }
break;
case 19:
#line 111 "parser.y"
{ yyval.regexp = yyvsp[0].regexp; }
break;
case 20:
#line 113 "parser.y"
{ yyval.regexp = yyvsp[0].regexp; }
break;
case 21:
#line 115 "parser.y"
{ yyval.regexp = yyvsp[-1].regexp; }
break;
#line 452 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: after reduction, shifting from state 0 to\
 state %d\n", YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("yydebug: state %d, reading %d (%s)\n",
                        YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("yydebug: after reduction, shifting from state %d \
to state %d\n", *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}

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


/* c:\mks\YACC.EXE -p pre ..\c\preparse.y */

// File generated from preparse.y using yaccpre.bat
#include <stdlib.h>
#include <stdio.h>
#include "wic.h"

static pToken _lastDefineTok;

#define PRESTYPE ParseUnion

#ifndef __SMALL__
#define __SMALL__ 0            // To avoid warnings in MKS yacc
#endif

#pragma disable_message(118)
#define Y_EOF   300
#define Y_EXCLAMATION   301
#define Y_NE    302
#define Y_POUND 303
#define Y_POUND_POUND   304
#define Y_AND   305
#define Y_AND_AND       306
#define Y_AND_EQUAL     307
#define Y_LEFT_PAREN    308
#define Y_RIGHT_PAREN   309
#define Y_TIMES 310
#define Y_TIMES_EQUAL   311
#define Y_PLUS  312
#define Y_PLUS_PLUS     313
#define Y_PLUS_EQUAL    314
#define Y_COMMA 315
#define Y_MINUS 316
#define Y_MINUS_MINUS   317
#define Y_MINUS_EQUAL   318
#define Y_ARROW 319
#define Y_DOT   320
#define Y_DOT_DOT_DOT   321
#define Y_DIVIDE        322
#define Y_DIVIDE_EQUAL  323
#define Y_COLON 324
#define Y_SEG_OP        325
#define Y_SEMICOLON     326
#define Y_LT    327
#define Y_LSHIFT        328
#define Y_LSHIFT_EQUAL  329
#define Y_LE    330
#define Y_EQUAL 331
#define Y_EQ    332
#define Y_GT    333
#define Y_GE    334
#define Y_RSHIFT        335
#define Y_RSHIFT_EQUAL  336
#define Y_QUESTION      337
#define Y_LEFT_BRACKET  338
#define Y_RIGHT_BRACKET 339
#define Y_XOR   340
#define Y_XOR_EQUAL     341
#define Y___BASED       342
#define Y___CDECL       343
#define Y___EXPORT      344
#define Y___FAR 345
#define Y___FAR16       346
#define Y___FORTRAN     347
#define Y___HUGE        348
#define Y___INTERRUPT   349
#define Y___LOADDS      350
#define Y___NEAR        351
#define Y___PASCAL      352
#define Y___PRAGMA      353
#define Y___SAVEREGS    354
#define Y___SEGMENT     355
#define Y___SEGNAME     356
#define Y___SELF        357
#define Y___STDCALL     358
#define Y__PACKED       359
#define Y__SEG16        360
#define Y__SYSCALL      361
#define Y_AUTO  362
#define Y_CHAR  363
#define Y_CONST 364
#define Y_DOUBLE        365
#define Y_ELSE  366
#define Y_ENUM  367
#define Y_EXTERN        368
#define Y_FLOAT 369
#define Y_INT   370
#define Y_LONG  371
#define Y_REGISTER      372
#define Y_SHORT 373
#define Y_SIGNED        374
#define Y_SIZEOF        375
#define Y_STATIC        376
#define Y_STRUCT        377
#define Y_TYPEDEF       378
#define Y_UNION 379
#define Y_UNSIGNED      380
#define Y_VOID  381
#define Y_VOLATILE      382
#define Y_LEFT_BRACE    383
#define Y_OR    384
#define Y_OR_EQUAL      385
#define Y_OR_OR 386
#define Y_RIGHT_BRACE   387
#define Y_TILDE 388
#define Y_ID    389
#define Y_STRING        390
#define Y_INCLUDE_FILE_NAME     391
#define Y_TYPEDEF_NAME  392
#define Y_NUMBER        393
#define Y_PERCENT       394
#define Y_PERCENT_EQUAL 395
#define Y_DEFINED       396
#define Y_PRE_COMMENT   500
#define Y_PRE_NULL      501
#define Y_PRE_NEWLINE   502
#define Y_PRE_DEFINE    503
#define Y_PRE_ELIF      504
#define Y_PRE_ELSE      505
#define Y_PRE_ENDIF     506
#define Y_PRE_ERROR     507
#define Y_PRE_IF        508
#define Y_PRE_IFDEF     509
#define Y_PRE_IFNDEF    510
#define Y_PRE_INCLUDE   511
#define Y_PRE_LINE      512
#define Y_PRE_PRAGMA    513
#define Y_PRE_UNDEF     514
#define Y_PRE_SPECIAL_LEFT_PAREN        515
extern int prechar, preerrflag;
extern PRESTYPE preval, prelval;
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

static pToken _dupTokenKeepPos(pToken tok) {
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
static short predef[] = {
         135,  135,   -1,  191,  195,  196,  197,  198,  199,  200,
         201,  202,  203,  204,  205,   -5,   27,  194,  193,  135,
          18,   17,   16,   15,   14,   13,   12,   11,   10,    9,
           8,    7,    6,    5,    4,  206,   33,   -9,   32,   31,
          30,   29,   23,   22,   21,   20,   35,   34,   37,  190,
         -13,   26,  -17,   19,  -21,   28,   31,   30,   29,   25,
         192,  108,  -25
};
static short preex[] = {
           0,    0,   -1,    1,  309,    3,   -1,    1,  309,   36,
          -1,    1,  309,   36,   -1,    1,  387,   24,   -1,    1,
         309,   36,   -1,    1,  309,   36,   -1,    1
};
static short preact[] = {
        -269, -141, -255, -252, -145, -142, -143, -258, -144,   -2,
          -1, -146, -257, -259, -246,  514,  513,  512,  511,  510,
         509,  508,  507,  506,  505,  504,  503,  502,  501,  256,
        -248, -149,  389,  256, -254, -253,  391,  390, -230, -233,
        -134, -234, -232, -231, -136, -229, -149, -178, -181, -133,
         396,  393,  390,  389,  388,  375,  316,  312,  310,  308,
         305,  301, -247,  502, -130,  389, -129,  389, -249, -230,
        -233, -134, -234, -232, -231, -136, -229, -149, -178, -181,
        -133, -250,  515,  396,  393,  390,  389,  388,  375,  316,
         312,  310,  308,  305,  301,  256, -178,  390, -127, -126,
         386,  337, -125,  306, -124,  384, -123,  340, -122,  305,
        -121, -120,  332,  302, -116, -117, -118, -119,  334,  333,
         330,  327, -115, -114,  335,  328, -112, -113,  316,  312,
        -109, -110, -111,  394,  322,  310, -230, -233, -108, -234,
        -232, -231, -136, -229, -149, -178, -181, -133,  396,  393,
         390,  389,  388,  375,  316,  312,  310,  308,  305,  301,
         -16, -107, -106, -105,  338,  320,  319,  308, -230, -233,
        -134, -234, -232, -231, -216, -210, -184, -205, -104, -214,
        -206, -208, -207, -215, -209, -204, -136, -213, -288, -212,
        -289, -203, -211, -183, -229, -149, -178, -219, -181, -133,
         396,  393,  392,  390,  389,  388,  382,  381,  380,  379,
         378,  377,  376,  375,  374,  373,  372,  371,  370,  369,
         368,  367,  365,  364,  363,  362,  316,  312,  310,  308,
         305,  301,  -20,  308, -149,  389,  -38,  -37,  -96, -296,
        -293, -294, -298, -295, -292, -297, -300, -299,  361,  358,
         352,  351,  348,  347,  346,  345,  343,  338,  310,  308,
         -95, -149,  389,  383,  -94, -286, -287,  392,  389,  383,
        -216, -210, -184, -205, -214, -206, -208, -207, -215, -209,
        -204, -213, -212, -203, -211, -183,  382,  381,  380,  378,
         376,  374,  373,  372,  371,  370,  369,  368,  365,  364,
         363,  362, -216, -210, -184, -205, -104, -214, -206, -208,
        -207, -215, -209, -204, -213, -288, -212, -289, -203, -211,
        -183, -219,  392,  382,  381,  380,  379,  378,  377,  376,
         374,  373,  372,  371,  370,  369,  368,  367,  365,  364,
         363,  362,  -93,  309, -244,  -92,  315,  309,  -92,  -90,
         324,  315, -235,  309,  -89,  315, -241,  309,  -92, -242,
         339,  315,  -48,  -47,  382,  364, -230, -233, -134, -234,
        -232, -231, -163, -136, -229, -149, -178, -181, -133,  396,
         393,  390,  389,  388,  375,  339,  316,  312,  310,  308,
         305,  301,  -38,  -37, -150,  -96, -296, -293, -294, -298,
        -295, -292, -297, -300, -299, -216, -210, -184, -205, -104,
        -214, -206, -208, -207, -215, -209, -204, -213, -288, -212,
        -289, -203, -211, -183, -219,  392,  382,  381,  380,  379,
         378,  377,  376,  374,  373,  372,  371,  370,  369,  368,
         367,  365,  364,  363,  362,  361,  358,  352,  351,  348,
         347,  346,  345,  343,  338,  321,  310,  308,  -51,  -85,
         338,  308,  -83,  383,  -94,  383, -157,  364, -158,  382,
        -162,  339,  -55,  -37,  -96, -296, -293, -294, -298, -295,
        -292, -297, -300, -299, -149, -291,  392,  389,  361,  358,
         352,  351,  348,  347,  346,  345,  343,  338,  310,  308,
         -79,  315, -160,  309, -166,  309, -150, -216, -210, -184,
        -205, -104, -214, -206, -208, -207, -215, -209, -204, -213,
        -288, -212, -289, -203, -211, -183, -219,  392,  382,  381,
         380,  379,  378,  377,  376,  374,  373,  372,  371,  370,
         369,  368,  367,  365,  364,  363,  362,  321, -230, -233,
        -134, -234, -232, -231, -165, -136, -229, -149, -178, -181,
        -133,  396,  393,  390,  389,  388,  375,  339,  316,  312,
         310,  308,  305,  301,  -76,  331, -187,  -60,  315,  256,
        -190,  387,  -74,  -37, -196, -296, -293, -294, -298, -295,
        -292, -297, -300, -299, -149, -291,  392,  389,  361,  358,
         352,  351,  348,  347,  346,  345,  343,  326,  310,  308,
        -197, -216, -210, -184, -205, -104, -214, -206, -208, -207,
        -215, -209, -204, -213, -288, -212, -289, -203, -211, -183,
        -200, -219,  392,  387,  382,  381,  380,  379,  378,  377,
         376,  374,  373,  372,  371,  370,  369,  368,  367,  365,
         364,  363,  362,  256, -245,  309,  -55,  -37, -150,  -96,
        -296, -293, -294, -298, -295, -292, -297, -300, -299, -216,
        -210, -184, -205, -104, -214, -206, -208, -207, -215, -209,
        -204, -213, -288, -212, -289, -203, -211, -183, -149,  -62,
         392,  389,  382,  381,  380,  379,  378,  377,  376,  374,
         373,  372,  371,  370,  369,  368,  367,  365,  364,  363,
         362,  361,  358,  352,  351,  348,  347,  346,  345,  343,
         338,  321,  310,  308,  -63,  -68,  338,  308, -161,  309,
        -164,  339, -191,  387,  -74,  -37, -296, -293, -294, -298,
        -295, -292, -297, -300, -299, -149, -291,  392,  389,  361,
         358,  352,  351,  348,  347,  346,  345,  343,  310,  308,
         -67,  324,  -66, -195,  326,  315, -173,  309, -230, -233,
        -134, -234, -232, -231, -171, -136, -229, -149, -178, -181,
        -133,  396,  393,  390,  389,  388,  375,  339,  316,  312,
         310,  308,  305,  301, -170,  309, -172,  339,   -1
};
static short prepact[] = {
          30,   30,   64,   97,  100,  103,  105,  107,  109,  112,
         118,  124,  128,  133,  164,   50,  248,  286,  322,  348,
         103,  105,  107,  109,  112,  118,  118,  124,  124,  124,
         124,  128,  128,  133,  133,  355,  364,  425,  460,  248,
         248,  248,  463,  465,  286,  286,  467,  469,  486,  501,
         527,  575,  578,  286,  690,  726,  486,  486,  486,  235,
         761,  768,  527,  797,  795,  747,   50,  781,  767,  764,
         747,  747,  747,  747,  733,   50,  731,  729,  527,  655,
         632,  596,  235,  581,  561,  505,  503,  471,   50,   50,
         235,   50,   50,  322,  235,  379,  360,  357,  353,  350,
         346,  343,  267,  262,   50,  235,  235,  200,   50,   50,
          50,   50,   50,   50,   50,   50,   50,   50,   50,   50,
          50,   50,   50,   50,   50,   50,   50,   63,   63,   63,
          63,   63,  233,  200,   50,  148,   63,   82,   67,   65,
          63,   63,   63,   50,   50,   36,   32,   15
};
static short prego[] = {
        -270, -262,   -3, -137, -147, -251, -261, -260, -265, -264,
        -263, -268, -267, -266,  141,  140,  136,  131,  130,  129,
         128,  127, -140, -139,  -91, -256,   19,    1,    0, -272,
         -15,  -98,  -36, -236, -274,  135, -135, -228, -227, -226,
        -225, -237, -275,  134,  110,  109,  108,   92,  -35,  -34,
         -14,  112,  111,  -33,  -32,  -13,  114,  113,  -31,  -30,
         -29,  -28,  -12,  118,  117,  116,  115,  -27,  -26,  -11,
         120,  119,  -25,  -10,  121,  -24,   -9,  122,  -23,   -8,
         123,  -22,   -7,  124,  -21,   -6,  125,   -5, -276, -276,
        -276, -276, -276, -276, -276, -276, -278,  133,  126,  107,
         104,   91,   89,   88,   15, -273, -238, -224, -223, -277,
          91,   89,   88,   15,  -97, -100, -101,  126,  104, -192,
         -64, -185,  -77,  -88, -128, -131, -132,  143,  137,   95,
          84,   75,   67,   66, -174,  -82,  -82,  -17,  -17,  -49,
         133,  107,   93,   80,  -19, -284, -222, -218, -284, -218,
        -279,   53,   45,   44,   18,   17,  -46,  -18,   18, -283,
         -54,  -45,   45, -280, -285, -217, -285, -217, -282,   53,
          45,   44,   17, -221,  -44, -103, -202, -201,   43,  -81,
        -198, -199,   80,  -70, -193, -194,   65, -220,  -75,  -84,
          82,  -53, -188, -186, -189,   59, -281,  -99, -102,  107,
        -271,   -4, -180, -179,    3, -152,  -69, -175, -176, -177,
        -177, -176, -175,  -69,  -61,   73,   72,   71,   70,   58,
          57,   56,   54,   48,  -56, -182, -167, -168, -169, -151,
        -167, -168, -169,  -86,   58,   57,   56,   48,   41,   40,
          39,   16,  -39,  -40,  -40,  -40,  -40,  -40,  -57,  -57,
         -57,  -57,  -57,  -73,   58,   57,   56,   54,   48,   41,
          40,   39,   37,   16,  -41,  -41,  -41,  -41,  -41,  -58,
         -58,  -58,  -58,  -58,  -72,   58,   57,   56,   54,   48,
          41,   40,   39,   37,   16,  -42,  -42,  -42,  -42,  -42,
         -59,  -59,  -59,  -59,  -59,  -71,   58,   57,   56,   54,
          48,   41,   40,   39,   37,   16, -159,  -78,  -65,  -87,
          62,   50,  -50, -153, -154,   78, -156, -155,   78, -290,
        -290, -290, -290, -290,  -52, -290, -290, -290, -290, -290,
        -290,  -52,  -80,  -52,  -43, -240, -239, -138, -243,  146,
         106,  105,  103,   94,   90,   82,   81,   73,   72,   71,
          70,   65,   59,   58,   57,   56,   54,   48,   -1
};
static short prepgo[] = {
           0,    0,    0,   31,   55,   55,   62,   62,   69,   69,
          69,   69,   73,   73,   76,   79,   82,   85,   87,  139,
         139,  139,  173,  187,  189,  189,  194,  198,  214,  233,
         233,  233,  233,  306,  306,  306,  309,  314,  338,  317,
         314,  314,  312,  312,  309,  309,  306,  306,  253,  242,
         242,  242,  242,  242,  242,  242,  233,  233,  233,  224,
         224,  224,  224,  224,  214,  214,  214,  203,  201,  201,
         200,  198,  196,  196,  194,  192,  191,  191,  191,  187,
         187,  185,  183,  183,  181,  181,  179,  179,  179,  177,
         173,  173,  168,  168,  168,  168,  168,  168,  168,  168,
         168,  163,  163,  163,  163,  163,  161,  161,  159,  157,
         157,  144,  116,   96,   50,   50,   50,   42,   36,   36,
          36,   36,   36,   36,   34,   34,   34,   32,   30,   30,
          30,   30,   29,   29,   30,   25,   13,    1,    1,    1,
           1,    4,    3,    3,    0,    0,    0,    0,    0,    0,
           0,    0,    0,    0,    0,    0,    0,    0,    2,    2,
          29,   30,   32,   42,   50,  109,  116,  126,  144,  150,
         150,  157,  157,  161,  161,  174,  174,  175,  175,  134,
         134,  274,  274,  274,  274,  295,  295,  295,  295,  295,
         309,  200,  185,  139,  139,   96,   87,   85,   82,   79,
          76,   73,   69,   62,   55,   34,   31,    0
};
static short prerlen[] = {
           0,    0,    0,    0,    3,    3,    3,    3,    3,    3,
           3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
           2,    2,    2,    2,    1,    2,    1,    1,    1,    1,
           1,    1,    1,    0,    1,    1,    0,    1,    1,    1,
           2,    2,    3,    1,    1,    3,    2,    2,    2,    3,
           4,    3,    2,    4,    3,    3,    2,    2,    2,    4,
           3,    4,    3,    1,    2,    2,    2,    1,    1,    2,
           1,    2,    1,    1,    3,    2,    2,    2,    1,    4,
           5,    3,    3,    1,    3,    2,    2,    2,    1,    3,
           2,    3,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    2,    2,    1,    1,
           1,    2,    3,    5,    3,    3,    3,    4,    1,    1,
           1,    1,    1,    1,    4,    2,    2,    3,    3,    3,
           4,    4,    1,    3,    5,    0,    1,    1,    2,    2,
           3,    1,    1,    1,    1,    2,    1,    1,    1,    4,
           4,    2,    3,    3,    3,    2,    2,    2,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    2
};
#define PRES0   147
#define PREDELTA        110
#define PRENPACT        148
#define PRENDEF 63

#define PREr205 0
#define PREr206 1
#define PREr207 2
#define PREr35  3
#define PREr56  4
#define PREr57  5
#define PREr59  6
#define PREr60  7
#define PREr62  8
#define PREr63  9
#define PREr64  10
#define PREr65  11
#define PREr67  12
#define PREr68  13
#define PREr70  14
#define PREr72  15
#define PREr74  16
#define PREr76  17
#define PREr78  18
#define PREr85  19
#define PREr86  20
#define PREr87  21
#define PREr119 22
#define PREr135 23
#define PREr137 24
#define PREr138 25
#define PREr143 26
#define PREr147 27
#define PREr159 28
#define PREr166 29
#define PREr168 30
#define PREr170 31
#define PREr171 32
#define PREr189 33
#define PREr190 34
#define PREr192 35
#define PREr194 36
#define PREr200 37
#define PREr204 38
#define PREr203 39
#define PREr202 40
#define PREr201 41
#define PREr199 42
#define PREr198 43
#define PREr197 44
#define PREr196 45
#define PREr193 46
#define PREr191 47
#define PREr179 48
#define PREr178 49
#define PREr177 50
#define PREr176 51
#define PREr175 52
#define PREr174 53
#define PREr173 54
#define PREr172 55
#define PREr169 56
#define PREr167 57
#define PREr165 58
#define PREr164 59
#define PREr163 60
#define PREr162 61
#define PREr161 62
#define PREr160 63
#define PREr158 64
#define PREr157 65
#define PREr156 66
#define PREr153 67
#define PREr152 68
#define PREr151 69
#define PREr150 70
#define PREr148 71
#define PREr146 72
#define PREr145 73
#define PREr144 74
#define PREr142 75
#define PREr141 76
#define PREr140 77
#define PREr139 78
#define PREr136 79
#define PREr134 80
#define PREr133 81
#define PREr131 82
#define PREr130 83
#define PREr129 84
#define PREr128 85
#define PREr127 86
#define PREr126 87
#define PREr125 88
#define PREr124 89
#define PREr118 90
#define PREr117 91
#define PREr116 92
#define PREr115 93
#define PREr114 94
#define PREr113 95
#define PREr112 96
#define PREr111 97
#define PREr110 98
#define PREr109 99
#define PREr108 100
#define PREr107 101
#define PREr106 102
#define PREr105 103
#define PREr104 104
#define PREr103 105
#define PREr102 106
#define PREr101 107
#define PREr98  108
#define PREr97  109
#define PREr96  110
#define PREr91  111
#define PREr83  112
#define PREr80  113
#define PREr54  114
#define PREr53  115
#define PREr52  116
#define PREr50  117
#define PREr48  118
#define PREr47  119
#define PREr46  120
#define PREr45  121
#define PREr44  122
#define PREr43  123
#define PREr42  124
#define PREr41  125
#define PREr40  126
#define PREr38  127
#define PREr34  128
#define PREr33  129
#define PREr32  130
#define PREr31  131
#define PREr29  132
#define PREr28  133
#define PREr26  134
#define PREr25  135
#define PREr24  136
#define PREr23  137
#define PREr22  138
#define PREr21  139
#define PREr20  140
#define PREr19  141
#define PREr18  142
#define PREr17  143
#define PREr16  144
#define PREr15  145
#define PREr14  146
#define PREr13  147
#define PREr12  148
#define PREr11  149
#define PREr10  150
#define PREr9   151
#define PREr8   152
#define PREr7   153
#define PREr6   154
#define PREr5   155
#define PREr4   156
#define PREr3   157
#define PREr2   158
#define PREr1   159
#define PRErACCEPT      PREr205
#define PRErERROR       PREr206
#define PRErLR2 PREr207

/*
 * Automaton to interpret LALR(1) tables.
 *
 *      Macros:
 *              preclearin - clear the lookahead token.
 *              preerrok - forgive a pending error
 *              PREERROR - simulate an error
 *              PREACCEPT - halt and return 0
 *              PREABORT - halt and return 1
 *              PRERETURN(value) - halt and return value.  You should use this
 *                      instead of return(value).
 *              PREREAD - ensure prechar contains a lookahead token by reading
 *                      one if it does not.  See also PRESYNC.
 *
 *      Preprocessor flags:
 *              PREDEBUG - includes debug code.  The parser will print
 *                       a travelogue of the parse if this is defined
 *                       and predebug is non-zero.
 *              PRESSIZE - size of state and value stacks (default 150).
 *              PRESTATIC - By default, the state stack is an automatic array.
 *                      If this is defined, the stack will be static.
 *                      In either case, the value stack is static.
 *              PREALLOC - Dynamically allocate both the state and value stacks
 *                      by calling malloc() and free().
 *              PRELR2 - defined if lookahead is needed to resolve R/R or S/R conflicts
 *              PRESYNC - if defined, yacc guarantees to fetch a lookahead token
 *                      before any action, even if it doesnt need it for a decision.
 *                      If PRESYNC is defined, PREREAD will never be necessary unless
 *                      the user explicitly sets prechar = -1
 *
 *      Copyright (c) 1983, by the University of Waterloo
 */

#ifndef PRESSIZE
# define PRESSIZE       150
#endif
#ifndef PREDEBUG
#define PREDEBUG        0
#endif
#define PREERROR                goto preerrlabel
#define preerrok                preerrflag = 0
#define preclearin      prechar = -1
#define PREACCEPT       PRERETURN(0)
#define PREABORT                PRERETURN(1)
#ifdef PREALLOC
# define PRERETURN(val) { retval = (val); goto preReturn; }
#else
# define PRERETURN(val) return(val)
#endif
#if PREDEBUG
/* The if..else makes this macro behave exactly like a statement */
# define PREREAD        if (prechar < 0) {                                      \
                        if ((prechar = prelex()) < 0)                   \
                                prechar = 0;                            \
                        if (predebug)                                   \
                                printf("read %s (%d)\n", preptok(prechar),\
                                prechar);                               \
                } else
#else
# define PREREAD        if (prechar < 0) {                                      \
                        if ((prechar = prelex()) < 0)                   \
                                prechar = 0;                            \
                } else
#endif
#define PREERRCODE      256             /* value of `error' */
#if defined(__TURBOC__)&&__SMALL__
#define PREQPREP        *(int *)((int)preq + ((int)preq-(int)prep))
#else
#define PREQPREP        preq[preq-prep]
#endif

PRESTYPE        preval,                         /* $$ */
        *prepvt,                                /* $n */
        prelval;                                /* prelex() sets this */

int     prechar,                                /* current token */
        preerrflag,                     /* error flag */
        prenerrs;                       /* error count */

#if PREDEBUG
int predebug = PREDEBUG-0;              /* debug flag & tables */
extern char     *presvar[], *prestoken[], *preptok();
extern short    prermap[], presmap[];
extern int      prenstate, prenvar, prentoken, prenrule;
# define preassert(condition, msg, arg) \
        if (!(condition)) { printf("\nyacc bug: "); printf(msg, arg); PREABORT; }
#else /* !PREDEBUG */
# define preassert(condition, msg, arg)
#endif

preparse()
{

        register short          prei, *prep;    /* for table lookup */
        register short          *preps;         /* top of state stack */
        register short          prestate;       /* current state */
        register PRESTYPE       *prepv;         /* top of value stack */
        register short          *preq;
        register int            prej;

#ifdef PRESTATIC
        static short    pres[PRESSIZE + 1];
        static PRESTYPE prev[PRESSIZE + 1];
#else
#ifdef PREALLOC
        PRESTYPE *prev;
        short   *pres;
        PRESTYPE save_prelval, save_preval, *save_prepvt;
        int save_prechar, save_preerrflag, save_prenerrs;
        int retval;
#if 0   /* defined in <stdlib.h>*/
        extern char     *malloc();
#endif
#else
        short           pres[PRESSIZE + 1];
        static PRESTYPE prev[PRESSIZE + 1];     /* historically static */
#endif
#endif

#ifdef PREALLOC
        pres = (short *) malloc((PRESSIZE + 1) * sizeof(short));
        prev = (PRESTYPE *) malloc((PRESSIZE + 1) * sizeof(PRESTYPE));
        if (pres == (short *)0 || prev == (PRESTYPE *)0) {
                preerror("Not enough space for parser stacks");
                return 1;
        }
        save_prelval = prelval;
        save_preval = preval;
        save_prepvt = prepvt;
        save_prechar = prechar;
        save_preerrflag = preerrflag;
        save_prenerrs = prenerrs;
#endif

        prenerrs = 0;
        preerrflag = 0;
        prechar = -1;
        preps = pres;
        prepv = prev;
        prestate = PRES0;               /* start state */

preStack:
        preassert((unsigned)prestate < prenstate, "state %d\n", prestate);
        if (++preps > &pres[PRESSIZE]) {
                preerror("Parser stack overflow");
                PREABORT;
        }
        *preps = prestate;      /* stack current state */
        *++prepv = preval;      /* ... and value */

#if PREDEBUG
        if (predebug)
                printf("state %d (%d), char %s (%d)\n", presmap[prestate],
                        prestate, preptok(prechar), prechar);
#endif

        /*
         *      Look up next action in action table.
         */
preEncore:
#ifdef PRESYNC
        PREREAD;
#endif
        if (prestate >= sizeof prepact/sizeof prepact[0])       /* simple state */
                prei = prestate - PREDELTA;     /* reduce in any case */
        else {
                if(*(prep = &preact[prepact[prestate]]) >= 0) {
                        /* Look for a shift on prechar */
#ifndef PRESYNC
                        PREREAD;
#endif
                        preq = prep;
                        prei = prechar;
#if 0&&defined(__TURBOC__)&&__SMALL__
        /* THIS ONLY WORKS ON TURBO C 1.5 !!! */
                        /* prei is in di, prep is in si */
                L01:
                        asm lodsw       /* ax = *prep++; */
                        asm cmp prei, ax
                        asm jl L01
#else
                        while (prei < *prep++)
                                ;
#endif
                        if (prei == prep[-1]) {
                                prestate = ~PREQPREP;
#if PREDEBUG
                                if (predebug)
                                        printf("shift %d (%d)\n", presmap[prestate], prestate);
#endif
                                preval = prelval;               /* stack what prelex() set */
                                prechar = -1;           /* clear token */
                                if (preerrflag)
                                        preerrflag--;   /* successful shift */
                                goto preStack;
                        }
                }

                /*
                 *      Fell through - take default action
                 */

                if (prestate >= sizeof predef /sizeof predef[0])
                        goto preError;
                if ((prei = predef[prestate]) < 0)       { /* default == reduce? */
                                                                                        /* Search exception table */
                        preassert((unsigned)~prei < sizeof preex/sizeof preex[0],
                                "exception %d\n", prestate);
                        prep = &preex[~prei];
#ifndef PRESYNC
                        PREREAD;
#endif
                        while((prei = *prep) >= 0 && prei != prechar)
                                prep += 2;
                        prei = prep[1];
                        preassert(prei >= 0,"Ex table not reduce %d\n", prei);
                }
        }

#ifdef PRELR2
preReduce:      /* reduce prei */
#endif
        preassert((unsigned)prei < prenrule, "reduce %d\n", prei);
        prej = prerlen[prei];
#if PREDEBUG
        if (predebug) printf("reduce %d (%d), pops %d (%d)\n", prermap[prei],
                prei, presmap[preps[-prej]], preps[-prej]);
#endif
        preps -= prej;          /* pop stacks */
        prepvt = prepv;         /* save top */
        prepv -= prej;
        preval = prepv[1];      /* default action $$ = $1 */
        switch (prei) {         /* perform semantic action */

case PREr1: {   /* directive :  control-line */

        if (prepvt[0].tree != NULL) {
            addSLListElem(g_dirList, prepvt[0].tree);
        }
        PREACCEPT;

} break;

case PREr2: {   /* directive :  error */
 recoverError(); PREACCEPT;
} break;

case PREr3: {   /* control-line :  Y_PRE_NULL line-end */
 preval.tree = NULL;
} break;

case PREr4: {   /* control-line :  Y_PRE_ELSE line-end */
 preval.tree = createCTreeRoot(createConstr1Label(LABCT_PRE_ELSE, prepvt[-1].token));
} break;

case PREr5: {   /* control-line :  Y_PRE_ENDIF line-end */
 preval.tree = createCTreeRoot(createConstr1Label(LABCT_PRE_ENDIF, prepvt[-1].token));
} break;

case PREr6: {   /* control-line :  Y_PRE_IF constant-expression line-end */

            if (g_opt.targetLang == TLT_FORTRAN) {
                reportError(CERR_PREDIR_NOT_SUPPORTED, "#if");
            }
            preval.tree = createCTree1(createConstr1Label(LABCT_PRE_IF, prepvt[-2].token), prepvt[-1].tree);

} break;

case PREr7: {   /* control-line :  Y_PRE_ELIF constant-expression line-end */

            if (g_opt.targetLang == TLT_FORTRAN) {
                reportError(CERR_PREDIR_NOT_SUPPORTED, "#elif");
            }
            preval.tree = createCTree1(createConstr1Label(LABCT_PRE_ELIF, prepvt[-2].token), prepvt[-1].tree);

} break;

case PREr8: {   /* control-line :  Y_PRE_INCLUDE file-name line-end */

            preval.tree = createCTreeRoot(createConstr2Label(LABCT_PRE_INCLUDE, prepvt[-2].token, prepvt[-1].token));
            pushTokFile(getTokenIdName(prepvt[-1].token));

} break;

case PREr9: {   /* control-line :  pre-define-keyword define-directive */
 preval.tree = prepvt[0].tree;
} break;

case PREr10: {  /* control-line :  Y_PRE_IFDEF no-expand-next-token-action Y_ID line-end */
 preval.tree = createCTreeRoot(createConstr2Label(LABCT_PRE_IFDEF, prepvt[-3].token, prepvt[-1].token));
} break;

case PREr11: {  /* control-line :  Y_PRE_IFNDEF no-expand-next-token-action Y_ID line-end */
 preval.tree = createCTreeRoot(createConstr2Label(LABCT_PRE_IFNDEF, prepvt[-3].token, prepvt[-1].token));
} break;

case PREr12: {  /* control-line :  Y_PRE_PRAGMA */

            reportError(CERR_PREDIR_NOT_SUPPORTED, "#pragma");
            preval.tree = eatPreDirList(prepvt[0].token);

} break;

case PREr13: {  /* control-line :  Y_PRE_ERROR */

            if (g_opt.targetLang == TLT_FORTRAN) {
                reportError(CERR_PREDIR_NOT_SUPPORTED, "#error");
            }
            preval.tree = eatPreDirList(prepvt[0].token);

} break;

case PREr14: {  /* control-line :  Y_PRE_LINE */

            if (g_opt.targetLang == TLT_FORTRAN) {
            }
            preval.tree = eatPreDirList(prepvt[0].token);

} break;

case PREr15: {  /* control-line :  Y_PRE_UNDEF no-expand-next-token-action */
 preval.tree = eatPreDirList(prepvt[-1].token);
} break;

case PREr16: {  /* control-line :  Y_PRE_NEWLINE */
 preval.tree = NULL;
} break;

case PREr17: {  /* file-name :  Y_STRING */
 preval.token = prepvt[0].token;
} break;

case PREr18: {  /* file-name :  Y_INCLUDE_FILE_NAME */
 preval.token = prepvt[0].token;
} break;

case PREr19: {  /* pre-define-keyword :  Y_PRE_DEFINE */

            expandNextToken = 0;
            expandThisLineHideErrors = 1;
            _lastDefineTok = prepvt[0].token;

} break;

case PREr20: {  /* define-directive :  identifier constant-expression line-end */
 preval.tree = createEnumFromDefine(_lastDefineTok, prepvt[-2].token, prepvt[-1].tree);
} break;

case PREr21: {  /* define-directive :  identifier Y_PRE_SPECIAL_LEFT_PAREN */
 expandThisLine = 0; preval.tree = createDefineMacroCTree();
} break;

case PREr22: {  /* define-directive :  identifier error */
 preval.tree = createDefineMacroCTree();
} break;

case PREr23: {  /* define-directive :  error */
 recoverError(); preval.tree = NULL;
} break;

case PREr24: {  /* line-end :  Y_PRE_NEWLINE */

            preval.token = prepvt[0].token;

} break;

case PREr25: {  /* no-expand-next-token-action :  */
  expandNextToken = 0;
} break;

case PREr26: {  /* postfix-expression :  Y_DEFINED Y_LEFT_PAREN no-expand-next-token-action identifier Y_RIGHT_PAREN */
 preval.tree = createCTreeRoot(
                createConstr4Label(LABCT_DEFINED, prepvt[-4].token, prepvt[-3].token, prepvt[-1].token, prepvt[0].token));
} break;

case PREr28: {  /* primary-expression :  Y_LEFT_PAREN expression Y_RIGHT_PAREN */
 preval.tree = createCTree1(createConstr2Label(LABCT_PAREN_EXPR, prepvt[-2].token, prepvt[0].token), prepvt[-1].tree);
} break;

case PREr29: {  /* primary-expression :  identifier */
 preval.tree = createCTreeRoot(createTokenLabel(prepvt[0].token));
} break;

case PREr31: {  /* postfix-expression :  postfix-expression Y_LEFT_BRACKET expression Y_RIGHT_BRACKET */
 preval.tree = createCTree2(createConstr2Label(LABCT_INDEX, prepvt[-2].token, prepvt[0].token), prepvt[-3].tree,  prepvt[-1].tree);
} break;

case PREr32: {  /* postfix-expression :  postfix-expression Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN */
 preval.tree = createCTree2(createConstr2Label(LABCT_CALL, prepvt[-2].token, prepvt[0].token), prepvt[-3].tree,  prepvt[-1].tree);
} break;

case PREr33: {  /* postfix-expression :  postfix-expression Y_DOT identifier */
 preval.tree = createCTree2(createConstr1Label(LABCT_DOT, prepvt[-1].token), prepvt[-2].tree,
                            createCTreeRoot(createTokenLabel(prepvt[0].token)));
} break;

case PREr34: {  /* postfix-expression :  postfix-expression Y_ARROW identifier */
 preval.tree = createCTree2(createConstr1Label(LABCT_ARROW, prepvt[-1].token), prepvt[-2].tree,
                            createCTreeRoot(createTokenLabel(prepvt[0].token)));
} break;

case PREr35: {  /* expression-list-opt :  */
 preval.tree = createNULLCTree();
} break;

case PREr38: {  /* expression-list :  expression-list Y_COMMA assignment-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_EXPR_LIST, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr40: {  /* unary-expression :  unary-operator cast-expression */
 preval.tree = createCTree1( prepvt[-1].label, prepvt[0].tree );
} break;

case PREr41: {  /* unary-expression :  Y_SIZEOF unary-expression */
 preval.tree = createCTree1(createConstr1Label(LABCT_SIZEOF_EXPR, prepvt[-1].token), prepvt[0].tree);
} break;

case PREr42: {  /* unary-expression :  Y_SIZEOF Y_LEFT_PAREN type-name Y_RIGHT_PAREN */
 preval.tree = createCTree1(createConstr3Label(LABCT_SIZEOF_TYPE, prepvt[-3].token, prepvt[-2].token, prepvt[0].token), prepvt[-1].tree);
} break;

case PREr43: {  /* unary-operator :  Y_TIMES */
 preval.label = createConstr1Label(LABCT_VALUE_AT_ADDR, prepvt[0].token);
} break;

case PREr44: {  /* unary-operator :  Y_AND */
 preval.label = createConstr1Label(LABCT_ADDR_OF_VALUE, prepvt[0].token);
} break;

case PREr45: {  /* unary-operator :  Y_PLUS */
 preval.label = createConstr1Label(LABCT_UNARY_PLUS, prepvt[0].token);
} break;

case PREr46: {  /* unary-operator :  Y_MINUS */
 preval.label = createConstr1Label(LABCT_UNARY_MINUS, prepvt[0].token);
} break;

case PREr47: {  /* unary-operator :  Y_EXCLAMATION */
 preval.label = createConstr1Label(LABCT_EXCLAMATION, prepvt[0].token);
} break;

case PREr48: {  /* unary-operator :  Y_TILDE */
 preval.label = createConstr1Label(LABCT_TILDE, prepvt[0].token);
} break;

case PREr50: {  /* cast-expression :  Y_LEFT_PAREN type-name Y_RIGHT_PAREN cast-expression */
 preval.tree = createCTree2(createConstr2Label(LABCT_CAST_EXPR, prepvt[-3].token, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr52: {  /* multiplicative-expression :  multiplicative-expression Y_TIMES cast-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_TIMES, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr53: {  /* multiplicative-expression :  multiplicative-expression Y_DIVIDE cast-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_DIVIDE, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr54: {  /* multiplicative-expression :  multiplicative-expression Y_PERCENT cast-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_PERCENT, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr56: {  /* additive-expression :  additive-expression Y_PLUS multiplicative-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_PLUS, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr57: {  /* additive-expression :  additive-expression Y_MINUS multiplicative-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_MINUS, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr59: {  /* shift-expression :  shift-expression Y_RSHIFT additive-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_RSHIFT, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr60: {  /* shift-expression :  shift-expression Y_LSHIFT additive-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_LSHIFT, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr62: {  /* relational-expression :  relational-expression Y_LT shift-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_LT, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr63: {  /* relational-expression :  relational-expression Y_LE shift-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_LE, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr64: {  /* relational-expression :  relational-expression Y_GT shift-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_GT, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr65: {  /* relational-expression :  relational-expression Y_GE shift-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_GE, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr67: {  /* equality-expression :  equality-expression Y_EQ relational-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_EQ, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr68: {  /* equality-expression :  equality-expression Y_NE relational-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_NE, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr70: {  /* and-expression :  and-expression Y_AND equality-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_AND, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr72: {  /* exclusive-or-expression :  exclusive-or-expression Y_XOR and-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_XOR, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr74: {  /* inclusive-or-expression :  inclusive-or-expression Y_OR exclusive-or-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_OR, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr76: {  /* logical-and-expression :  logical-and-expression Y_AND_AND inclusive-or-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_AND_AND, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr78: {  /* logical-or-expression :  logical-or-expression Y_OR_OR logical-and-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_OR_OR, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr80: {  /* conditional-expression :  logical-or-expression Y_QUESTION expression Y_COLON assignment-expression */

            preval.tree = createCTree2(createConstr1Label(LABCT_QUESTION, prepvt[-3].token), prepvt[-4].tree,
                        createCTree2(createConstr1Label(LABCT_COLON, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree));

} break;

case PREr83: {  /* expression :  expression Y_COMMA assignment-expression */
 preval.tree = createCTree2(createConstr1Label(LABCT_EXPR_SEPARATOR, prepvt[-1].token), prepvt[-2].tree, prepvt[0].tree);
} break;

case PREr85: {  /* decl-specs :  non-type-decl-specs type-spec maybe-type-decl-specs */
 preval.dinfo = combine2DeclInfo(combine2DeclInfo(prepvt[-2].dinfo, prepvt[-1].dinfo), prepvt[0].dinfo);
} break;

case PREr86: {  /* decl-specs :  non-type-decl-specs type-spec */
 preval.dinfo = combine2DeclInfo(prepvt[-1].dinfo, prepvt[0].dinfo);
} break;

case PREr87: {  /* decl-specs :  type-spec maybe-type-decl-specs */
 preval.dinfo = combine2DeclInfo(prepvt[-1].dinfo, prepvt[0].dinfo);
} break;

case PREr91: {  /* non-type-decl-specs :  non-type-decl-specs non-type-decl-spec */
 preval.dinfo = combine2DeclInfo(prepvt[-1].dinfo, prepvt[0].dinfo);
} break;

case PREr96: {  /* type-spec :  struct-or-union-spec */
 preval.dinfo = createDeclInfoSTRUCT(prepvt[0].dsinfo);
} break;

case PREr97: {  /* type-spec :  enum-spec */
 preval.dinfo = createDeclInfoENUM(prepvt[0].declEnum);
} break;

case PREr98: {  /* typedef-name :  Y_TYPEDEF_NAME */
 preval.dinfo = dupDeclInfo(prepvt[0].token->data->repr.pTypeDecl, prepvt[0].token->pos); zapToken(prepvt[0].token);
} break;

case PREr101: { /* maybe-type-decl-specs :  maybe-type-decl-specs non-type-decl-spec */
 preval.dinfo = combine2DeclInfo(prepvt[-1].dinfo, prepvt[0].dinfo);
} break;

case PREr102: { /* maybe-type-decl-specs :  maybe-type-decl-specs scalar-type-spec */
 preval.dinfo = combine2DeclInfo(prepvt[-1].dinfo, prepvt[0].dinfo);
} break;

case PREr103: { /* storage-class-spec :  Y_AUTO */
 preval.dinfo = createStgClassDeclInfo(STG_AUTO, prepvt[0].token);
} break;

case PREr104: { /* storage-class-spec :  Y_REGISTER */
 preval.dinfo = createStgClassDeclInfo(STG_REGISTER, prepvt[0].token);
} break;

case PREr105: { /* storage-class-spec :  Y_EXTERN */
 preval.dinfo = createStgClassDeclInfo(STG_EXTERN, prepvt[0].token);
} break;

case PREr106: { /* storage-class-spec :  Y_STATIC */
 preval.dinfo = createStgClassDeclInfo(STG_STATIC, prepvt[0].token);
} break;

case PREr107: { /* storage-class-spec :  Y_TYPEDEF */
 preval.dinfo = createStgClassDeclInfo(STG_TYPEDEF, prepvt[0].token);
} break;

case PREr108: { /* scalar-type-spec :  Y_VOID */
 preval.dinfo = createDeclInfoSCALAR(STM_VOID, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr109: { /* scalar-type-spec :  Y_CHAR */
 preval.dinfo = createDeclInfoSCALAR(STM_CHAR, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr110: { /* scalar-type-spec :  Y_SHORT */
 preval.dinfo = createDeclInfoSCALAR(STM_SHORT, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr111: { /* scalar-type-spec :  Y_INT */
 preval.dinfo = createDeclInfoSCALAR(STM_INT, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr112: { /* scalar-type-spec :  Y_LONG */
 preval.dinfo = createDeclInfoSCALAR(STM_LONG, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr113: { /* scalar-type-spec :  Y_FLOAT */
 preval.dinfo = createDeclInfoSCALAR(STM_FLOAT, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr114: { /* scalar-type-spec :  Y_DOUBLE */
 preval.dinfo = createDeclInfoSCALAR(STM_DOUBLE, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr115: { /* scalar-type-spec :  Y_SIGNED */
 preval.dinfo = createDeclInfoSCALAR(STM_SIGNED, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr116: { /* scalar-type-spec :  Y_UNSIGNED */
 preval.dinfo = createDeclInfoSCALAR(STM_UNSIGNED, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr117: { /* struct-or-union-spec :  struct-or-union struct-id struct-or-union-body */
 preval.dsinfo = createDeclStructInfo(prepvt[-2].token, prepvt[-1].token, prepvt[0].dsbody);
} break;

case PREr118: { /* struct-or-union-spec :  struct-or-union struct-or-union-body */
 preval.dsinfo = createDeclStructInfo(prepvt[-1].token, NULL, prepvt[0].dsbody);
} break;

case PREr119: { /* struct-or-union-spec :  struct-or-union struct-id */
 preval.dsinfo = createDeclStructInfo(prepvt[-1].token, prepvt[0].token, NULL);
} break;

case PREr124: { /* struct-or-union-body :  Y_LEFT_BRACE struct-decl-list Y_RIGHT_BRACE */

            preval.dsbody = createDeclStructBody(prepvt[-1].declList, prepvt[0].token->pos);
            prepvt[0].token->pos = NULL; zapTokens2(prepvt[-2].token, prepvt[0].token);

} break;

case PREr125: { /* struct-decl-list :  struct-decl */
 preval.declList = createDeclList(prepvt[0].dinfo);
} break;

case PREr126: { /* struct-decl-list :  struct-decl-list struct-decl */
 preval.declList = addDeclList(prepvt[-1].declList, prepvt[0].dinfo);
} break;

case PREr127: { /* struct-decl-list :  struct-decl-list error */
 recoverError(); preval.declList = prepvt[-1].declList;
} break;

case PREr128: { /* struct-decl :  decl-specs Y_SEMICOLON */
 preval.dinfo = addDeclInfoDclrList(prepvt[-1].dinfo, NULL); zapToken(prepvt[0].token);
} break;

case PREr129: { /* struct-decl :  decl-specs struct-declarator-list Y_SEMICOLON */
 preval.dinfo = addDeclInfoDclrList(prepvt[-2].dinfo, prepvt[-1].dclrList); zapToken(prepvt[0].token);
} break;

case PREr130: { /* struct-declarator-list :  struct-declarator */
 preval.dclrList = createDclrList(prepvt[0].dclr);
} break;

case PREr131: { /* struct-declarator-list :  struct-declarator-list Y_COMMA struct-declarator */
 preval.dclrList = addDclrList(prepvt[-2].dclrList, prepvt[0].dclr); zapToken(prepvt[-1].token);
} break;

case PREr133: { /* struct-declarator :  declarator Y_COLON constant-expression */

            reportError(CERR_CANT_CONVERT_BIT_FIELDS);
            preval.dclr = prepvt[-2].dclr;
            zapToken(prepvt[-1].token);  zapCTree(prepvt[0].tree);

} break;

case PREr134: { /* enum-spec :  Y_ENUM identifier Y_LEFT_BRACE enum-list Y_RIGHT_BRACE */
 preval.declEnum = createDeclEnum(prepvt[-4].token, prepvt[-1].enumList); zapTokens3(prepvt[-3].token, prepvt[-2].token, prepvt[0].token);
} break;

case PREr135: { /* enum-spec :  Y_ENUM identifier */
 preval.declEnum = createDeclEnum(prepvt[-1].token, NULL); zapToken(prepvt[0].token);
} break;

case PREr136: { /* enum-spec :  Y_ENUM Y_LEFT_BRACE enum-list Y_RIGHT_BRACE */
 preval.declEnum = createDeclEnum(prepvt[-3].token, prepvt[-1].enumList); zapTokens2(prepvt[-2].token, prepvt[0].token);
} break;

case PREr137: { /* enum-list :  enum-list-collect */
 preval.enumList = finishEnumListCreation(prepvt[0].enumList);
} break;

case PREr138: { /* enum-list :  enum-list-collect Y_COMMA */
 preval.enumList = finishEnumListCreation(prepvt[-1].enumList);
} break;

case PREr139: { /* enum-list-collect :  enumerator */
 preval.enumList = createEnumList(prepvt[0].enumElem);
} break;

case PREr140: { /* enum-list-collect :  enum-list-collect comma-and-enumerator */
 preval.enumList = addEnumList(prepvt[-1].enumList,  prepvt[0].enumElem);
} break;

case PREr141: { /* enum-list-collect :  enum-list-collect error */
 recoverError();  preval.enumList = prepvt[-1].enumList;
} break;

case PREr142: { /* comma-and-enumerator :  Y_COMMA enumerator */
 preval.enumElem = addEnumElemBegPunct(prepvt[0].enumElem); zapToken(prepvt[-1].token);
} break;

case PREr143: { /* enumerator :  identifier */
 preval.enumElem = createEnumElem(prepvt[0].token, NULL, NULL);
} break;

case PREr144: { /* enumerator :  identifier Y_EQUAL constant-expression */
 preval.enumElem = createEnumElem(prepvt[-2].token, prepvt[-1].token, prepvt[0].tree);
} break;

case PREr145: { /* cv-qualifier :  Y_CONST */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
          preval.dinfo = createQualifierDeclInfo(STY_CONST,  prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr146: { /* cv-qualifier :  Y_VOLATILE */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
          preval.dinfo = createQualifierDeclInfo(STY_VOLATILE,  prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr147: { /* type-name :  decl-specs */
 preval.tree = createCTreeRoot(createDeclInfoLabel(prepvt[0].dinfo));
} break;

case PREr148: { /* type-name :  decl-specs declarator-no-id */
 preval.tree = createCTreeRoot(createDeclInfoLabel(addDeclDclr(prepvt[-1].dinfo, prepvt[0].dclr)));
} break;

case PREr150: { /* literal :  Y_NUMBER */
 preval.tree = createCTreeRoot(createTokenLabel(prepvt[0].token));
} break;

case PREr151: { /* strings :  strings single-string */

            preval.tree = createCTree2(createConstr0Label(LABCT_STRINGS), prepvt[-1].tree, prepvt[0].tree);

} break;

case PREr152: { /* strings :  single-string */
 preval.tree = prepvt[0].tree;
} break;

case PREr153: { /* single-string :  Y_STRING */
 preval.tree = createCTreeRoot(createTokenLabel(prepvt[0].token));
} break;

case PREr156: { /* declarator :  pragma-modifier declarator */
 preval.dclr = addDclrPragmaModifier(prepvt[0].dclr, prepvt[-1].token);
} break;

case PREr157: { /* declarator :  mem-modifier declarator */
 preval.dclr = addDclrMemModifier(prepvt[0].dclr, prepvt[-1].token);
} break;

case PREr158: { /* declarator :  ptr-modifier declarator */
 preval.dclr = addDclrPtrModifier(prepvt[0].dclr, prepvt[-1].dclrPtr);
} break;

case PREr159: { /* declarator :  actual-declarator */
 preval.dclr = prepvt[0].dclr;
} break;

case PREr160: { /* actual-declarator :  declarator-id */
 preval.dclr = createDclr(prepvt[0].token);
} break;

case PREr161: { /* actual-declarator :  Y_LEFT_PAREN declarator Y_RIGHT_PAREN */
 preval.dclr = prepvt[-1].dclr; zapTokens2(prepvt[-2].token, prepvt[0].token);
} break;

case PREr162: { /* actual-declarator :  actual-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET */
 preval.dclr = addDclrArray(prepvt[-3].dclr, prepvt[-2].token, prepvt[-1].tree, prepvt[0].token);
} break;

case PREr163: { /* actual-declarator :  actual-declarator Y_LEFT_BRACKET Y_RIGHT_BRACKET */
 preval.dclr = addDclrArray(prepvt[-2].dclr, prepvt[-1].token, NULL, prepvt[0].token);
} break;

case PREr164: { /* actual-declarator :  actual-declarator Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN */
 preval.dclr = addDclrFuncArgs(prepvt[-3].dclr, prepvt[-2].token, prepvt[-1].declList, prepvt[0].token);
} break;

case PREr165: { /* declarator-no-id :  pragma-modifier declarator-no-id */
 preval.dclr = addDclrPragmaModifier(prepvt[0].dclr, prepvt[-1].token);
} break;

case PREr166: { /* declarator-no-id :  pragma-modifier */
 preval.dclr = addDclrPragmaModifier(createDclr(NULL), prepvt[0].token);
} break;

case PREr167: { /* declarator-no-id :  mem-modifier declarator-no-id */
 preval.dclr = addDclrMemModifier(prepvt[0].dclr, prepvt[-1].token);
} break;

case PREr168: { /* declarator-no-id :  mem-modifier */
 preval.dclr = addDclrMemModifier(createDclr(NULL), prepvt[0].token);
} break;

case PREr169: { /* declarator-no-id :  ptr-modifier declarator-no-id */
 preval.dclr = addDclrPtrModifier(prepvt[0].dclr, prepvt[-1].dclrPtr);
} break;

case PREr170: { /* declarator-no-id :  ptr-modifier */
 preval.dclr = addDclrPtrModifier(createDclr(NULL), prepvt[0].dclrPtr);
} break;

case PREr171: { /* declarator-no-id :  actual-declarator-no-id */
 preval.dclr = prepvt[0].dclr;
} break;

case PREr172: { /* actual-declarator-no-id :  Y_LEFT_PAREN declarator-no-id Y_RIGHT_PAREN */
 preval.dclr = prepvt[-1].dclr; zapTokens2(prepvt[-2].token, prepvt[0].token);
} break;

case PREr173: { /* actual-declarator-no-id :  actual-declarator-no-id Y_LEFT_BRACKET Y_RIGHT_BRACKET */
 preval.dclr = addDclrArray(prepvt[-2].dclr, prepvt[-1].token, NULL, prepvt[0].token);
} break;

case PREr174: { /* actual-declarator-no-id :  actual-declarator-no-id Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET */
 preval.dclr = addDclrArray(prepvt[-3].dclr, prepvt[-2].token, prepvt[-1].tree, prepvt[0].token);
} break;

case PREr175: { /* actual-declarator-no-id :  Y_LEFT_BRACKET Y_RIGHT_BRACKET */
 preval.dclr = addDclrArray(createDclr(NULL), prepvt[-1].token, NULL, prepvt[0].token);
} break;

case PREr176: { /* actual-declarator-no-id :  Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET */
 preval.dclr = addDclrArray(createDclr(NULL), prepvt[-2].token, prepvt[-1].tree, prepvt[0].token);
} break;

case PREr177: { /* actual-declarator-no-id :  actual-declarator-no-id Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN */
 preval.dclr = addDclrFuncArgs(prepvt[-3].dclr, prepvt[-2].token, prepvt[-1].declList, prepvt[0].token);
} break;

case PREr178: { /* actual-declarator-no-id :  Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN */
 preval.dclr = addDclrFuncArgs(createDclr(NULL), prepvt[-2].token, prepvt[-1].declList, prepvt[0].token);
} break;

case PREr179: { /* ptr-modifier :  Y_TIMES cv-qualifiers-opt */
 preval.dclrPtr = createDclrPtr(prepvt[-1].token, prepvt[0].flag);
} break;

case PREr189: { /* cv-qualifiers-opt :  */
 preval.flag = STY_NULL;
} break;

case PREr190: { /* cv-qualifiers-opt :  Y_CONST */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        preval.flag = STY_CONST; zapToken(prepvt[0].token);
} break;

case PREr191: { /* cv-qualifiers-opt :  Y_CONST Y_VOLATILE */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        preval.flag = STY_CONST | STY_VOLATILE; zapTokens2(prepvt[-1].token, prepvt[0].token);
} break;

case PREr192: { /* cv-qualifiers-opt :  Y_VOLATILE */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        preval.flag = STY_VOLATILE; zapToken(prepvt[0].token);
} break;

case PREr193: { /* cv-qualifiers-opt :  Y_VOLATILE Y_CONST */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        preval.flag = STY_CONST | STY_VOLATILE; zapTokens2(prepvt[-1].token, prepvt[0].token);
} break;

case PREr194: { /* abstract-args :  */
 preval.declList = createSLList();
} break;

case PREr196: { /* abstract-args :  arg-decl-list Y_COMMA dot-dot-dot-decl */
 addDeclPunct(prepvt[0].dinfo, prepvt[-1].token); preval.declList = addDeclList(prepvt[-2].declList, prepvt[0].dinfo );
} break;

case PREr197: { /* abstract-args :  dot-dot-dot-decl */
 preval.declList = createDeclList(prepvt[0].dinfo);
} break;

case PREr198: { /* arg-decl-list :  arg-decl-elem */
 preval.declList = createDeclList(prepvt[0].dinfo);
} break;

case PREr199: { /* arg-decl-list :  arg-decl-list Y_COMMA arg-decl-elem */
 addDeclPunct(prepvt[0].dinfo, prepvt[-1].token); preval.declList = addDeclList(prepvt[-2].declList, prepvt[0].dinfo );
} break;

case PREr200: { /* arg-decl-elem :  decl-specs */
 preval.dinfo = prepvt[0].dinfo;
} break;

case PREr201: { /* arg-decl-elem :  decl-specs declarator */
 preval.dinfo = addDeclDclr(prepvt[-1].dinfo, prepvt[0].dclr);
} break;

case PREr202: { /* arg-decl-elem :  decl-specs declarator-no-id */
 preval.dinfo = addDeclDclr(prepvt[-1].dinfo, prepvt[0].dclr);
} break;

case PREr203: { /* dot-dot-dot-decl :  Y_DOT_DOT_DOT */
 preval.dinfo = createDeclInfoSCALAR(STM_DOT_DOT_DOT, prepvt[0].token->pos);
          prepvt[0].token->pos = NULL; zapToken(prepvt[0].token);
} break;

case PREr204: { /* identifier :  Y_ID */
 preval.token = prepvt[0].token;
} break;
        case PRErACCEPT:
                PREACCEPT;
        case PRErERROR:
                goto preError;
#ifdef PRELR2
        case PRErLR2:
#ifndef PRESYNC
                PREREAD;
#endif
                prej = 0;
                while(prelr2[prej] >= 0) {
                        if(prelr2[prej] == prestate && prelr2[prej+1] == prechar
                        && prelook(pres+1,preps,prestate,prechar,pre2lex(),prelr2[prej+2]))
                                        break;
                        prej += 3;
                }
                if(prelr2[prej] < 0)
                        goto preError;
                if(prelr2[prej+2] < 0) {
                        prestate = ~ prelr2[prej+2];
                        goto preStack;
                }
                prei = prelr2[prej+2];
                goto preReduce;
#endif
        }

        /*
         *      Look up next state in goto table.
         */

        prep = &prego[prepgo[prei]];
        preq = prep++;
        prei = *preps;
#if     0&&defined(__TURBOC__) && __SMALL__
        /* THIS ONLY WORKS ON TURBO C 1.5 !!! */
        /* prei is in di, prep is in si */
L02:
        asm lodsw               /* ax = *prep++; */
        asm cmp prei, ax
        asm jl L02
#else
        while (prei < *prep++)
                ;
#endif
        prestate = ~(prei == *--prep? PREQPREP: *preq);
        goto preStack;

preerrlabel:    ;               /* come here from PREERROR      */
/*
#pragma used preerrlabel
 */
        preerrflag = 1;
        preps--, prepv--;

preError:
        switch (preerrflag) {

        case 0:         /* new error */
                prenerrs++;
                prei = prechar;
                preerror("Syntax error");
                if (prei != prechar) {
                        /* user has changed the current token */
                        /* try again */
                        preerrflag++;   /* avoid loops */
                        goto preEncore;
                }

        case 1:         /* partially recovered */
        case 2:
                preerrflag = 3; /* need 3 valid shifts to recover */

                /*
                 *      Pop states, looking for a
                 *      shift on `error'.
                 */

                for ( ; preps > pres; preps--, prepv--) {
                        if (*preps >= sizeof prepact/sizeof prepact[0])
                                continue;
                        prep = &preact[prepact[*preps]];
                        preq = prep;
                        do
                                ;
                        while (PREERRCODE < *prep++);
                        if (PREERRCODE == prep[-1]) {
                                prestate = ~PREQPREP;
                                goto preStack;
                        }

                        /* no shift in this state */
#if PREDEBUG
                        if (predebug && preps > pres+1)
                                printf("Error recovery pops state %d (%d), uncovers %d (%d)\n",
                                        presmap[preps[0]], preps[0],
                                        presmap[preps[-1]], preps[-1]);
#endif
                        /* pop stacks; try again */
                }
                /* no shift on error - abort */
                break;

        case 3:
                /*
                 *      Erroneous token after
                 *      an error - discard it.
                 */

                if (prechar == 0)  /* but not EOF */
                        break;
#if PREDEBUG
                if (predebug)
                        printf("Error recovery discards %s (%d), ",
                                preptok(prechar), prechar);
#endif
                preclearin;
                goto preEncore; /* try again in same state */
        }
        PREABORT;

#ifdef PREALLOC
preReturn:
        prelval = save_prelval;
        preval = save_preval;
        prepvt = save_prepvt;
        prechar = save_prechar;
        preerrflag = save_preerrflag;
        prenerrs = save_prenerrs;
        free((char *)pres);
        free((char *)prev);
        return(retval);
#endif
}

#ifdef PRELR2
prelook(s,rsp,state,c1,c2,i)
short *s;               /* stack                */
short *rsp;             /* real top of stack    */
int state;              /* current state        */
int c1;                 /* current char         */
int c2;                 /* next char            */
int i;                  /* action S < 0, R >= 0 */
{
        int j;
        short *p,*q;
        short *sb,*st;
#if PREDEBUG
        if(predebug) {
                printf("LR2 state %d (%d) char %s (%d) lookahead %s (%d)",
                        presmap[state],state,preptok(c1),c1,preptok(c2),c2);
                if(i > 0)
                        printf("reduce %d (%d)\n", prermap[i], i);
                else
                        printf("shift %d (%d)\n", presmap[i], i);
        }
#endif
        st = sb = rsp+1;
        if(i >= 0)
                goto reduce;
  shift:
        state = ~i;
        c1 = c2;
        if(c1 < 0)
                return 1;
        c2 = -1;

  stack:
        if(++st >= &s[PRESSIZE]) {
                preerror("Parser Stack Overflow");
                return 0;
        }
        *st = state;
        if(state >= sizeof prepact/sizeof prepact[0])
                i = state- PREDELTA;
        else {
                p = &preact[prepact[state]];
                q = p;
                i = c1;
                while(i < *p++)
                        ;
                if(i == p[-1]) {
                        state = ~q[q-p];
                        c1 = c2;
                        if(c1 < 0)
                                return 1;
                        c2 = -1;
                        goto stack;
                }
                if(state >= sizeof predef/sizeof predef[0])
                        return 0
                if((i = predef[state]) < 0) {
                        p = &preex[~i];
                        while((i = *p) >= 0 && i != c1)
                                p += 2;
                        i = p[1];
                }
        }
  reduce:
        j = prerlen[i];
        if(st-sb >= j)
                st -= j;
        else {
                rsp -= j+st-sb;
                st = sb;
        }
        switch(i) {
        case PRErERROR:
                return 0;
        case PRErACCEPT:
                return 1;
        case PRErLR2:
                j = 0;
                while(prelr2[j] >= 0) {
                        if(prelr2[j] == state && prelr2[j+1] == c1)
                                if((i = prelr2[j+2]) < 0)
                                        goto shift;
                                else
                                        goto reduce;
                }
                return 0;
        }
        p = &prego[prepgo[i]];
        q = p++;
        i = st==sb ? *rsp : *st;
        while(i < *p++);
        state = ~(i == *--p? q[q-p]: *q);
        goto stack;
}
#endif

#if PREDEBUG

/*
 *      Print a token legibly.
 *      This won't work if you roll your own token numbers,
 *      but I've found it useful.
 */
char *
preptok(i)
{
        static char     buf[10];

        if (i >= PREERRCODE)
                return prestoken[i-PREERRCODE];
        if (i < 0)
                return "";
        if (i == 0)
                return "$end";
        if (i < ' ')
                sprintf(buf, "'^%c'", i+'@');
        else
                sprintf(buf, "'%c'", i);
        return buf;
}
#endif
#if PREDEBUG
char * prestoken[] = {
        "error",
        "Y_EOF",
        "Y_EXCLAMATION",
        "Y_NE",
        "Y_POUND",
        "Y_POUND_POUND",
        "Y_AND",
        "Y_AND_AND",
        "Y_AND_EQUAL",
        "Y_LEFT_PAREN",
        "Y_RIGHT_PAREN",
        "Y_TIMES",
        "Y_TIMES_EQUAL",
        "Y_PLUS",
        "Y_PLUS_PLUS",
        "Y_PLUS_EQUAL",
        "Y_COMMA",
        "Y_MINUS",
        "Y_MINUS_MINUS",
        "Y_MINUS_EQUAL",
        "Y_ARROW",
        "Y_DOT",
        "Y_DOT_DOT_DOT",
        "Y_DIVIDE",
        "Y_DIVIDE_EQUAL",
        "Y_COLON",
        "Y_SEG_OP",
        "Y_SEMICOLON",
        "Y_LT",
        "Y_LSHIFT",
        "Y_LSHIFT_EQUAL",
        "Y_LE",
        "Y_EQUAL",
        "Y_EQ",
        "Y_GT",
        "Y_GE",
        "Y_RSHIFT",
        "Y_RSHIFT_EQUAL",
        "Y_QUESTION",
        "Y_LEFT_BRACKET",
        "Y_RIGHT_BRACKET",
        "Y_XOR",
        "Y_XOR_EQUAL",
        "Y___BASED",
        "Y___CDECL",
        "Y___EXPORT",
        "Y___FAR",
        "Y___FAR16",
        "Y___FORTRAN",
        "Y___HUGE",
        "Y___INTERRUPT",
        "Y___LOADDS",
        "Y___NEAR",
        "Y___PASCAL",
        "Y___PRAGMA",
        "Y___SAVEREGS",
        "Y___SEGMENT",
        "Y___SEGNAME",
        "Y___SELF",
        "Y___STDCALL",
        "Y__PACKED",
        "Y__SEG16",
        "Y__SYSCALL",
        "Y_AUTO",
        "Y_CHAR",
        "Y_CONST",
        "Y_DOUBLE",
        "Y_ELSE",
        "Y_ENUM",
        "Y_EXTERN",
        "Y_FLOAT",
        "Y_INT",
        "Y_LONG",
        "Y_REGISTER",
        "Y_SHORT",
        "Y_SIGNED",
        "Y_SIZEOF",
        "Y_STATIC",
        "Y_STRUCT",
        "Y_TYPEDEF",
        "Y_UNION",
        "Y_UNSIGNED",
        "Y_VOID",
        "Y_VOLATILE",
        "Y_LEFT_BRACE",
        "Y_OR",
        "Y_OR_EQUAL",
        "Y_OR_OR",
        "Y_RIGHT_BRACE",
        "Y_TILDE",
        "Y_ID",
        "Y_STRING",
        "Y_INCLUDE_FILE_NAME",
        "Y_TYPEDEF_NAME",
        "Y_NUMBER",
        "Y_PERCENT",
        "Y_PERCENT_EQUAL",
        "Y_DEFINED",
        "Y_PRE_COMMENT",
        "Y_PRE_NULL",
        "Y_PRE_NEWLINE",
        "Y_PRE_DEFINE",
        "Y_PRE_ELIF",
        "Y_PRE_ELSE",
        "Y_PRE_ENDIF",
        "Y_PRE_ERROR",
        "Y_PRE_IF",
        "Y_PRE_IFDEF",
        "Y_PRE_IFNDEF",
        "Y_PRE_INCLUDE",
        "Y_PRE_LINE",
        "Y_PRE_PRAGMA",
        "Y_PRE_UNDEF",
        "Y_PRE_SPECIAL_LEFT_PAREN",
        0
};
char * presvar[] = {
        "$accept",
        "control-line",
        "define-directive",
        "directive",
        "file-name",
        "pre-define-keyword",
        "line-end",
        "no-expand-next-token-action",
        "primary-expression",
        "postfix-expression",
        "expression-list-opt",
        "expression-list",
        "unary-expression",
        "unary-operator",
        "cast-expression",
        "multiplicative-expression",
        "additive-expression",
        "shift-expression",
        "relational-expression",
        "equality-expression",
        "and-expression",
        "exclusive-or-expression",
        "inclusive-or-expression",
        "logical-and-expression",
        "logical-or-expression",
        "conditional-expression",
        "assignment-expression",
        "expression",
        "constant-expression",
        "declarator-id",
        "decl-specs",
        "non-type-decl-specs",
        "non-type-decl-spec",
        "type-spec",
        "typedef-name",
        "maybe-type-decl-specs",
        "storage-class-spec",
        "scalar-type-spec",
        "struct-or-union-spec",
        "struct-id",
        "struct-or-union",
        "struct-or-union-body",
        "struct-decl-list",
        "struct-decl",
        "struct-declarator-list",
        "struct-declarator",
        "enum-spec",
        "enum-list",
        "enum-list-collect",
        "comma-and-enumerator",
        "enumerator",
        "cv-qualifier",
        "type-name",
        "literal",
        "strings",
        "single-string",
        "declarator",
        "actual-declarator",
        "declarator-no-id",
        "actual-declarator-no-id",
        "ptr-modifier",
        "mem-modifier",
        "pragma-modifier",
        "cv-qualifiers-opt",
        "abstract-args",
        "arg-decl-list",
        "arg-decl-elem",
        "dot-dot-dot-decl",
        "identifier",
        0
};
short prermap[] = {
         205,  206,  207,   35,   56,   57,   59,   60,   62,   63,
          64,   65,   67,   68,   70,   72,   74,   76,   78,   85,
          86,   87,  119,  135,  137,  138,  143,  147,  159,  166,
         168,  170,  171,  189,  190,  192,  194,  200,  204,  203,
         202,  201,  199,  198,  197,  196,  193,  191,  179,  178,
         177,  176,  175,  174,  173,  172,  169,  167,  165,  164,
         163,  162,  161,  160,  158,  157,  156,  153,  152,  151,
         150,  148,  146,  145,  144,  142,  141,  140,  139,  136,
         134,  133,  131,  130,  129,  128,  127,  126,  125,  124,
         118,  117,  116,  115,  114,  113,  112,  111,  110,  109,
         108,  107,  106,  105,  104,  103,  102,  101,   98,   97,
          96,   91,   83,   80,   54,   53,   52,   50,   48,   47,
          46,   45,   44,   43,   42,   41,   40,   38,   34,   33,
          32,   31,   29,   28,   26,   25,   24,   23,   22,   21,
          20,   19,   18,   17,   16,   15,   14,   13,   12,   11,
          10,    9,    8,    7,    6,    5,    4,    3,    2,    1,
          27,   30,   37,   49,   51,   81,   82,   84,   90,   92,
          93,   94,   95,   99,  100,  120,  121,  122,  123,  154,
         155,  180,  181,  182,  183,  184,  185,  186,  187,  188,
         195,  149,  132,   89,   88,   79,   77,   75,   73,   71,
          69,   66,   61,   58,   55,   39,   36,    0
};
short presmap[] = {
           7,    8,   18,   32,   34,   35,   36,   37,   38,   39,
          40,   41,   42,   43,   54,   97,   99,  128,  129,  134,
         141,  142,  143,  144,  145,  146,  147,  148,  149,  150,
         151,  152,  153,  154,  155,  163,  175,  177,  178,  179,
         180,  181,  184,  189,  192,  194,  204,  205,  210,  213,
         216,  221,  223,  232,  243,  245,  246,  247,  248,  260,
         267,  276,  277,  295,  293,  290,  289,  278,  275,  269,
         266,  265,  264,  263,  262,  257,  255,  254,  251,  235,
         228,  226,  225,  224,  217,  215,  214,  207,  201,  199,
         198,  196,  195,  185,  183,  176,  165,  164,  159,  140,
         133,  132,  105,  102,   98,   96,   95,   93,   91,   90,
          89,   88,   87,   86,   85,   84,   83,   82,   81,   80,
          79,   78,   77,   76,   75,   74,   73,   70,   67,   66,
          61,   60,   59,   57,   53,   52,   28,   24,   21,   20,
          15,   14,   13,   12,   11,   10,    9,    0,   22,  209,
         241,  242,  282,  211,  212,  283,  238,  239,  206,  252,
         284,  240,  208,  285,  256,  253,  218,  219,  220,  298,
         294,  299,  292,  244,  279,  280,  281,   29,   30,   72,
          31,  182,  100,  101,  286,  287,  258,  259,  222,  261,
         288,  296,  297,  268,  291,  270,  271,  272,  227,  273,
         188,  229,  106,  107,  108,  109,  110,  111,  112,  113,
         114,  115,  116,  117,  118,  119,  230,  231,  120,  121,
         122,  193,  234,  236,  156,  157,  158,  233,   46,   47,
          48,   49,   50,   51,  200,   92,   94,  237,  160,  161,
         202,  203,   56,  197,  274,    3,   62,   23,   68,   69,
         139,    1,   26,   27,    2,   19,    4,    5,    6,  137,
         138,   25,   71,  135,  136,   63,   64,   65,   16,   17,
          58,   55,  162,   45,   44,  131,  130,   33,  127,  126,
         125,  124,  123,  191,  190,  187,  186,  104,  103,  250,
         249,  174,  173,  172,  171,  170,  169,  168,  167,  166
};
int prentoken = 115, prenvar = 69, prenstate = 300, prenrule = 208;
#endif

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


/* c:\mks\YACC.EXE -p c ..\c\cparse.y */


#include <stdlib.h>
#include <stdio.h>
#include "wic.h"

#define CSTYPE ParseUnion

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
extern int cchar, cerrflag;
extern CSTYPE cval, clval;
static pToken savedToken;

int cerror(char *str) {
    str = str;
    return 0;
}

void cparseInterface(void) {
    savedToken = NULL;
    cparse();
}

static int clex(void) {
    int retval;
    int dummy;

    if (savedToken != NULL) {
        clval.token = savedToken;
        savedToken = NULL;
    } else {
        clval.token = getExpandToken(EXP_OP_EXPAND, &dummy);
    }
    while (getTokDataType(clval.token->data) == TT_PREPROCESSOR) {
        if (clval.token->data->code == Y_PRE_NEWLINE) {
            //This is an optimization
            zapToken(clval.token);
        } else {
            preparseInterface(clval.token);
        }
        clval.token = getExpandToken(1, &dummy);
    }
    retval = clval.token->data->code;
    return retval;
}

#define cclearin        cchar = -1
                        // We have to copy yacc def'n of cclearin since
                        // yacc puts recoverError() function before it declares
                        // 'cclearin'. If yacc's def'n ever changes,
                        // we will get a redefinition error.
#define CLEARIN cclearin  // Needed for recoverError

static void recoverError(void) {
    pToken tok;
    int code;
    pSLList context = createSLList();
    int recordMaxTok = 15;
    char *s1, *s2;

    for (tok = clval.token, code = tok->data->code;
            code != Y_SEMICOLON && code != Y_EOF && code != Y_RIGHT_BRACE &&
            getTokDataType(tok->data) != TT_PREPROCESSOR;
         clex(), tok = clval.token, code = tok->data->code)
    {
        if (recordMaxTok-- > 0) {
            addSLListElem(context, tok);
        }
    }

    s1 = getTokListString(context);
    s2 = staticGetTokenStr(tok, 0);
    reportError(RERR_CPARSE_WITH_CONTEXT, s1, s2);
    zapSLList(context, zapToken);
    CLEARIN;
    savedToken = tok;
}
static short cdef[] = {
         185,  184,  105,   -1,   -5,   25,   24,   23,   22,   35,
          30,  198,   28,   -9,   21,   37,   36,  -13,   27,  183,
          39,  181,  182,  186,  187,  188,  189,  190,  191,  192,
         193,  194,  195,  196,   34,  -17,   33,   32,   31,  -21,
          29,    4,  -25,  110,   20,   19,   18,   17,   16,   15,
          14,   13,   12,   11,   10,    9,    8,    7,    6,  197,
         -29,   33,   32,   31
};
static short cex[] = {
           0,    3,   -1,  139,    0,    0,   -1,    1,  387,   26,
          -1,    1,  309,   38,   -1,    1,  309,   38,   -1,    1,
         309,    5,   -1,    1,  309,   38,   -1,    1,  309,   38,
          -1,    1
};
static short cact[] = {
          -4, -248, -206, -200, -174, -195, -137,   -3, -196, -198,
        -197, -205, -199, -194, -203, -267, -202, -268, -193, -201,
        -173, -209,  392,  382,  381,  380,  379,  378,  377,  376,
         374,  373,  372,  371,  370,  369,  368,  367,  365,  364,
         363,  362,  300,  256, -133, -139,  389,  383, -132, -265,
        -266,  392,  389,  383, -206, -200, -174, -195, -204, -196,
        -198, -197, -205, -199, -194, -203, -202, -193, -201, -173,
         382,  381,  380,  378,  376,  374,  373,  372,  371,  370,
         369,  368,  365,  364,  363,  362, -206, -200, -174, -195,
        -137, -204, -196, -198, -197, -205, -199, -194, -203, -267,
        -202, -268, -193, -201, -173, -209,  392,  382,  381,  380,
         379,  378,  377,  376,  374,  373,  372,  371,  370,  369,
         368,  367,  365,  364,  363,  362, -131,  -10, -240, -275,
        -272, -273, -277, -274, -271, -276, -279, -278, -139, -270,
         392,  389,  361,  358,  352,  351,  348,  347,  346,  345,
         343,  326,  310,  308, -126,  390, -238, -247, -206, -200,
        -174, -195, -137,   -3, -196, -198, -197, -205, -199, -194,
        -203, -267, -202, -268, -193, -201, -173, -209,  392,  382,
         381,  380,  379,  378,  377,  376,  374,  373,  372,  371,
         370,  369,  368,  367,  365,  364,  363,  362,  300,  256,
        -139,  389, -124,  383, -132,  383,  -17,  -16,  382,  364,
        -131,  -10, -275, -272, -273, -277, -274, -271, -276, -279,
        -278, -139, -270,  392,  389,  361,  358,  352,  351,  348,
         347,  346,  345,  343,  310,  308,  -18, -120,  338,  308,
        -119,  331, -118, -239,  326,  315, -238, -206, -200, -174,
        -195, -137, -204, -196, -198, -197, -205, -199, -194, -203,
        -267, -202, -268, -193, -201, -173, -117, -209,  392,  383,
         382,  381,  380,  379,  378,  377,  376,  374,  373,  372,
         371,  370,  369,  368,  367,  365,  364,  363,  362,  256,
        -116,  331, -177,  -19,  315,  256, -180,  387, -131,  -10,
        -186, -275, -272, -273, -277, -274, -271, -276, -279, -278,
        -139, -270,  392,  389,  361,  358,  352,  351,  348,  347,
         346,  345,  343,  326,  310,  308, -187, -206, -200, -174,
        -195, -137, -204, -196, -198, -197, -205, -199, -194, -203,
        -267, -202, -268, -193, -201, -173, -190, -209,  392,  387,
         382,  381,  380,  379,  378,  377,  376,  374,  373,  372,
         371,  370,  369,  368,  367,  365,  364,  363,  362,  256,
        -147,  364, -148,  382, -163,  309, -140, -206, -200, -174,
        -195, -137, -204, -196, -198, -197, -205, -199, -194, -203,
        -267, -202, -268, -193, -201, -173, -209,  392,  382,  381,
         380,  379,  378,  377,  376,  374,  373,  372,  371,  370,
         369,  368,  367,  365,  364,  363,  362,  321, -220, -223,
        -109, -224, -222, -221, -161, -111, -219, -139, -168, -171,
         393,  390,  389,  388,  375,  339,  316,  312,  310,  308,
         305,  301, -220, -223, -109, -224, -222, -221, -111, -219,
        -139, -168, -171,  393,  390,  389,  388,  375,  316,  312,
         310,  308,  305,  301, -238, -206, -200, -174, -195, -137,
          -3, -196, -198, -197, -205, -199, -194, -203, -267, -202,
        -268, -193, -201, -173, -209,  392,  382,  381,  380,  379,
         378,  377,  376,  374,  373,  372,  371,  370,  369,  368,
         367,  365,  364,  363,  362,  256, -181,  387, -107,  324,
        -106, -185,  326,  315,  -36,  -10, -105, -275, -272, -273,
        -277, -274, -271, -276, -279, -278, -139, -270,  392,  389,
         361,  358,  352,  351,  348,  347,  346,  345,  343,  338,
         310,  308, -104,  315, -160,  309, -162,  339, -168,  390,
        -103, -102,  386,  337, -101,  306, -100,  384,  -99,  340,
         -98,  305,  -97,  -96,  332,  302,  -92,  -93,  -94,  -95,
         334,  333,  330,  327,  -91,  -90,  335,  328,  -88,  -89,
         316,  312,  -85,  -86,  -87,  394,  322,  310, -220, -223,
         -84, -224, -222, -221, -111, -219, -139, -168, -171,  393,
         390,  389,  388,  375,  316,  312,  310,  308,  305,  301,
         -40,  -83,  -82,  -81,  338,  320,  319,  308, -220, -223,
        -109, -224, -222, -221, -206, -200, -174, -195, -137, -204,
        -196, -198, -197, -205, -199, -194, -111, -203, -267, -202,
        -268, -193, -201, -173, -219, -139, -168, -209, -171,  393,
         392,  390,  389,  388,  382,  381,  380,  379,  378,  377,
         376,  375,  374,  373,  372,  371,  370,  369,  368,  367,
         365,  364,  363,  362,  316,  312,  310,  308,  305,  301,
        -238, -241, -206, -200, -174, -195, -137,   -3, -196, -198,
        -197, -205, -199, -194, -203, -267, -202, -268, -193, -201,
        -173,  -42, -209,  392,  387,  382,  381,  380,  379,  378,
         377,  376,  374,  373,  372,  371,  370,  369,  368,  367,
         365,  364,  363,  362,  300,  256, -220, -223, -109, -224,
        -222, -221, -153, -111, -219, -139, -168, -171,  393,  390,
         389,  388,  375,  339,  316,  312,  310,  308,  305,  301,
         -43,  -77,  338,  308,  -36,  -10, -140, -105, -275, -272,
        -273, -277, -274, -271, -276, -279, -278, -206, -200, -174,
        -195, -137, -204, -196, -198, -197, -205, -199, -194, -203,
        -267, -202, -268, -193, -201, -173, -139,  -44,  392,  389,
         382,  381,  380,  379,  378,  377,  376,  374,  373,  372,
         371,  370,  369,  368,  367,  365,  364,  363,  362,  361,
         358,  352,  351,  348,  347,  346,  345,  343,  338,  321,
         310,  308,  -61,  -10, -105, -275, -272, -273, -277, -274,
        -271, -276, -279, -278,  361,  358,  352,  351,  348,  347,
         346,  345,  343,  338,  310,  308,  -70,  309, -234,  -69,
         315,  309, -242,  326, -152,  339, -220, -223, -109, -224,
        -222, -221, -155, -111, -219, -139, -168, -171,  393,  390,
         389,  388,  375,  339,  316,  312,  310,  308,  305,  301,
        -150,  309, -156,  309,  -69,  -66,  324,  315, -225,  309,
         -65,  315, -231,  309,  -69, -232,  339,  315,  -61,  -10,
        -140, -105, -275, -272, -273, -277, -274, -271, -276, -279,
        -278, -206, -200, -174, -195, -137, -204, -196, -198, -197,
        -205, -199, -194, -203, -267, -202, -268, -193, -201, -173,
        -209,  392,  382,  381,  380,  379,  378,  377,  376,  374,
         373,  372,  371,  370,  369,  368,  367,  365,  364,  363,
         362,  361,  358,  352,  351,  348,  347,  346,  345,  343,
         338,  321,  310,  308, -151,  309, -154,  339,   -1
};
static short cpact[] = {
          70,  106,  155,  156,  200,  203,  205,   70,   70,  208,
         238,  241,  291,  294,   70,  371,  373,  397,  201,  509,
         528,  543,  549,  552,  555,  557,  559,  561,  564,  570,
         576,  580,  585,  614,  752,  788,  528,  528,  528,  453,
         834,  853,  397,  884,  555,  557,  559,  561,  564,  570,
         570,  576,  576,  576,  576,  580,  580,  585,  585,  891,
         931,  834,  834,  834,  453,  453,  967,  965,  453,  453,
         896,  893,  889,  886,  883,  881,  868,  855,  850,  847,
         453,  201,  201,  649,  453,  453,  453,  453,  453,  453,
         453,  453,  453,  453,  453,  453,  453,  453,  453,  453,
         453,  453,  453,  397,  738,  223,  453,  703,  649,  453,
         599,  547,  545,  512,  507,  453,  485,  223,  453,  430,
         375,  348,  312,  201,  297,  268,  244,  223,  223,  223,
         223,  106,  201,  178,  140,   51,   46,   22
};
static short cgo[] = {
        -243, -244,  125, -236, -237,  117, -235, -127,   -5, -245,
        -245, -246,  133,  107, -108, -134,  116, -251,  -34,  -72,
         -60, -226, -253,  110, -110, -218, -217, -216, -215, -227,
        -254,  109,   86,   85,   84,   69,  -59,  -58,  -33,   88,
          87,  -57,  -56,  -32,   90,   89,  -55,  -54,  -53,  -52,
         -31,   94,   93,   92,   91,  -51,  -50,  -30,   96,   95,
         -49,  -29,   97,  -48,  -28,   98,  -47,  -27,   99,  -46,
         -26,  100,  -45,  -25,  101,  -24, -257, -257, -257, -257,
        -257, -255,  119,  115,  106,  104,   76, -252, -228, -214,
        -213, -249, -256,  118,   68,   65,   64,   39,  -71,  -74,
         -79,  102,   80,  -67,  -78, -182, -175, -112,  115,  106,
         104,   76, -164,  -21,  -21,  -21,  -21,  -41,  -21,  -41,
        -123, -123, -135,  131,  121,  108,  103,   83,   60,   42,
          35,   17,   -2, -263, -212, -208, -263, -208, -258,   14,
           8,    7,    1,    0,   -9,   -1,    1, -262,  -15,   -8,
           8, -259, -264, -207, -264, -207, -261,   14,    8,    7,
           0, -211,   -7, -136, -192, -191,    6, -122, -188, -189,
         121, -114, -183, -184,  105, -210, -115, -125,  123,  -14,
        -178, -176, -179,   18, -260,  -73,  -80,   83, -250,  -23,
        -170, -169,   22, -142, -121, -165, -166, -167,  -20,  -20,
        -167, -166, -165, -121,  -12,  130,  129,  128,  127,  122,
         105,   38,   37,   36,   35,   20,  -11, -141, -157, -158,
        -159, -172, -157, -158, -159,  -75,   63,   62,   61,   40,
          38,   37,   36,   20,  -35,  -37,  -37,  -37,  -37,  -37,
         -62,  -62,  -62,  -62,  -62, -130,   63,   62,   61,   60,
          40,   38,   37,   36,   35,   20,  -38,  -38,  -38,  -38,
         -38,  -63,  -63,  -63,  -63,  -63, -129,   63,   62,   61,
          60,   40,   38,   37,   36,   35,   20,  -39,  -39,  -39,
         -39,  -39,  -64,  -64,  -64,  -64,  -64, -128,   63,   62,
          61,   60,   40,   38,   37,   36,   35,   20, -149, -113,
         -68,  -76,   42,   17,  -22, -143, -144,  103, -146, -145,
         103,  -13, -269, -269, -269, -269, -269, -230, -229, -269,
        -269, -269,  -13, -269, -269, -269, -269,  -13, -269,   -6,
        -233,  136,  134,  132,  130,  129,  128,  127,  123,  122,
         117,  105,   82,   81,   38,   37,   36,   35,   20,   18,   -1
};
static short cpgo[] = {
           0,    0,    0,    8,   11,   19,   43,   43,   50,   50,
          57,   57,   57,   57,   61,   61,   64,   67,   70,   73,
          75,  122,  122,  122,  161,  175,  177,  177,  182,  186,
         204,  225,  225,  225,  225,  298,  298,  298,  301,  306,
         330,  309,  306,  306,  304,  304,  301,  301,  298,  298,
         245,  234,  234,  234,  234,  234,  234,  234,  225,  225,
         225,  216,  216,  216,  216,  216,  204,  204,  204,  191,
         189,  189,  188,  186,  184,  184,  182,  180,  179,  179,
         179,  175,  175,  173,  171,  171,  169,  169,  167,  167,
         167,  165,  161,  161,  156,  156,  156,  156,  156,  156,
         156,  156,  156,  151,  151,  151,  151,  151,  149,  149,
         147,  145,  145,  132,  100,   81,   38,   38,   38,   30,
          24,   24,   24,   24,   24,   24,   22,   22,   22,   20,
          18,   18,   18,   18,   17,   17,    4,    7,    7,    1,
           1,    1,   11,   11,   11,   11,   15,   15,    8,    8,
           6,   17,   18,   20,   30,   38,   92,  100,  107,  132,
         138,  138,  145,  145,  149,  149,  162,  162,  163,  163,
         112,  112,  266,  266,  266,  266,  287,  287,  287,  287,
         287,  301,  188,  173,  122,  122,   81,   75,   73,   70,
          67,   64,   61,   57,   50,   43,   22,   19,    4,    0
};
static short crlen[] = {
           0,    0,    0,    1,    5,    0,    3,    3,    3,    3,
           3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
           3,    3,    2,    2,    2,    2,    1,    2,    1,    1,
           1,    1,    1,    1,    1,    0,    1,    1,    0,    1,
           1,    1,    2,    2,    3,    1,    1,    3,    2,    2,
           2,    3,    4,    3,    2,    4,    3,    3,    2,    2,
           2,    4,    3,    4,    3,    1,    2,    2,    2,    1,
           1,    2,    1,    2,    1,    1,    3,    2,    2,    2,
           1,    4,    5,    3,    3,    1,    3,    2,    2,    2,
           1,    3,    2,    3,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    2,    2,
           1,    1,    1,    2,    3,    5,    3,    3,    3,    4,
           1,    1,    1,    1,    1,    1,    4,    2,    2,    3,
           3,    3,    4,    4,    1,    3,    3,    3,    1,    1,
           3,    2,    5,    6,    3,    1,    2,    1,    2,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
           1,    1,    1,    1,    1,    1,    1,    1,    1,    2
};
#define CS0     137
#define CDELTA  98
#define CNPACT  138
#define CNDEF   64

#define Cr197   0
#define Cr198   1
#define Cr199   2
#define Cr3     3
#define Cr8     4
#define Cr27    5
#define Cr48    6
#define Cr49    7
#define Cr51    8
#define Cr52    9
#define Cr54    10
#define Cr55    11
#define Cr56    12
#define Cr57    13
#define Cr59    14
#define Cr60    15
#define Cr62    16
#define Cr64    17
#define Cr66    18
#define Cr68    19
#define Cr70    20
#define Cr77    21
#define Cr78    22
#define Cr79    23
#define Cr111   24
#define Cr127   25
#define Cr129   26
#define Cr130   27
#define Cr135   28
#define Cr139   29
#define Cr151   30
#define Cr158   31
#define Cr160   32
#define Cr162   33
#define Cr163   34
#define Cr181   35
#define Cr182   36
#define Cr184   37
#define Cr186   38
#define Cr192   39
#define Cr196   40
#define Cr195   41
#define Cr194   42
#define Cr193   43
#define Cr191   44
#define Cr190   45
#define Cr189   46
#define Cr188   47
#define Cr185   48
#define Cr183   49
#define Cr171   50
#define Cr170   51
#define Cr169   52
#define Cr168   53
#define Cr167   54
#define Cr166   55
#define Cr165   56
#define Cr164   57
#define Cr161   58
#define Cr159   59
#define Cr157   60
#define Cr156   61
#define Cr155   62
#define Cr154   63
#define Cr153   64
#define Cr152   65
#define Cr150   66
#define Cr149   67
#define Cr148   68
#define Cr145   69
#define Cr144   70
#define Cr143   71
#define Cr142   72
#define Cr140   73
#define Cr138   74
#define Cr137   75
#define Cr136   76
#define Cr134   77
#define Cr133   78
#define Cr132   79
#define Cr131   80
#define Cr128   81
#define Cr126   82
#define Cr125   83
#define Cr123   84
#define Cr122   85
#define Cr121   86
#define Cr120   87
#define Cr119   88
#define Cr118   89
#define Cr117   90
#define Cr116   91
#define Cr110   92
#define Cr109   93
#define Cr108   94
#define Cr107   95
#define Cr106   96
#define Cr105   97
#define Cr104   98
#define Cr103   99
#define Cr102   100
#define Cr101   101
#define Cr100   102
#define Cr99    103
#define Cr98    104
#define Cr97    105
#define Cr96    106
#define Cr95    107
#define Cr94    108
#define Cr93    109
#define Cr90    110
#define Cr89    111
#define Cr88    112
#define Cr83    113
#define Cr75    114
#define Cr72    115
#define Cr46    116
#define Cr45    117
#define Cr44    118
#define Cr42    119
#define Cr40    120
#define Cr39    121
#define Cr38    122
#define Cr37    123
#define Cr36    124
#define Cr35    125
#define Cr34    126
#define Cr33    127
#define Cr32    128
#define Cr30    129
#define Cr26    130
#define Cr25    131
#define Cr24    132
#define Cr23    133
#define Cr21    134
#define Cr20    135
#define Cr17    136
#define Cr15    137
#define Cr14    138
#define Cr13    139
#define Cr12    140
#define Cr11    141
#define Cr10    142
#define Cr9     143
#define Cr7     144
#define Cr6     145
#define Cr5     146
#define Cr4     147
#define Cr2     148
#define Cr1     149
#define CrACCEPT        Cr197
#define CrERROR Cr198
#define CrLR2   Cr199

/*
 * Automaton to interpret LALR(1) tables.
 *
 *      Macros:
 *              cclearin - clear the lookahead token.
 *              cerrok - forgive a pending error
 *              CERROR - simulate an error
 *              CACCEPT - halt and return 0
 *              CABORT - halt and return 1
 *              CRETURN(value) - halt and return value.  You should use this
 *                      instead of return(value).
 *              CREAD - ensure cchar contains a lookahead token by reading
 *                      one if it does not.  See also CSYNC.
 *
 *      Preprocessor flags:
 *              CDEBUG - includes debug code.  The parser will print
 *                       a travelogue of the parse if this is defined
 *                       and cdebug is non-zero.
 *              CSSIZE - size of state and value stacks (default 150).
 *              CSTATIC - By default, the state stack is an automatic array.
 *                      If this is defined, the stack will be static.
 *                      In either case, the value stack is static.
 *              CALLOC - Dynamically allocate both the state and value stacks
 *                      by calling malloc() and free().
 *              CLR2 - defined if lookahead is needed to resolve R/R or S/R conflicts
 *              CSYNC - if defined, yacc guarantees to fetch a lookahead token
 *                      before any action, even if it doesnt need it for a decision.
 *                      If CSYNC is defined, CREAD will never be necessary unless
 *                      the user explicitly sets cchar = -1
 *
 *      Copyright (c) 1983, by the University of Waterloo
 */

#ifndef CSSIZE
# define CSSIZE 150
#endif
#ifndef CDEBUG
#define CDEBUG  0
#endif
#define CERROR          goto cerrlabel
#define cerrok          cerrflag = 0
#define cclearin        cchar = -1
#define CACCEPT CRETURN(0)
#define CABORT          CRETURN(1)
#ifdef CALLOC
# define CRETURN(val)   { retval = (val); goto cReturn; }
#else
# define CRETURN(val)   return(val)
#endif
#if CDEBUG
/* The if..else makes this macro behave exactly like a statement */
# define CREAD  if (cchar < 0) {                                        \
                        if ((cchar = clex()) < 0)                       \
                                cchar = 0;                              \
                        if (cdebug)                                     \
                                printf("read %s (%d)\n", cptok(cchar),\
                                cchar);                         \
                } else
#else
# define CREAD  if (cchar < 0) {                                        \
                        if ((cchar = clex()) < 0)                       \
                                cchar = 0;                              \
                } else
#endif
#define CERRCODE        256             /* value of `error' */
#if defined(__TURBOC__)&&__SMALL__
#define CQCP    *(int *)((int)cq + ((int)cq-(int)cp))
#else
#define CQCP    cq[cq-cp]
#endif

CSTYPE  cval,                           /* $$ */
        *cpvt,                          /* $n */
        clval;                          /* clex() sets this */

int     cchar,                          /* current token */
        cerrflag,                       /* error flag */
        cnerrs;                 /* error count */

#if CDEBUG
int cdebug = CDEBUG-0;          /* debug flag & tables */
extern char     *csvar[], *cstoken[], *cptok();
extern short    crmap[], csmap[];
extern int      cnstate, cnvar, cntoken, cnrule;
# define cassert(condition, msg, arg) \
        if (!(condition)) { printf("\nyacc bug: "); printf(msg, arg); CABORT; }
#else /* !CDEBUG */
# define cassert(condition, msg, arg)
#endif

cparse()
{

        register short          ci, *cp;        /* for table lookup */
        register short          *cps;           /* top of state stack */
        register short          cstate; /* current state */
        register CSTYPE *cpv;           /* top of value stack */
        register short          *cq;
        register int            cj;

#ifdef CSTATIC
        static short    cs[CSSIZE + 1];
        static CSTYPE   cv[CSSIZE + 1];
#else
#ifdef CALLOC
        CSTYPE *cv;
        short   *cs;
        CSTYPE save_clval, save_cval, *save_cpvt;
        int save_cchar, save_cerrflag, save_cnerrs;
        int retval;
#if 0   /* defined in <stdlib.h>*/
        extern char     *malloc();
#endif
#else
        short           cs[CSSIZE + 1];
        static CSTYPE   cv[CSSIZE + 1]; /* historically static */
#endif
#endif

#ifdef CALLOC
        cs = (short *) malloc((CSSIZE + 1) * sizeof(short));
        cv = (CSTYPE *) malloc((CSSIZE + 1) * sizeof(CSTYPE));
        if (cs == (short *)0 || cv == (CSTYPE *)0) {
                cerror("Not enough space for parser stacks");
                return 1;
        }
        save_clval = clval;
        save_cval = cval;
        save_cpvt = cpvt;
        save_cchar = cchar;
        save_cerrflag = cerrflag;
        save_cnerrs = cnerrs;
#endif

        cnerrs = 0;
        cerrflag = 0;
        cchar = -1;
        cps = cs;
        cpv = cv;
        cstate = CS0;           /* start state */

cStack:
        cassert((unsigned)cstate < cnstate, "state %d\n", cstate);
        if (++cps > &cs[CSSIZE]) {
                cerror("Parser stack overflow");
                CABORT;
        }
        *cps = cstate;  /* stack current state */
        *++cpv = cval;  /* ... and value */

#if CDEBUG
        if (cdebug)
                printf("state %d (%d), char %s (%d)\n", csmap[cstate],
                        cstate, cptok(cchar), cchar);
#endif

        /*
         *      Look up next action in action table.
         */
cEncore:
#ifdef CSYNC
        CREAD;
#endif
        if (cstate >= sizeof cpact/sizeof cpact[0])     /* simple state */
                ci = cstate - CDELTA;   /* reduce in any case */
        else {
                if(*(cp = &cact[cpact[cstate]]) >= 0) {
                        /* Look for a shift on cchar */
#ifndef CSYNC
                        CREAD;
#endif
                        cq = cp;
                        ci = cchar;
#if 0&&defined(__TURBOC__)&&__SMALL__
        /* THIS ONLY WORKS ON TURBO C 1.5 !!! */
                        /* ci is in di, cp is in si */
                L01:
                        asm lodsw       /* ax = *cp++; */
                        asm cmp ci, ax
                        asm jl L01
#else
                        while (ci < *cp++)
                                ;
#endif
                        if (ci == cp[-1]) {
                                cstate = ~CQCP;
#if CDEBUG
                                if (cdebug)
                                        printf("shift %d (%d)\n", csmap[cstate], cstate);
#endif
                                cval = clval;           /* stack what clex() set */
                                cchar = -1;             /* clear token */
                                if (cerrflag)
                                        cerrflag--;     /* successful shift */
                                goto cStack;
                        }
                }

                /*
                 *      Fell through - take default action
                 */

                if (cstate >= sizeof cdef /sizeof cdef[0])
                        goto cError;
                if ((ci = cdef[cstate]) < 0)     { /* default == reduce? */
                                                                                        /* Search exception table */
                        cassert((unsigned)~ci < sizeof cex/sizeof cex[0],
                                "exception %d\n", cstate);
                        cp = &cex[~ci];
#ifndef CSYNC
                        CREAD;
#endif
                        while((ci = *cp) >= 0 && ci != cchar)
                                cp += 2;
                        ci = cp[1];
                        cassert(ci >= 0,"Ex table not reduce %d\n", ci);
                }
        }

#ifdef CLR2
cReduce:        /* reduce ci */
#endif
        cassert((unsigned)ci < cnrule, "reduce %d\n", ci);
        cj = crlen[ci];
#if CDEBUG
        if (cdebug) printf("reduce %d (%d), pops %d (%d)\n", crmap[ci],
                ci, csmap[cps[-cj]], cps[-cj]);
#endif
        cps -= cj;              /* pop stacks */
        cpvt = cpv;             /* save top */
        cpv -= cj;
        cval = cpv[1];  /* default action $$ = $1 */
        switch (ci) {           /* perform semantic action */

case Cr1: {     /* parse-interface :  Y_EOF */
 outputZapEOF(cpvt[0].token); CACCEPT;
} break;

case Cr2: {     /* parse-interface :  translation-unit Y_EOF */
 outputZapEOF(cpvt[0].token); CACCEPT;
} break;

case Cr3: {     /* parse-interface :  error */
 recoverError(); CACCEPT;
} break;

case Cr4: {     /* translation-unit :  external-definition */

} break;

case Cr5: {     /* translation-unit :  translation-unit external-definition */

} break;

case Cr6: {     /* external-definition :  decl */
 outputZapAll(cpvt[0].declList);
} break;

case Cr7: {     /* external-definition :  Y_EXTERN Y_STRING decl */
 outputZapAll(cpvt[0].declList); zapTokens2(cpvt[-2].token, cpvt[-1].token);
} break;

case Cr8: {     /* external-definition :  Y_EXTERN Y_STRING Y_LEFT_BRACE translation-unit Y_RIGHT_BRACE */
 zapTokens2(cpvt[-4].token, cpvt[-3].token); zapTokens2(cpvt[-2].token, cpvt[0].token);
} break;

case Cr9: {     /* external-definition :  Y_EXTERN Y_STRING Y_LEFT_BRACE translation-unit Y_RIGHT_BRACE Y_SEMICOLON */
 zapTokens2(cpvt[-5].token, cpvt[-4].token); zapTokens3(cpvt[-3].token, cpvt[-1].token, cpvt[0].token);
} break;

case Cr10: {    /* external-definition :  Y_EXTERN Y_STRING Y_LEFT_BRACE translation-unit Y_EOF */

            reportError(RERR_MISSING_RIGHT_BRACE);
            zapTokens2(cpvt[-4].token, cpvt[-3].token); zapTokens2(cpvt[-2].token, cpvt[0].token);

} break;

case Cr11: {    /* decl :  decl-specs Y_SEMICOLON */
 cval.declList = transformDecl(cpvt[-1].dinfo); zapToken(cpvt[0].token);
} break;

case Cr12: {    /* decl :  decl-specs init-declarator-list Y_SEMICOLON */
 cval.declList = transformDecl(addDeclInfoDclrList(cpvt[-2].dinfo, cpvt[-1].dclrList)); zapToken(cpvt[0].token);
} break;

case Cr13: {    /* decl :  error */
 recoverError(); cval.declList = NULL;
} break;

case Cr14: {    /* init-declarator-list :  init-declarator */
 cval.dclrList = createDclrList(cpvt[0].dclr);
} break;

case Cr15: {    /* init-declarator-list :  init-declarator-list Y_COMMA init-declarator */
 cval.dclrList = addDclrList(cpvt[-2].dclrList,  cpvt[0].dclr); zapToken(cpvt[-1].token);
} break;

case Cr17: {    /* init-declarator :  declarator Y_EQUAL initializer */
 addDclrInitializer(cpvt[-2].dclr, cpvt[-1].token, cpvt[0].tree);
} break;

case Cr20: {    /* primary-expression :  Y_LEFT_PAREN expression Y_RIGHT_PAREN */
 cval.tree = createCTree1(createConstr2Label(LABCT_PAREN_EXPR, cpvt[-2].token, cpvt[0].token), cpvt[-1].tree);
} break;

case Cr21: {    /* primary-expression :  identifier */
 cval.tree = createCTreeRoot(createTokenLabel(cpvt[0].token));
} break;

case Cr23: {    /* postfix-expression :  postfix-expression Y_LEFT_BRACKET expression Y_RIGHT_BRACKET */
 cval.tree = createCTree2(createConstr2Label(LABCT_INDEX, cpvt[-2].token, cpvt[0].token), cpvt[-3].tree,  cpvt[-1].tree);
} break;

case Cr24: {    /* postfix-expression :  postfix-expression Y_LEFT_PAREN expression-list-opt Y_RIGHT_PAREN */
 cval.tree = createCTree2(createConstr2Label(LABCT_CALL, cpvt[-2].token, cpvt[0].token), cpvt[-3].tree,  cpvt[-1].tree);
} break;

case Cr25: {    /* postfix-expression :  postfix-expression Y_DOT identifier */
 cval.tree = createCTree2(createConstr1Label(LABCT_DOT, cpvt[-1].token), cpvt[-2].tree,
                            createCTreeRoot(createTokenLabel(cpvt[0].token)));
} break;

case Cr26: {    /* postfix-expression :  postfix-expression Y_ARROW identifier */
 cval.tree = createCTree2(createConstr1Label(LABCT_ARROW, cpvt[-1].token), cpvt[-2].tree,
                            createCTreeRoot(createTokenLabel(cpvt[0].token)));
} break;

case Cr27: {    /* expression-list-opt :  */
 cval.tree = createNULLCTree();
} break;

case Cr30: {    /* expression-list :  expression-list Y_COMMA assignment-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_EXPR_LIST, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr32: {    /* unary-expression :  unary-operator cast-expression */
 cval.tree = createCTree1( cpvt[-1].label, cpvt[0].tree );
} break;

case Cr33: {    /* unary-expression :  Y_SIZEOF unary-expression */
 cval.tree = createCTree1(createConstr1Label(LABCT_SIZEOF_EXPR, cpvt[-1].token), cpvt[0].tree);
} break;

case Cr34: {    /* unary-expression :  Y_SIZEOF Y_LEFT_PAREN type-name Y_RIGHT_PAREN */
 cval.tree = createCTree1(createConstr3Label(LABCT_SIZEOF_TYPE, cpvt[-3].token, cpvt[-2].token, cpvt[0].token), cpvt[-1].tree);
} break;

case Cr35: {    /* unary-operator :  Y_TIMES */
 cval.label = createConstr1Label(LABCT_VALUE_AT_ADDR, cpvt[0].token);
} break;

case Cr36: {    /* unary-operator :  Y_AND */
 cval.label = createConstr1Label(LABCT_ADDR_OF_VALUE, cpvt[0].token);
} break;

case Cr37: {    /* unary-operator :  Y_PLUS */
 cval.label = createConstr1Label(LABCT_UNARY_PLUS, cpvt[0].token);
} break;

case Cr38: {    /* unary-operator :  Y_MINUS */
 cval.label = createConstr1Label(LABCT_UNARY_MINUS, cpvt[0].token);
} break;

case Cr39: {    /* unary-operator :  Y_EXCLAMATION */
 cval.label = createConstr1Label(LABCT_EXCLAMATION, cpvt[0].token);
} break;

case Cr40: {    /* unary-operator :  Y_TILDE */
 cval.label = createConstr1Label(LABCT_TILDE, cpvt[0].token);
} break;

case Cr42: {    /* cast-expression :  Y_LEFT_PAREN type-name Y_RIGHT_PAREN cast-expression */
 cval.tree = createCTree2(createConstr2Label(LABCT_CAST_EXPR, cpvt[-3].token, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr44: {    /* multiplicative-expression :  multiplicative-expression Y_TIMES cast-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_TIMES, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr45: {    /* multiplicative-expression :  multiplicative-expression Y_DIVIDE cast-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_DIVIDE, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr46: {    /* multiplicative-expression :  multiplicative-expression Y_PERCENT cast-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_PERCENT, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr48: {    /* additive-expression :  additive-expression Y_PLUS multiplicative-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_PLUS, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr49: {    /* additive-expression :  additive-expression Y_MINUS multiplicative-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_MINUS, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr51: {    /* shift-expression :  shift-expression Y_RSHIFT additive-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_RSHIFT, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr52: {    /* shift-expression :  shift-expression Y_LSHIFT additive-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_LSHIFT, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr54: {    /* relational-expression :  relational-expression Y_LT shift-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_LT, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr55: {    /* relational-expression :  relational-expression Y_LE shift-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_LE, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr56: {    /* relational-expression :  relational-expression Y_GT shift-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_GT, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr57: {    /* relational-expression :  relational-expression Y_GE shift-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_GE, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr59: {    /* equality-expression :  equality-expression Y_EQ relational-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_EQ, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr60: {    /* equality-expression :  equality-expression Y_NE relational-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_NE, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr62: {    /* and-expression :  and-expression Y_AND equality-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_AND, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr64: {    /* exclusive-or-expression :  exclusive-or-expression Y_XOR and-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_XOR, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr66: {    /* inclusive-or-expression :  inclusive-or-expression Y_OR exclusive-or-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_OR, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr68: {    /* logical-and-expression :  logical-and-expression Y_AND_AND inclusive-or-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_AND_AND, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr70: {    /* logical-or-expression :  logical-or-expression Y_OR_OR logical-and-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_OR_OR, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr72: {    /* conditional-expression :  logical-or-expression Y_QUESTION expression Y_COLON assignment-expression */

            cval.tree = createCTree2(createConstr1Label(LABCT_QUESTION, cpvt[-3].token), cpvt[-4].tree,
                        createCTree2(createConstr1Label(LABCT_COLON, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree));

} break;

case Cr75: {    /* expression :  expression Y_COMMA assignment-expression */
 cval.tree = createCTree2(createConstr1Label(LABCT_EXPR_SEPARATOR, cpvt[-1].token), cpvt[-2].tree, cpvt[0].tree);
} break;

case Cr77: {    /* decl-specs :  non-type-decl-specs type-spec maybe-type-decl-specs */
 cval.dinfo = combine2DeclInfo(combine2DeclInfo(cpvt[-2].dinfo, cpvt[-1].dinfo), cpvt[0].dinfo);
} break;

case Cr78: {    /* decl-specs :  non-type-decl-specs type-spec */
 cval.dinfo = combine2DeclInfo(cpvt[-1].dinfo, cpvt[0].dinfo);
} break;

case Cr79: {    /* decl-specs :  type-spec maybe-type-decl-specs */
 cval.dinfo = combine2DeclInfo(cpvt[-1].dinfo, cpvt[0].dinfo);
} break;

case Cr83: {    /* non-type-decl-specs :  non-type-decl-specs non-type-decl-spec */
 cval.dinfo = combine2DeclInfo(cpvt[-1].dinfo, cpvt[0].dinfo);
} break;

case Cr88: {    /* type-spec :  struct-or-union-spec */
 cval.dinfo = createDeclInfoSTRUCT(cpvt[0].dsinfo);
} break;

case Cr89: {    /* type-spec :  enum-spec */
 cval.dinfo = createDeclInfoENUM(cpvt[0].declEnum);
} break;

case Cr90: {    /* typedef-name :  Y_TYPEDEF_NAME */
 cval.dinfo = dupDeclInfo(cpvt[0].token->data->repr.pTypeDecl, cpvt[0].token->pos); zapToken(cpvt[0].token);
} break;

case Cr93: {    /* maybe-type-decl-specs :  maybe-type-decl-specs non-type-decl-spec */
 cval.dinfo = combine2DeclInfo(cpvt[-1].dinfo, cpvt[0].dinfo);
} break;

case Cr94: {    /* maybe-type-decl-specs :  maybe-type-decl-specs scalar-type-spec */
 cval.dinfo = combine2DeclInfo(cpvt[-1].dinfo, cpvt[0].dinfo);
} break;

case Cr95: {    /* storage-class-spec :  Y_AUTO */
 cval.dinfo = createStgClassDeclInfo(STG_AUTO, cpvt[0].token);
            reportError(CERR_CANT_CONVERT, "auto");
} break;

case Cr96: {    /* storage-class-spec :  Y_REGISTER */
 cval.dinfo = createStgClassDeclInfo(STG_REGISTER, cpvt[0].token);
            reportError(CERR_CANT_CONVERT, "register");
} break;

case Cr97: {    /* storage-class-spec :  Y_EXTERN */
 cval.dinfo = createStgClassDeclInfo(STG_EXTERN, cpvt[0].token);
} break;

case Cr98: {    /* storage-class-spec :  Y_STATIC */
 cval.dinfo = createStgClassDeclInfo(STG_STATIC, cpvt[0].token);
            reportError(CERR_CANT_CONVERT, "static");
} break;

case Cr99: {    /* storage-class-spec :  Y_TYPEDEF */
 cval.dinfo = createStgClassDeclInfo(STG_TYPEDEF, cpvt[0].token);
} break;

case Cr100: {   /* scalar-type-spec :  Y_VOID */
 cval.dinfo = createDeclInfoSCALAR(STM_VOID, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr101: {   /* scalar-type-spec :  Y_CHAR */
 cval.dinfo = createDeclInfoSCALAR(STM_CHAR, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr102: {   /* scalar-type-spec :  Y_SHORT */
 cval.dinfo = createDeclInfoSCALAR(STM_SHORT, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr103: {   /* scalar-type-spec :  Y_INT */
 cval.dinfo = createDeclInfoSCALAR(STM_INT, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr104: {   /* scalar-type-spec :  Y_LONG */
 cval.dinfo = createDeclInfoSCALAR(STM_LONG, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr105: {   /* scalar-type-spec :  Y_FLOAT */
 cval.dinfo = createDeclInfoSCALAR(STM_FLOAT, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr106: {   /* scalar-type-spec :  Y_DOUBLE */
 cval.dinfo = createDeclInfoSCALAR(STM_DOUBLE, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr107: {   /* scalar-type-spec :  Y_SIGNED */
 cval.dinfo = createDeclInfoSCALAR(STM_SIGNED, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr108: {   /* scalar-type-spec :  Y_UNSIGNED */
 cval.dinfo = createDeclInfoSCALAR(STM_UNSIGNED, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr109: {   /* struct-or-union-spec :  struct-or-union struct-id struct-or-union-body */
 cval.dsinfo = createDeclStructInfo(cpvt[-2].token, cpvt[-1].token, cpvt[0].dsbody);
} break;

case Cr110: {   /* struct-or-union-spec :  struct-or-union struct-or-union-body */
 cval.dsinfo = createDeclStructInfo(cpvt[-1].token, NULL, cpvt[0].dsbody);
} break;

case Cr111: {   /* struct-or-union-spec :  struct-or-union struct-id */
 cval.dsinfo = createDeclStructInfo(cpvt[-1].token, cpvt[0].token, NULL);
} break;

case Cr116: {   /* struct-or-union-body :  Y_LEFT_BRACE struct-decl-list Y_RIGHT_BRACE */

            cval.dsbody = createDeclStructBody(cpvt[-1].declList, cpvt[0].token->pos);
            cpvt[0].token->pos = NULL; zapTokens2(cpvt[-2].token, cpvt[0].token);

} break;

case Cr117: {   /* struct-decl-list :  struct-decl */
 cval.declList = createDeclList(cpvt[0].dinfo);
} break;

case Cr118: {   /* struct-decl-list :  struct-decl-list struct-decl */
 cval.declList = addDeclList(cpvt[-1].declList, cpvt[0].dinfo);
} break;

case Cr119: {   /* struct-decl-list :  struct-decl-list error */
 recoverError(); cval.declList = cpvt[-1].declList;
} break;

case Cr120: {   /* struct-decl :  decl-specs Y_SEMICOLON */
 cval.dinfo = addDeclInfoDclrList(cpvt[-1].dinfo, NULL); zapToken(cpvt[0].token);
} break;

case Cr121: {   /* struct-decl :  decl-specs struct-declarator-list Y_SEMICOLON */
 cval.dinfo = addDeclInfoDclrList(cpvt[-2].dinfo, cpvt[-1].dclrList); zapToken(cpvt[0].token);
} break;

case Cr122: {   /* struct-declarator-list :  struct-declarator */
 cval.dclrList = createDclrList(cpvt[0].dclr);
} break;

case Cr123: {   /* struct-declarator-list :  struct-declarator-list Y_COMMA struct-declarator */
 cval.dclrList = addDclrList(cpvt[-2].dclrList, cpvt[0].dclr); zapToken(cpvt[-1].token);
} break;

case Cr125: {   /* struct-declarator :  declarator Y_COLON constant-expression */

            reportError(CERR_CANT_CONVERT_BIT_FIELDS);
            cval.dclr = cpvt[-2].dclr;
            zapToken(cpvt[-1].token);  zapCTree(cpvt[0].tree);

} break;

case Cr126: {   /* enum-spec :  Y_ENUM identifier Y_LEFT_BRACE enum-list Y_RIGHT_BRACE */
 cval.declEnum = createDeclEnum(cpvt[-4].token, cpvt[-1].enumList); zapTokens3(cpvt[-3].token, cpvt[-2].token, cpvt[0].token);
} break;

case Cr127: {   /* enum-spec :  Y_ENUM identifier */
 cval.declEnum = createDeclEnum(cpvt[-1].token, NULL); zapToken(cpvt[0].token);
} break;

case Cr128: {   /* enum-spec :  Y_ENUM Y_LEFT_BRACE enum-list Y_RIGHT_BRACE */
 cval.declEnum = createDeclEnum(cpvt[-3].token, cpvt[-1].enumList); zapTokens2(cpvt[-2].token, cpvt[0].token);
} break;

case Cr129: {   /* enum-list :  enum-list-collect */
 cval.enumList = finishEnumListCreation(cpvt[0].enumList);
} break;

case Cr130: {   /* enum-list :  enum-list-collect Y_COMMA */
 cval.enumList = finishEnumListCreation(cpvt[-1].enumList);
} break;

case Cr131: {   /* enum-list-collect :  enumerator */
 cval.enumList = createEnumList(cpvt[0].enumElem);
} break;

case Cr132: {   /* enum-list-collect :  enum-list-collect comma-and-enumerator */
 cval.enumList = addEnumList(cpvt[-1].enumList,  cpvt[0].enumElem);
} break;

case Cr133: {   /* enum-list-collect :  enum-list-collect error */
 recoverError();  cval.enumList = cpvt[-1].enumList;
} break;

case Cr134: {   /* comma-and-enumerator :  Y_COMMA enumerator */
 cval.enumElem = addEnumElemBegPunct(cpvt[0].enumElem); zapToken(cpvt[-1].token);
} break;

case Cr135: {   /* enumerator :  identifier */
 cval.enumElem = createEnumElem(cpvt[0].token, NULL, NULL);
} break;

case Cr136: {   /* enumerator :  identifier Y_EQUAL constant-expression */
 cval.enumElem = createEnumElem(cpvt[-2].token, cpvt[-1].token, cpvt[0].tree);
} break;

case Cr137: {   /* cv-qualifier :  Y_CONST */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
          cval.dinfo = createQualifierDeclInfo(STY_CONST,  cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr138: {   /* cv-qualifier :  Y_VOLATILE */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
          cval.dinfo = createQualifierDeclInfo(STY_VOLATILE,  cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr139: {   /* type-name :  decl-specs */
 cval.tree = createCTreeRoot(createDeclInfoLabel(cpvt[0].dinfo));
} break;

case Cr140: {   /* type-name :  decl-specs declarator-no-id */
 cval.tree = createCTreeRoot(createDeclInfoLabel(addDeclDclr(cpvt[-1].dinfo, cpvt[0].dclr)));
} break;

case Cr142: {   /* literal :  Y_NUMBER */
 cval.tree = createCTreeRoot(createTokenLabel(cpvt[0].token));
} break;

case Cr143: {   /* strings :  strings single-string */

            cval.tree = createCTree2(createConstr0Label(LABCT_STRINGS), cpvt[-1].tree, cpvt[0].tree);

} break;

case Cr144: {   /* strings :  single-string */
 cval.tree = cpvt[0].tree;
} break;

case Cr145: {   /* single-string :  Y_STRING */
 cval.tree = createCTreeRoot(createTokenLabel(cpvt[0].token));
} break;

case Cr148: {   /* declarator :  pragma-modifier declarator */
 cval.dclr = addDclrPragmaModifier(cpvt[0].dclr, cpvt[-1].token);
} break;

case Cr149: {   /* declarator :  mem-modifier declarator */
 cval.dclr = addDclrMemModifier(cpvt[0].dclr, cpvt[-1].token);
} break;

case Cr150: {   /* declarator :  ptr-modifier declarator */
 cval.dclr = addDclrPtrModifier(cpvt[0].dclr, cpvt[-1].dclrPtr);
} break;

case Cr151: {   /* declarator :  actual-declarator */
 cval.dclr = cpvt[0].dclr;
} break;

case Cr152: {   /* actual-declarator :  declarator-id */
 cval.dclr = createDclr(cpvt[0].token);
} break;

case Cr153: {   /* actual-declarator :  Y_LEFT_PAREN declarator Y_RIGHT_PAREN */
 cval.dclr = cpvt[-1].dclr; zapTokens2(cpvt[-2].token, cpvt[0].token);
} break;

case Cr154: {   /* actual-declarator :  actual-declarator Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET */
 cval.dclr = addDclrArray(cpvt[-3].dclr, cpvt[-2].token, cpvt[-1].tree, cpvt[0].token);
} break;

case Cr155: {   /* actual-declarator :  actual-declarator Y_LEFT_BRACKET Y_RIGHT_BRACKET */
 cval.dclr = addDclrArray(cpvt[-2].dclr, cpvt[-1].token, NULL, cpvt[0].token);
} break;

case Cr156: {   /* actual-declarator :  actual-declarator Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN */
 cval.dclr = addDclrFuncArgs(cpvt[-3].dclr, cpvt[-2].token, cpvt[-1].declList, cpvt[0].token);
} break;

case Cr157: {   /* declarator-no-id :  pragma-modifier declarator-no-id */
 cval.dclr = addDclrPragmaModifier(cpvt[0].dclr, cpvt[-1].token);
} break;

case Cr158: {   /* declarator-no-id :  pragma-modifier */
 cval.dclr = addDclrPragmaModifier(createDclr(NULL), cpvt[0].token);
} break;

case Cr159: {   /* declarator-no-id :  mem-modifier declarator-no-id */
 cval.dclr = addDclrMemModifier(cpvt[0].dclr, cpvt[-1].token);
} break;

case Cr160: {   /* declarator-no-id :  mem-modifier */
 cval.dclr = addDclrMemModifier(createDclr(NULL), cpvt[0].token);
} break;

case Cr161: {   /* declarator-no-id :  ptr-modifier declarator-no-id */
 cval.dclr = addDclrPtrModifier(cpvt[0].dclr, cpvt[-1].dclrPtr);
} break;

case Cr162: {   /* declarator-no-id :  ptr-modifier */
 cval.dclr = addDclrPtrModifier(createDclr(NULL), cpvt[0].dclrPtr);
} break;

case Cr163: {   /* declarator-no-id :  actual-declarator-no-id */
 cval.dclr = cpvt[0].dclr;
} break;

case Cr164: {   /* actual-declarator-no-id :  Y_LEFT_PAREN declarator-no-id Y_RIGHT_PAREN */
 cval.dclr = cpvt[-1].dclr; zapTokens2(cpvt[-2].token, cpvt[0].token);
} break;

case Cr165: {   /* actual-declarator-no-id :  actual-declarator-no-id Y_LEFT_BRACKET Y_RIGHT_BRACKET */
 cval.dclr = addDclrArray(cpvt[-2].dclr, cpvt[-1].token, NULL, cpvt[0].token);
} break;

case Cr166: {   /* actual-declarator-no-id :  actual-declarator-no-id Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET */
 cval.dclr = addDclrArray(cpvt[-3].dclr, cpvt[-2].token, cpvt[-1].tree, cpvt[0].token);
} break;

case Cr167: {   /* actual-declarator-no-id :  Y_LEFT_BRACKET Y_RIGHT_BRACKET */
 cval.dclr = addDclrArray(createDclr(NULL), cpvt[-1].token, NULL, cpvt[0].token);
} break;

case Cr168: {   /* actual-declarator-no-id :  Y_LEFT_BRACKET constant-expression Y_RIGHT_BRACKET */
 cval.dclr = addDclrArray(createDclr(NULL), cpvt[-2].token, cpvt[-1].tree, cpvt[0].token);
} break;

case Cr169: {   /* actual-declarator-no-id :  actual-declarator-no-id Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN */
 cval.dclr = addDclrFuncArgs(cpvt[-3].dclr, cpvt[-2].token, cpvt[-1].declList, cpvt[0].token);
} break;

case Cr170: {   /* actual-declarator-no-id :  Y_LEFT_PAREN abstract-args Y_RIGHT_PAREN */
 cval.dclr = addDclrFuncArgs(createDclr(NULL), cpvt[-2].token, cpvt[-1].declList, cpvt[0].token);
} break;

case Cr171: {   /* ptr-modifier :  Y_TIMES cv-qualifiers-opt */
 cval.dclrPtr = createDclrPtr(cpvt[-1].token, cpvt[0].flag);
} break;

case Cr181: {   /* cv-qualifiers-opt :  */
 cval.flag = STY_NULL;
} break;

case Cr182: {   /* cv-qualifiers-opt :  Y_CONST */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        cval.flag = STY_CONST; zapToken(cpvt[0].token);
} break;

case Cr183: {   /* cv-qualifiers-opt :  Y_CONST Y_VOLATILE */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        cval.flag = STY_CONST | STY_VOLATILE; zapTokens2(cpvt[-1].token, cpvt[0].token);
} break;

case Cr184: {   /* cv-qualifiers-opt :  Y_VOLATILE */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        cval.flag = STY_VOLATILE; zapToken(cpvt[0].token);
} break;

case Cr185: {   /* cv-qualifiers-opt :  Y_VOLATILE Y_CONST */
 reportError(CERR_CANT_CONVERT_QUALIFIER);
        cval.flag = STY_CONST | STY_VOLATILE; zapTokens2(cpvt[-1].token, cpvt[0].token);
} break;

case Cr186: {   /* abstract-args :  */
 cval.declList = createSLList();
} break;

case Cr188: {   /* abstract-args :  arg-decl-list Y_COMMA dot-dot-dot-decl */
 addDeclPunct(cpvt[0].dinfo, cpvt[-1].token); cval.declList = addDeclList(cpvt[-2].declList, cpvt[0].dinfo );
} break;

case Cr189: {   /* abstract-args :  dot-dot-dot-decl */
 cval.declList = createDeclList(cpvt[0].dinfo);
} break;

case Cr190: {   /* arg-decl-list :  arg-decl-elem */
 cval.declList = createDeclList(cpvt[0].dinfo);
} break;

case Cr191: {   /* arg-decl-list :  arg-decl-list Y_COMMA arg-decl-elem */
 addDeclPunct(cpvt[0].dinfo, cpvt[-1].token); cval.declList = addDeclList(cpvt[-2].declList, cpvt[0].dinfo );
} break;

case Cr192: {   /* arg-decl-elem :  decl-specs */
 cval.dinfo = cpvt[0].dinfo;
} break;

case Cr193: {   /* arg-decl-elem :  decl-specs declarator */
 cval.dinfo = addDeclDclr(cpvt[-1].dinfo, cpvt[0].dclr);
} break;

case Cr194: {   /* arg-decl-elem :  decl-specs declarator-no-id */
 cval.dinfo = addDeclDclr(cpvt[-1].dinfo, cpvt[0].dclr);
} break;

case Cr195: {   /* dot-dot-dot-decl :  Y_DOT_DOT_DOT */
 cval.dinfo = createDeclInfoSCALAR(STM_DOT_DOT_DOT, cpvt[0].token->pos);
          cpvt[0].token->pos = NULL; zapToken(cpvt[0].token);
} break;

case Cr196: {   /* identifier :  Y_ID */
 cval.token = cpvt[0].token;
} break;
        case CrACCEPT:
                CACCEPT;
        case CrERROR:
                goto cError;
#ifdef CLR2
        case CrLR2:
#ifndef CSYNC
                CREAD;
#endif
                cj = 0;
                while(clr2[cj] >= 0) {
                        if(clr2[cj] == cstate && clr2[cj+1] == cchar
                        && clook(cs+1,cps,cstate,cchar,c2lex(),clr2[cj+2]))
                                        break;
                        cj += 3;
                }
                if(clr2[cj] < 0)
                        goto cError;
                if(clr2[cj+2] < 0) {
                        cstate = ~ clr2[cj+2];
                        goto cStack;
                }
                ci = clr2[cj+2];
                goto cReduce;
#endif
        }

        /*
         *      Look up next state in goto table.
         */

        cp = &cgo[cpgo[ci]];
        cq = cp++;
        ci = *cps;
#if     0&&defined(__TURBOC__) && __SMALL__
        /* THIS ONLY WORKS ON TURBO C 1.5 !!! */
        /* ci is in di, cp is in si */
L02:
        asm lodsw               /* ax = *cp++; */
        asm cmp ci, ax
        asm jl L02
#else
        while (ci < *cp++)
                ;
#endif
        cstate = ~(ci == *--cp? CQCP: *cq);
        goto cStack;

cerrlabel:      ;               /* come here from CERROR        */
/*
#pragma used cerrlabel
 */
        cerrflag = 1;
        cps--, cpv--;

cError:
        switch (cerrflag) {

        case 0:         /* new error */
                cnerrs++;
                ci = cchar;
                cerror("Syntax error");
                if (ci != cchar) {
                        /* user has changed the current token */
                        /* try again */
                        cerrflag++;     /* avoid loops */
                        goto cEncore;
                }

        case 1:         /* partially recovered */
        case 2:
                cerrflag = 3;   /* need 3 valid shifts to recover */

                /*
                 *      Pop states, looking for a
                 *      shift on `error'.
                 */

                for ( ; cps > cs; cps--, cpv--) {
                        if (*cps >= sizeof cpact/sizeof cpact[0])
                                continue;
                        cp = &cact[cpact[*cps]];
                        cq = cp;
                        do
                                ;
                        while (CERRCODE < *cp++);
                        if (CERRCODE == cp[-1]) {
                                cstate = ~CQCP;
                                goto cStack;
                        }

                        /* no shift in this state */
#if CDEBUG
                        if (cdebug && cps > cs+1)
                                printf("Error recovery pops state %d (%d), uncovers %d (%d)\n",
                                        csmap[cps[0]], cps[0],
                                        csmap[cps[-1]], cps[-1]);
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

                if (cchar == 0)  /* but not EOF */
                        break;
#if CDEBUG
                if (cdebug)
                        printf("Error recovery discards %s (%d), ",
                                cptok(cchar), cchar);
#endif
                cclearin;
                goto cEncore;   /* try again in same state */
        }
        CABORT;

#ifdef CALLOC
cReturn:
        clval = save_clval;
        cval = save_cval;
        cpvt = save_cpvt;
        cchar = save_cchar;
        cerrflag = save_cerrflag;
        cnerrs = save_cnerrs;
        free((char *)cs);
        free((char *)cv);
        return(retval);
#endif
}

#ifdef CLR2
clook(s,rsp,state,c1,c2,i)
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
#if CDEBUG
        if(cdebug) {
                printf("LR2 state %d (%d) char %s (%d) lookahead %s (%d)",
                        csmap[state],state,cptok(c1),c1,cptok(c2),c2);
                if(i > 0)
                        printf("reduce %d (%d)\n", crmap[i], i);
                else
                        printf("shift %d (%d)\n", csmap[i], i);
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
        if(++st >= &s[CSSIZE]) {
                cerror("Parser Stack Overflow");
                return 0;
        }
        *st = state;
        if(state >= sizeof cpact/sizeof cpact[0])
                i = state- CDELTA;
        else {
                p = &cact[cpact[state]];
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
                if(state >= sizeof cdef/sizeof cdef[0])
                        return 0
                if((i = cdef[state]) < 0) {
                        p = &cex[~i];
                        while((i = *p) >= 0 && i != c1)
                                p += 2;
                        i = p[1];
                }
        }
  reduce:
        j = crlen[i];
        if(st-sb >= j)
                st -= j;
        else {
                rsp -= j+st-sb;
                st = sb;
        }
        switch(i) {
        case CrERROR:
                return 0;
        case CrACCEPT:
                return 1;
        case CrLR2:
                j = 0;
                while(clr2[j] >= 0) {
                        if(clr2[j] == state && clr2[j+1] == c1)
                                if((i = clr2[j+2]) < 0)
                                        goto shift;
                                else
                                        goto reduce;
                }
                return 0;
        }
        p = &cgo[cpgo[i]];
        q = p++;
        i = st==sb ? *rsp : *st;
        while(i < *p++);
        state = ~(i == *--p? q[q-p]: *q);
        goto stack;
}
#endif

#if CDEBUG

/*
 *      Print a token legibly.
 *      This won't work if you roll your own token numbers,
 *      but I've found it useful.
 */
char *
cptok(i)
{
        static char     buf[10];

        if (i >= CERRCODE)
                return cstoken[i-CERRCODE];
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
#if CDEBUG
char * cstoken[] = {
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
char * csvar[] = {
        "$accept",
        "decl",
        "init-declarator",
        "initializer",
        "init-declarator-list",
        "parse-interface",
        "external-definition",
        "translation-unit",
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
short crmap[] = {
         197,  198,  199,    3,    8,   27,   48,   49,   51,   52,
          54,   55,   56,   57,   59,   60,   62,   64,   66,   68,
          70,   77,   78,   79,  111,  127,  129,  130,  135,  139,
         151,  158,  160,  162,  163,  181,  182,  184,  186,  192,
         196,  195,  194,  193,  191,  190,  189,  188,  185,  183,
         171,  170,  169,  168,  167,  166,  165,  164,  161,  159,
         157,  156,  155,  154,  153,  152,  150,  149,  148,  145,
         144,  143,  142,  140,  138,  137,  136,  134,  133,  132,
         131,  128,  126,  125,  123,  122,  121,  120,  119,  118,
         117,  116,  110,  109,  108,  107,  106,  105,  104,  103,
         102,  101,  100,   99,   98,   97,   96,   95,   94,   93,
          90,   89,   88,   83,   75,   72,   46,   45,   44,   42,
          40,   39,   38,   37,   36,   35,   34,   33,   32,   30,
          26,   25,   24,   23,   21,   20,   17,   15,   14,   13,
          12,   11,   10,    9,    7,    6,    5,    4,    2,    1,
          18,   19,   22,   29,   41,   43,   73,   74,   76,   82,
          84,   85,   86,   87,   91,   92,  112,  113,  114,  115,
         146,  147,  172,  173,  174,  175,  176,  177,  178,  179,
         180,  187,  141,  124,   81,   80,   71,   69,   67,   65,
          63,   61,   58,   53,   50,   47,   31,   28,   16,    0
};
short csmap[] = {
          28,   29,   31,   34,   37,   40,   45,   49,   51,   61,
          64,   70,   78,   80,   89,   90,   91,   94,  107,  110,
         121,  124,  131,  133,  134,  135,  136,  137,  138,  139,
         140,  141,  142,  153,  172,  173,  174,  175,  176,  205,
         207,  212,  217,  221,  228,  229,  230,  231,  232,  233,
         234,  235,  236,  237,  238,  239,  240,  241,  242,  250,
         253,  254,  255,  256,  270,  268,  264,  263,  259,  258,
         252,  251,  246,  227,  220,  219,  218,  215,  210,  209,
         206,  204,  203,  201,  199,  198,  197,  196,  195,  194,
         193,  192,  191,  190,  189,  188,  187,  186,  185,  184,
         183,  182,  181,  177,  171,  167,  166,  162,  156,  152,
         151,  127,  125,  112,  109,  104,  102,  100,   99,   95,
          93,   85,   83,   82,   81,   74,   72,   67,   66,   65,
          62,   41,   39,   35,   30,    6,    3,    0,   38,  120,
         169,  170,  225,  122,  123,  226,  117,  118,   92,  266,
         275,  262,  216,  276,  265,  267,  222,  223,  224,  178,
         126,  179,  119,   63,   96,   97,   98,  128,  129,  180,
         130,  257,    1,    2,  163,  164,  105,  106,   79,  108,
         165,  213,  214,  111,  168,  113,  114,  115,   84,  116,
          44,   86,    7,    8,    9,   10,   11,   12,   13,   14,
          15,   16,   17,   46,   18,   19,   87,   88,   20,   21,
          22,   50,  274,  277,  243,  244,  245,  273,  145,  146,
         147,  148,  149,  150,  269,  200,  202,  278,  247,  248,
         271,  272,  155,  260,  160,  161,   71,   75,  101,   73,
         211,  261,  103,   32,   76,   33,   77,   36,  159,  157,
         154,  249,  144,  143,  158,  208,  132,   27,   26,   25,
          24,   23,   48,   47,   43,   42,    5,    4,   69,   68,
          60,   59,   58,   57,   56,   55,   54,   53,   52
};
int cntoken = 115, cnvar = 69, cnstate = 279, cnrule = 200;
#endif

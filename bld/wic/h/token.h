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


#include <limits.h>

#define MAX_TOKEN_SIZE          501 /* max tok size is 500 + terminating 0 */
#define MAX_PRINT_TOKEN_SIZE    (MAX_TOKEN_SIZE+10)
#define MAX_INCLUDE_NEST        20  /* #include nesting level */
#define TAB_SIZE                8

/* Tokens for used by parsers.  NOTE: If any values are modified or added,
   make sure the same is done for cparse.y and preparse.y
   NOTE: The biggest Y_xxx must not exceed 16000. */

#define BEGIN_Y_OTHER           300
#define Y_EOF                   300  /* MKS yacc will not allow it to be 0*/
#define Y_EXCLAMATION           301
#define Y_NE                    302
#define Y_POUND                 303
#define Y_POUND_POUND           304
#define Y_AND                   305
#define Y_AND_AND               306
#define Y_AND_EQUAL             307
#define Y_LEFT_PAREN            308
#define Y_RIGHT_PAREN           309
#define Y_TIMES                 310
#define Y_TIMES_EQUAL           311
#define Y_PLUS                  312
#define Y_PLUS_PLUS             313
#define Y_PLUS_EQUAL            314
#define Y_COMMA                 315
#define Y_MINUS                 316
#define Y_MINUS_MINUS           317
#define Y_MINUS_EQUAL           318
#define Y_ARROW                 319
#define Y_DOT                   320
#define Y_DOT_DOT_DOT           321
#define Y_DIVIDE                322
#define Y_DIVIDE_EQUAL          323
#define Y_COLON                 324
#define Y_SEG_OP                325
#define Y_SEMICOLON             326
#define Y_LT                    327
#define Y_LSHIFT                328
#define Y_LSHIFT_EQUAL          329
#define Y_LE                    330
#define Y_EQUAL                 331
#define Y_EQ                    332
#define Y_GT                    333
#define Y_GE                    334
#define Y_RSHIFT                335
#define Y_RSHIFT_EQUAL          336
#define Y_QUESTION              337
#define Y_LEFT_BRACKET          338
#define Y_RIGHT_BRACKET         339
#define Y_XOR                   340
#define Y_XOR_EQUAL             341
#define Y___BASED               342
#define Y___CDECL               343
#define Y___EXPORT              344
#define Y___FAR                 345
#define Y___FAR16               346
#define Y___FORTRAN             347
#define Y___HUGE                348
#define Y___INTERRUPT           349
#define Y___LOADDS              350
#define Y___NEAR                351
#define Y___PASCAL              352
#define Y___PRAGMA              353
#define Y___SAVEREGS            354
#define Y___SEGMENT             355
#define Y___SEGNAME             356
#define Y___SELF                357
#define Y___STDCALL             358
#define Y__PACKED               359
#define Y__SEG16                360
#define Y__SYSCALL              361
#define Y_AUTO                  362
#define Y_CHAR                  363
#define Y_CONST                 364
#define Y_DOUBLE                365
#define Y_ELSE                  366
#define Y_ENUM                  367
#define Y_EXTERN                368
#define Y_FLOAT                 369
#define Y_INT                   370
#define Y_LONG                  371
#define Y_REGISTER              372
#define Y_SHORT                 373
#define Y_SIGNED                374
#define Y_SIZEOF                375
#define Y_STATIC                376
#define Y_STRUCT                377
#define Y_TYPEDEF               378
#define Y_UNION                 379
#define Y_UNSIGNED              380
#define Y_VOID                  381
#define Y_VOLATILE              382
#define Y_LEFT_BRACE            383
#define Y_OR                    384
#define Y_OR_EQUAL              385
#define Y_OR_OR                 386
#define Y_RIGHT_BRACE           387
#define Y_TILDE                 388
#define Y_ID                    389
#define Y_STRING                390
#define Y_INCLUDE_FILE_NAME     391
#define Y_TYPEDEF_NAME          392
#define Y_NUMBER                393
#define Y_PERCENT               394
#define Y_PERCENT_EQUAL         395
#define Y_DEFINED               396
#define END_Y_OTHER             396

#define BEGIN_Y_PREPROCESSOR    500
#define Y_PRE_COMMENT           500
#define Y_PRE_NULL              501  /* # followed by newline */
#define Y_PRE_NEWLINE           502
#define Y_PRE_DEFINE            503
#define Y_PRE_ELIF              504
#define Y_PRE_ELSE              505
#define Y_PRE_ENDIF             506
#define Y_PRE_ERROR             507
#define Y_PRE_IF                508
#define Y_PRE_IFDEF             509
#define Y_PRE_IFNDEF            510
#define Y_PRE_INCLUDE           511
#define Y_PRE_LINE              512
#define Y_PRE_PRAGMA            513
#define Y_PRE_UNDEF             514
#define Y_PRE_SPECIAL_LEFT_PAREN 515
#define END_Y_PREPROCESSOR      515

typedef enum {
    CONSTT_CHAR_CONST,
    CONSTT_INT_CONST,
    CONSTT_UINT_CONST,
    CONSTT_LONG_CONST,
    CONSTT_ULONG_CONST,
    CONSTT_FLOAT_CONST,
    CONSTT_DOUBLE_CONST,
    CONSTT_LDOUBLE_CONST,   /* Long double const */
    CONSTT_STRING_CONST,
    CONSTT_MAX
} ConstType;

typedef enum {
    TT_PREPROCESSOR,
    TT_OTHER
} TokenType;

/* Tokenizer states */
typedef enum {
    TS_START,
    TS_COMMENT
} TStates;

#define MAX_LINE_NUM    SHRT_MAX
#define MAX_COL_NUM     SHRT_MAX
typedef struct {
    char *fileName;
    int8 fileLevel;
    int16 lineNum;
    int16  colNum;
    uint8  linesBefore;
    uint8  spacesBefore;

    long orderLineNum;

} TokPos, *pTokPos;

typedef enum {
    RADT_DECIMAL,
    RADT_HEX,
    RADT_OCTAL,
    RADT_MAX
} RadixType;


typedef struct {
    uint16 code;
    union {
        struct {      // General info
            char* string;
            pDeclInfo pTypeDecl;  // This only points to a symbol table entry
        };
        struct {
            ConstType type: 6;
            RadixType radix: 2;
            union {
                unsigned long lIntConst;
                long double lDoubleConst;
                long double data;
            } repr;
        } constant;
        struct {
            char *s;
            int strLen;
        } s;  // String representation
    } repr;
} *pTokData;

typedef struct {
    pTokData data;
    pTokPos pos;
} *pToken;

char* initTokenizer(char *fname);
void zapTokenizer(void);
char* pushTokFile(char *fname);
int popTokFile(void);
WicErrors getNextToken(pToken tok);
TokenType getTokDataType(pTokData tokData);

pToken createToken(pTokData data, pTokPos pos);
pTokPos createTokPos(void);
pTokData createTokData(void);
pToken createEnumNumToken(unsigned long num);
pToken createPLUSToken(void);
pToken createEQUALToken(void);
pToken createIDTokenAfter(char *str, pTokPos newPos);
pToken createCommentToken(char *str, pTokPos pos);
pTokPos combine2TokPos(pTokPos p1, pTokPos p2);
pToken combine2Token(pToken t1, pToken t2);
void setTokPos(pTokPos pos, char *fileName, int8 fileLevel, int16 lineNum,
    int16  colNum, uint8  linesBefore, uint8  spacesBefore, long orderLineNum);

char *getTokDataIdName(pTokData data);
char *getTokenIdName(pToken tok);

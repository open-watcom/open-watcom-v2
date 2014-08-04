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
#include <ctype.h>
#include <string.h>

#include "wic.h"

typedef struct {
    int8 linesBefore;
    int32 lineNum;
    int16 colNum;
    char *fileName;
    FILE *file;
    long int filePos;
    TStates state;       /* tokenizer state */
    char  nextChar;      /* 2-char lookahead is only needed to detect '\' */
    char  _nextNextChar; /* followed by newline and get rid of it. */
                         /* _nextNextChar may only be used in getNextChar*/
    int tokNumAfterNewLine; /* Used for #preprocessor directives */
    int prevTokLineNum;
} TokFilePos;

static TokFilePos       tokF[MAX_INCLUDE_NEST];
static int              currTokF = -1;
static int              currTokLen;
static char             currTok[MAX_TOKEN_SIZE];
static int              currLineIsInclude;
static long             orderLineNum = 1;

#define TOK_FILE        (tokF[currTokF].file)
#define CURR_FILE_POS   (tokF[currTokF].filePos)
#define STATE           (tokF[currTokF].state)
#define NEXT_CHAR       (tokF[currTokF].nextChar)
#define _NEXT_NEXT_CHAR (tokF[currTokF]._nextNextChar)
#define LINE_NUM        (tokF[currTokF].lineNum)
#define COL_NUM         (tokF[currTokF].colNum)
#define LINES_BEFORE    (tokF[currTokF].linesBefore)
#define TOK_NUM_AFTER_NEW_LINE    (tokF[currTokF].tokNumAfterNewLine)
#define TOK_FILE_NAME   (tokF[currTokF].fileName)
#define PREV_TOK_LINE_NUM  (tokF[currTokF].prevTokLineNum)

static int getTabSize(int colNum) {
//    assert(colNum > 0);
    return  TAB_SIZE  -  ((colNum-1) % TAB_SIZE);
}

static void getNextChar(void)
{
    static unsigned long counter = 0;
    enum { COUNTER_STEP = 1024 };

    if (counter++ % COUNTER_STEP == 0) {
        dribble();
    }

    if ((char) NEXT_CHAR == (char) EOF){
        printf("INTERNAL ERROR in getNextChar: Attempted to read beyond EOF!\n");
        return;
    }

    if (NEXT_CHAR == '\t'){
        COL_NUM += getTabSize(COL_NUM);
    } else if (NEXT_CHAR != '\n') {
        COL_NUM++;
    } else {
        g_totalNumLines++;
        LINE_NUM++;
        orderLineNum++;
        COL_NUM = 0;
        TOK_NUM_AFTER_NEW_LINE = 0;
    }

    NEXT_CHAR = _NEXT_NEXT_CHAR;
    if ((char) NEXT_CHAR != (char) EOF){
        _NEXT_NEXT_CHAR = fgetc(TOK_FILE);
        if (STATE != TS_COMMENT) {
            while (_NEXT_NEXT_CHAR == '\n' && NEXT_CHAR == '\\'){
                g_totalNumLines++;
                NEXT_CHAR = fgetc(TOK_FILE);
                _NEXT_NEXT_CHAR = fgetc(TOK_FILE);
                LINE_NUM++;
                COL_NUM = 0;
            }
        }
        if ((char) _NEXT_NEXT_CHAR == (char) EOF && NEXT_CHAR == '\\'){
            NEXT_CHAR = (char) EOF;
        }
    }
}

void pushChar(char ch)
{
    if (currTokLen < MAX_TOKEN_SIZE - 1) {
        currTok[currTokLen++] = ch;
        currTok[currTokLen] = 0;
    }
}

void pushGetNextChar(void)
{
    pushChar(NEXT_CHAR);
    getNextChar();
}

void popChars(int num)  /* Delete last 'num' chars from currTok */
{
    currTokLen -= num;
    if (currTokLen < 0)
    {
        assert(0);
        currTokLen = 0;
    }
    currTok[currTokLen] = 0;
}

static FILE *_openIncludeFile(char *fname, char **newAllocName) {
    FILE *file = NULL;
    char fullName[_MAX_PATH];
    pFDReadInd dir = NULL;
    char *fullNameCopy;

    strcpy(fullName, fname);
    rewindCurrSLListPos(g_opt.incPathList);
    do {
        setDefaultExt(fullName, "h");
        if (fileReadable(fullName)) {
            file = wicFopen(fullName, "r");
            if (file != NULL) {
                pFDReadInd newElem;
                fullNameCopy = wicStrdup(fullName);
                newElem = createFDReadInd(fullNameCopy,
                            dir == NULL ? 0 : dir->readOnly);
                if (!addHTableElem(g_fileTable, newElem)) {
                    reportError(ERR_FILE_WAS_INCLUDED, fullNameCopy);
                    wicFclose(file);
                    zapFDReadInd(newElem);
                    file = NULL;
                }
                break;
            }
        }
        if (!getCurrSLListPosElem(g_opt.incPathList, &dir)) {
            strcpy(fullName, fname);
            setDefaultExt(fullName, "h");
            reportError(ERR_INC_FILE_NOT_FOUND, fullName);
            break;
        }
        sprintf(fullName, "%s\\%s", dir->name, fname);
        incCurrSLListPos(g_opt.incPathList);
    } while (1);

    if (file == NULL) {
        *newAllocName = NULL;
    } else {
        *newAllocName = fullNameCopy;
    }
    return file;
}

static void restoreTokFile(void) {
    currTokF--;
    if (currTokF >= 0) {
        g_currFileName = TOK_FILE_NAME;
        g_currLineNum = LINE_NUM;
        g_currColNum = COL_NUM;
        TOK_FILE = wicFopen(TOK_FILE_NAME, "r");
        if (TOK_FILE == NULL) {
            reportError(ERR_OPEN_FILE, TOK_FILE_NAME, strerror(errno));
            reportError(FATAL_DIE);
        }
        if (fseek(TOK_FILE, CURR_FILE_POS, SEEK_SET) != 0) {
            reportError(ERR_COULD_NOT_SEEK, TOK_FILE_NAME, strerror(errno));
            reportError(FATAL_DIE);
        }
    }
}

char* pushTokFile(char *fname)
{
    if (currTokF >= 0) {
        if (TOK_FILE != NULL) {
            CURR_FILE_POS = ftell(TOK_FILE);
            wicFclose(TOK_FILE);
        }
    }
    currTokF++;
    assert(currLineIsInclude == 0);
    currLineIsInclude = 0;

    if (currTokF >= MAX_INCLUDE_NEST) {
        currTokF--;
        reportError(ERR_NEST_INCLUDE);
        return NULL;
    } else {
        assert(fname != NULL);
        TOK_FILE = _openIncludeFile(fname, &TOK_FILE_NAME);
        if (TOK_FILE == NULL) {
            restoreTokFile();
            return NULL;
        } else {
            debugOut("Push file: %s\n", fname);
            g_currFileName = TOK_FILE_NAME;
            STATE = TS_START;
            LINE_NUM = 1;
            PREV_TOK_LINE_NUM = 1;
            COL_NUM = 0;
            g_currLineNum = LINE_NUM;
            g_currColNum = COL_NUM;
            NEXT_CHAR = 0;
            _NEXT_NEXT_CHAR = fgetc(TOK_FILE);
            TOK_NUM_AFTER_NEW_LINE = 0;
            getNextChar();
        }
    }
    return TOK_FILE_NAME;
}

int popTokFile(void) {
    assert(currTokF >= 0);
    wicFclose(TOK_FILE);
    if (currTokF == 0) {
        g_currFileName = NULL;
        return 0;
    } else {
        debugOut("Pop  file: %s\n", TOK_FILE_NAME);
        restoreTokFile();
        return 1;
    }
}

char* initTokenizer(char *fname)
{
    char *retName;
    currLineIsInclude = 0;
    orderLineNum = 1;
    currTokF = -1;
    g_fileTable = createHTable(32,  FDReadIndHashFunc, FDReadIndCmpFunc);
    retName = pushTokFile(fname);
    if (retName == NULL) {
        wicExit(-1);
        return  NULL; // avoid warning
    }
    return retName;
}

void zapTokenizer(void) {
    zapHTable(g_fileTable, zapFDReadInd);
}

static int skipBlank(void)
{
    int count = 0;

    while (NEXT_CHAR == ' ' || NEXT_CHAR == '\t') {
        if (NEXT_CHAR == ' ') {
            count++;
        } else {
            count += getTabSize(COL_NUM);
        }
        getNextChar();
    }

    return count;
}

int scanId(void)  // Returns 0 iff 1st char is 'L' and 2nd is '"'
{
    char firstChar = NEXT_CHAR;
    pushGetNextChar();
    if ((NEXT_CHAR == '"' || NEXT_CHAR == '\'') && firstChar == 'L') {
        return 0;
    }
    while (isalnum(NEXT_CHAR) || NEXT_CHAR == '_') {
        pushGetNextChar();
    }
    return 1;
}

static WicErrors scanComment(pTokData tokData)
{
    while (1) {
        if (NEXT_CHAR == '*') {
            pushGetNextChar();
            if (NEXT_CHAR == '/') {
                STATE = TS_START;
                getNextChar();
                popChars(1);
                goto NormalReturn;
            }
        } else if (NEXT_CHAR == '\n') {
            getNextChar();
            goto NormalReturn;
        } else if ((char) NEXT_CHAR == (char) EOF) {
            STATE = TS_START;
            return RERR_EOF_IN_COMMENT;
        } else {
            pushGetNextChar();
        }
    }

    NormalReturn:
        tokData->code = Y_PRE_COMMENT;
        tokData->repr.string = registerString(wicStrdup(currTok), FREE_STRING);
        return ERR_NONE;
}

static WicErrors scanCPlusPlusComment(pTokData tokData)
{
    char prevChar = 0;
    while (1) {
        if (NEXT_CHAR == '\n') {
            tokData->code = Y_PRE_COMMENT;
            tokData->repr.string = registerString(wicStrdup(currTok),
                                    FREE_STRING);
            if (prevChar != '\\') {
                STATE = TS_START;
            }
            return ERR_NONE;
        } else {
            prevChar = NEXT_CHAR;
            pushGetNextChar();
        }
    }
}

static char scanESCChar(void) {
    int i;
    char n;

    if( NEXT_CHAR >= '0'  &&  NEXT_CHAR <= '7' ) { /* get octal escape sequence */
        n = 0;
        i = 3;
        while( NEXT_CHAR >= '0'  &&  NEXT_CHAR <= '7' ) {
            n = (n << 3) + NEXT_CHAR - '0';
            --i;
            getNextChar();
            if( i == 0 ) break;
        }
    } else if( NEXT_CHAR == 'x' ) {         /* get hex escape sequence */
        getNextChar();
        n = 0;
        i = 0;
        while(isxdigit(NEXT_CHAR)) {
            n = (n << 4) + NEXT_CHAR - '0';
            getNextChar();
            i++;
        }
        if (i == 0) {
            n = 'x';
        }
    } else {
        switch( NEXT_CHAR ) {
        case 'a':
            n = '\a';
            break;
        case 'b':
            n = '\b';
            break;
        case 'f':
            n = '\f';
            break;
        case 'n':
            n = '\n';
            break;
        case 'r':
            n = '\r';
            break;
        case 't':
            n = '\t';
            break;
        case 'v':
            n = '\v';
            break;
        default:
            n = NEXT_CHAR;
            break;
        }
        getNextChar();
    }
    return( n );
}

static WicErrors scanStr(pTokData tokData)
{
    char charVal;
    WicErrors retVal = ERR_NONE;

    for( ;; ) {
        if (NEXT_CHAR == '\n' || NEXT_CHAR == (char) EOF) {
            retVal = RERR_INV_STRING_CONST;
            break;
        }
        if( NEXT_CHAR == '\"' ) {
            getNextChar();
            break;
        }

        if( NEXT_CHAR == '\\' ) {
            getNextChar();
            charVal = scanESCChar();
        } else {
            charVal = NEXT_CHAR;
            getNextChar();
        }
        pushChar(charVal);
    }

    if (retVal == ERR_NONE) {
        tokData->code = Y_STRING;
        tokData->repr.s.s = wicMalloc(currTokLen);
        memcpy(tokData->repr.s.s, currTok, currTokLen);
        tokData->repr.s.strLen = currTokLen;
    }
    return( retVal );
}

static WicErrors scanChar(pTokData tokData)
{
    register int i;
    unsigned long value;
    char charVal;
    WicErrors retVal = ERR_NONE;

    i = 0;
    value = 0;
    for( ;; ) {
        if( i > 4 ) {
            retVal = RERR_INV_CHAR_CONST;
            break;
        }
        if (NEXT_CHAR == '\n' || NEXT_CHAR == (char) EOF) {
            retVal = RERR_INV_CHAR_CONST;
            break;
        }
        if( NEXT_CHAR == '\'' ) {
            if (i != 0)
            {
                getNextChar();
                break;
            }
        }

        if( NEXT_CHAR == '\\' ) {
            getNextChar();
            charVal = scanESCChar();
        } else {
            charVal = NEXT_CHAR;
            getNextChar();
        }
        ++i;
        value = (value << 8) + charVal;
    }

    if( g_opt.signedChar ) {
        if( (value & 0xFFFFFF00) == 0 ) {
            if( value & 0x80 ) {            /* if sign bit is on */
                value |= 0xFFFFFF00;        /* - sign extend it */
            }
        }
    }

    if (retVal == ERR_NONE) {
        tokData->code = Y_NUMBER;
        tokData->repr.constant.type = CONSTT_CHAR_CONST;
        tokData->repr.constant.repr.lIntConst = value;
    }
    return( retVal );
}

static WicErrors scanIncludeFileName(pTokData tokData, char termChar) {
    WicErrors retVal = ERR_NONE;

    for( ;; ) {
        if (NEXT_CHAR == '\n' || NEXT_CHAR == (char) EOF) {
            retVal = RERR_INV_INCLUDE_FILE_NAME;
            break;
        }
        if( NEXT_CHAR == termChar ) {
            getNextChar();
            break;
        }

        pushGetNextChar();
    }

    if (retVal == ERR_NONE) {
        tokData->code = Y_INCLUDE_FILE_NAME;
        tokData->repr.string = registerString(wicStrdup(currTok), FREE_STRING);
    }
    return( retVal );
}

static WicErrors convStr2Const(pTokData tok)
{
    int n;

    if (tok->repr.constant.type == CONSTT_INT_CONST ||
        tok->repr.constant.type == CONSTT_LONG_CONST ||
        tok->repr.constant.type == CONSTT_UINT_CONST ||
        tok->repr.constant.type == CONSTT_ULONG_CONST) {
        n = sscanf(currTok, "%li", &tok->repr.constant.repr.lIntConst);

        if (tok->repr.constant.type == CONSTT_UINT_CONST &&
            tok->repr.constant.repr.lIntConst > 0xFFFF) {
                tok->repr.constant.type = CONSTT_ULONG_CONST;
        }
        if (tok->repr.constant.type == CONSTT_INT_CONST &&
            tok->repr.constant.repr.lIntConst > 0x7FFF) {
                tok->repr.constant.type = CONSTT_UINT_CONST;
        }
        if (tok->repr.constant.type == CONSTT_UINT_CONST &&
            tok->repr.constant.repr.lIntConst > 0xFFFF) {
                tok->repr.constant.type = CONSTT_LONG_CONST;
        }
        if (tok->repr.constant.repr.lIntConst > 0x7FFFFFFFul) {
            tok->repr.constant.type = CONSTT_ULONG_CONST;
        }
    } else {
        n = sscanf(currTok, "%Lf", &tok->repr.constant.repr.lIntConst);
    }
    if (n != 1) {
        reportError(FATAL_INTERNAL, "in convStr2Const: could not convert");
        return FATAL_INTERNAL;
    } else {
        tok->code = Y_NUMBER;
        return ERR_NONE;
    }
}

static int pushFloatDotExp(pTokData tokData, int dotWasPushed)
{
    tokData->repr.constant.radix = RADT_DECIMAL;
    if (NEXT_CHAR == '.') {
        pushGetNextChar();
        dotWasPushed = 1;
    }

    if (dotWasPushed) {
        while( NEXT_CHAR >= '0' && NEXT_CHAR <= '9' ) {
            pushGetNextChar();
        }
    }

    if( NEXT_CHAR == 'e'  ||  NEXT_CHAR == 'E' ) {
        pushGetNextChar();
        if( NEXT_CHAR == '+'  ||  NEXT_CHAR == '-' ) {
            pushGetNextChar();
        }
        if( NEXT_CHAR < '0'  ||  NEXT_CHAR > '9' ) {
            return 0;
        }
        while( NEXT_CHAR >= '0'  &&  NEXT_CHAR <= '9' ) {
            pushGetNextChar();
        }
    }

    if( NEXT_CHAR == 'f' || NEXT_CHAR == 'F' ) {
        getNextChar();
        tokData->repr.constant.type = CONSTT_FLOAT_CONST;
    } else if( NEXT_CHAR == 'l' || NEXT_CHAR == 'L' ) {
        getNextChar();
        tokData->repr.constant.type = CONSTT_LDOUBLE_CONST;
    } else {
        tokData->repr.constant.type = CONSTT_DOUBLE_CONST;
    }

    return 1;
}

static WicErrors scanNum(pTokData tokData)
{
    tokData->repr.constant.type = CONSTT_INT_CONST;
    tokData->repr.constant.radix = RADT_DECIMAL;
    if( NEXT_CHAR == '0' ) {
        pushGetNextChar();
        if( NEXT_CHAR == 'x'  ||  NEXT_CHAR == 'X' ) {
            tokData->repr.constant.radix = RADT_HEX;
            pushGetNextChar();
            while(isxdigit(NEXT_CHAR)) {
                pushGetNextChar();
            }
        } else {                    /* scan octal or float number */
            if (NEXT_CHAR >= '0' && NEXT_CHAR <= '7') {
                tokData->repr.constant.radix = RADT_OCTAL;
            }
            while (NEXT_CHAR >= '0'  &&  NEXT_CHAR <= '7') {
                pushGetNextChar();
            }
            if( isdigit(NEXT_CHAR) ||
                NEXT_CHAR == '.' || NEXT_CHAR == 'e' || NEXT_CHAR == 'E' ) {
                while (isdigit(NEXT_CHAR)) {
                    pushGetNextChar();
                }
                if (NEXT_CHAR == '.' || NEXT_CHAR == 'e' ||
                    NEXT_CHAR == 'E' ) {
                    if (!pushFloatDotExp(tokData, 0)){
                        return RERR_INV_FLOAT_CONST;
                    }
                } else {
                    return RERR_INV_INT_CONST;
                }
                goto Convert;
            }
        }
    } else {                /* scan decimal number */
        while( NEXT_CHAR >= '0'  &&  NEXT_CHAR <= '9' ) {
            pushGetNextChar();
        }
        if( NEXT_CHAR == '.' || NEXT_CHAR == 'e' || NEXT_CHAR == 'E' ) {
            if (!pushFloatDotExp(tokData, 0)){
                return RERR_INV_FLOAT_CONST;
            }
            goto Convert;
        }
    }
    if( NEXT_CHAR == 'L'  ||  NEXT_CHAR == 'l' ) {
        tokData->repr.constant.type = CONSTT_LONG_CONST;
        getNextChar();
        if( NEXT_CHAR == 'u' || NEXT_CHAR == 'U' ) {
            getNextChar();
            tokData->repr.constant.type = CONSTT_ULONG_CONST;
        }
    } else if( NEXT_CHAR == 'u' || NEXT_CHAR == 'U' ) {
        getNextChar();
        if( NEXT_CHAR == 'l' || NEXT_CHAR == 'L' ) {
            getNextChar();
            tokData->repr.constant.type = CONSTT_ULONG_CONST;
        } else {
            tokData->repr.constant.type = CONSTT_UINT_CONST;
        }
    }

    Convert:
        return convStr2Const(tokData);
}

WicErrors getNextToken(pToken tok)
{
    pTokTab tokTabPtr;
    WicErrors retVal = ERR_NONE;
    static long tokAfterDefine = 2;  // used to flag '(' in #define x( as a
                                  // special parentheses
    int temp;

    assert(currTokF >= 0);

    currTokLen = 0;
    currTok[currTokLen] = 0;
    TOK_NUM_AFTER_NEW_LINE++;   /* Used for #preprocessor directives */
    tokAfterDefine++;   /* Used for #preprocessor directives */

    g_currLineNum = LINE_NUM;
    g_currColNum = COL_NUM;

    /* When getNextToken gets called, STATE may be one of:
        TS_START, TS_COMMENT. */

    temp = skipBlank();
    if (STATE == TS_START) {
        setTokPos(
            tok->pos,
            TOK_FILE_NAME,
            currTokF,
            LINE_NUM,
            COL_NUM,
            LINES_BEFORE,
            temp,
            orderLineNum
        );
        while (NEXT_CHAR == '') {
            getNextChar();
            tok->pos->spacesBefore = skipBlank();
        }

        if (isalpha(NEXT_CHAR) || NEXT_CHAR == '_') {
            if (!scanId()) {
                char saveChar = NEXT_CHAR;
                currTokLen = 0;
                currTok[currTokLen] = 0;
                getNextChar();
                if (saveChar == '"') {
                    retVal = scanStr(tok->data);
                } else {
                    retVal = scanChar(tok->data);
                }
                goto Return;
            }
        } else if (isdigit(NEXT_CHAR)) {
            retVal = scanNum(tok->data);
            goto Return;
        } else switch (NEXT_CHAR) {
            case '\'':
                getNextChar();
                retVal = scanChar(tok->data);
                goto Return;
                break;

            case '"':
                if (currLineIsInclude) {
                    getNextChar();
                    retVal = scanIncludeFileName(tok->data, '"');
                    goto Return;
                } else {
                    getNextChar();
                    retVal = scanStr(tok->data);
                    goto Return;
                    break;
                }

            case '\n':
                pushGetNextChar();
                currLineIsInclude = 0;
                break;

            case '!':
                pushGetNextChar();
                if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                }
                break;

            case '#':
                pushGetNextChar();
                if (TOK_NUM_AFTER_NEW_LINE == 1) {
                    skipBlank();
                    if (isalpha(NEXT_CHAR) || NEXT_CHAR == '_') {
                        scanId();
                    } else {
                        tok->data->code = Y_PRE_NULL;
                        retVal = ERR_NONE;
                        goto Return;
                    }
                } else {
                    if (NEXT_CHAR == '#') {
                        pushGetNextChar();
                    }
                }
                break;

            case '%':
                pushGetNextChar();
                if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                }
                break;

            case '&':
                pushGetNextChar();
                if (NEXT_CHAR == '&') {
                    pushGetNextChar();
                    if (NEXT_CHAR == '=') {
                        pushGetNextChar();
                    }
                }
                break;

            case '(':
                pushGetNextChar();
                break;

            case ')':
                pushGetNextChar();
                break;

            case '*':
                pushGetNextChar();
                if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                }
                break;

            case '+':
                pushGetNextChar();
                if (NEXT_CHAR == '+') {
                    pushGetNextChar();
                } else if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                }
                break;

            case ',':
                pushGetNextChar();
                break;

            case '-':
                pushGetNextChar();
                if (NEXT_CHAR == '-') {
                    pushGetNextChar();
                } else if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                } else if (NEXT_CHAR == '>') {
                    pushGetNextChar();
                }
                break;

            case '.':
                pushGetNextChar();
                if (NEXT_CHAR == '.') {
                    pushGetNextChar();
                    if (NEXT_CHAR == '.') {
                        pushGetNextChar();
                    } else {
                        retVal = RERR_INV_CHAR;
                        goto Return;
                    }
                } else if (isdigit(NEXT_CHAR)) {
                    if (pushFloatDotExp(tok->data, 1)) {
                        retVal = convStr2Const(tok->data);
                        goto Return;
                    } else {
                        retVal = RERR_INV_CHAR;
                        goto Return;
                    }
                }

                break;

            case '/':
                pushGetNextChar();
                if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                } else if (NEXT_CHAR == '*') {          /* comment begin */
                    popChars(1);
                    STATE = TS_COMMENT;
                    getNextChar();
                    retVal = scanComment(tok->data);
                    goto Return;
                } else if (NEXT_CHAR == '/') {
                    popChars(1);
                    STATE = TS_COMMENT;
                    getNextChar();
                    retVal = scanCPlusPlusComment(tok->data);
                    goto Return;
                }
                break;

            case ':':
                pushGetNextChar();
                if (NEXT_CHAR == '>') {
                    pushGetNextChar();
                }
                break;

            case ';':
                pushGetNextChar();
                break;

            case '<':
                if (currLineIsInclude) {
                    getNextChar();
                    retVal = scanIncludeFileName(tok->data, '>');
                    goto Return;
                } else {
                    pushGetNextChar();
                    if (NEXT_CHAR == '<') {
                        pushGetNextChar();
                        if (NEXT_CHAR == '=') {
                            pushGetNextChar();
                        }
                    } else if (NEXT_CHAR == '=') {
                        pushGetNextChar();
                    }
                }
                break;

            case '=':
                pushGetNextChar();
                if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                }
                break;

            case '>':
                pushGetNextChar();
                if (NEXT_CHAR == '>') {
                    pushGetNextChar();
                    if (NEXT_CHAR == '=') {
                        pushGetNextChar();
                    }
                } else if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                }
                break;

            case '?':
                pushGetNextChar();
                break;

            case '[':
                pushGetNextChar();
                break;

            case ']':
                pushGetNextChar();
                break;

            case '^':
                pushGetNextChar();
                if (NEXT_CHAR == '=')
                    pushGetNextChar();
                break;

            case '{':
                pushGetNextChar();
                break;

            case '|':
                pushGetNextChar();
                if (NEXT_CHAR == '=') {
                    pushGetNextChar();
                } else if (NEXT_CHAR == '|') {
                    pushGetNextChar();
                }
                break;

            case '}':
                pushGetNextChar();
                break;

            case '~':
                pushGetNextChar();
                break;

            case (char) EOF:
                tok->data->code = Y_EOF;
                retVal = ERR_NONE;
                goto Return;
                break;

            default:
                /* Eat up an ivalid character */
                getNextChar();
                retVal = RERR_INV_CHAR;
                goto Return;
        }

        tokTabPtr = tabLookup(currTok);
        if (tokTabPtr != NULL)
        {
            tok->data->code = tokTabPtr->code;
            if (tok->data->code == Y_PRE_INCLUDE) {
                currLineIsInclude = 1;
            }
            if (tok->data->code == Y_PRE_DEFINE) {
                tokAfterDefine = 0;
            }
            if (tok->data->code == Y_LEFT_PAREN && tokAfterDefine == 2) {
                // the case of #define x(...
                if (tok->pos->spacesBefore == 0) {
                    tok->data->code = Y_PRE_SPECIAL_LEFT_PAREN;
                }
            }
            tok->data->repr.string = registerString(tokTabPtr->name,
                                                  !FREE_STRING);
        }
        else {
            if (currTok[0] == '#') {
                retVal = RERR_INV_PREPROCESSOR;
                goto Return;
            } else {
                tok->data->code = Y_ID;
                tok->data->repr.string = registerString(wicStrdup(currTok),
                                                      FREE_STRING);
            }
        }
    } else if (STATE == TS_COMMENT) {
        setTokPos(tok->pos, TOK_FILE_NAME, currTokF, LINE_NUM, COL_NUM,
              LINES_BEFORE, 0, orderLineNum);
        retVal = scanComment(tok->data);
        goto Return;
    } else {
        assert(0);
    }

    Return:
        if (tok->data->code != Y_PRE_NEWLINE) {
            tok->pos->linesBefore  = tok->pos->lineNum - PREV_TOK_LINE_NUM;
            PREV_TOK_LINE_NUM = tok->pos->lineNum;
        } else {
            tok->pos->linesBefore = 0;
        }
        zapTokPos(g_currPos);
        g_currPos = dupTokPos(tok->pos, NULL);
        return retVal;
}

void setTokPos(pTokPos pos, char *fileName, int8 fileLevel, int16 lineNum,
    int16  colNum, uint8  linesBefore, uint8  spacesBefore, long orderLineNum)
{
    pos->fileName = fileName;
    pos->fileLevel = fileLevel;
    pos->lineNum = lineNum;
    pos->colNum = colNum;
    pos->linesBefore = linesBefore;
    pos->spacesBefore = spacesBefore;
    pos->orderLineNum = orderLineNum;
}

TokenType getTokDataType(pTokData tokData) {
    int code;
    if (tokData == NULL) {
        code = Y_EOF;
    } else {
        code = tokData->code;
    }
    if (code >= BEGIN_Y_OTHER && code <= END_Y_OTHER) {
        return TT_OTHER;
    } else if (code >= BEGIN_Y_PREPROCESSOR && code <= END_Y_PREPROCESSOR) {
        return TT_PREPROCESSOR;
    } else {
        assert(0);
        return 0;
    }
}

char *getTokDataIdName(pTokData data) {
// Note: Use staticGetTokenStr for debugging purposes!
    if (data == NULL) {
        return NULL;
    } else {
        return data->repr.string;
    }
}

char *getTokenIdName(pToken tok) {
    if (tok == NULL) {
        return NULL;
    } else {
        return getTokDataIdName(tok->data);
    }
}


pToken createToken(pTokData data, pTokPos pos) {
    pToken tok;
    tok = wicMalloc(sizeof *tok);
    tok->data = data;
    tok->pos = pos;
    return tok;
}

pTokPos createTokPos(void) {
    pTokPos newPos = wicMalloc(sizeof *newPos);
    memset(newPos, 0, sizeof *newPos);
    return newPos;
}

pTokData createTokData(void) {
    pTokData newData = wicMalloc(sizeof *newData);
    memset(newData, 0, sizeof *newData);
    newData->repr.string = NULL;
    return newData;
}

pToken createEnumNumToken(unsigned long num) {
    pToken tok = createToken(createTokData(), NULL);
    tok->data->code = Y_NUMBER;
    tok->data->repr.constant.type = CONSTT_UINT_CONST;
    tok->data->repr.constant.radix = RADT_DECIMAL;
    tok->data->repr.constant.repr.lIntConst = num;
    return tok;
}

pToken createPLUSToken(void) {
    pToken tok = createToken(createTokData(), NULL);
    tok->data->code = Y_PLUS;
    tok->data->repr.string = registerString("+", !FREE_STRING);
    return tok;
}

pToken createEQUALToken(void) {
    pToken tok = createToken(createTokData(), NULL);
    tok->data->code = Y_EQUAL;
    tok->data->repr.string = registerString("=", !FREE_STRING);
    return tok;
}

pToken createIDTokenAfter( char *str, pTokPos pos) {
    pToken tok = createToken(createTokData(), dupTokPos(pos, NULL));
    tok->pos->spacesBefore = 1;
    tok->pos->linesBefore = 0;
    tok->data->code = Y_ID;
    tok->data->repr.string = registerString(wicStrdup(str), FREE_STRING);
    return tok;
}

pToken createCommentToken(char *str, pTokPos pos) {
    pToken tok = createToken(createTokData(), pos);
    tok->data->code = Y_PRE_COMMENT;
    tok->data->repr.string = registerString(str, FREE_STRING);
    return tok;
}

int cmpTokData(pTokData d1, pTokData d2) {
    if (d1 == NULL && d2 != NULL) {
        return 1;
    } else if (d1 != NULL && d2 == NULL) {
        return 1;
    } else if (d1 == NULL && d2 == NULL) {
        return 0;
    } else {
        return (d1->code != d2->code);
    }
}

pTokPos combine2TokPos(pTokPos p1, pTokPos p2) {
    if (p1 == NULL) {
        if (p2 == NULL) {
            return NULL;
        } else {
            return p2;
        }
    } else {
        if (p2 == NULL) {
            return p1;
        } else {
            zapTokPos(p2);
            return p1;
        }
    }
}

pToken combine2Token(pToken t1, pToken t2) {
    pToken newTok;
    if (t1 != NULL && t2 != NULL) {
        if (cmpTokData(t1->data, t2->data) == 0) {
            newTok = t1;
            if (t1 != t2) {
                zapToken(t2); t2 = NULL;
            }
        } else {
            char errStr[100];
            char *s;
            int m;
            s = staticGetTokenStr( t1, 0 );
            m = strlen( s );
            if( m > 99 )
                m = 99;
            memcpy( errStr, s, m );
            errStr[m] = 0;
            reportError(RERR_CANT_COMBINE_2_TOKENS, errStr,
                                                staticGetTokenStr(t2, 0) );
            zapToken(t2); t2 = NULL;
            newTok = t1; t1 = NULL;
        }
    } else if (t1 != NULL) {
        newTok = t1;
        zapToken(t2);
        t2 = NULL;
    } else if (t2 != NULL) {
        newTok = t2;
        zapToken(t1);
        t1 = NULL;
    } else {
        newTok = NULL;
    }
    return newTok;
}

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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include "wic.h"
#include "output.h"
#include "outfort.h"
#include "outasm.h"

typedef struct {
    PrintType type;  // If type == LIST then the type is in 'list'.

    void* data;
    struct {
        PrintType type;
        pSLList list;
    } list;

    int fileNum;  // MAIN_FILE for normal output (.fi for fortran, .inc for asm)
                  // SECONDARY_FILE for alternative output (.fap for fortran,
                  // none for asm)
    void* param;
} PT_StackElem, *pPT_StackElem;  // print type stack, used by output.c

static struct {
    char fileName[MAX_NUM_OF_OUT_FILES][_MAX_PATH];
    FILE *output[MAX_NUM_OF_OUT_FILES];
    char lastChar[MAX_NUM_OF_OUT_FILES];  // Last char written
    char line[MAX_NUM_OF_OUT_FILES][MAX_OUT_LINE_LEN];
    char lineComment[MAX_OUT_LINE_LEN];
    char lineLen[MAX_NUM_OF_OUT_FILES];
    enum {
        LF_NONE = 0x0,
        LF_CONT = 0x1,
        LF_NEED_CONT_SIG = 0x2,  // Need continue signature at beginning of line
        LF_DIRECTIVE = 0x4,
    }  lineFlags[MAX_NUM_OF_OUT_FILES];
} fileStack[MAX_INCLUDE_NEST];

#define LF_INIT_FORT_FLAGS      LF_NONE
#define LF_INIT_ASM_FLAGS       LF_NONE

#define PT_STACK_MAX  50

typedef struct {
    pOUnit nextOUnit;
    PT_StackElem stack[PT_STACK_MAX];
    int stackPos;
} PrintInfo;

static pHTable keywordsTable = NULL;
static PrintInfo printData[NUM_OF_PRINT_TYPES];
static PrintStackType currWorkStack;
static pOUnit currOUnit;
static int linesLeftToSkip = 0;

static int fileStackLevel = -1;
#define CURR_OUTPUT(i) (fileStack[fileStackLevel].output[(i)])
#define CURR_FILE_NAME(i) (fileStack[fileStackLevel].fileName[(i)])
#define CURR_FILE_POS(i) (fileStack[fileStackLevel].filePos[(i)])
#define CURR_LAST_CHAR(i) (fileStack[fileStackLevel].lastChar[(i)])
#define CURR_LINE(i) (fileStack[fileStackLevel].line[(i)])
#define CURR_LINE_LEN(i) (fileStack[fileStackLevel].lineLen[(i)])
#define CURR_LINE_COMMENT (fileStack[fileStackLevel].lineComment)
#define CURR_LINE_FLAGS(i) (fileStack[fileStackLevel].lineFlags[(i)])

TargetLangOpt g_tlang;

/* Forward declarations */
int popPrintStack(PrintStackType *type, PrintType *listType, void **data, void**param, int *fileNum);


static pOUnit _createOUnit(OUnitType type, pTokPos pos, char *string) {
    pOUnit newUnit = wicMalloc(sizeof *newUnit);
    newUnit->type = type;
    if (pos != NULL) {
        memcpy(&(newUnit->pos), pos, sizeof *pos);
        newUnit->posPresent = 1;
    } else {
        newUnit->posPresent = 0;
    }

    newUnit->fileNum = MAIN_FILE;
    newUnit->preStackLevel = 0;
    newUnit->postStackLevel = 0;
    if (string != NULL) {
        strncpy(newUnit->string, string, MAX_TOKEN_SIZE);
    } else {
        newUnit->string[0] = 0;
    }
    newUnit->stringIsId = 0;
    return newUnit;
}

pOUnit createOUnitText(char *string, pTokPos pos) {
    pOUnit newUnit = _createOUnit(OUT_TEXT, pos, string);
    return newUnit;
}

pOUnit createOUnitTextId(char *string, pTokPos pos) {
    pOUnit newUnit = createOUnitText(string, pos);
    newUnit->stringIsId = 1;
    return newUnit;
}

pOUnit createOUnitNewline(void) {
    pOUnit newUnit = _createOUnit(OUT_NEWLINE, NULL, NULL);
    return newUnit;
}

pOUnit createOUnitFromTok(pToken tok) {
    pOUnit newUnit;
    int code;
    if (tok == NULL) {
        return NULL;
    }
    code = tok->data->code;
    if (code == Y_EOF) {
        newUnit = _createOUnit(OUT_EOF, tok->pos, NULL);
    } else {
        OUnitType newType = OUT_TEXT;
        if (code == Y_PRE_COMMENT) {
            newType = OUT_COMMENT;
        }
        newUnit = _createOUnit(newType, tok->pos, NULL);
        getTokDataStr(tok->data, 1, newUnit->string, MAX_TOKEN_SIZE);
        newUnit->stringIsId = (code == Y_ID);
    }
    return newUnit;
}

pOUnit addOUnitPrefix(pOUnit unit, char *preString) {
    if (unit->preStackLevel >= _OUNIT_STR_STACK_MAX) {
        reportError(ERR_INTERNAL_STACK_OVERFLOW);
    } else {
        unit->preStrStack[unit->preStackLevel++] = preString;
    }
    return unit;
}

pOUnit addOUnitPostfix(pOUnit unit, char *postString) {
    if (unit->postStackLevel >= _OUNIT_STR_STACK_MAX) {
        reportError(ERR_INTERNAL_STACK_OVERFLOW);
    } else {
        unit->postStrStack[unit->postStackLevel++] = postString;
    }
    return unit;
}

pOUnit addOUnitPrePost(pOUnit unit, char *preString, char* postString) {
    return addOUnitPostfix( addOUnitPrefix( unit, preString ), postString );
}

void zapOUnit(pOUnit unit) {
    wicFree(unit);
}

pOUnit addOUnitPostfixImmediate(pOUnit unit, char *postString) {
    int len;
    char *s;
    if (unit == NULL || postString == NULL) {
        return unit;
    }
    s = unit->string;
    len = strlen(s);
    strncpy(s+len, postString, max(0, MAX_TOKEN_SIZE - len - 1));
    return unit;
}

pOUnit createOUnitDclrName(char *name, pTokPos typePos, char *postfix) {
    char *middle = g_opt.structFieldsSep;
    if (postfix == NULL) {
        middle = NULL;
    }
    if (!g_opt.prefixStructFields) {
        postfix = NULL;
    }

    return addOUnitPostfixImmediate(
        addOUnitPostfixImmediate(
            createOUnitTextId(name, typePos),
            middle
        ),
        postfix
    );
}
/*--------------------------------------------------------------------------*/

static void fprintfCheckNULL(FILE *fp, const char *format, ...) {
    if (fp != NULL) {
        va_list arglist;
        va_start(arglist, format);
        vfprintf(fp, format, arglist);
        va_end(arglist);
    }
}

void _outputCurrAsmLine(int fnum, int continueToNextLine) {
    char *line = CURR_LINE(fnum);
    FILE *output = CURR_OUTPUT(fnum);
    int len = CURR_LINE_LEN(fnum);
    int flags = CURR_LINE_FLAGS(fnum);
    #ifndef NDEBUG
        if (g_opt.debug) {
            output = stdout;
        }
    #endif
    fprintfCheckNULL(output, "%.*s", len, line);
    if (continueToNextLine) {
        if (len > 0) {
            fprintfCheckNULL(output, "\\");
            flags |= LF_CONT;
        } else {
            if (flags & LF_CONT) {
                fprintfCheckNULL(output, "\\");
            }
        }
    }
    if (fnum == MAIN_FILE) {
        fprintfCheckNULL(output, "%s", CURR_LINE_COMMENT);
    }
    fprintfCheckNULL(output, "\n");

    /* Printing done, initialize next line */
    if (!continueToNextLine) {
        flags &= ~LF_CONT;
    }
    CURR_LINE_FLAGS(fnum) = flags;
    CURR_LINE_LEN(fnum) = 0;
    if (fnum == MAIN_FILE) {
        CURR_LINE_COMMENT[0] = 0;
    }
    CURR_LAST_CHAR(fnum) = '\n';
}

void _outputCurrFortLine(int fnum, int continueToNextLine) {
    char *line = CURR_LINE(fnum);
    int len = CURR_LINE_LEN(fnum);
    FILE *output = CURR_OUTPUT(fnum);
    int flags = CURR_LINE_FLAGS(fnum);
    char *prependString = "      ";
    int spacesBegLine = 0;
    char *leftOver = NULL;
    int leftOverLen = 0;

    assert(fileStackLevel >= 0);
    #ifndef NDEBUG
        if (g_opt.debug) {
            output = stdout;
        }
    #endif

    /* Get rid of spaces */
    for (; line[0] == '\t' && len > 0;) {
        line += 1;
        len -= 1;
        spacesBegLine += TAB_SIZE;
    }
    while (line[0] == ' ' && len > 0) {
        for (; line[0] == ' ' && len > 0;) {
            line++;
            len--;
            spacesBegLine += 1;
        }
        for (; line[0] == '\t' && len > 0;) {
            line += 1;
            len -= 1;
            spacesBegLine += TAB_SIZE;
        }
    }
    for (; line[0] == ' ' && len > 0;) {
        line++;
        len--;
        spacesBegLine += 1;
    }


    if (!(flags & LF_CONT) || !(flags & LF_NEED_CONT_SIG)) {
        if (len > 1 && line[0] == '*' && line[1] == '$') {   // Is a directive
            prependString = "";
            spacesBegLine = 0;
            flags |= LF_DIRECTIVE;
        }
    }
    if (flags & LF_DIRECTIVE || len == 0) {
        if (flags & LF_NEED_CONT_SIG) {
            prependString = "*";
        } else {
            spacesBegLine = 0;
            prependString = "";
        }
    } else {
        if (flags & LF_CONT   &&   flags & LF_NEED_CONT_SIG) {
            prependString = "     &";
        }
    }

    fprintfCheckNULL(output, "%s", prependString);
    fprintfCheckNULL(output, "%*s", spacesBegLine, "");
    if (continueToNextLine) {
        int i = len-1;
        int class1, class2;
        if (! (flags & LF_DIRECTIVE)) {
            i -= 6;  // Take into account the 6 spaces at the beginning of line
        }

        while (i > 1) {
            class2 = isalnum(line[i]);
            class1 = isalnum(line[i-1]);
            if ((class1 == 0) || (class2 == 0) || (class1 != class2)) {
                // found a place to split!
                leftOverLen = len - i;
                len = i;
                leftOver = line+len;
                break;
            }
            i--;
        }
    }
    fprintfCheckNULL(output, "%.*s", len, line);
    if (continueToNextLine   &&   flags & LF_DIRECTIVE) {
        fprintfCheckNULL(output, "\\");
    }
    if (fnum == MAIN_FILE && CURR_LINE_COMMENT[0] != 0) {
        char *s = CURR_LINE_COMMENT;
        int i = 0;
        while(s[0] == ' ') {
            i++; s++;
        }
        if (i == 5 && len == 0) {
            i++;  // Avoid having '!' in sixth column
        }
        fprintfCheckNULL(output, "%*.s%s", i, "", s);
    }
    fprintfCheckNULL(output, "\n");


    /* Printing done, initialize next line */
    if (continueToNextLine) {
        flags |= LF_CONT;
        if (len != 0) {
            flags |= LF_NEED_CONT_SIG;
        }
    } else {
        flags = g_tlang.lfInitFlags;
    }
    CURR_LINE_FLAGS(fnum) = flags;
    if (leftOver != NULL) {
        CURR_LINE_LEN(fnum) = leftOverLen;
        memcpy(CURR_LINE(fnum), leftOver, CURR_LINE_LEN(fnum));
    } else {
        CURR_LINE_LEN(fnum) = 0;
    }
    if (fnum == MAIN_FILE) {
        CURR_LINE_COMMENT[0] = 0;
    }
    CURR_LAST_CHAR(fnum) = '\n';
}

static void _outputChar(int fnum, char ch) {
    int substract = g_tlang.substractFromLineLen;
    if (CURR_LINE_LEN(fnum) >= g_opt.outLineLen-substract) {
        g_tlang.outputCurrLine(fnum, 1);
    }
    CURR_LINE(fnum)[CURR_LINE_LEN(fnum)++] = ch;
    if (ch == '\n') {
        assert(0);
        g_tlang.outputCurrLine(fnum, 0);
        CURR_LAST_CHAR(fnum) = ch;
    } else if (isspace(ch)) {
        CURR_LAST_CHAR(fnum) = ch;
    }
}

static void _outputString(int fnum, char *s, int stringIsId) {
    char lastChar;
    int i;
    if (s == NULL) {
        return;
    }
    assert(0 <= fnum && fnum < g_tlang.numOfOutFiles);
    if (s[0] == 0) {
        return;
    }

    lastChar = s[strlen(s)-1];
    if ((isalnum(CURR_LAST_CHAR(fnum)) || CURR_LAST_CHAR(fnum) == '_') &&
        (isalnum(lastChar) || lastChar == '_') ) {
        _outputChar(fnum, ' ');
    }

    if (stringIsId) {
        if (findHTableElem(keywordsTable, s) != NULL) {
            char *s;
            for (s = g_opt.conflictPrefix; *s != 0; s++) {
                _outputChar(fnum, *s);
            }
        }
    }

    for (i = 0; s[i] != NULL; i++) {
        _outputChar(fnum, s[i]);
    }
    CURR_LAST_CHAR(fnum) = lastChar;
}

void _outputContinueLine(fileNum) {
    g_tlang.outputCurrLine(fileNum, 1);
}

void _outputOUnit(pOUnit unit) {
    int spacesBefore = unit->pos.spacesBefore;
    int commentSize, commentLeft;
    int i;

    assert(unit->posPresent);
    if (unit->type == OUT_TEXT) {
        for (; spacesBefore > 0; spacesBefore --) {
            _outputChar(unit->fileNum, ' ');
        }
    } else if (unit->type == OUT_COMMENT) {
        commentSize = strlen(CURR_LINE_COMMENT);
        commentLeft = sizeof CURR_LINE_COMMENT  - commentSize-1;
        for (; spacesBefore > 0 && commentLeft > 0;
             spacesBefore--, commentLeft--)
        {
            CURR_LINE_COMMENT[commentSize++] = ' ';
        }
        CURR_LINE_COMMENT[commentSize] = 0;
    }

    for (i = unit->preStackLevel-1; i >= 0; i--) {
        _outputString(unit->fileNum, unit->preStrStack[i], 0);
    }

    switch (unit->type) {
        case OUT_TEXT:
            _outputString(unit->fileNum, unit->string, unit->stringIsId);
            break;

        case OUT_NEWLINE:
            g_tlang.outputCurrLine(unit->fileNum, 0);
            break;

        case OUT_COMMENT:
            strncpy(CURR_LINE_COMMENT+commentSize, unit->string, commentLeft);
            break;

        case OUT_EOF:
            break;
    }

    for (i = unit->postStackLevel-1; i >= 0; i--) {
        _outputString(unit->fileNum, unit->postStrStack[i], 0);
    }
}

void _popFileStack(void) {
    int i;
    assert(fileStackLevel >= 0);
    for (i = 0; i < g_tlang.numOfOutFiles; i++) {
        g_tlang.outputCurrLine(i, 0);
        if (CURR_OUTPUT(i) != NULL) {
            wicFclose(CURR_OUTPUT(i));
        }
    }
    fileStackLevel--;
    if (fileStackLevel >= 0) {
        for (i = 0; i < g_tlang.numOfOutFiles; i++) {
            CURR_OUTPUT(i) = wicFopen(CURR_FILE_NAME(i), "at");
            if (CURR_OUTPUT(i) == NULL) {
                reportError(ERR_OPEN_FILE, CURR_FILE_NAME(i), strerror(errno));
                reportError(FATAL_DIE);
            }
        }
    }
}

void _popAllFiles(void) {
    assert(fileStackLevel >= 0);
    while (fileStackLevel >= 0) {
        _popFileStack();
    }
}

void _pushFileStack(char *fileName) {
    int i;

    if (fileStackLevel >= 0) {
        for (i = 0; i < g_tlang.numOfOutFiles; i++) {
            g_tlang.outputCurrLine(i, 0);
            if (CURR_OUTPUT(i) != NULL) {
                wicFclose(CURR_OUTPUT(i));
            }
        }
    }
    fileStackLevel++;

    if (fileStackLevel >= MAX_INCLUDE_NEST) {
        fileStackLevel--;
        reportError(ERR_NEST_INCLUDE);
        return;
    } else {
        FDReadInd entry;
        pFDReadInd incFile;
        initFDReadInd(&entry,  fileName, 0);
        incFile = findHTableElem(g_fileTable, &entry);
        assert(incFile != NULL);

        assert(fileName != NULL);
        for (i = 0; i < g_tlang.numOfOutFiles; i++) {
            setNewFileExt(CURR_FILE_NAME(i), fileName, g_tlang.extension[i]);
            if (incFile->readOnly) {
                CURR_OUTPUT(i) = NULL;
            } else {
                CURR_OUTPUT(i) = wicFopen(CURR_FILE_NAME(i), "wt");
                if (CURR_OUTPUT(i) == NULL) {
                    int j;
                    for (j = i-1; j >= 0; j--) {
                        wicFclose(CURR_OUTPUT(j));
                        CURR_OUTPUT(j) = NULL;
                    }
                    // fileStackLevel--;
                    return;
                }
            }

            CURR_LAST_CHAR(i) = 0;
            CURR_LINE_LEN(i) = 0;
            CURR_LINE(i)[0] = 0;
            CURR_LINE_FLAGS(i) = g_tlang.lfInitFlags;
        }
    }
}



/*--------------------------------------------------------------------------*/

static void _putUnprintableChar(char **buffer, char ch, int *spacesLeft) {
    int d1, d2, d3;
    if (*spacesLeft < 15) {
        *spacesLeft = 0;
        return;
    }
    strcpy(*buffer, "'//CHAR(   )//'");
    d1 = (unsigned char) ch % 10;
    ch = (ch-d1)/10;
    d2 = (unsigned char) ch % 10;
    ch = (ch-d2)/10;
    d3 = (unsigned char) ch % 10;
    *buffer += 8;
    (*buffer)[0] = d3 + '0';
    (*buffer)[1] = d2 + '0';
    (*buffer)[2] = d1 + '0';
    *buffer += 7;
    *spacesLeft -= 15;
}


static void _getFortranString(char *buffer, int spacesLeft,
                              char *string, char strLen) {
    spacesLeft -= 8;
    memcpy(buffer, "LOC('", 5);
    buffer += 5;
    while(strLen != 0) {
        if (isprint(string[0]) && string[0] != '\'') {
            if (spacesLeft > 0) {
                buffer++[0] = string[0]; spacesLeft--;
            } else {
                break;
            }
        } else {
            _putUnprintableChar(&buffer, string[0], &spacesLeft);
        }
        strLen--;
        string++;
    }

    buffer++[0] = '\'';
    buffer++[0] = 'C';
    buffer++[0] = ')';
    buffer++[0] = 0;

    assert(spacesLeft >= 0);
}

static void _getAsmString(char *buffer, int spacesLeft,
                              char *string, char strLen) {
    char ch;
    spacesLeft -= 5;

    buffer++[0] = '"';
    while(strLen != 0) {
        ch = string[0];
        if (ch == '"') {
            if (spacesLeft >= 2) {
                buffer++[0] = '"'; spacesLeft--;
                buffer++[0] = '"'; spacesLeft--;
            } else {
                break;
            }
        } else if (!isprint(ch)) {
            if (spacesLeft >= 10) {
                int size;
                size = sprintf(buffer, "\",%d", (unsigned int) ch);
                if (g_opt.asmAttachD) {
                    buffer[size++] = 'D';
                }
                buffer[size++] = ',';
                buffer[size++] = '"';
                buffer += size;
            } else {
                break;
            }
        } else {
            if (spacesLeft > 0) {
                buffer++[0] = ch; spacesLeft--;
            } else {
                break;
            }
        }
        strLen--;
        string++;
    }

    buffer++[0] = '"';
    buffer++[0] = ',';
    buffer++[0] = '0';
    buffer++[0] = 0;
    assert(spacesLeft >= 0);
}

static void _getCString(char *buffer, int spacesLeft,
                              char *string, char strLen) {
    spacesLeft -= 3;
    buffer++[0] = '"';
    while(strLen != 0) {
        if (string[0] != '"') {
            if (spacesLeft > 0) {
                buffer++[0] = string[0]; spacesLeft--;
            } else {
                break;
            }
        } else {
            if (spacesLeft >= 4) {
                char ch = string[0];
                spacesLeft -= 4;
                buffer++[0] = '\\';
                buffer[0] = ch/64;  ch -= buffer++[0]*64;
                buffer[0] = ch/8;  ch -= buffer++[0]*8;
                buffer++[0] = ch;
            } else {
                break;
            }
        }
        strLen--;
        string++;
    }

    buffer++[0] = '"';
    buffer++[0] = 0;
    assert(spacesLeft >= 0);
}

static void _convertConstant(pTokData t, TargetLangType lang, char *s,
                             int spaceLeft) {
    ConstType type = t->repr.constant.type;
    RadixType radix = t->repr.constant.radix;
    long int longInt = t->repr.constant.repr.lIntConst;
    long int longDouble = t->repr.constant.repr.lDoubleConst;
    char *format;

    switch  (type) {
    case CONSTT_CHAR_CONST:
        switch (lang) {
        case TLT_C:
            if ((unsigned long int) longInt < 255 && isprint((char)longInt)) {
                sprintf(s, "'%c'", (char) longInt);
            } else {
                sprintf(s, "'(0x%lx)'",  longInt);
            }
            break;
        case TLT_FORTRAN:
            sprintf(s, "%ld",  longInt);
            break;
        case TLT_ASM:
            if ((unsigned long int) longInt < 255 && isprint((char)longInt)) {
                sprintf(s, "'%c'", (char) longInt);
            } else {
                sprintf(s, "0%lXH",  longInt);
            }
            break;
        default:
            assert(0);
        }
        break;

    case CONSTT_STRING_CONST:
        sprintf(s, "\"%.*s\"", spaceLeft, t->repr.string);
        break;

    case CONSTT_INT_CONST:
        switch (lang) {
        case TLT_C:
            switch (radix) {
            case RADT_DECIMAL: format = "%li";  break;
            case RADT_HEX: format = "0x%lX";  break;
            case RADT_OCTAL: format = "0%lo";  break;
            default: assert(0);
            }
            break;
        case TLT_FORTRAN:
            switch (radix) {
            case RADT_DECIMAL: format = "%li";  break;
            case RADT_HEX: format = "'%lX'x";  break;
            case RADT_OCTAL: format = "'%lo'o";  break;
            default: assert(0);
            }
            break;
        case TLT_ASM:
            switch (radix) {
            case RADT_DECIMAL: format = g_opt.asmAttachD ? "%liD": "%li";  break;
            case RADT_HEX: format = "0%lXH";  break;
            case RADT_OCTAL: format = "0%loO)";  break;
            default: assert(0);
            }
            break;
        default:
            assert(0);
        }
        sprintf(s, format, longInt);
        break;

    case CONSTT_UINT_CONST:
        switch (lang) {
        case TLT_C:
            switch (radix) {
            case RADT_DECIMAL: format = "%lu";  break;
            case RADT_HEX: format = "0x%lX";  break;
            case RADT_OCTAL: format = "0%lo";  break;
            default: assert(0);
            }
            break;
        case TLT_FORTRAN:
            switch (radix) {
            case RADT_DECIMAL: format = "%lu";  break;
            case RADT_HEX: format = "'%lX'x";  break;
            case RADT_OCTAL: format = "'%lo'o";  break;
            default: assert(0);
            }
            break;
        case TLT_ASM:
            switch (radix) {
            case RADT_DECIMAL: format = g_opt.asmAttachD ? "%luD": "%lu";  break;
            case RADT_HEX: format = "0%lXH";  break;
            case RADT_OCTAL: format = "0%loO";  break;
            default: assert(0);
            }
            break;
        default:
            assert(0);
        }
        sprintf(s, format, longInt);
        break;

    case CONSTT_LONG_CONST:
        switch (lang) {
        case TLT_C:
            switch (radix) {
            case RADT_DECIMAL: format = "%liL";  break;
            case RADT_HEX: format = "0x%lXL";  break;
            case RADT_OCTAL: format = "0%loL";  break;
            default: assert(0);
            }
            break;
        case TLT_FORTRAN:
            switch (radix) {
            case RADT_DECIMAL: format = "%li";  break;
            case RADT_HEX: format = "'%lX'x";  break;
            case RADT_OCTAL: format = "'%lo'o";  break;
            default: assert(0);
            }
            break;
        case TLT_ASM:
            switch (radix) {
            case RADT_DECIMAL: format = g_opt.asmAttachD ? "%liD": "%li";  break;
            case RADT_HEX: format = "0%lXH";  break;
            case RADT_OCTAL: format = "0%loO";  break;
            default: assert(0);
            }
            break;
        default:
            assert(0);
        }
        sprintf(s, format, longInt);
        break;

    case CONSTT_ULONG_CONST:
        switch (lang) {
        case TLT_C:
            switch (radix) {
            case RADT_DECIMAL: format = "%luLU";  break;
            case RADT_HEX: format = "0x%lXLU";  break;
            case RADT_OCTAL: format = "0%loLU";  break;
            default: assert(0);
            }
            break;
        case TLT_FORTRAN:
            switch (radix) {
            case RADT_DECIMAL: format = "%li";  break;
            case RADT_HEX: format = "'%lX'x";  break;
            case RADT_OCTAL: format = "'%lo'o";  break;
            default: assert(0);
            }
            break;
        case TLT_ASM:
            switch (radix) {
            case RADT_DECIMAL: format = g_opt.asmAttachD ? "%liD": "%li";  break;
            case RADT_HEX: format = "0%lXH";  break;
            case RADT_OCTAL: format = "0%loO";  break;
            default: assert(0);
            }
            break;
        default:
            assert(0);
        }
        sprintf(s, format, (unsigned long) longInt);
        break;

    case CONSTT_FLOAT_CONST:
        switch (lang) {
        case TLT_C:
            sprintf(s, "%lgF", longDouble);
            break;
        case TLT_FORTRAN:
            sprintf(s, "%lg", longDouble);
            break;
        case TLT_ASM:
            sprintf(s, "%lg.", longDouble);
            break;
        default:
            break;
        }
        break;

    case CONSTT_DOUBLE_CONST:
        switch (lang) {
        case TLT_C:
            sprintf(s, "%lgDF", longDouble);
            break;
        case TLT_FORTRAN:
            sprintf(s, "%lg", longDouble);
            break;
        case TLT_ASM:
            sprintf(s, "%lg.", longDouble);
            break;
        }
        break;

    case CONSTT_LDOUBLE_CONST:
        switch (lang) {
        case TLT_C:
            sprintf(s, "%lgLDF", longDouble);
            break;
        case TLT_FORTRAN:
            sprintf(s, "%lg", longDouble);
            break;
        case TLT_ASM:
            sprintf(s, "%lg.", longDouble);
            break;
        }
        break;

    default:
        assert(0);
        break;
    }
}

char *getTokDataStr(pTokData t, int useTargetLang, char *s, int spaceLeft) {
    TargetLangType lang;

    spaceLeft--;
    if (useTargetLang) {
        lang = g_opt.targetLang;
    } else {
        lang = TLT_C;
    }

    if (t == NULL) {
        reportError(FATAL_INTERNAL, "in getTokenStr: Null token passed");
    } else {
        switch (t->code) {
        case Y_PRE_NEWLINE:
            sprintf(s, "<NL>");
            break;

        case Y_NUMBER:
            _convertConstant(t, lang, s, spaceLeft);
            break;

        case Y_EOF:
            sprintf(s, "END-OF-FILE");
            break;

        case Y_PRE_COMMENT:
            sprintf(s, "%c%.*s", g_tlang.commentChar,
                    spaceLeft-1, t->repr.string);
            break;

        case Y_STRING:
            switch(lang) {
            case TLT_C:
                _getCString(s, spaceLeft, t->repr.s.s, t->repr.s.strLen);
                break;
            case TLT_FORTRAN:
                _getFortranString(s, spaceLeft, t->repr.s.s, t->repr.s.strLen);
                break;
            case TLT_ASM:
                _getAsmString(s, spaceLeft, t->repr.s.s, t->repr.s.strLen);
                break;
            default: assert(0);
            }
            break;

        case Y_INCLUDE_FILE_NAME:
            sprintf(s, "%.*s", spaceLeft, getTokDataIdName(t));
            setNewFileExt(s, s, g_tlang.extension[0]);
            break;

        case Y_ID:
        case Y_TYPEDEF_NAME:
            sprintf(s, "%.*s", spaceLeft, getTokDataIdName(t));
            break;

        default:
            {
                pTokTab elem = tabLookup(t->repr.string);
                char *tokStr;
                if (elem == NULL) {
                    printf("DEBUG: t->code = %d", t->code);
                    assert(elem != NULL);
                }
                if (lang == TLT_C) {
                    tokStr = elem->name;
                } else {
                    tokStr = elem->tname[lang];
                }
                sprintf(s, "%.*s", spaceLeft, tokStr);
            }
        }
    }
    return s;
}

char *getTokenStr(pToken t, int useTargetLang, char *s) {
    int spacesToWrite;
    int spaceLeft;

    spacesToWrite = t->pos->spacesBefore;
    memset(s, ' ', spacesToWrite);
    spaceLeft = MAX_PRINT_TOKEN_SIZE - spacesToWrite;

    getTokDataStr(t->data, useTargetLang, s+spacesToWrite, spaceLeft);
    return s;
}

char *staticGetTokenStr(pToken t, int useTargetLang) {
    static char buffer[MAX_PRINT_TOKEN_SIZE];
    return getTokenStr(t, useTargetLang, buffer);
}

char *getTokListString(pSLList context) {
    enum { maxBufSize = 500 };
    static char buffer[maxBufSize];
    int bufSize = 0;
    pToken tok;
    static char *tokStr;
    int tokStrSize = 0;

    buffer[0] = 0;
    rewindCurrSLListPos(context);
    while (getCurrSLListPosElem(context, &tok)) {
        incCurrSLListPos(context);
        tokStr = staticGetTokenStr(tok, 0);
        tokStrSize = strlen(tokStr);
        tokStrSize = min(maxBufSize-bufSize-1,  tokStrSize);
        if (tokStrSize <= 0) {
            goto Return;
        }
        memcpy(buffer + bufSize, tokStr, tokStrSize);
        bufSize += tokStrSize;
        buffer[bufSize] = 0;
    }

    Return:
        return buffer;
}


//=====================================================================

void expandPushTree(int fileNum, void *_tree) {
    pCTree tree = _tree;
    pLabel label;
    int constrType;
    int pushType;
    int i;
    pPrintCTreeTable printTable = g_tlang.printCTreeTable;

    label = getCTreeLabel(tree);
    if (label == NULL) {
        return;
    }

    switch (label->type) {
    case LABT_LIST:
        initExpandPushList(fileNum, TOKEN, label->repr.list);
        break;

    case LABT_TOKEN:
        pushPrintStack(fileNum, TOKEN, label->repr.token);
        break;

    case LABT_CONSTRUCT_ROOT:
        constrType = label->repr.constr.type;
        assert(strlen(printTable[constrType].printList) ==
               printTable[constrType].num);
        for (i = printTable[constrType].num-1; i >= 0; i--) {
            pushType = printTable[constrType].printList[i];

            if (pushType == 'L') {
                pushPrintStack(fileNum, TREE, getCTreeChild1(tree));
            } else if (pushType == 'R') {
                pushPrintStack(fileNum, TREE, getCTreeChild2(tree));
            } else if (pushType >= '0'  && pushType <= '9') {
                assert(label->repr.constr.numTokens > pushType-'0');
                pushPrintStack(fileNum, TOKEN,
                        label->repr.constr.tokens[pushType-'0']);
            } else if (pushType >= 'a'  && pushType <= 'z') {
                pushPrintStack(fileNum, OUNIT,
                    createOUnitText(
                        printTable[constrType].extraStrings[pushType - 'a'],
                        NULL
                    )
                );
            } else if (pushType == 'N') {
                pushPrintStack(fileNum, OUNIT, createOUnitNewline());
            } else {
                reportError(FATAL_INTERNAL, "Invalid pushType in _expandPushTree");
            }
        }
        break;

    case LABT_DECL_INFO:
        pushPrintStack(fileNum, DECL_INFO, label->repr.dinfo);
        break;

    case LABT_DECL_LIST:
        initExpandPushList(fileNum, DECL_INFO, label->repr.declList);
        break;

    default:
        reportError(FATAL_INTERNAL, "default reached in _expandPushTree");
    }
}

pOUnit getNextCTreeOUnit(PrintStackType stack) {
    pOUnit oUnit;
    void *data;
    void *param;
    PrintType type, listType;
    int fileNum;

    currWorkStack = stack;
    do {
        if (!popPrintStack(&type, &listType, &data, &param, &fileNum)) {
            return NULL;
        }

        switch (type) {
            case OUNIT:   // Terminal symbol, "output unit"
                oUnit = data;
                oUnit->fileNum = fileNum;
                if (!oUnit->posPresent) {
                    pTokPos pos = &(currOUnit->pos);
                    memcpy(&(oUnit->pos), pos, sizeof *pos);
                    oUnit->pos.spacesBefore = 0;
                    oUnit->pos.linesBefore = 0;
                    oUnit->posPresent = 1;
                }
                if (oUnit->type == OUT_NEWLINE) {
                    oUnit->pos.colNum = MAX_COL_NUM;
                }
                return oUnit;

            case LIST:
                expandPushListParam(fileNum, listType, data, param);
                break;

            default:
            {
                ExpandFuncEntry entry =
                            (*g_tlang.printExpandTable)[type];

                assert(entry.expandFunc != NULL);
                if (entry.numParams == 0) {
                    entry.expandFunc(fileNum, data);
                } else {
                    entry.expandFuncParam(fileNum, data, param);
                }
                break;
            }
        }
    } while (1);
}

void pushPrintStackParam(int fileNum,
                            int type, void *data, void* param){
    pPT_StackElem newElem;
    PrintStackType stack = currWorkStack;
    int *pStackPos = &(printData[stack].stackPos);

    if (data == NULL) {
        return;
    }
    if (*pStackPos >= PT_STACK_MAX) {
        reportError(ERR_INTERNAL_STACK_OVERFLOW);
        return;
    }
    (*pStackPos)++;
    newElem = &(printData[stack].stack[*pStackPos]);
    newElem->type = type;
    newElem->fileNum = fileNum;
    newElem->data = data;
    newElem->param = param;
}

void pushPrintStackListParam(int fileNum,
                            int type, pSLList list, void* param) {
    pPT_StackElem newElem;
    PrintStackType stack = currWorkStack;
    int *pStackPos = &(printData[stack].stackPos);

    assert(list != NULL);
    if (*pStackPos >= PT_STACK_MAX) {
        reportError(ERR_INTERNAL_STACK_OVERFLOW);
        return;
    }
    (*pStackPos)++;
    newElem = &(printData[stack].stack[*pStackPos]);
    newElem->type = LIST;
    newElem->fileNum = fileNum;
    newElem->list.type = type;
    newElem->list.list = list;
    newElem->param = param;
}

int popPrintStack(PrintStackType *type,
                PrintType *listType, void **data, void**param, int *fileNum) {
    pPT_StackElem elem;
    PrintStackType stack = currWorkStack;
    int *pStackPos = &(printData[stack].stackPos);

    if (*pStackPos < 0) {
        *data = NULL;
        *param = 0;
        return 0;
    } else {
        elem = &(printData[stack].stack[*pStackPos]);
        *type = elem->type;
        if (elem->type == LIST) {
            *listType = elem->list.type;
            *data = elem->list.list;
        } else {
            *data = elem->data;
        }
        *fileNum = elem->fileNum;
        *param = elem->param;
        (*pStackPos)--;
        return 1;
    }
}

void initExpandPushListParam(int fileNum, PrintType type,
                                pSLList list, void *param) {
    if (list != NULL) {
        rewindCurrSLListPos(list);
        expandPushListParam(fileNum, type, list, param);
    }
}

void expandPushListParam(int fileNum,
                            PrintType type, pSLList list, void *param) {
    void *elem;

    if (getCurrSLListPosElem(list, &elem))
    {
        incCurrSLListPos(list);
        pushPrintStackListParam(fileNum, type, list, param);
        pushPrintStackParam(fileNum, type, elem, param);
    }
}

int cmpTokPos(pTokPos p1, pTokPos p2) {
    if (p1 == NULL) {
        if (p2 == NULL) {
            return 0;
        } else {
            return 1;
        }
    } else {
        if (p2 == NULL) {
            return -1;
        } else {
            // see below
        }
    }
    assert(p2 != NULL);
    if (p1->orderLineNum < p2->orderLineNum) {
        return -1;
    } else if (p1->orderLineNum == p2->orderLineNum) {
        if (p1->colNum < p2->colNum) {
            return -1;
        } else if (p1->colNum == p2->colNum) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}

pTokPos getDeclPos(pDeclInfo decl) {
    pTokPos pos;
    switch (decl->type) {
        case DIT_SCALAR:
            pos = decl->repr.scalar.scalarPos;
            break;
        case DIT_STRUCT_OR_UNION:
            pos = decl->repr.s->typePos;
            break;
        case DIT_ENUM:
            pos = decl->repr.e->enumPos;
            break;
        case DIT_NULL:
        default:
            assert(0);
    }
    if (cmpTokPos(pos, decl->prefixPos) > 0) {
        pos = decl->prefixPos;
    }
    assert(pos != NULL);
    return pos;
}

int paramListNotVoid(pDeclList list) {
    pDeclInfo decl;

    if (isEmptySLList(list)) {
        return 0;
    }
    rewindCurrSLListPos(list);
    getCurrSLListPosElem(list, &decl);
    if (decl->type == DIT_SCALAR) {
        if (decl->repr.scalar.scalar == SCL_DOT_DOT_DOT) {
            return 0;
        } else if (decl->repr.scalar.scalar == SCL_VOID) {
            if (!isDclrPtr(decl->dclr)) {
                return 0;
            }
        }
    }
    return 1;
}

static int _findMinOUnit(void) {
    int minPrintType = 0;
    int i;

    for (i = 1; i < NUM_OF_PRINT_TYPES; i++) {
        if (printData[i].nextOUnit == NULL) {
            continue;
        } else if (printData[minPrintType].nextOUnit == NULL) {
            minPrintType = i;
        } else if (printData[i].nextOUnit->fileNum == SECONDARY_FILE &&
                   printData[minPrintType].nextOUnit->fileNum == MAIN_FILE)
        {
            minPrintType = i;
        } else if (cmpTokPos(&(printData[minPrintType].nextOUnit->pos),
                             &(printData[i].nextOUnit->pos)) > 0)
        {
            minPrintType = i;
        }
    }
    return minPrintType;
}

static int _advanceNextOUnit(void) {
    PrintType min;
    int pushFiles;
    pOUnit nextOUnit;
    static unsigned long counter = 0;

    if (counter++ % 1024 == 0) {
        dribble();
    }
    min = _findMinOUnit();
    nextOUnit = printData[min].nextOUnit;
    if (nextOUnit != NULL) {
        pushFiles = nextOUnit->pos.fileLevel - currOUnit->pos.fileLevel;
        assert(pushFiles <= 1);
        if (pushFiles == 1) {
            _pushFileStack(nextOUnit->pos.fileName);
        } else if (pushFiles < 0) {
            while (pushFiles < 0) {
                _popFileStack();
                pushFiles++;
            }
        }
        linesLeftToSkip = nextOUnit->pos.linesBefore;
        if (currOUnit->type == OUT_NEWLINE) {
            if (linesLeftToSkip > 0) {
                linesLeftToSkip--;
            }
        }
        zapOUnit(currOUnit);
        currOUnit = nextOUnit;
        printData[min].nextOUnit = getNextCTreeOUnit(min);
        return 1;
    } else {
        return 0;
    }
}

void initPrinting(char *fname) {
    int i;

    currOUnit = createOUnitText("", NULL);
    setTokPos(&(currOUnit->pos), fname, 0, 1, 1, 0, 0, 0);

    _pushFileStack(currOUnit->pos.fileName);

    for (i = 0; i < NUM_OF_PRINT_TYPES; i++) {
        printData[i].stackPos = -1;
    }
}

static void initKeywordsTable(void) {
    int i;
    char **table = g_tlang.keywordsTable;
    keywordsTable = createHTable(512, (pHashFunc)stringiHashFunc, (pHashElemCmp)stricmp);
    for (i = 0; table[i] != NULL; i++) {
        addHTableElem(keywordsTable, table[i]);
    }
}

void initOutputSystem(void) {
    if (g_opt.targetLang == TLT_FORTRAN) {
        g_tlang.numOfOutFiles = 2;
        g_tlang.extension[0] = "fi";
        g_tlang.extension[1] = "fap";
        g_tlang.commentChar = '!';
        g_tlang.printCTreeTable = printCTreeFortTable;
        g_tlang.printExpandTable = printExpandFortTable;
        g_tlang.outputCurrLine = _outputCurrFortLine;
        g_tlang.keywordsTable = fortKeywordsTable;
        g_tlang.lfInitFlags = LF_INIT_FORT_FLAGS;
        g_tlang.substractFromLineLen = 0;
    } else if (g_opt.targetLang == TLT_ASM) {
        g_tlang.numOfOutFiles = 1;
        g_tlang.extension[0] = "inc";
        g_tlang.commentChar = ';';
        g_tlang.printCTreeTable = printCTreeAsmTable;
        g_tlang.printExpandTable = printExpandAsmTable;
        g_tlang.keywordsTable = asmKeywordsTable;
        g_tlang.outputCurrLine = _outputCurrAsmLine;
        g_tlang.lfInitFlags = LF_INIT_ASM_FLAGS;
        g_tlang.substractFromLineLen = 1;
    } else assert(0);
    initKeywordsTable();
}

void zapOutputSystem(void) {
    zapHTable(keywordsTable, NULL);
}

void zapPrinting(void) {
    int i;
    for (i = 0; i < NUM_OF_PRINT_TYPES; i++) {
        assert(printData[i].stackPos == -1);
    }
    _popAllFiles();
    zapOUnit(currOUnit);
}

void printAll(pDeclList codeList, pSLList dirList,
              pSLList commentList, pLogList logList){
    int fileNum;
    int i;

    currWorkStack = PT_CODE;
    initExpandPushList(MAIN_FILE, DECL_INFO, codeList);
    currWorkStack = PT_PREPROCESSOR;
    initExpandPushList(MAIN_FILE, TREE,      dirList);
    currWorkStack = PT_COMMENT;
    initExpandPushList(MAIN_FILE, TOKEN,     commentList);
    currWorkStack = PT_LOG;
    initExpandPushList(MAIN_FILE, LOG_ENTRY, logList);

    for (i = 0; i < NUM_OF_PRINT_TYPES; i++) {
        printData[i].nextOUnit = getNextCTreeOUnit(i);
    }

    while (_advanceNextOUnit()) {
        fileNum = currOUnit->fileNum;
        while (linesLeftToSkip > 0) {
            linesLeftToSkip--;
            _outputContinueLine(fileNum);
        }

        _outputOUnit(currOUnit);
    }

//    _popFileStack();
}

void outputZapAll(pDeclList declList) {
    printAll(declList, g_dirList, g_commentList, g_logList);
    zapSLList(declList, zapDeclInfo);  //  declList = createSLList();
    zapSLList(g_dirList, zapCTree);  g_dirList = createSLList();
    zapSLList(g_commentList, zapToken);  g_commentList = createSLList();
    zapSLList(g_logList, zapToken);  g_logList = createSLList();
}

void outputZapEOF(pToken eofToken) {
    addSLListElem(g_logList, eofToken); // !! This is a band-aid.
    outputZapAll(NULL);
}

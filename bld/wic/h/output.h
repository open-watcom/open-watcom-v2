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


/*------------------------------------------------------*/

typedef enum {
    OUT_TEXT,
    OUT_NEWLINE,
    OUT_COMMENT,
    OUT_EOF
} OUnitType;

#define _OUNIT_STR_STACK_MAX    5
struct OUnit {
    OUnitType type;
    int fileNum;
    char *preStrStack[_OUNIT_STR_STACK_MAX];
    int preStackLevel;
    char string[MAX_TOKEN_SIZE];
    int stringIsId;
    char *postStrStack[_OUNIT_STR_STACK_MAX];
    int postStackLevel;

    TokPos pos;
    // This is faster than having to do malloc and free for pTokPos
    // millions of times.  However, care must be taken because the
    // not all of fields of pos are used and thus init function for
    // pTokPos is not called
    int posPresent;
};

pOUnit createOUnitText(char *string, pTokPos pos);
pOUnit createOUnitTextId(char *string, pTokPos pos);
pOUnit createOUnitNewline(void);
pOUnit createOUnitFromTok(pToken tok);
pOUnit addOUnitPrefix(pOUnit unit, char *preString);
pOUnit addOUnitPostfix(pOUnit unit, char *postString);
pOUnit addOUnitPrePost(pOUnit unit, char *preString, char* postString);
pOUnit addOUnitPostfixImmediate(pOUnit unit, char *postString);
pOUnit createOUnitDclrName(char *name, pTokPos typePos, char *postfix);
void zapOUnit(pOUnit unit);


/*------------------------------------------------------*/

#define MAIN_FILE               0
#define SECONDARY_FILE          1
#define MAX_NUM_OF_OUT_FILES    2

typedef enum {
    OUNIT,
    LIST,         // All lists, one tag
    TOKEN,
    TREE,
    DECL_PREFIX,
    PRAGMA_DECL_INFO,
    DECL_INFO,
    DECL_STRUCT_INFO,
    UNION_ELEM,
    DECL_ENUM,
    ENUM_ELEM,
    DCLR,
    ARRAY,
    LOG_ENTRY,
    MAX_PRINT_TYPE
} PrintType;

typedef enum {
    PT_CODE = 0,
    PT_PREPROCESSOR,
    PT_COMMENT,
    PT_LOG,

    NUM_OF_PRINT_TYPES
} PrintStackType;

void _outputContinueLine(int fileNum);
void _outputOUnit(pOUnit unit);
void _popFileStack(void);
void _pushFileStack(char *fileName);
void _popAllFiles(void);

/*========================================================================*/


typedef struct {
    int num;  /* Number of terminals and non-terminals */
    char *printList; /* Array of tokens to use: n :=
                        '0'..'9' for terminal,
                        'L' for left subtree,
                        'R' for right subtree,
                        'a'..'z' for extraStrings[n-'a']
                        'N' for 'END OF STATEMENT' (statement separator) newline
                      */
    char **extraStrings;   /* Additional strings for which token does not exist*/
} PrintCTreeTable, *pPrintCTreeTable;

typedef void ExpandFunc (int fileNum, void *data);
typedef ExpandFunc *pExpandFunc;
typedef void ExpandFuncParam (int fileNum, void *data, void *param);
typedef ExpandFuncParam *pExpandFuncParam;
typedef struct {
    int numParams;
    union {
        pExpandFunc  expandFunc;
        pExpandFuncParam  expandFuncParam;
    };
} ExpandFuncEntry;

typedef ExpandFuncEntry ExpandFuncTable[MAX_PRINT_TYPE];
typedef ExpandFuncTable *pExpandFuncTable;

void expandPushTree(int fileNum, void *tree);

typedef struct {
    int numOfOutFiles;
    char commentChar;
    char *extension[MAX_NUM_OF_OUT_FILES];
    pPrintCTreeTable printCTreeTable;
    pExpandFuncTable printExpandTable;
    char **keywordsTable;
    void (*outputCurrLine)(int fnum, int continueToNextLine);
    int lfInitFlags;
    int substractFromLineLen;
} TargetLangOpt;

extern TargetLangOpt g_tlang;  // Output (target) language options, set in initOutputSystem()

void pushPrintStackParam(int fileNum,
                            int type, void *data, void* param);
void pushPrintStackListParam(int fileNum,
                            int type, pSLList list, void* param);
#define pushPrintStack(fileNum, type, data)  \
            pushPrintStackParam(fileNum, type, data, NULL)
#define pushPrintStackList(fileNum, type, list)  \
            pushPrintStackParam(fileNum, type, list, NULL)
void initExpandPushListParam(int fileNum, PrintType type,
                                pSLList list, void *param);
#define initExpandPushList(fileNum, type, list) \
            initExpandPushListParam(fileNum, type, list, NULL)

void expandPushListParam(int fileNum,
                            PrintType type, pSLList list, void *param);
pTokPos getDeclPos(pDeclInfo decl);
int paramListNotVoid(pDeclList list);

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


typedef enum {
    LABT_LIST,
    LABT_TOKEN,
    LABT_CONSTRUCT_ROOT,
    LABT_DECL_INFO,
    LABT_DECL_LIST
} LabelType;

typedef enum {
    LABCT_ADDR_OF_VALUE,
    LABCT_AND,
    LABCT_AND_AND,
    LABCT_ARROW,
    LABCT_CALL,
    LABCT_CAST_EXPR,
    LABCT_COLON,
    LABCT_DEFINED,
    LABCT_DIVIDE,
    LABCT_DOT,
    LABCT_EQ,
    LABCT_EXCLAMATION,
    LABCT_EXPR_LIST,
    LABCT_EXPR_SEPARATOR,
    LABCT_GE,
    LABCT_GT,
    LABCT_INDEX,
    LABCT_LE,
    LABCT_LSHIFT,
    LABCT_LT,
    LABCT_MINUS,
    LABCT_NE,
    LABCT_OR,
    LABCT_OR_OR,
    LABCT_PAREN_EXPR,
    LABCT_PERCENT,
    LABCT_PLUS,
    LABCT_PRE_DEFINE_MACRO,
    LABCT_PRE_ELIF,
    LABCT_PRE_ELSE,
    LABCT_PRE_ENDIF,
    LABCT_PRE_IF,
    LABCT_PRE_IFDEF,
    LABCT_PRE_IFNDEF,
    LABCT_PRE_INCLUDE,
    LABCT_PRE_DIR_LIST,
    LABCT_QUESTION,
    LABCT_RSHIFT,
    LABCT_SIZEOF_EXPR,
    LABCT_SIZEOF_TYPE,
    LABCT_STRINGS,
    LABCT_TILDE,
    LABCT_TIMES,
    LABCT_UNARY_MINUS,
    LABCT_UNARY_PLUS,
    LABCT_VALUE_AT_ADDR,
    LABCT_XOR,

    LABCT_MAX
} LabelConstrType;

typedef struct {
    LabelType type;
    union {
        struct { /* Construct in case type=LABT_CONSTRUCT_ROOT*/
            LabelConstrType type: 8;
            int8 numTokens;
            pToken *tokens;
        } constr;
        pSLList list; // List of tokens
        pToken token;
        pDeclInfo dinfo;
        pDeclList declList;
        void *data;
    } repr;
} Label, *pLabel;

pLabel createConstr0Label(LabelConstrType type);
pLabel createConstr1Label(LabelConstrType type, pToken t0);
pLabel createConstr2Label(LabelConstrType type, pToken t0, pToken t1);
pLabel createConstr3Label(LabelConstrType type, pToken t0, pToken t1,
                          pToken t2);
pLabel createConstr4Label(LabelConstrType type, pToken t0, pToken t1,
                          pToken t2, pToken t3);
pLabel createTokenLabel(pToken token);
pLabel createListLabel(pSLList list);
pLabel createDeclInfoLabel(pDeclInfo dinfo);
pLabel createDeclListLabel(pDeclList declList);

typedef struct _CTree {
    pLabel label;
    struct _CTree *child1; /* Left child */
    struct _CTree *child2;  /* Right child */
} CTree, *pCTree;

pCTree createCTreeRoot(pLabel label); /* Create a single node */
pCTree createNULLCTree(void);
pCTree createCTree1(pLabel label, pCTree child); /* Create Unary tree */
pCTree createCTree2(pLabel label, pCTree child1, pCTree child2); /* Create binary tree */
pLabel getCTreeLabel(pCTree tree);
pLabel getCTreeChild1Label(pCTree tree);
pLabel getCTreeChild2Label(pCTree tree);
pCTree getCTreeChild1(pCTree tree);
pCTree getCTreeChild2(pCTree tree);

void deleteLabel(pLabel label);
void deleteCTree(pCTree tree);

void printCTree(pCTree tree);


typedef union {
    pOUnit oUnit;
    pToken token;
    pCTree tree;
    pLabel label;
    pSLList tokenList;
    pDeclInfo dinfo;
    pDeclTree declList;
    pDeclTreeElem declTreeElem;
    pDeclStructInfo dsinfo;
    pDeclStructBody dsbody;
    pEnumList enumList;
    pEnumElem enumElem;
    pDeclEnum declEnum;
    pDclr dclr;
    pDclrList dclrList;
    pDclrPtr dclrPtr;
    pArrElem arrElem;
    int flag;

    void* data;
} ParseUnion;
//=========================================================================
// Declaration Information
//=========================================================================

/* types used for collecting decl-specifiers */

/* only one of these may be specified */
typedef enum {
    STG_NULL            = 0,
    STG_TYPEDEF,
    STG_REGISTER,
    STG_AUTO,
    STG_EXTERN,
    STG_STATIC,
    STG_MAX
} StgClass;

/* any one of these may be specified */
typedef enum {
    STY_CONST           = 0x01,
    STY_VOLATILE        = 0x02,
    STY_NULL            = 0x00
} TypeQualifier;

/* only certain combinations may be specified */
typedef enum {
    STM_NULL            = 0x0000,
    STM_CHAR            = 0x0001,
    STM_INT             = 0x0002,
    STM_LONG            = 0x0004,
    STM_FLOAT           = 0x0008,
    STM_DOUBLE          = 0x0010,
    STM_SIGNED          = 0x0020,
    STM_UNSIGNED        = 0x0040,
    STM_SHORT           = 0x0080,
    STM_VOID            = 0x0100,
    STM_DOT_DOT_DOT     = 0x0200      // Used exclusively for function args
} YScalarType;

typedef enum {
    DIT_NULL            = 0,
    DIT_SCALAR,                 // Things like int, uint and alike
    DIT_STRUCT_OR_UNION,
    DIT_ENUM
} DeclInfoType;

// Scalar types
typedef enum {
    SCL_NULL            = 0x00,
    SCL_CHAR,
    SCL_SCHAR,
    SCL_UCHAR,
    SCL_WCHAR,            // extension: long char
    SCL_SSHORT,
    SCL_USHORT,
    SCL_SINT,
    SCL_UINT,
    SCL_SLONG,
    SCL_ULONG,
    SCL_FLOAT,
    SCL_DOUBLE,
    SCL_LDOUBLE,        // long double
    SCL_VOID,
    SCL_DOT_DOT_DOT,
    SCL_MAX
} ScalarType;

typedef enum {
    MT_NULL = 0,
    MT_NEAR,
    MT_FAR,
    MT_FAR16,
    MT_HUGE,
    MT_MAX
} MemType;

pDclrList createDclrList(pDclr dclr);
pDclrList addDclrList(pDclrList list, pDclr elem);

struct DeclInfo {
    pToken begPunct;  // Punctuation mark at the beginning of a declarator
    StgClass storage;
    TypeQualifier qualifier;

    pTokPos prefixPos;

    DeclInfoType type: 8;
    union {
        struct {        // DIT_SCALAR
            YScalarType scalarCombo;
            ScalarType scalar;
            pTokPos scalarPos;
        } scalar;

        pDeclEnum e;                    // DIT_ENUM

        pDeclStructInfo s;              // DIT_STRUCT_OR_UNION
    } repr;
    pDclrList dclrList;  // List of declarators (optional)
    pDclr dclr;  // Declarator that may come from defined type
};

int   isExternDecl(pDeclInfo decl);

pDeclList createDeclList(pDeclInfo elem);
pDeclList addDeclList(pDeclList list, pDeclInfo elem);

pDeclInfo createDeclInfoENUM(pDeclEnum e);
pDeclInfo createDeclInfoSTRUCT(pDeclStructInfo s);
pDeclInfo createDeclInfoSCALAR(YScalarType tokCode, pTokPos pos);
void initNoTypeDecl(pDeclInfo decl);
pDeclInfo createStgClassDeclInfo(StgClass stgClass, pToken pos);
pDeclInfo createQualifierDeclInfo(TypeQualifier qualifier, pTokPos pos);
pDeclInfo combine2DeclInfo(pDeclInfo d1, pDeclInfo d2);
pDeclInfo addDeclInfoDclrList(pDeclInfo decl, pDclrList list);
pDeclInfo setDeclDclr(pDeclInfo decl, pDclr dclr);
pDeclInfo addDeclDclr(pDeclInfo decl, pDclr dclr);
pDeclInfo addDeclPunct(pDeclInfo decl, pToken begPunct);

typedef enum {
    DSIT_STRUCT,
    DSIT_UNION
} DeclStructInfoType;

struct DeclStructInfo {
    DeclStructInfoType type;
    pTokPos typePos;
    pToken name;                // Name (optional) of the structure
    pDeclStructBody body;       // Body (optional) of the structure
};
pDeclStructInfo createDeclStructInfo(pToken typeTok,
                                     pToken name, pDeclStructBody body);

struct DeclStructBody {
    pDeclList declList;
    pTokPos endPos;
};

pDeclStructBody createDeclStructBody(pDeclList list, pTokPos endPos);

struct ArrElem {
    pToken lBracket;
    pCTree constExpr;
    pToken rBracket;
};

struct Dclr {
    MemType memType;
    pTokPos memPos;

    pToken pragmaMod;

    pDclrPtr ptr;

    pToken id;

    pArrList arrList;

    pToken argBegin;  // '('
    pDeclList args;
    pToken argEnd;  // ')'

    pToken equalTok;
    pCTree initializer;
};

pDclr createDclr(pToken id);
int   isDclrPtr(pDclr dclr);  // Returns true iff dclr is a pointer
int   isDclrFunc(pDclr dclr); // Returns true iff dclr is a function
char *getDclrName(pDclr dclr);
pDclr addDclrPragmaModifier(pDclr dclr, pToken pragmaTok);
pDclr addDclrMemModifier(pDclr dclr, pToken memTok);
pDclr addDclrPtrModifier(pDclr dclr, pDclrPtr ptr);
pDclr addDclrArray(pDclr dclr, pToken begin, pCTree constExpr, pToken end);
pDclr addDclrFuncArgs(pDclr dclr, pToken lParen, pDeclList args, pToken rParen);
pDclr addDclrInitializer(pDclr dclr, pToken equalTok, pCTree initializer);
pDclr combine2Dclr(pDclr dclr1, pDclr dclr2);

struct DclrPtr {
    MemType memType;
    TypeQualifier qualifiers;
    pTokPos pos;
};
pDclrPtr createDclrPtr(pToken yTimes, TypeQualifier qualifiers);

/*--------*/

struct DeclEnum {
    pTokPos enumPos;            // enum
//    pToken name;              // optional
    pEnumList list;             // optional
};
pDeclEnum createDeclEnum(pToken enumPos, pEnumList list);

pEnumList createEnumList(pEnumElem elem);
pEnumList addEnumList(pEnumList list,  pEnumElem elem);
pEnumList finishEnumListCreation(pEnumList list);

struct EnumElem {
    char   begPunctExists;  // comma exists before this element
    pToken name;
    pToken equal;
    pCTree expression;
};

pEnumElem createEnumElem(pToken name, pToken equal, pCTree expression);
pCTree createEnumFromDefine(pToken defTok, pToken id, pCTree expr);
pEnumElem addEnumElemBegPunct(pEnumElem elem);

/*--------*/

void logMessage(char *str);

/*----------- Duplicate functions -------------*/
pCTree dupCTree(pCTree elem, pTokPos pos);
pLabel dupLabel(pLabel elem, pTokPos pos);
pDeclInfo dupDeclInfo(pDeclInfo elem, pTokPos pos);
pDclr dupDclr(pDclr elem, pTokPos pos);
pDeclStructInfo dupDeclStructInfo(pDeclStructInfo elem, pTokPos pos);
pDeclStructBody dupDeclStructBody(pDeclStructBody elem, pTokPos pos);
pArrElem dupArrElem(pArrElem elem, pTokPos pos);
pDclrPtr dupDclrPtr(pDclrPtr elem, pTokPos pos);
pDeclEnum dupDeclEnum(pDeclEnum elem, pTokPos pos);
pEnumElem dupEnumElem(pEnumElem, pTokPos pos);
pToken dupToken(pToken elem, pTokPos pos);
pTokPos dupTokPos(pTokPos elem, pTokPos pos);

/*---------------- Zap functions ---------------------------------*/
void zapCTree(void *elem);
void zapLabel(pLabel elem);
void zapDeclInfo(void *elem);
void zapDclr(void *elem);
void zapDeclStructInfo(pDeclStructInfo elem);
void zapDeclStructBody(pDeclStructBody elem);
void zapArrElem(pArrElem elem);
void zapDclrPtr(pDclrPtr elem);
void zapDeclEnum(pDeclEnum elem);
void zapEnumElem(pEnumElem elem);
void zapToken(void *elem);
void zapTokens2(pToken elem1, pToken elem2);
void zapTokens3(pToken elem1, pToken elem2, pToken elem3);
void zapTokPos(pTokPos elem);

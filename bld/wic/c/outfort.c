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


#include "wic.h"
#include "output.h"
#include "outfort.h"

#define UNARY_OP_LABEL {2, {"0L"}, NULL}
#define BINARY_OP_LABEL {3, {"L0R"}, NULL}

static char *_fortStrADDR_OF_VALUE[] = { "LOC(", ")" };
static char *_fortStrAND_AND[] = { "(", ".NE.0)", "(", ".NE.0)" };
static char *_fortStrLSHIFT[] = { "ISHL(", ",", ")" };
static char *_fortStrRSHIFT[] = { "ISHL(", ", -", ")" };
static char *_fortStrOR_OR[] = { "(", ".NE.0)", "(", ".NE.0)" };
static char *_fortStrPERCENT[] = { "MOD(", ",", ")" };
static char *_fortStrSIZEOF_EXPR[] = { "(", ")" };
PrintCTreeTable printCTreeFortTable[LABCT_MAX] = {
/* ADDR_OF_VALUE */     {3, {"aLb"}, _fortStrADDR_OF_VALUE },
/* AND */               BINARY_OP_LABEL,
/* AND_AND */           {7, {"aLb0cRd"}, _fortStrAND_AND },
/* ARROW */             BINARY_OP_LABEL,
/* CALL */              {4,  {"L0R1"}, NULL},
/* CAST_EXPR */         {1,  {"R"}, NULL},
/* COLON */             BINARY_OP_LABEL,
/* DEFINED */           {4,  {"0123"}, NULL},
/* DIVIDE */            BINARY_OP_LABEL,
/* DOT */               BINARY_OP_LABEL,
/* EQ */                BINARY_OP_LABEL,
/* EXCLAMATION */       UNARY_OP_LABEL,
/* EXPR_LIST */         BINARY_OP_LABEL,
/* EXPR_SEPARATOR */    BINARY_OP_LABEL,
/* GE */                BINARY_OP_LABEL,
/* GT */                BINARY_OP_LABEL,
/* INDEX */             {4,  {"L0R1"}, NULL},
/* LE */                BINARY_OP_LABEL,
/* LSHIFT */            {5,   {"aLbRc"},  _fortStrLSHIFT},
/* LT */                BINARY_OP_LABEL,
/* MINUS */             BINARY_OP_LABEL,
/* NE */                BINARY_OP_LABEL,
/* OR */                BINARY_OP_LABEL,
/* OR_OR */             {7, {"aLb0cRd"}, _fortStrOR_OR },
/* PAREN_EXPR */        {3,  {"0L1"}, NULL},
/* PERCENT */           {5,   {"aLbRc"},  _fortStrPERCENT},
/* PLUS */              BINARY_OP_LABEL,
/* PRE_DEFINE_MACRO */  {4,  {"0LRN"}, NULL},
/* PRE_ELIF */          {3,  {"0LN"}, NULL},
/* PRE_ELSE */          {2,  {"0N"}, NULL},
/* PRE_ENDIF */         {2,  {"0N"}, NULL},
/* PRE_IF */            {3,  {"0LN"}, NULL},
/* PRE_IFDEF */         {3,  {"01N"}, NULL},
/* PRE_IFNDEF */        {3,  {"01N"}, NULL},
/* PRE_INCLUDE */       {3,  {"01N"}, NULL},
/* PRE_DIR_LIST*/       {3,  {"0LN"}, NULL},
/* QUESTION */          BINARY_OP_LABEL,
/* RSHIFT */            {5,   {"aLbRc"},  _fortStrRSHIFT},
/* SIZEOF_EXPR */       {4,  {"0aLb"}, _fortStrSIZEOF_EXPR},
/* SIZEOF_TYPE */       {4,  {"01L2"}, NULL},
/* STRINGS */           {2,  {"LR"}, NULL},
/* TILDE */             UNARY_OP_LABEL,
/* TIMES */             BINARY_OP_LABEL,
/* UNARY_MINUS */       UNARY_OP_LABEL,
/* UNARY_PLUS */        UNARY_OP_LABEL,
/* VALUE_AT_ADDR */     UNARY_OP_LABEL,
/* XOR */               BINARY_OP_LABEL
};


#define int1Str         "INTEGER*1"
#define int2Str         "INTEGER*2"
#define int4Str         "INTEGER"
#define int6Str         "INTEGER*6"

#if 0
static void _expandPushTypeQualifier(int fileNum, pDeclInfo dinfo) {
    if (dinfo->qualifier & STY_CONST) {
        pushPrintStack( fileNum, OUNIT,
                     createOUnitText("CONST", dinfo->constPos) );
    }
    if (dinfo->qualifier & STY_VOLATILE) {
        pushPrintStack( fileNum, OUNIT,
                     createOUnitText("VOLATILE", dinfo->volatilePos) );
    }
}
#endif

#if 0
static void _expandPushTypeStorage(int fileNum, pDeclInfo dinfo) {
    static char *printString[STG_MAX] = {
        "",                 // Null is allowed
        "TYPEDEF",
        "REGISTER",
        "AUTO",
        "EXTERN",
        "STATIC"
    };

    assert((dinfo->storage <= STG_MAX) && (dinfo->storage >= STG_NULL));
    assert(dinfo->storage != STG_TYPEDEF);
    if (dinfo->storage != STG_NULL) {
        pushPrintStack( fileNum, OUNIT,
            createOUnitText(printString[dinfo->storage], dinfo->prefixPos));
    }
}
#endif

static DeclInfoType getSimpleIntType(pDeclInfo decl, int *size, char **s ) {

    struct StringSize {
        char *string;
        int size;
    };
    static struct StringSize fortranSclType[SCL_MAX][SIZE_MAX] = {
                    /* SIZE_16      SIZE_32         SIZE_48*/
    /*NULL*/    {"NULL", 0,     "NULL", 0,      "NULL", 0 },
    /*CHAR*/    {int1Str, 1,    int1Str, 1,     int1Str, 1},
    /*SCHAR*/   {int1Str, 1,    int1Str, 1,     int1Str, 1},
    /*UCHAR*/   {int1Str, 1,    int1Str, 1,     int1Str, 1},
    /*WCHAR*/   {int4Str, 4,    int4Str, 4,     int4Str, 4},
    /*SSHORT*/  {int2Str, 2,    int2Str, 2,     int2Str, 2},
    /*USHORT*/  {int2Str, 2,    int2Str, 2,     int2Str, 2},
    /*SINT*/    {int2Str, 2,    int4Str, 4,     int4Str, 4},
    /*UINT*/    {int2Str, 2,    int4Str, 4,     int4Str, 4},
    /*SLONG*/   {int4Str, 4,    int4Str, 4,     int6Str, 6},
    /*ULONG*/   {int4Str, 4,    int4Str, 4,     int6Str, 6},
    /*FLOAT*/   {"REAL*4", 4,   "REAL*4", 4,    "REAL*4", 4 },
    /*DOUBLE*/  {"REAL*8", 8,   "REAL*8", 8,    "REAL*8", 8 },
    /*LDOUBLE*/ {"REAL*8", 8,   "REAL*8", 8,    "REAL*8", 8 },
    /*VOID*/    {"VOID",   0,   "VOID", 0,      "VOID", 0 },
    /*DOT_DOT_DOT*/     {"...",    0,   "...", 0,       "...", 0 }
    };
    static struct StringSize ptrStr[SIZE_MAX] =
        { int2Str, 2,  int4Str, 4, int6Str, 6 };

    pDclr dclr = decl->dclr;
    DeclInfoType retVal = DIT_SCALAR;


    if (isDclrPtr(dclr)) {
        SizeType temp;
        switch (dclr->ptr->memType) {
            case MT_NULL: temp = g_opt.ptrSize; break;
            case MT_NEAR: temp = g_opt.nearPtrSize; break;
            case MT_FAR: temp = g_opt.farPtrSize; break;
            case MT_FAR16: temp = g_opt.farPtrSize; break;
            case MT_HUGE: temp = g_opt.hugePtrSize; break;
            default: assert(0);
        }
        *s = ptrStr[temp].string;
        *size = ptrStr[temp].size;
    } else {
        switch (decl->type) {
            case DIT_SCALAR:
                assert(decl->repr.scalar.scalar <= SCL_MAX);
                assert(decl->repr.scalar.scalar >= SCL_NULL);
                *s = fortranSclType[decl->repr.scalar.scalar][g_opt.intSize]
                            .string;
                *size = fortranSclType[decl->repr.scalar.scalar][g_opt.intSize]
                            .size;
                break;
            case DIT_STRUCT_OR_UNION:
                *size = 0;
                *s = NULL;
                retVal = DIT_STRUCT_OR_UNION;
                break;
            case DIT_ENUM:
                *size = fortranSclType[SCL_SINT][g_opt.intSize].size;
                *s = NULL;
                retVal = DIT_ENUM;
                break;
            default:
                assert(0);
        }
    }
    return retVal;
}

static void _expandDeclPrefix(int fileNum, pDeclInfo decl) {
    pTokPos pos = getDeclPos(decl);
    char *s;
    int size;

    switch (getSimpleIntType(decl, &size, &s )) {
        case DIT_SCALAR:
            pushPrintStack( fileNum, OUNIT, createOUnitText(s, pos));
            break;
        case DIT_STRUCT_OR_UNION:
            pushPrintStack(fileNum, DECL_STRUCT_INFO, decl->repr.s);
            break;
        case DIT_ENUM:
            pushPrintStack(fileNum, DCLR, decl->dclr);
            pushPrintStack(fileNum, DECL_ENUM, decl->repr.e);
            break;
        default:
            assert(0);
    }
}

static pOUnit getMemModelOUnit(pDclr dclr) {
    if (dclr->memType != MT_NULL) {
        char *s;
        switch (dclr->memType) {
            case MT_NEAR: s = "NEAR"; break;
            case MT_FAR: s = "FAR"; break;
            case MT_FAR16: s = "FAR16"; break;
            case MT_HUGE: s = "HUGE"; break;
            default: assert(0);
        }
        return createOUnitText(s, dclr->memPos);
    } else {
        return NULL;
    }
}

static _expandPushPragmaDeclInfo(int fileNum, pDeclInfo decl) {
    pTokPos pos = getDeclPos(decl);
    int size;
    char *s;

    fileNum = fileNum;  // Dummy param
    getSimpleIntType(decl, &size, &s );
    switch (size) {
        case 0:
            if (decl->type == DIT_SCALAR) {
                if (decl->repr.scalar.scalar == SCL_DOT_DOT_DOT) {
                    return;
                }
            } else {
                s = "VALUE"; // If it is a structure or union
            }
            break;
        case 1: s = "VALUE*1"; break;
        case 2: s = "VALUE*2"; break;
        case 4: s = "VALUE*4"; break;
        case 6: s = "VALUE*6"; break;  // This is not supported
        case 8: s = "VALUE*8"; break;
        default: assert(0); s = NULL;
    }
    pushPrintStack( SECONDARY_FILE, OUNIT, createOUnitText(s, pos));
    pushPrintStack( SECONDARY_FILE, TOKEN, decl->begPunct);
}

static void _expandFuncPragma(pDeclInfo dinfo) {
    pDclr dclr = dinfo->dclr;

    if (dclr == NULL) {
        return;
    }
    if (!isDclrFunc(dclr) || dclr->id == NULL) {
        return;
    }

    pushPrintStack( SECONDARY_FILE, OUNIT, createOUnitNewline());

    pushPrintStack(SECONDARY_FILE, OUNIT, getMemModelOUnit(dclr));

    if (paramListNotVoid(dclr->args)) {
        pushPrintStack( SECONDARY_FILE, TOKEN, dclr->argEnd);
        initExpandPushList(SECONDARY_FILE, PRAGMA_DECL_INFO, dclr->args);
        pushPrintStack( SECONDARY_FILE, TOKEN, dclr->argBegin);
    }
    pushPrintStack( SECONDARY_FILE, OUNIT,
                     createOUnitText("parm", NULL) );


    pushPrintStack( SECONDARY_FILE, TOKEN, dclr->id);
    if (dclr->pragmaMod != NULL) {
        pushPrintStack( SECONDARY_FILE, OUNIT,
            addOUnitPrePost(createOUnitFromTok(dclr->pragmaMod), "(", ")"));
    }
    pushPrintStack( SECONDARY_FILE, OUNIT,
                     createOUnitText("*$PRAGMA aux", NULL) );

}


static void _expandPushDeclInfo(int fileNum, pDeclInfo dinfo, char *postfix) {
    pDclr dclr = dinfo->dclr;
    if (isExternDecl(dinfo) && !isDclrFunc(dclr)) {
        char *name = getDclrName(dclr);
        if (name == NULL) {
            name = "";
        }
        reportError(CERR_EXTERN_NOT_SUPPORTED, name);
    }
    _expandFuncPragma(dinfo);
    pushPrintStack(fileNum, OUNIT, createOUnitNewline());

    initExpandPushList(fileNum, DCLR, dinfo->dclrList); //Should be NULL
    pushPrintStackParam(fileNum, DCLR, dclr, postfix);

    if (isDclrFunc(dinfo->dclr)) {
        pushPrintStack(fileNum, OUNIT,
            createOUnitText("EXTERNAL", getDeclPos(dinfo)));
    } else {
        pushPrintStack(fileNum, DECL_PREFIX, dinfo);
//      _expandPushTypeQualifier(fileNum, dinfo);
//      _expandPushTypeStorage(fileNum, dinfo);
    }
}

static void _expandPushEnumElem(int fileNum, pEnumElem elem) {
    pOUnit nameOUnit;
    pOUnit paramOUnit;
    assert(elem->expression != NULL);
    pushPrintStack(fileNum, OUNIT, createOUnitText(")", NULL));
    pushPrintStack(fileNum, TREE, elem->expression);
    pushPrintStack(fileNum, OUNIT, createOUnitText("=", NULL));

    nameOUnit = createOUnitFromTok(elem->name);
    nameOUnit->pos.spacesBefore = 0;
    nameOUnit->pos.linesBefore = 0;
    paramOUnit = createOUnitText("PARAMETER (", elem->name->pos);
    paramOUnit->pos.linesBefore = 0;
    pushPrintStack(fileNum, OUNIT, nameOUnit);
    pushPrintStack(fileNum, OUNIT, paramOUnit);

    pushPrintStack(fileNum, OUNIT, createOUnitNewline());
    nameOUnit = createOUnitFromTok(elem->name);
    nameOUnit->pos.spacesBefore = 0;
    nameOUnit->pos.linesBefore = 0;
    pushPrintStack(fileNum, OUNIT, nameOUnit);
    pushPrintStack(fileNum, OUNIT,
        createOUnitText("INTEGER    ", elem->name->pos));
    if (elem->begPunctExists) {
        pushPrintStack(fileNum, OUNIT, createOUnitNewline());
    }
}

static void _expandPushDeclEnum(int fileNum,
                                pDeclEnum declEnum) {
    if (declEnum->list == NULL) {
        pushPrintStack( fileNum, OUNIT,
                createOUnitText("ENUM_INT", declEnum->enumPos ));
    } else {
        initExpandPushList(fileNum, ENUM_ELEM, declEnum->list);
    }
}

static void _expandPushUnionElem(int fileNum,
                                      pDeclInfo decl, char *postfix) {
    pTokPos pos = dupTokPos(getDeclPos(decl), NULL);

    pos->linesBefore = 0;
    pushPrintStack(fileNum, OUNIT, createOUnitNewline());
    pushPrintStack(fileNum, OUNIT, createOUnitText("    END MAP", pos));
    pushPrintStackParam(fileNum, DECL_INFO, decl, postfix);
    pushPrintStack(fileNum, OUNIT, createOUnitNewline());
    pushPrintStack(fileNum, OUNIT, createOUnitText("    MAP", pos));

    zapTokPos(pos);
}

static void _expandPushDeclStructInfo(int fileNum,
                                      pDeclStructInfo dsinfo) {
    pDeclStructBody body = dsinfo->body;
    char *name = getTokenIdName(dsinfo->name);

    if (body == NULL) {
        pushPrintStack(fileNum, OUNIT,
                addOUnitPrePost(createOUnitFromTok(dsinfo->name), "/", "/"));
        pushPrintStack(fileNum, OUNIT,
                createOUnitText("RECORD", dsinfo->typePos));
    } else {
        pushPrintStack(fileNum, OUNIT, createOUnitNewline());
        pushPrintStack(fileNum, OUNIT,
                createOUnitText("END STRUCTURE", body->endPos));
        if (dsinfo->type == DSIT_STRUCT) {
            initExpandPushListParam(fileNum, DECL_INFO, body->declList, name);
        } else if (dsinfo->type == DSIT_UNION) {
            pushPrintStack(fileNum, OUNIT, createOUnitNewline());
            pushPrintStack(fileNum, OUNIT,
                createOUnitText("END UNION", body->endPos));
            initExpandPushListParam(fileNum, UNION_ELEM, body->declList, name);
            pushPrintStack(fileNum, OUNIT, createOUnitNewline());
            pushPrintStack(fileNum, OUNIT,
                createOUnitText("UNION", body->endPos));
        } else {
            assert(0);
        }
        pushPrintStack(fileNum, OUNIT, createOUnitNewline());
        pushPrintStack(fileNum, OUNIT,
                addOUnitPrePost(createOUnitFromTok(dsinfo->name), "/", "/"));
        pushPrintStack(fileNum, OUNIT,
                createOUnitText("STRUCTURE", dsinfo->typePos));
    }
}

static void _expandPushDclr(int fileNum, pDclr dclr, char *postfix) {
    pToken name = dclr->id;
    if (dclr->initializer != NULL) {
        pushPrintStack(fileNum, OUNIT, createOUnitText("/", NULL));
        pushPrintStack(fileNum, TREE, dclr->initializer);
        pushPrintStack(fileNum, OUNIT, createOUnitText("/", NULL));
    }
    if (dclr->arrList != NULL) {
        rewindCurrSLListPos(dclr->arrList);
        initExpandPushList(fileNum, ARRAY, dclr->arrList);
    }
    if (name != NULL) {
        pushPrintStack(fileNum, OUNIT,
            createOUnitDclrName(getTokenIdName(name), name->pos, postfix));
    }

}

static void _expandPushArrElem(int fileNum, pArrElem elem) {
    pushPrintStack(fileNum, TOKEN, elem->rBracket);
    pushPrintStack(fileNum, TREE, elem->constExpr);
    pushPrintStack(fileNum, TOKEN, elem->lBracket);
}

static void _expandPushToken(int fileNum, pToken token) {
    pushPrintStack(fileNum, OUNIT, createOUnitFromTok(token));
}

static void _expandPushLogEntry(int fileNum, pToken logEntry) {
    _expandPushToken(fileNum, logEntry);
    pushPrintStack( fileNum, OUNIT, createOUnitNewline());
}

/*-----------------------------------------------------------------------*/

ExpandFuncTable printExpandFortTable =
{
//    Params    FuncName
        0,      NULL,                       //OUNIT
        0,      NULL,                       //LIST
        0,      _expandPushToken,           //TOKEN
        0,      expandPushTree,             //TREE
        0,      _expandDeclPrefix,          //DECL_PREFIX
        0,      _expandPushPragmaDeclInfo,  //PRAGMA_DECL_INFO
        1,      (pExpandFunc)_expandPushDeclInfo,        //DECL_INFO
        0,      _expandPushDeclStructInfo,  //DECL_STRUCT_INFO
        1,      (pExpandFunc)_expandPushUnionElem,       //UNION_ELEM
        0,      _expandPushDeclEnum,        //DECL_ENUM
        0,      _expandPushEnumElem,        //ENUM_ELEM
        1,      (pExpandFunc)_expandPushDclr,            //DCLR
        0,      _expandPushArrElem,         //ARRAY
        0,      _expandPushLogEntry,        //LOG_ENTRY
};

/*-----------------------------------------------------------------------*/

char *fortKeywordsTable[] = {
"ADMIT", "ALLOCATE", "ASSIGN", "ATEND", "BACKSPACE", "BLOCKDATA",
"CALL", "CASE", "CHARACTER", "CLOSE", "COMMON", "COMPLEX",
"CONTINUE", "CYCLE", "DATA", "DEALLOCATE", "DIMENSION", "DO", "WHILE",
"DOUBLECOMPLEX", "DOUBLEPRECISION", "ELSE", "ELSEIF", "END", "ENDATEND",
"ENDBLOCK", "ENDDO", "ENDFILE", "ENDGUESS", "ENDIF", "ENDLOOP",
"ENDMAP", "ENDSELECT", "ENDSTRUCTURE", "ENDUNION", "ENDWHILE", "ENTRY",
"EQUIVALENCE", "EXECUTE", "EXIT", "EXTENDEDCOMPLEX", "EXTENDEDPRECISION", "EXTERNAL",
"FORMAT", "FUNCTION", "GOTO", "GUESS", "IF", "IMPLICIT",
"INCLUDE", "INQUIRE", "INTEGER", "INTRINSIC", "LOGICAL", "LOOP",
"MAP", "NAMELIST", "OPEN", "OTHERWISE", "PARAMETER", "PAUSE",
"PRINT", "PROGRAM", "QUIT", "READ", "REAL", "RECORD",
"REMOTEBLOCK", "RETURN", "REWIND", "SAVE", "SELECT", "STOP",
"STRUCTURE", "SUBROUTINE", "UNION", "UNTIL", "WHILE", "WRITE",
"DO", "ABS", "ACOS", "AIMAG", "AINT", "ALGAMA",
"ALLOCATED", "ALOG", "ALOG10", "AMAX0", "AMAX1", "AMIN0",
"AMIN1", "AMOD", "ANINT", "ASIN", "ATAN", "ATAN2",
"BTEST", "CABS", "CCOS", "CDABS", "CDCOS", "CDEXP",
"CDLOG", "CDSIN", "CDSQRT", "CEXP", "CHAR", "CLOG",
"CMPLX", "CONJG", "COS", "COSH", "COTAN", "CQABS",
"CQCOS", "CQEXP", "CQLOG", "CQSIN", "CQSQRT", "CSIN",
"CSQRT", "DABS", "DACOS", "DASIN", "DATAN", "DATAN2",
"DBLE", "DCMPLX", "DCONJG", "DCOS", "DCOSH", "DCOTAN",
"DDIM", "DERF", "DERFC", "DEXP", "DFLOAT", "DGAMMA",
"DIM", "DIMAG", "DINT", "DLGAMA", "DLOG", "DLOG10",
"DMAX1", "DMIN1", "DMOD", "DNINT", "DPROD", "DREAL",
"DSIGN", "DSIN", "DSINH", "DSQRT", "DTAN", "DTANH",
"ERF", "ERFC", "EXP", "FLOAT", "GAMMA", "HFIX",
"I1ABS", "I1AND", "I1BCHNG", "I1BCLR", "I1BSET", "I1BTEST",
"I1DIM", "I1EOR", "I1MAX0", "I1MIN0", "I1MOD", "I1NOT",
"I1OR", "I1SHA", "I1SHC", "I1SHFT", "I1SHL", "I1SIGN",
"I2ABS", "I2AND", "I2BCHNG", "I2BCLR", "I2BSET", "I2BTEST",
"I2DIM", "I2EOR", "I2MAX0", "I2MIN0", "I2MOD", "I2NOT",
"I2OR", "I2SHA", "I2SHC", "I2SHFT", "I2SHL", "I2SIGN",
"IABS", "IAND", "IBCHNG", "IBCLR", "IBSET", "ICHAR",
"IDIM", "IDINT", "IDNINT", "IEOR", "IFIX", "IMAG",
"INDEX", "INT", "IOR", "IQINT", "IQNINT", "ISHA",
"ISHC", "ISHFT", "ISHL", "ISIGN", "ISIZEOF", "LEN",
"LENTRIM", "LGAMMA", "LGE", "LGT", "LLE", "LLT",
"LOC", "LOG", "LOG10", "MAX", "MAX0", "MAX1",
"MIN", "MIN0", "MIN1", "MOD", "NINT", "NOT",
"QABS", "QACOS", "QASIN", "QATAN", "QATAN2", "QCMPLX",
"QCONJG", "QCOS", "QCOSH", "QCOTAN", "QDIM", "QERF",
"QERFC", "QEXP", "QEXT", "QGAMMA", "QIMAG", "QINT",
"QLGAMA", "QLOG", "QLOG10", "QMAX1", "QMIN1", "QMOD",
"QNINT", "QPROD", "QSIGN", "QSIN", "QSINH", "QSQRT",
"QTAN", "QTANH", "REAL", "SIGN", "SIN", "SINH",
"SNGL", "SQRT", "TAN", "TANH", "VOLATILE", ".EQ.",
".NE.", ".LT.", ".GE.", ".LE.", ".GT.", ".EQV.",
".NEQV.", ".OR.", ".AND.", ".NOT.",
NULL
};

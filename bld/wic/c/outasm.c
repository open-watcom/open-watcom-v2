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
#include "outasm.h"

#define UNARY_OP_LABEL {2, {"0L"}, NULL}
#define BINARY_OP_LABEL {3, {"L0R"}, NULL}

static char *_asmStrADDR_OF_VALUE[] = { "OFFSET" };
static char *_asmStrAND_AND[] = { "(", "NE 0)", "(", " NE 0)" };
static char *_asmStrEXCLAMATION[] = { "(", "NE 0)" };
static char *_asmStrOR_OR[] = { "(", "NE 0)", "(", "NE 0)" };
static char *_asmStrPRE_DEFINE_MACRO[] = { "EQU" };
PrintCTreeTable printCTreeAsmTable[LABCT_MAX] = {
/* ADDR_OF_VALUE */     {2, {"aL"}, _asmStrADDR_OF_VALUE },
/* AND */               BINARY_OP_LABEL,
/* AND_AND */           {7, {"aLb0cRd"}, _asmStrAND_AND },
/* ARROW */             BINARY_OP_LABEL,
/* CALL */              {4,  {"L0R1"}, NULL},
/* CAST_EXPR */         {1,  {"R"}, NULL},
/* COLON */             BINARY_OP_LABEL,
/* DEFINED */           {4,  {"0123"}, NULL},
/* DIVIDE */            BINARY_OP_LABEL,
/* DOT */               BINARY_OP_LABEL,
/* EQ */                BINARY_OP_LABEL,
/* EXCLAMATION */       { 3, {"aLb"}, _asmStrEXCLAMATION },
/* EXPR_LIST */         BINARY_OP_LABEL,
/* EXPR_SEPARATOR */    BINARY_OP_LABEL,
/* GE */                BINARY_OP_LABEL,
/* GT */                BINARY_OP_LABEL,
/* INDEX */             {4,  {"L0R1"}, NULL},
/* LE */                BINARY_OP_LABEL,
/* LSHIFT */            BINARY_OP_LABEL,
/* LT */                BINARY_OP_LABEL,
/* MINUS */             BINARY_OP_LABEL,
/* NE */                BINARY_OP_LABEL,
/* OR */                BINARY_OP_LABEL,
/* OR_OR */             {7,  {"aLb0cRd"}, _asmStrOR_OR },
/* PAREN_EXPR */        {3,  {"0L1"}, NULL},
/* PERCENT */           BINARY_OP_LABEL,
/* PLUS */              BINARY_OP_LABEL,
/* PRE_DEFINE_MACRO */  {5,  {"0LaRN"}, _asmStrPRE_DEFINE_MACRO},
/* PRE_ELIF */          {3,  {"0LN"}, NULL},
/* PRE_ELSE */          {2,  {"0N"}, NULL},
/* PRE_ENDIF */         {2,  {"0N"}, NULL},
/* PRE_IF */            {3,  {"0LN"}, NULL},
/* PRE_IFDEF */         {3,  {"01N"}, NULL},
/* PRE_IFNDEF */        {3,  {"01N"}, NULL},
/* PRE_INCLUDE */       {3,  {"01N"}, NULL},
/* PRE_DIR_LIST*/       {3,  {"0LN"}, NULL},
/* QUESTION */          BINARY_OP_LABEL,
/* RSHIFT */            BINARY_OP_LABEL,
/* SIZEOF_EXPR */       UNARY_OP_LABEL,
/* SIZEOF_TYPE */       UNARY_OP_LABEL,
/* STRINGS */           {2,  {"LR"}, NULL},
/* TILDE */             UNARY_OP_LABEL,
/* TIMES */             BINARY_OP_LABEL,
/* UNARY_MINUS */       UNARY_OP_LABEL,
/* UNARY_PLUS */        UNARY_OP_LABEL,
/* VALUE_AT_ADDR */     UNARY_OP_LABEL,
/* XOR */               BINARY_OP_LABEL
};

// Declaration types
typedef enum {
    DT_PTR            = 1,
    DT_FUNC           = 2,
    DT_SIMPLE         = 4,
    DT_STRUCT_DEF     = 8,
    DT_STRUCT_VAR     = 16,
    DT_ENUM_DEF       = 32,
    DT_ENUM_VAR       = 64,
    DT_INVALID        = 128
}  DeclType;

static DeclType getDeclType(pDeclInfo decl) {
    pDclr dclr;
    assert(decl != NULL);
    dclr = decl->dclr;

    if (isDclrFunc(dclr)) {
        return DT_FUNC;
    } else if (isDclrPtr(dclr)) {
        return DT_PTR;
    } else {
        switch (decl->type) {
            case DIT_NULL:
                return DT_INVALID;

            case DIT_SCALAR:
            {
                ScalarType t = decl->repr.scalar.scalar;
                if (t == SCL_DOT_DOT_DOT || t == SCL_VOID || t == SCL_NULL) {
                    debugOut("Bad scalar type: %d", t);
                    return DT_INVALID;
                } else {
                    return DT_SIMPLE;
                }
            }

            case DIT_STRUCT_OR_UNION:
            {
                pDeclStructInfo s = decl->repr.s;
                if (s == NULL) {
                    debugOut("Null structure");
                    return DT_INVALID;
                } else {
                    if (s->body == NULL) {
                        if (s->name == NULL) {
                            return DT_INVALID;
                        } else {
                            return DT_STRUCT_VAR;
                        }
                    } else {
                        return DT_STRUCT_DEF;
                    }
                }
            }

            case DIT_ENUM:
            {
                pDeclEnum e = decl->repr.e;
                if (e == NULL) {
                    return DT_INVALID;
                } else {
                    if (e->list == NULL) {
                        return DT_ENUM_VAR;
                    } else {
                        return DT_ENUM_DEF;
                    }
                }
            }

            default:
                assert(0);
                return DT_INVALID;
        }
    }
}

char *getDeclIdName(pDeclInfo decl, DeclType type) {
    switch (type) {
    case DT_PTR:
    case DT_SIMPLE:
    case DT_STRUCT_VAR:
    case DT_ENUM_VAR:
    case DT_FUNC:
        return getDclrName(decl->dclr);

    case DT_STRUCT_DEF:
        assert(decl->repr.s->name != NULL);
        return getTokenIdName(decl->repr.s->name);

    case DT_ENUM_DEF:
        return NULL;

    case DT_INVALID:
    {
        char *retVal = getDclrName(decl->dclr);
        if (retVal == NULL) {
            return "???";
        } else {
            return retVal;
        }
    }

    default:
        assert(0);
        return NULL;
    }
}

static int getDeclTypeSize(pDeclInfo decl, DeclType type) {
    static int asmSclType[SCL_MAX][SIZE_MAX] = {
                 /* SIZE_16      SIZE_32         SIZE_48*/
    /*NULL*/       {0,           0,              0},
    /*CHAR*/       {1,           1,              1},
    /*SCHAR*/      {1,           1,              1},
    /*UCHAR*/      {1,           1,              1},
    /*WCHAR*/      {4,           4,              4},
    /*SSHORT*/     {2,           2,              2},
    /*USHORT*/     {2,           2,              2},
    /*SINT*/       {2,           4,              4},
    /*UINT*/       {2,           4,              4},
    /*SLONG*/      {4,           4,              6},
    /*ULONG*/      {4,           4,              6},
    /*FLOAT*/      {4,           4,              4},
    /*DOUBLE*/     {8,           8,              8},
    /*LDOUBLE*/    {8,           8,              8},
    /*VOID*/       {0,           0,              0},
    /*DOT_DOT_DOT*/{0,           0,              0}
    };
    static int ptrStr[SIZE_MAX] =
        { 2,  4, 6 };

    switch (type) {
    case DT_PTR:
    {
        SizeType temp;
        switch (decl->dclr->ptr->memType) {
            case MT_NULL: temp = g_opt.ptrSize; break;
            case MT_NEAR: temp = g_opt.nearPtrSize; break;
            case MT_FAR: temp = g_opt.farPtrSize; break;
            case MT_FAR16: temp = g_opt.farPtrSize; break;
            case MT_HUGE: temp = g_opt.hugePtrSize; break;
            default: assert(0);
        }
        return ptrStr[temp];
    }

    case DT_SIMPLE:
        assert(decl->repr.scalar.scalar <= SCL_MAX);
        assert(decl->repr.scalar.scalar >= SCL_NULL);
        return asmSclType[decl->repr.scalar.scalar][g_opt.intSize];

    case DT_ENUM_VAR:
        return asmSclType[SCL_SINT][g_opt.intSize];
    case DT_STRUCT_DEF:
        return 0;
    case DT_ENUM_DEF:
        return NULL;
    case DT_INVALID:
        return 0;
    case DT_FUNC:
    case DT_STRUCT_VAR:
        return 0;
    default:
        assert(0);
        return 0;
    }
}

char* getAsmPragmaName(pDclr dclr) {
    if (dclr == NULL) {
        return NULL;
    }
    if (dclr->pragmaMod == NULL) {
        return NULL;
    }
    switch (dclr->pragmaMod->data->code) {
    case Y___CDECL:  return "C";
    case Y___PASCAL:  return "PASCAL";
    case Y___FORTRAN:  return "FORTRAN";
    case Y__SYSCALL:  return "";  // Leave it up to user
    case Y___STDCALL:  return "";  // Leave it up to user
    default: assert(0); return NULL;
    }
}

char* getFuncMemTypeAsmName(pDclr dclr) {
    char *memTypeTable[MT_MAX] = {
        "PROC",
        "NEAR",
        "FAR",
        "FAR",
        "FAR"
    };
    assert(dclr != NULL);
    return memTypeTable[dclr->memType];
}

char* getStructName(pDeclInfo decl) {
    return getTokenIdName(decl->repr.s->name);
}

static void _expandPushDeclInfo(int fileNum, pDeclInfo decl,
                                char *postfix) {
    char *sizeStr1Table[9] = {
        "INV_SIZE_0", "BYTE", "WORD", "INV_SIZE_3", "DWORD",
        "INV_SIZE_5", "FWORD", "INV_SIZE_7", "QWORD"
    };
    char *sizeStr2Table[9] = {
        "INV_SIZE_0", "DB", "DW", "INV_SIZE_3", "DD",
        "INV_SIZE_5", "DF", "INV_SIZE_7", "DQ"
    };
    DeclType type =  getDeclType(decl);
    pTokPos typePos = getDeclPos(decl);
    char *name = getDeclIdName(decl, type);
    int typeSize = getDeclTypeSize(decl, type);

    pushPrintStack( fileNum, OUNIT, createOUnitNewline());

    switch (type) {
    case DT_PTR:
    case DT_SIMPLE:
    case DT_STRUCT_VAR:
    case DT_ENUM_VAR:
    case DT_FUNC:
    {
        pDclr dclr = decl->dclr;
        if (isExternDecl(decl) || (type == DT_FUNC)) {
            char *pragmaName;
            if (type == DT_STRUCT_VAR) {
                pushPrintStack( fileNum,  OUNIT,
                    createOUnitTextId(getStructName(decl), NULL));
            } else if (type == DT_FUNC) {
                pushPrintStack( fileNum,  OUNIT,
                    createOUnitText(getFuncMemTypeAsmName(dclr), NULL));
            } else {
                pushPrintStack( fileNum,  OUNIT,
                    createOUnitText(sizeStr1Table[typeSize], NULL));
            }

            pushPrintStack( fileNum,  OUNIT,
                    addOUnitPostfix(createOUnitText(name, NULL), ":"));

            pragmaName = getAsmPragmaName(dclr);
            if (pragmaName != NULL) {
                pushPrintStack( fileNum,  OUNIT,
                    createOUnitText(pragmaName, NULL));
            }
            pushPrintStack( fileNum,  OUNIT,
                    createOUnitText("EXTRN", typePos));
        } else {
            pOUnit typeName;
            if (dclr != NULL) if (dclr->arrList != NULL) {
                pushPrintStack( fileNum,  ARRAY,  dclr->arrList);
            } else {
                pushPrintStack( fileNum,  OUNIT,
                    createOUnitText(" ?", NULL));
            }

            if (type == DT_STRUCT_VAR) {
                typeName = createOUnitTextId(getStructName(decl), NULL);
            } else {
                typeName = createOUnitText(sizeStr2Table[typeSize], NULL);
            }

            pushPrintStack(fileNum, OUNIT,  typeName);

            pushPrintStack(fileNum, OUNIT,
                    createOUnitDclrName(name, typePos, postfix) );
        }
        break;
    }

    case DT_STRUCT_DEF:
    {
        char *type;
        pDeclStructBody body = decl->repr.s->body;
        pushPrintStack( fileNum, OUNIT,
            addOUnitPostfix(createOUnitTextId(name, body->endPos), "ENDS" ) );
        initExpandPushListParam(fileNum, DECL_INFO, body->declList, name);
        pushPrintStack( fileNum, OUNIT, createOUnitNewline());
        switch (decl->repr.s->type) {
            case DSIT_STRUCT: type = "STRUC"; break;
            case DSIT_UNION: type = "UNION"; break;
            default: assert(0);
        }
        pushPrintStack( fileNum, OUNIT,
                    addOUnitPostfix(createOUnitTextId(name, typePos), type) );
        break;
    }

    case DT_ENUM_DEF:
        initExpandPushList(fileNum, ENUM_ELEM, decl->repr.e->list);
        break;

    case DT_INVALID:
        pushPrintStack( fileNum, OUNIT,
            addOUnitPostfix(createOUnitText("INVALID DECLARATION: ", NULL),
                         name ));
        break;

    default:
        assert(0);
    }
}

static void _expandPushEnumElem(int fileNum, pEnumElem elem) {
    assert(elem->expression != NULL);
    pushPrintStack(fileNum, TREE, elem->expression);
    pushPrintStack(fileNum, OUNIT, createOUnitText("EQU", NULL));
    pushPrintStack(fileNum, OUNIT,
        createOUnitTextId(getTokenIdName(elem->name), elem->name->pos));
    if (elem->begPunctExists) {
        pushPrintStack(fileNum, OUNIT, createOUnitNewline());
    }
}

static void _expandPushArray(int fileNum, pArrList arrList) {
    pArrElem elem, dummy;
    if (!getCurrSLListPosElem(arrList, &elem)) {
        assert(0);
    }
    incCurrSLListPos(arrList);
    pushPrintStack(fileNum, OUNIT, createOUnitText(")", NULL));
    if (!getCurrSLListPosElem(arrList, &dummy)) { // Any elements left?
        pushPrintStack(fileNum, OUNIT, createOUnitText("?", NULL));
    } else {
        pushPrintStack( fileNum,  ARRAY,  arrList);
    }
    pushPrintStack(fileNum, OUNIT, createOUnitText(" DUP(", NULL));
    pushPrintStack(fileNum, TREE, elem->constExpr);
}

static void _expandPushToken(int fileNum, pToken token) {
    pushPrintStack(fileNum, OUNIT, createOUnitFromTok(token));
}

static void _expandPushLogEntry(int fileNum, pToken logEntry) {
    _expandPushToken(fileNum, logEntry);
    pushPrintStack( fileNum, OUNIT, createOUnitNewline());
}

ExpandFuncTable printExpandAsmTable =
{
//    Params    FuncName
      0,        NULL,                       //OUNIT
      0,        NULL,                       //LIST
      0,        _expandPushToken,           //TOKEN
      0,        expandPushTree,             //TREE
      0,        NULL,                       //DECL_PREFIX
      0,        NULL,                   //PRAGMA_DECL_INFO
      1,        (pExpandFunc) _expandPushDeclInfo,        //DECL_INFO
      0,        NULL,                       //DECL_STRUCT_INFO
      0,        NULL,                       //UNION_ELEM
      0,        NULL,                       //DECL_ENUM
      0,        _expandPushEnumElem,        //ENUM_ELEM
      0,        NULL,                       //DCLR
      0,        _expandPushArray,           //ARRAY
      0,        _expandPushLogEntry,        //LOG_ENTRY
};

/*-----------------------------------------------------------------------*/

char *asmKeywordsTable[] = {
".186", ".286", ".286C", ".286P", ".287", ".386", ".386P",
".387", ".486", ".486P", ".586", ".586P", ".8086", ".8087",
".ALPHA", ".BREAK", ".CODE", ".CONST", ".CONTINUE", ".CREF", ".DATA",
".DATA_UN", ".DOSSEG", ".ENDW", ".ERR", ".ERRB", ".ERRDEF", ".ERRDIF",
".ERRDIFI", ".ERRE", ".ERRIDN", ".ERRIDNI", ".ERRNB", ".ERRNDEF", ".ERRNZ",
".EXIT", ".FARDATA", ".FARDATA_UN", ".LFCOND", ".LIST", ".LISTALL", ".LISTIF",
".LISTMACRO", ".LISTMACROALL", ".MODEL", ".NOCREF", ".NOLIST", ".RADIX", ".REPEAT",
".SALL", ".SEQ", ".SFCOND", ".STACK", ".STARTUP", ".TFCOND", ".UNTIL",
".WHILE", ".XCREF", ".XLIST", "AAA", "AAD", "AAM", "AAS",
"ABS2", "ADC", "ADD", "ADDR", "AH", "AL", "ALIAS",
"ALIGN", "AND", "ARPL", "ASSUME", "AT", "AX", "BASIC",
"BH", "BL", "BOUND", "BP", "BSF", "BSR", "BSWAP",
"BT", "BTC", "BTR", "BTS", "BX", "BYTE", /*"C",*/
"CALL", "CALLF", "CASEMAP", "CATSTR", "CBW", "CDQ", "CH",
"CL", "CLC", "CLD", "CLI", "CLTS", "CMC", "CMP",
"CMPS", "CMPSB", "CMPSD", "CMPSW", "CMPXCHG", "CMPXCHG8B", "COMM",
"COMMENT", "COMMON", "COMPACT", "CPUID", "CR0", "CR2", "CR3",
"CR4", "CS", "CWD", "CWDE", "CX", "DAA", "DAS",
"DB", "DD", "DEC", "DF", "DH", "DI", "DIV",
"DL", "DP", "DQ", "DR0", "DR1", "DR2", "DR3",
"DR6", "DR7", "DS", "DT", "DUP", "DW", "DWORD",
"DX", "EAX", "EBP", "EBX", "ECHO", "ECX", "EDI",
"EDX", "ELSE", "ELSEIF", "END", "ENDIF", "ENDP", "ENDS",
"ENTER", "EQ", "EQU", "EQU2", "ERROR", "ES", "ESI",
"ESP", "EVEN", "EXPORT", "EXTERN", "EXTERNDEF", "EXTRN", "F2XM1",
"FABS", "FADD", "FADDP", "FAR", "FARSTACK", "FBLD", "FBSTP",
"FCHS", "FCLEX", "FCOM", "FCOMP", "FCOMPP", "FCOS", "FDECSTP",
"FDISI", "FDIV", "FDIVP", "FDIVR", "FDIVRP", "FENI", "FFREE",
"FIADD", "FICOM", "FICOMP", "FIDIV", "FIDIVR", "FILD", "FIMUL",
"FINCSTP", "FINIT", "FIST", "FISTP", "FISUB", "FISUBR", "FLAT",
"FLD", "FLD1", "FLDCW", "FLDENV", "FLDENVD", "FLDENVW", "FLDL2E",
"FLDL2T", "FLDLG2", "FLDLN2", "FLDPI", "FLDZ", "FLOAT", "FMUL",
"FMULP", "FNCLEX", "FNDISI", "FNENI", "FNINIT", "FNOP", "FNSAVE",
"FNSAVED", "FNSAVEW", "FNSTCW", "FNSTENV", "FNSTENVD", "FNSTENVW", "FNSTSW",
"FOR", "FORC", "FORTRAN", "FPATAN", "FPREM", "FPREM1", "FPTAN",
"FRNDINT", "FRSTOR", "FRSTORD", "FRSTORW", "FS", "FSAVE", "FSAVED",
"FSAVEW", "FSCALE", "FSETPM", "FSIN", "FSINCOS", "FSQRT", "FST",
"FSTCW", "FSTENV", "FSTENVD", "FSTENVW", "FSTP", "FSTSW", "FSUB",
"FSUBP", "FSUBR", "FSUBRP", "FTST", "FUCOM", "FUCOMP", "FUCOMPP",
"FWAIT", "FWORD", "FXAM", "FXCH", "FXTRACT", "FYL2X", "FYL2XP1",
"GE", "GLOBAL", "GROUP", "GS", "GT", "HIGH", "HIGHWORD",
"HLT", "HUGE", "IDIV", "IF", "IF1", "IF2", "IFB",
"IFDEF", "IFDIF", "IFDIFI", "IFE", "IFIDN", "IFIDNI", "IFNB",
"IFNDEF", "IGNORE", "IMUL", "IN", "INC", "INCLUDE", "INCLUDELIB",
"INS2", "INSB", "INSD", "INSW", "INT", "INTO", "INVD",
"INVLPG", "INVOKE", "IRET", "IRETD", "IRP", "JA", "JAE",
"JB", "JBE", "JC", "JCXZ", "JE", "JECXZ", "JG",
"JGE", "JL", "JLE", "JMP", "JMPF", "JNA", "JNAE",
"JNB", "JNBE", "JNC", "JNE", "JNG", "JNGE", "JNL",
"JNLE", "JNO", "JNP", "JNS", "JNZ", "JO", "JP",
"JPE", "JPO", "JS", "JZ", "LABEL", "LAHF", "LAR",
"LARGE", "LDS", "LE", "LEA", "LEAVE", "LENGTH", "LENGTHOF",
"LES", "LFS", "LGDT", "LGS", "LIDT", "LLDT", "LMSW",
"LOCAL", "LOCK", "LODS", "LODSB", "LODSD", "LODSW", "LOOP",
"LOOPE", "LOOPNE", "LOOPNZ", "LOOPZ", "LOW", "LOWWORD", "LROFFSET",
"LSL", "LSS", "LT", "LTR", "MACRO", "MASK", "MEDIUM",
"MEMORY", "MOD", "MOV", "MOVS", "MOVSB", "MOVSD", "MOVSW",
"MOVSX", "MOVZX", "MUL", "NAME", "NE", "NEAR", "NEARSTACK",
"NEG", "NO87", "NOP", "NOT", "NOTHING", "OFFSET", "OPATTR",
"OPTION", "OR", "ORG", "OS_DOS", "OS_OS2", "OUT", "OUTS",
"OUTSB", "OUTSD", "OUTSW", "PAGE", "PARA", "PASCAL", "POP",
"POPA", "POPAD", "POPCONTEXT", "POPF", "POPFD", "PRIVATE", "PROC",
"PROTO", "PTR", "PUBLIC", "PURGE", "PUSH", "PUSHA", "PUSHAD",
"PUSHCONTEXT", "PUSHF", "PUSHFD", "PWORD", "QWORD", "RCL", "RCR",
"RDMSR", "RDTSC", "READONLY", "RECORD", "REP", "REPE", "REPNE",
"REPNZ", "REPZ", "RET", "RETF", "RETN", "ROL", "ROR",
"RSM", "SAHF", "SAL", "SAR", "SBB", "SBYTE", "SCAS",
"SCASB", "SCASD", "SCASW", "SDWORD", "SEG2", "SEGMENT", "SETA",
"SETAE", "SETB", "SETBE", "SETC", "SETE", "SETG", "SETGE",
"SETL", "SETLE", "SETNA", "SETNAE", "SETNB", "SETNBE", "SETNC",
"SETNE", "SETNG", "SETNGE", "SETNL", "SETNLE", "SETNO", "SETNP",
"SETNS", "SETNZ", "SETO", "SETP", "SETPE", "SETPO", "SETS",
"SETZ", "SGDT", "SHL", "SHLD", "SHORT", "SHR", "SHRD",
"SI", "SIDT", "SIZE", "SIZEOF", "SLDT", "SMALL", "SMSW",
"SP", "SS", "ST", "STC", "STD", "STDCALL", "STI",
"STOS", "STOSB", "STOSD", "STOSW", "STR", "STRUC", "STRUCT",
"SUB", "SUBTITLE", "SUBTTL", "SWORD", "SYSCALL", "TBYTE", "TEST",
"TEXTEQU", "THIS", "TINY", "TITLE", "TR3", "TR4", "TR5",
"TR6", "TR7", "TYPEDEF", "UNION", "USE16", "USE32", "USES",
"VARARG", "VERR", "VERW", "WAIT", "WATCOM_C", "WBINVD", "WIDTH",
"WORD", "WRMSR", "XADD", "XCHG", "XLAT", "XLATB", "XOR",
NULL
};

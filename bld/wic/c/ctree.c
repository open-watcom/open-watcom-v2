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


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "wic.h"

static pLabel _createLabel(LabelType type, void *data) {
    pLabel newLabel = wicMalloc(sizeof(Label));
    newLabel->type = type;
    newLabel->repr.data = data;
    return newLabel;
}

pLabel createTokenLabel(pToken token) {
    return _createLabel(LABT_TOKEN, token);
}

pLabel createListLabel(pSLList list) {
    return _createLabel(LABT_LIST, list);
}

pLabel createDeclInfoLabel(pDeclInfo dinfo) {
    return _createLabel(LABT_DECL_INFO, dinfo);
}

int isExternDecl(pDeclInfo decl) {
    if (decl == NULL) {
        return 0;
    } else {
        return decl->storage == STG_EXTERN;
    }
}

pLabel createDeclListLabel(pDeclList declList) {
    return _createLabel(LABT_DECL_LIST, declList);
}

pLabel createConstr0Label(LabelConstrType type) {
    pLabel newLabel = _createLabel(LABT_CONSTRUCT_ROOT, NULL);
    newLabel->repr.constr.type = type;
    newLabel->repr.constr.numTokens = 0;
    return newLabel;
}

pLabel createConstr1Label(LabelConstrType type, pToken t0) {
    pLabel newLabel = _createLabel(LABT_CONSTRUCT_ROOT, NULL);
    newLabel->repr.constr.type = type;
    newLabel->repr.constr.numTokens = 1;
    newLabel->repr.constr.tokens = wicMalloc(sizeof(pToken) * 1);
    newLabel->repr.constr.tokens[0] = t0;
    return newLabel;
}

pLabel createConstr2Label(LabelConstrType type, pToken t0, pToken t1) {
    pLabel newLabel = _createLabel(LABT_CONSTRUCT_ROOT, NULL);
    newLabel->repr.constr.type = type;
    newLabel->repr.constr.numTokens = 2;
    newLabel->repr.constr.tokens = wicMalloc(sizeof(pToken) * 2);
    newLabel->repr.constr.tokens[0] = t0;
    newLabel->repr.constr.tokens[1] = t1;
    return newLabel;
}

pLabel createConstr3Label(LabelConstrType type, pToken t0, pToken t1,
                          pToken t2) {
    pLabel newLabel = _createLabel(LABT_CONSTRUCT_ROOT, NULL);
    newLabel->repr.constr.type = type;
    newLabel->repr.constr.numTokens = 3;
    newLabel->repr.constr.tokens = wicMalloc(sizeof(pToken) * 3);
    newLabel->repr.constr.tokens[0] = t0;
    newLabel->repr.constr.tokens[1] = t1;
    newLabel->repr.constr.tokens[2] = t2;
    return newLabel;
}

pLabel createConstr4Label(LabelConstrType type, pToken t0, pToken t1,
                          pToken t2, pToken t3) {
    pLabel newLabel = _createLabel(LABT_CONSTRUCT_ROOT, NULL);
    newLabel->repr.constr.type = type;
    newLabel->repr.constr.numTokens = 4;
    newLabel->repr.constr.tokens = wicMalloc(sizeof(pToken) * 4);
    newLabel->repr.constr.tokens[0] = t0;
    newLabel->repr.constr.tokens[1] = t1;
    newLabel->repr.constr.tokens[2] = t2;
    newLabel->repr.constr.tokens[3] = t3;
    return newLabel;
}

pCTree createCTreeRoot(pLabel label) {
    pCTree newTree;

    newTree = wicMalloc(sizeof(CTree));
    newTree->label = label;
    newTree->child1 = NULL;
    newTree->child2 = NULL;
    return newTree;
}

pCTree createNULLCTree(void) {
    return NULL;
}

pCTree createCTree1(pLabel label, pCTree child) {
    pCTree newTree;

    newTree = createCTreeRoot(label);
    newTree->child1 = child;
    return newTree;
}

pCTree createCTree2(pLabel label, pCTree child1, pCTree child2) {
    pCTree newTree;

    newTree = createCTreeRoot(label);
    newTree->child1 = child1;
    newTree->child2 = child2;
    return newTree;
}

pLabel getCTreeLabel(pCTree tree) {
    return tree->label;
}

pLabel getCTreeChild1Label(pCTree tree) {
    return tree->child1->label;
}

pLabel getCTreeChild2Label(pCTree tree) {
    return tree->child2->label;
}

pCTree getCTreeChild1(pCTree tree) {
    return tree->child1;
}

pCTree getCTreeChild2(pCTree tree) {
    return tree->child2;
}

/*************************************************************************/

pDclrList createDclrList(pDclr elem) {
    pDclrList list = createSLList();
    addSLListElem(list, elem);
    return list;
}

pDclrList addDclrList(pDclrList list, pDclr elem) {
    addSLListElem(list, elem);
    return list;
}

static void _addScalarTypeToDeclInfo(pDeclInfo dinfo, YScalarType tokCode);

static pDeclInfo _createDeclInfo(DeclInfoType type) {
    pDeclInfo newDInfo;

    newDInfo = wicMalloc(sizeof *newDInfo);

    newDInfo->begPunct = NULL;
    newDInfo->qualifier = STY_NULL;
    newDInfo->storage = STG_NULL;
    newDInfo->prefixPos = NULL;
    newDInfo->dclrList = NULL;
    newDInfo->dclr = NULL;
    newDInfo->type = type;
    return newDInfo;
};

pDeclInfo createQualifierDeclInfo(TypeQualifier qualifier, pTokPos pos) {
    pDeclInfo newDecl = _createDeclInfo(DIT_NULL);
    newDecl->qualifier = qualifier;
    newDecl->prefixPos = combine2TokPos(newDecl->prefixPos, pos);
    return newDecl;
}

pDeclInfo createStgClassDeclInfo(StgClass stgClass, pToken pos) {
    pDeclInfo newDecl = _createDeclInfo(DIT_NULL);
    newDecl->storage = stgClass;
    newDecl->prefixPos = combine2TokPos(newDecl->prefixPos, pos->pos);
    pos->pos = NULL;
    zapToken(pos);
    return newDecl;
}

pDeclInfo createDeclInfoENUM(pDeclEnum e) {
    pDeclInfo newDecl = _createDeclInfo(DIT_ENUM);
    newDecl->repr.e = e;
    return newDecl;
}

pDeclInfo createDeclInfoSTRUCT(pDeclStructInfo s) {
    pDeclInfo newDecl = _createDeclInfo(DIT_STRUCT_OR_UNION);
    newDecl->repr.s = s;
    return newDecl;
}

pDeclInfo createDeclInfoSCALAR(YScalarType tokCode, pTokPos pos) {
    pDeclInfo newDecl = _createDeclInfo(DIT_SCALAR);
    newDecl->repr.scalar.scalar = SCL_NULL;
    newDecl->repr.scalar.scalarCombo = STM_NULL;
    newDecl->repr.scalar.scalarPos = pos;
    _addScalarTypeToDeclInfo(newDecl, tokCode);
    return newDecl;
}

void initNoTypeDecl(pDeclInfo decl) {
    if (decl->type == DIT_NULL) {
        decl->type = DIT_SCALAR;
        decl->repr.scalar.scalar = SCL_NULL;
        decl->repr.scalar.scalarCombo = STM_NULL;
        decl->repr.scalar.scalarPos = NULL;
        _addScalarTypeToDeclInfo(decl, STM_INT);
    }
}

static void _addScalarTypeToDeclInfo(pDeclInfo dinfo, YScalarType tokCode) {
    int i;

    static struct {
        ScalarType     id;
        YScalarType    combo;
    } listOfScalarTypes[] = {
        { SCL_CHAR,              STM_CHAR },
        { SCL_SCHAR,             STM_CHAR | STM_SIGNED },
        { SCL_UCHAR,             STM_CHAR | STM_UNSIGNED },
        { SCL_SSHORT,            STM_SHORT },
        { SCL_SSHORT,            STM_SHORT | STM_INT },
        { SCL_SSHORT,            STM_SHORT | STM_SIGNED },
        { SCL_SSHORT,            STM_SHORT | STM_SIGNED | STM_INT },
        { SCL_USHORT,            STM_SHORT | STM_UNSIGNED },
        { SCL_USHORT,            STM_SHORT | STM_UNSIGNED | STM_INT },
        { SCL_SINT,              STM_SIGNED },
        { SCL_SINT,              STM_INT },
        { SCL_SINT,              STM_INT | STM_SIGNED },
        { SCL_UINT,              STM_INT | STM_UNSIGNED },
        { SCL_UINT,              STM_UNSIGNED },
        { SCL_SLONG,             STM_LONG },
        { SCL_SLONG,             STM_LONG | STM_SIGNED },
        { SCL_ULONG,             STM_LONG | STM_UNSIGNED },
        { SCL_SLONG,             STM_LONG | STM_INT },
        { SCL_SLONG,             STM_LONG | STM_INT | STM_SIGNED },
        { SCL_ULONG,             STM_LONG | STM_INT | STM_UNSIGNED },
        { SCL_FLOAT,             STM_FLOAT },
        { SCL_DOUBLE,            STM_DOUBLE },
        { SCL_VOID,              STM_VOID },
        { SCL_LDOUBLE,           STM_LONG | STM_DOUBLE },
        { SCL_WCHAR,             STM_LONG | STM_CHAR },
        { SCL_DOT_DOT_DOT,       STM_DOT_DOT_DOT },
        { SCL_NULL,              STM_NULL }
    };

    assert(dinfo->type == DIT_SCALAR);

    tokCode |= dinfo->repr.scalar.scalarCombo;
    i = 0;
    while (listOfScalarTypes[i].id != 0) {
        if (listOfScalarTypes[i].combo == tokCode) {
            dinfo->repr.scalar.scalar = listOfScalarTypes[i].id;
            dinfo->repr.scalar.scalarCombo = tokCode;
            return;
        }
        i++;
    }
    reportError(RERR_INV_TYPE_COMBO);
}

pDeclInfo combine2DeclInfo(pDeclInfo d1, pDeclInfo d2) {
    assert(d1 != NULL); assert(d2 != NULL);
    assert(d1 != d2);
    assert(d1->dclrList == NULL); assert(d2->dclrList == NULL);

    d1->prefixPos = combine2TokPos(d1->prefixPos, d2->prefixPos);
    d2->prefixPos = NULL;
    if (d1->storage == STG_NULL) {
        if (d2->storage != STG_NULL) {
            d1->storage = d2->storage;
        }
    } else if (d2->storage != STG_NULL) {
        reportError(RERR_INV_TYPE_COMBO);
    }

    if (d1->qualifier != STY_NULL && d1->qualifier == d2->qualifier) {
        reportError(RERR_INV_TYPE_COMBO);
    } else {
        d1->qualifier |= d2->qualifier;
    }

    if (d1->type == DIT_NULL) {
        d1->type = d2->type;
        switch (d2->type) {
            case DIT_NULL:
                break;

            case DIT_SCALAR:
                d1->repr.scalar.scalar = SCL_NULL;
                d1->repr.scalar.scalarCombo = STM_NULL;
                d1->repr.scalar.scalarPos =
                                    d2->repr.scalar.scalarPos;
                                    d2->repr.scalar.scalarPos = NULL;
                _addScalarTypeToDeclInfo(d1, d2->repr.scalar.scalarCombo);
                break;

            case DIT_STRUCT_OR_UNION:
                d1->repr.s = d2->repr.s; d2->repr.s = NULL;
                break;

            case DIT_ENUM:
                d1->repr.e = d2->repr.e;   d2->repr.e = NULL;
                break;

            default:
                assert(0);
        }
    } else if (d1->type == DIT_SCALAR && d2->type == DIT_SCALAR) {
        _addScalarTypeToDeclInfo(d1, d2->repr.scalar.scalarCombo);
    } else {
        reportError(RERR_INV_TYPE_COMBO);
    }

    d1->dclr = combine2Dclr(d1->dclr, d2->dclr);    d2->dclr = NULL;

    zapDeclInfo(d2);
    return d1;
}

pDeclStructInfo createDeclStructInfo(pToken typeTok,
                                     pToken name, pDeclStructBody body) {
    pDeclStructInfo newSI;

    newSI = wicMalloc(sizeof *newSI);
    if (typeTok->data->code == Y_STRUCT) {
        newSI->type = DSIT_STRUCT;
    } else if (typeTok->data->code == Y_UNION) {
        newSI->type = DSIT_UNION;
    } else {
        assert(0);
    }
    newSI->typePos = typeTok->pos;
    typeTok->pos = NULL; zapToken(typeTok);
    newSI->name = name;
    newSI->body = body;
    return newSI;
};

pDeclStructBody createDeclStructBody(pDeclList list, pTokPos endPos) {
    pDeclStructBody newSB;

    newSB = wicMalloc(sizeof *newSB);
    newSB->declList = list;
    newSB->endPos = endPos;
    return newSB;
}

pDeclInfo addDeclInfoDclrList(pDeclInfo decl, pDclrList list) {
    decl->dclrList = list;
    return decl;
}

pDeclInfo setDeclDclr(pDeclInfo decl, pDclr dclr) {
    decl->dclr = dclr;
    return decl;
}

pDeclInfo addDeclDclr(pDeclInfo decl, pDclr dclr) {
    decl->dclr = combine2Dclr(dclr, decl->dclr);
    return decl;
}

pDeclInfo addDeclPunct(pDeclInfo decl, pToken begPunct) {
    decl->begPunct = begPunct;
    return decl;
}

pDeclList createDeclList(pDeclInfo elem) {
    pDeclList newList = createSLList();
    addSLListElem(newList, elem);
    return newList;
}

pDeclList addDeclList(pDeclList list, pDeclInfo elem) {
    addSLListElem(list, elem);
    return list;
}

/*------------------------------*/

pDclr createDclr(pToken id) {
    pDclr newDclr = wicMalloc(sizeof *newDclr);

    newDclr->memType = MT_NULL;
    newDclr->memPos = NULL;
    newDclr->pragmaMod = NULL;
    newDclr->ptr = NULL;
    newDclr->id = id;
    newDclr->arrList = NULL;
    newDclr->argBegin = NULL;
    newDclr->args = NULL;
    newDclr->argEnd = NULL;
    newDclr->equalTok = NULL;
    newDclr->initializer = NULL;
    return newDclr;
}

int isDclrFunc(pDclr dclr) {  // Returns true iff dclr is a function
    if (dclr == NULL) {
        return 0;
    }
    return (dclr->args != NULL);
}

int isDclrPtr(pDclr dclr) {  // Returns true iff dclr is a pointer
    if (dclr == NULL) {
        return 0;
    }
    return (dclr->ptr != NULL && dclr->args == NULL);
}

char *getDclrName(pDclr dclr) {
    if (dclr == NULL) {
        return NULL;
    }
    return getTokenIdName(dclr->id);
}


pDclr addDclrPragmaModifier(pDclr dclr, pToken pragmaTok) {
    dclr->pragmaMod = pragmaTok;
    return dclr;
}

static void _addDclrPtrMemModel(pDclrPtr ptr, MemType memType, pTokPos pos) {
    if (ptr->memType  == MT_NULL) {
        ptr->memType = memType;
        ptr->pos = dupTokPos(pos, NULL);
    }
}

pDclr addDclrMemModifier(pDclr dclr, pToken memTok) {
    MemType memType;

    switch (memTok->data->code) {
        case Y___NEAR:  memType = MT_NEAR; break;
        case Y___FAR:   memType = MT_FAR; break;
        case Y___FAR16: memType = MT_FAR16; break;
        case Y___HUGE:  memType = MT_HUGE; break;
        default: assert(0);
    }
    if (isDclrPtr(dclr)) {
        _addDclrPtrMemModel(dclr->ptr, memType, memTok->pos);
    } else {
        dclr->memType = memType;
        dclr->memPos = dupTokPos(memTok->pos, NULL);
    }
    zapToken(memTok);
    return dclr;
}

pDclr addDclrPtrModifier(pDclr dclr, pDclrPtr ptr) {
    if (dclr->ptr == NULL && ptr != NULL) {
        dclr->ptr = ptr;
        dclr->memType = MT_NULL;
        zapTokPos(dclr->memPos); dclr->memPos = NULL;
    }
    return dclr;
}

static void _addArrList(pDclr dclr, pToken begin, pCTree constExpr, pToken end){
    pArrElem elem;
    if (dclr->arrList == NULL) {
        dclr->arrList = createSLList();
    }
    elem = wicMalloc(sizeof *elem);
    elem->lBracket = begin;
    elem->constExpr = constExpr;
    elem->rBracket = end;
    addSLListElem(dclr->arrList, elem);
}

pDclr addDclrArray(pDclr dclr, pToken begin, pCTree constExpr, pToken end) {
    if (isDclrPtr(dclr)) {
        // Nothing
    } else if (dclr->args != NULL) {
        reportError(RERR_CANT_HAVE_ARR_OF_FUNC);
    } else {
        _addArrList(dclr, begin, constExpr, end);
    }

    return dclr;
}

pDclr addDclrFuncArgs(pDclr dclr, pToken lParen, pDeclList args, pToken rParen) {
    if (args != NULL) {
        if (isDclrPtr(dclr)) {
            // Do nothing
        } else if (dclr->args != NULL) {
            reportError(RERR_2_OR_MORE_ARG_LISTS);
        } else if (dclr->arrList != NULL) {
            reportError(RERR_FUNC_CANT_RET_ARR);
        } else  {
            dclr->argBegin = lParen;
            dclr->args = args;
            dclr->argEnd = rParen;
        }
    }

    return dclr;
}

pDclr addDclrInitializer(pDclr dclr, pToken equalTok, pCTree initializer) {
    if (dclr->initializer != NULL) {
        reportError(RERR_DCLR_CANT_BE_INIT_TWICE);
    }
    dclr->equalTok = equalTok;
    dclr->initializer = initializer;
    return dclr;
}


pDclr combine2Dclr(pDclr dclr1, pDclr dclr2) {
    pDclrPtr saveDclr1Ptr;
    if (dclr1 == NULL) {
        if (dclr2 != NULL) {
            return dclr2;
        } else {
            return NULL;
        }
    } else {
        if (dclr2 == NULL) {
            return dclr1;
        } else {
            // Combine the two -- below.
        }
    }

    assert(dclr1 != dclr2);
    if (dclr1->memType != MT_NULL && dclr2->memType != MT_NULL &&
        dclr1->memType != dclr2->memType) {
        reportError(RERR_INV_MEM_MODEL_COMBO);
    } else if (dclr1->memType != MT_NULL) {
        //
    } else if (dclr2->memType != MT_NULL) {
        dclr1->memType = dclr2->memType;  dclr2->memType = MT_NULL;
        dclr2->memPos = dclr2->memPos;  dclr2->memPos = NULL;
    }

    dclr1->pragmaMod = combine2Token(dclr1->pragmaMod, dclr2->pragmaMod);
    dclr2->pragmaMod = NULL;

    saveDclr1Ptr = dclr1->ptr;  dclr1->ptr = NULL;
    addDclrPtrModifier(dclr1, dclr2->ptr);  dclr2->ptr = NULL;
    addDclrPtrModifier(dclr1, saveDclr1Ptr);

    dclr1->id = combine2Token(dclr1->id, dclr2->id); dclr2->id = NULL;

    dclr1->arrList = combine2SLList(dclr2->arrList, dclr1->arrList);
    dclr2->arrList = NULL;

    addDclrFuncArgs(dclr1, dclr2->argBegin, dclr2->args, dclr2->argEnd);
    dclr2->argBegin = NULL; dclr2->args = NULL; dclr2->argEnd = NULL;

    addDclrInitializer(dclr1, dclr2->equalTok, dclr2->initializer);
    dclr2->equalTok = NULL; dclr2->initializer = NULL;

    zapDclr(dclr2);

    return dclr1;
}

pDclrPtr createDclrPtr(pToken yTimes, TypeQualifier qualifiers) {
    pDclrPtr ptr = wicMalloc(sizeof *ptr);
    ptr->memType = MT_NULL;
    ptr->qualifiers = qualifiers;
    ptr->pos = yTimes->pos; yTimes->pos = NULL;
    zapToken(yTimes);
    return ptr;
}

/*------------*/

pDeclEnum createDeclEnum(pToken enumPos, pEnumList list) {
    pDeclEnum body = wicMalloc(sizeof *body);
    body->enumPos = enumPos->pos;  enumPos->pos = NULL;
    zapToken(enumPos);
    body->list = list;
    return body;
}

pEnumList createEnumList(pEnumElem elem) {
    pEnumList list = createSLList();

    addSLListElem(list, elem);
    return list;
}

pEnumList finishEnumListCreation(pEnumList list) {
    pEnumElem elem;
    pCTree prevExpr;
    pToken prevTok;
    pEnumElem prevElem;

    rewindCurrSLListPos(list);
    if (!getCurrSLListPosElem(list, &elem)) {
        reportError(FATAL_INTERNAL, "in finishEnumListCreation");
    }
    incCurrSLListPos(list);
    if (elem->expression == NULL) {
        elem->expression =
            createCTreeRoot(createTokenLabel(createEnumNumToken(0)));
    }
    prevElem = elem;
    if (getCurrSLListPosElem(list, &elem)) {
        do {
            if (elem->expression == NULL) {
                prevTok = dupToken(prevElem->name, NULL);
                prevTok->pos->spacesBefore = 1;
                prevTok->pos->linesBefore = 0;
                prevExpr = createCTreeRoot(createTokenLabel(prevTok));
                elem->expression = createCTree2(
                    createConstr1Label(LABCT_PLUS, createPLUSToken()),
                    prevExpr,
                    createCTreeRoot(
                        createTokenLabel(
                            createEnumNumToken(1)
                        )
                    )
                );
            }
            prevElem = elem;
            incCurrSLListPos(list);
        } while (getCurrSLListPosElem(list, &elem));
    }
    return list;
}

pEnumList addEnumList(pEnumList list,  pEnumElem elem) {
    addSLListElem(list, elem);
    return list;
}

pEnumElem createEnumElem(pToken name, pToken equal, pCTree expression) {
    pEnumElem elem = wicMalloc(sizeof *elem);
    elem->begPunctExists = 0;
    elem->name = name;
    elem->equal = equal;
    elem->expression = expression;
    return elem;
}

pCTree createEnumFromDefine(pToken defTok, pToken id, pCTree expr) {
    zapTokPos(id->pos);
    id->pos = dupTokPos(defTok->pos, NULL);
    return createCTreeRoot( createDeclListLabel( transformDecl (
        createDeclInfoENUM(
            createDeclEnum(
                defTok,
                createEnumList(
                    createEnumElem(
                        id,
                        createEQUALToken(),
                        expr
                    )
                )
            )
        )
    )));
}

pEnumElem addEnumElemBegPunct(pEnumElem elem) {
    elem->begPunctExists = 1;
    return elem;
}

/*--------------------*/

void logMessage(char *str) {
#if 0   // Disable for now, MAYBE fix later or delete it altogether
        // The problem may occur when
        // it is called from within printAll while expanding the list.
    pTokPos pos;
    int len;
    char *comment;

    if (g_currPos == NULL || str[0] == 0) {
        return;
    }

    // I think there is a bug in this code: If str does not contain '\n',
    // nothing is logged.  Fix this one too! (Or make an assumption that
    // str does not contain '\n')
    len = strcspn(str, "\n");
    if (len != 0) {
        comment = wicMalloc(len+1);
        memcpy(comment, str, len);
        comment[len] = 0;
        pos = dupTokPos(g_currPos, NULL);
        pos->spacesBefore = 2;
        pos->linesBefore = 0;
        addSLListElem(g_logList, createCommentToken(comment, pos));
        logMessage(str+len+1);
    }
#else
    str = str;  // Avoid warning
#endif
}

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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "wic.h"


/*============== Duplicate functions ========================*/

static pTokPos _dupPos;

static pCTree _dupCTree(pCTree elem);
static pLabel _dupLabel(pLabel elem);
static pDeclInfo _dupDeclInfo(pDeclInfo elem);
static pDclr _dupDclr(pDclr elem);
static pDeclStructInfo _dupDeclStructInfo(pDeclStructInfo elem);
static pDeclStructBody _dupDeclStructBody(pDeclStructBody elem);
static pArrElem _dupArrElem(pArrElem elem);
static pDclrPtr _dupDclrPtr(pDclrPtr elem);
static pDeclEnum _dupDeclEnum(pDeclEnum elem);
static pEnumElem _dupEnumElem(pEnumElem elem);
static pToken _dupToken(pToken elem);
static pTokPos _dupTokPos(pTokPos elem);
static pTokData _dupTokData(pTokData elem);
#define _dupSLList dupSLList

static pCTree _dupCTree(pCTree elem) {
    pCTree newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    newElem->label = _dupLabel(elem->label);
    newElem->child1 = _dupCTree(elem->child1);
    newElem->child2 = _dupCTree(elem->child2);

    return newElem;
}

static pLabel _dupLabel(pLabel elem) {
    pLabel newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    newElem->type = elem->type;
    switch (elem->type) {
    case LABT_LIST:
        newElem->repr.list = _dupSLList(elem->repr.list, _dupToken);
        break;
    case LABT_TOKEN:
        newElem->repr.token = _dupToken(elem->repr.token);
        break;
    case LABT_CONSTRUCT_ROOT:
        {
            int size = elem->repr.constr.numTokens;
            int i;
            memcpy(&(newElem->repr.constr), &(elem->repr.constr),
                    sizeof elem->repr.constr);
            if (size > 0) {
                newElem->repr.constr.tokens = wicMalloc(size *
                                      sizeof newElem->repr.constr.tokens[0]);
            }
            for (i = 0; i < size; i++) {
                newElem->repr.constr.tokens[i] =
                    _dupToken(elem->repr.constr.tokens[i]);
            }
        }
        break;
    case LABT_DECL_INFO:
        newElem->repr.dinfo = _dupDeclInfo(elem->repr.dinfo);
        break;
    case LABT_DECL_LIST:
        newElem->repr.declList =
                _dupSLList(elem->repr.declList, _dupDeclInfo);
        break;
    default:
        assert(0);
    }
    return newElem;
}

static pDeclInfo _dupDeclInfo(pDeclInfo elem) {
    pDeclInfo newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->begPunct = _dupToken(elem->begPunct);
    newElem->prefixPos = _dupTokPos(elem->prefixPos);

    switch(elem->type) {
        case DIT_SCALAR:
            newElem->repr.scalar.scalarPos =
                    _dupTokPos(elem->repr.scalar.scalarPos);
            break;

        case DIT_STRUCT_OR_UNION:
            newElem->repr.s = _dupDeclStructInfo(elem->repr.s);
            break;

        case DIT_ENUM:
            newElem->repr.e = _dupDeclEnum(elem->repr.e);
            break;

        case DIT_NULL:
            break;

        default:
            break;
    }
    newElem->dclrList = _dupSLList(elem->dclrList, _dupDclr);
    newElem->dclr = _dupDclr(elem->dclr);

    return newElem;
}

static pDclr _dupDclr(pDclr elem) {
    pDclr newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->memType = elem->memType;
    newElem->memPos = _dupTokPos(elem->memPos);

    newElem->pragmaMod = _dupToken(elem->pragmaMod);

    newElem->ptr = _dupDclrPtr(elem->ptr);

    newElem->id = _dupToken(elem->id);

    newElem->arrList = _dupSLList(elem->arrList, _dupArrElem);
    newElem->argBegin = _dupToken(elem->argBegin);
    newElem->args = _dupSLList(elem->args, _dupDeclInfo);
    newElem->argEnd = _dupToken(elem->argEnd);

    newElem->equalTok = _dupToken(elem->equalTok);
    newElem->initializer = _dupCTree(elem->initializer);

    return newElem;
}

static pDeclStructInfo _dupDeclStructInfo(pDeclStructInfo elem) {
    pDeclStructInfo newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->type = elem->type;
    newElem->typePos = _dupTokPos(elem->typePos);
    newElem->name = _dupToken(elem->name);
    newElem->body = _dupDeclStructBody(elem->body);

    return newElem;
}

static pDeclStructBody _dupDeclStructBody(pDeclStructBody elem) {
    pDeclStructBody newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->declList = _dupSLList(elem->declList, _dupDeclInfo);
    newElem->endPos = _dupTokPos(elem->endPos);
    return newElem;
}

static pArrElem _dupArrElem(pArrElem elem) {
    pArrElem newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);

    newElem->lBracket = _dupToken(elem->lBracket);
    newElem->constExpr = _dupCTree(elem->constExpr);
    newElem->rBracket = _dupToken(elem->rBracket);

    return newElem;
}

static pDclrPtr _dupDclrPtr(pDclrPtr elem) {
    pDclrPtr newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->memType = elem->memType;
    newElem->qualifiers = elem->qualifiers;
    newElem->pos = _dupTokPos(elem->pos);

    return newElem;
}

static pDeclEnum _dupDeclEnum(pDeclEnum elem) {
    pDeclEnum newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->enumPos = _dupTokPos(elem->enumPos);
    newElem->list = _dupSLList(elem->list, _dupEnumElem);

    return newElem;
}

static pEnumElem _dupEnumElem(pEnumElem elem) {
    pEnumElem newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->name = _dupToken(elem->name);
    newElem->equal = _dupToken(elem->equal);
    newElem->expression = _dupCTree(elem->expression);

    return newElem;
}

static pToken _dupToken(pToken elem) {
    pToken newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, elem, sizeof *newElem);

    newElem->data = _dupTokData(elem->data);
    newElem->pos = _dupTokPos(elem->pos);
    return newElem;
}

static pTokData _dupTokData(pTokData elem) {
    pTokData newElem;
    if (elem == NULL) {
        return NULL;
    }
    newElem = wicMalloc(sizeof *newElem);

    memcpy(newElem, elem, sizeof *newElem);
    if (elem->code == Y_NUMBER) {
        // Nothing to copy
    } else if (elem->code == Y_STRING) {
        newElem->repr.s.s = wicMalloc(elem->repr.s.strLen);
        memcpy(newElem->repr.s.s, elem->repr.s.s, elem->repr.s.strLen);
    } else {
        registerString(elem->repr.string, !FREE_STRING);
    }
    return newElem;
}

static int _firstPosChange;

static pTokPos _dupTokPos(pTokPos elem) {
    pTokPos newElem;
    pTokPos temp;
    int saveSpacesBefore, saveLinesBefore;
    if (elem == NULL) {
        return NULL;
    }

    if (_dupPos == NULL) {
        temp = elem;
    } else {
        temp = _dupPos;
    }

    if (temp == NULL) {
        return NULL;
    }

    if (_firstPosChange) {
        saveSpacesBefore = temp->spacesBefore;
        saveLinesBefore = temp->linesBefore;
        _firstPosChange = 0;
    } else {
        saveSpacesBefore = elem->spacesBefore;
        saveLinesBefore = elem->linesBefore;
    }

    newElem = wicMalloc(sizeof *newElem);
    memcpy(newElem, temp, sizeof *newElem);

    newElem->linesBefore = saveLinesBefore;
    newElem->spacesBefore = saveSpacesBefore;

    return newElem;
}




#define CALL_INTERNAL_DUP(name)   { \
    _dupPos = pos;\
    _firstPosChange = 1; \
    return _dup##name(elem); \
}

pCTree dupCTree(pCTree elem, pTokPos pos)  CALL_INTERNAL_DUP(CTree)
pLabel dupLabel(pLabel elem, pTokPos pos)  CALL_INTERNAL_DUP(Label)
pDeclInfo dupDeclInfo(pDeclInfo elem, pTokPos pos)  CALL_INTERNAL_DUP(DeclInfo)
pDclr dupDclr(pDclr elem, pTokPos pos)  CALL_INTERNAL_DUP(Dclr)
pDeclStructInfo dupDeclStructInfo(pDeclStructInfo elem, pTokPos pos)  CALL_INTERNAL_DUP(DeclStructInfo)
pDeclStructBody dupDeclStructBody(pDeclStructBody elem, pTokPos pos)  CALL_INTERNAL_DUP(DeclStructBody)
pArrElem dupArrElem(pArrElem elem, pTokPos pos)  CALL_INTERNAL_DUP(ArrElem)
pDclrPtr dupDclrPtr(pDclrPtr elem, pTokPos pos)  CALL_INTERNAL_DUP(DclrPtr)
pDeclEnum dupDeclEnum(pDeclEnum elem, pTokPos pos)  CALL_INTERNAL_DUP(DeclEnum)
pEnumElem dupEnumElem(pEnumElem elem, pTokPos pos)  CALL_INTERNAL_DUP(EnumElem)
pToken dupToken(pToken elem, pTokPos pos)  CALL_INTERNAL_DUP(Token)
pTokPos dupTokPos(pTokPos elem, pTokPos pos)  CALL_INTERNAL_DUP(TokPos)


/*====================== Zap functions ================================*/

static void _zapCTree(pCTree elem);
static void _zapLabel(pLabel elem);
static void _zapDeclInfo(pDeclInfo elem);
static void _zapDclr(pDclr elem);
static void _zapDeclStructInfo(pDeclStructInfo elem);
static void _zapDeclStructBody(pDeclStructBody elem);
static void _zapArrElem(pArrElem elem);
static void _zapDclrPtr(pDclrPtr elem);
static void _zapDeclEnum(pDeclEnum elem);
static void _zapEnumElem(pEnumElem elem);
static void _zapToken(pToken elem);
static void _zapTokPos(pTokPos elem);
#define _zapSLList zapSLList

static void _zapCTree(pCTree elem) {
    if (elem == NULL) {
        return;
    }
    _zapLabel(elem->label);
    _zapCTree(elem->child1);
    _zapCTree(elem->child2);

    wicFree(elem);
}

static void _zapLabel(pLabel elem) {
    if (elem == NULL) {
        return;
    }
    switch (elem->type) {
    case LABT_LIST:
        _zapSLList(elem->repr.list, _zapToken);
        break;
    case LABT_TOKEN:
        _zapToken(elem->repr.token);
        break;
    case LABT_CONSTRUCT_ROOT:
        {
            int size = elem->repr.constr.numTokens;
            int i;
            for (i = 0; i < size; i++) {
                _zapToken(elem->repr.constr.tokens[i]);
            }
            if (elem->repr.constr.numTokens > 0) {
                wicFree(elem->repr.constr.tokens);
            }
        }
        break;
    case LABT_DECL_INFO:
        _zapDeclInfo(elem->repr.dinfo);
        break;
    case LABT_DECL_LIST:
        _zapSLList(elem->repr.declList, _zapDeclInfo);
        break;
    default:
        assert(0);
    }
    wicFree(elem);
}

static void _zapDeclInfo(pDeclInfo elem) {
    if (elem == NULL) {
        return;
    }

    _zapToken(elem->begPunct);
    _zapTokPos(elem->prefixPos);

    switch(elem->type) {
        case DIT_SCALAR:
            _zapTokPos(elem->repr.scalar.scalarPos);
            break;

        case DIT_STRUCT_OR_UNION:
            _zapDeclStructInfo(elem->repr.s);
            break;

        case DIT_ENUM:
            _zapDeclEnum(elem->repr.e);
            break;

        case DIT_NULL:
            break;

        default:
            break;
    }

    _zapSLList(elem->dclrList, _zapDclr);
    _zapDclr(elem->dclr);

    wicFree(elem);
}

static void _zapDclr(pDclr elem) {
    if (elem == NULL) {
        return;
    }

    _zapTokPos(elem->memPos);

    _zapToken(elem->pragmaMod);

    _zapDclrPtr(elem->ptr);

    _zapToken(elem->id);

    _zapSLList(elem->arrList, _zapArrElem);
    _zapToken(elem->argBegin);
    _zapSLList(elem->args, _zapDeclInfo);
    _zapToken(elem->argEnd);

    _zapToken(elem->equalTok);
    _zapCTree(elem->initializer);

    wicFree(elem);
}

static void _zapDeclStructInfo(pDeclStructInfo elem) {
    if (elem == NULL) {
        return;
    }

    _zapTokPos(elem->typePos);
    _zapToken(elem->name);
    _zapDeclStructBody(elem->body);

    wicFree(elem);
}

static void _zapDeclStructBody(pDeclStructBody elem) {
    if (elem == NULL) {
        return;
    }

    _zapSLList(elem->declList, _zapDeclInfo);
    _zapTokPos(elem->endPos);
    wicFree(elem);
}

static void _zapArrElem(pArrElem elem) {
    if (elem == NULL) {
        return;
    }

    _zapToken(elem->lBracket);
    _zapCTree(elem->constExpr);
    _zapToken(elem->rBracket);

    wicFree(elem);
}

static void _zapDclrPtr(pDclrPtr elem) {
    if (elem == NULL) {
        return;
    }

    _zapTokPos(elem->pos);

    wicFree(elem);
}

static void _zapDeclEnum(pDeclEnum elem) {
    if (elem == NULL) {
        return;
    }

    _zapTokPos(elem->enumPos);
    _zapSLList(elem->list, _zapEnumElem);

    wicFree(elem);
}

static void _zapEnumElem(pEnumElem elem) {
    if (elem == NULL) {
        return;
    }

    _zapToken(elem->name);
    _zapToken(elem->equal);
    _zapCTree(elem->expression);

    wicFree(elem);
}

static void _zapToken(pToken elem) {
    if (elem == NULL) {
        return;
    }
    _zapTokPos(elem->pos);
    _zapTokData(elem->data);
    wicFree(elem);
}

static void _zapTokPos(pTokPos elem) {
    if (elem == NULL) {
        return;
    }
    wicFree(elem);
}

static void _zapTokData(pTokData elem) {
    if (elem == NULL) {
        return;
    }
    if (elem->code == Y_NUMBER) {
        // Nothing to free
    } else if (elem->code == Y_STRING) {
        wicFree(elem->repr.s.s);
    } else {
        zapString(elem->repr.string);
    }
    wicFree(elem);
}



#define CALL_INTERNAL_zap(name)   { \
    _zap##name(elem); \
}

void zapCTree(pCTree elem)  CALL_INTERNAL_zap(CTree)
void zapLabel(pLabel elem)  CALL_INTERNAL_zap(Label)
void zapDeclInfo(pDeclInfo elem)  CALL_INTERNAL_zap(DeclInfo)
void zapDclr(pDclr elem)  CALL_INTERNAL_zap(Dclr)
void zapDeclStructInfo(pDeclStructInfo elem)  CALL_INTERNAL_zap(DeclStructInfo)
void zapDeclStructBody(pDeclStructBody elem)  CALL_INTERNAL_zap(DeclStructBody)
void zapArrElem(pArrElem elem)  CALL_INTERNAL_zap(ArrElem)
void zapDclrPtr(pDclrPtr elem)  CALL_INTERNAL_zap(DclrPtr)
void zapDeclEnum(pDeclEnum elem)  CALL_INTERNAL_zap(DeclEnum)
void zapEnumElem(pEnumElem elem)  CALL_INTERNAL_zap(EnumElem)
void zapToken(pToken elem)  CALL_INTERNAL_zap(Token)
void zapTokens2(pToken elem1, pToken elem2) {
    _zapToken(elem1);
    _zapToken(elem2);
}
void zapTokens3(pToken elem1, pToken elem2, pToken elem3) {
    _zapToken(elem1);
    _zapToken(elem2);
    _zapToken(elem3);
}
void zapTokPos(pTokPos elem)  CALL_INTERNAL_zap(TokPos)

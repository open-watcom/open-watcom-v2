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
#include <string.h>
#include "wic.h"

static char * NAME_PREFIX = "STRUN"; // struct/union

typedef enum {
    DECL_INFO,
    DECL_TREE_ELEM
} DeclTreeElemType;

struct DeclTreeElem {
    DeclTreeElemType type;
    union {
        pDeclInfo decl;
        pDeclTree list;
    };
};

pDeclTreeElem createDeclTreeElem(DeclTreeElemType type, void *data) {
    pDeclTreeElem newElem = wicMalloc(sizeof *newElem);
    newElem->type = type;
    if (type == DECL_TREE_ELEM) {
        newElem->list = data;
    } else if (type == DECL_INFO) {
        newElem->decl = data;
    } else {
        assert(0);
    }
    return newElem;
}

static void zapDeclTreeElem(pDeclTreeElem elem) {
    if (elem == NULL) {
        return;
    }

    switch(elem->type) {
        case DECL_INFO:
            zapDeclInfo(elem->decl);
            break;
        case DECL_TREE_ELEM:
            zapSLList(elem->list, zapDeclTreeElem);
            break;
        default:
            assert(0);
    }

    wicFree(elem);
}


char *_get1stDclrName(pDclrList list) {
    pDclr dclr;

    rewindCurrSLListPos(list);
    if (!getCurrSLListPosElem(list, &dclr)) {
        reportError(FATAL_INTERNAL, "In _get1stDclrName");
    }
    assert(dclr != NULL);
    assert(dclr->id != NULL);
    return getDclrName(dclr);
}

char *_createUnnamedDeclStructDclrList(pDeclInfo decl) {
    static char noname[] = { "noname           " };
    static unsigned int counter = 1;
    pTokPos endPos;

    sprintf(noname+6, "%d", counter);  // 6 is strlen("noname")
    counter++;

    if (decl->repr.s->body == NULL) {
        endPos = decl->repr.s->name->pos;
    } else {
        endPos = decl->repr.s->body->endPos;
    }

    decl->dclrList = createDclrList (
        createDclr(
            createIDTokenAfter(
                noname,
                endPos
            )
        )
    );

    return noname;
}

static char *_nameUnnamedStruct(pDeclInfo decl, char *baseName, int level){
// Name the sturct/union if it does not have name;
// Return NULL if error occures; Return a named struct otherwise.

    char *name;
    char *retVal;
    int addBase = 1;

    if (decl->storage == STG_TYPEDEF) {
        zapToken(decl->repr.s->name);  decl->repr.s->name = NULL;
        addBase = 0;
    }
    if (decl->repr.s->name == NULL) {
        if (decl->dclrList == NULL) {
            if (level > 0) {
                name = _createUnnamedDeclStructDclrList(decl);
            } else {
                name = "INVALID";
                reportError(RERR_INCOMPLETE_TYPE);
            }
        } else {
            name = _get1stDclrName(decl->dclrList);
        }
    } else {
        name = getTokenIdName(decl->repr.s->name);
        if (decl->dclrList == NULL && level > 0) {
            name = _createUnnamedDeclStructDclrList(decl);
        }
    }
    if (decl->repr.s->name == NULL) {
        if (addBase) {
            retVal = wicMalloc(strlen(baseName) + strlen(name) + 2);
            sprintf(retVal, "%s_%s", baseName, name);
        } else {
            retVal = wicStrdup(name);
        }
        decl->repr.s->name = createIDTokenAfter(
                                retVal,
                                dupTokPos(decl->repr.s->typePos, NULL)
                             );
    }
    retVal = getTokenIdName(decl->repr.s->name);
    return retVal;
}


static int _nameUnnamed(pDeclInfo decl, char *baseName, int level) {
    char *name;

    switch (decl->type) {
    case DIT_ENUM:
    case DIT_SCALAR:
        return 1;
    case DIT_STRUCT_OR_UNION:
        assert(decl->repr.s != NULL);
        name = _nameUnnamedStruct(decl, baseName, level);
        if (name == NULL) {
            reportError(RERR_INCOMPLETE_TYPE);
            return 0;
        }
        if (decl->repr.s->body != NULL) {
            pDeclList list = decl->repr.s->body->declList;
            pDeclInfo node;

            assert(list != NULL);
            rewindCurrSLListPos(list);
            while (getCurrSLListPosElem(list, &node)) {
                incCurrSLListPos(list);
                _nameUnnamed(node, name, level+1);
            }
        }
        return 1;

    case DIT_NULL:
        reportError(RERR_INCOMPLETE_TYPE);
        return 0;

    default:
        assert(0);
    }
    return 1;
}

static pDeclTree _unNestDecl(pDeclInfo decl) {
    pDeclTree retVal = NULL;

    switch (decl->type) {
    case DIT_SCALAR:
        retVal = createSLList();
        addSLListElem(retVal, createDeclTreeElem(DECL_INFO, decl));
        break;

    case DIT_STRUCT_OR_UNION:
    {
        pDeclStructBody body;
        pDeclTree prepend = createSLList();

        body = decl->repr.s->body;
        if (body != NULL) {
            pDeclInfo node;
            pDeclTreeElem node1, node2;
            pDeclTree inside;

            rewindCurrSLListPos(body->declList);

            while (getCurrSLListPosElem(body->declList, &node)) {
                inside = _unNestDecl(node);

                rewindCurrSLListPos(inside);
                if (!getCurrSLListPosElem(inside, &node1)) {
                    reportError(FATAL_INTERNAL, "In _unNestDecl");
                }
                incCurrSLListPos(inside);
                if (getCurrSLListPosElem(inside, &node2))
                {
                    addSLListElem(prepend, node1);
                    setCurrSLListPosElem(body->declList, node2->decl);
                }
                incCurrSLListPos(body->declList);
            }

            if (decl->dclrList != NULL) {
                pDeclInfo newDecl;
                assert(decl->repr.s->typePos != NULL);
                newDecl = dupDeclInfo(decl, decl->repr.s->typePos);
                zapDeclStructBody(newDecl->repr.s->body);
                newDecl->repr.s->body = NULL;
                zapSLList(decl->dclrList, zapDclr);
                decl->dclrList = NULL;

                addSLListElem(prepend, createDeclTreeElem(DECL_INFO, decl));
                retVal = createSLList();
                addSLListElem(retVal,
                    createDeclTreeElem(DECL_TREE_ELEM, prepend) );
                addSLListElem(retVal, createDeclTreeElem(DECL_INFO, newDecl));
            } else {
                addSLListElem(prepend, createDeclTreeElem(DECL_INFO, decl));
                retVal = prepend;
            }
        } else {
            retVal = createSLList();
            addSLListElem(retVal, createDeclTreeElem(DECL_INFO, decl));
        }
        break;
    }

    case DIT_ENUM:
    {
        pDeclEnum e;
        pDclrList saveDclrList;

        e = decl->repr.e;
        assert(e != NULL);
        retVal = createSLList();
        assert(decl->repr.e->enumPos != NULL);
        saveDclrList = decl->dclrList;
        decl->dclrList = NULL;
        if (e->list == NULL && saveDclrList == NULL) {
            reportError(RERR_INCOMPLETE_TYPE);
        }
        if (e->list != NULL) {
            addSLListElem(retVal, createDeclTreeElem(DECL_INFO, decl));
        }
        if (saveDclrList != NULL) {
            pDeclInfo newDecl = createDeclInfoSCALAR(
                STM_INT,
                dupTokPos(decl->repr.e->enumPos, NULL)
            );
            if (isEmptySLList(retVal)) {  // Create a dummy element
                addSLListElem(retVal, createDeclTreeElem(DECL_INFO, NULL));
            }
            addSLListElem(retVal,
                createDeclTreeElem(
                    DECL_INFO,
                    addDeclInfoDclrList(
                        newDecl,
                        saveDclrList
                    )
                )
            );
        }
        break;
    }
    default:
        assert(0);
    }

    return retVal;
}

static void _storeTypedef(pDclr dclr, pDeclInfo decl) {
    pDeclInfo newType;
    pDeclStructBody saveBody;
    pEnumList saveEnumList;
    char *name = getTokenIdName(dclr->id);

    if (name == NULL) {
        reportError(RERR_DCLR_NOT_PRESENT);
        return;
    }
    if (dclr->initializer != NULL) {
        reportError(RERR_TYPEDEF_CANT_BE_INITIALIZED);
        dclr->initializer = NULL;
    }
    switch (decl->type) {
        case DIT_SCALAR:
            break;
        case DIT_STRUCT_OR_UNION:
            saveBody = decl->repr.s->body;
            decl->repr.s->body = NULL;
            break;
        case DIT_ENUM:
            saveEnumList = decl->repr.e->list;
            decl->repr.e->list = NULL;
            break;
        default:
            assert(0);
    }

    newType = dupDeclInfo(decl, NULL);
    addDeclDclr(newType, dclr); dclr = NULL;
    zapToken(newType->dclr->id); newType->dclr->id = NULL;
    newType->storage &= !STG_TYPEDEF;
    addSymbol(createTabEntry(name, SYMT_TYPEDEF, newType));

    switch (decl->type) {
        case DIT_SCALAR:
            break;
        case DIT_STRUCT_OR_UNION:
            decl->repr.s->body = saveBody;
            break;
        case DIT_ENUM:
            decl->repr.e->list = saveEnumList;
            break;
        default:
            assert(0);
    }
}

static pDeclInfo _stripStoreTypedef(pDeclInfo decl) {
    pDclrList dclrList;
    pDclr dclr;

    if (decl->storage != STG_TYPEDEF) {
        return decl;
    }
    dclrList = decl->dclrList;
    if (dclrList == NULL) {
        reportError(RERR_DCLR_NOT_PRESENT);
        return NULL;
    }

    decl->dclrList = NULL;
    rewindCurrSLListPos(dclrList);
    while (getCurrSLListPosElem(dclrList, &dclr)) {
        incCurrSLListPos(dclrList);
        _storeTypedef(dclr, decl);
    }
    zapSLList(dclrList, NULL);  // NOTE: All elements of this list were
                                // previously zapped.
    switch (decl->type) {
        case DIT_SCALAR:
            zapDeclInfo(decl);
            return NULL;
        case DIT_STRUCT_OR_UNION:
            if (decl->repr.s->body == NULL) {
                zapDeclInfo(decl);
                return NULL;
            } else {
                decl->storage &= !STG_TYPEDEF;
                if (decl->prefixPos != STG_NULL) {
                    zapTokPos(decl->repr.s->typePos);
                    decl->repr.s->typePos = decl->prefixPos;
                    decl->prefixPos = NULL;
                }
                return decl;
            }
        case DIT_ENUM:
            if (decl->repr.e->list == NULL) {
                zapDeclInfo(decl);
                return NULL;
            } else {
                decl->storage &= !STG_TYPEDEF;
                return decl;
            }
        default:
            assert(0);
            return NULL;
    }
}

void _pushDeclTree(pDeclTreeElem *stack, int *stackPos, int MAX,
                   pDeclTree declTree) {
    pDeclTreeElem elem;

    rewindCurrSLListPos(declTree);
    while (getCurrSLListPosElem(declTree, &elem)) {
        if (*stackPos >= MAX) {
            reportError(ERR_INTERNAL_STACK_OVERFLOW);
            return;
        }
        incCurrSLListPos(declTree);
        stack[*stackPos] = elem;
        (*stackPos)++;
    }

    zapSLList(declTree, NULL);
}

pDeclList _flattenDeclTree(pDeclTree declTree) {
    pDeclList retVal;
    enum { MAX = 100 };
    pDeclTreeElem stack[MAX];
    int stackPos = 0;

    if (declTree == NULL) {
        return NULL;
    }

    retVal = createSLList();
    _pushDeclTree(stack, &stackPos, MAX, declTree);
    while(stackPos > 0) {
        stackPos--;
        if (stack[stackPos]->type == DECL_TREE_ELEM) {
            _pushDeclTree(stack, &stackPos, MAX, stack[stackPos]->list);
        } else if (stack[stackPos]->type == DECL_INFO) {
            if (stack[stackPos]->decl != NULL) {
                addBegSLListElem(retVal, stack[stackPos]->decl);
            }
            stack[stackPos]->decl = NULL; zapDeclTreeElem(stack[stackPos]);
        } else {
            assert(0);
        }
    }
    return retVal;
}

pDeclList _expandDeclDclrList(pDeclList declList) {
    pDeclInfo currDecl;
    pDclrList dclrList;
    if (declList == NULL) {
        return NULL;
    }
    rewindCurrSLListPos(declList);
    while (getCurrSLListPosElem(declList, &currDecl)) {
        if (currDecl->type == DIT_STRUCT_OR_UNION) {
            if (currDecl->repr.s->body != NULL) {
                _expandDeclDclrList(currDecl->repr.s->body->declList);
            }
        }

        dclrList = currDecl->dclrList;
        currDecl->dclrList = NULL;
        if (dclrList != NULL) {
            pDclr dclr1, dclrNext;
            rewindCurrSLListPos(dclrList);
            if (!getCurrSLListPosElem(dclrList, &dclr1)) {
                reportError(FATAL_INTERNAL, "In _expandDeclDclrList");
            }
            incCurrSLListPos(dclrList);
            while(getCurrSLListPosElem(dclrList, &dclrNext)) {
                pDeclInfo newDecl;
                incCurrSLListPos(dclrList);
                newDecl = dupDeclInfo(currDecl, NULL);
                addDeclDclr(newDecl, dclrNext);
                dclrNext = NULL;  /* zapped in combine2Dclr */
                insertSLListAfterCurrPosElem(declList, newDecl);
                incCurrSLListPos(declList);
            }
            addDeclDclr(currDecl, dclr1);
            zapSLList(dclrList, NULL); dclrList = NULL;
        }
        incCurrSLListPos(declList);
    }
    return declList;
}

int _checkNonTranslatableConstructs(pDeclInfo decl) {
    switch (decl->type) {
    case DIT_SCALAR:
        if (decl->dclrList == NULL) {
            reportError(RERR_DCLR_NOT_PRESENT);
            return 0;
        }
        return 1;

    case DIT_STRUCT_OR_UNION:
    {
        pDeclStructInfo dsinfo = decl->repr.s;
        if (decl->dclrList == NULL && dsinfo->body == NULL) {
            char *s = "";
            if (dsinfo->name != NULL) {
                s = getTokenIdName(dsinfo->name);
            }
            reportError(CERR_FORW_DEFN_NOT_ALLOWED, s);
            return 0;
        }
        return 1;
    }

    case DIT_ENUM:
        if (decl->dclrList == NULL && decl->repr.e->list == NULL) {
            reportError(RERR_INCOMPLETE_ENUM_DECL);
            return 0;
        }
        return 1;

    default: assert(0); return 0;
    }
}

pDeclList transformDecl(pDeclInfo decl) {
    pDeclList declList;
    initNoTypeDecl(decl);
    if (!_nameUnnamed(decl, NAME_PREFIX, 0)) {
        return NULL;
    }

    if (!_checkNonTranslatableConstructs(decl)) {
        return NULL;
    }

    decl = _stripStoreTypedef(decl);
    if (decl == NULL) {
        return NULL;
    }

    declList = _expandDeclDclrList(_flattenDeclTree(_unNestDecl(decl)));
    return declList;
}

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


#include <stdlib.h>
#include <string.h>
#include "wic.h"

static pSLList tokQue;
static eofScanned = 0;

static pToken getNextTokQueElem(void);
static pToken getNextTokQueElemEmitEOL(int emitEol);

void initExpandToken(void) {
    tokQue = createSLList();
    g_currLineCode = createSLList();
    eofScanned = 0;
}

void zapExpandToken(void) {
    assert(isEmptySLList(tokQue));
    zapSLList(tokQue, NULL);
    zapSLList(g_currLineCode, NULL);
}

static pTokPos _pushMacroCodePosArgument;
int _pushMacroCodeFirstToken;

static pToken changeToken(pToken token) {
    pToken newToken;
    newToken = dupToken(token, _pushMacroCodePosArgument);

    if (_pushMacroCodeFirstToken) {
        _pushMacroCodeFirstToken = 0;
    } else {
        newToken->pos->spacesBefore = token->pos->spacesBefore;
    }
    return newToken;
}

static void prependTokQue(pSLList list, pTokPos pos) {
    rewindCurrSLListPos(list);
    _pushMacroCodePosArgument = pos;
    _pushMacroCodeFirstToken = 1;
    addBegSLListSLList(tokQue, list, changeToken);
}


static int expandMacroWithParams(pSLList mCode, char *name, pTokPos pos,
                                  ExpOpType expandOp) {
    #define maxArgs 15
    int numArgs = 0;
    pToken tok;
    char* mArg[maxArgs];
    pSLList realArg[maxArgs];
    pSLList expanded = NULL;
    pSLList backList = NULL;
    int code = 0;
    int retval = 1;
    int i;

    memset(realArg, 0, (sizeof realArg[0])*maxArgs);

    // Get params (at the beginning of mCode). (Here we DO report errors)
    rewindCurrSLListPos(mCode);
    incCurrSLListPos(mCode); // Skip Y_PRE_SPECIAL_LEFT_PAREN
    for (numArgs = 0; numArgs < maxArgs; numArgs++) {
        getCurrSLListPosElem(mCode, &tok);
        incCurrSLListPos(mCode);
        code = tok->data->code;
        mArg[numArgs] = getTokDataIdName(tok->data);
        if (numArgs == 0 && code == Y_RIGHT_PAREN) {
            break;  // no arguments is OK
        }
        if (code != Y_ID) {
            reportError(RERR_EXPECTING_BUT_FOUND, "identifier", mArg[numArgs]);
        }
        getCurrSLListPosElem(mCode, &tok);
        incCurrSLListPos(mCode);
        code = tok->data->code;
        if (code == Y_RIGHT_PAREN) {
            numArgs++;
            break;
        } else if (code == Y_COMMA) {
        } else {
            break; // error will be displayed upon exit
        }
    }
    if (code != Y_RIGHT_PAREN) {
        reportError(RERR_EXPECTING_BUT_FOUND, ")",
                getTokDataIdName(tok->data));
    }

    // get the replacements for arguments -- here we have to allow
    // errors, so that if expansion is unsuccessful and we are in preparser,
    // we repass the line without expanding. This means no premature return!
    // We'll push tokens back on stack if error occurs.

    backList = createSLList();
    tok = getNextTokQueElemEmitEOL(expandOp&EXP_OP_EMIT_EOL);
    addBegSLListElem(backList, tok);
    if (tok->data->code != Y_LEFT_PAREN) {
        if (!(expandOp & EXP_OP_HIDE_ERRORS)) {
            reportError(RERR_EXPECTING_BUT_FOUND, "(",
                staticGetTokenStr(tok, 0));
        }
        retval = 0;
    } else {
        for (i = 0; i < numArgs ; i++) {
            realArg[i] = createSLList();
        }
        for (i = 0; i < numArgs ; i++) {
            int parenNest = 0;
            for (;;) {
                tok = getNextTokQueElemEmitEOL(expandOp&EXP_OP_EMIT_EOL);
                addBegSLListElem(backList, tok);
                code = tok->data->code;
                if (code == Y_EOF ||
                    getTokDataType(tok->data) == TT_PREPROCESSOR)
                {
                    if (!(expandOp&EXP_OP_HIDE_ERRORS)) {
                        char errstr[80];
                        strcpy(errstr, "an argument to ");
                        strncpy(errstr+strlen(errstr), name, 60);
                        errstr[75] = 0;
                        reportError(RERR_EXPECTING_BUT_FOUND,
                            errstr, staticGetTokenStr(tok, 0));
                    }
                    retval = 0;
                } else if (code == Y_LEFT_PAREN) {
                        parenNest++;
                } else if (code == Y_RIGHT_PAREN) {
                    if (parenNest > 0) {
                        parenNest--;
                    } else {
                        if (i != numArgs-1) {
                            if (!(EXP_OP_HIDE_ERRORS&expandOp)) {
                                reportError(RERR_NOT_ENOUGH_ARGS, name);
                            }
                            i = numArgs-1;
                            retval = 0;
                        }
                        break;
                    }
                } else if (code == Y_COMMA && parenNest == 0) {
                    break;
                }
                addSLListElem(realArg[i], tok);
            }
        }
        if (code != Y_RIGHT_PAREN) {
            if (!(EXP_OP_HIDE_ERRORS&expandOp)) {
                reportError(RERR_EXPECTING_BUT_FOUND, ")", staticGetTokenStr(tok, 0));
            }
            retval = 0;
        }

        if (retval) {
            // Do the actual expansion
            expanded = createSLList();
            while (getCurrSLListPosElem(mCode, &tok)) {
                if (tok->data->code == Y_ID) {
                    char *s = getTokDataIdName(tok->data);
                    for (i = 0; i < numArgs; i++) {
                        if (s == mArg[i]) break;
                    }
                } else {
                    i = numArgs;
                }
                if (i == numArgs) {
                    addSLListElem(expanded, tok);
                } else {
                    rewindCurrSLListPos(realArg[i]);
                    while (getCurrSLListPosElem(realArg[i], &tok)) {
                        addSLListElem(expanded, tok);
                        incCurrSLListPos(realArg[i]);
                    }
                }
                incCurrSLListPos(mCode);
            }
            // And now, put back into tokQue
            prependTokQue(expanded, pos);
        }

        // cleanup
        for (i = 0; i < numArgs; i++) {
            zapSLList(realArg[i], NULL);
        }
        zapSLList(expanded, NULL);
    }
    if (!retval) {
        // error has occured: put backList back into tokQue
        while (popStackElem(backList, &tok)) {
            addBegSLListElem(tokQue, tok);
        }
    }

    zapSLList(backList, NULL);

    return retval;
}


static int pushMacroCode(pSLList macroCode, char *name, pTokPos pos,
                          ExpOpType expandOp) {
    pToken tok;
    rewindCurrSLListPos(macroCode);
    if (getCurrSLListPosElem(macroCode, &tok)) {
        if (tok->data->code == Y_PRE_SPECIAL_LEFT_PAREN) {
            return expandMacroWithParams(macroCode, name, pos, expandOp);
        } else {
            prependTokQue(macroCode, pos);
            return 1;
        }
    }
    return 1;
}

static pToken getNextTokQueElemEmitEOL(int emitEol) {
    pToken tok;
    do {
        tok = getNextTokQueElem();
    } while (!emitEol && tok->data->code == Y_PRE_NEWLINE);
    return tok;
}

static pToken getNextTokQueElem(void) {
    pToken token;
    WicErrors err;
    int continueFlag;
    static int beginNewLine = 1;

    if (unqueSLListElem(tokQue, &token)) {
        return token;
    }

    if (beginNewLine) {
        zapSLList(g_currLineCode, NULL);
        g_currLineCode = createSLList();
        beginNewLine = 0;
    }

    do {
        continueFlag = 0;
        token = createToken(createTokData(), createTokPos());
        err = getNextToken(token);
        while (err != ERR_NONE) {
            reportError(err);
            err = getNextToken(token);
        }

        if (token->data->code == Y_PRE_NEWLINE) {
            beginNewLine = 1;
        } else if (token->data->code == Y_PRE_COMMENT) {
            continueFlag = 1;
            /*  Place all of the comments at the end of the line, but
            before newline */
            token->pos->colNum = MAX_COL_NUM-1;
            addSLListElem(g_commentList, token);
            continue;
        } else if (token->data->code == Y_EOF) {
            if (!eofScanned) {
                if (popTokFile()) {
                    zapToken(token);
                    continueFlag = 1;
                } else {
                    eofScanned = 1;
                }
            }
        } else {
            addSLListElem(g_currLineCode, token);
        }
    } while (continueFlag);

    return token;
}

/*----------------------- getExpandToken ---------------------------*/
pToken getExpandToken(ExpOpType expandOp, int *expandResult) {
    pToken token;
    pSymTabEntry symbol;
    int continueFlag;

    *expandResult = 1;
    do {
        continueFlag = 0;
        token = getNextTokQueElemEmitEOL(expandOp&EXP_OP_EMIT_EOL);
        if ((expandOp&EXP_OP_EXPAND) && token->data->code == Y_ID) {
            char *name = getTokenIdName(token);
            symbol = findSymbol(name);
            if (symbol != NULL) {
                if (symbol->type == SYMT_MACRO) {
                    *expandResult = pushMacroCode(symbol->repr.macroCode,
                        name, token->pos, expandOp);
                    // In case of error above, the tokQue is unmodified, and we
                    // may return the current token
                    continueFlag = *expandResult;
                } else if (symbol->type == SYMT_TYPEDEF) {
                    token->data->code = Y_TYPEDEF_NAME;
                    token->data->repr.pTypeDecl = symbol->repr.typeCode;
                }
            }
        }
    } while(continueFlag);

    return token;
}


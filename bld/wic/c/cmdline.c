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
#include <stdio.h>
#include <stdlib.h>
#include "wic.h"

static int _scanSize(char *str, SizeType *type) {
    SizeType temp = 0;

    sscanf(str, "%d", &temp);
    switch (temp) {
        case 16: *type = SIZE_16; break;
        case 32: *type = SIZE_32; break;
        case 48: *type = SIZE_48; break;
        default:
            reportError(ERR_INV_CMD_LINE_OPTION, str);
            return 0;
    }
    return 1;
}

void initFDReadInd(pFDReadInd entry, char *name, int readOnly) {
    entry->name = name;
    entry->readOnly = readOnly;
}

pFDReadInd createFDReadInd(char *name, int readOnly) {
    pFDReadInd entry = wicMalloc(sizeof *entry);
    assert(name != NULL);
    initFDReadInd(entry, name, readOnly);
    return entry;
}

unsigned FDReadIndHashFunc(void *_entry, unsigned size) {
    pFDReadInd entry = _entry;

    return stringHashFunc(entry->name, size);;
}

int FDReadIndCmpFunc(void *_entry1, void *_entry2) {
    pFDReadInd entry1 = _entry1;
    pFDReadInd entry2 = _entry2;

    return strcmp(entry1->name, entry2->name);
}

void zapFDReadInd(void *_entry) {
    pFDReadInd entry = _entry;

    if (entry == NULL) {
        return;
    }
    wicFree(entry->name);
    wicFree(entry);
}

static void addIncPath(char *s, int readOnly, int addAtEnd) {
    static char *delims = ";";
    pFDReadInd entry;
    char *p;
    if (s == NULL) {
        return;
    }
    s = wicStrdup(s);
    p = strtok(s, delims);
    while (p != NULL) {
        entry = createFDReadInd(wicStrdup(p), readOnly);
        if (addAtEnd) {
            addSLListElem(g_opt.incPathList, entry);
        } else {
            addBegSLListElem(g_opt.incPathList, entry);
        }
        p = strtok(NULL, delims);
    }
    wicFree(s);
}

static void _scanCurrArg(char *currArg) {
    char *saveCurrArg = currArg;

    switch (currArg[0]) {
    case '-':
    case '/':
        currArg++;
        switch (currArg++[0]) {
        case 'F': g_opt.structFieldsSep = currArg; break;
        case 'f': g_opt.targetLang = TLT_FORTRAN; break;
        case '?':
        case 'h':
            printUsageAndExit();
            break;
        case 'i': addIncPath(currArg, 0, 1); break;
        case 'I': _scanSize(currArg, &(g_opt.intSize)); break;
        case 'j': g_opt.signedChar = 1; break;
        case 'l':
            if (currArg[0] == '=') {
                currArg++;
            }
            sscanf(currArg, "%u", &(g_opt.outLineLen));
            if (!WITHIN_RANGE(g_opt.outLineLen, 30, MAX_OUT_LINE_LEN)) {
                reportError(ERR_LINE_LEN_OUT_OF_RANGE, g_opt.outLineLen);
                g_opt.outLineLen = 0;
            }
            break;
        case 'a':
            switch (currArg++[0]) {
            case 'd': g_opt.asmAttachD = 1; break;
            case 0: g_opt.targetLang = TLT_ASM; break;
            default:
                reportError(ERR_INV_CMD_LINE_OPTION, saveCurrArg);
                break;
            }
            break;
        case 's':
            switch (currArg++[0]) {
            case '-':
                g_opt.prefixStructFields = 0;
                _scanCurrArg("-F");
                break;
            case '+': g_opt.prefixStructFields = 1; break;
            case '0': g_opt.supressLevel = 0; break;
            case '1': g_opt.supressLevel = 1; break;
            case '2': g_opt.supressLevel = 2; break;
            case '3': g_opt.supressLevel = 3; break;
            default: g_opt.supressLevel = 0; break;
            }
            break;
        case '1':
            if (currArg[0] == '6') {
                _scanCurrArg("-P16");
                _scanCurrArg("-I16");
            }
            break;
        case '3':
            if (currArg[0] == '2') {
                _scanCurrArg("-P32");
                _scanCurrArg("-I32");
            }
            break;
        case 'p': g_opt.conflictPrefix = currArg; break;
        case 'P':
            if (currArg[0] == 'n') {
                _scanSize(currArg+1, &(g_opt.nearPtrSize));
            } else if (currArg[0] == 'p' || currArg[0] == 'd') {
                _scanSize(currArg+1, &(g_opt.ptrSize));
            } else if (currArg[0] == 'f') {
                _scanSize(currArg+1, &(g_opt.farPtrSize));
            } else if (currArg[0] == 'h') {
                _scanSize(currArg+1, &(g_opt.hugePtrSize));
            } else {
                SizeType temp = SIZE_MAX;
                _scanSize(currArg, &temp);
                if (temp == SIZE_16) {
                    _scanCurrArg("-Pn16");
                    _scanCurrArg("-Pd16");
                    _scanCurrArg("-Pf32");
                    _scanCurrArg("-Ph32");
                } else if (temp == SIZE_32) {
                    _scanCurrArg("-Pn32");
                    _scanCurrArg("-Pd32");
                    _scanCurrArg("-Pf32");
                    _scanCurrArg("-Ph48");
                }
            }
            break;

#ifndef NDEBUG
        case 'd':
            g_opt.debug = 1;
            break;
#endif
        default:
            reportError(ERR_INV_CMD_LINE_OPTION, saveCurrArg);
        }
        break;

    case '?':
        printUsageAndExit();
        break;

    default:
        {
            char driveDir[_MAX_DRIVE+_MAX_DIR+10];
            char drive[_MAX_DRIVE];
            char dir[_MAX_DIR];
            char name[_MAX_FNAME];
            char ext[_MAX_EXT];
            int len;

            _splitpath(currArg, drive, dir, name, ext);
            _makepath(driveDir, drive, dir, "", "");
            len = strlen(driveDir);
            if (len > 0) if (driveDir[len-1] == '\\') {
                driveDir[len-1] = 0;
            }

            addSLListElem(g_opt.fileNameList, wicStrdup(currArg));
            addIncPath(driveDir, 0, 0);  // Add at the beginning
        }
    }
}

void getCmdLineOptions(int argc, char *argv[]) {
    int i;
    char *currArg;

    g_opt.targetLang = TLT_FORTRAN;
    g_opt.signedChar = 0;
    g_opt.asmAttachD = 0;
    g_opt.prefixStructFields = -1;
    g_opt.structFieldsSep = NULL;
    g_opt.conflictPrefix = "WIC_";
    g_opt.supressLevel = 1;
    g_opt.outLineLen = 0;
    g_opt.fileNameList = createSLList();
    g_opt.incPathList = createSLList();
    g_opt.intSize = SIZE_32;
    g_opt.nearPtrSize = SIZE_32;
    g_opt.ptrSize = SIZE_32;
    g_opt.farPtrSize = SIZE_32;
    g_opt.hugePtrSize = SIZE_48;
    g_opt.debug = 0;

    if (argc <= 1) {
        printUsageAndExit();
    } else {
        for (i = 1; i < argc; i++) {
            currArg = argv[i];
            _scanCurrArg(currArg);
        }
    }

    addIncPath(getenv("INCLUDE"), 1, 1);
    if (isEmptySLList(g_opt.fileNameList)) {
        reportError(FATAL_FILE_NOT_SPECIFIED);
    }
    if (g_opt.outLineLen == 0) {
        switch (g_opt.targetLang) {
            case TLT_FORTRAN: g_opt.outLineLen = 72; break;
            case TLT_ASM: g_opt.outLineLen = 79; break;
            default: assert(0);
        }
    }
    if (g_opt.prefixStructFields == -1) {
        switch (g_opt.targetLang) {
            case TLT_FORTRAN: g_opt.prefixStructFields = 0; break;
            case TLT_ASM: g_opt.prefixStructFields = 1; break;
            default: assert(0);
        }
    }
    if (g_opt.structFieldsSep == NULL) {
        switch (g_opt.targetLang) {
            case TLT_FORTRAN: g_opt.structFieldsSep = ""; break;
            case TLT_ASM: g_opt.structFieldsSep = "_FS_"; break;
            default: assert(0);
        }
    }

}

void zapCmdLineOptions(void) {
    zapSLList(g_opt.fileNameList, wicFree);
    zapSLList(g_opt.incPathList, zapFDReadInd);
}

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
#include <stdio.h>
#include <malloc.h>
#include "wic.h"


void initWicBasics(int argc, char *argv[], float dummy) {
    dummy = dummy; /* Dummy argument to include floating point routines */
    initMemory();
    initWicResources(argv[0]);
    initDebug();
    g_commentList = createSLList();
    g_dirList = createSLList();
    g_logList = createSLList();
    g_currPos = NULL;
    getCmdLineOptions(argc, argv);
    initHashTable();
    initOutputSystem();
}

void wicExit(int exitCode) {
    if (exitCode == 0) {
        zapOutputSystem();
        zapCmdLineOptions();
        zapTokPos(g_currPos);
        assert(isEmptySLList(g_logList));  zapSLList(g_logList, NULL);
        assert(isEmptySLList(g_dirList));  zapSLList(g_dirList, NULL);
        assert(isEmptySLList(g_commentList));  zapSLList(g_commentList, NULL);
        zapMemory();
        zapDebug();
        zapWicResources();
        checkMemory();
    } else {
        printf("WIC: Terminating with error...\n");
        fcloseall();
    }
    exit(exitCode);
}

void doConversion(void **name) {
    char *newName = *name;
    initErrorFile(newName);
    initExpandToken();
    initSymbolTable();
    newName = initTokenizer(newName);
    initPrinting(newName);

    cparseInterface();

    zapPrinting();
    zapTokenizer();
    zapSymbolTable();
    zapExpandToken();
    zapStringTable();
    zapErrorFile();
}


void main(int argc, char *argv[]){
    initWicBasics(argc, argv, 1.1); // 3-rd dummy argument is to include
                                    // floating point library

    forAllInSLList(g_opt.fileNameList, doConversion);
    dribble();
    wicExit(0);
}

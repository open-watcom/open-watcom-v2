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


#define  INCL_DOSMODULEMGR
#define  INCL_DOSMISC
#include <os2.h>

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "trpimp.h"
#include "tcerr.h"

static trap_version     (TRAPENTRY *InitFunc)(char *,char *, bool);
static void             (TRAPENTRY *FiniFunc)(void);
static unsigned_16      (TRAPENTRY *InfoFunc)(HAB, HWND);
static char             (TRAPENTRY *HardFunc)(char);

extern trap_version     TrapVer;
extern unsigned         (TRAPENTRY *ReqFunc)(unsigned, mx_entry *,
                                             unsigned, mx_entry *);

bool IsTrapFilePumpingMessageQueue()
{
    return (InfoFunc != NULL);
}

void TellHandles(HAB hab, HWND hwnd)
{
    if (InfoFunc == NULL)
        return;

    InfoFunc(hab, hwnd);
}


char TellHardMode(char hard)
{
    if (HardFunc == NULL)
        return  0;

    return HardFunc(hard);
}

char *LoadTrap(char *trapbuff, char *buff, trap_version *trap_ver)
{
    char                trpfile[CCHMAXPATH];
    int                 len;
    char                *ptr;
    char                *parm;
    HMODULE             hmodDll;
    APIRET              rc;
    char                trpname[CCHMAXPATH] = "";
    char                trppath[CCHMAXPATH] = "";

    if (trapbuff == NULL)
        trapbuff = "std";

    ptr = trapbuff;
    while (*ptr != '\0' && *ptr != ';') {
        ++ptr;
    }
    parm = (*ptr != '\0') ? ptr + 1 : ptr;
    len = ptr - trapbuff;
    memcpy( trpfile, trapbuff, len );
    trpfile[len] = '\0';

    /* To prevent conflicts with the 16-bit DIP DLLs, the 32-bit versions have the "D32"
     * extension. We will search for them along the PATH (not in LIBPATH);
     */
    strcpy(trpname, trpfile);
    strcat(trpname, ".D32");
    if (DosSearchPath(SEARCH_IGNORENETERRS | SEARCH_ENVIRONMENT | SEARCH_CUR_DIRECTORY,
            "PATH", trpname, trppath, sizeof(trppath)) != 0) {
        strcpy(buff, TC_ERR_CANT_LOAD_TRAP);
        return buff;
        }

    rc = DosLoadModule(NULL, 0, trppath, &hmodDll);
    if (rc != 0) {
        strcpy(buff, TC_ERR_CANT_LOAD_TRAP);
        return buff;
    }
    strcpy(buff, TC_ERR_WRONG_TRAP_VERSION);
    if (DosQueryProcAddr(hmodDll, 1, NULL, (PFN*)&InitFunc) != 0
     || DosQueryProcAddr(hmodDll, 2, NULL, (PFN*)&FiniFunc) != 0
     || DosQueryProcAddr(hmodDll, 3, NULL, (PFN*)&ReqFunc) != 0) {
        return buff;
    }
    if (DosQueryProcAddr(hmodDll, 4, NULL, (PFN*)&InfoFunc) != 0) {
        InfoFunc = NULL;
    }
    if (DosQueryProcAddr(hmodDll, 5, NULL, (PFN*)&HardFunc) != 0 ) {
        HardFunc = NULL;
    }
    *trap_ver = InitFunc(parm, trpfile, trap_ver->remote);
    if (trpfile[0] != '\0') {
        strcpy(buff, (char *)trpfile);
        return buff;
    }
    if (!TrapVersionOK(*trap_ver)) {
        KillTrap();
        return buff;
    }
    TrapVer = *trap_ver;
    return NULL;
}


void KillTrap()
{
    FiniFunc();
    ReqFunc  = NULL;
    InitFunc = NULL;
    FiniFunc = NULL;
    InfoFunc = NULL;
    HardFunc = NULL;
}

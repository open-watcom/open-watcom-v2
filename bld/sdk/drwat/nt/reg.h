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


typedef struct {
    DWORD       pid;
    DWORD       priority;
    char        name[ MAX_PROC_NAME ];
} ProcList;

typedef struct {
    DWORD       tid;
    DWORD       priority;
} ThreadList;

typedef struct {
    DWORD       pid;
    DWORD       priority;
    char        name[ MAX_PROC_NAME ];
} ProcStats;

typedef struct {
    DWORD       pid;
    DWORD       tid;
    DWORD       base_pri;
    DWORD       cur_pri;
    DWORD       state;
    DWORD       wait_reason;
} ThreadStats;

typedef struct {
    DWORD       noaccess;
    DWORD       read;
    DWORD       write;
    DWORD       copy;
    DWORD       exec;
    DWORD       execread;
    DWORD       execwrite;
    DWORD       execcopy;
    DWORD       tot;
}MemByType;

typedef struct {
    DWORD       procid;
    MemByType   image;
    MemByType   mapped;
    MemByType   res;
    DWORD       modcnt;
    char        **modlist;
} MemInfo;

/*
 * structures defined below this point are PRIVATE to reg.c
 */

#ifdef CHICAGO

#include <tlhelp32.h>

typedef struct {
    HANDLE                      hdl;
    LPPROCESSENTRY32            procdata;
}ProcPlace;

typedef struct {
    DWORD                       pid;
    HANDLE                      hdl;
    LPTHREADENTRY32             thrddata;
} ThreadPlace;

#else

typedef struct {
    DWORD                       index;
    PERF_OBJECT_TYPE            *obj;
    PERF_INSTANCE_DEFINITION    *inst;
}ProcPlace;

typedef struct {
    DWORD                       index;
    DWORD                       pid;
    PERF_OBJECT_TYPE            *obj;
    PERF_INSTANCE_DEFINITION    *inst;
} ThreadPlace;

#endif

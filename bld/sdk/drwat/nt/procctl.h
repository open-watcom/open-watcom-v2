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


#include "windows.h"
#include "dip.h"
#include "exepe.h"

#define MAX_PROC_NAME               50
#define PROCCTL_ATTATCH             100
#define PROCCTL_THREAD              101
#define PROCCTL_TASKLIST            102
#define PROCCTL_KILL                103
#define PROCCTL_TASKNAME            107
#define PROCCTL_PRIORITY            110
#define PROCCTL_SET_PRIORITY        104
#define PROCCTL_PID                 108
#define PROCCTL_PATH                109
#define PROCCTL_MEM                 201
#define PROCCTL_REFRESH             202
#define PROCCTL_VIEWMEM             203

typedef struct {
    DWORD   segoff;
    BOOL    code;       //TRUE IN CODE ; FALSE IN DATA
} SegInfo;

typedef struct {
    DWORD               segcnt;
    mod_handle          hdl;
    process_info        *procinfo;
    SegInfo             seginfo[1]; /* dynamic array */
} SymInfoNode;

typedef struct {
    DWORD       rva;
    char        name[PE_OBJ_NAME_LEN];
} ObjectInfo;

typedef struct modnode {
    struct modnode      *next;
    SymInfoNode         *syminfo;
    DWORD               num_objects;
    ObjectInfo          *objects;
    DWORD               base;
    DWORD               size;
    HANDLE              fhdl;
    char                *name;
    struct ProcNode     *procnode;
} ModuleNode;

typedef struct ThreadNode {
    struct ThreadNode   *next;
    HANDLE              threadhdl;
    DWORD               threadid;
    DWORD               stack;
} ThreadNode;

typedef struct ProcNode {
    struct ProcNode     *next;
    ModuleNode          *module;
    HANDLE              prochdl;
    DWORD               procid;
    ThreadNode          *thread;
    DWORD               SegCs;
    DWORD               SegDs;
    char                procname[MAX_PROC_NAME];
} ProcNode;

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
* Description:  Resource Compiler pass 2 structures and constants.
*
****************************************************************************/


#ifndef EXEFMT_INCLUDED
#define EXEFMT_INCLUDED

#include "wres.h"
#include "exeos2.h"
#include "exepe.h"
#include "exeflat.h"
#include "exerespe.h"
#include "exereslx.h"
#include "exeseg.h"
#include "exeres.h"

#define PEHDR(h)    (*(h)->WinHead)

typedef enum {
    EXE_TYPE_UNKNOWN,
    EXE_TYPE_PE,        // PE format, Win32
    EXE_TYPE_NE_WIN,    // NE format, Win16
    EXE_TYPE_NE_OS2,    // NE format, 16-bit OS/2
    EXE_TYPE_LX         // LX format, 32-bit OS/2
} ExeType;

typedef struct ResFileInfo {
    struct ResFileInfo  *next;
    char                *name;
    int                 IsOpen;
    WResFileID          Handle;
    WResDir             Dir;
} ResFileInfo;

typedef struct NEExeInfo {
    os2_exe_header  WinHead;
    SegTable        Seg;
    ResTable        Res;
    OS2ResTable     OS2Res;
} NEExeInfo;

typedef struct PEExeInfo {
    exe_pe_header   *WinHead;
    pe_object       *Objects;   /* array of objects. wlink no initialize */
    PEResDir        Res;        /* non-initialized */
    exe_pe_header   WinHeadData; // never access this value directly.  Use
                                 // WinHead to get at it instead
} PEExeInfo;

typedef struct LXExeInfo {
    os2_flat_header OS2Head;
    object_record   *Objects;
    lx_map_entry    *Pages;
    LXResTable      Res;
    uint_32         FirstResObj;
    uint_32         FirstResPage;
} LXExeInfo;

typedef struct ExeFileInfo {
    int             IsOpen;
    int             Handle;
    char            *name;
    uint_32         WinHeadOffset;      /* wlink doesn't initialize this */
    ExeType         Type;
    union {
        NEExeInfo   NEInfo;
        PEExeInfo   PEInfo;
        LXExeInfo   LXInfo;
    } u;
    uint_32         DebugOffset;        /* wlink doesn't initialize this */
} ExeFileInfo;

typedef struct RcPass2Info {
    char            TmpFileName[ _MAX_PATH ];
    ExeFileInfo     TmpFile;
    ExeFileInfo     OldFile;
    ResFileInfo     *ResFiles;
    int             AllResFilesOpen;
    void *          IoBuffer;
} RcPass2Info;

extern int MergeResExePE( void );
extern int MergeResExeLX( void );
extern int MergeResExeWINNE( void );
extern int MergeResExeOS2NE( void );

#endif

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


#ifndef EXEFMT_INCLUDED
#define EXEFMT_INCLUDED

#include <stdlib.h>
#include "watcom.h"
#include "wres.h"
#include "exeos2.h"
#include "exepe.h"
#include "exerespe.h"
#include "exeseg.h"
#include "exeres.h"
#ifdef UNIX
    #include "clibext.h"
#endif

typedef enum {
    EXE_TYPE_UNKNOWN,
    EXE_TYPE_PE,
    EXE_TYPE_NE
} ExeType;

typedef struct ResFileInfo {
    struct ResFileInfo  *next;
    char                *name;
    int                 IsOpen;
    int                 Handle;
    WResDir             Dir;
} ResFileInfo;

typedef struct NEExeInfo {
    os2_exe_header  WinHead;
    SegTable        Seg;
    ResTable        Res;
} NEExeInfo;

typedef struct PEExeInfo {
    pe_header       *WinHead;
    pe_object       *Objects;   /* array of objects. wlink no initialize */
    PEResDir        Res;        /* non-initialized */
    pe_header       WinHeadData; // never access this value directly.  Use
                                 // WinHead to get at it instead
} PEExeInfo;

typedef struct ExeFileInfo {
    int             IsOpen;
    int             Handle;
    char            *name;
    uint_32         WinHeadOffset;      /* wlink doesn't initialize this */
    ExeType         Type;
    union {
        NEExeInfo   NEInfo;
        PEExeInfo   PEInfo;
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

extern int MergeResExeNE( void );
extern int MergeResExePE( void );

#endif

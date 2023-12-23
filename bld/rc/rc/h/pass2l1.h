/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Resource Compiler pass 2 structures and constants core.
*
****************************************************************************/


#ifndef PASS2L1_INCLUDED
#define PASS2L1_INCLUDED

#include "wres.h"
#include "exeos2.h"
#include "exepe.h"
#include "exeflat.h"
#include "rcstrblk.h"


#define PEHDR(h)    (*(h)->WinHead)

typedef enum {
    EXE_TYPE_UNKNOWN,
    EXE_TYPE_PE,        // PE format, Win32
    EXE_TYPE_NE_WIN,    // NE format, Win16
    EXE_TYPE_NE_OS2,    // NE format, 16-bit OS/2
    EXE_TYPE_LX         // LX format, 32-bit OS/2
} ExeType;

typedef struct FullResourceRecord {
    struct FullResourceRecord   *Next;
    struct FullResourceRecord   *Prev;
    resource_record             Info;
} FullResourceRecord;

typedef struct FullTypeRecord {
    struct FullTypeRecord   *Next;
    struct FullTypeRecord   *Prev;
    FullResourceRecord      *Head;
    FullResourceRecord      *Tail;
    resource_type_record    Info;
} FullTypeRecord;

typedef struct ExeResDir {
    uint_16             ResShiftCount;
    uint_16             NumTypes;
    uint_16             NumResources;
    uint_16             TableSize;
    FullTypeRecord      *Head;
    FullTypeRecord      *Tail;
} ExeResDir;

typedef struct ResTable {
    ExeResDir       Dir;
    StringsBlock    Str;
} ResTable;

typedef struct OS2ResEntry {
    uint_16         res_type;   /* resource type */
    uint_16         res_name;   /* resource name */
    WResDirWindow   wind;       /* window into the current WResDir */
    bool            first_part; /* true unless non-first bit of > 64K resource */
    uint_16         seg_length; /* length of resource segment */
    uint_16         mem_flags;  /* resource flags */
} OS2ResEntry;

typedef struct OS2ResTable {
    OS2ResEntry     *resources;
    uint_16         table_size;     /* size of NE resource table in bytes */
    uint_16         num_res_segs;   /* number of resource segments/'resources' entries */
} OS2ResTable;

/* NB: NumOS2ResSegs is a subset of total segments (must be <= NumSegs) but
 * is not always equal to number of resources, because resources > 64K will
 * be split into multiple segments! Only applicable to OS/2 NE, not Windows.
 */
typedef struct SegTable {
    uint_16             NumSegs;        /* Total number of segments */
    uint_16             NumOS2ResSegs;  /* Number of resource segments */
    segment_record      *Segments;      /* array of size NumSegs */
} SegTable;

typedef struct NEExeInfo {
    os2_exe_header      WinHead;
    SegTable            Seg;
    ResTable            Res;
    OS2ResTable         OS2Res;
} NEExeInfo;

typedef struct PEResDirEntry {
    resource_dir_header Head;
    int                 NumUnused;
    struct PEResEntry   *Children;
} PEResDirEntry;

typedef struct PEResDataEntry {
    resource_entry      Entry;
    WResDirWindow       Wind;           /* window into the current WResDir */
} PEResDataEntry;

typedef struct PEResEntry {
    resource_dir_entry  Entry;
    void                *Name;
    bool                IsDirEntry;
    union {
        PEResDataEntry  Data;
        PEResDirEntry   Dir;
    } u;
} PEResEntry;

typedef struct PEResDir {
    PEResDirEntry   Root;
    uint_32         DirSize;
    pe_va           ResRVA;
    uint_32         ResOffset;
    uint_32         ResSize;
    StringsBlock    String;
} PEResDir;

typedef struct PEExeInfo {
    pe_exe_header       *WinHead;
    pe_object           *Objects;       /* array of objects. wlink no initialize */
    PEResDir            Res;            /* non-initialized */
    pe_exe_header       WinHeadData;    /* never access this value directly.  Use
                                           WinHead to get at it instead */
} PEExeInfo;

typedef struct LXResEntry {
    flat_res_table  resource;
    WResDirWindow   wind;       /* window into the current WResDir */
    uint_16         mem_flags;
    uint_16         assigned;
} LXResEntry;

typedef struct LXResTable {
    LXResEntry      *resources;
    uint_32         res_count;
    uint_32         table_size;
    uint_32         num_objects;
    uint_32         num_pages;
} LXResTable;

typedef struct LXExeInfo {
    os2_flat_header     OS2Head;
    object_record       *Objects;
    lx_map_entry        *Pages;
    LXResTable          Res;
    uint_32             FirstResObj;
    uint_32             FirstResPage;
} LXExeInfo;

typedef struct ExeFileInfo {
    FILE                *fp;
    const char          *name;
    uint_32             WinHeadOffset;      /* wlink doesn't initialize this */
    ExeType             Type;
    union {
        NEExeInfo       NEInfo;
        PEExeInfo       PEInfo;
        LXExeInfo       LXInfo;
    } u;
    uint_32             DebugOffset;        /* wlink doesn't initialize this */
} ExeFileInfo;

typedef struct ResFileInfo {
    struct ResFileInfo  *next;
    const char          *name;
    FILE                *fp;
    WResDir             Dir;
} ResFileInfo;

#endif

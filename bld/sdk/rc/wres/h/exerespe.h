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



#ifndef EXERESPE_H_INCLUDED
#define EXERESPE_H_INCLUDED

#include "watcom.h"
#include "exepe.h"
#include "wresall.h"
#include "reserr.h"
#include "rcstr.h"

typedef struct PEResDirEntry {
    resource_dir_header Head;
    int                 NumUnused;
    struct PEResEntry * Children;
} PEResDirEntry;

typedef struct PEResDataEntry {
    resource_entry      Entry;
    WResDirWindow       Wind;           /* window into the current WResDir */
} PEResDataEntry;

typedef struct PEResEntry {
    resource_dir_entry  Entry;
    void *              Name;
    char                IsDirEntry;
    union {
        PEResDataEntry  Data;
        PEResDirEntry   Dir;
    };
} PEResEntry;

typedef struct PEResDir {
    PEResDirEntry   Root;
    uint_32         DirSize;
    pe_va           ResRVA;
    uint_32         ResOffset;
    uint_32         ResSize;
    StringBlock     String;
} PEResDir;


typedef struct _ResInOutInfo {
    // Values that need to be set before calling PutPEResources
    int res;   // -- resource file handle
    int obj;   // -- object file handle
    pe_va rva; // -- RVA for where to put the resources in object file
    uint_32 offset;// -- offset for where to put the resources
    int alignRva;  // NOTE: rva and offset must be aligned
    int alignFile;    // before calling PutPEResources!

    // Values that are returned by PutPEResources
    pe_hdr_table_entry *resTblEntry; // points to table[PE_TBL_RESOURCE]
    pe_object *res_obj;  // Point to the header of resource
} ResInOutInfo, *pResInOutInfo;

// PutPEResources:  Put PE resource file into object file.
// Also sets info->res_obj and info->resTblEntry.
WResStatus PutPEResources(pResInOutInfo info);
WResStatus CopyPEResData(pResInOutInfo info, PEResDir *outRes);
int BuildPEResDir( PEResDir * res, WResDir dir );
WResStatus WriteDirectory( PEResDir * dir, int handle );
void SetPEResObjRecord(pe_object *res_obj, pe_hdr_table_entry *resTblEntry,
                       PEResDir *outRes);
void FreePEResDir( PEResDir * dir );

#endif

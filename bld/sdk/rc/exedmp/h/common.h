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
* Description:  PE Dump Utility common structures and constants.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "exepe.h"
#include "exedos.h"


#ifndef COMMON_H
#define COMMON_H


/*** predefined resource type numbers ***/

#define RT_NONE                 0
#define RT_CURSOR               1
#define RT_BITMAP               2
#define RT_ICON                 3
#define RT_MENU                 4
#define RT_DIALOG               5
#define RT_STRING               6
#define RT_FONTDIR              7
#define RT_FONT                 8
#define RT_ACCELERATOR          9
#define RT_RCDATA               10      /* note: no type 11 resource */
#define RT_GROUP_CURSOR         12      /* note: no type 13 resource */
#define RT_GROUP_ICON           14

#define RT_COUNT                15


typedef enum {
    false = 0,
    true
} bool;

typedef enum {
    TABLE,
    DATA
} TableOrData;

typedef enum {
    NAME,
    ID
} NameOrID;

typedef struct ExeFile {
    FILE                        *file;
    dos_exe_header               dosHdr;
    pe_header                    pexHdr;
    unsigned_32                  pexHdrAddr;
    pe_object                    resObj;
    long int                     resObjAddr;
    struct ResTableEntry        *tabEnt;
} ExeFile;

typedef struct ResTableEntry {
    resource_dir_header  header;
    struct ResDirEntry  *dirs;
} ResTableEntry;

typedef struct ResDirEntry {
    resource_dir_entry   dir;
    ResTableEntry       *table;
    struct ResDataEntry *data;
    NameOrID             nameID;
    TableOrData          entryType;
    unsigned_16         *name;
    unsigned_16          nameSize;
} ResDirEntry;

typedef struct ResDataEntry {
    resource_entry       entry;
} ResDataEntry;


#endif

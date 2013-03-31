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
* Description:  Watcom resource file format structures and constants.
*
****************************************************************************/


#ifndef FILEFMT_INCLUDED
#define FILEFMT_INCLUDED

#include "pushpck1.h"
typedef struct WResHeader {
    uint_32     Magic[ 2 ];     /* must be WRESMAGIC0 and WRESMAGIC1 */
    uint_32     DirOffset;      /* offset to the start of the directory */
    uint_16     NumResources;   /* number of resourses in the file */
    uint_16     NumTypes;       /* number of different types of resources in file */
    uint_16     WResVer;        /* WRESVERSION */
} WResHeader;

enum {
    WRES_OS_WIN16 = 1,
    WRES_OS_WIN32,
    WRES_OS_OS2
};
typedef uint_16 WResTargetOS;

typedef struct WResExtHeader {  /* Only present if WResVer >= 1 */
    WResTargetOS        TargetOS;
    uint_16             reserved[ 4 ]; /* reserved for future use */
} WResExtHeader;

#define WRESMAGIC0  0xC3D4C1D7
#define WRESMAGIC1  0xC3D2CDCF
#define WRESVERSION 3       /* set to version number of the file format */


typedef struct WResIDName {             /* this is a "Pascal style" string */
    uint_16     NumChars;               /* length */
    char        Name[1];                /* followed by that many characters */
} _WCUNALIGNED WResIDName;              /* NOTE: there is no trailing '\0' */

typedef struct WResID {
    uint_8      IsName; /* if TRUE then ID is a Name otherwise Num */
    union {
        uint_16     Num;        /* number of the ID */
        WResIDName  Name;       /* name of the ID */
    } ID;
} _WCUNALIGNED WResID;

typedef struct WResHelpID {
    uint_8      IsName; /* if TRUE then ID is a Name otherwise Num */
    union {
        uint_32     Num;        /* number of the Help ID */
        WResIDName  Name;       /* name of the Help ID */
    } ID;
} _WCUNALIGNED WResHelpID;

typedef struct WResTypeInfo {
    uint_16     NumResources;/* number of resources of this type */
    WResID      TypeName;
} WResTypeInfo;


/*
 * WResID2, WResIDName2, WResResInfo2 are used in version 2 and below
 */
typedef struct WResIDName2 {            /* this is a "Pascal style" string */
    uint_8      NumChars;               /* length */
    char        Name[1];                /* followed by that many characters */
} WResIDName2;                          /* NOTE: there is no trailing '\0' */

typedef struct WResID2 {
    uint_8      IsName; /* if TRUE then ID is a Name otherwise Num */
    union {
        uint_16     Num;        /* number of the ID */
        WResIDName2 Name;       /* name of the ID */
    } ID;
} _WCUNALIGNED WResID2;

typedef struct WResResInfo2 {
    uint_16     NumResources;   /* # of resources of this type and name */
    WResID2     ResName;
} WResResInfo2;

typedef struct WResTypeInfo2 {
    uint_16     NumResources;   /* number of resources of this type */
    WResID2     TypeName;
} WResTypeInfo2;


/*
 * WResResInfo1 is the structure used in version 1 and below
 */
typedef struct WResResInfo10 {
    uint_16     MemoryFlags;
    uint_32     Offset;/* offset of resource body in file */
    uint_32     Length;/* length in bytes of resource body */
    WResID2     ResName;
} _WCUNALIGNED WResResInfo1;


typedef struct WResResInfo {
    uint_16     NumResources;   /* # of resources of this type and name */
    WResID      ResName;
} WResResInfo;

typedef struct WResLangType {
    uint_16     lang;
    uint_8      sublang;
}WResLangType;

typedef struct WResLangInfo {
    WResLangType        lang;
    uint_16             MemoryFlags;
    uint_32             Offset; /* offset of resource body in file */
    uint_32             Length; /* length in bytes of resource body */
} _WCUNALIGNED WResLangInfo;
#include "poppck.h"

#if 0
/* Wes uses this stuff in the resource editors - not really file formats, don't have to be packed */
#define WRESMAXCHUNK  ( 31 * 1024 )

typedef struct WResDataChunk {
    uint_16              size;
    void                 *data;
    struct WResDataChunk *next;
}WResDataChunk;
#endif

#endif

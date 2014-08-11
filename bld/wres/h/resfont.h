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


#ifndef RESFONT_INCLUDED
#define RESFONT_INCLUDED

#include "pushpck1.h"
typedef struct FontInfo {
    uint_16         dfVersion;
    uint_32         dfSize;
    char            dfCopyright[ 60 ];
    uint_16         dfType;     // 0 == bitmap, 1 == vector
    uint_16         dfPoints;
    uint_16         dfVertRes;
    uint_16         dfHorizRes;
    uint_16         dfAscent;
    uint_16         dfInternalLeading;
    uint_16         dfExternalLeading;
    uint_8          dfItalic;
    uint_8          dfUnderline;
    uint_8          dfStrikeOut;
    uint_16         dfWeight;
    uint_8          dfCharSet;
    uint_16         dfPixWidth;
    uint_16         dfPixHeight;
    uint_8          dfPitchAndFamily;
    uint_16         dfAvgWidth;
    uint_16         dfMaxWidth;
    uint_8          dfFirstChar;
    uint_8          dfLastChar;
    uint_8          dfDefaultChar;
    uint_8          dfBreakChar;
    uint_16         dfWidthBytes;
    uint_32         dfDevice;       /* offset from top of struct to Dev Name */
    uint_32         dfFace;         /* offset from top of struct to Face Name */
    uint_32         dfBitsPointer;
} _WCUNALIGNED FontInfo;

typedef struct FontDirEntry {
    uint_16         StructSize;     /* See note below */
    uint_16         FontID;
    FontInfo        Info;
    char            DevAndFaceName[ 1 ];
} FontDirEntry;
#include "poppck.h"

/* NOTE: The StructSize field is not acutally part of the directory entry and */
/* is not output to the disk. It is used during output to allow the chars in */
/* DevAndFaceName to be output. It should be set to size of the FonDirEntry */
/* including the char in DevAndFaceName but not StrucSize */

extern bool ResWriteFontInfo( FontInfo * info, WResFileID handle );
extern bool ResWriteFontDirEntry( FontDirEntry * entry, WResFileID handle );
/* this funtion expects the StructSize field to be set */

#endif

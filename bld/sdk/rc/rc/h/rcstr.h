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


#ifndef RCSTR_INCLUDED
#define RCSTR_INCLUDED

#include "wresall.h"

typedef struct StringItem32 {
    uint_16     NumChars;       /* length */
    char        Name[2];        /* followed by that many characters */
} StringItem32;

typedef struct StringItem16 {
    uint_8      NumChars;       /* length */
    char        Name[1];        /* followed by that many characters */
} StringItem16;

typedef struct StringBlock {
    int         UseUnicode;
    uint_16     StringBlockSize;
    void *      StringBlock;
    uint_16     StringListLen;  /* number of entries in the string list */
    void        **StringList;   /* sorted array of pointers to strings
                                 * in the StringBlock */
} StringBlock;


extern void StringBlockBuild( StringBlock *, WResDir, int use_unicode );
extern int_32 StringBlockFind( StringBlock *, WResIDName * name );
int CompareStringItems32( const StringItem32 *item1,
                          const StringItem32 *item2 );
#endif

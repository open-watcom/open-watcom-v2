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
* Description:  Help table structures and definitions.
*
****************************************************************************/


#ifndef SEMHELP_INCLUDED
#define SEMHELP_INCLUDED

#include "wresall.h"
#include "watcom.h"
#include "os2res.h"

/**** Semantic structures ****/

typedef struct FullHelpEntryOS2 {
    struct FullHelpEntryOS2 *  next;
    struct FullHelpEntryOS2 *  prev;
    HelpTableEntryOS2          entry;
} FullHelpEntryOS2;

typedef struct FullHelpTableOS2 {
    struct FullHelpEntryOS2 *  head;
    struct FullHelpEntryOS2 *  tail;
} FullHelpTableOS2;

typedef struct FullHelpSubEntryOS2 {
    struct FullHelpSubEntryOS2 *  next;
    struct FullHelpSubEntryOS2 *  prev;
    DataElemList               *  dataListHead;
} FullHelpSubEntryOS2;

typedef struct FullHelpSubTableOS2 {
    struct FullHelpSubEntryOS2 *  head;
    struct FullHelpSubEntryOS2 *  tail;
    int                           numWords;
} FullHelpSubTableOS2;

/**** Semantic routines ****/
extern FullHelpTableOS2 *SemOS2NewHelpTable( FullHelpEntryOS2 );
extern FullHelpTableOS2 *SemOS2AddHelpItem( FullHelpEntryOS2,
                                            FullHelpTableOS2 * );
extern FullHelpEntryOS2 SemOS2MakeHelpItem( unsigned long id1, unsigned long id2,
                                            unsigned long id3 );
extern void SemOS2WriteHelpTable( WResID * name, ResMemFlags flags,
                                  FullHelpTableOS2 * helptable );

extern FullHelpSubTableOS2 *SemOS2NewHelpSubTable( DataElemList * );
extern FullHelpSubTableOS2 *SemOS2AddHelpSubItem( DataElemList *,
                                            FullHelpSubTableOS2 * );
extern void SemOS2WriteHelpSubTable( WResID * name, int numWords, ResMemFlags flags,
                                     FullHelpSubTableOS2 * helptable );
#endif

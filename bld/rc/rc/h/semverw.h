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


#ifndef SEMVER_H_INCLUDED
#define SEMVER_H_INCLUDED

typedef struct VersionPair {
    uint_16     LowWord;
    uint_16     HighWord;
} VersionPair;

typedef struct VerFixedOption {
    YTOKEN      token;
    union {
        uint_32     Option;
        struct {
            VersionPair     Low;
            VersionPair     High;
        }           Version;
    } u;
} VerFixedOption;

typedef struct FullVerValueList {
    int             NumItems;
    VerValueItem *  Item;       // array of items
} FullVerValueList;

struct FullVerBlock;
typedef struct FullVerBlockNest {
    struct FullVerBlock *   Head;
    struct FullVerBlock *   Tail;
} FullVerBlockNest;

typedef struct FullVerBlock {
    struct FullVerBlock *   Next;
    struct FullVerBlock *   Prev;
    int                     UseUnicode;
    VerBlockHeader          Head;
    FullVerValueList *      Value;
    FullVerBlockNest *      Nest;
} FullVerBlock;

extern FullVerValueList *SemWINNewVerValueList( VerValueItem item );
extern FullVerValueList *SemWINAddVerValueList( FullVerValueList *, VerValueItem);
extern FullVerBlock     *SemWINNewBlockVal( char * name, FullVerValueList * list );
extern FullVerBlock     *SemWINNameVerBlock( char * name, FullVerBlockNest * nest );
extern FullVerBlockNest *SemWINNewBlockNest( FullVerBlock * child );
extern FullVerBlockNest *SemWINAddBlockNest( FullVerBlockNest *, FullVerBlock * );
extern FullVerBlockNest *SemWINMergeBlockNest( FullVerBlockNest * nest1,
                            FullVerBlockNest * nest2 );
extern VerFixedInfo     *SemWINNewVerFixedInfo( VerFixedOption option );
extern VerFixedInfo     *SemWINAddVerFixedInfo( VerFixedInfo * info, VerFixedOption );
extern void             SemWINWriteVerInfo( WResID * name, ResMemFlags flags,
                            VerFixedInfo * info, FullVerBlockNest * nest );

#endif

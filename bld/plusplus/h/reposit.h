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


#ifndef __REPOSIT_H__
#define __REPOSIT_H__

// REPOSIT -- simulation of repository

typedef union  repo_rec      REPO_REC;
typedef struct repo_rec_fun  REPO_REC_FUN;
typedef struct repo_rec_base REPO_REC_BASE;


struct repo_rec_base            // REPOS_REC_BASE -- repository record, base
{   REPO_REC *next;             // - next in ring
};


typedef enum                    // REPO_FUN_FLAGS -- flags for function
{   RFFLAG_CAN_LONGJUMP = 0x80000000 // - function could throw or equivalent
,   RFFLAG_NO_LONGJUMP  = 0x40000000 // - function never throws or equivalent
,   RFFLAG_IG_LONGJUMP  = 0x20000000 // - ignore (R/T, based on parameters)
} REPO_FUN_FLAGS;


struct repo_rec_fun             // REPO_REC_FUN -- repository record, function
{   REPO_REC_BASE base;         // - base
    NAME name;                  // - function name
    REPO_FUN_FLAGS flags;       // - flags
};

union repo_rec                  // REPO_REC -- union of all repository records
{   REPO_REC_BASE base;         // - base
    REPO_REC_FUN fun;           // - function
};


// PROTOTYPES:

REPO_REC_FUN* RepoFunAdd(       // ADD FUNCTION TO REPOSITORY
    NAME name,                  // - function name
    REPO_FUN_FLAGS flags )      // - function flags
;
REPO_REC_FUN* RepoFunRead(      // GET FUNCTION RECORD FROM REPOSITORY
    NAME name )                 // - function name
;
void RepoFunRelease(            // RELEASE FUNCTION RECORD
    REPO_REC_FUN* rec )         // - record to be released
;

#endif

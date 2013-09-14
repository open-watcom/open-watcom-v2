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
* Description:  msuffix.c interfaces
*
****************************************************************************/


#ifndef _MSUFFIX_H
#define _MSUFFIX_H  1

#include "mhash.h"
#include "mtarget.h"

typedef struct Suffix   SUFFIX;
typedef struct Creator  CREATOR;
typedef NODE            PATHRING;

/*
 * The suffix name (without '.') is stored in node.name.  PATHRING is a closed
 * linked-list containing the paths on which you can find this suffix.  'id'
 * is the ordering of the suffixes.  The first suffix on the .EXTENSIONS line
 * is given id 0, from there the id's increment by one.
 */
struct Suffix {
    /* node.name has been FixName'd() before being hashed */
    HASHNODE    node;
                /* these fields are private to msuffix.c */
    PATHRING    *first;         /* first path in ring               */
    PATHRING    *pathring;      /* current path in ring (.optimize) */
    UINT16      id;             /* id for this suffix               */
                /* these fields are public */
    CREATOR     *creator;       /* first creator for this suffix    */
};

/*
 * Creators are stored in ascending order of creator->suffix->id.  (This is to
 * implement the precedence that suffixes further to the left on the
 * .EXTENSIONS line have priority over those to the right.)  cretarg points
 * to a target with name ".src.dest" which describes the implicit rule.
 * (ie: cretarg->depend->clist are the commands to be executed.)
 */
struct Creator {
    CREATOR     *next;          /* next creator for this suffix     */
    SUFFIX      *suffix;        /* suffix node for this creation    */
    SLIST       *slist;
};

extern void     SuffixInit( void );
extern void     SuffixFini( void );

extern void     ClearSuffixes( void );
extern BOOLEAN  SufExists( const char *suf );
extern BOOLEAN  SufBothExist( const char *sufsuf );
extern void     AddSuffix( char *name );
extern void     SetSufPath( const char *name, const char *path );
extern void     AddCreator( const char *sufsuf, const char *fullsufsuf );
extern void     PrintSuffixes( void );
extern SUFFIX   *FindSuffix( const char *name );
extern RET_T    TrySufPath( char *buffer, const char *filename, TARGET **chktarg, BOOLEAN tryenv );

#endif


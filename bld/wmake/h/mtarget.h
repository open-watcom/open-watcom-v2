/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  mtarget.c interfaces
*
****************************************************************************/


#ifndef _MTARGET_H
#define _MTARGET_H  1

#include <time.h>

#include "mhash.h"
#include "mlex.h"

typedef struct TargAttr     TATTR;
typedef struct Target       TARGET;
typedef struct Depend       DEPEND;
typedef struct TargList     TLIST;
typedef struct CmdList      CLIST;
typedef struct fileStruct   FLIST;
typedef struct envStruct    ELIST;
typedef struct fileList     NKLIST;
typedef struct sufsufList   SLIST;

/*
 *  Attributes which are placed on the right hand side of a targ : dep line,
 *  and also may be passed in implicit rules.
 */
struct TargAttr {
    boolbit     multi       : 1;    /* .multiple flag                       */
    boolbit     precious    : 1;    /* .precious flag                       */
    boolbit     symbolic    : 1;    /* .symbolic flag                       */
    boolbit     explicit    : 1;    /* .explicit flag                       */
    boolbit     always      : 1;    /* .always flag                         */
    boolbit     auto_dep    : 1;    /* .auto_depend flag                    */
    boolbit     existsonly  : 1;    /* .existsonly flag                     */
    boolbit     recheck     : 1;    /*  re-check timestamp flag             */
};


struct Target {
    /*
     * used for all targets, dependents, implicit rules, and dot rules
     */

    /* these fields are for the private use of mtarget.c only! */
    HASHNODE    node;

    /*
     * node.name has been FixName()'d, and includes path.
     * each target "owns" its name (ie: targ->name = StrDupSafe( name ) )
     * name is read only - change with RenameTarget() only!
     */

    /* the rest is public */

    time_t      date;               /* last date read for target            */
    DEPEND      *depend;            /* NULL if no known dependents          */
    TATTR       attr;               /* attributes of target                 */

    /*
     * if !scolon and depend == NULL then "target" has only appeared on the
     * right side of a dependency.  (ie: it's only a dependent)
     */

    boolbit     updated     : 1;    /* target updated or not                */
    boolbit     busy        : 1;    /* recursion protection                 */
    boolbit     touched     : 1;    /* has target been touched?             */
    boolbit     existing    : 1;    /* does target exist?                   */
    boolbit     mentioned   : 1;    /* target mentioned in a makefile       */
    boolbit     executed    : 1;    /* cmds executed since last getdate     */
    boolbit     error       : 1;    /* error occured while updating target  */
    boolbit     scolon      : 1;    /* scolon == true; dcolon == false      */

    boolbit     special     : 1;    /* special targ - dotname|implicit rule */
    boolbit     before_after: 1;    /* is it .BEFORE or .AFTER              */
    boolbit     dot_default : 1;    /* is it a .DEFAULT                     */
    boolbit     backdated   : 1;    /* touched re: .JUST_ENOUGH */
    boolbit     allow_nocmd : 1;    /* allow no command list to update */
    boolbit     cmds_done   : 1;    /* command list was executed to update it */
    boolbit     sufsuf      : 1;    /* is this an implicit rule             */

};

/*
 * some special target cases:
 *
 * targ : .symbolic
 *      creates target 'targ' with targ->scolon && targ->depend != NULL &&
 *          targ->depend->tlist == NULL && targ->depend->clist == NULL
 *      a further line such as
 * targ : dep1 dep2
 *      would cause targ->depend->tlist != NULL
 *
 * Then if a further line said:
 * targ : dep3 dep4
 *      An Error 'coerced to double colon' would result since targ->scolon &&
 *      targ->depend != NULL && targ->depend->tlist != NULL.
 */

/*
 * Depend's are stored in order of appearence.  (ie: target->depend is the
 * first depend that appeared in the makefile for this target)
 */
struct Depend {
    DEPEND  *next;      /* next dependent for dcolon targets    */
    TLIST   *targs;     /* list of dependent targets            */
    CLIST   *clist;     /* list of commands to execute          */
};

struct fileStruct {
    char    *fileName;
    char    *body;
    bool    keep;
    FLIST   *next;
};

struct envStruct {
    char    *envVarName;
    char    *envOldVal;
    ELIST   *next;
};


struct fileList {
    char    *fileName;
    NKLIST  *next;
};


struct CmdList {
    CLIST   *next;      /* next command or NULL                      */
    FLIST   *inlineHead;/* contains the information for inline files */
                        /* associated with the command               */
    char    *text;      /* Text of command to execute                */
};

struct TargList {
    TLIST   *next;      /* next in list or NULL             */
    TARGET  *target;
};

struct sufsufList {
    SLIST   *next;
    char    *targ_path;
    char    *dep_path;
    TARGET  *cretarg;   /* target node for creation target  */
};


extern void     TargetInit( void );
extern void     TargetFini( void );

extern TLIST    *NewTList( void );
extern TARGET   *NewTarget( const char *name );
extern DEPEND   *NewDepend( void );
extern NKLIST   *NewNKList( void );
extern SLIST    *NewSList( void );
// extern ELIST    *NewEList( void );
extern FLIST    *NewFList( void );
extern CLIST    *NewCList( void );
extern CLIST    *DupCList( const CLIST *clist );
extern TLIST    *DupTList( const TLIST *old );
extern DEPEND   *DupDepend( const DEPEND *dep );
extern void     FreeTList( TLIST *tlist );
extern void     FreeNKList( NKLIST *rule );
extern void     FreeSList( SLIST *rule );
extern void     FreeEList( ELIST *rule );
extern void     FreeFList( FLIST *rule );
extern void     FreeCList( CLIST *rule );
extern void     FreeDepend( DEPEND *dep );
extern void     KillTarget( const char *name );

extern void     RenameTarget( const char *oldname, const char *newname );
extern TARGET   *FindTarget( const char *name );
extern void     PrintCList( const CLIST *list );
extern void     PrintTargFlags( const TATTR *tattr );
extern void     PrintTargets( void );
extern CLIST    *DotCList( DotName dot );
extern void     ResetExecuted( void );
extern void     CheckNoCmds( void );
extern bool     WildTList( TLIST **stack, const char *base, bool mentioned, bool expandWildCardPath );
extern void     TargInitAttr( TATTR *attr );
extern void     TargAttrOrAttr( TATTR *tattr, TATTR attr );

#endif  /* !_MTARGET_H */


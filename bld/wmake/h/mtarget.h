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

#define BEFORE_S   "BEFORE"
#define AFTER_S    "AFTER"
#define DEFAULT_S  "DEFAULT"

/*
 *  Attributes which are placed on the right hand side of a targ : dep line,
 *  and also may be passed in implicit rules.
 */
struct TargAttr {
    BIT     multi       : 1;    /* .multiple flag                       */
    BIT     prec        : 1;    /* .precious flag                       */
    BIT     symb        : 1;    /* .symbolic flag                       */
    BIT     explicit    : 1;    /* .explicit flag                       */
    BIT     always      : 1;    /* .always flag                         */
    BIT     auto_dep    : 1;    /* .auto_depend flag                    */
    BIT     existsonly  : 1;    /* .existsonly flag                     */
    BIT     recheck     : 1;    /*  re-check timestamp flag             */
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

    time_t      date;           /* last date read for target            */
    DEPEND      *depend;        /* NULL if no known dependents          */
    TATTR       attr;           /* attributes of target                 */

    /*
     * if !scolon and depend == NULL then "target" has only appeared on the
     * right side of a dependency.  (ie: it's only a dependent)
     */

    BIT     updated     : 1;    /* target updated or not                */
    BIT     busy        : 1;    /* recursion protection                 */
    BIT     touched     : 1;    /* has target been touched?             */
    BIT     existing    : 1;    /* does target exist?                   */
    BIT     mentioned   : 1;    /* target mentioned in a makefile       */
    BIT     executed    : 1;    /* cmds executed since last getdate     */
    BIT     error       : 1;    /* error occured while updating target  */
    BIT     scolon      : 1;    /* scolon == TRUE; dcolon == FALSE      */

    BIT     special     : 1;    /* special targ - dotname|implicit rule */
    BIT     before_after: 1;    /* is it .BEFORE or .AFTER              */
    BIT     dot_default : 1;    /* is it a .DEFAULT                     */
    BIT     backdated   : 1;    /* touched re: .JUST_ENOUGH */
    BIT     allow_nocmd : 1;    /* allow no command list to update */
    BIT     cmds_done   : 1;    /* command list was executed to update it */
    BIT     sufsuf      : 1;    /* is this an implicit rule             */

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
    SLIST   *slist;     /* list of suffixes with paths associated   */
    SLIST   *slistCmd;  /* pointer to the slist which contains the command */
                        /* list for implied targets */
};

struct fileStruct {
    char    *fileName;
    char    *body;
    BOOLEAN keep;
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
    char    *targ_path;
    char    *dep_path;
    CLIST   *clist;
    SLIST   *next;
};


extern const TATTR  FalseAttr;

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

extern void     RenameTarget( TARGET *targ, const char *newname );
extern TARGET   *FindTarget( const char *name );
extern void     PrintCList( const CLIST *list );
extern void     PrintTargFlags( const TATTR *tattr );
extern void     PrintTargets( void );
extern CLIST    *DotCList( DotName dot );
extern void     ResetExecuted( void );
extern void     CheckNoCmds( void );
extern RET_T    WildTList( TLIST **stack, const char *base, BOOLEAN mentioned,
                BOOLEAN expandWildCardPath );
extern void     TargInitAttr( TATTR *attr );
extern void     TargAttrOrAttr( TATTR *tattr, TATTR attr );

#endif  /* !_MTARGET_H */


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


#include <string.h>

#include "make.h"
#include "massert.h"
#include "memory.h"
#include "mhash.h"
#include "misc.h"
#include "mlex.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mtarget.h"
#include "mtypes.h"


/* just for people to copy in */
const TATTR FalseAttr = { FALSE, FALSE, FALSE, FALSE };

#define HASH_PRIME    211
#define CASESENSITIVE FALSE  // Is Target Name case sensitive

STATIC HASHTAB    *targTab;
STATIC DEPEND     *freeDepends;
STATIC TLIST      *freeTLists;
STATIC CLIST      *freeCLists;
STATIC FLIST      *freeFLists;
#ifdef CLEAN_ENVIRONMENT_VAR
STATIC ELIST      *freeELists;
#endif
STATIC NKLIST     *freeNKLists;
STATIC SLIST      *freeSLists;

#ifdef CLEAN_ENVIRONMENT_VAR
extern ELIST *NewEList( void )
/*****************************
 * allocate a FLIST, fill in default values
 */
{
    ELIST *e;

    if( freeELists != NULL ) {
        e = freeELists;
        freeELists = e->next;
        memset( e, 0, sizeof( *e ) );
        return( e );
    }
    return( (ELIST *) CallocSafe( sizeof( ELIST ) ) );
}
#endif

extern FLIST *NewFList( void )
/*****************************
 * allocate a FLIST, fill in default values
 */
{
    FLIST *f;

    if( freeFLists != NULL ) {
        f = freeFLists;
        freeFLists = f->next;
        memset( f, 0, sizeof( *f ) );
        return( f );
    }
    return( (FLIST *) CallocSafe( sizeof( FLIST ) ) );
}

extern NKLIST *NewNKList( void )
/*****************************
 * allocate a NKLIST, fill in default values
 */
{
    NKLIST *nk;

    if( freeNKLists != NULL ) {
        nk = freeNKLists;
        freeNKLists = nk->next;
        memset( nk, 0, sizeof( *nk ) );
        return( nk );
    }
    return( (NKLIST *) CallocSafe( sizeof( NKLIST ) ) );
}


extern SLIST *NewSList( void )
/*****************************
 * allocate a NKLIST, fill in default values
 */
{
    SLIST *s;

    if( freeSLists != NULL ) {
        s = freeSLists;
        freeSLists = s->next;
        memset( s, 0, sizeof( *s ) );
        return( s );
    }
    return( (SLIST *) CallocSafe( sizeof( SLIST ) ) );
}


extern TLIST *NewTList( void )
/*****************************
 * allocate a TLIST, fill in default values
 */
{
    TLIST *t;

    if( freeTLists != NULL ) {
        t = freeTLists;
        freeTLists = t->next;
        memset( t, 0, sizeof( *t ) );
        return( t );
    }
    return( (TLIST *)CallocSafe( sizeof( TLIST ) ) );
}


extern void RenameTarget( TARGET *targ, const char *newname )
/***********************************************************/
{
    (void) RemHashNode( targTab, targ->node.name, CASESENSITIVE );
    if( targ->node.name != NULL ) {
        FreeSafe( targ->node.name );
    }
    targ->node.name = FixName( StrDupSafe( newname ) );
    AddHashNode( targTab, (HASHNODE *)targ );
}


extern TARGET *NewTarget( const char *name )
/*******************************************
 * allocate a newtarget with name, and default values
 */
{
    TARGET *new;

    new = CallocSafe( sizeof( *new ) );
    new->executed = TRUE;
#if OLDEST_DATE != 0L
    new->date = OLDEST_DATE;
#endif
    new->node.name = FixName( StrDupSafe( name ) );
    AddHashNode( targTab, (HASHNODE *)new );

    return( new );
}


extern TARGET *FindTarget( const char *name )
/********************************************
 * be sure that name has been FixName'd!
 */
{
    assert( name != NULL );

    return( (TARGET *)FindHashNode( targTab, name, CASESENSITIVE ) );
}


#pragma on (check_stack);
extern CLIST *DotCList( enum DotNames dot )
/*****************************************
 * find clist associated with dotname
 */
{
    char    name[ MAX_DOT_NAME ];
    TARGET  *cur;

    name[ 0 ] = DOT;
    FixName( strcpy( name + 1, DotNames[ dot ] ) );

    cur = FindTarget( name );

    if( cur == NULL || cur->depend == NULL ) {
        return( NULL );
    }
    return( cur->depend->clist );
}
#pragma off(check_stack);


extern DEPEND *NewDepend( void )
/******************************/
{
    DEPEND *dep;

    if( freeDepends != NULL ) {
        dep = freeDepends;
        freeDepends = dep->next;
        memset( dep, 0, sizeof( *dep ) );
        return( dep );
    }
    return( (DEPEND *) CallocSafe( sizeof( DEPEND ) ) );
}


extern CLIST *NewCList( void )
/**************************/
{
    CLIST *c;

    if( freeCLists != NULL ) {
        c = freeCLists;
        freeCLists = c->next;
        memset( c, 0, sizeof( *c ) );
        return( c );
    }
    return( (CLIST *) CallocSafe( sizeof( CLIST ) ) );
}

// Duplicate the inline file information of the CLIST
STATIC FLIST *DupFList( FLIST *old )
/**********************************/
{
    FLIST *new;
    FLIST *cur;
    FLIST *head;

    if( old == NULL ) {
        return( NULL );
    }

    head = NewFList();
    head->fileName = StrDupSafe( old->fileName );
    if (old->body != NULL) {
        head->body     = StrDupSafe( old->body );
    } else {
        new->body      = NULL;
    }
    head->keep     = old->keep;

    cur = head;
    old = old->next;
    while( old != NULL ) {
        new = NewFList();
        new->fileName = StrDupSafe( old->fileName );
        if (old->body != NULL) {
            new->body     = StrDupSafe( old->body );
        } else {
            new->body      = NULL;
        }
        new->keep     = old->keep;
        cur->next     = new;

        cur = new;
        old = old->next;
    }

    return( head );
}

STATIC SLIST *DupSList( SLIST *old )
/**********************************/
{
    SLIST *new;
    SLIST *cur;
    SLIST *head;

    if( old == NULL ) {
        return( NULL );
    }

    head = NewSList();
    head->targ_path  = StrDupSafe( old->targ_path );
    head->dep_path   = StrDupSafe( old->dep_path );
    head->clist      = DupCList  ( old->clist );

    cur = head;
    old = old->next;
    while( old != NULL ) {
        new = NewSList();
        new->targ_path  = StrDupSafe( old->targ_path );
        new->dep_path   = StrDupSafe( old->dep_path );
        new->clist      = DupCList  ( old->clist );
        cur->next = new;
        cur = new;
        old = old->next;
    }

    return( head );
}


extern CLIST *DupCList( CLIST *old )
/**********************************/
{
    CLIST *new;
    CLIST *cur;
    CLIST *head;

    if( old == NULL ) {
        return( NULL );
    }

    head = NewCList();
    head->text       = StrDupSafe( old->text );
    head->inlineHead = DupFList  ( old->inlineHead );

    cur = head;
    old = old->next;
    while( old != NULL ) {
        new = NewCList();
        new->text       = StrDupSafe( old->text );
        new->inlineHead = DupFList  ( old->inlineHead );
        cur->next = new;

        cur = new;
        old = old->next;
    }

    return( head );
}

extern TLIST *DupTList ( TLIST * old)
/**********************************
 *  duplicate the tlist
 */
{
    TLIST *new;
    TLIST *currentNew;
    TLIST *currentOld;
    new = NULL;
    if ( old != NULL ) {
        new = NewTList();
        new ->target = old->target;
        currentNew = new;
        currentOld = old->next;
        while( currentOld != NULL ) {
            currentNew->next = NewTList();
            currentNew->next->target = currentOld->target;
            currentNew = currentNew->next;
            currentOld = currentOld->next;

        }
    }
    return (new);

}
extern DEPEND *DupDepend( DEPEND *old )
/*************************************/
/* doesn't recursively descend old->next, or old->targs->target->depend */
{
    DEPEND  *new;

    if( old == NULL ) {
        return( NULL );                 /* no need to dup */
    }

    new = NewDepend();
    new->targs    = DupTList(old->targs);
    new->clist    = DupCList( old->clist );
    new->slist    = DupSList( old->slist );
    new->slistCmd = old->slistCmd;

    return( new );
}


extern void FreeTList( TLIST *tlist )   /* non-recursive */
/***********************************/
{
    TLIST   *cur;

    while( tlist != NULL ) {
        cur = tlist;
        tlist = tlist->next;
        cur->next = freeTLists;
        freeTLists = cur;
    }
}

/* frees the no keep list */
extern void FreeNKList( NKLIST *nklist )   /* non-recursive */
/***********************************/
{
    NKLIST   *cur;

    while( nklist != NULL ) {
        cur = nklist;
        nklist = nklist->next;
        FreeSafe(cur->fileName);
        cur->next = freeNKLists;
        freeNKLists = cur;
    }
}

/* frees the sufsuf list */
extern void FreeSList( SLIST *slist )   /* non-recursive */
/***********************************/
{
    SLIST   *cur;

    while( slist != NULL ) {
        cur = slist;
        FreeSafe ( cur->targ_path);
        FreeSafe ( cur->dep_path);
        FreeCList( cur->clist );
        slist = slist->next;
        cur->next = freeSLists;
        freeSLists = cur;
    }
}


/* frees the inline file information for the clist */
extern void FreeFList( FLIST *flist )   /* non-recursive */
/***********************************/
{
    FLIST   *cur;

    while( flist != NULL ) {
        cur        = flist;
        flist      = flist->next;
        cur->next  = freeFLists;
        FreeSafe(cur->body);
        FreeSafe(cur->fileName);
        freeFLists = cur;
    }
}

#ifdef CLEAN_ENVIRONMENT_VAR
extern void FreeEList( ELIST *elist )   /* non-recursive */
/***********************************/
{
    ELIST   *cur;

    while( elist != NULL ) {
        cur        = elist;
        if (elist->envVarName != NULL) {
            FreeSafe(elist->envVarName);
        }
        elist      = elist->next;
        cur->next  = freeELists;
        freeELists = cur;
    }
}
#endif

extern void FreeCList( CLIST *clist )
/***********************************/
{
    CLIST    *cur;

    while( clist != NULL ) {
        cur = clist;
        clist = clist->next;
        FreeSafe( cur->text );
        FreeFList(cur->inlineHead);
        cur->next = freeCLists;
        freeCLists = cur;
    }
}


extern void FreeDepend( DEPEND *dep )   /* free's tlist, and clist */
/***********************************/
{
    DEPEND *cur;

    while( dep != NULL ) {
        cur = dep;
        dep = dep->next;
        FreeTList( cur->targs );
        FreeCList( cur->clist );
        FreeSList( cur->slist );
        cur->next = freeDepends;
        freeDepends = cur;
    }
}


STATIC void freeTarget( TARGET *targ )
/************************************/
{
    assert( targ != NULL );

    FreeDepend( targ->depend );
    FreeSafe( targ->node.name );
    FreeSafe( targ );
}


extern void KillTarget( const char *name )
/*****************************************
 * name is not FreeTarget because one must be careful when using this
 * function that the target is not a member of some TLIST
 */
{
    TARGET *kill;

    kill = (TARGET *) RemHashNode( targTab, name, CASESENSITIVE );
    if( kill != NULL ) {
        freeTarget( kill );
    }
}


STATIC TARGET *findOrNewTarget( const char *tname, BOOLEAN mentioned )
/*********************************************************************
 * Return a pointer to a target with name name.  Create target if necessary.
 */
{
    char    name[ _MAX_PATH ];
    TARGET  *targ;

    targ = FindTarget( FixName( strcpy( name, tname ) ) );
    if( targ == NULL ) {
        targ = NewTarget( name );
        if( name[ 0 ] == DOT && isextc( name[ 1 ] ) ) {
            targ->special = TRUE;
            if (strcmpi(name+1,BEFORE_S) == 0 ||
                strcmpi(name+1,AFTER_S)  == 0 ) {
                targ->before_after = TRUE;
            }
            if (strcmpi(name+1,DEFAULT_S) == 0) {
                targ->dot_default = TRUE;
            }
        }
    }

         /* mentioned in a makefile */
    targ->mentioned = targ->mentioned || mentioned;

    return( targ );
}


extern RET_T WildTList( TLIST **list, const char *base, BOOLEAN mentioned ,
                        BOOLEAN expandWildCardPath)
/***************************************************************************
 * Build a TLIST using base as a wildcarded path.  Uses DoWildCard().
 * Pushes targets onto list.
 */
{
    TARGET      *targ;
    TLIST       *new;
    TLIST       *temp;
    TLIST       *temp2;
    TLIST       *current;
    TLIST       *endOfList;
    const char  *file;


    if (expandWildCardPath) {
        /* we want to expand the wildCard path */
        file = DoWildCard( base );
        if( strpbrk( file, WILD_METAS ) != NULL ) {
            PrtMsg( ERR|LOC| NO_EXISTING_FILE_MATCH, file );
            return( RET_ERROR );
        }
    } else {
        file = base;
    }

    temp = NULL;
    while( file != NULL ) {
        targ = findOrNewTarget( file, mentioned );
        new  = NewTList();
        new->target = targ;
        new->next   = temp;
        temp = new;
        file = DoWildCard( NULL );
    }


    /* reverse the target list  */
    current = temp;
    temp    = temp->next;
    current->next = NULL;

    while (temp != NULL) {
        temp2      = temp->next;
        temp->next = current;
        current    = temp;
        temp       = temp2;
    }
    if (*list == NULL) {
        *list = current;
    } else {
        endOfList = *list;
        while (endOfList->next != NULL) {
            endOfList = endOfList->next;
        }
        endOfList->next = current;
    }
    return( RET_SUCCESS );
}



extern void PrintCList( CLIST *clist )
/************************************/
{
    for( ; clist != NULL; clist = clist->next ) {
        PrtMsg( INF|NEOL| JUST_A_TAB );
        PrtMsg( INF| PRNTSTR, clist->text );
    }
}


extern void PrintTargFlags( TARGET *targ )
/****************************************/
{
    if( targ->attr.prec ) {
        PrtMsg( INF|NEOL| PTARG_DOTNAME, DotNames[ DOT_PRECIOUS ] );
    }
    if( targ->attr.symb ) {
        PrtMsg( INF|NEOL| PTARG_DOTNAME, DotNames[ DOT_SYMBOLIC ] );
    }
    if( targ->attr.multi ) {
        PrtMsg( INF|NEOL| PTARG_DOTNAME, DotNames[ DOT_MULTIPLE ] );
    }
    if( targ->attr.explicit ) {
        PrtMsg( INF|NEOL| PTARG_DOTNAME, DotNames[ DOT_EXPLICIT ] );
    }
    if( targ->attr.always ) {
        PrtMsg( INF|NEOL| PTARG_DOTNAME, DotNames[ DOT_ALWAYS ] );
    }
    if( targ->attr.auto_dep ) {
        PrtMsg( INF|NEOL| PTARG_DOTNAME, DotNames[ DOT_AUTO_DEPEND ] );
    }
}


#pragma off(unreferenced);
STATIC BOOLEAN printTarg( void *node, void *ptr )
#pragma on (unreferenced);
/***********************************************/
{
    TARGET  *targ = node;
    DEPEND  *curdep;
    TLIST   *curtlist;

    if( targ->special ) {
        return( FALSE );             /* don't print special targets */
    } else {
        if( !targ->scolon && targ->depend == NULL ) {
            PrtMsg( INF|NEOL| PTARG_NAME, targ->node.name );
        } else {
            PrtMsg( INF|NEOL| PTARG_IS_TYPE_M, targ->node.name,
                targ->scolon ? M_SCOLON : M_DCOLON );
        }
        PrintTargFlags( targ );
        PrtMsg( INF| NEWLINE );
    }
    if( targ->depend ) {
        curdep = targ->depend;
        while( curdep != NULL ) {
            if( curdep->targs != NULL ) {
                PrtMsg( INF| PTARG_DEPENDS_ON );
                for( curtlist = curdep->targs; curtlist != NULL;
                    curtlist = curtlist->next ) {
                    PrtMsg( INF| PTARG_TAB_TAB_ENV,
                        curtlist->target->node.name );
                }
            }
            if( curdep->clist ) {
                PrtMsg( INF| PTARG_WOULD_EXECUTE_CMDS );
                PrintCList( curdep->clist );
            }
            curdep = curdep->next;
            if( curdep != NULL ) {
                PrtMsg( INF| NEWLINE );
            }
        }
    } else {
        PrtMsg( INF| PTARG_NO_DEPENDENTS );
    }
    PrtMsg( INF| NEWLINE );

    return( FALSE );
}


STATIC void printDot( enum DotNames dot )
/***************************************/
{
    char    buf[ MAX_DOT_NAME ];
    CLIST   *cmds;

    cmds = DotCList( dot );
    if( cmds == NULL ) {
        return;
    }
    FmtStr( buf, ".%s", DotNames[ dot ] );
    PrtMsg( INF| PDOT_CMDS, buf );
    PrintCList( cmds );
    PrtMsg( INF| NEWLINE );
}


extern void PrintTargets( void )
/******************************/
{
    enum DotNames   i;

    for( i = DOT_MIN; i < DOT_MAX; ++i ) {
        if( IsDotWithCmds( i ) ) {
            printDot( i );
        }
    }

    WalkHashTab( targTab, printTarg, NULL );
}

extern void TargInitAttr( TATTR *attr )
/*************************************/
{
    attr->prec = FALSE;
    attr->symb = FALSE;
    attr->multi = FALSE;
    attr->explicit = FALSE;
    attr->always = FALSE;
    attr->auto_dep = FALSE;
}

extern void TargOrAttr( TARGET *targ, TATTR attr )
/************************************************/
{
    targ->attr.prec |= attr.prec;
    targ->attr.symb |= attr.symb;
    targ->attr.multi |= attr.multi;
    targ->attr.explicit |= attr.explicit;
    targ->attr.always |= attr.always;
    targ->attr.auto_dep |= attr.auto_dep;
}


#pragma off(unreferenced);
STATIC BOOLEAN resetEx( TARGET *targ, void *ptr )
#pragma on (unreferenced);
/***********************************************/
{
    targ->executed = TRUE;
    return( FALSE );
}

extern void ResetExecuted( void )
/*******************************/
{
    WalkHashTab( targTab, resetEx, NULL );
}

STATIC RET_T cleanupLeftovers( void )
/***********************************/
{
    DEPEND     *dep;
    CLIST      *c;
    SLIST      *s;
    TLIST      *t;
    FLIST      *f;
#ifdef CLEAN_ENVIRONMENT_VAR
    ELIST      *e;
#endif
    NKLIST *nk;

    if( freeDepends != NULL ) {
        do {
            dep = freeDepends;
            freeDepends = dep->next;
            FreeSafe( dep );
        } while( freeDepends != NULL );
        return( RET_SUCCESS );
    }
    if( freeTLists != NULL ) {
        do {
            t = freeTLists;
            freeTLists = t->next;
            FreeSafe( t );
        } while( freeTLists != NULL );
        return( RET_SUCCESS );
    }
    if( freeNKLists != NULL ) {
        do {
            nk = freeNKLists;
            freeNKLists = nk->next;
            FreeSafe( nk );
        } while( freeNKLists != NULL );
        return( RET_SUCCESS );
    }
    if( freeSLists != NULL ) {
        do {
            s = freeSLists;
            freeSLists = s->next;
            FreeSafe( s );
        } while( freeSLists != NULL );
        return( RET_SUCCESS );
    }
    if( freeFLists != NULL ) {
        do {
            f = freeFLists;
            freeFLists = f->next;
            FreeSafe( f );
        } while( freeFLists != NULL );
        return( RET_SUCCESS );
    }
#ifdef CLEAN_ENVIRONMENT_VAR
    if( freeELists != NULL ) {
        do {
            e = freeELists;
            freeELists = e->next;
            FreeSafe( e );
        } while( freeELists != NULL );
        return( RET_SUCCESS );
    }
#endif
    if( freeCLists != NULL ) {
        do {
            c = freeCLists;
            freeCLists = c->next;
            FreeSafe( c );
        } while( freeCLists != NULL );
        return( RET_SUCCESS );
    }
    return( RET_ERROR );
}

extern void TargetInit( void )
/****************************/
{
    targTab = NULL;
    freeDepends = NULL;
    freeTLists = NULL;
    freeCLists = NULL;
    freeFLists = NULL;
#ifdef CLEAN_ENVIRONMENT_VAR
    freeELists = NULL;
#endif
    freeNKLists = NULL;
    freeSLists = NULL;
    targTab = NewHashTab( HASH_PRIME );
    IfMemScarce( cleanupLeftovers );
}

#ifndef NDEBUG
#pragma off(unreferenced);
STATIC BOOLEAN walkFree( TARGET *targ, void *ptr )
#pragma on (unreferenced);
/************************************************/
{
    freeTarget( targ );
    return( FALSE );
}
#endif


extern void TargetFini( void )
/****************************/
{
#ifndef NDEBUG
    WalkHashTab( targTab, walkFree, NULL );
    FreeHashTab( targTab );
    targTab = NULL;
    while (cleanupLeftovers() != RET_ERROR);
#endif
}

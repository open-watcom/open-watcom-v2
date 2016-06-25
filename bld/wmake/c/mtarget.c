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
* Description:  Target management routines.
*
****************************************************************************/


#include "make.h"
#include "mmemory.h"
#include "mhash.h"
#include "mmisc.h"
#include "mlex.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mtarget.h"

#include "clibext.h"


/* just for people to copy in */
const TATTR FalseAttr = { false, false, false, false, false, false, false };

#define HASH_PRIME    211
#define CASESENSITIVE false  // Is Target Name case sensitive

STATIC HASHTAB    *targTab;
STATIC DEPEND     *freeDepends;
STATIC TLIST      *freeTLists;
STATIC CLIST      *freeCLists;
STATIC FLIST      *freeFLists;
STATIC NKLIST     *freeNKLists;
STATIC SLIST      *freeSLists;


FLIST *NewFList( void )
/**********************
 * allocate a FLIST, fill in default values
 */
{
    FLIST   *f;

    if( freeFLists != NULL ) {
        f = freeFLists;
        freeFLists = f->next;
        memset( f, 0, sizeof( *f ) );
        return( f );
    }
    return( (FLIST *)CallocSafe( sizeof( FLIST ) ) );
}


NKLIST *NewNKList( void )
/************************
 * allocate a NKLIST, fill in default values
 */
{
    NKLIST  *nk;

    if( freeNKLists != NULL ) {
        nk = freeNKLists;
        freeNKLists = nk->next;
        memset( nk, 0, sizeof( *nk ) );
        return( nk );
    }
    return( (NKLIST *)CallocSafe( sizeof( NKLIST ) ) );
}


SLIST *NewSList( void )
/**********************
 * allocate a NKLIST, fill in default values
 */
{
    SLIST   *s;

    if( freeSLists != NULL ) {
        s = freeSLists;
        freeSLists = s->next;
        memset( s, 0, sizeof( *s ) );
        return( s );
    }
    return( (SLIST *)CallocSafe( sizeof( SLIST ) ) );
}


TLIST *NewTList( void )
/**********************
 * allocate a TLIST, fill in default values
 */
{
    TLIST   *t;

    if( freeTLists != NULL ) {
        t = freeTLists;
        freeTLists = t->next;
        memset( t, 0, sizeof( *t ) );
        return( t );
    }
    return( (TLIST *)CallocSafe( sizeof( TLIST ) ) );
}


void RenameTarget( TARGET *targ, const char *newname )
/****************************************************/
{
    (void)RemHashNode( targTab, targ->node.name, CASESENSITIVE );
    if( targ->node.name != NULL ) {
        FreeSafe( targ->node.name );
    }
    targ->node.name = FixName( StrDupSafe( newname ) );
    AddHashNode( targTab, (HASHNODE *)targ );
}


TARGET *NewTarget( const char *name )
/************************************
 * allocate a newtarget with name, and default values
 */
{
    TARGET  *new;

    new = CallocSafe( sizeof( *new ) );
    new->executed = true;
    new->date = OLDEST_DATE;
    new->node.name = FixName( StrDupSafe( name ) );
    AddHashNode( targTab, (HASHNODE *)new );

    return( new );
}


TARGET *FindTarget( const char *name )
/*************************************
 * be sure that name has been FixName'd!
 */
{
    assert( name != NULL );

    return( (TARGET *)FindHashNode( targTab, name, CASESENSITIVE ) );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
CLIST *DotCList( DotName dot )
/***********************************
 * find clist associated with dotname
 */
{
    char                name[MAX_DOT_NAME];
    TARGET const        *cur;

    name[0] = DOT;
    FixName( strcpy( name + 1, DotNames[dot] ) );

    cur = FindTarget( name );

    if( cur == NULL || cur->depend == NULL ) {
        return( NULL );
    }
    return( cur->depend->clist );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


DEPEND *NewDepend( void )
/***********************/
{
    DEPEND  *dep;

    if( freeDepends != NULL ) {
        dep = freeDepends;
        freeDepends = dep->next;
        memset( dep, 0, sizeof( *dep ) );
        return( dep );
    }
    return( (DEPEND *)CallocSafe( sizeof( DEPEND ) ) );
}


CLIST *NewCList( void )
/*********************/
{
    CLIST   *c;

    if( freeCLists != NULL ) {
        c = freeCLists;
        freeCLists = c->next;
        memset( c, 0, sizeof( *c ) );
        return( c );
    }
    return( (CLIST *)CallocSafe( sizeof( CLIST ) ) );
}


STATIC FLIST *DupFList( const FLIST *old )
/*****************************************
 * Duplicate the inline file information of the CLIST
 */
{
    FLIST   *new;
    FLIST   *cur;
    FLIST   *head;  // resulting FLIST

    if( old == NULL ) {
        return( NULL );
    }

    head = NewFList();
    head->fileName = StrDupSafe( old->fileName );
    if( old->body != NULL ) {
        head->body = StrDupSafe( old->body );
    } else {
        head->body = NULL;
    }
    head->keep     = old->keep;

    cur = head;
    for( old = old->next; old != NULL; old = old->next ) {
        new = NewFList();
        new->fileName = StrDupSafe( old->fileName );
        if( old->body != NULL ) {
            new->body = StrDupSafe( old->body );
        } else {
            new->body = NULL;
        }
        new->keep     = old->keep;
        cur->next     = new;

        cur = new;
    }

    return( head );
}


CLIST *DupCList( const CLIST *old )
/*********************************/
{
    CLIST   *new;
    CLIST   *cur;
    CLIST   *head;

    if( old == NULL ) {
        return( NULL );
    }

    head = NewCList();
    head->text       = StrDupSafe( old->text );
    head->inlineHead = DupFList( old->inlineHead );

    cur = head;
    for( old = old->next; old != NULL; old = old->next ) {
        new = NewCList();
        new->text       = StrDupSafe( old->text );
        new->inlineHead = DupFList( old->inlineHead );
        cur->next = new;

        cur = new;
    }

    return( head );
}


TLIST *DupTList( const TLIST * old )
/***********************************
 *  duplicate the tlist
 */
{
    TLIST              *new;
    TLIST              *currentNew;
    TLIST const        *currentOld;

    new = NULL;
    if( old != NULL ) {
        new = NewTList();
        new ->target = old->target;
        currentNew = new;
        for( currentOld = old->next; currentOld != NULL; currentOld = currentOld->next ) {
            currentNew->next = NewTList();
            currentNew->next->target = currentOld->target;
            currentNew = currentNew->next;
        }
    }
    return( new );
}


DEPEND *DupDepend( const DEPEND *old )
/*************************************
 * doesn't recursively descend old->next, or old->targs->target->depend
 */
{
    DEPEND  *new;

    if( old == NULL ) {
        return( NULL );                 /* no need to dup */
    }

    new = NewDepend();
    new->targs = DupTList( old->targs );
    new->clist = DupCList( old->clist );

    return( new );
}


void FreeTList( TLIST *tlist )   /* non-recursive */
/****************************/
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
void FreeNKList( NKLIST *nklist )   /* non-recursive */
/*******************************/
{
    NKLIST  *cur;

    while( nklist != NULL ) {
        cur = nklist;
        nklist = nklist->next;
        FreeSafe( cur->fileName );
        cur->next = freeNKLists;
        freeNKLists = cur;
    }
}


/* frees the sufsuf list */
void FreeSList( SLIST *slist )   /* non-recursive */
/****************************/
{
    SLIST   *cur;

    while( slist != NULL ) {
        cur = slist;
        if( *cur->targ_path != NULLCHAR ) {
            FreeSafe( cur->targ_path );
        }
        if( *cur->dep_path != NULLCHAR ) {
            FreeSafe( cur->dep_path );
        }
        KillTarget( cur->cretarg->node.name );
        slist = slist->next;
        cur->next = freeSLists;
        freeSLists = cur;
    }
}


void FreeFList( FLIST *flist )   /* non-recursive */
/*****************************
 * frees the inline file information for the clist
 */
{
    FLIST   *cur;

    while( flist != NULL ) {
        cur        = flist;
        flist      = flist->next;
        cur->next  = freeFLists;
        FreeSafe( cur->body );
        FreeSafe( cur->fileName );
        freeFLists = cur;
    }
}


void FreeCList( CLIST *clist )
/****************************/
{
    CLIST   *cur;

    while( clist != NULL ) {
        cur = clist;
        clist = clist->next;
        FreeSafe( cur->text );
        FreeFList( cur->inlineHead );
        cur->next = freeCLists;
        freeCLists = cur;
    }
}


void FreeDepend( DEPEND *dep )
/*****************************
 * frees tlist, and clist
 */
{
    DEPEND  *cur;

    while( dep != NULL ) {
        cur = dep;
        dep = dep->next;
        FreeTList( cur->targs );
        FreeCList( cur->clist );
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


void KillTarget( const char *name )
/**********************************
 * name is not FreeTarget because one must be careful when using this
 * function that the target is not a member of some TLIST
 */
{
    void    *mykill;

    mykill = RemHashNode( targTab, name, CASESENSITIVE );
    if( mykill != NULL ) {
        freeTarget( mykill );
    }
}


STATIC TARGET *findOrNewTarget( const char *tname, bool mentioned )
/******************************************************************
 * Return a pointer to a target with name name.  Create target if necessary.
 */
{
    char    name[_MAX_PATH];
    TARGET  *targ;

    targ = FindTarget( FixName( strcpy( name, tname ) ) );
    if( targ == NULL ) {
        targ = NewTarget( name );
        if( name[0] == DOT && isextc( name[1] ) ) {
            targ->special = true;
            if( stricmp( name + 1, BEFORE_S ) == 0 ||
                stricmp( name + 1, AFTER_S )  == 0 ) {
                targ->before_after = true;
            }
            if( stricmp( name + 1, DEFAULT_S ) == 0 ) {
                targ->dot_default = true;
            }
        }
    }

    /* mentioned in a makefile */
    targ->mentioned = targ->mentioned || mentioned;

    return( targ );
}


RET_T WildTList( TLIST **list, const char *base, bool mentioned,
                 bool expandWildCardPath )
/***************************************************************
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

    if( expandWildCardPath ) {
        /* we want to expand the wildCard path */
        file = DoWildCard( base );
        assert( file != NULL );
        if( strpbrk( file, WILD_METAS ) != NULL ) {
            PrtMsg( ERR | LOC | NO_EXISTING_FILE_MATCH, file );
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
    assert( temp );
    current = temp;
    temp    = temp->next;
    current->next = NULL;

    while( temp != NULL ) {
        temp2      = temp->next;
        temp->next = current;
        current    = temp;
        temp       = temp2;
    }
    if( *list == NULL ) {
        *list = current;
    } else {
        endOfList = *list;
        while( endOfList->next != NULL ) {
            endOfList = endOfList->next;
        }
        endOfList->next = current;
    }
    return( RET_SUCCESS );
}


void PrintCList( const CLIST *clist )
/***********************************/
{
    for( ; clist != NULL; clist = clist->next ) {
        PrtMsg( INF | NEOL | JUST_A_TAB );
        PrtMsg( INF | PRNTSTR, clist->text );
    }
}


void PrintTargFlags( const TATTR *tattr )
/***************************************/
{
    if( tattr->precious ) {
        PrtMsg( INF | NEOL | PTARG_DOTNAME, DotNames[DOT_PRECIOUS] );
    }
    if( tattr->symbolic ) {
        PrtMsg( INF | NEOL | PTARG_DOTNAME, DotNames[DOT_SYMBOLIC] );
    }
    if( tattr->multi ) {
        PrtMsg( INF | NEOL | PTARG_DOTNAME, DotNames[DOT_MULTIPLE] );
    }
    if( tattr->explicit ) {
        PrtMsg( INF | NEOL | PTARG_DOTNAME, DotNames[DOT_EXPLICIT] );
    }
    if( tattr->always ) {
        PrtMsg( INF | NEOL | PTARG_DOTNAME, DotNames[DOT_ALWAYS] );
    }
    if( tattr->auto_dep ) {
        PrtMsg( INF | NEOL | PTARG_DOTNAME, DotNames[DOT_AUTO_DEPEND] );
    }
}


STATIC bool printTarg( void *node, void *ptr )
/********************************************/
{
    TARGET const * const    targ = node;
    DEPEND const            *curdep;
    TLIST const             *curtlist;

    (void)ptr; // Unused
    if( targ->special ) {
        return( false );             /* don't print special targets */
    } else {
        if( !targ->scolon && targ->depend == NULL ) {
            PrtMsg( INF | NEOL | PTARG_NAME, targ->node.name );
        } else {
            PrtMsg( INF | NEOL | PTARG_IS_TYPE_M, targ->node.name,
                targ->scolon ? M_SCOLON : M_DCOLON );
        }
        PrintTargFlags( &targ->attr );
        PrtMsg( INF | NEWLINE );
    }
    if( targ->depend != NULL ) {
        curdep = targ->depend;
        while( curdep != NULL ) {
            if( curdep->targs != NULL ) {
                PrtMsg( INF | PTARG_DEPENDS_ON );
                for( curtlist = curdep->targs; curtlist != NULL;
                    curtlist = curtlist->next ) {
                    PrtMsg( INF | PTARG_TAB_TAB_ENV,
                        curtlist->target->node.name );
                }
            }
            if( curdep->clist != NULL ) {
                PrtMsg( INF | PTARG_WOULD_EXECUTE_CMDS );
                PrintCList( curdep->clist );
            }
            curdep = curdep->next;
            if( curdep != NULL ) {
                PrtMsg( INF | NEWLINE );
            }
        }
    } else {
        PrtMsg( INF | PTARG_NO_DEPENDENTS );
    }
    PrtMsg( INF | NEWLINE );

    return( false );
}


STATIC void printDot( DotName dot )
/***************************************/
{
    char                buf[MAX_DOT_NAME];
    CLIST const         *cmds;

    cmds = DotCList( dot );
    if( cmds == NULL ) {
        return;
    }
    FmtStr( buf, ".%s", DotNames[dot] );
    PrtMsg( INF | PDOT_CMDS, buf );
    PrintCList( cmds );
    PrtMsg( INF | NEWLINE );
}


void PrintTargets( void )
/***********************/
{
    DotName i;

    for( i = DOT_MIN; i < DOT_MAX; ++i ) {
        if( IsDotWithCmds( i ) ) {
            printDot( i );
        }
    }

    WalkHashTab( targTab, printTarg, NULL );
}


void TargInitAttr( TATTR *attr )
/******************************/
{
    attr->precious   = false;
    attr->symbolic   = false;
    attr->multi      = false;
    attr->explicit   = false;
    attr->always     = false;
    attr->auto_dep   = false;
    attr->existsonly = false;
    attr->recheck    = false;
}


void TargAttrOrAttr( TATTR *tattr, TATTR attr )
/*********************************************/
{
    tattr->precious   |= attr.precious;
    tattr->symbolic   |= attr.symbolic;
    tattr->multi      |= attr.multi;
    tattr->explicit   |= attr.explicit;
    tattr->always     |= attr.always;
    tattr->auto_dep   |= attr.auto_dep;
    tattr->existsonly |= attr.existsonly;
    tattr->recheck    |= attr.recheck;
}


STATIC bool resetEx( void *targ, void *ptr )
/******************************************/
{
    (void)ptr; // Unused
    ((TARGET *)targ)->executed = true;
    return( false );
}


void ResetExecuted( void )
/************************/
{
    WalkHashTab( targTab, resetEx, NULL );
}


STATIC bool noCmds( void *trg, void *ptr )
/****************************************/
{
    TARGET  *targ = trg;

    (void)ptr; // Unused
    if( targ->depend != NULL && targ->depend->clist == NULL ) {
        targ->allow_nocmd = true;
    }
    return( false );
}


void CheckNoCmds( void )
/**********************/
{
    WalkHashTab( targTab, noCmds, NULL );
}


#if defined( USE_SCARCE ) || !defined( NDEBUG )
STATIC RET_T cleanupLeftovers( void )
/***********************************/
{
    DEPEND      *dep;
    CLIST       *c;
    SLIST       *s;
    TLIST       *t;
    FLIST       *f;
    NKLIST      *nk;

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
#endif


void TargetInit( void )
/*********************/
{
    targTab     = NULL;
    freeDepends = NULL;
    freeTLists  = NULL;
    freeCLists  = NULL;
    freeFLists  = NULL;
    freeNKLists = NULL;
    freeSLists  = NULL;
    targTab = NewHashTab( HASH_PRIME );
#ifdef USE_SCARCE
    IfMemScarce( cleanupLeftovers );
#endif
}


#ifndef NDEBUG
STATIC bool walkFree( void *targ, void *ptr )
/*******************************************/
{
    (void)ptr; // Unused
    freeTarget( (TARGET*)targ );
    return( false );
}
#endif


void TargetFini( void )
/*********************/
{
#ifndef NDEBUG
    WalkHashTab( targTab, walkFree, NULL );
    FreeHashTab( targTab );
    targTab = NULL;
    while( cleanupLeftovers() != RET_ERROR )
        ;
#endif
}

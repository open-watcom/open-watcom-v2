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
* Description:  Suffix (extension) management.
*
****************************************************************************/


#include "make.h"
#include "mcache.h"
#include "mmemory.h"
#include "mhash.h"
#include "mmisc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mtarget.h"
#include "mvecstr.h"
#include "msuffix.h"

#include "clibext.h"
#include "pathgrp.h"


#define HASH_PRIME      13
#define CASESENSITIVE   false       // Is suffix name case insensitive

STATIC HASHTAB          *sufTab;
STATIC UINT16           nextId;
STATIC UINT16           prevId;     // Has to be one less than nextId


STATIC void freePathRing( PATHRING pathring )
/*******************************************/
{
    PATHNODE    *walk;
    PATHNODE    *cur;

    if( pathring == NULL ) {
        return;
    }

    walk = pathring;
    do {
        cur = walk;
        walk = walk->next;
        FreeSafe( cur->name );
        FreeSafe( cur );
    } while( walk != pathring );
}


STATIC bool freeSuffix( void *node, void *ptr )
/*********************************************/
{
    SUFFIX  *suffix = node;
    CREATOR *creator;
    CREATOR *creator_next;

    (void)ptr; // Unused
    FreeSafe( suffix->node.name );
    freePathRing( suffix->pathring );

    for( creator = suffix->creator; creator != NULL; creator = creator_next ) {
        creator_next = creator->next;
        FreeSList( creator->slist );
        FreeSafe( creator );
    }

    FreeSafe( suffix );

    return( false );
}


void ClearSuffixes( void )
/********************************
 * clear all suffix definitions
 */
{
    WalkHashTab( sufTab, freeSuffix, NULL );
    FreeHashTab( sufTab );
    sufTab = NewHashTab( HASH_PRIME );
    nextId = 32768U;
    prevId = nextId - 1;
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC SUFFIX *findSuffixNode( const char *name, const char **p )
/****************************************************************
 * returns: pointer to SUFFIX named name, or NULL.  If p != NULL, then
 *          *p will be the pointer to the first dot not in the first
 *          position of name.  ie:
 *          .src.dest   returns SUFFIX src, and p = ".dest"
 *          src.dest    same
 *          .src or src returns SUFFIX src, and p = NULL
 */
{
    char        sufname[MAX_SUFFIX];
    const char  *s;
    char        *d;

    assert( name != NULL );

    if( name[0] == DOT ) {
        ++name;
    }

    d = sufname;
    s = name;
    while( *s != NULLCHAR && *s != DOT ) {
        *d++ = *s++;
    }
    *d = NULLCHAR;

    if( p != NULL ) {
        if( *s == DOT ) {
            *p = s;
        } else {
            *p = NULL;
        }
    }

    FixName( sufname );

    return( (SUFFIX *)FindHashNode( sufTab, sufname, CASESENSITIVE ) );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


SUFFIX *FindSuffix( const char *name )
/*******************************************/
{
    return( findSuffixNode( name, NULL ) );
}


bool SufExists( const char *name )    /* with . */
/********************************/
{
    assert( name != NULL && name[0] == DOT );

    return( FindSuffix( name ) != NULL );
}


STATIC void AddFrontSuffix( char const *name )
/*********************************************
 * pass name with leading .; adds name to suffix table and assigns id
 * retains use of name after call
 * adds the suffix to the front of the extensions list by giving an id
 * that is decremented instead of incremented for microsoft option only
 */
{
    SUFFIX  *new;

    assert( (name + 1) != NULL && name[0] == DOT && !SufExists( name ) );

    new = CallocSafe( sizeof( *new ) );
    new->node.name = FixName( StrDupSafe( name + 1 ) );
    new->id = prevId;
    --prevId;

    AddHashNode( sufTab, (HASHNODE *)new );
}


bool SufBothExist( const char *sufsuf )   /* .src.dest */
/**************************************
 *  for MS-Option it only checks if the dependent suffix is defined
 *  so no need for checking the target suffix if it exists
 */
{
    char const  *ptr;

    assert( sufsuf != NULL && sufsuf[0] == DOT && strchr( sufsuf + 1, DOT ) != NULL );

    if( findSuffixNode( sufsuf, &ptr ) == NULL ) {
        return( false );
    }

    if( FindSuffix( ptr ) == NULL ) {
        if( Glob.compat_nmake ) {
            AddFrontSuffix( ptr );
            return( true );
        } else {
            return( false );
        }

    } else {
        return( true );
    }

}


void AddSuffix( const char *name )
/*********************************
 * pass name with leading .; adds name to suffix table and assigns id
 * retains use of name after call
 */
{
    SUFFIX  *new;

    assert( ( name != NULL && name[0] == DOT && !SufExists( name ) ) ||
            ( name != NULL && name[0] == DOT && SufExists( name ) &&
            Glob.compat_nmake ) );

    new = CallocSafe( sizeof( *new ) );
    new->node.name = FixName( StrDupSafe( name + 1 ) ); /* skip leading dot */
    new->id = nextId;
    ++nextId;

    AddHashNode( sufTab, (HASHNODE *)new );
}


STATIC void addPathToPathRing( PATHRING *pathring, const char *path )
/*******************************************************************/
{
    PATHNODE    **tail;
    PATHNODE    *newpath;
    const char  *p;
    size_t      len;

    assert( pathring != NULL && path != NULL );

    tail = pathring;            /* find "tail" of ring */
    if( *tail != NULL ) {
        do {
            tail = &(*tail)->next;
        } while( *tail != *pathring );
    }
    p = path;
    while( *p != NULLCHAR ) {
        /* find end of path in string */
        while( *p != NULLCHAR && *p != PATH_SPLIT && *p != ';' ) {
            ++p;
        }
        len = p - path;                                     /* get length of sub-path */
        if( len > 0 ) {
            char    *p1;

            p1 = MallocSafe( len + 1 );                     /* make copy of sub-path */
            memcpy( p1, path, len );
            p1[len] = NULLCHAR;
            FixName( p1 );
            newpath = MallocSafe( sizeof( *newpath ) );     /* get a new node */
            newpath->name = p1;
            *tail = newpath;           /* link into ring - but don't close ring yet */
            tail = &newpath->next;
        }
        if( *p != NULLCHAR ) {
            ++p;
            path = p;       /* advance to next path in string */
        }
    }
    *tail = *pathring;      /* now we finally close the ring up */
}


void SetSufPath( const char *name, const char *path )
/***************************************************/
/* name with . */
{
    SUFFIX      *suffix;

    assert( name != NULL && name[0] == DOT );

    suffix = FindSuffix( name );

    assert( suffix != NULL );

    if( path == NULL ) {
        freePathRing( suffix->pathring );
        suffix->pathring = NULL;
        suffix->currpath = NULL;
        return;
    }

    addPathToPathRing( &suffix->pathring, path );
    if( suffix->currpath == NULL ) {
        suffix->currpath = suffix->pathring;
    }
}


STATIC CREATOR *newCreator( void )
/********************************/
{
    return( (CREATOR *)CallocSafe( sizeof( CREATOR ) ) );
}


STATIC char *getFullSufSuf( const SUFFIX *deps, const char *dep, const SUFFIX *targs, const char *targ )
/******************************************************************************************************/
{
    VECSTR      fullsufsuf;

    fullsufsuf = StartVec();
    if( *dep != NULLCHAR ) {
        WriteNVec( fullsufsuf, "{", 1 );
        WriteVec( fullsufsuf, dep );
        WriteNVec( fullsufsuf, "}", 1 );
    }
    WriteNVec( fullsufsuf, ".", 1 );
    WriteVec( fullsufsuf, deps->node.name );
    if( *targ != NULLCHAR ) {
        WriteNVec( fullsufsuf, "{", 1 );
        WriteVec( fullsufsuf, targ );
        WriteNVec( fullsufsuf, "}", 1 );
    }
    WriteNVec( fullsufsuf, ".", 1 );
    WriteVec( fullsufsuf, targs->node.name );
    return( FinishVec( fullsufsuf ) );
}


char *AddCreator( const char *sufsuf )
/*************************************
 * add the creation .src.dest
 */
{
    SUFFIX      *src;
    SUFFIX      *dest;
    char const  *ptr;
    CREATOR     *new;
    CREATOR     **cur;
    SLIST       *slist;
    SLIST       **sl;
    char        *fullsufsuf;
    char        *cur_targ_path;
    char        *cur_dep_path;
    char        buf[_MAX_PATH];

    assert( sufsuf != NULL && sufsuf[0] == DOT && strchr( sufsuf + 1, DOT ) != NULL );

    src = findSuffixNode( sufsuf, &ptr );
    dest = FindSuffix( ptr );

    assert( src != NULL && dest != NULL );

    if( !Glob.compat_nmake && !Glob.compat_posix && src->id < dest->id ) {
        PrtMsg( ERR | LOC | EXTENSIONS_REVERSED );
    }
    for( cur = &dest->creator; *cur != NULL; cur = &(*cur)->next ) {
        if( src->id <= (*cur)->suffix->id ) {
            break;
        }
    }

    cur_targ_path = targ_path;
    if( *targ_path != NULLCHAR ) {
        _makepath( buf, NULL, targ_path, NULL, NULL );
        cur_targ_path = StrDupSafe( FixName( buf ) );
    }
    cur_dep_path = dep_path;
    if( *dep_path != NULLCHAR ) {
        _makepath( buf, NULL, dep_path, NULL, NULL );
        cur_dep_path = StrDupSafe( FixName( buf ) );
    }

    sl = NULL;
    if( *cur != NULL && src->id == (*cur)->suffix->id ) {
        for( slist = (*cur)->slist; ; slist = slist->next ) {
            if( stricmp( slist->targ_path, cur_targ_path ) == 0 && stricmp( slist->dep_path, cur_dep_path ) == 0 ) {
                if( *cur_targ_path != NULLCHAR ) {
                    FreeSafe( cur_targ_path );
                }
                if( *cur_dep_path != NULLCHAR ) {
                    FreeSafe( cur_dep_path );
                }
                return( StrDupSafe( slist->cretarg->node.name ) );
            }
            if( slist->next == NULL ) {
                sl = &slist->next;
                break;
            }
        }
    }
    if( sl == NULL ) {
        new = newCreator();
        new->suffix = src;
        new->slist = NULL;
        sl = &new->slist;

        new->next = *cur;
        *cur = new;
    }

    fullsufsuf = getFullSufSuf( src, cur_dep_path, dest, cur_targ_path );

    slist = NewSList();
    slist->targ_path = cur_targ_path;
    slist->dep_path = cur_dep_path;
    slist->cretarg = NewTarget( fullsufsuf );
    slist->cretarg->special = true;
    slist->cretarg->sufsuf  = true;
    slist->cretarg->depend = NewDepend();
    slist->next = *sl;
    *sl = slist;
    return( fullsufsuf );
}


STATIC bool printSuf( void *node, void *ptr )
/*******************************************/
{
    SUFFIX      *suffix = node;
    CREATOR     *cur;
    PATHNODE    *currpath;
    SLIST       *slist;
    CLIST       *cmds;
    bool        printed;

    (void)ptr; // Unused
    PrtMsg( INF | PSUF_SUFFIX, suffix->node.name );
    if( suffix->currpath != NULL ) {
        currpath = suffix->currpath;
        do {
            PrtMsg( INF | PSUF_FOUND_IN, currpath->name );
            currpath = currpath->next;
        } while( currpath != suffix->currpath );
        printed = true;
    } else {
        printed = false;
    }
    cur = suffix->creator;
    if( cur != NULL && printed ) {
        PrtMsg( INF | NEWLINE );
    }
    while( cur != NULL ) {
        slist = cur->slist;
        while( slist != NULL ) {
            PrtMsg( INF | NEOL | PSUF_MADE_FROM, cur->suffix->node.name );
            PrintTargFlags( &slist->cretarg->attr );
            PrtMsg( INF | NEWLINE );
            if( *slist->targ_path != NULLCHAR ) {
                PrtMsg( INF | PSUF_OUTPUT_DIR, slist->targ_path );
            }
            if( *slist->dep_path != NULLCHAR ) {
                PrtMsg( INF | PSUF_SOURCE_DIR, slist->dep_path );
            }
            cmds = slist->cretarg->depend->clist;
            if( cmds != NULL ) {
                PrtMsg( INF | PSUF_USING_CMDS );
                PrintCList( cmds );
            }
            slist = slist->next;
            if( slist != NULL ) {
                PrtMsg( INF | NEWLINE );
            }
        }
        cur = cur->next;
        if( cur != NULL ) {
            PrtMsg( INF | NEWLINE );
        }
    }
    PrtMsg( INF | NEWLINE );

    return( false );
}


void PrintSuffixes( void )
/*******************************/
{
    WalkHashTab( sufTab, printSuf, NULL );
}


STATIC RET_T chkOneName( char *buffer, TARGET **chktarg )
/*******************************************************/
{
    TARGET  *tmp;

    FixName( buffer );

    if( chktarg != NULL ) {
        tmp = FindTarget( buffer );
        if( tmp != NULL ) {
            *chktarg = tmp;
            return( RET_SUCCESS );
        }
    }
    if( CacheExists( buffer ) ) {
        return( RET_SUCCESS );
    }
    return( RET_ERROR );
}


STATIC RET_T findInPathRing( PATHRING *pathring, char *buffer,
    const char *dir, const char *fname, const char *ext, TARGET **chktarg )
/**************************************************************************
 * walk a path ring, and attempt to find fname.ext using different paths
 */
{
    PATHNODE    *pathnode;
    char        fake_name[_MAX_PATH];

    assert( pathring != NULL );
    if( *pathring == NULL ) {
        return( RET_ERROR );
    }
    if( dir[0] == NULLCHAR ) {
        dir = NULL;
    }
    _makepath( fake_name, NULL, dir, fname, ext );
    pathnode = *pathring;
    do {
        _makepath( buffer, NULL, pathnode->name, fake_name, NULL );
        if( chkOneName( buffer, chktarg ) == RET_SUCCESS ) {
            if( Glob.optimize ) {       /* nail down pathring here */
                *pathring = pathnode;
            }
            return( RET_SUCCESS );
        }
        pathnode = pathnode->next;
    } while( pathnode != *pathring );

    return( RET_ERROR );
}


RET_T TrySufPath( char *buffer, const char *filename, TARGET **chktarg, bool tryenv )
/************************************************************************************
 * it is NOT necessary that filename != buffer
 * the contents of buffer may be destroyed even if RET_ERROR is returned
 * first checks current directory, then any in suffix path
 * assumes buffer is at least _MAX_PATH, and that if buffer != filename
 * then buffer, and filename do not overlap
 */
{
    PGROUP      pg;
    SUFFIX      *suffix;
    char        *env;
    PATHRING    envpathring;
    RET_T       ret;

    if( chktarg != NULL ) { /* always NULL the chktarg before working */
        *chktarg = NULL;
    }

    /* check if filename given exists */
    if( filename != buffer ) {
        strcpy( buffer, filename );
    }
    if( chkOneName( buffer, chktarg ) == RET_SUCCESS ) {
        return( RET_SUCCESS );
    }

    /* split up filename */
    _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );

    if( pg.drive[0] != NULLCHAR || cisdirc( pg.dir[0] ) ) {
        /* is an absolute path name */
        return( RET_ERROR );
    }

    suffix = FindSuffix( pg.ext );

    ret = RET_ERROR;

    if( suffix == NULL || suffix->currpath == NULL ) {
        if( tryenv ) {
            /* no suffix info - use %PATH */
            env = getenv( "PATH" );
            if( env != NULL ) {
                envpathring = NULL;
                addPathToPathRing( &envpathring, env );

                /* never cache %path */
                Glob.cachedir = false;
                ret = findInPathRing( &envpathring, buffer, pg.dir, pg.fname, pg.ext, chktarg );
                Glob.cachedir = true;

                freePathRing( envpathring );
            }
        }
    } else {
        ret = findInPathRing( &suffix->currpath, buffer, pg.dir, pg.fname, pg.ext, chktarg );
    }

    return( ret );
}


void SuffixInit( void )
/****************************/
{
    sufTab = NewHashTab( HASH_PRIME );
    nextId = 32768U;
    prevId = nextId - 1;
}


void SuffixFini( void )
/****************************/
{
#ifdef DEVELOPMENT
    ClearSuffixes();
    FreeHashTab( sufTab );
    sufTab = NULL;
#else
    WalkHashTab( sufTab, freeSuffix, NULL );
    FreeHashTab( sufTab );
#endif
}

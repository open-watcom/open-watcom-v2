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


#if !defined(__QNX__)
 #include <direct.h>
 #include <dos.h>
#endif
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "make.h"
#include "massert.h"
#include "mhash.h"
#include "memory.h"
#include "misc.h"
#include "mpathgrp.h"
#include "mrcmsg.h"
#include "msg.h"


#if defined( __DOS__ ) || defined( __OS2__ ) || defined( __WINDOWS__ ) || defined( __NT__ )

/*
 * Implement a directory cache in far memory for MSDOS machines.  The
 * cache is released every time a command is executed - but the overall
 * effect is a dramatic speed increase.  (DOS uses a LINEAR search to locate
 * files - it's not that difficult to do better! :)
 */


#if 1 && defined(__NT__)
/*
    Windows NT makes us call an expensive convert GMT to local function
    for each time-stamp we want to look at!
*/
static unsigned cacheDelay;
#define CACHE_DELAY_CHECK       ((cacheDelay==0)||((--cacheDelay)==0))
#define CACHE_DELAY_RELEASE     cacheDelay = 512
#else
#define CACHE_DELAY_RELEASE
#endif

/*
 * info on packed DOS date/time
 */

typedef UINT16  DOSDATE_T;


/*
 * enum cacheRet are used internally to determine what the error return
 * from a CacheDir was
 */
enum cacheRet {
    CACHE_OK,
    CACHE_DIR_NOT_FOUND,
    CACHE_NOT_ENUF_MEM,
    CACHE_FILE_NOT_FOUND
};


/*
 * struct cacheEntry is the detail we need on each file.  We copy these
 * values from the struct dirent for each file.
 */
typedef struct cacheEntry FAR *CENTRYPTR;
struct cacheEntry {
    CENTRYPTR   ce_next;
    char        ce_name[ NAME_MAX + 1 ];
    DOSDATE_T   ce_date;
    DOSDATE_T   ce_time;
};


#define HASH_PRIME      97      /* for the hash function            */


/*
 * struct directHead is the head for a linked list of CacheNodes
 */
typedef struct directHead FAR *DHEADPTR;
struct directHead {
    DHEADPTR    dh_next;
    char        dh_name[ _MAX_PATH ];
    CENTRYPTR   dh_table[ HASH_PRIME ];
};

STATIC DHEADPTR cacheHead;

#ifdef USE_FAR
#   define  myMalloc(size)      FarMaybeMalloc(size)
#   define  myFree(ptr)         FarFree(ptr)
#   define  myCmp(f,n)          _fFNameCmp(f,n)
#else
#   define  myMalloc(size)      MallocUnSafe(size)
#   define  myFree(ptr)         FreeSafe(ptr)
#   define  myCmp(f,n)          FNameCmp(f,n)
#endif


/*
 * convert a Dos packed date/time to a time_t
 */
STATIC time_t d2t( DOSDATE_T date, DOSDATE_T time )
/*************************************************/
{
    return( SysDOSStampToTime( date, time ) );
}


STATIC void freeDirectList( DHEADPTR dhead )
/******************************************/
{
    DHEADPTR    dcur;
    CENTRYPTR   ccur;
    CENTRYPTR   cwalk;
    HASH_T      h;
#ifdef CACHE_STATS
    UINT32      bytes = 0UL;
#endif

    while( dhead != NULL ) {
        dcur = dhead;
        dhead = dhead->dh_next;
        for( h = 0; h < HASH_PRIME; h++ ) {
            cwalk = dcur->dh_table[ h ];
            while( cwalk != NULL ) {
                ccur = cwalk;
                cwalk = cwalk->ce_next;
                myFree( ccur );
#ifdef CACHE_STATS
                bytes += sizeof( *ccur );
#endif
            }
        }
        myFree( dcur );
#ifdef CACHE_STATS
        bytes += sizeof( *dcur );
#endif
    }
#ifdef CACHE_STATS
    if( Glob.cachestat ) {
        PrtMsg( INF| CACHE_FREED_BYTES, bytes );
    }
#endif
}


/*
 * Given a full pathname or just a path ending in \ cache all the files
 * in that directory.  Assumes that this directory is not already
 * cached.  Does not link into list off cacheHead.
 */
STATIC enum cacheRet cacheDir( DHEADPTR *pdhead, char *path )
/***********************************************************/
{
    CENTRYPTR       cnew;       /* new cacheEntry struct */
    DIR             *parent;    /* parent directory entry */
    DIR             *entry;     /* current directory entry */
    HASH_T          h;          /* hash value */
    size_t          len;
#   ifdef CACHE_STATS
        UINT32          bytes = 0;  /* counter */
        UINT32          files = 0;  /* counter */
        UINT32          hits = 0;   /* counter */

        if( Glob.cachestat ) {
            PrtMsg( INF| CACHING_DIRECTORY, path );
        }
#   endif

    *pdhead = myMalloc( sizeof( **pdhead ) );
    if( *pdhead == NULL ) {
        return( CACHE_NOT_ENUF_MEM );
    }
#   ifdef CACHE_STATS
        bytes += sizeof( **pdhead );
#   endif

                                    /* clear the memory */
    _fmemset( *pdhead, 0, sizeof( **pdhead ) );

    len = strlen( path );
    _fmemcpy( (*pdhead)->dh_name, path, len + 1 );

    strcpy( &path[len], "*.*" );
    parent = opendir( path );
    if( parent == NULL ) {
#       ifdef CACHE_STATS
            if( Glob.cachestat ) {
                PrtMsg( INF|NEOL| CACHE_FILES_BYTES, files, bytes, hits );
            }
#       endif
        return( CACHE_OK );     /* an empty, or nonexistent directory */
    }

    entry = readdir( parent );
    while( entry != NULL ) {
        if( !( entry->d_attr & IGNORE_MASK ) ) {
                        /* we tromp on entry, and get hash value */
            h = Hash( FixName( entry->d_name ), HASH_PRIME );
            cnew = myMalloc( sizeof( *cnew ) );
            if( cnew == NULL ) {
                freeDirectList( *pdhead );  /* roll back, and abort */
                *pdhead = NULL;
#           ifdef CACHE_STATS
                if( Glob.cachestat ) {
                    if( hits % 8 != 0 ) {
                        PrtMsg( INF| NEWLINE );
                    }
                    PrtMsg( INF| CACHE_MEM );
                }
#           endif
                return( CACHE_NOT_ENUF_MEM );
            }
#           ifdef CACHE_STATS
                bytes += sizeof( *cnew );
                ++files;
#           endif

            cnew->ce_date = entry->d_date;
            cnew->ce_time = entry->d_time;
            ConstMemCpy( cnew->ce_name, entry->d_name, NAME_MAX + 1 );

            cnew->ce_next = (*pdhead)->dh_table[ h ];
            (*pdhead)->dh_table[ h ] = cnew;
#           ifdef CACHE_STATS
                if( Glob.cachestat && cnew->ce_next != NULL ) {
                    ++hits;
                    PrtMsg( INF|( ( hits % 8 == 0 ) ? 0 : NEOL )| HIT_ON_HASH,
                                                            h );
                }
#           endif
        }
        entry = readdir( parent );
    }
    closedir( parent );

#   ifdef CACHE_STATS
        if( Glob.cachestat ) {
            if( hits % 8 != 0 ) {
                PrtMsg( INF| NEWLINE );
            }
            PrtMsg( INF| CACHE_FILES_BYTES, files, bytes, hits );
        }
#   endif

    return( CACHE_OK );
}


/*
 * Walk the directory linked list, and find the directory with the name
 * path.  (Path must end in \ - ie: as returned by _splitpath() )
 * Returns NULL if not found, or if cacheHead == NULL.
 * Move the directory to the beginning of the list (adaptive search)
 */
STATIC DHEADPTR findDir( const char *path )
/*****************************************/
{
    DHEADPTR    dcur;
    DHEADPTR    dlast;
    char        first;

    dlast = NULL;
    dcur = cacheHead;
    first = path[0];
    while( dcur != NULL && ( first != dcur->dh_name[0] ||
                            myCmp( dcur->dh_name, path ) != 0 ) ) {
        dlast = dcur;
        dcur = dcur->dh_next;
    }

    if( dlast != NULL && dcur != NULL ) {   /* promote directory to head */
        dlast->dh_next = dcur->dh_next;
        dcur->dh_next = cacheHead;
        cacheHead = dcur;
    }

    return( dcur );
}


/*
 * Given a directory, find a file within that directory.
 * Return NULL if file not found.
 */
STATIC CENTRYPTR findFile( DHEADPTR dir, const char *name )
/*********************************************************/
{
    CENTRYPTR   ccur;
    HASH_T      h;

    h = Hash( name, HASH_PRIME );

    ccur = dir->dh_table[ h ];
    while( ccur != NULL ) {
        if( myCmp( ccur->ce_name, name ) == 0 ) {
            return( ccur );
        }
        ccur = ccur->ce_next;
    }

    return( NULL );
}


/*
 * Given a directory, find an 8.3 file within that directory.
 * Return NULL if file not found.
 */
STATIC CENTRYPTR findDOSFile( DHEADPTR dir, const char *name )
/************************************************************/
{
#if _MAX_FNAME == ( 8 + 1 )
    dir = dir;
    name = name;
    return( NULL );
#else
    size_t len;
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    char DOSname[_MAX_PATH];

    _splitpath( name, NULL, NULL, fname, ext );
    len = strlen( fname );
    if( len > 8 ) {
        fname[8] = '\0';
    }
    _makepath( DOSname, NULL, NULL, fname, ext );
    return( findFile( dir, DOSname ) );
#endif
}


/*
 * Called at the beginning of the program
 */
extern void CacheInit( void )
/***************************/
{
}


/*
 * Called at any time we want to invalidate the cache
 */
extern void CacheRelease( void )
/******************************/
{
#ifdef CACHE_STATS
    if( Glob.cachestat ) {
        PrtMsg( INF| CACHERELEASE );
    }
#endif

    freeDirectList( cacheHead );
    cacheHead = NULL;
    MemShrink();
    CACHE_DELAY_RELEASE;
}


/*
 * Called while the program is exiting
 */
extern void CacheFini( void )
/***************************/
{
#ifdef CACHE_STATS
    Glob.cachestat = 0;
#endif
#ifndef NDEBUG
    CacheRelease();
#endif
}


STATIC RET_T regStat( const char *filename, time_t *ptime )
/**********************************************************
 * not quite a regular stat(), but functional for what we need
 */
{
    DIR *parent;
    DIR *entry;

    parent = opendir( filename );
    if( parent == NULL ) {
        return( RET_ERROR );
    }

    entry = readdir( parent );

    assert( entry != NULL );

    *ptime = d2t( entry->d_date, entry->d_time );

    closedir( parent );

    return( RET_SUCCESS );
}


STATIC void splitFullPath( const char *fullpath, char *pathbuf, char *filebuf )
/*****************************************************************************/
{
    PGROUP      *pg;

    assert( fullpath != NULL && pathbuf != NULL && filebuf != NULL );

    pg = SplitPath( fullpath );

    _makepath( pathbuf, pg->drive, pg->dir, NULL, NULL );
    if( pg->ext[0] == '.' && pg->ext[1] == 0 ) {
        _makepath( filebuf, NULL, NULL, pg->fname, NULL );
    } else {
        _makepath( filebuf, NULL, NULL, pg->fname, pg->ext );
    }

    DropPGroup( pg );
}


#pragma on (check_stack);
STATIC enum cacheRet maybeCache( const char *fullpath, CENTRYPTR *pc )
/********************************************************************/
{
    char        path[ _MAX_PATH ];
    char        name[ NAME_MAX + 1 ];
    DHEADPTR    dcur;
    CENTRYPTR   centry;
    enum cacheRet ret;

    assert( fullpath != NULL );

    splitFullPath( fullpath, path, name );
    FixName( path );
    FixName( name );

    dcur = findDir( path );
    if( dcur == NULL ) {        /* must cache new directory */
        ret = cacheDir( &dcur, path );
        if( ret != CACHE_OK ) {
            return( ret );
        }
        dcur->dh_next = cacheHead;
        cacheHead = dcur;
    }

    /* now dcur points to Cached directory */
    assert( dcur != NULL );

    centry = findFile( dcur, name );
    if( centry == NULL ) {
        centry = findDOSFile( dcur, name );
        if( centry == NULL ) {
            return( CACHE_FILE_NOT_FOUND );
        }
    }

    if( pc != NULL ) {
        *pc = centry;
    }

    return( CACHE_OK );
}
#pragma off(check_stack);


/*
 * Given a full path to a file, get the st_mtime for that file.  If there
 * are no errors, return 0, otherwise 1.  If the file is in directory not
 * cached yet, then cache it first.
 */
extern RET_T CacheTime( const char *fullpath, time_t *ptime )
/***********************************************************/
{
    CENTRYPTR   centry;

    assert( fullpath != NULL && ptime != NULL );

#ifdef CACHE_DELAY_CHECK
    if( Glob.cachedir && CACHE_DELAY_CHECK ) {
#else
    if( Glob.cachedir ) {
#endif
        switch( maybeCache( fullpath, &centry ) ) {
        case CACHE_OK:
            *ptime = d2t( centry->ce_date, centry->ce_time );
            return( RET_SUCCESS );
        case CACHE_NOT_ENUF_MEM:
            break;
        default:
            return( RET_ERROR );
        }
    }

    return( regStat( fullpath, ptime ) );
}


/*
 * return TRUE if the file in fullpath exists, FALSE otherwise
 */
extern BOOLEAN CacheExists( const char *fullpath )
/************************************************/
{
    assert( fullpath != NULL );

#ifdef CACHE_DELAY_CHECK
    if( Glob.cachedir && CACHE_DELAY_CHECK ) {
#else
    if( Glob.cachedir ) {
#endif
        switch( maybeCache( fullpath, NULL ) ) {
        case CACHE_OK:
            return( TRUE );
        case CACHE_NOT_ENUF_MEM:
            break;
        default:
            return( FALSE );
        }
    }

    return( access( fullpath, 0 ) == 0 );
}

#else

/*
 * stubbed out cache routines for non ms-dos support
 */
extern void CacheInit( void ) {}
extern void CacheRelease( void ) {}
extern void CacheFini( void ) {}


extern RET_T CacheTime( const char *name, time_t *ptime )
/*******************************************************/
{
    struct stat buf;

    if( stat( name, &buf ) == 0 ) {
        *ptime = buf.st_mtime;
        return( RET_SUCCESS );
    }

    return( RET_ERROR );
}


extern BOOLEAN CacheExists( const char *name )
/********************************************/
{
    if( access( name, 0 ) == 0 ) {
        return( TRUE );
    }
    return( FALSE );
}
#endif

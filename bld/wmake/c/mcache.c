/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Directory caching routines.
*
****************************************************************************/


#if defined( __UNIX__ )
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#if defined( __RDOS__ )
    #include "rdos.h"
#endif
#include "make.h"
#include "wio.h"
#include "mhash.h"
#include "mmemory.h"
#include "mmisc.h"
#include "dostimet.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "pathgrp2.h"
#include "mcache.h"

#include "clibext.h"


#ifdef USE_DIR_CACHE

/*
 * Implement a directory cache in far memory for MSDOS machines.  The
 * cache is released every time a command is executed - but the overall
 * effect is a dramatic speed increase.  (DOS uses a LINEAR search to locate
 * files - it's not that difficult to do better! :)
 */


#if defined( __NT__ )
/*
    Windows NT makes us call an expensive convert GMT to local function
    for each time-stamp we want to look at!
*/
static unsigned cacheDelay;
#define CACHE_DELAY_CHECK()     ( cacheDelay == 0 || ( --cacheDelay == 0 ) )
#define CACHE_DELAY_RELEASE()   ( cacheDelay = 512 )

#include "_dtaxxx.h"

#else

#define CACHE_DELAY_RELEASE()

#endif


/*
 * enum cacheRet values are used internally to determine what the error return
 * from a CacheDir was
 */
enum cacheRet {
    CACHE_OK = 0,
    // CACHE_DIR_NOT_FOUND = 1,
    CACHE_NOT_ENUF_MEM = 2,
    CACHE_FILE_NOT_FOUND = 3
};


/*
 * struct cacheEntry is the detail we need on each file.  We copy these
 * values from the struct dirent for each file.
 */
typedef struct cacheEntry {
    struct cacheEntry FAR   *ce_next;
    time_t                  ce_tt;
    char                    ce_name[NAME_MAX + 1];
} FAR *CENTRYPTR;


#define HASH_PRIME      97      /* for the hash function            */


/*
 * struct directHead is the head for a linked list of CacheNodes
 */
typedef struct directHead {
    struct directHead FAR   *dh_next;
    char                    dh_name[_MAX_PATH];
    CENTRYPTR               dh_table[HASH_PRIME];
} FAR *DHEADPTR;

STATIC DHEADPTR cacheHead;


STATIC void freeDirectList( DHEADPTR dhead )
/******************************************/
{
    DHEADPTR    dhead_next;
    CENTRYPTR   cwalk_next;
    CENTRYPTR   cwalk;
    HASH_T      h;
#ifdef CACHE_STATS
    UINT32      bytes = 0UL;
#endif

    for( ; dhead != NULL; dhead = dhead_next ) {
        dhead_next = dhead->dh_next;
        for( h = 0; h < HASH_PRIME; h++ ) {
            for( cwalk = dhead->dh_table[h]; cwalk != NULL; cwalk = cwalk_next ) {
                cwalk_next = cwalk->ce_next;
                FarFreeSafe( cwalk );
#ifdef CACHE_STATS
                bytes += sizeof( *cwalk );
#endif
            }
        }
        FarFreeSafe( dhead );
#ifdef CACHE_STATS
        bytes += sizeof( *dhead );
#endif
    }
#ifdef CACHE_STATS
    if( Glob.cachestat ) {
        PrtMsg( INF | CACHE_FREED_BYTES, bytes );
    }
#endif
}

static time_t get_direntry_timestamp( struct dirent *entry )
{
#if defined( __UNIX__ )
    return( YOUNGEST_DATE );
#elif defined( __WATCOMC__ ) && __WATCOMC__ < 1300
    /*
     * OW1.x bootstrap compiler workaround
     */
    return( _dos2timet( entry->d_date * 0x10000L + entry->d_time ) );
#elif defined( __NT__ )
    return( DTAXXX_TSTAMP_OF( entry->d_dta ) );
#elif defined( __RDOS__ )
    unsigned short date;
    unsigned short time;

    RdosTicsToDosTimeDate(entry->d_msb_time, entry->d_lsb_time, &date, &time);
    return( _dos2timet( date * 0x10000L + time ) );
#else
    return( _dos2timet( entry->d_date * 0x10000L + entry->d_time ) );
#endif
}

STATIC enum cacheRet cacheDir( DHEADPTR *pdhead, char *path )
/************************************************************
 * Given a full pathname or just a path ending in \ cache all the files
 * in that directory.  Assumes that this directory is not already
 * cached.  Does not link into list off cacheHead.
 */
{
    CENTRYPTR       cnew;       /* new cacheEntry struct */
    DIR             *dirp;      /* parent directory entry */
    struct dirent   *dire;      /* current directory entry */
    HASH_T          h;          /* hash value */
    size_t          len;
    char            *name_ptr;
#ifdef CACHE_STATS
    UINT32          bytes = 0;  /* counter */
    UINT32          files = 0;  /* counter */
    UINT32          hits = 0;   /* counter */

    if( Glob.cachestat ) {
        PrtMsg( INF | CACHING_DIRECTORY, path );
    }
#endif

    *pdhead = FarMallocUnSafe( sizeof( **pdhead ) );
    if( *pdhead == NULL ) {
        return( CACHE_NOT_ENUF_MEM );
    }
#ifdef CACHE_STATS
    bytes += sizeof( **pdhead );
#endif

    /* clear the memory */
    FarMemSet( *pdhead, 0, sizeof( **pdhead ) );

    len = strlen( path );
    FarMemCpy( (*pdhead)->dh_name, path, len + 1 );
    name_ptr = path + len;

#if !defined( __UNIX__ ) //|| defined( __WATCOMC__ )
    memcpy( name_ptr, "*.*", 4 );
#endif
    dirp = opendir( path );
    if( dirp == NULL ) {
#ifdef CACHE_STATS
        if( Glob.cachestat ) {
            PrtMsg( INF | NEOL | CACHE_FILES_BYTES, files, bytes, hits );
        }
#endif
        return( CACHE_OK );     /* an empty, or nonexistent directory */
    }

    while( (dire = readdir( dirp )) != NULL ) {
#if !defined( __UNIX__ )
        if( dire->d_attr & IGNORE_MASK )
            continue;
#endif
        /* we tromp on entry, and get hash value */
        h = Hash( FixName( dire->d_name ), HASH_PRIME );
        cnew = FarMallocUnSafe( sizeof( *cnew ) );
        if( cnew == NULL ) {
            closedir( dirp );
            freeDirectList( *pdhead );  /* roll back, and abort */
            *pdhead = NULL;
#ifdef CACHE_STATS
            if( Glob.cachestat ) {
                if( hits % 8 != 0 ) {
                    PrtMsg( INF | NEWLINE );
                }
                PrtMsg( INF | CACHE_MEM );
            }
#endif
            return( CACHE_NOT_ENUF_MEM );
        }
#ifdef CACHE_STATS
        bytes += sizeof( *cnew );
        ++files;
#endif
        cnew->ce_tt = get_direntry_timestamp( dire );
        ConstMemCpy( cnew->ce_name, dire->d_name, NAME_MAX + 1 );
        cnew->ce_next = (*pdhead)->dh_table[h];
        (*pdhead)->dh_table[h] = cnew;
#ifdef CACHE_STATS
        if( Glob.cachestat && cnew->ce_next != NULL ) {
            ++hits;
            PrtMsg( INF | ((hits % 8 == 0) ? 0 : NEOL) | HIT_ON_HASH, h );
        }
#endif
    }
    closedir( dirp );

#ifdef CACHE_STATS
    if( Glob.cachestat ) {
        if( hits % 8 != 0 ) {
            PrtMsg( INF | NEWLINE );
        }
        PrtMsg( INF | CACHE_FILES_BYTES, files, bytes, hits );
    }
#endif

    return( CACHE_OK );
}


STATIC DHEADPTR findDir( const char *path )
/******************************************
 * Walk the directory linked list, and find the directory with the name
 * path.  (Path must end in \ - ie: as returned by _splitpath2() )
 * Returns NULL if not found, or if cacheHead == NULL.
 * Move the directory to the beginning of the list (adaptive search)
 */
{
    DHEADPTR    dcur;
    DHEADPTR    dlast;

    dlast = NULL;
    for( dcur = cacheHead; dcur != NULL; dcur = dcur->dh_next ) {
        if( FarFNameEq( dcur->dh_name, path ) ) {
            break;
        }
        dlast = dcur;
    }

    if( dlast != NULL && dcur != NULL ) {   /* promote directory to head */
        dlast->dh_next = dcur->dh_next;
        dcur->dh_next = cacheHead;
        cacheHead = dcur;
    }

    return( dcur );
}


STATIC CENTRYPTR findFile( DHEADPTR dir, const char *name )
/**********************************************************
 * Given a directory, find a file within that directory.
 * Return NULL if file not found.
 */
{
    CENTRYPTR   ccur;
    HASH_T      h;

    h = Hash( name, HASH_PRIME );

    for( ccur = dir->dh_table[h]; ccur != NULL; ccur = ccur->ce_next ) {
        if( FarFNameEq( ccur->ce_name, name ) ) {
            return( ccur );
        }
    }

    return( NULL );
}

#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC enum cacheRet maybeCache( const char *fullpath, CENTRYPTR *pc )
/********************************************************************/
{
    char            path[_MAX_PATH];
    char            name[NAME_MAX + 1];
    DHEADPTR        dcur;
    CENTRYPTR       centry;
    enum cacheRet   ret;
    pgroup2         pg;
    char const      *ext;

    assert( fullpath != NULL );

    _splitpath2( fullpath, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( path, pg.drive, pg.dir, NULL, NULL );
    ext = pg.ext;
    if( ext[0] == '.' && ext[1] == NULLCHAR ) {
        ext = NULL;
    }
    _makepath( name, NULL, NULL, pg.fname, ext );

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
        return( CACHE_FILE_NOT_FOUND );
    }

    if( pc != NULL ) {
        *pc = centry;
    }

    return( CACHE_OK );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif

#endif  /* USE_DIR_CACHE */

/*
 * cache routines stubbed out for non ms-dos support
 */

void CacheInit( void )
/****************************
 * Called at the beginning of the program
 */
{
#ifdef USE_DIR_CACHE
    Glob.cachedir = true;
#endif
}


void CacheRelease( void )
/*******************************
 * Called at any time we want to invalidate the cache
 */
{
#ifdef USE_DIR_CACHE
  #ifdef CACHE_STATS
    if( Glob.cachestat ) {
        PrtMsg( INF | CACHERELEASE );
    }
  #endif
    freeDirectList( cacheHead );
    cacheHead = NULL;
    MemShrink();
    CACHE_DELAY_RELEASE();
#endif
}


void CacheFini( void )
/****************************
 * Called while the program is exiting
 */
{
#ifdef USE_DIR_CACHE
    Glob.cachedir = false;
  #ifdef CACHE_STATS
    Glob.cachestat = false;
  #endif
  #ifndef NDEBUG
    CacheRelease();
  #endif
#endif
}


bool CacheTime( const char *fullpath, time_t *ptime )
/****************************************************
 * Given a full path to a file, get the st_mtime for that file.  If there
 * are no errors, return true, otherwise false.  If the file is in directory not
 * cached yet, then cache it first.
 */
{
    struct stat buf;
#ifdef USE_DIR_CACHE
    CENTRYPTR   centry;
#endif

    assert( fullpath != NULL && ptime != NULL );

#ifdef USE_DIR_CACHE
  #ifdef CACHE_DELAY_CHECK
    if( Glob.cachedir && CACHE_DELAY_CHECK() ) {
  #else
    if( Glob.cachedir ) {
  #endif
        switch( maybeCache( fullpath, &centry ) ) {
        case CACHE_OK:
            if( centry->ce_tt == YOUNGEST_DATE ) {
                if( stat( fullpath, &buf ) == 0 ) {
                    centry->ce_tt = buf.st_mtime;
                }
            }
            *ptime = centry->ce_tt;
            return( true );
        case CACHE_NOT_ENUF_MEM:
            break;
        default:
            return( false );
        }
    }
#endif
    if( stat( fullpath, &buf ) == 0 ) {
        *ptime = buf.st_mtime;
        return( true );
    }
    return( false );
}


bool CacheExists( const char *fullpath )
/***************************************
 * return true if the file in fullpath exists, false otherwise
 */
{
    assert( fullpath != NULL );

#ifdef USE_DIR_CACHE
  #ifdef CACHE_DELAY_CHECK
    if( Glob.cachedir && CACHE_DELAY_CHECK() ) {
  #else
    if( Glob.cachedir ) {
  #endif
        switch( maybeCache( fullpath, NULL ) ) {
        case CACHE_OK:
            return( true );
        case CACHE_NOT_ENUF_MEM:
            break;
        default:
            return( false );
        }
    }
#endif
    return( ExistFile( fullpath ) );
}

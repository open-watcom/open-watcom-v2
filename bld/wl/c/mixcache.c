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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
  MIXCACHE - object file caching routines which do both full file caching
                and paged caching.
*/

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "objio.h"
#include "objcache.h"


#define CACHE_PAGE_SIZE _8KB

static bool             Multipage;

static unsigned NumCacheBlocks( unsigned long len )
/*************************************************/
// figure out the number of cache blocks necessary
{
    unsigned    numblocks;

    numblocks = len / CACHE_PAGE_SIZE;
    if( (len % CACHE_PAGE_SIZE) != 0 ) {
        numblocks++;
    }
    return( numblocks );
}

bool CacheOpen( file_list *list )
/**************************************/
{
    infilelist  *infile;
    unsigned    numblocks;
    char        **cache;

    if( list == NULL )
        return( true );
    infile = list->infile;
    if( infile->status & INSTAT_IOERR )
        return( false );
    if( DoObjOpen( infile ) ) {
        infile->status |= INSTAT_IN_USE;
    } else {
        infile->status |= INSTAT_IOERR;
        return( false );
    }
    if( infile->len == 0 ) {
        infile->len = QFileSize( infile->handle );
        if( infile->len == 0 ) {
            LnkMsg( ERR+MSG_BAD_OBJECT, "s", infile->name );
            infile->status |= INSTAT_IOERR;
            return( false );
        }
    }
    if( (infile->status & INSTAT_SET_CACHE) == 0 ) {
        if( LinkFlags & LF_CACHE_FLAG ) {
            infile->status |= INSTAT_FULL_CACHE;
        } else if( LinkFlags & LF_NOCACHE_FLAG ) {
            infile->status |= INSTAT_PAGE_CACHE;
        } else {
            if( infile->status & INSTAT_LIBRARY ) {
                infile->status |= INSTAT_PAGE_CACHE;
            } else {
                infile->status |= INSTAT_FULL_CACHE;
            }
        }
    }
    if( infile->cache == NULL ) {
        if( infile->status & INSTAT_FULL_CACHE ) {
            _ChkAlloc( infile->cache, infile->len );
            if( infile->currpos != 0 ) {
                QLSeek( infile->handle, 0, SEEK_SET, infile->name.u.ptr );
            }
            QRead( infile->handle, infile->cache, infile->len, infile->name.u.ptr );
            infile->currpos = infile->len;
        } else {
            numblocks = NumCacheBlocks( infile->len );
            _Pass1Alloc( infile->cache, numblocks * sizeof( char * ) );
            cache = infile->cache;
            while( numblocks-- > 0 ) {
                *cache++ = NULL;
            }
        }
    }
    return( true );
}

static bool DumpFileCache( infilelist *infile, bool nuke )
/******************************************************/
{
    unsigned    num;
    unsigned    savenum;
    unsigned    index;
    char **     blocklist;
    bool        blockfreed;

    blockfreed = false;
    if( nuke ) {
        savenum = UINT_MAX;
    } else {
        savenum = infile->currpos / CACHE_PAGE_SIZE;
    }
    if( infile->cache != NULL ) {
        num = NumCacheBlocks( infile->len );
        blocklist = infile->cache;
        for( index = 0; index < num; index++ ) {
            if( index != savenum && *blocklist != NULL ) {
                _LnkFree( *blocklist );
                *blocklist = NULL;
                blockfreed = true;
            }
            blocklist++;
        }
    }
    return( blockfreed );
}

void CacheClose( file_list *list, unsigned pass )
/******************************************************/
{
    infilelist  *infile;
    bool        nukecache;

    if( list == NULL )
        return;
    infile = list->infile;
//    if( infile->handle == NIL_FHANDLE )
//        return;
    infile->status &= ~INSTAT_IN_USE;
    switch( pass ) {
    case 1: /* first pass */
        nukecache = ( (infile->status & INSTAT_LIBRARY) == 0 );
        if( infile->status & INSTAT_FULL_CACHE ) {
            if( nukecache ) {
                FreeObjCache( list );
            }
        } else {
            DumpFileCache( infile, nukecache );   // don't cache .obj's
        }
        break;
    case 3: /* freeing structure */
        FreeObjCache( list );
        if( infile->handle != NIL_FHANDLE ) {
            QClose( infile->handle, infile->name.u.ptr );
            infile->handle = NIL_FHANDLE;
        }
        break;
    }
}

void *CachePermRead( file_list *list, unsigned long pos, size_t len )
/*******************************************************************/
{
    char        *buf;
    char        *result;

    buf = CacheRead( list, pos, len );
    if( list->infile->status & INSTAT_FULL_CACHE )
        return( buf );
    if( Multipage ) {
        _LnkRealloc( result, buf, len );
        _ChkAlloc( TokBuff, TokSize );
        Multipage = false;              // indicate that last read is permanent.
    } else {
        _ChkAlloc( result, len );
        memcpy( result, buf, len );
    }
    return( result );
}

void *CacheRead( file_list *list, unsigned long pos, size_t len )
/***************************************************************/
/* read len bytes out of the cache. */
{
    size_t          bufnum;
    size_t          startnum;
    size_t          offset;
    size_t          amtread;
    char            *result;
    char            **cache;
    unsigned long   newpos;
    infilelist      *infile;

    if( list->infile->status & INSTAT_FULL_CACHE ) {
        if( pos + len > list->infile->len )
            return( NULL );
        return( (char *)list->infile->cache + pos );
    }
    Multipage = false;
    infile = list->infile;
    offset = pos % CACHE_PAGE_SIZE;
    amtread = CACHE_PAGE_SIZE - offset;
    startnum = pos / CACHE_PAGE_SIZE;
    bufnum = startnum;
    cache = infile->cache;
    for( ;; ) {
        if( cache[bufnum] == NULL ) {   // make sure page is in.
            _ChkAlloc( cache[bufnum], CACHE_PAGE_SIZE );
            newpos = (unsigned long)bufnum * CACHE_PAGE_SIZE;
            if( infile->currpos != newpos ) {
                QSeek( infile->handle, newpos, infile->name.u.ptr );
            }
            infile->currpos = newpos + CACHE_PAGE_SIZE;
            QRead( infile->handle, cache[bufnum], CACHE_PAGE_SIZE, infile->name.u.ptr );
        }
        if( amtread >= len )
            break;
        amtread += CACHE_PAGE_SIZE;     // it spans pages.
        bufnum++;
        Multipage = true;
    }
    if( !Multipage ) {
        result = cache[startnum] + offset;
    } else {
        if( len > TokSize ) {
            TokSize = ROUND_UP( len, SECTOR_SIZE );
            _LnkRealloc( TokBuff, TokBuff, TokSize );
        }
        amtread = CACHE_PAGE_SIZE - offset;
        memcpy( TokBuff, cache[startnum] + offset, amtread );
        len -= amtread;
        result = TokBuff + amtread;
        for( ;; ) {
            startnum++;
            if( len <= CACHE_PAGE_SIZE ) {
                memcpy( result, cache[startnum], len );
                break;
            } else {
                memcpy( result, cache[startnum], CACHE_PAGE_SIZE );
                len -= CACHE_PAGE_SIZE;
                result += CACHE_PAGE_SIZE;
            }
        }
        result = TokBuff;
    }
    return( result );
}

bool CacheIsPerm( void )
/*****************************/
{
    return( !Multipage );
}

bool CacheEnd( file_list *list, unsigned long pos )
/*********************************************************/
{
    return( pos >= list->infile->len );
}

void CacheFini( void )
/********************/
{
}

void CacheFree( file_list *list, void *mem )
/*************************************************/
// used for disposing things allocated by CachePermRead
{
    if( list->infile->status & INSTAT_PAGE_CACHE ) {
        _LnkFree( mem );
    }
}

void FreeObjCache( file_list *list )
/*****************************************/
{
    if( list == NULL )
        return;
    if( list->infile->status & INSTAT_FULL_CACHE ) {
        _LnkFree( list->infile->cache );
    } else {
        DumpFileCache( list->infile, true );
    }
    list->infile->cache = NULL;
}

bool DumpObjCache( void )
/******************************/
// find and dump an object file cache.
{
    infilelist *infile;

    for( infile = CachedFiles; infile != NULL; infile = infile->next ) {
        if( infile->status & INSTAT_PAGE_CACHE ) {
            if( CurrMod == NULL || CurrMod->f.source == NULL
                                || CurrMod->f.source->infile != infile ) {
                if( DumpFileCache( infile, true ) ) {
                    return( true );
                }
            }
        }
    }
    return( false );
}

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
* Description:  Search library files.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "linkstd.h"
#include "pcobj.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "library.h"
#include "objnode.h"
#include "fileio.h"
#include "objio.h"
#include "objcache.h"
#include "procfile.h"
#include "ar.h"
#include "omfhash.h"

typedef struct omf_dict_entry {
    void            **cache;        /* for extra memory store of dictionary */
    long            start;          /* recno of start of dictionary         */
    unsigned        pages;          /* number of pages in dictionary        */
    long            rec_length;     /* record alignment of obj recs         */
    unsigned_8      *buffer;
} omf_dict_entry;

typedef struct ar_dict_entry {
    unsigned_32         *filepostab;
    unsigned_16         *offsettab;
    char                **symbtab;
    unsigned_32         num_entries;
} ar_dict_entry;

typedef union dict_entry {
    omf_dict_entry      o;
    ar_dict_entry       a;
} dict_entry;

#define PAGES_IN_CACHE      0x40U

static  bool            OMFSearchExtLib( file_list *, char *, unsigned long * );
static  bool            ARSearchExtLib( file_list *, char *, unsigned long * );
static  unsigned        OMFCompName( const char *, const unsigned_8 *, unsigned );
static  void            **AllocDict( unsigned, unsigned );
static  void            SetDict( file_list *, unsigned );


static void BadLibrary( file_list *list )
/***************************************/
{
    list->file->flags |= INSTAT_IOERR;
    _LnkFree( list->u.dict );
    list->u.dict = NULL;
    Locator( list->file->name, NULL, 0 );
    LnkMsg( ERR+MSG_LIB_FILE_ATTR, NULL );
}

static int ReadOMFDict( file_list *list, unsigned_8 *header, bool makedict )
/***************************************************************************/
{
    omf_dict_entry  *omf_dict;
    unsigned        reclength;

    header += sizeof( unsigned_8 );
    reclength = _ReadLittleEndian16UN( header ) + 3;
    if( makedict ) {
        if( list->u.dict == NULL ) {
            _ChkAlloc( list->u.dict, sizeof( dict_entry ) );
        }
        omf_dict = &list->u.dict->o;
        omf_dict->cache = NULL;
        header += sizeof( unsigned_16 );
        omf_dict->start = _ReadLittleEndian32UN( header );
        header += sizeof( unsigned_32 );
        omf_dict->pages = _ReadLittleEndian16UN( header );
        header += sizeof( unsigned_16 );
        if( omf_dict->start == 0 || omf_dict->pages == 0 ) {
            BadLibrary( list );
            return( -1 );
        }
        omf_dict->rec_length = reclength;
    }
    return( reclength );
}

typedef unsigned    index_type;

static char         **d_symbtab;   /* pointer to AR dictionary structures */

static int ARCompI( const void *index1, const void *index2 )
/**********************************************************/
{
    return( stricmp( d_symbtab[*(index_type *)index1], d_symbtab[*(index_type *)index2] ) );
}

static int ARComp( const void *index1, const void *index2 )
/**********************************************************/
{
    return( strcmp( d_symbtab[*(index_type *)index1], d_symbtab[*(index_type *)index2] ) );
}

static void SortARDict( ar_dict_entry *ar_dict )
/**********************************************/
{
    index_type      *index_tab;
    index_type      ix;
    index_type      ix_next;
    index_type      ix1;
    char            *symb_save;
    unsigned_32     offset_save;
    ar_dict_entry   d;

    if( ar_dict->num_entries < 2 )
        return;

    d = *ar_dict;
    // store the dictionary pointer into memory so we can fetch ar_dict in ARCompI
    d_symbtab = d.symbtab;

    // Create an index table that we will sort to match the
    // case-insensitive sort order that we want for our symbol names.
    _ChkAlloc( index_tab, sizeof( index_type ) * d.num_entries );
    for( ix = 0; ix < d.num_entries; ix++ ) {
        index_tab[ix] = ix;
    }
    // Sort the index table using the corresponding symbol names
    // to determine the sort order (see ARCompI() for more info).
    if( LinkFlags & CASE_FLAG ) {
        qsort( index_tab, d.num_entries, sizeof( index_type ), ARComp );
    } else {
        qsort( index_tab, d.num_entries, sizeof( index_type ), ARCompI );
    }

    // Reorder the function name table (a vector of pointers to
    // symbol names) and the offset table (a vector of 16-bit offsets
    // into the file position table) (see ReadARDict() for info).
    for( ix = 0; ix < d.num_entries; ++ix ) {
        // If this entry hasn't been corrected
        // then move out the entry that is present
        // so that we can correct it.
        if( ix != index_tab[ix] ) {
            symb_save = d.symbtab[ix];
            if( d.offsettab == NULL ) {
                offset_save = d.filepostab[ix];
            } else {
                offset_save = d.offsettab[ix];
            }
            // Correct all the entries in this sequence
            for( ix1 = ix; (ix_next = index_tab[ix1]) != ix; ix1 = ix_next ) {
                d.symbtab[ix1] = d.symbtab[ix_next];
                if( d.offsettab == NULL ) {
                    d.filepostab[ix1] = d.filepostab[ix_next];
                } else {
                    d.offsettab[ix1] = d.offsettab[ix_next];
                }
                index_tab[ix1] = ix1;
            }
            // Update this final entry in the sequence from the
            // values we set aside.
            d.symbtab[ix1] = symb_save;
            if( d.offsettab == NULL ) {
                d.filepostab[ix1] = offset_save;
            } else {
                d.offsettab[ix1] = offset_save;
            }
            index_tab[ix1] = ix1;
        }
    }
    _LnkFree( index_tab );
}

static void ReadARDictData( file_list *list, unsigned long *loc, unsigned size, int numdicts )
/********************************************************************************************/
{
    ar_dict_entry   *dict;
    char            *data;
    unsigned_32     num;
    unsigned_32     index;

    /* a dictionary in an AR archive commonly starts with a header marked with '/ '.

    Be careful, more AR formats exists, GNU, BSD and COFF. Each handle dictionaries
    a little bit different way.

    GNU (Linux) and BSD AR archives commonly have only one dictionary which is of the form:

    (all numbers in *big* endian format)
    unsigned_32: number of entries (num)
    num unsigned_32's: offset within AR file of the object file that the symbol name belongs too.
    num zero terminated strings: the symbols themselves (unsorted).

    COFF AR archieves however use two dictionaries.
    First one is the same as for GNU and BSD, but WLINK uses second one, more efficient dictionary:

    (all numbers in *little* endian format)
    unsigned_32: number of files in object
    nfiles unsigned_32's: offsets of the files within the archive
    unsigned_32: number of entries (num)
    num unsigned_16's: the file number that the symbol belongs to
    num zero terminated strings: the symbols themselves (sorted case-sensitively).

    however WLINK needs to be able to parse both kinds.
    (dict->offsettab == NULL) means that we only know about an unsorted dictionary and
    it will be sorted later.
    */

    dict = &list->u.dict->a;
    data = CachePermRead( list, *loc, size );
    if( numdicts == 1 ) {
        num = _ReadBigEndian32UN( data ); /* number of symbols */
        data += sizeof( unsigned_32 );
        dict->filepostab = (unsigned_32 *)data;
        for( index = 0; index < num; index++ ) {
            dict->filepostab[index] = _ReadBigEndian32UN( data );
            data += sizeof( unsigned_32 );
        }
        dict->num_entries = num;
        dict->offsettab = NULL;
        dict->symbtab = NULL;
        if( num > 0 ) {
            _ChkAlloc( dict->symbtab, sizeof( char * ) * num );
        }
    } else /* if( numdicts == 2 ) */ {
        num = _ReadLittleEndian32UN( data );    /* number of files */
        data += sizeof( unsigned_32 );
        dict->filepostab = (unsigned_32 *)data; /* first file off */
        for( index = 0; index < num; index++ ) {
            dict->filepostab[index] = _ReadLittleEndian32UN( data );
            data += sizeof( unsigned_32 );
        }
        num = _ReadLittleEndian32UN( data );    /* number of symbols */
        data += sizeof( unsigned_32 );
        dict->offsettab = (unsigned_16 *)data;  /* first offset */
        for( index = 0; index < num; index++ ) {
            dict->offsettab[index] = _ReadLittleEndian16UN( data );
            data += sizeof( unsigned_16 );
        }
        dict->num_entries = num;
        if( num > 0 && dict->symbtab == NULL ) {
            _ChkAlloc( dict->symbtab, sizeof( char * ) * num );
        }
    }
    for( index = 0; index < num; index++ ) {
        dict->symbtab[index] = data;
        data += strlen( data ) + 1;
    }
}

static void ReadARStringTable( file_list *list, unsigned long *loc, unsigned size )
/*********************************************************************************/
{
    char            *data;
    unsigned        i;

    if( list->strtab == NULL && size > 0 ) {
        list->strtab = CachePermRead( list, *loc, size );
        data = list->strtab;
        for( i = 0; i < size; ++i ) {
            if( *data == '\n' || *data == '/' && *(data + 1) == '\n' )
                *data = '\0';
            ++data;
        }
    }
}

static bool ReadARDict( file_list *list, unsigned long *loc, bool makedict )
/**************************************************************************/
{
    ar_header       *ar_hdr;
    unsigned long   size;
    int             numdicts;

    numdicts = 0;
    if( makedict ) {
        if( list->u.dict == NULL ) {
            _ChkAlloc( list->u.dict, sizeof( dict_entry ) );
        }
    }
    for( ;; ) {
        ar_hdr = CacheRead( list, *loc, sizeof( ar_header ) );
        size = GetARValue( ar_hdr->size, AR_SIZE_LEN );
        if( ar_hdr->name[0] == '/' && ar_hdr->name[1] == ' ' ) {
            ++numdicts;
            *loc += sizeof( ar_header );
            if( makedict )
                ReadARDictData( list, loc, size, numdicts );
            *loc += MAKE_EVEN( size );
        } else if( ar_hdr->name[0] == '/' && ar_hdr->name[1] == '/' ) {
            *loc += sizeof( ar_header );
            ReadARStringTable( list, loc, size );
            *loc += MAKE_EVEN( size );
        } else {
            break;         // found an actual object file
        }
    }
    if( makedict ) {
        if( numdicts == 0 ) {
            Locator( list->file->name, NULL, 0 );
            LnkMsg( ERR+MSG_NO_DICT_FOUND, NULL );
            _LnkFree( list->u.dict );
            list->u.dict = NULL;
            return( FALSE );
        }
        if( !(LinkFlags & CASE_FLAG) || numdicts == 1 ) {
            SortARDict( &list->u.dict->a );
        }
    }
    return( TRUE );
}

int CheckLibraryType( file_list *list, unsigned long *loc, bool makedict )
/************************************************************************/
{
    unsigned_8          *header;
    int                 reclength;

    reclength = 0;
    header = CacheRead( list, *loc, sizeof( lib_header ) );
    if( header[0] == 0xf0 && header[1] == 0x01 ) {
        // COFF object for PPC
    } else if( header[0] == LIB_HEADER_REC ) {   // reading from a library
        list->status |= STAT_OMF_LIB;
        reclength = ReadOMFDict( list, header, makedict );
        if( reclength < 0 ) {
            return( -1 );
        }
        *loc += ROUND_UP( sizeof( lib_header ), reclength );
    } else if( memcmp( header, AR_IDENT, AR_IDENT_LEN ) == 0 ) {
        list->status |= STAT_AR_LIB;
        reclength = 2;
        *loc += AR_IDENT_LEN;
        if( !ReadARDict( list, loc, makedict ) ) {
            return( -1 );
        }
    }
    return( reclength );
}

mod_entry *SearchLib( file_list *lib, char *name )
/********************************************************/
/* Search the specified library file for the specified name & make a module */
{
    mod_entry           *obj;
    unsigned long       pos;
    unsigned long       dummy;
    bool                retval;

    pos = 0;
    if( lib->u.dict == NULL ) {
        if( CheckLibraryType( lib, &pos, TRUE ) == -1 )
            return( NULL );
        if( !(lib->status & STAT_IS_LIB) ) {
            BadLibrary( lib );
            return( NULL );
        }
    }
    if( lib->status & STAT_OMF_LIB ) {
        retval = OMFSearchExtLib( lib, name, &pos );
    } else {
        retval = ARSearchExtLib( lib, name, &pos );
    }
    if( !retval )
        return( NULL );

/*
    update lib struct since we found desired object file
*/
    obj = NewModEntry();
    obj->name = IdentifyObject( lib, &pos, &dummy );
    obj->location = pos;
    obj->f.source = lib;
    obj->modtime = lib->file->modtime;
    obj->modinfo = (lib->status & DBI_MASK) | (ObjFormat & FMT_OBJ_FMT_MASK);
    return( obj );
}


static bool OMFSearchExtLib( file_list *lib, char *name, unsigned long *off )
/***************************************************************************/
/* Search library for specified member. */
{
    unsigned        num_blocks;
    unsigned        sector;
    unsigned        i;
    unsigned        j;
    hash_entry      h;
    omf_dict_entry  *dict;

    dict = &lib->u.dict->o;
    num_blocks = dict->pages;

    omflib_hash( name, strlen( name ), &h, num_blocks );

    for( i = 0; i < num_blocks; ++i ) {
        SetDict( lib, h.block );
        for( j = 0; j < NUM_BUCKETS; ++j ) {
            if( dict->buffer[h.bucket] == LIB_NOT_FOUND ) {
                if( dict->buffer[NUM_BUCKETS] != LIB_FULL_PAGE )
                    return( FALSE );
                break;
            }
            sector = OMFCompName( name, dict->buffer, h.bucket );
            if( sector != 0 ) {
                *off = dict->rec_length * sector;
                return( TRUE );
            }
            h.bucket += h.bucketd;
            if( h.bucket >= NUM_BUCKETS ) {
                h.bucket -= NUM_BUCKETS;
            }
        }
        h.block += h.blockd;
        if( h.block >= num_blocks ) {
            h.block -= num_blocks;
        }
    }
    return( FALSE );
}

static void SetDict( file_list *lib, unsigned dict_page )
/*******************************************************/
/* set lib->buffer to the dict_page th page in lib 's dictionary */
{
    unsigned        pages;
    unsigned        num_buckets;
    unsigned        residue;
    unsigned        bucket;
    long   off;
    long   dictoff;
    omf_dict_entry  *dict;

    dict = &lib->u.dict->o;
    if( dict->cache == NULL ) {
        pages = dict->pages;
        num_buckets = pages / PAGES_IN_CACHE;
        residue = pages - num_buckets * PAGES_IN_CACHE;
        dict->cache = AllocDict( num_buckets, residue );
        if( dict->cache != NULL ) {
            QSeek( lib->file->handle, dict->start, lib->file->name );
            for( bucket = 0; bucket < num_buckets; ++bucket ) {
                QRead( lib->file->handle, dict->cache[ bucket ], DIC_REC_SIZE * PAGES_IN_CACHE, lib->file->name );
            }
            QRead( lib->file->handle, dict->cache[bucket], DIC_REC_SIZE * residue, lib->file->name );
            lib->file->currpos = dict->start + DIC_REC_SIZE * ( residue + PAGES_IN_CACHE * num_buckets );
        }
    }
    if( dict->cache == NULL ) {
        off = dict_page * DIC_REC_SIZE;
        dictoff = dict->start + off;
        dict->buffer = (unsigned_8 *)TokBuff;
        QSeek( lib->file->handle, dictoff, lib->file->name );
        QRead( lib->file->handle, dict->buffer, DIC_REC_SIZE, lib->file->name );
        lib->file->currpos = dictoff + DIC_REC_SIZE;
    } else {
        bucket = dict_page / PAGES_IN_CACHE;
        residue = dict_page - bucket * PAGES_IN_CACHE;
        dict->buffer = (unsigned_8 *)dict->cache[bucket] + residue * DIC_REC_SIZE;
    }
}


static void FreeDictCache( void **cache, unsigned buckets )
/*********************************************************/
{
    while( buckets != 0 ) {
        _LnkFree( cache[--buckets] );
    }
    _LnkFree( cache );
}

static void **AllocDict( unsigned num_buckets, unsigned residue )
/***************************************************************/
/* allocate a chunk of dict memory, down from the top */
{
    void            **cache;
    unsigned        bucket;

    _LnkAlloc( cache, sizeof( void * ) * ( num_buckets + 1 ) );
    if( cache == NULL )
        return( NULL );
    for( bucket = 0; bucket < num_buckets; ++bucket ) {
        _LnkAlloc( cache[ bucket ], DIC_REC_SIZE * PAGES_IN_CACHE );
        if( cache[ bucket ] == NULL ) {
            FreeDictCache( cache, bucket );
            return( NULL );
        }
    }

    if( residue != 0 ) {
        _LnkAlloc( cache[bucket], residue * DIC_REC_SIZE );
        if( cache[bucket] == NULL ) {
            FreeDictCache( cache, bucket );
            return( NULL );
        }
    } else {
       cache[bucket] = NULL;
    }
    return( cache );
}


bool DiscardDicts( void )
/******************************/
/* called when dictionaries forced out of dict memory */
{
    omf_dict_entry      *ptr;
    file_list           *curr;

    ptr = NULL;
    for( curr = ObjLibFiles; curr != NULL; curr = curr->next_file ) {
        if( curr->u.dict == NULL )
            continue;
        if( curr->status & STAT_AR_LIB )
            continue;
        if( curr->u.dict->o.cache == NULL )
            continue;
        ptr = &curr->u.dict->o;
    }
    if( ptr == NULL )
        return( FALSE ); /* no dicts in memory */
    FreeDictCache( ptr->cache, ( ptr->pages / PAGES_IN_CACHE ) + 1 );
    ptr->cache = NULL;
    return( TRUE );
}


void BurnLibs( void )
/**************************/
/* let dict memory know it's no longer needed */
{
    file_list   *temp;
    dict_entry  *dict;

    for( temp = ObjLibFiles; temp != NULL; temp = temp->next_file ) {
        if( temp->status & STAT_AR_LIB ) {
            CacheFree( temp, temp->strtab );
            temp->strtab = NULL;
        }
        dict = temp->u.dict;
        if( dict == NULL )
            continue;
        if( temp->status & STAT_AR_LIB ) {
            CacheFree( temp, dict->a.filepostab - 1 );
            _LnkFree( dict->a.symbtab );
        } else {
            if( dict->o.cache != NULL ) {
                FreeDictCache( dict->o.cache, ( dict->o.pages / PAGES_IN_CACHE ) + 1 );
            }
        }
        _LnkFree( dict );
        temp->u.dict = NULL;
        FreeObjCache( temp );
    }
}

static unsigned OMFCompName( const char *name, const unsigned_8 *buff, unsigned index )
/*************************************************************************************/
/* Compare name. */
{
    unsigned    len;
    unsigned    off;
    unsigned    returnval;
    int         result;

    returnval = 0;
    off = buff[index];
    buff += off * 2;
    len = *buff++;
    if( LinkFlags & CASE_FLAG ) {
        result = memcmp( buff, name, len );
    } else {
        result = memicmp( buff, name, len );
    }
    if( result == 0 && name[len] == '\0' ) {
        returnval = _ReadLittleEndian16UN( buff + len );
    }
    return( returnval );
}

static int ARCompName( const void *key, const void *vbase )
/**********************************************************/
{
    char        **base;

    base = (char **)vbase;
    return( strcmp( key, *base ) );
}

static int ARCompIName( const void *key, const void *vbase )
/**********************************************************/
{
    char        **base;

    base = (char **)vbase;
    return( stricmp( key, *base ) );
}

static bool ARSearchExtLib( file_list *lib, char *name, unsigned long *off )
/**************************************************************************/
/* Search AR format library for specified member. */
{
    char                **result;
    ar_dict_entry       *dict;
    unsigned            tabidx;

    dict = &lib->u.dict->a;
    if( LinkFlags & CASE_FLAG ) {
        result = bsearch( name, dict->symbtab, dict->num_entries, sizeof( char * ), ARCompName );
    } else {
        result = bsearch( name, dict->symbtab, dict->num_entries, sizeof( char * ), ARCompIName );
    }
    if( result != NULL ) {
        tabidx = result - dict->symbtab;
        if( dict->offsettab == NULL ) {
            *off = dict->filepostab[tabidx];
        } else {
            *off = dict->filepostab[dict->offsettab[tabidx] - 1];
        }
        return( TRUE );
    }
    return( FALSE );
}

char *GetARName( ar_header *header, file_list *list, unsigned long *loc )
/***********************************************************************/
{
    char            *buf;
    char            *name;
    unsigned long   val;
    size_t          len;

    name = NULL;
    if( header->name[0] == '/' ) {
        val = GetARValue( &header->name[1], AR_NAME_LEN - 1 );
        buf = list->strtab + val;
        len = strlen( buf );
    } else if( header->name[0] == '#' && header->name[1] == '1' && header->name[2] == '/') {
        len = GetARValue( &header->name[3], AR_NAME_LEN - 3 );
        buf = CacheRead( list, *loc, len );
        *loc += len;
    } else {
        len = AR_NAME_LEN;
        buf = memchr( header->name, '/', len );
        if( buf != NULL ) {
            len = buf - header->name;
        }
        buf = header->name;
    }
    if( len > 0 ) {
        name = ChkToString( buf, len );
    }
    return( name );
}

unsigned long GetARValue( char *str, unsigned max )
/***************************************************/
// get a numeric value from an ar_header
{
    char                save;
    unsigned long       value;

    save = *(str + max);
    *(str + max) = '\0';
    value = strtoul( str, NULL, 10 );
    *(str + max) = save;
    return( value );
}

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


/*
 *  LIB      : search library file.
 *
 */

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
#include <ar.h>

typedef struct omf_dict_entry {
    void **     cache;          /* for extra memory store of dictionary */
    unsigned_32 start;          /* recno of start of dictionary         */
    unsigned_16 pages;          /* number of pages in dictionary        */
    unsigned_16 rec_length;     /* record alignment of obj recs         */
    byte *      buffer;
} omf_dict_entry;

typedef struct ar_dict_entry {
    unsigned_32 *       filepostab;
    unsigned_16 *       offsettab;
    char **             fnametab;
    unsigned_32         num_entries;
} ar_dict_entry;

typedef union dict_entry {
    omf_dict_entry      o;
    ar_dict_entry       a;
} dict_entry;

#define PAGES_IN_CACHE      0x40U

static  bool            OMFSearchExtLib( file_list *, char *, unsigned long * );
static  bool            ARSearchExtLib( file_list *, char *, unsigned long * );
static  unsigned_16     OMFCompName( char *, char *, unsigned_16 );
static  void **         AllocDict( unsigned_16, unsigned_16 );

#if defined( __386__ ) || defined(M_I86)
#if defined( __386__ )
#define DATA16   0x66
#else
#define DATA16
#endif

unsigned_16 Rotr16( unsigned_16 value, unsigned_16 shift );
#pragma aux Rotr16 =\
    DATA16 0xd3 0xc8 /* ror ax,cl */ \
    parm            [ax] [cx]   \
    value           [ax];
unsigned_16 Rotl16( unsigned_16 value, unsigned_16 shift );
#pragma aux Rotl16 =\
    DATA16 0xd3 0xc0 /* rol ax,cl */ \
    parm            [ax] [cx]   \
    value           [ax];
#else
static unsigned_16 Rotr16( unsigned_16 value, unsigned_16 shift )
/***************************************************************/
{
    return (value << (16 - shift)) | (value >> shift);
}

static unsigned_16 Rotl16( unsigned_16 value, unsigned_16 shift )
/***************************************************************/
{
    return (value >> (16 - shift)) | (value << shift);
}
#endif

static void BadLibrary( file_list *list )
/***************************************/
{
    list->file->flags |= INSTAT_IOERR;
    _LnkFree( list->u.dict );
    list->u.dict = NULL;
    Locator( list->file->name, NULL, 0 );
    LnkMsg( ERR+MSG_LIB_FILE_ATTR, NULL );
}

static bool ReadARDict( file_list *list, unsigned long *loc, unsigned *numdicts,
                        bool makedict )
/*****************************************************************************/
{
    ar_header *         ar_hdr;
    unsigned long       size;
    ar_dict_entry *     dict;
    char *              data;
    unsigned_32         num;
    unsigned            index;
    bool                retval;

    ar_hdr = CacheRead( list, *loc, sizeof(ar_header) );
    size = GetARValue( ar_hdr->size, AR_SIZE_LEN );
    retval = TRUE;
    if( ar_hdr->name[0] == '/' && ar_hdr->name[1] == ' ' ) {
        *numdicts += 1;
        *loc += sizeof(ar_header);
        if( *numdicts == 2 && makedict ) {
            dict = &list->u.dict->a;
            data = CachePermRead( list, *loc, size );
            num = *((unsigned_32 *)data);
            data += sizeof(unsigned_32);
            dict->filepostab = (unsigned_32 *) data;
            data += num * sizeof(unsigned_32);
            num = *((unsigned_32 *)data);
            data += sizeof(unsigned_32);
            dict->offsettab = (unsigned_16 *) data;
            data += num * sizeof(unsigned_16);
            dict->num_entries = num;
            if( num > 0 ) {
                _ChkAlloc( dict->fnametab, sizeof(char *) * num );
                for( index = 0; index < num; index++ ) {
                    dict->fnametab[index] = data;
                    data = strchr( data, '\0' ) + 1;
                }
            } else {
                dict->fnametab = NULL;
            }
        }
        *loc += MAKE_EVEN( size );
    } else if( ar_hdr->name[0] == '/' && ar_hdr->name[1] == '/' ) {
        *loc += sizeof(ar_header);
        list->strtab = CachePermRead( list, *loc, size );
        *loc += MAKE_EVEN( size );
    } else {
        retval = FALSE;         // found an actual object file
    }
    return retval;
}


static ar_dict_entry *ARDict;   /* pointer to AR dictionary structures */

static int ARCompI( const unsigned_16 *index1, const unsigned_16 *index2 )
/************************************************************************/
{
    return stricmp( ARDict->fnametab[ *index1 ], ARDict->fnametab[ *index2 ] );
}

extern int CheckLibraryType( file_list *list, unsigned long *loc, bool makedict)
/******************************************************************************/
{
    lib_header *        omf_hdr;
    unsigned_8 *        header;
    int                 reclength;
    omf_dict_entry *    omf_dict;
    ar_dict_entry *     ar_dict;
    unsigned            numdicts;

    unsigned_16 *       index_tab;
    unsigned            ix;
    unsigned            ix_next;
    unsigned            ix_save;
    char *              fname_save;
    unsigned_16         offset_save;

    omf_hdr = CacheRead( list, 0, sizeof(lib_header) );
    header = (unsigned_8*) omf_hdr;
    reclength = 0;

    if( makedict ) {
        _ChkAlloc( list->u.dict, sizeof(dict_entry) );
    }
    if( header[0] == 0xf0 && header[1] == 0x01 ) {
        // COFF object for PPC
    } else if( omf_hdr->cmd == LIB_HEADER_REC ) {   // reading from a library
        list->status |= STAT_OMF_LIB;
        omf_dict = &list->u.dict->o;
        _TargU16toHost( omf_hdr->length, reclength );
        reclength += 3;
        if( makedict ) {
            omf_dict->cache = NULL;
            if( omf_hdr->dict_start == 0 || omf_hdr->dict_size == 0 ) {
                BadLibrary( list );
                return -1;
            }
            _TargU16toHost( omf_hdr->dict_size, omf_dict->pages );
            _TargU32toHost( omf_hdr->dict_start, omf_dict->start );
            omf_dict->rec_length = reclength;
        }
        *loc = CalcAlign( sizeof(lib_header), reclength ) + sizeof(lib_header);
    } else if( memcmp( (void *)omf_hdr, AR_IDENT, AR_IDENT_LEN ) == 0 ) {
        list->status |= STAT_AR_LIB;
        reclength = 2;
        *loc = AR_IDENT_LEN;
        numdicts = 0;
        while( ReadARDict( list, loc, &numdicts, makedict ) ) {} // NULL loop
        if( numdicts < 2 && makedict ) {
            Locator( list->file->name, NULL, 0 );
            LnkMsg( ERR+MSG_NO_DICT_FOUND, NULL );
            _LnkFree( list->u.dict );
            list->u.dict = NULL;
            return -1;
        }
        ar_dict = &list->u.dict->a;
        if( (!(LinkFlags & CASE_FLAG)) && (ar_dict->num_entries > 0) ) {
            // Create an index table that we will sort to match the
            // case-insensitive sort order that we want for our symbol names.
            _ChkAlloc( index_tab, sizeof(unsigned_16) * ar_dict->num_entries );
            for( ix = 0; ix < ar_dict->num_entries; ix++ ) {
                index_tab[ix] = ix;
            }
            // store the dictionary pointer into memory so we can fetch ar_dict in ARCompI
            ARDict = ar_dict;
            // Sort the index table using the corresponding symbol names
            // to determine the sort order (see ARCompI() for more info).
            qsort( index_tab, ar_dict->num_entries, sizeof(unsigned_16),
                ARCompI );

            // Reorder the function name table (a vector of pointers to
            // symbol names) and the offset table (a vector of 16-bit offsets
            // into the file position table) (see ReadARDict() for info).
            for( ix = 0; ix < ar_dict->num_entries; ) {
                ix_save = ix;
                // If this entry hasn't been corrected
                // then move out the entry that is present
                // so that we can correct it.
                if( ix != index_tab[ix] ) {
                    fname_save = ar_dict->fnametab[ix];
                    offset_save = ar_dict->offsettab[ix];
                    // Correct all the entries in this sequence
                    for(;;) {
                        ix_next = index_tab[ix];
                        index_tab[ix] = ix;

                        if( ix_next == ix_save ) break;

                        ar_dict->fnametab[ix] = ar_dict->fnametab[ix_next];
                        ar_dict->offsettab[ix] = ar_dict->offsettab[ix_next];
                        ix = ix_next;
                    }
                    // Update this final entry in the sequence from the
                    // values we set aside.
                    ar_dict->fnametab[ix] = fname_save;
                    ar_dict->offsettab[ix] = offset_save;
                }
                ix = ix_save + 1;
            }

            _LnkFree( index_tab );
        }
    }
    return reclength;
}

extern mod_entry * SearchLib( file_list *lib, char *name )
/********************************************************/
/* Search the specified library file for the specified name & make a module */
{
    mod_entry *         obj;
    unsigned long       pos;
    bool                retval;

    if( lib->u.dict == NULL ) {
        if( !CacheOpen( lib ) ) return( NULL );
        if( CheckLibraryType( lib, &pos, TRUE ) == -1 ) return( NULL );
        if( !(lib->status & STAT_IS_LIB) ) {
            BadLibrary( lib );
            return NULL;
        }
    } else {
        if( !CacheOpen( lib ) ) return( NULL );
    }
    if( lib->status & STAT_OMF_LIB ) {
        retval = OMFSearchExtLib( lib, name, &pos );
    } else {
        retval = ARSearchExtLib( lib, name, &pos );
    }
    if( !retval ) return NULL;

/*
    update lib struct since we found desired object file
*/
    obj = NewModEntry();
    obj->location = pos;
    obj->f.source = lib;
    obj->modtime = obj->f.source->file->modtime;
    obj->modinfo = lib->status & DBI_MASK;
    return( obj );
}


static void HashSymbol( hash_entry *hash, char *name )
/****************************************************/
/* Hash specified name. */
{
    unsigned_16     major_class;
    unsigned_16     minor_class;
    unsigned_16     major_inc;
    unsigned_16     minor_inc;
    unsigned_16     count;
    unsigned_16     curr;
    unsigned char * leftptr;
    unsigned char * rightptr;

    count = strlen( name );
    leftptr = name;
    rightptr = leftptr + count;
    major_class = count | 0x20;
    major_inc = 0;
    minor_class = 0;
    minor_inc = count | 0x20;
    for(;;) {
        --rightptr;
        curr = *rightptr | 0x20;/*  zap to lower case (sort of) */
        major_inc = curr ^ Rotl16( major_inc, 2 );
        minor_class = curr ^ Rotr16( minor_class, 2 );
        if( --count == 0 ) break;
        curr = *leftptr | 0x20;
        ++leftptr;
        major_class = curr ^ Rotl16( major_class, 2 );
        minor_inc = curr ^ Rotr16( minor_inc, 2 );
    }
    minor_class = minor_class % 37;
    minor_inc = minor_inc % 37;
    if( minor_inc == 0 ) {
        minor_inc = 1;
    }
    hash->major_class = major_class;
    hash->major_inc = major_inc;
    hash->minor_class = minor_class;
    hash->minor_inc = minor_inc;
}


static bool OMFSearchExtLib( file_list *lib, char *name, unsigned long *off )
/***************************************************************************/
/* Search library for specified member. */
{
    unsigned_16     minor_class;
    unsigned_16     major_count;
    unsigned_16     major_class;
    unsigned_16     major_inc;
    unsigned_16     minor_count;
    unsigned_16     sector;
    hash_entry      hash;
    omf_dict_entry *dict;

    dict = &lib->u.dict->o;
    major_count = dict->pages;
    HashSymbol( &hash, name );
    major_class = hash.major_class % major_count;
    major_inc = hash.major_inc % major_count;
    if( major_inc == 0 ) {
        major_inc = 1;
    }
    minor_class = hash.minor_class;  /* ATK */
    do {/*  over major classes */
        SetDict( lib, major_class );
        minor_count = 37;
        do {/*  over minor classes */
            if( dict->buffer[ minor_class ] == LIB_NOT_FOUND ) {
                if( dict->buffer[ 37 ] != LIB_FULL_PAGE ) return( FALSE );
                break;
            }
            sector = OMFCompName( name, dict->buffer, minor_class );
            if( sector != 0 ) {
                *off = (unsigned long) dict->rec_length * sector;
                return TRUE;
            }
            minor_class += hash.minor_inc;
            if( minor_class >= 37 ) {
                minor_class -= 37;
            }
        } while( !( --minor_count == 0 ) );
        major_class += major_inc;
        if( major_class >= dict->pages ) {
            major_class -= dict->pages;
        }
    } while( !( --major_count == 0 ) );
    return( FALSE );
}

static void SetDict( file_list *lib, unsigned_16 dict_page )
/**********************************************************/
/* set lib->buffer to the dict_page th page in lib 's dictionary */
{
    unsigned_16     pages;
    unsigned_16     num_buckets;
    unsigned_16     residue;
    unsigned_16     bucket;
    unsigned long   off;
    unsigned long   dictoff;
    omf_dict_entry *dict;

    dict = &lib->u.dict->o;
    if( dict->cache == NULL ) {
        pages = dict->pages;
        num_buckets = pages / PAGES_IN_CACHE;
        residue = pages - num_buckets * PAGES_IN_CACHE;
        dict->cache = AllocDict( num_buckets, residue );
        if( dict->cache != NULL ) {
            QSeek( lib->file->handle, dict->start, lib->file->name );
            for( bucket = 0; bucket != num_buckets; ++bucket ) {
                QRead( lib->file->handle, dict->cache[ bucket ],
                    DIC_REC_SIZE * PAGES_IN_CACHE, lib->file->name );
            }
            QRead( lib->file->handle, dict->cache[ bucket ],
                    DIC_REC_SIZE * residue, lib->file->name );
            lib->file->currpos = dict->start + DIC_REC_SIZE * residue +
                (unsigned long)  DIC_REC_SIZE * PAGES_IN_CACHE * num_buckets;
        }
    }
    if( dict->cache == NULL ) {
        off = dict_page;
        off *= DIC_REC_SIZE;
        dictoff = dict->start + off;
        dict->buffer = TokBuff;
        QSeek( lib->file->handle, dictoff, lib->file->name );
        QRead( lib->file->handle, dict->buffer, DIC_REC_SIZE, lib->file->name );
        lib->file->currpos = dictoff + DIC_REC_SIZE;
    } else {
        bucket = dict_page / PAGES_IN_CACHE;
        residue = dict_page - bucket * PAGES_IN_CACHE;
        dict->buffer = (char *)dict->cache[ bucket ] + residue * DIC_REC_SIZE;
    }
}


static void FreeDictCache( void **cache, unsigned_16 buckets )
/************************************************************/
{
    while( buckets != 0 ) {
        _LnkFree( cache[ --buckets ] );
    }
    _LnkFree( cache );
}

static void ** AllocDict( unsigned_16 num_buckets, unsigned_16 residue )
/**********************************************************************/
/* allocate a chunk of dict memory, down from the top */
{
    void **         cache;
    unsigned_16     bucket;

    _LnkAlloc( cache,sizeof( void * ) * ( num_buckets + 1 ) );
    if( cache == NULL ) return( NULL );
    for( bucket = 0; bucket != num_buckets; ++bucket ) {
        _LnkAlloc( cache[ bucket ], DIC_REC_SIZE * PAGES_IN_CACHE );
        if( cache[ bucket ] == NULL ) {
            FreeDictCache( cache, bucket );
            return( NULL );
        }
    }

    if( residue != 0 ) {
        _LnkAlloc( cache[ bucket ], residue * DIC_REC_SIZE );
        if( cache[ bucket ] == NULL ) {
            FreeDictCache( cache, bucket );
            return( NULL );
        }
    } else {
       cache[ bucket ] = NULL;
    }
    return( cache );
}


extern bool DiscardDicts( void )
/******************************/
/* called when dictionaries forced out of dict memory */
{
    omf_dict_entry *    ptr;
    file_list *         curr;

    ptr = NULL;
    for( curr = ObjLibFiles; curr != NULL; curr = curr->next_file ) {
        if( curr->u.dict == NULL ) continue;
        if( curr->status & STAT_AR_LIB ) continue;
        if( curr->u.dict->o.cache == NULL ) continue;
        ptr = &curr->u.dict->o;
    }
    if( ptr == NULL ) return( FALSE ); /* no dicts in memory */
    FreeDictCache( ptr->cache, (ptr->pages/PAGES_IN_CACHE) + 1 );
    ptr->cache = NULL;
    return( TRUE );
}


extern void BurnLibs( void )
/**************************/
/* let dict memory know it's no longer needed */
{
    file_list * temp;
    dict_entry *dict;

    for( temp = ObjLibFiles; temp != NULL; temp = temp->next_file ) {
        if( temp->status & STAT_AR_LIB ) {
            CacheFree( temp, temp->strtab );
            temp->strtab = NULL;
        }
        dict = temp->u.dict;
        if( dict == NULL ) continue;
        if( temp->status & STAT_AR_LIB ) {
            CacheFree( temp, dict->a.filepostab - 1 );
            _LnkFree( dict->a.fnametab );
        } else {
            if( dict->o.cache != NULL ) {
                FreeDictCache( dict->o.cache, (dict->o.pages/PAGES_IN_CACHE)+1);
            }
        }
        _LnkFree( dict );
        temp->u.dict = NULL;
        FreeObjCache( temp );
    }
}

static unsigned_16 OMFCompName( char *name, char *buff, unsigned_16 index )
/*************************************************************************/
/* Compare name. */
{
    int         len;
    unsigned_16 off;
    unsigned_16 returnval;
    size_t      result;

    returnval = 0;
    off = buff[ index ];
    buff += off * 2;
    len = *buff++;
    if( LinkFlags & CASE_FLAG ) {
        result = memcmp( buff, name, len );
    } else {
        result = memicmp( buff, name, len );
    }
    if( result == 0 && name[len] == '\0' ) {
        _TargU16toHost( _GetU16( &buff[ len ] ), returnval );
    }
    return( returnval );
}

static int ARCompName( const void *key, const void *vbase )
/**********************************************************/
{
    char **     base;

    base = (char **)vbase;
    return strcmp( key, *base );
}

static int ARCompIName( const void *key, const void *vbase )
/**********************************************************/
{
    char **     base;

    base = (char **)vbase;
    return stricmp( key, *base );
}

static bool ARSearchExtLib( file_list *lib, char *name, unsigned long *off )
/**************************************************************************/
/* Search AR format library for specified member. */
{
    char **             result;
    ar_dict_entry *     dict;
    unsigned            tabidx;

    dict = &lib->u.dict->a;
    if( LinkFlags & CASE_FLAG ) {
        result = bsearch( name, dict->fnametab, dict->num_entries,
                          sizeof(char *), ARCompName );
    } else {
        result = bsearch( name, dict->fnametab, dict->num_entries,
                          sizeof(char *), ARCompIName );
    }
    if( result != NULL ) {
        tabidx = result - dict->fnametab;
        *off = dict->filepostab[dict->offsettab[tabidx] - 1];
        return TRUE;
    }
    return FALSE;
}

extern char *GetARName( ar_header *header, file_list *list )
/**********************************************************/
{
    char *              buf;
    char *              name;
    unsigned long       len;

    name = NULL;
    if( header->name[0] == '/' ) {
        len = GetARValue( &header->name[1], AR_NAME_LEN - 1 );
        buf = list->strtab + len;
        len = strlen( buf );
    } else {
        buf = memchr( header->name, '/', AR_NAME_LEN );
        if( buf == NULL ) {
            len = AR_NAME_LEN;
        } else {
            len = buf - header->name;
        }
        buf = header->name;
    }
    if( len > 0 ) {
        _ChkAlloc( name, len + 1 );
        memcpy( name, buf, len );
        name[len] = '\0';
    }
    return( name );
}

extern unsigned long GetARValue( char *str, int max )
/***************************************************/
// get a numeric value from an ar_header
{
    char                save;
    unsigned long       value;

    save = *(str + max);
    *(str + max) = '\0';
    value = strtoul( str, NULL, 10 );
    *(str + max) = save;
    return value;
}

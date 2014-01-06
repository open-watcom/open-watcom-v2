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


#include <string.h>     // for memset
#include <stdarg.h>     // for BRIAddType
#include <assert.h>
#include "b_write.h"
#include "carve.h"
#include "bool.h"

#ifndef EOF
#include <stdio.h>
#endif

#define BRI_write( cli, buf, len )      cli->rtns.write( cli->io_cookie, buf, len )
#define BRI_lseek( cli, off, whe )      cli->rtns.lseek( cli->io_cookie, off, whe )
#define BRI_malloc( cli, size )         cli->rtns.malloc( size )
#define BRI_free( cli, ptr )            cli->rtns.free( ptr )

#define LIST_BLOCK      0x180
#define CHAR_BLOCK      0x180
#define NUM_BUCKETS     0x100

#define BRI_MAX_LINE  0x00007ffful
#define BRI_MAX_COL   0x0000007ful

#define TYPE_BRANCH     8
#define TYPE_FULL( t )  ((t)->type_ids[TYPE_BRANCH-2] != 0)
#define TYPE_BOTTOM( t ) ((t)->next[0]==NULL)

/******************************
    Type Declarations.
******************************/

typedef struct BRI_FileCounter {
    BRI_StringID                filename_id;
    uint_32                     line,column;
    bool                        template;
    struct BRI_FileCounter *    next;
} BRI_FileCounter;

typedef struct BRI_TypeCounter {
    struct BRI_TypeCounter      *next[TYPE_BRANCH];
    BRI_TypeID                  type_ids[TYPE_BRANCH-1];
} BRI_TypeCounter;

typedef struct BRI_HashString {
    struct BRI_HashString *     next;
    BRI_StringID                index;
    unsigned                    offset;
} BRI_HashString;

struct BRI_Handle {
    /* callback functions */
    BRI_Routines        rtns;

    /* Browse header */
    BRI_Header *        hdr;

    /* file io information */
    int                 io_cookie;
    unsigned long       start;

    /* other data */
    BRI_FileCounter *   files;
    BRI_TypeCounter *   types;
    carve_t             carve_types;

    /* string storage */
    BRI_HashString *    table[NUM_BUCKETS];
    char *              char_buf;
    unsigned            buf_size;
    unsigned            buf_top;
    carve_t             carve_string;
};


/*****************************
    Static functions.
*****************************/

static uint_32 const bmask[5] = {
    0x00000000,
    0x000000ff,
    0x0000ffff,
    0x00ffffff,
    0xffffffff,
};


/*  hashString
*/

static int hashString( char const *string, int len )
/**************************************************/
{
    uint_32 *s = (uint_32*) string;
    uint_32 mask;
    uint_32 c;
    uint_32 g;
    uint_32 h;

    h = len;
    c = len;
    if( len > sizeof( uint_32 ) ) {
        do {
            c += *s;
            h = ( h << 4 ) + c;
            g = h & ~0x0ffffff;
            h ^= g;
            h ^= g >> (4+4+4+4+4);
            ++s;
            len -= sizeof( uint_32 );
        } while( len > sizeof( uint_32 ) );
    }
    mask = bmask[ len ];
    c += *s & mask;
    h = ( h << 4 ) + c;
    g = h & ~0x0ffffff;
    h ^= g;
    h ^= g >> (4+4+4+4+4);
    g = h & ~0x0fff;
    h ^= g;
    h ^= g >> (4+4+4);
    h ^= h >> (2+4);
    assert( ( h & ~0x0fff ) == 0 );
    return( (int) (h%NUM_BUCKETS) );
}


/*  insertStringID      -- Insert a string into the string table without
                           replacement.
*/

static BRI_StringID insertStringID( BRI_HANDLE handle, BRI_StringID index,
                            char const *string, uint_32 length )
/************************************************************************/
{
    BRI_StringID        result;
    BRI_HashString *    current;
    BRI_HashString *    prev;
    BRI_HashString *    newstring;
    int                 hash_val;
    int                 comparison;

    hash_val = hashString( string, length );

    prev = NULL;
    current = handle->table[hash_val];
    while( current != NULL ){
        comparison = strcmp( handle->char_buf+current->offset, string );
        if( comparison >= 0 ){
            break;
        }
        prev = current;
        current = current->next;
    }

    if( current == NULL || comparison > 0 ){
        newstring = CarveAlloc( handle->carve_string );
        newstring->next = current;
        newstring->index = index;
        if( prev != NULL ){
            prev->next = newstring;
        } else {
            handle->table[hash_val] = newstring;
        }
        if( length + handle->buf_top > handle->buf_size ){
            int         new_size;
            char *      temp;

            new_size = 2*handle->buf_size;
            if( new_size < length + handle->buf_top ){
                new_size = length + handle->buf_top;
            }
            temp = BRI_malloc( handle, new_size*sizeof(char) );
            memcpy( temp, handle->char_buf, handle->buf_size );
            BRI_free( handle, handle->char_buf );
            handle->char_buf = temp;
            handle->buf_size = new_size;
        }
        newstring->offset = handle->buf_top;
        memcpy( handle->char_buf+handle->buf_top, string,
                length*sizeof(char) );
        handle->buf_top += length;
        result = index;
    } else {
        result = current->index;
    }

    return result;
}


/*  insertTypeID        -- Record a type id as having been seen.
                           Assumes the id was not seen previously.
*/

static void insertTypeID( BRI_HANDLE handle, BRI_TypeID type_id )
/***************************************************************/
{
    BRI_TypeCounter     *current;
    BRI_TypeCounter     *parent;
    BRI_TypeID          temp_id;
    int                 follow;
    int                 i;

    current = handle->types;
    if( current == NULL ){
        current = CarveAlloc( handle->carve_types );
        memset( current->type_ids, 0,
                (TYPE_BRANCH-1)*sizeof(BRI_TypeID) );
        memset( current->next, 0, TYPE_BRANCH*sizeof(BRI_TypeCounter *) );
        current->type_ids[0] = type_id;
        handle->types = current;
        return;
    }
    parent = NULL;
    for(;;){
        if( TYPE_FULL( current ) ){
            // Split the current node
            BRI_TypeCounter     *sibling;
            BRI_TypeCounter     *temp_ctr1, *temp_ctr2;
            BRI_TypeID          promote;
            int                 divider;

            divider = TYPE_BRANCH/2;
            sibling = CarveAlloc( handle->carve_types );
            memset( sibling->next, 0, TYPE_BRANCH*sizeof(BRI_TypeCounter *) );
            memset( sibling->type_ids, 0,
                    (TYPE_BRANCH-1)*sizeof(BRI_TypeID) );
            memcpy( sibling->next, current->next+divider,
                    divider*sizeof( BRI_TypeCounter * ) );
            memcpy( sibling->type_ids, current->type_ids+divider,
                    (divider-1)*sizeof( BRI_TypeID ) );
            promote = current->type_ids[divider-1];
            memset( current->next+divider, 0,
                    divider*sizeof( BRI_TypeCounter * ) );
            memset( current->type_ids+divider-1, 0,
                    divider*sizeof( BRI_TypeID ) );
            if( parent == NULL ){
                temp_ctr1 = CarveAlloc( handle->carve_types );
                memset( temp_ctr1->next, 0,
                        TYPE_BRANCH*sizeof(BRI_TypeCounter *) );
                memset( temp_ctr1->type_ids, 0,
                        (TYPE_BRANCH-1)*sizeof(BRI_TypeID) );
                temp_ctr1->next[0] = current;
                temp_ctr1->next[1] = sibling;
                temp_ctr1->type_ids[0] = promote;
                handle->types = temp_ctr1;
            } else {
                for( i=follow; i<TYPE_BRANCH-1; i++ ){
                    temp_id = parent->type_ids[i];
                    parent->type_ids[i] = promote;
                    promote = temp_id;
                }
                temp_ctr2 = sibling;
                for( i=follow+1; i<TYPE_BRANCH; i++ ){
                    temp_ctr1 = parent->next[i];
                    parent->next[i] = temp_ctr2;
                    temp_ctr2 = temp_ctr1;
                }
            }
            if( type_id > promote ){
                current = sibling;
            }
        }
        if( TYPE_BOTTOM( current ) ){
            break;
        }
        for( follow=0; follow<TYPE_BRANCH-1; follow++ ){
            if( current->type_ids[follow] == BRI_NULL_ID ){
                break;
            } else if( current->type_ids[follow] > type_id ){
                break;
            }
        }
        parent = current;
        current = current->next[follow];
    }

    for( follow=0; follow<TYPE_BRANCH-1; follow++ ){
        if( current->type_ids[follow] == BRI_NULL_ID ){
            break;
        } else if( current->type_ids[follow] > type_id ){
            break;
        }
    }
    for( i=follow; i<TYPE_BRANCH-1; i++ ){
        temp_id = current->type_ids[i];
        current->type_ids[i] = type_id;
        type_id = temp_id;
    }

    return;
}


/******************************
    External functions.
******************************/


/*  BRIBeginWrite       -- Create a browse handle, ready for use.
*/

BRI_HANDLE BRIBeginWrite( BRI_Routines const *rtns, int io_cookie,
                          unsigned long start )
/*****************************************************************/
{
    BRI_HANDLE          result;
    BRI_Header *        hdr;

    result = (BRI_HANDLE) rtns->malloc( sizeof(BRI_Handle) );
    if( result == NULL ) {
        // TODO:  FATAL ERROR
        return (BRI_HANDLE) NULL;
    }

    memset( result, 0, sizeof( *result ) );
    result->rtns = *rtns;
    result->io_cookie = io_cookie;
    result->start = start;
    result->files = NULL;
    result->types = NULL;
    result->carve_types = CarveCreate( sizeof(BRI_TypeCounter), LIST_BLOCK );
    result->char_buf = BRI_malloc( result, CHAR_BLOCK*sizeof(char) );
    result->buf_size = CHAR_BLOCK;
    result->buf_top = 0;
    memset( result->table, 0, NUM_BUCKETS*sizeof(BRI_HashString*) );
    result->carve_string = CarveCreate( sizeof(BRI_HashString), LIST_BLOCK );

    hdr = (BRI_Header *) BRI_malloc( result, sizeof( BRI_Header ) );
    memset( hdr, 0, sizeof(*hdr) );
    hdr->magic = BRI_MAGIC;
    hdr->major_ver = BRI_MAJOR_VER;
    hdr->minor_ver = BRI_MINOR_VER;
    BRI_write( result, hdr, sizeof( *hdr ) );
    result->hdr = hdr;

    return result;
}


/*  BRIEndWrite         -- Finish using a browse handle and destroy it.
*/

void BRIEndWrite( BRI_HANDLE handle )
/***********************************/
{
    int                 out_len;
    // int                 i;

    // Get the current position in the file.
    out_len = BRI_lseek( handle, 0, BRI_SEEK_CUR );

    handle->hdr->file_len = out_len - handle->start;
    BRI_lseek( handle, handle->start, BRI_SEEK_SET );
    BRI_write( handle, handle->hdr, sizeof(*handle->hdr) );

    BRI_free( handle, handle->hdr );

    {
        BRI_FileCounter *current;
        BRI_FileCounter *prev;

        prev = NULL;
        current = handle->files;
        while( current != NULL ){
            prev = current;
            current = current->next;
            BRI_free( handle, prev );
        }
    }
    CarveDestroy( handle->carve_types );
    CarveDestroy( handle->carve_string );
    BRI_free( handle, handle->char_buf );

    BRI_free( handle, handle );
}


/*  BRICreate           -- Create a browse handle, but don't prepare
                           it for use.  Handle can be prepared later
                           by calling BRIOpen.
*/

BRI_HANDLE BRICreate( BRI_Routines const *rtns )
/**********************************************/
{
    BRI_HANDLE          result;

    result = (BRI_HANDLE) rtns->malloc( sizeof(BRI_Handle) );
    if( result == NULL ) {
        // TODO:  FATAL ERROR
        return (BRI_HANDLE) NULL;
    }

    memset( result, 0, sizeof( *result ) );
    result->rtns = *rtns;
    result->io_cookie = 0;
    result->start = 0;
    result->files = NULL;
    result->types = NULL;
    result->carve_types = CarveCreate( sizeof(BRI_TypeCounter), LIST_BLOCK );
    result->char_buf = BRI_malloc( result, CHAR_BLOCK*sizeof(char) );
    result->buf_size = CHAR_BLOCK;
    result->buf_top = 0;
    memset( result->table, 0, NUM_BUCKETS*sizeof(BRI_HashString*) );
    result->carve_string = CarveCreate( sizeof(BRI_HashString), LIST_BLOCK );
    result->hdr = NULL;

    return result;
}


/*  BRIOpen             -- Prepare a previously created browse handle
                           for use.
*/

BRI_HANDLE BRIOpen( BRI_HANDLE handle, BRI_Routines const *rtns,
                    int io_cookie, unsigned long start )
/*****************************************************************/
{
    BRI_Header *        hdr;

    if( handle == NULL ){
        return NULL;
    }

    handle->rtns = *rtns;
    handle->io_cookie = io_cookie;
    handle->start = start;
    handle->files = NULL;

    hdr = (BRI_Header *) BRI_malloc( handle, sizeof( BRI_Header ) );
    memset( hdr, 0, sizeof(*hdr) );
    hdr->magic = BRI_MAGIC;
    hdr->major_ver = BRI_MAJOR_VER;
    hdr->minor_ver = BRI_MINOR_VER;
    BRI_write( handle, hdr, sizeof( *hdr ) );
    handle->hdr = hdr;

    return handle;
}


/*  BRIClose            -- Stop using a browse handle.  The handle can
                           be re-used later by calling BRIOpen.
*/

BRI_HANDLE BRIClose( BRI_HANDLE handle )
/***********************************/
{
    BRI_FileCounter     *current;
    BRI_FileCounter     *prev;
    int                 out_len;

    // Get the current position in the file.
    out_len = BRI_lseek( handle, 0, BRI_SEEK_CUR );

    handle->hdr->file_len = out_len - handle->start;
    BRI_lseek( handle, handle->start, BRI_SEEK_SET );
    BRI_write( handle, handle->hdr, sizeof(*handle->hdr) );

    BRI_free( handle, handle->hdr );
    handle->hdr = NULL;

    prev = NULL;
    current = handle->files;
    while( current != NULL ){
        prev = current;
        current = current->next;
        BRI_free( handle, prev );
    }
    handle->files = NULL;

    return handle;
}


/*   BRIStartFile
*/

void BRIStartFile( BRI_HANDLE handle, BRI_StringID filename )
/*************************************************************/
{
    BRI_RecordType      file_rt = BRI_Rec_File;
    BRI_FileCounter *   new_counter;

    new_counter = (BRI_FileCounter *)
                  BRI_malloc( handle, sizeof( BRI_FileCounter ) );
    new_counter->filename_id = filename;
    new_counter->line = 0;
    new_counter->column = 0;
    new_counter->template = FALSE;
    new_counter->next = handle->files;
    handle->files = new_counter;

    BRI_write( handle, &file_rt, 1 );
    BRI_write( handle, &filename, sizeof(filename) );
    handle->hdr->num_file += 1;
}


/*   BRIEndFile
*/

void BRIEndFile( BRI_HANDLE handle )
/************************************/
{
    BRI_RecordType      fileend_rt = BRI_Rec_FileEnd;
    BRI_FileCounter *   old_counter;

    assert(handle->files != NULL);
    old_counter = handle->files;
    handle->files = handle->files->next;

    BRI_free( handle, old_counter );

    BRI_write( handle, &fileend_rt, BRI_SIZE_RECORDTYPE );
}


/*  BRIStartTemplate
*/

void BRIStartTemplate( BRI_HANDLE handle, BRI_StringID filename )
/***************************************************************/
{
    BRI_RecordType      template_rt = BRI_Rec_Template;
    BRI_FileCounter *   new_counter;

    new_counter = (BRI_FileCounter *)
                  BRI_malloc( handle, sizeof( BRI_FileCounter ) );
    new_counter->filename_id = filename;
    new_counter->line = 0;
    new_counter->column = 0;
    new_counter->template = TRUE;
    new_counter->next = handle->files;
    handle->files = new_counter;

    BRI_write( handle, &template_rt, 1 );
    BRI_write( handle, &filename, sizeof( filename ) );
    handle->hdr->num_template += 1;
}


/*  BRIEndTemplate
*/

void BRIEndTemplate( BRI_HANDLE handle )
/**************************************/
{
    BRI_RecordType      fileend_rt = BRI_Rec_FileEnd;
    BRI_RecordType      template_rt = BRI_Rec_TemplateEnd;
    BRI_FileCounter *   current;
    BRI_FileCounter *   prev;
    bool                template = FALSE;

    for( current = handle->files; current != NULL && !template; ){
        template = current->template;
        prev = current;
        current = current->next;

        if( template ) {
            BRI_write( handle, &template_rt, 1 );
        } else {
            BRI_write( handle, &fileend_rt, 1 );
        }
        BRI_free( handle, prev );
    }
    handle->files = current;
}


/*  BRIStartScope
*/

BRI_ScopeID BRIStartScope( BRI_HANDLE handle, BRI_ScopeID index,
                           BRI_ScopeType flags, uint_32 owner )
/**************************************************************/
{
    BRI_RecordType      scope_rt = BRI_Rec_Scope;

    BRI_write( handle, &scope_rt, BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &index, sizeof(BRI_ScopeID) );
    BRI_write( handle, &flags, BRI_SIZE_SCOPETYPE );
    BRI_write( handle, &owner, sizeof(uint_32) );

    handle->hdr->num_scope += 1;

    return index;
}


/*  BRIStartFnScope
*/

BRI_ScopeID BRIStartFnScope( BRI_HANDLE handle, BRI_ScopeID index,
                             BRI_StringID name_id, BRI_TypeID type_id )
/*********************************************************************/
{
    BRI_RecordType      scope_rt = BRI_Rec_Scope;
    BRI_ScopeType       flags = BRI_ST_Function;

    BRI_write( handle, &scope_rt,  BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &index, sizeof(BRI_ScopeID) );
    BRI_write( handle, &flags, BRI_SIZE_SCOPETYPE );
    BRI_write( handle, &name_id, sizeof(BRI_StringID) );
    BRI_write( handle, &type_id, sizeof(BRI_TypeID) );

    handle->hdr->num_scope += 1;

    return index;
}


/*  BRIEndScope
*/

void BRIEndScope( BRI_HANDLE handle )
/***********************************/
{
    BRI_RecordType      scope_end_rt = BRI_Rec_ScopeEnd;

    BRI_write( handle, &scope_end_rt, BRI_SIZE_RECORDTYPE );
}


/*  BRIAddString
*/

BRI_StringID BRIAddString( BRI_HANDLE handle, BRI_StringID index,
                   const char *string )
/*****************************************************************/
{
    BRI_RecordType      string_rt = BRI_Rec_String;
    BRI_StringID        result;
    uint_32             string_length;

    if( string == NULL ){
        result = (BRI_StringID) 0;
    } else {
        string_length = strlen(string) + 1;
        result = insertStringID( handle, index, string, string_length );
        if( result == index ){
            BRI_write( handle, &string_rt,  BRI_SIZE_RECORDTYPE );
            BRI_write( handle, &index, sizeof(index) );
            BRI_write( handle, &string_length, sizeof(string_length) );
            BRI_write( handle, string, string_length * sizeof( char ) );
            handle->hdr->num_string += 1;
        }
    }

    return result;
}


/*  BRIAddDefinition
*/

void BRIAddDefinition( BRI_HANDLE handle, BRI_StringID file,
                      uint_32 line, uint_32 col,
                      BRI_SymbolID symbol )
/***********************************************************/
{
    BRI_RecordType      defn_rt  = BRI_Rec_Definition;

    BRI_write( handle, &defn_rt,  BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &col, sizeof(uint_32) );
    BRI_write( handle, &line, sizeof(uint_32) );
    BRI_write( handle, &file, sizeof(BRI_StringID) );
    BRI_write( handle, &symbol, sizeof(symbol) );
    handle->hdr->num_definition += 1;
}


/*  BRIAddReference
*/

void BRIAddReference( BRI_HANDLE handle, BRI_StringID file,
                      uint_32 line, uint_32 col,
                      uint_32 target, BRI_ReferenceType ref_type )
/*******************************************************************/
{
    BRI_RecordType      delta_rt = BRI_Rec_Delta;
    BRI_RecordType      usage_rt   = BRI_Rec_Usage;

    int         line_sign;
    int         col_sign;
    uint_32     delta_line;
    uint_32     delta_col;
    int_8       col_incr;
    int_16      line_incr;

    if( file != BRI_NO_CHANGE ){
        BRI_FileCounter *       current;

        current = handle->files;
        while( current != NULL && current->filename_id != file ){
            if( current->template ){
                current = NULL;
                break;
            }
            current = current->next;
        }
        if( current != NULL ){
            while( handle->files != current ){
                BRIEndFile( handle );
            }
        } else {
            BRIStartFile( handle, file );
        }
    }

    if( line == BRI_NO_CHANGE ){
        delta_line = 0;
        line_sign = 1;
    } else {
        line_sign = (line >= handle->files->line) ? 1 : -1;
        if( line_sign == 1 ){
            delta_line = line - handle->files->line;
        } else {
            delta_line = handle->files->line - line;
        }
        handle->files->line = line;
    }
    if( col == BRI_NO_CHANGE ){
        delta_col = 0;
        col_sign = 1;
    } else {
        col_sign  = (col  >= handle->files->column) ? 1 : -1;
        if( col_sign == 1 ){
            delta_col = col - handle->files->column;
        } else {
            delta_col = handle->files->column - col;
        }
        handle->files->column = col;
    }

    while( delta_line > BRI_MAX_LINE || delta_col > BRI_MAX_COL ){
        if( delta_line > BRI_MAX_LINE ){
            line_incr = (int_16) ( BRI_MAX_LINE * line_sign );
            delta_line -= BRI_MAX_LINE;
        } else {
            line_incr = (int_16) ( delta_line * line_sign);
            delta_line = 0;
        }
        if( delta_col > BRI_MAX_COL ){
            col_incr = (int_8) ( BRI_MAX_COL * col_sign );
            delta_col -= BRI_MAX_COL;
        } else {
            col_incr = (int_8) ( delta_col * col_sign );
            delta_col = 0;
        }
        BRI_write( handle, &delta_rt,  BRI_SIZE_RECORDTYPE );
        BRI_write( handle, &col_incr, sizeof(col_incr) );
        BRI_write( handle, &line_incr, sizeof(line_incr) );
        handle->hdr->num_delta += 1;
    }

    line_incr = (int_16) ( delta_line * line_sign );
    col_incr  = (int_8)  ( delta_col * col_sign );
    BRI_write( handle, &usage_rt,  BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &ref_type, BRI_SIZE_REFERENCETYPE );
    BRI_write( handle, &col_incr, sizeof(col_incr) );
    BRI_write( handle, &line_incr, sizeof(line_incr) );
    BRI_write( handle, &target, sizeof(target) );
    handle->hdr->num_usage += 1;
}


/*  BRIAddSymbol
*/

BRI_SymbolID BRIAddSymbol( BRI_HANDLE handle,
                             BRI_SymbolID sym_id,
                             BRI_StringID name_id,
                             BRI_TypeID type_id,
                             BRI_SymbolAttributes sym_type,
                             BRI_SymbolAttributes access )
/*********************************************************/
{
    BRI_RecordType              decl_rt = BRI_Rec_Declaration;
    BRI_SymbolAttributes        attribs = sym_type | access;

    BRI_write( handle, &decl_rt,  BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &sym_id, sizeof(sym_id) );
    BRI_write( handle, &attribs, BRI_SIZE_SYMBOLATTRIBUTES );
    BRI_write( handle, &name_id, sizeof(name_id) );
    BRI_write( handle, &type_id, sizeof(type_id) );

    handle->hdr->num_declaration += 1;

    return sym_id;
}


/*  BRITypeAlreadySeen
*/

int BRITypeAlreadySeen( BRI_HANDLE handle,
                         BRI_TypeID type_id )
/*******************************************/
{
    BRI_TypeCounter *   current;
    BRI_TypeID          curr_id;
    int                 follow;

    current = handle->types;
    while( current != NULL ){
        for( follow=0; follow<TYPE_BRANCH-1; follow++ ){
            curr_id = current->type_ids[follow];
            if( curr_id == BRI_NULL_ID ){
                break;
            } else if( curr_id == type_id ){
                return TRUE;
            } else if( curr_id > type_id ){
                break;
            }
        }
        current = current->next[follow];
    }

    return FALSE;
}


/*  BRIAddType
*/

BRI_TypeID BRIAddType( BRI_HANDLE handle,
                       BRI_TypeID type_id,
                       BRI_TypeCode code,
                       int num_ops,
                       ... )
/****************************************/
{
    BRI_RecordType      type_rt = BRI_Rec_Type;
    va_list             parms;
    uint_32             op;
    int                 i;

    insertTypeID( handle, type_id );

    BRI_write( handle, &type_rt, BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &type_id, sizeof(type_id) );
    BRI_write( handle, &code, BRI_SIZE_TYPECODE );
    BRI_write( handle, &num_ops, sizeof(uint_32) );
    va_start( parms, num_ops );
    for( i=0; i<num_ops; i++ ){
        op = va_arg( parms, uint_32 );
        BRI_write( handle, &op, sizeof( uint_32 ) );
    }
    va_end( parms );
    handle->hdr->num_type += 1;

    return type_id;
}


/*  BRIVAddType
*/

BRI_TypeID BRIVAddType( BRI_HANDLE handle,
                        BRI_TypeID type_id,
                        BRI_TypeCode code,
                        int num_ops,
                        uint_32 *ops )
/****************************************/
{
    BRI_RecordType      type_rt = BRI_Rec_Type;
    int                 i;

    insertTypeID( handle, type_id );

    BRI_write( handle, &type_rt, BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &type_id, sizeof(type_id) );
    BRI_write( handle, &code, BRI_SIZE_TYPECODE );
    BRI_write( handle, &num_ops, sizeof(uint_32) );
    for( i=0; i<num_ops; i++ ){
        BRI_write( handle, ops+i, sizeof( uint_32 ) );
    }
    handle->hdr->num_type += 1;

    return type_id;
}


/*  BRIAddGuard
*/

void BRIAddGuard( BRI_HANDLE handle, BRI_GuardTypes type,
                  BRI_StringID string_id, uint_32 num_params,
                  uint_32 length, uint_8 const * defn )
/***********************************************************/
{
    BRI_RecordType      guard_rt = BRI_Rec_Guard;

    BRI_write( handle, &guard_rt, BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &type, BRI_SIZE_GUARDTYPES );
    BRI_write( handle, &string_id, sizeof(BRI_StringID) );
    BRI_write( handle, &num_params, sizeof(uint_32) );
    BRI_write( handle, &length, sizeof(uint_32) );
    BRI_write( handle, defn, length );
    handle->hdr->num_guard += 1;
}


/*  BRISetPos
*/

void BRISetPos( BRI_HANDLE handle, uint_32 line, uint_32 col )
/************************************************************/
{
    BRI_RecordType      delta_rt = BRI_Rec_Delta;
    int                 line_sign;
    int                 col_sign;
    uint_32             delta_line;
    uint_32             delta_col;
    int_8               col_incr;
    int_16              line_incr;

    if( handle->files == NULL ){
        return;
    }

    if( line == BRI_NO_CHANGE ){
        delta_line = 0;
        line_sign = 1;
    } else {
        line_sign = (line >= handle->files->line) ? 1 : -1;
        if( line_sign == 1 ){
            delta_line = line - handle->files->line;
        } else {
            delta_line = handle->files->line - line;
        }
        handle->files->line = line;
    }
    if( col == BRI_NO_CHANGE ){
        delta_col = 0;
        col_sign = 1;
    } else {
        col_sign  = (col  >= handle->files->column) ? 1 : -1;
        if( col_sign == 1 ){
            delta_col = col - handle->files->column;
        } else {
            delta_col = handle->files->column - col;
        }
        handle->files->column = col;
    }

    while( delta_line > BRI_MAX_LINE || delta_col > BRI_MAX_COL ){
        if( delta_line > BRI_MAX_LINE ){
            line_incr = (int_16) ( BRI_MAX_LINE * line_sign );
            delta_line -= BRI_MAX_LINE;
        } else {
            line_incr = (int_16) ( delta_line * line_sign);
            delta_line = 0;
        }
        if( delta_col > BRI_MAX_COL ){
            col_incr = (int_8) ( BRI_MAX_COL * col_sign );
            delta_col -= BRI_MAX_COL;
        } else {
            col_incr = (int_8) ( delta_col * col_sign );
            delta_col = 0;
        }
        BRI_write( handle, &delta_rt,  BRI_SIZE_RECORDTYPE );
        BRI_write( handle, &col_incr, sizeof(col_incr) );
        BRI_write( handle, &line_incr, sizeof(line_incr) );
        handle->hdr->num_delta += 1;
    }

    line_incr = (int_16) ( delta_line * line_sign );
    col_incr  = (int_8)  ( delta_col * col_sign );

    BRI_write( handle, &delta_rt,  BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &col_incr, sizeof(col_incr) );
    BRI_write( handle, &line_incr, sizeof(line_incr) );
    handle->hdr->num_delta += 1;

    return;
}


/*  BRIAddPCHInclude
*/

void BRIAddPCHInclude( BRI_HANDLE handle, BRI_StringID filename_id )
/******************************************************************/
{
    BRI_RecordType      PCH_rt = BRI_Rec_PCHInclude;

    BRI_write( handle, &PCH_rt, BRI_SIZE_RECORDTYPE );
    BRI_write( handle, &filename_id, sizeof(BRI_StringID) );
    handle->hdr->num_pch += 1;
}


/*  BRIBuildHandle
*/

BRI_HANDLE BRIBuildHandle( BRI_HANDLE handle, BRI_PCHRtns const *rtns,
                     void * io_cookie )
/***************************************************************/
{
    BRI_Header          header;
    BRI_Header          *temp;
    int                 file_len;
    int                 file_pos;
    int                 record_flag;
    int                 delta;

    if( handle == NULL ){
        return NULL;
    }

    temp = rtns->read( io_cookie, sizeof( header ) );
    memcpy( &header, temp, sizeof( header ) );
    if( header.magic != BRI_MAGIC ){
        return NULL;
    }
    file_len = header.file_len;
    file_pos = sizeof( header );

    while( file_pos < file_len ){
        record_flag = rtns->read1( io_cookie );
        if( record_flag == EOF ){
            return NULL;
        }
        file_pos ++;
        switch( record_flag ){
            case BRI_Rec_Declaration:
                delta = BRI_SIZE_DECLARATION;
                rtns->read( io_cookie, delta );
            break;

            case BRI_Rec_File:
                delta = BRI_SIZE_FILE;
                rtns->read( io_cookie, delta );
            break;

            case BRI_Rec_FileEnd:
                // nothing; no data associated with a FileEnd record
            break;

            case BRI_Rec_Template:
                delta = BRI_SIZE_TEMPLATE;
                rtns->read( io_cookie, delta );
            break;

            case BRI_Rec_TemplateEnd:
                // nothing; no data associated with a TemplateEnd record
            break;

            case BRI_Rec_Scope:
                delta = BRI_MIN_SIZE_SCOPE;
                {
                    BRI_ScopeType       flag;

                    rtns->read( io_cookie, sizeof(BRI_ScopeID) );
                    flag = rtns->read1( io_cookie );
                    if( flag == BRI_ST_Function ){
                        delta += sizeof(BRI_StringID);
                        rtns->read( io_cookie,
                              sizeof(BRI_StringID) + sizeof(BRI_TypeID) );
                    } else {
                        rtns->read( io_cookie, sizeof(BRI_TypeID) );
                    }
                }
            break;

            case BRI_Rec_ScopeEnd:
                // nothing; no data associated with a ScopeEnd record
            break;

            case BRI_Rec_Delta:
                delta = BRI_SIZE_DELTA;
                rtns->read( io_cookie, delta );
            break;

            case BRI_Rec_Usage:
                delta = BRI_SIZE_USAGE;
                rtns->read( io_cookie, delta );
            break;

            case BRI_Rec_String:
                delta = BRI_MIN_SIZE_STRING;
                {
                    BRI_StringID        index;
                    uint_32             string_length;
                    char                *buf;

                    index = rtns->read4( io_cookie );
                    string_length = rtns->read4( io_cookie );
                    delta += string_length;
                    buf = rtns->read( io_cookie, string_length );
                    insertStringID( handle, index, buf, string_length );
                }
            break;

            case BRI_Rec_Type:
                delta = BRI_MIN_SIZE_TYPE;
                {
                    BRI_TypeID          index;
                    uint_32             num_ops;

                    index = rtns->read4( io_cookie );
                    rtns->read( io_cookie, BRI_SIZE_TYPECODE );
                    num_ops = rtns->read4( io_cookie );
                    delta += num_ops * sizeof(uint_32);
                    rtns->read( io_cookie, num_ops * sizeof(uint_32) );

                    if( !BRITypeAlreadySeen( handle, index ) ){
                        insertTypeID( handle, index );
                    }
                }
            break;

            case BRI_Rec_Guard:
                delta = BRI_MIN_SIZE_GUARD;
                rtns->read( io_cookie, delta - sizeof(uint_32) );
                {
                    uint_32     defn_len;

                    defn_len = rtns->read4( io_cookie );
                    delta += defn_len;
                    rtns->read( io_cookie, defn_len );
                }
            break;

            case BRI_Rec_Definition:
                delta = BRI_SIZE_DEFINITION;
                rtns->read( io_cookie, delta );
            break;

            case BRI_Rec_PCHInclude:
                delta = BRI_SIZE_PCHINCLUDE;
                rtns->read( io_cookie, delta );
            break;

            default:
                return NULL;
        }
        file_pos += delta;
    }

    return handle;
}

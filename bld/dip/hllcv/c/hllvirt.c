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
* Description:  HLL/CV virtual memory support.
*
****************************************************************************/


#include <string.h>
#include <limits.h>
#include "bool.h"
#include "hllinfo.h"


#define PAGE_BITS       12
#define DIR_BITS        4
#define PAGE_SIZE       (1U<<PAGE_BITS)
#define DIR_SIZE        (1UL<<DIR_BITS)

#define GET_DIR( v )    ((v) >> (DIR_BITS+PAGE_BITS))
#define GET_PAGE( v )   (((v) >> PAGE_BITS) & (DIR_SIZE-1))
#define GET_OFFSET( v ) ((v) & (PAGE_SIZE-1))

typedef struct vm_dtor {
    struct vm_dtor      *next;
    void                *user;
    void                ( *dtor )( imp_image_handle *, void * );
} vm_dtor;

typedef struct {
    vm_dtor             *first_dtor;
    unsigned_16         time_stamp;
    //unsigned_16         len;
    unsigned_32         len;
    unsigned_8          data[1];        /* variable sized */
} loaded_block;

typedef struct virt_page {
    loaded_block        *block;
    unsigned_32         offset;
    //unsigned_16         offset;
} virt_page;


static unsigned         TimeStamp;

dip_status VMInit( imp_image_handle *ii, unsigned long size )
{
    ii->vm_dir_num = BLOCK_FACTOR( size, DIR_SIZE*PAGE_SIZE );
    ii->virt = DCAllocZ( ii->vm_dir_num * sizeof( ii->virt ) );
    if( ii->virt == NULL ) {
        DCStatus( DS_ERR|DS_NO_MEM );
        return( DS_ERR|DS_NO_MEM );
    }
    return( DS_OK );
}

void VMFini( imp_image_handle *ii )
{
    int         i;
    int         j;
    virt_page   *pg;

    if( ii->virt != NULL ) {
        for( i = ii->vm_dir_num-1; i >= 0; --i ) {
            if( ii->virt[i] != NULL ) {
                for( j = DIR_SIZE-1; j >= 0; --j ) {
                    pg = ii->virt[i][j];
                    if( pg != NULL && pg->offset == 0 ) {
                        DCFree( pg );
                    }
                }
                DCFree( ii->virt[i] );
            }
        }
        DCFree( ii->virt );
        ii->virt = NULL;
    }
}

/*
 * Adds a destructor to a block.
 */
dip_status VMAddDtor( imp_image_handle *ii, virt_mem start,
                      void ( *dtor )( imp_image_handle *ii, void *user ),
                      void *user )
{
    unsigned            dir_idx;
    unsigned            pg_idx;
    virt_page           *pg;

    /* locate the block. */
    dir_idx = GET_DIR( start );
    if( ii->virt[dir_idx] != NULL ) {
        pg_idx = GET_PAGE( start );
        pg = ii->virt[dir_idx][pg_idx];
        if( pg != NULL ) {
            /* create a new dtor node and link it in */
            vm_dtor *node = DCAlloc( sizeof( *node ) );
            if( node != NULL ) {
                node->user = user;
                node->dtor = dtor;
                node->next = pg->block->first_dtor;
                pg->block->first_dtor = node;
                return( DS_OK );
            }
            return( DS_ERR | DS_NO_MEM );
        }
    }

    return( DS_ERR | DS_FAIL );
}

/*
 * Check for destructors and free the block.
 */
static void vmFreeBlock( imp_image_handle *ii, virt_page *pg )
{
    /* check for destructors */
    while ( pg->block->first_dtor ) {
        struct vm_dtor *cur = pg->block->first_dtor;
        pg->block->first_dtor = cur->next;
        cur->dtor( ii, cur->user );
        DCFree( cur );
    }

    /* free */
    DCFree( pg );
}

static unsigned KillPages( imp_image_handle *ii, unsigned i, unsigned j )
{
    unsigned            idx;
    unsigned            num_pages;
    virt_page           *pg;

    pg = ii->virt[i][j];
    num_pages = pg->block->len / PAGE_SIZE;
    for( idx = 0; idx < num_pages; ++idx ) {
        if( j >= DIR_SIZE ) {
            ++i;
            j = 0;
        }
        ii->virt[i][j] = NULL;
        ++j;
    }
    vmFreeBlock( ii, pg );
    return( num_pages * PAGE_SIZE );
}

static bool InitPageDir( imp_image_handle *ii, unsigned dir_idx )
{
    ii->virt[dir_idx] = DCAllocZ( sizeof( virt_page * ) * DIR_SIZE );
    if( ii->virt[dir_idx] == NULL ) {
        DCStatus( DS_ERR | DS_NO_MEM );
        return( FALSE );
    }
    return( TRUE );
}

void *VMBlock( imp_image_handle *ii, virt_mem start, unsigned len )
{
    unsigned            dir_idx;
    unsigned            pg_idx;
    unsigned            tmp_idx;
    int                 i;
    int                 j;
    unsigned            num_pages;
    virt_mem            pg_start;
    virt_page           *pg;
    virt_page           *zero;
    loaded_block        *block;

    dir_idx = GET_DIR( start );
    if( ii->virt[dir_idx] == NULL ) {
        if( !InitPageDir( ii, dir_idx ) ) return( NULL );
    }
    pg_idx = GET_PAGE( start );
    len += start % PAGE_SIZE;
    pg_start = start & ~(virt_mem)(PAGE_SIZE - 1);
    pg = ii->virt[dir_idx][pg_idx];
    if( pg == NULL || (pg->block->len - pg->offset) < len ) {
        /* unloaded previously loaded block */
        if( pg != NULL ) {
            tmp_idx = dir_idx;
            /* find first page of the block */
            i = pg_idx;
            for( ;; ) {
                ii->virt[tmp_idx][i] = NULL;
                if( pg->offset == 0 ) break;
                if( i == 0 ) {
                    --tmp_idx;
                    i = DIR_SIZE;
                }
                --i;
                --pg;
            }
            vmFreeBlock( ii, pg );
        }
        num_pages = BLOCK_FACTOR( len, PAGE_SIZE );
        pg = DCAlloc( num_pages * (sizeof( *pg ) + PAGE_SIZE)
                    + sizeof( loaded_block ) - 1 );
        if( pg == NULL ) {
            DCStatus( DS_ERR|DS_NO_MEM );
            return( NULL );
        }
        /* set up new page table entries */
        block = (loaded_block *)&pg[num_pages];
        tmp_idx = dir_idx;
        for( j = pg_idx, i = 0; i < num_pages; ++j, ++i ) {
            pg[i].block = block;
            pg[i].offset = i * PAGE_SIZE;
            if( j >= DIR_SIZE ) {
                ++tmp_idx;
                j = 0;
            }
            if( ii->virt[tmp_idx] == NULL ) {
                if( !InitPageDir( ii, tmp_idx ) ) {
                    /* unwind the setup already done */
                    num_pages = i;
                    for( i = 0; i < num_pages; ++i, ++pg_idx ) {
                        if( pg_idx >= DIR_SIZE ) {
                            ++dir_idx;
                            pg_idx = 0;
                        }
                        ii->virt[dir_idx][pg_idx] = NULL;
                    }
                    DCFree( pg );
                    return( NULL );
                }
            }
            if( ii->virt[tmp_idx][j] != NULL ) {
                /*
                    We just ran into another allocated block, so we have
                    to kill all the pages mapped in by it. We know that
                    if the page pointer is non-NULL, it will be offset==0
                    since KillPages will clean out the others.
                */
                KillPages( ii, tmp_idx, j );
            }
            ii->virt[tmp_idx][j] = &pg[i];
        }
        /* read in new block */
        len = num_pages * PAGE_SIZE;
        block->len = len;
        block->first_dtor = NULL;
        pg_start += ii->bias;
        if( DCSeek( ii->sym_file, pg_start, DIG_ORG ) != pg_start ) {
            DCStatus( DS_ERR | DS_FSEEK_FAILED );
            return( NULL );
        }
        /* last block might be a short read */
        if( DCRead( ii->sym_file, pg->block->data, len ) == (unsigned)-1 ) {
            DCStatus( DS_ERR | DS_FREAD_FAILED );
            return( NULL );
        }
        pg = ii->virt[dir_idx][pg_idx];
    }
    ++TimeStamp;
    if( TimeStamp == 0 ) {
        /* deal with wrap-around */
        for( ii = ImageList; ii != NULL; ii = ii->next_image ) {
            if( ii->virt != NULL ) {
                for( i = ii->vm_dir_num-1; i >= 0; --i ) {
                    if( ii->virt[i] != NULL ) {
                        for( j = DIR_SIZE-1; j >= 0; --j ) {
                            zero = ii->virt[i][j];
                            if( zero != NULL ) {
                                zero->block->time_stamp = 0;
                            }
                        }
                    }
                }
            }
        }
        ++TimeStamp;
    }
    pg->block->time_stamp = TimeStamp;
    return( &pg->block->data[ (start & (PAGE_SIZE - 1)) + pg->offset ] );
}

/*
 * Get a 8-bit value.
 */
bool VMGetU8( imp_image_handle *ii, virt_mem start, unsigned_8 *valp )
{
    unsigned_8 *ptr = VMBlock( ii, start, sizeof(*valp) );
    if( ptr ) {
        *valp = *ptr;
        return( TRUE );
    }
    return( FALSE );
}

/*
 * Get a 16-bit value.
 */
bool VMGetU16( imp_image_handle *ii, virt_mem start, unsigned_16 *valp )
{
    unsigned_16 *ptr = VMBlock( ii, start, sizeof(*valp) );
    if( ptr ) {
        *valp = *ptr;
        return( TRUE );
    }
    return( FALSE );
}

/*
 * Get a 32-bit value.
 */
bool VMGetU32( imp_image_handle *ii, virt_mem start, unsigned_32 *valp )
{
    unsigned_32 *ptr = VMBlock( ii, start, sizeof(*valp) );
    if( ptr ) {
        *valp = *ptr;
        return( TRUE );
    }
    return( FALSE );
}

/*
 * Gets a symbol scope table record.
 * Returns pointer to a hll_ssr_common structure on success.
 */
void *VMRecord( imp_image_handle *ii, virt_mem rec_off, virt_mem *next_rec, unsigned_16 *sizep )
{
    unsigned_16 size = 0;
    unsigned_8 *p;

    p = VMBlock( ii, rec_off, 2 );
    if( p != NULL ) {
        size = *p;
        if( size & 0x80 ) {
            size = (size & 0x7f) << 8;
            size |= p[1];
            rec_off++;
        }
        rec_off++;
        p = VMBlock( ii, rec_off, size );
    }
    if( sizep ) {
        *sizep = size;
    }
    if( next_rec ) {
        *next_rec = rec_off + size;
    }
    return( p );
}

/*
 * Get a block within a section.
 */
void *VMSsBlock( imp_image_handle *ii, hll_dir_entry *hde, unsigned_32 start, unsigned len )
{
    if(     start < hde->cb
       &&   start + len <= hde->cb ) {
        return( VMBlock( ii, hde->lfo + start, len ) );
    }
    return( NULL );
}

/*
 * Get a 8-bit value.
 */
bool VMSsGetU8( imp_image_handle *ii, hll_dir_entry *hde, unsigned_32 start, unsigned_8 *valp )
{
    if( start < hde->cb ) {
        unsigned_8 *ptr = VMBlock( ii, hde->lfo + start, sizeof(*valp) );
        if( ptr ) {
            *valp = *ptr;
            return( TRUE );
        }
    }
    return( FALSE );
}

/*
 * Get a 16-bit value.
 */
bool VMSsGetU16( imp_image_handle *ii, hll_dir_entry *hde, unsigned_32 start, unsigned_16 *valp )
{
    if( start <= hde->cb - sizeof(*valp) ) {
        unsigned_16 *ptr = VMBlock( ii, hde->lfo + start, sizeof(*valp) );
        if( ptr ) {
            *valp = *ptr;
            return( TRUE );
        }
    }
    return( FALSE );
}

/*
 * Get a 32-bit value.
 */
bool VMSsGetU32( imp_image_handle *ii, hll_dir_entry *hde, unsigned_32 start, unsigned_32 *valp )
{
    if( start <= hde->cb - sizeof(*valp) ) {
        unsigned_32 *ptr = VMBlock( ii, hde->lfo + start, sizeof(*valp) );
        if( ptr ) {
            *valp = *ptr;
            return( TRUE );
        }
    }
    return( FALSE );
}

/*
 * Gets a symbol scope table record.
 * Returns pointer to a hll_ssr_common structure on success.
 */
void *VMSsRecord( imp_image_handle *ii, hll_dir_entry *hde, unsigned_32 rec_off,
                  unsigned_32 *next_rec, unsigned_16 *sizep )
{
    unsigned_16 size = 0;
    unsigned_8 *p = NULL;

    if( rec_off + 2 <= hde->cb) {
        p = VMBlock( ii, hde->lfo + rec_off, 2 );
        if( p != NULL ) {
            size = *p;
            if( size & 0x80 ) {
                size = (size & 0x7f) << 8;
                size |= p[1];
                rec_off++;
            }
            rec_off++;
            if( rec_off + size <= hde->cb ) {
                p = VMBlock( ii, hde->lfo + rec_off, size );
            }
        }
    }
    if( sizep ) {
        *sizep = size;
    }
    if( next_rec ) {
        *next_rec = rec_off + size;
    }
    return( p );
}


unsigned VMShrink()
{
    int                 i;
    int                 j;
    unsigned            kill_time;
    unsigned            kill_i;
    unsigned            kill_j;
    virt_page           *pg;
    imp_image_handle    *ii;

    kill_time = UINT_MAX;
    kill_i = 0;
    kill_j = 0;
    for( ii = ImageList; ii != NULL; ii = ii->next_image ) {
        if( ii->virt != NULL ) {
            for( i = ii->vm_dir_num-1; i >= 0; --i ) {
                if( ii->virt[i] != NULL ) {
                    for( j = DIR_SIZE-1; j >= 0; --j ) {
                        pg = ii->virt[i][j];
                        if( pg != NULL && pg->offset == 0 ) {
                            if( kill_time > pg->block->time_stamp ) {
                                kill_time = pg->block->time_stamp;
                                kill_i = i;
                                kill_j = j;
                            }
                        }
                    }
                }
            }
        }
    }
    if( kill_time == UINT_MAX ) return( 0 );
    return( KillPages( ii, kill_i, kill_j ) );
}


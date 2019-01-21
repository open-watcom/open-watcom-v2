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


#include "hllinfo.h"
#include <limits.h>
#include "bool.h"

#define VM_PAGE_BITS       12
#define DIR_BITS        4
#define VM_PAGE_SIZE     (1U<<VM_PAGE_BITS)
#define DIR_SIZE        (1UL<<DIR_BITS)

#define GET_DIR( v )    ((v) >> (DIR_BITS+VM_PAGE_BITS))
#define GET_PAGE( v )   (((v) >> VM_PAGE_BITS) & (DIR_SIZE-1))
#define GET_OFFSET( v ) ((v) & (VM_PAGE_SIZE-1))

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

dip_status VMInit( imp_image_handle *iih, unsigned long size )
{
    iih->vm_dir_num = BLOCK_FACTOR( size, DIR_SIZE * VM_PAGE_SIZE );
    iih->virt = DCAllocZ( iih->vm_dir_num * sizeof( iih->virt ) );
    if( iih->virt == NULL ) {
        DCStatus( DS_ERR | DS_NO_MEM );
        return( DS_ERR | DS_NO_MEM );
    }
    return( DS_OK );
}

void VMFini( imp_image_handle *iih )
{
    int         i;
    int         j;
    virt_page   *pg;

    if( iih->virt != NULL ) {
        for( i = iih->vm_dir_num-1; i >= 0; --i ) {
            if( iih->virt[i] != NULL ) {
                for( j = DIR_SIZE-1; j >= 0; --j ) {
                    pg = iih->virt[i][j];
                    if( pg != NULL && pg->offset == 0 ) {
                        DCFree( pg );
                    }
                }
                DCFree( iih->virt[i] );
            }
        }
        DCFree( iih->virt );
        iih->virt = NULL;
    }
}

/*
 * Adds a destructor to a block.
 */
dip_status VMAddDtor( imp_image_handle *iih, virt_mem start,
                      void ( *dtor )( imp_image_handle *iih, void *user ),
                      void *user )
{
    unsigned            dir_idx;
    unsigned            pg_idx;
    virt_page           *pg;

    /* locate the block. */
    dir_idx = GET_DIR( start );
    if( iih->virt[dir_idx] != NULL ) {
        pg_idx = GET_PAGE( start );
        pg = iih->virt[dir_idx][pg_idx];
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
static void vmFreeBlock( imp_image_handle *iih, virt_page *pg )
{
    /* check for destructors */
    while( pg->block->first_dtor ) {
        struct vm_dtor *cur = pg->block->first_dtor;
        pg->block->first_dtor = cur->next;
        cur->dtor( iih, cur->user );
        DCFree( cur );
    }

    /* free */
    DCFree( pg );
}

static unsigned KillPages( imp_image_handle *iih, unsigned i, unsigned j )
{
    unsigned            idx;
    unsigned            num_pages;
    virt_page           *pg;

    pg = iih->virt[i][j];
    num_pages = pg->block->len / VM_PAGE_SIZE;
    for( idx = 0; idx < num_pages; ++idx ) {
        if( j >= DIR_SIZE ) {
            ++i;
            j = 0;
        }
        iih->virt[i][j] = NULL;
        ++j;
    }
    vmFreeBlock( iih, pg );
    return( num_pages * VM_PAGE_SIZE );
}

static bool InitPageDir( imp_image_handle *iih, unsigned dir_idx )
{
    iih->virt[dir_idx] = DCAllocZ( sizeof( virt_page * ) * DIR_SIZE );
    if( iih->virt[dir_idx] == NULL ) {
        DCStatus( DS_ERR | DS_NO_MEM );
        return( false );
    }
    return( true );
}

void *VMBlock( imp_image_handle *iih, virt_mem start, size_t len )
{
    unsigned            dir_idx;
    unsigned            pg_idx;
    unsigned            tmp_idx;
    int                 i;
    int                 j;
    unsigned            num_pages;
    unsigned            pageno;
    virt_mem            pg_start;
    virt_page           *pg;
    virt_page           *zero;
    loaded_block        *block;

    dir_idx = GET_DIR( start );
    if( iih->virt[dir_idx] == NULL ) {
        if( !InitPageDir( iih, dir_idx ) ) {
            return( NULL );
        }
    }
    pg_idx = GET_PAGE( start );
    len += start % VM_PAGE_SIZE;
    pg_start = start & ~(virt_mem)(VM_PAGE_SIZE - 1);
    pg = iih->virt[dir_idx][pg_idx];
    if( pg == NULL || (pg->block->len - pg->offset) < len ) {
        /* unloaded previously loaded block */
        if( pg != NULL ) {
            tmp_idx = dir_idx;
            /* find first page of the block */
            i = pg_idx;
            for( ;; ) {
                iih->virt[tmp_idx][i] = NULL;
                if( pg->offset == 0 )
                    break;
                if( i == 0 ) {
                    --tmp_idx;
                    i = DIR_SIZE;
                }
                --i;
                --pg;
            }
            vmFreeBlock( iih, pg );
        }
        num_pages = BLOCK_FACTOR( len, VM_PAGE_SIZE );
        pg = DCAlloc( num_pages * (sizeof( *pg ) + VM_PAGE_SIZE) + sizeof( loaded_block ) - 1 );
        if( pg == NULL ) {
            DCStatus( DS_ERR | DS_NO_MEM );
            return( NULL );
        }
        /* set up new page table entries */
        block = (loaded_block *)&pg[num_pages];
        tmp_idx = dir_idx;
        for( j = pg_idx, pageno = 0; pageno < num_pages; ++j, ++pageno ) {
            pg[pageno].block = block;
            pg[pageno].offset = pageno * VM_PAGE_SIZE;
            if( j >= DIR_SIZE ) {
                ++tmp_idx;
                j = 0;
            }
            if( iih->virt[tmp_idx] == NULL ) {
                if( !InitPageDir( iih, tmp_idx ) ) {
                    /* unwind the setup already done */
                    num_pages = pageno;
                    for( pageno = 0; pageno < num_pages; ++pageno, ++pg_idx ) {
                        if( pg_idx >= DIR_SIZE ) {
                            ++dir_idx;
                            pg_idx = 0;
                        }
                        iih->virt[dir_idx][pg_idx] = NULL;
                    }
                    DCFree( pg );
                    return( NULL );
                }
            }
            if( iih->virt[tmp_idx][j] != NULL ) {
                /*
                    We just ran into another allocated block, so we have
                    to kill all the pages mapped in by it. We know that
                    if the page pointer is non-NULL, it will be offset==0
                    since KillPages will clean out the others.
                */
                KillPages( iih, tmp_idx, j );
            }
            iih->virt[tmp_idx][j] = &pg[pageno];
        }
        /* read in new block */
        len = num_pages * VM_PAGE_SIZE;
        block->len = len;
        block->first_dtor = NULL;
        pg_start += iih->bias;
        if( DCSeek( iih->sym_fp, pg_start, DIG_ORG ) ) {
            DCStatus( DS_ERR | DS_FSEEK_FAILED );
            return( NULL );
        }
        /* last block might be a short read */
        if( DCRead( iih->sym_fp, pg->block->data, len ) == DIG_RW_ERROR ) {
            DCStatus( DS_ERR | DS_FREAD_FAILED );
            return( NULL );
        }
        pg = iih->virt[dir_idx][pg_idx];
    }
    ++TimeStamp;
    if( TimeStamp == 0 ) {
        /* deal with wrap-around */
        for( iih = ImageList; iih != NULL; iih = iih->next_image ) {
            if( iih->virt != NULL ) {
                for( i = iih->vm_dir_num - 1; i >= 0; --i ) {
                    if( iih->virt[i] != NULL ) {
                        for( j = DIR_SIZE - 1; j >= 0; --j ) {
                            zero = iih->virt[i][j];
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
    return( &pg->block->data[(start & (VM_PAGE_SIZE - 1)) + pg->offset] );
}

/*
 * Get a 8-bit value.
 */
bool VMGetU8( imp_image_handle *iih, virt_mem start, unsigned_8 *valp )
{
    unsigned_8 *ptr = VMBlock( iih, start, sizeof(*valp) );
    if( ptr ) {
        *valp = *ptr;
        return( true );
    }
    return( false );
}

/*
 * Get a 16-bit value.
 */
bool VMGetU16( imp_image_handle *iih, virt_mem start, unsigned_16 *valp )
{
    unsigned_16 *ptr = VMBlock( iih, start, sizeof(*valp) );
    if( ptr ) {
        *valp = *ptr;
        return( true );
    }
    return( false );
}

/*
 * Get a 32-bit value.
 */
bool VMGetU32( imp_image_handle *iih, virt_mem start, unsigned_32 *valp )
{
    unsigned_32 *ptr = VMBlock( iih, start, sizeof(*valp) );
    if( ptr ) {
        *valp = *ptr;
        return( true );
    }
    return( false );
}

/*
 * Gets a symbol scope table record.
 * Returns pointer to a hll_ssr_common structure on success.
 */
void *VMRecord( imp_image_handle *iih, virt_mem rec_off, virt_mem *next_rec, unsigned_16 *sizep )
{
    unsigned_16 size = 0;
    unsigned_8 *p;

    p = VMBlock( iih, rec_off, 2 );
    if( p != NULL ) {
        size = *p;
        if( size & 0x80 ) {
            size = (size & 0x7f) << 8;
            size |= p[1];
            rec_off++;
        }
        rec_off++;
        p = VMBlock( iih, rec_off, size );
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
void *VMSsBlock( imp_image_handle *iih, hll_dir_entry *hde, virt_mem start, size_t len )
{
    if( start < hde->cb && start + len <= hde->cb ) {
        return( VMBlock( iih, hde->lfo + start, len ) );
    }
    return( NULL );
}

/*
 * Get a 8-bit value.
 */
bool VMSsGetU8( imp_image_handle *iih, hll_dir_entry *hde, virt_mem start, unsigned_8 *valp )
{
    if( start < hde->cb ) {
        unsigned_8 *ptr = VMBlock( iih, hde->lfo + start, sizeof(*valp) );
        if( ptr ) {
            *valp = *ptr;
            return( true );
        }
    }
    return( false );
}

/*
 * Get a 16-bit value.
 */
bool VMSsGetU16( imp_image_handle *iih, hll_dir_entry *hde, virt_mem start, unsigned_16 *valp )
{
    if( start <= hde->cb - sizeof(*valp) ) {
        unsigned_16 *ptr = VMBlock( iih, hde->lfo + start, sizeof(*valp) );
        if( ptr ) {
            *valp = *ptr;
            return( true );
        }
    }
    return( false );
}

/*
 * Get a 32-bit value.
 */
bool VMSsGetU32( imp_image_handle *iih, hll_dir_entry *hde, virt_mem start, unsigned_32 *valp )
{
    if( start <= hde->cb - sizeof(*valp) ) {
        unsigned_32 *ptr = VMBlock( iih, hde->lfo + start, sizeof(*valp) );
        if( ptr ) {
            *valp = *ptr;
            return( true );
        }
    }
    return( false );
}

/*
 * Gets a symbol scope table record.
 * Returns pointer to a hll_ssr_common structure on success.
 */
void *VMSsRecord( imp_image_handle *iih, hll_dir_entry *hde, unsigned_32 rec_off,
                  unsigned_32 *next_rec, unsigned_16 *sizep )
{
    unsigned_16 size = 0;
    unsigned_8 *p = NULL;

    if( rec_off + 2 <= hde->cb) {
        p = VMBlock( iih, hde->lfo + rec_off, 2 );
        if( p != NULL ) {
            size = *p;
            if( size & 0x80 ) {
                size = (size & 0x7f) << 8;
                size |= p[1];
                rec_off++;
            }
            rec_off++;
            if( rec_off + size <= hde->cb ) {
                p = VMBlock( iih, hde->lfo + rec_off, size );
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
    imp_image_handle    *iih;

    kill_time = UINT_MAX;
    kill_i = 0;
    kill_j = 0;
    for( iih = ImageList; iih != NULL; iih = iih->next_image ) {
        if( iih->virt != NULL ) {
            for( i = iih->vm_dir_num-1; i >= 0; --i ) {
                if( iih->virt[i] != NULL ) {
                    for( j = DIR_SIZE-1; j >= 0; --j ) {
                        pg = iih->virt[i][j];
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
    if( kill_time == UINT_MAX )
        return( 0 );
    return( KillPages( iih, kill_i, kill_j ) );
}


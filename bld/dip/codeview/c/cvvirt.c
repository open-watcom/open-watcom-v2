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


#include "cvinfo.h"
#include <limits.h>

#define VM_PAGE_BITS    12
#define DIR_BITS        4
#define VM_PAGE_SIZE    (1U<<VM_PAGE_BITS)
#define DIR_SIZE        (1UL<<DIR_BITS)

#define GET_DIR( v )    ((v) >> (DIR_BITS+VM_PAGE_BITS))
#define GET_PAGE( v )   (((v) >> VM_PAGE_BITS) & (DIR_SIZE-1))
#define GET_OFFSET( v ) ((v) & (VM_PAGE_SIZE-1))

typedef struct {
    unsigned_16         time_stamp;
    unsigned_16         len;
    unsigned_8          data[1];        /* variable sized */
} loaded_block;

typedef struct virt_page {
    loaded_block        *block;
    unsigned_16         offset;
} virt_page;


static unsigned         TimeStamp;

dip_status VMInit( imp_image_handle *iih, unsigned long size )
{
    iih->vm_dir_num = BLOCK_FACTOR( size, DIR_SIZE * VM_PAGE_SIZE );
    iih->virt = DCAlloc( iih->vm_dir_num * sizeof( *iih->virt ) );
    if( iih->virt == NULL ) {
        DCStatus( DS_ERR | DS_NO_MEM );
        return( DS_ERR | DS_NO_MEM );
    }
    memset( iih->virt, 0, iih->vm_dir_num * sizeof( *iih->virt ) );
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
    DCFree( pg );
    return( num_pages * VM_PAGE_SIZE );
}

static int InitPageDir( imp_image_handle *iih, unsigned dir_idx )
{
    iih->virt[dir_idx] = DCAlloc( sizeof( virt_page * ) * DIR_SIZE );
    if( iih->virt[dir_idx] == NULL ) {
        DCStatus( DS_ERR | DS_NO_MEM );
        return( 0 );
    }
    memset( iih->virt[dir_idx], 0, sizeof( virt_page * ) * DIR_SIZE );
    return( 1 );
}

void *VMBlock( imp_image_handle *iih, virt_mem start, size_t len )
{
    unsigned            dir_idx;
    unsigned            pg_idx;
    unsigned            tmp_idx;
    unsigned            i;
    unsigned            j;
    unsigned            num_pages;
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
    if( pg == NULL || ( pg->block->len - pg->offset ) < len ) {
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
            DCFree( pg );
        }
        num_pages = BLOCK_FACTOR( len, VM_PAGE_SIZE );
        pg = DCAlloc( num_pages * ( sizeof( *pg ) + VM_PAGE_SIZE ) + sizeof( loaded_block ) - 1 );
        if( pg == NULL ) {
            DCStatus( DS_ERR | DS_NO_MEM );
            return( NULL );
        }
        /* set up new page table entries */
        block = (loaded_block *)&pg[num_pages];
        tmp_idx = dir_idx;
        for( j = pg_idx, i = 0; i < num_pages; ++j, ++i ) {
            pg[i].block = block;
            pg[i].offset = i * VM_PAGE_SIZE;
            if( j >= DIR_SIZE ) {
                ++tmp_idx;
                j = 0;
            }
            if( iih->virt[tmp_idx] == NULL ) {
                if( !InitPageDir( iih, tmp_idx ) ) {
                    /* unwind the setup already done */
                    num_pages = i;
                    for( i = 0; i < num_pages; ++i, ++pg_idx ) {
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
            iih->virt[tmp_idx][j] = &pg[i];
        }
        /* read in new block */
        len = num_pages * VM_PAGE_SIZE;
        block->len = len;
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
                for( i = iih->vm_dir_num; i-- > 0; ) {
                    if( iih->virt[i] != NULL ) {
                        for( j = DIR_SIZE; j-- > 0; ) {
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

void *VMRecord( imp_image_handle *iih, virt_mem rec )
{
    s_common    *p;

    p = VMBlock( iih, rec, sizeof( *p ) );
    if( p != NULL ) {
        p = VMBlock( iih, rec, p->length + sizeof( p->length ) );
    }
    return( p );
}

unsigned VMShrink( void )
{
    int                 i;
    int                 j;
    unsigned            kill_time;
    unsigned            kill_i;
    unsigned            kill_j;
    virt_page           *pg;
    imp_image_handle    *iih;
    imp_image_handle    *kill_iih;

    kill_time = UINT_MAX;
    kill_i = 0;
    kill_j = 0;
    kill_iih = NULL;
    /* search oldest page by time_stamp */
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
                                kill_iih = iih;
                            }
                        }
                    }
                }
            }
        }
    }
    if( kill_iih == NULL )
        return( 0 );
    return( KillPages( kill_iih, kill_i, kill_j ) );
}

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
 Description:
 ============
     This is a system indepedent debug memory package. Compile this
     module and use the entries as noted.

*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define DBG_INTERNAL
#include "dbgmem.h"

/*** internal stuff ***/

#define ALLOC_BEGIN_TAG 0xab
#define ALLOC_END_TAG   0xae
#define FREE_BEGIN_TAG  0xfb
#define FREE_END_TAG    0xfe

#define ALLOC_FILL      0xaf
#define FREE_FILL       0xff


typedef struct dbg_mem {
    char                start_tag;      // start tag
    unsigned long       size;           // size of alloc
    struct dbg_mem DFAR *next;      // next memory alloc
    char                mem[1];         // the alloc space
} dbg_mem;

typedef struct {
    dbg_info            info;
    long                curr_num_blocks;
    long                total_num_allocs;
    long                total_num_frees;
    long                total_num_reallocs;
    dbg_mem             DFAR *alloc_list;
} dbg_list;


#define _mk_dbg( x ) (dbg_mem DFAR *)( (char DFAR *)x - sizeof( dbg_mem ) + 1 )

extern int dbg_mem_check( dbg_list DFAR *mem_ctl );

static int dbg_error(
/*******************/
/* always returns FALSE */

    dbg_list DFAR       *mem_ctl,
    char                *msg,
    dbg_mem DFAR        *dbg
) {
    char                buf[200];

    sprintf( buf, "*ERROR* in %s: %s (memory ptr: %p)",
                        mem_ctl->info.rpt_title, msg, (void far *) dbg->mem );
    (mem_ctl->info.prt_line)( buf, TRUE );

    return( FALSE );
}


static dbg_mem DFAR * DFAR *dbg_find(
/***********************************/
/* find forward ptr to this block. report error if not found */

    dbg_list DFAR       *mem_ctl,
    dbg_mem             *dbg
) {
    dbg_mem DFAR        * DFAR *ptr;
    int                 i;

    i = 0;
    for( ptr = &mem_ctl->alloc_list; *ptr != NULL; ptr = &((*ptr)->next) ) {
        if( *ptr == dbg ) {
            break;
        }
        ++i;
        if( i == mem_ctl->curr_num_blocks ) {
            break;
        }
    }

    if( *ptr == NULL ) {
        dbg_error( mem_ctl, "block not found in memory list", dbg );
        ptr = NULL;
    }

    return( ptr );
}

static void dbg_do_free(
/**********************/

    dbg_list DFAR       *mem_ctl,
    dbg_mem DFAR        *dbg
) {
    dbg_mem DFAR        * DFAR *ptr;

    memset( dbg->mem, FREE_FILL, dbg->size );

    ptr = dbg_find( mem_ctl, dbg );
    *ptr = (*ptr)->next;    // fix alloc chain

    dbg->start_tag = FREE_BEGIN_TAG;
    dbg->mem[dbg->size] = FREE_END_TAG;

    (mem_ctl->info.free)( dbg );

    --mem_ctl->curr_num_blocks;
}

static void DFAR *dbg_do_alloc(
/*****************************/

    dbg_list DFAR       *mem_ctl,
    unsigned long       size
) {
    dbg_mem DFAR        *dbg;

    if( size == 0 ) {
        return( NULL );
    }

    dbg = (mem_ctl->info.alloc)( sizeof( dbg_mem ) + size );

    if( dbg == NULL ) {
        return( NULL );
    }

    dbg->start_tag = ALLOC_BEGIN_TAG;
    dbg->size = size;
    dbg->next = mem_ctl->alloc_list;
    mem_ctl->alloc_list = dbg;
    memset ( dbg->mem, ALLOC_FILL, size );
    dbg->mem[size] = ALLOC_END_TAG;
    ++mem_ctl->curr_num_blocks;

    return( dbg->mem );
}

static int dbg_check_block(
/*************************/
/* check a particular (alloced) block to make sure its ok */

    dbg_list DFAR       *mem_ctl,
    dbg_mem DFAR        *dbg
) {
    char                tag;

    if( mem_ctl->info.level >= DBG_MEM_2 ) {
        tag = dbg->start_tag;
        if( tag == FREE_BEGIN_TAG ) {
            return( dbg_error( mem_ctl, "block is freed", dbg ) );
        } else if( tag != ALLOC_BEGIN_TAG ) {
            return( dbg_error( mem_ctl, "bad start tag", dbg ) );
        }

        tag = dbg->mem[dbg->size];
        if( tag == FREE_END_TAG ) {
            return( dbg_error( mem_ctl, "block is freed", dbg ) );
        } else if( tag != ALLOC_END_TAG ) {
            return( dbg_error( mem_ctl, "bad end tag", dbg ) );
        }
    }

    return( TRUE );
}

static int do_check(
/******************/

    dbg_list DFAR       *mem_ctl
) {
    if( mem_ctl->info.level >= DBG_MEM_3 ) {
        return( dbg_mem_check( mem_ctl ) >= 0 );
    }

    return( TRUE );
}

/***** external routines *****/

void dbg_mem_line(
/****************/

    dbg_list DFAR       *mem_ctl,
    char                *msg,
    int                 err
) {
    char                buf[200];

    sprintf( buf, "*NOTE* (%s): %s", mem_ctl->info.rpt_title, msg );
    (mem_ctl->info.prt_line)( buf, err );
}


int dbg_mem_check(
/****************/
/* check to make sure memory management is in good shape. Errors reported */

    dbg_list DFAR       *mem_ctl
) {
    dbg_mem DFAR        *ptr;
    int                 num;
    int                 ret;
    dbg_level           save_level;

    if( mem_ctl->info.level < DBG_MEM_1 ) {
        return( 0 );
    }

    /* if debug stuff there, always do the check from this routine */
    save_level = mem_ctl->info.level;
    mem_ctl->info.level = DBG_MEM_3;

    num = 0;
    ret = -1;
    for( ptr = mem_ctl->alloc_list; ptr != NULL; ptr = ptr->next ) {
        ++num;
        if( num > mem_ctl->curr_num_blocks ) {
            dbg_error( mem_ctl, "Too many allocated blocks", NULL );
            goto done;
        }
        if( !dbg_check_block( mem_ctl, ptr ) ) {
            goto done;
        }
    }

    if( num != mem_ctl->curr_num_blocks ) {
        dbg_error( mem_ctl, "Not enough allocated blocks", NULL );
    }
    ret = mem_ctl->curr_num_blocks;

done:
    mem_ctl->info.level = save_level;
    return( ret );
}

int dbg_mem_report(
/*****************/

    dbg_list DFAR       *mem_ctl
) {
    dbg_mem DFAR        *dbg;
    char                buf[200];
    char                str[11];
    int                 i;

    if( mem_ctl->info.level < DBG_MEM_1 ) {
        return( 0 );
    }

    if( dbg_mem_check( mem_ctl ) < 0 ) {
        return( -1 );
    }
    (mem_ctl->info.prt_line)( "", FALSE );
    sprintf( buf, "Memory Report: %s", mem_ctl->info.rpt_title );
    (mem_ctl->info.prt_line)( buf, FALSE );
    (mem_ctl->info.prt_line)( "===================================", FALSE );
    (mem_ctl->info.prt_line)( "", FALSE );

    sprintf( buf, "current unfreed allocated blocks: %ld",
                                        mem_ctl->curr_num_blocks );
    (mem_ctl->info.prt_line)( buf, FALSE );
    sprintf( buf, "Total number of allocs: %ld",
                                        mem_ctl->total_num_allocs );
    (mem_ctl->info.prt_line)( buf, FALSE );
    sprintf( buf, "Total number of reallocs: %ld",
                                        mem_ctl->total_num_reallocs );
    (mem_ctl->info.prt_line)( buf, FALSE );
    sprintf( buf, "Total number of frees: %ld",
                                        mem_ctl->total_num_frees );
    (mem_ctl->info.prt_line)( buf, FALSE );

    (mem_ctl->info.prt_line)( "Current unfreed chunks:", FALSE );

    (mem_ctl->info.prt_line)( "address             size           First 10 Characters/First 10 Bytes", FALSE );
    (mem_ctl->info.prt_line)( "======================================================================", FALSE );

    str[10] = '\0';
    for( dbg = mem_ctl->alloc_list; dbg != NULL; dbg = dbg->next ) {
        for( i = 0; i < 10; ++i ) {
            if( isprint( dbg->mem[i] ) ) {
                str[i] = dbg->mem[i];
            } else {
                str[i] = '.';
            }
        }
        sprintf( buf,
            "%-20p%-15ld%-11s%-3x%-3x%-3x%-3x%-3x%-3x%-3x%-3x%-3x%-3x",
            (void far *) dbg->mem, dbg->size, str,
            dbg->mem[0], dbg->mem[1], dbg->mem[2], dbg->mem[3], dbg->mem[4],
            dbg->mem[5], dbg->mem[6], dbg->mem[7], dbg->mem[8], dbg->mem[9] );
        (mem_ctl->info.prt_line)( buf, FALSE );
    }

    return( mem_ctl->curr_num_blocks );
}

void DFAR *dbg_mem_init(
/**********************/

    dbg_info            *info
) {
    dbg_list DFAR       *mem_ctl;

    mem_ctl = (*info->alloc)( sizeof( dbg_list ) );
    if( mem_ctl != NULL ) {
        memset( mem_ctl, 0, sizeof( dbg_list ) );
        mem_ctl->info = *info;
    }

    return( mem_ctl );
}

void dbg_mem_close(
/*****************/

    dbg_list DFAR       *mem_ctl
) {
    (((dbg_list DFAR *)mem_ctl)->info.free)( mem_ctl );
}

void dbg_mem_set_level(
/*********************/

    dbg_list DFAR       *mem_ctl,
    dbg_level           level
) {
    ((dbg_list DFAR *)mem_ctl)->info.level = level;
}

void DFAR *dbg_mem_alloc(
/***********************/

    dbg_list DFAR       *mem_ctl,
    unsigned long       size
) {
    if( mem_ctl->info.level == DBG_MEM_0 ) {
        return( (mem_ctl->info.alloc)( size ) );
    }

    if( do_check( mem_ctl ) ) {
        ++mem_ctl->total_num_allocs;
        return( dbg_do_alloc( mem_ctl, size ) );
    }
    return( NULL );
}

void dbg_mem_free(
/****************/

    dbg_list DFAR       *mem_ctl,
    void DFAR           *mem
) {
    dbg_mem DFAR        *dbg;

    if( mem_ctl->info.level == DBG_MEM_0 ) {
        (mem_ctl->info.free)( mem );
        return;
    }

    if( do_check( mem_ctl ) ) {
        ++mem_ctl->total_num_frees;
        if( mem != NULL ) {
            dbg = _mk_dbg( mem );
            if( dbg_check_block( mem_ctl, dbg ) ) {
                dbg_do_free( mem_ctl, dbg );
            }
        }
    }
}

void DFAR *dbg_mem_realloc(
/*************************/

    dbg_list DFAR       *mem_ctl,
    void DFAR           *mem,
    unsigned long       size
) {
    dbg_mem             *dbg;
    dbg_mem             **ptr;

    if( mem_ctl->info.level == DBG_MEM_0 ) {
        return( (mem_ctl->info.realloc)( mem, size ) );
    }

    if( do_check( mem_ctl ) ) {
        ++mem_ctl->total_num_reallocs;

        if( mem == NULL ) {
            // first call to realloc for this memory
            return( dbg_do_alloc( mem_ctl, size ) );
        }

        dbg = _mk_dbg( mem );
        if( dbg_check_block( mem_ctl, dbg ) ) {
            if( size == 0 ) {
                /* we are freeing this piece instead */
                dbg_do_free( mem_ctl, dbg );
                return( NULL );
            }

            ptr = dbg_find( mem_ctl, dbg );

            dbg = (mem_ctl->info.realloc)( dbg, size + sizeof( dbg_mem ) );
            if( dbg == NULL ) {
                return( NULL );
            }

            *ptr = dbg;         // fix Alloc chain
            dbg->size = size;
            dbg->mem[size] = ALLOC_END_TAG;

            return( dbg->mem );
        }
    }

    return( NULL );
}

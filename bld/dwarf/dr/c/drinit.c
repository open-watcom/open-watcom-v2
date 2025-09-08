/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Initialize DWARF debug info for a module and distil
*               the .debug_abbrev section.
*
****************************************************************************/


#include "drpriv.h"
#include "drgettab.h"


dr_dbg_handle   DR_CurrNode = NULL;

/* function prototypes */

static void ReadCUAbbrevTable( dr_dbg_handle dbg, dr_cu_handle compunit )
/***********************************************************************/
/* this reads in the abbrev. table for a compilation unit, and fills in an
 * array of pointers to it.
 */
{
    dr_abbrev_idx   maxnum;
    dr_abbrev_idx   sizealloc;
    dr_abbrev_idx   oldsize;
    dr_abbrev_idx   i;
    drmem_hdl       start;
    drmem_hdl       finish;
    drmem_hdl       *abbrevs;
    dr_abbrev_idx   code;
    dr_cu_handle    cu;

    // if a previous compilation unit shares the same table, reuse it
    for( cu = &(dbg->compunit); cu != compunit; cu = cu->next ) {
        if( cu->abbrev_start == compunit->abbrev_start ) {
            compunit->numabbrevs  = cu->numabbrevs;
            compunit->abbrevs     = cu->abbrevs;
            compunit->abbrev_refs = cu->abbrev_refs;
            ++(*compunit->abbrev_refs); // increase abbrevs reference count
            return;
        }
    }
    sizealloc = ABBREV_TABLE_GUESS;
    abbrevs = DR_ALLOC( sizealloc * sizeof( drmem_hdl ) );
    for( i = 0; i < sizealloc; ++i ) {
        abbrevs[i] = DRMEM_HDL_NULL;
    }
    maxnum = 0;
    start = dbg->sections[DR_DEBUG_ABBREV].base + compunit->abbrev_start;
    finish = dbg->sections[DR_DEBUG_ABBREV].base + dbg->sections[DR_DEBUG_ABBREV].size;
    while( start < finish ) {
        code = DR_VMReadULEB128( &start );
        if( code == 0 )
            break;                  // indicates end of table
        if( code > maxnum )
            maxnum = code;
        if( code >= sizealloc ) {
            oldsize = sizealloc;
            sizealloc = code + ABBREV_TABLE_INCREMENT;
            abbrevs = DR_REALLOC( abbrevs, sizealloc * sizeof( drmem_hdl ) );
            for( i = oldsize; i < sizealloc; ++i ) {
                abbrevs[i] = DRMEM_HDL_NULL;
            }
        }
        if( abbrevs[code] == DRMEM_HDL_NULL ) {
            abbrevs[code] = start;
        }
        DR_VMSkipLEB128( &start );              // skip tag
        start++;                                // skip child indicator
        do {
            code = DR_VMReadULEB128( &start );  // read attribute
            DR_VMSkipLEB128( &start );          // skip form
        } while( code != 0 );
    }
    if( sizealloc > maxnum ) {  // reduce size to actual amount needed
        /* abbrev[0] not used but we want abbrev[code] = start to work */
        abbrevs = DR_REALLOC( abbrevs, ( maxnum + 1 ) * sizeof( drmem_hdl ) );
    }
    compunit->numabbrevs = maxnum + 1;
    compunit->abbrevs = abbrevs;
    compunit->abbrev_refs = DR_ALLOC( sizeof( unsigned ) );
    *compunit->abbrev_refs = 1;
}

static dr_dbg_handle  InitDbgHandle( void *file, unsigned long *sizes, bool byteswap )
/************************************************************************************/
{
    dr_dbg_handle       dbg;
    int                 i;
    unsigned long       size;

    dbg = DR_ALLOC( sizeof( *dbg ) );
    DR_CurrNode = dbg;    /* must be set for DR_VMAlloc in virtstub.c */
    if( dbg == NULL )
        return( NULL );
    dbg->next = NULL;
    dbg->file = file;
    dbg->addr_size = 0;
    dbg->wat_producer_ver = VER_NONE;
    dbg->byte_swap = byteswap;
    dbg->last_ccu = &dbg->compunit;
    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        size = sizes[i];
        dbg->sections[i].size = size;
        if( size != 0 ) {
            dbg->sections[i].base = DR_VMAlloc( size, i );
            if( dbg->sections[i].base == DRMEM_HDL_NULL ) {
                DR_FREE( dbg );
                return( NULL );
            }
        } else {
            dbg->sections[i].base = DRMEM_HDL_NULL;
        }
    }
    return( dbg );
}

bool  DRENTRY DRDbgClear( dr_dbg_handle dbg )
/*******************************************/
{
    bool                ret;
    int                 i;

    /* unused parameters */ (void)dbg;

    ret = false;
    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        DR_VMSwap( dbg->sections[i].base, dbg->sections[i].size, &ret );
    }
    return( ret );
}

void DRENTRY DRDbgDone( dr_dbg_handle dbg )
/*****************************************/
{
    int                 i;

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        DR_VMSectDone( dbg->sections[i].base, dbg->sections[i].size );
        dbg->sections[i].base = DRMEM_HDL_NULL;
        dbg->sections[i].size = 0;
    }
}

void DRENTRY DRDbgWatProducerVer( dr_dbg_handle dbg, df_ver wat_producer_ver )
/****************************************************************************/
{
    dbg->wat_producer_ver = wat_producer_ver;
}

static void ReadCompUnits( dr_dbg_handle dbg, int read_ftab )
/***********************************************************/
{
    dr_cu_handle        compunit;
    dr_cu_handle        next;
    drmem_hdl           start;
    drmem_hdl           finish;
    unsigned_16         version;
    unsigned_8          addr_size;
    unsigned_8          curr_addr_size;

    compunit = &DR_CurrNode->compunit;
    start = DR_CurrNode->sections[DR_DEBUG_INFO].base;
    finish = start + DR_CurrNode->sections[DR_DEBUG_INFO].size;
    addr_size = DR_VMReadByte( start + offsetof( compuhdr_prologue, addr_size ) );
    for( ;; ) {
        compunit->next = NULL;
        compunit->start = start;
        compunit->end = start + sizeof( unsigned_32 ) + DR_VMReadDWord( start );
        version = DR_VMReadWord( start + offsetof( compuhdr_prologue, version ) );
        if( DWARF_VER_INVALID( version ) )
            DR_EXCEPT( DREXCEP_BAD_DBG_VERSION );
        compunit->abbrev_start = DR_VMReadDWord( start + offsetof( compuhdr_prologue, abbrev_offset ) );
        curr_addr_size = DR_VMReadByte( start + offsetof( compuhdr_prologue, addr_size ) );
        if( curr_addr_size != addr_size ) {
            addr_size = 0;
        }
        ReadCUAbbrevTable( dbg, compunit );
        if( read_ftab ) {
            DR_InitFileTable( &compunit->filetab );
            DR_ScanFileTable( start, &DR_FileNameTable, &compunit->filetab );
        } else {
            compunit->filetab.len = 0;
            compunit->filetab.tab = NULL;
        }
        if( compunit->end >= finish )
            break;
        start = compunit->end;
        next = DR_ALLOC( sizeof( *next ) );
        compunit->next = next;
        compunit = next;
    }
    DR_CurrNode->addr_size = addr_size;
}

dr_dbg_handle DRENTRY DRDbgInitNFT( void * file, unsigned long * sizes, bool byteswap )
/*************************************************************************************/
{
    dr_dbg_handle       dbg;

    dbg = InitDbgHandle( file, sizes, byteswap );
    if( dbg != NULL ) {
        ReadCompUnits( dbg, false );
    }
    return( dbg );
}

dr_dbg_handle DRENTRY DRDbgInit( void * file, unsigned long * sizes, bool byteswap )
/**********************************************************************************/
{
    dr_dbg_handle       dbg;

    dbg = InitDbgHandle( file, sizes, byteswap );
    if( dbg != NULL ) {
        ReadCompUnits( dbg, true );
    }
    return( dbg );
}

void DRENTRY DRDbgFini( dr_dbg_handle dbg )
/******************************************
 * don't have a way of deallocating virtual memory space.  Assume that any
 * pages that are allocated to this module will eventually be swapped out
 */
{
    dr_cu_handle        compunit;
    dr_cu_handle        next;

    compunit = dbg->compunit.next;
    while( compunit != NULL ) {
        next = compunit->next;
        DR_FiniFileTable( &compunit->filetab, false );
        if( compunit->abbrevs != NULL ) {
            --(*compunit->abbrev_refs);
            if( *compunit->abbrev_refs == 0 ) {
                DR_FREE( compunit->abbrevs );
                DR_FREE( compunit->abbrev_refs );
            }
            compunit->abbrevs = NULL;
            compunit->abbrev_refs = NULL;
        }
        DR_FREE( compunit );
        compunit = next;
    }
    DR_FiniFileTable( &dbg->compunit.filetab, false );
    DR_FREE( dbg );
}

dr_dbg_handle DRENTRY DRSetDebug( dr_dbg_handle dbg )
/***************************************************/
{
    dr_dbg_handle  old;
    old = DR_CurrNode;
    DR_CurrNode = dbg;
    return( old );
}

dr_dbg_handle DRENTRY DRGetDebug( void )
/**************************************/
{
    return( DR_CurrNode );
}

void DRENTRY DRInit( void )
/*************************/
{
    DR_VMInit();
    DR_InitFileTable( &DR_FileNameTable.fnametab );
    DR_InitFileTable( &DR_FileNameTable.pathtab );
}

void DRENTRY DRFini( void )
/*************************/
{
    DR_VMDestroy();
    DR_FiniFileTable( &DR_FileNameTable.fnametab, true );
    DR_FiniFileTable( &DR_FileNameTable.pathtab, true );
}

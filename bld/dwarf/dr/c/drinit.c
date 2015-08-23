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
* Description:  Initialize DWARF debug info for a module and distil
*               the .debug_abbrev section.
*
****************************************************************************/


#include "drpriv.h"
#include "drgettab.h"
#include <string.h>

struct dr_dbg_info  *DWRCurrNode = NULL;

/* function prototypes */

static void ReadCUAbbrevTable( struct dr_dbg_info *dbg, compunit_info *compunit )
/*******************************************************************************/
/* this reads in the abbrev. table for a compilation unit, and fills in an
 * array of pointers to it.
 */
{
    dr_abbrev_idx   maxnum;
    dr_abbrev_idx   sizealloc;
    dr_abbrev_idx   oldsize;
    dr_abbrev_idx   i;
    dr_handle       start;
    dr_handle       finish;
    dr_handle       *abbrevs;
    dr_abbrev_idx   code;
    compunit_info   *cu;

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
    abbrevs = DWRALLOC( sizealloc * sizeof( dr_handle ) );
    for( i = 0; i < sizealloc; ++i ) {
        abbrevs[i] = DR_HANDLE_NUL;
    }
    maxnum = 0;
    start = dbg->sections[DR_DEBUG_ABBREV].base + compunit->abbrev_start;
    finish = dbg->sections[DR_DEBUG_ABBREV].base + dbg->sections[DR_DEBUG_ABBREV].size;
    while( start < finish ) {
        code = DWRVMReadULEB128( &start );
        if( code == 0 )
            break;                  // indicates end of table
        if( code > maxnum )
            maxnum = code;
        if( code >= sizealloc ) {
            oldsize = sizealloc;
            sizealloc = code + ABBREV_TABLE_INCREMENT;
            abbrevs = DWRREALLOC( abbrevs, sizealloc * sizeof( dr_handle ) );
            for( i = oldsize; i < sizealloc; ++i ) {
                abbrevs[i] = DR_HANDLE_NUL;
            }
        }
        if( abbrevs[code] == DR_HANDLE_NUL ) {
            abbrevs[code] = start;
        }
        DWRVMSkipLEB128( &start );              // skip tag
        start++;                                // skip child indicator
        do {
            code = DWRVMReadULEB128( &start );  // read attribute
            DWRVMSkipLEB128( &start );          // skip form
        } while( code != 0 );
    }
    if( sizealloc > maxnum ) {  // reduce size to actual amount needed
        /* abbrev[0] not used but we want abbrev[code] = start to work */
        abbrevs = DWRREALLOC( abbrevs, ( maxnum + 1 ) * sizeof( dr_handle ) );
    }
    compunit->numabbrevs = maxnum + 1;
    compunit->abbrevs = abbrevs;
    compunit->abbrev_refs = DWRALLOC( sizeof( unsigned ) );
    *compunit->abbrev_refs = 1;
}

static dr_dbg_handle  InitDbgHandle( void *file, unsigned long *sizes, bool byteswap )
/************************************************************************************/
{
    dr_dbg_handle       dbg;
    int                 i;
    unsigned long       size;

    dbg = DWRALLOC( sizeof( struct dr_dbg_info ) );
    DWRCurrNode = dbg;    /* must be set for DWRVMAlloc in virtstub.c */
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
            dbg->sections[i].base = DWRVMAlloc( size, i );
            if( dbg->sections[i].base == DR_HANDLE_NUL ) {
                DWRFREE( dbg );
                return( NULL );
            }
        } else {
            dbg->sections[i].base = DR_HANDLE_NUL;
        }
    }
    return( dbg );
}

bool  DRDbgClear( dr_dbg_handle dbg )
/***********************************/
{
    bool                ret;
    int                 i;

    dbg = dbg;
    ret = FALSE;
    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        DWRVMSwap( dbg->sections[i].base, dbg->sections[i].size, &ret );
    }
    return( ret );
}

extern void DRDbgDone( dr_dbg_handle dbg )
/****************************************/
{
    int                 i;

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        DWRVMSectDone( dbg->sections[i].base, dbg->sections[i].size );
        dbg->sections[i].base = DR_HANDLE_NUL;
        dbg->sections[i].size = 0;
    }
}

extern void DRDbgWatProducerVer( dr_dbg_handle dbg, df_ver wat_producer_ver )
/***************************************************************************/
{
    dbg->wat_producer_ver = wat_producer_ver;
}

static void ReadCompUnits( struct dr_dbg_info *dbg, int read_ftab )
/*****************************************************************/
{
    compunit_info       *compunit;
    compunit_info       *next;
    dr_handle           start;
    dr_handle           finish;
    unsigned_16         version;
    unsigned_8          addr_size;
    unsigned_8          curr_addr_size;

    compunit = &DWRCurrNode->compunit;
    start = DWRCurrNode->sections[DR_DEBUG_INFO].base;
    finish = start + DWRCurrNode->sections[DR_DEBUG_INFO].size;
    addr_size = DWRVMReadByte( start + COMPILE_UNIT_HDR_ADDR_SIZE );
    for( ;; ) {
        compunit->next = NULL;
        compunit->start = start;
        compunit->end = start + sizeof( unsigned_32 ) + DWRVMReadDWord( start );
        version = DWRVMReadWord( start + COMPILE_UNIT_HDR_VERSION );
        if( DWARF_VER_INVALID( version ) )
            DWREXCEPT( DREXCEP_BAD_DBG_VERSION );
        compunit->abbrev_start = DWRVMReadDWord( start + COMPILE_UNIT_HDR_ABBREV_OFFSET );
        curr_addr_size = DWRVMReadByte( start + COMPILE_UNIT_HDR_ADDR_SIZE );
        if( curr_addr_size != addr_size ) {
            addr_size = 0;
        }
        ReadCUAbbrevTable( dbg, compunit );
        if( read_ftab ) {
            DWRInitFileTable( &compunit->filetab );
            DWRScanFileTable( start, &FileNameTable, &compunit->filetab );
        } else {
            compunit->filetab.len = 0;
            compunit->filetab.tab = NULL;
        }
        if( compunit->end >= finish )
            break;
        start = compunit->end;
        next = DWRALLOC( sizeof( compunit_info ) );
        compunit->next = next;
        compunit = next;
    }
    DWRCurrNode->addr_size = addr_size;
}

dr_dbg_handle DRDbgInitNFT( void * file, unsigned long * sizes, bool byteswap )
/*****************************************************************************/
{
    dr_dbg_handle       dbg;

    dbg = InitDbgHandle( file, sizes, byteswap );
    if( dbg != NULL ) {
        ReadCompUnits( dbg, FALSE );
    }
    return( dbg );
}

dr_dbg_handle DRDbgInit( void * file, unsigned long * sizes, bool byteswap )
/**************************************************************************/
{
    dr_dbg_handle       dbg;

    dbg = InitDbgHandle( file, sizes, byteswap );
    if( dbg != NULL ) {
        ReadCompUnits( dbg, TRUE );
    }
    return( dbg );
}

void DRDbgFini( dr_dbg_handle dbg )
/**********************************/
/* don't have a way of deallocating virtual memory space.  Assume that any
 * pages that are allocated to this module will eventually be swapped out
*/
{
    compunit_info       *compunit;
    compunit_info       *next;

    compunit = dbg->compunit.next;
    while( compunit != NULL ) {
        next = compunit->next;
        DWRFiniFileTable( &compunit->filetab, FALSE );
        if( compunit->abbrevs != NULL ) {
            --(*compunit->abbrev_refs);
            if( *compunit->abbrev_refs == 0 ) {
                DWRFREE( compunit->abbrevs );
                DWRFREE( compunit->abbrev_refs );
            }
            compunit->abbrevs = NULL;
            compunit->abbrev_refs = NULL;
        }
        DWRFREE( compunit );
        compunit = next;
    }
    DWRFiniFileTable( &dbg->compunit.filetab, FALSE );
    DWRFREE( dbg );
}

dr_dbg_handle  DRSetDebug( dr_dbg_handle dbg )
/********************************************/
{
    dr_dbg_handle  old;
    old = DWRCurrNode;
    DWRCurrNode = dbg;
    return( old );
}

dr_dbg_handle  DRGetDebug( void )
/*******************************/
{
    return( DWRCurrNode );
}

void DRInit( void )
/*****************/
{
    DWRVMInit();
    DWRInitFileTable( &FileNameTable.fnametab );
    DWRInitFileTable( &FileNameTable.pathtab );
}

void DRFini( void )
/*****************/
{
    DWRVMDestroy();
    DWRFiniFileTable( &FileNameTable.fnametab, TRUE );
    DWRFiniFileTable( &FileNameTable.pathtab, TRUE );
}

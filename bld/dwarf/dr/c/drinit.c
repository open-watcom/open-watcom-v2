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


#include "drpriv.h"
#include "drgettab.h"
#include <string.h>

struct dr_dbg_info * DWRCurrNode = NULL;

/* function prototypes */

static ReadAbbrevTable( struct dr_dbg_info *dbg )
/**********************/
/* this reads in the abbrev. table, and fills in an array of pointers to it */
{
    unsigned    maxnum;
    unsigned    sizealloc;
    unsigned    oldsize;
    dr_handle   start;
    dr_handle   finish;
    dr_handle  *abbrevs;
    unsigned    code;

    sizealloc = ABBREV_TABLE_GUESS;
    abbrevs = DWRALLOC( sizealloc * sizeof(dr_handle));
    memset( abbrevs, 0, sizealloc * sizeof(dr_handle) );
    maxnum = 0;
    start = dbg->sections[DR_DEBUG_ABBREV].base;
    finish = start + dbg->sections[DR_DEBUG_ABBREV].size;
    while( start < finish ) {
        code = DWRVMReadULEB128( &start );
        if( code == 0 ) continue;       // NOTE! <----- weird control flow.
        if( code > maxnum ) maxnum = code;
        if( code >= sizealloc ) {
            oldsize = sizealloc;
            sizealloc = code + ABBREV_TABLE_INCREMENT;
            abbrevs = DWRREALLOC( abbrevs, sizealloc * sizeof(dr_handle) );
            memset( &abbrevs[oldsize], 0,
                        (sizealloc - oldsize) * sizeof(dr_handle) );
        }
        if( abbrevs[code] == 0 ){
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
        abbrevs = DWRREALLOC( abbrevs, (maxnum+1) * sizeof(dr_handle) );
    }
    dbg->numabbrevs = maxnum+1;
    dbg->abbrevs = abbrevs;
}

static dr_dbg_handle  InitDbgHandle( void *file, unsigned long *sizes ){
/**********************************************************************/
    dr_dbg_handle       dbg;
    int                 i;

    dbg = DWRALLOC( sizeof(struct dr_dbg_info) );
    DWRCurrNode = dbg;    /* must be set for DWRVMAlloc in virtstub.c */
    if( dbg == NULL )goto error;
    dbg->file = file;
    dbg->abbrevs = NULL;
    dbg->addr_size = 0;
    dbg->old_version = 0;
    dbg->last_ccu = &dbg->compunit;
    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        dbg->sections[i].size = *sizes;
        if( *sizes != 0 ){
            dbg->sections[i].base = DWRVMAlloc( *sizes, i );
            if( dbg->sections[i].base == NULL )goto error;
        }else{
            dbg->sections[i].base = NULL;
        }
        sizes++;
    }
    return( dbg );
error:
    return( NULL );
}

extern int  DRDbgClear( dr_dbg_handle dbg ){
/*******************************************/
    int                 ret;
    int                 i;

    ret = FALSE;
    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        DWRVMSwap( dbg->sections[i].base, dbg->sections[i].size, &ret );
    }
    return( ret );
}

extern void DRDbgDone( dr_dbg_handle dbg ){
/*******************************************/
    int                 i;

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i++ ) {
        DWRVMSectDone( dbg->sections[i].base, dbg->sections[i].size );
        dbg->sections[i].base = 0;
        dbg->sections[i].size = 0;
    }
}

extern void DRDbgOldVersion( dr_dbg_handle dbg ){
/*******************************************/
    dbg->old_version = 1; // remove when pat 10.5
}

dr_dbg_handle DRDbgInit( void * file, unsigned long * sizes )
/***********************************************************/
{
    dr_dbg_handle       dbg;

    dbg = InitDbgHandle( file, sizes );
    if( dbg != NULL ){
        ReadAbbrevTable( dbg );
        ReadFileTable();
    }
    return dbg;
}

dr_dbg_handle DRDbgInitNFT( void * file, unsigned long * sizes )
/***********************************************************/
{
    dr_dbg_handle       dbg;

    dbg = InitDbgHandle( file, sizes );
    if( dbg != NULL ){
        ReadAbbrevTable( dbg );
        ReadCompUnits();
    }
    return dbg;
}

void DRDbgFini( dr_dbg_handle dbg )
/**********************************/
/* don't have a way of deallocating virtual memory space.  Assume that any
 * pages that are allocated to this module will eventually be swapped out
*/
{
    compunit_info *     compunit;
    compunit_info *     next;

    compunit = dbg->compunit.next;
    while( compunit != NULL ) {
        next = compunit->next;
        DWRFiniFileTable( &compunit->filetab, FALSE );
        DWRFREE( compunit );
        compunit = next;
    }
    DWRFiniFileTable( &dbg->compunit.filetab, FALSE );
    if( dbg->abbrevs != NULL ){
        DWRFREE( dbg->abbrevs );
    }
    DWRFREE( dbg );
}

dr_dbg_handle  DRSetDebug( dr_dbg_handle dbg )
/**********************************/
{
    dr_dbg_handle  old;
    old = DWRCurrNode;
    DWRCurrNode = dbg;
    return( old );
}

dr_dbg_handle  DRGetDebug()
/*************************/
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

static void ReadFileTable( void )
/*******************************/
{
    compunit_info *     compunit;
    compunit_info *     next;
    dr_handle           start;
    dr_handle           finish;
    unsigned_32         length;
    unsigned_16         version;
    unsigned_8          addr_size;
    unsigned_8          curr_addr_size;

    compunit = &DWRCurrNode->compunit;
    start = DWRCurrNode->sections[DR_DEBUG_INFO].base;
    finish = start + DWRCurrNode->sections[DR_DEBUG_INFO].size;
    addr_size = DWRVMReadByte( start + 10 );
    for(;;) {
        compunit->next = NULL;
        compunit->start = start;
        length = DWRVMReadDWord( start );
        version = DWRVMReadWord( start + sizeof(unsigned_32) );
        if( version != DWARF_VERSION ) DWREXCEPT( DREXCEP_BAD_DBG_VERSION );
        curr_addr_size = DWRVMReadByte( start + 10 );
        if( curr_addr_size != addr_size ){
            addr_size = 0;
        }
        DWRInitFileTable( &compunit->filetab );
        DWRScanFileTable( start, &FileNameTable, &compunit->filetab );
        start += length + sizeof(unsigned_32);
        if( start >= finish ) break;
        next = DWRALLOC( sizeof( compunit_info ) );
        compunit->next = next;
        compunit = next;
    }
    DWRCurrNode->addr_size = addr_size;
}

static void ReadCompUnits( void )
/*******************************/
{
    compunit_info *     compunit;
    compunit_info *     next;
    dr_handle           start;
    dr_handle           finish;
    unsigned_32         length;
    unsigned_16         version;
    unsigned_8          addr_size;
    unsigned_8          curr_addr_size;

    compunit = &DWRCurrNode->compunit;
    start = DWRCurrNode->sections[DR_DEBUG_INFO].base;
    finish = start + DWRCurrNode->sections[DR_DEBUG_INFO].size;
    addr_size = DWRVMReadByte( start + 10 );
    for(;;) {
        compunit->next = NULL;
        compunit->start = start;
        length = DWRVMReadDWord( start );
        version = DWRVMReadWord( start + sizeof(unsigned_32) );
        if( version != DWARF_VERSION ) DWREXCEPT( DREXCEP_BAD_DBG_VERSION );
        curr_addr_size = DWRVMReadByte( start + 10 );
        if( curr_addr_size != addr_size ){
            addr_size = 0;
        }
        compunit->filetab.len  = 0;
        compunit->filetab.tab  = NULL;
        start += length + sizeof(unsigned_32);
        if( start >= finish ) break;
        next = DWRALLOC( sizeof( compunit_info ) );
        compunit->next = next;
        compunit = next;
    }
    DWRCurrNode->addr_size = addr_size;
}

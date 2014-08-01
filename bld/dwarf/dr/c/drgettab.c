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
* Description:  DWARF file table utility functions.
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"
#include <string.h>
#include "walloca.h"
#include "clibext.h"

// these are the increments in which the dynamically growing tables will
// grow. NOTE that the code relies on VBL_ARRAY_DELTA being a power of 2

#define VBL_ARRAY_DELTA     0x10
#define VBL_ARRAY_MASK      (VBL_ARRAY_DELTA - 1)

file_info               FileNameTable;

static bool GrabLineAddr( dr_handle abbrev, dr_handle mod, mod_scan_info *x,
                          void *_data )
/***************************************************************************/
/* this is called by ScanCompileUnit with abbrevptr and dataptr pointing at
 * the start of a compile unit die.  This picks out the line number info.
 * offset, and stores it in data */
{
    long    *data = _data;

    x = x;      // to avoid a warning
    if( DWRScanForAttrib( &abbrev, &mod, DW_AT_stmt_list ) ) {
        *data = DWRReadConstant( abbrev, mod );
    }
    return( FALSE );    // do not continue with the search.
}

extern void DWRInitFileTable( file_table *tab )
/*********************************************/
{
    tab->len = 0;
    tab->tab = DWRALLOC( VBL_ARRAY_DELTA * sizeof( filetab_entry ) );
}

extern void DWRFiniFileTable( file_table *tab, bool freenames )
/*************************************************************/
{
    file_tab_idx    ftidx;

    if( freenames ) {
        for( ftidx = 0; ftidx < tab->len; ftidx++ ) {
            DWRFREE( tab->tab[ftidx].name );
        }
    }
    if( tab->tab != NULL ) {
        DWRFREE( tab->tab );
    }
}

static void GrowTable( file_table *tab )
/**************************************/
{
    tab->len++;
    if( !(tab->len & VBL_ARRAY_MASK) ) {        // it will overflow
        tab->tab = DWRREALLOC( tab->tab,
                   (tab->len + VBL_ARRAY_DELTA) * sizeof(filetab_entry) );
    }
}

extern file_tab_idx DWRAddFileName( char *name, file_table *tab )
/***************************************************************/
{
    file_tab_idx    ftidx;
    char            **names;

    names = (char **)tab->tab;
    ftidx = 0;
    while( ftidx < tab->len ) {
        if( strcmp( name, *names ) == 0 ) {
            DWRFREE( name );
            return( ftidx );
        }
        ftidx++;
        names++;
    }
    GrowTable( tab );
    tab->tab[ftidx].name = name;
    return( ftidx );
}

static void DWRInsertIndex( file_tab_idx ftidx, file_table *tab, unsigned where )
/*******************************************************************************/
{
    if( where == TAB_IDX_FNAME ) {
        tab->tab[tab->len - 1].idx.fnameidx = ftidx;
    } else {
        tab->tab[tab->len - 1].idx.pathidx = ftidx;
    }
}

extern void DWRAddIndex( file_tab_idx ftidx, file_table *tab, unsigned where )
/****************************************************************************/
{
    GrowTable( tab );
    DWRInsertIndex( ftidx, tab, where );
}

extern file_tab_idx DWRIndexPath( file_tab_idx ftidx, file_table *tab )
/*********************************************************************/
{
    return( tab->tab[ftidx].idx.pathidx );
}

extern file_tab_idx DWRIndexFile( file_tab_idx ftidx, file_table *tab )
/*********************************************************************/
{
    return( tab->tab[ftidx].idx.fnameidx );
}

extern char * DWRIndexFileName( file_tab_idx ftidx, file_table *tab )
/*******************************************************************/
{
    return( tab->tab[ftidx].name );
}

static void DWRTrimTableSize( file_table *tab )
/*********************************************/
{
    tab->tab = DWRREALLOC( tab->tab, tab->len * sizeof(filetab_entry) );
}

static void ReadNameEntry( dr_handle *start, file_info *nametab,
                           file_table *idxtab, file_table *maptab )
/*****************************************************************/
{
    char            *name;
    file_tab_idx    ftidx;

    name = DWRCopyString( start );
    ftidx = DWRAddFileName( name, &nametab->fnametab );
    DWRAddIndex( ftidx, idxtab, TAB_IDX_FNAME );
    ftidx = DWRVMReadULEB128( start );
    ftidx = DWRIndexPath( ftidx, maptab );
    DWRInsertIndex( ftidx, idxtab, TAB_IDX_PATH );
    DWRVMSkipLEB128( start );   // skip time
    DWRVMSkipLEB128( start );   // skip length
}

extern void DWRScanFileTable( dr_handle start, file_info *nametab,
                              file_table *idxtab )
/****************************************************************/
// find the filenames in the line information, and return them in a table
{
    dr_handle       finish;
    file_table      curridxmap;
    char            *name;
    int             index;
    file_tab_idx    ftidx;
    unsigned        length;
    unsigned_8      *oparray;
    int             op_base;
    int             value;
    long            stmt_offset;

    stmt_offset = -1;
    DWRGetCompileUnitHdr( start, GrabLineAddr, &stmt_offset );
    if( stmt_offset == -1 ) {
        return;
    }
    start = DWRCurrNode->sections[DR_DEBUG_LINE].base;
    start += stmt_offset;
    finish = start + DWRVMReadDWord( start );
    start += 4 + 2 + 4 + 1 + 1 + 1 + 1;         // skip the start of the header
    op_base = DWRVMReadByte( start );
    oparray = __alloca( op_base - 1 );
    start++;
    for( index = 0; index < op_base - 1; index++ ) {
        oparray[index] = DWRVMReadByte( start );
        start++;
    }
    DWRInitFileTable( &curridxmap );
    while( start < finish ) {           // get directory table
        value = DWRVMReadByte( start );
        if( value == 0 ) break;
        name = DWRCopyString( &start );
        ftidx = DWRAddFileName( name, &nametab->pathtab );
        DWRAddIndex( ftidx, &curridxmap, TAB_IDX_PATH );
    }
    start++;
    while( start < finish ) {           // get filename table
        value = DWRVMReadByte( start );
        if( value == 0 ) break;
        ReadNameEntry( &start, nametab, idxtab, &curridxmap );
    }
    start++;
    while( start < finish ) {   // now go through the statement program
        value = DWRVMReadByte( start );
        start++;
        if( value == 0 ) {      // it's an extended opcode
            length = DWRVMReadULEB128( &start );
            value = DWRVMReadByte( start );
            if( value == DW_LNE_define_file ) {
                start++;
                ReadNameEntry( &start, nametab, idxtab, &curridxmap );
            } else {
                start += length;
            }
        } else if( value < op_base ) {  // it is a standard opcode
            if( value == DW_LNS_fixed_advance_pc ) {
                start += sizeof(unsigned_16);    // it is a fixed size
            } else {    // it is a variable # of blocks
                value = oparray[value-1];
                while( value > 0 ) {
                    DWRVMSkipLEB128( &start );
                    value--;
                }
            }
        }       // else it was a special op, and thus only 1 byte long
    }
    DWRTrimTableSize( idxtab );
    DWRFiniFileTable( &curridxmap, FALSE );
}

extern char * DWRFindFileName( unsigned fileno, dr_handle entry )
/***************************************************************/
{
    compunit_info   *compunit;
    file_tab_idx    ftidx;

    if( fileno != 0 ) {
        compunit = DWRFindCompileInfo( entry );
        ftidx = DWRIndexFile( fileno - 1, &compunit->filetab );
        return( DWRIndexFileName( ftidx, &FileNameTable.fnametab ) );
    }
    return( NULL );
}

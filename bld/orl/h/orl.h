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
* Description:  Object Reader Library public interface.
*
****************************************************************************/


#ifndef ORL_H
#define ORL_H

#include <stdio.h>
#include "orlglobl.h"
#include "orlcomon.h"

extern orl_handle           ORLENTRY ORLInit( orl_funcs *funcs );
extern orl_return           ORLENTRY ORLGetError( orl_handle );
extern orl_return           ORLENTRY ORLFini( orl_handle );

extern orl_file_format      ORLENTRY ORLFileIdentify( orl_handle, FILE * );
extern orl_file_handle      ORLENTRY ORLFileInit( orl_handle, FILE *, orl_file_format );
extern orl_return           ORLENTRY ORLFileFini( orl_file_handle );
extern orl_return           ORLENTRY ORLFileScan( orl_file_handle, const char *, orl_sec_return_func );
extern orl_machine_type     ORLENTRY ORLFileGetMachineType( orl_file_handle );
extern orl_file_flags       ORLENTRY ORLFileGetFlags( orl_file_handle );
extern orl_file_type        ORLENTRY ORLFileGetType( orl_file_handle );
extern orl_file_format      ORLENTRY ORLFileGetFormat( orl_file_handle );
extern orl_file_size        ORLENTRY ORLFileGetSize( orl_file_handle );
extern orl_sec_handle       ORLENTRY ORLFileGetSymbolTable( orl_file_handle );

extern const char           * ORLENTRY ORLSecGetName( orl_sec_handle );
extern orl_return           ORLENTRY ORLSecGetBase( orl_sec_handle, orl_sec_base * );
extern orl_sec_size         ORLENTRY ORLSecGetSize( orl_sec_handle );
extern orl_sec_type         ORLENTRY ORLSecGetType( orl_sec_handle );
extern orl_sec_flags        ORLENTRY ORLSecGetFlags( orl_sec_handle );
extern orl_sec_alignment    ORLENTRY ORLSecGetAlignment( orl_sec_handle );
extern orl_sec_handle       ORLENTRY ORLSecGetStringTable( orl_sec_handle );
extern orl_sec_handle       ORLENTRY ORLSecGetSymbolTable( orl_sec_handle );
extern orl_sec_handle       ORLENTRY ORLSecGetRelocTable( orl_sec_handle );
extern orl_linnum           ORLENTRY ORLSecGetLines( orl_sec_handle );
extern orl_table_index      ORLENTRY ORLSecGetNumLines( orl_sec_handle );
extern orl_sec_offset       ORLENTRY ORLSecGetOffset( orl_sec_handle );
extern orl_return           ORLENTRY ORLSecGetContents( orl_sec_handle, unsigned char ** );
extern orl_return           ORLENTRY ORLSecQueryReloc( orl_sec_handle, orl_sec_offset, orl_reloc_return_func );
extern orl_return           ORLENTRY ORLSecScanReloc( orl_sec_handle, orl_reloc_return_func );
extern orl_table_index      ORLENTRY ORLCvtSecHdlToIdx( orl_sec_handle );
extern orl_sec_handle       ORLENTRY ORLCvtIdxToSecHdl( orl_file_handle, orl_table_index );

extern const char           * ORLENTRY ORLSecGetClassName( orl_sec_handle );
extern orl_sec_combine      ORLENTRY ORLSecGetCombine( orl_sec_handle );
extern orl_sec_frame        ORLENTRY ORLSecGetAbsFrame( orl_sec_handle );
extern orl_sec_handle       ORLENTRY ORLSecGetAssociated( orl_sec_handle );
extern orl_group_handle     ORLENTRY ORLSecGetGroup( orl_sec_handle );

extern orl_return           ORLENTRY ORLRelocSecScan( orl_sec_handle, orl_reloc_return_func );
extern orl_return           ORLENTRY ORLSymbolSecScan( orl_sec_handle, orl_symbol_return_func );
extern orl_return           ORLENTRY ORLNoteSecScan( orl_sec_handle, orl_note_callbacks *, void * );

extern const char           * ORLENTRY ORLSymbolGetName( orl_symbol_handle );
extern orl_return           ORLENTRY ORLSymbolGetValue( orl_symbol_handle, orl_symbol_value * );
extern orl_symbol_binding   ORLENTRY ORLSymbolGetBinding( orl_symbol_handle );
extern orl_symbol_type      ORLENTRY ORLSymbolGetType( orl_symbol_handle );
extern unsigned char        ORLENTRY ORLSymbolGetRawInfo( orl_symbol_handle );
extern orl_sec_handle       ORLENTRY ORLSymbolGetSecHandle( orl_symbol_handle );
extern orl_symbol_handle    ORLENTRY ORLSymbolGetAssociated( orl_symbol_handle );

extern orl_return           ORLENTRY ORLGroupsScan( orl_file_handle, orl_group_return_func );
extern const char           * ORLENTRY ORLGroupName( orl_group_handle );
extern orl_table_index      ORLENTRY ORLGroupSize( orl_group_handle );
extern const char           * ORLENTRY ORLGroupMember( orl_group_handle, orl_table_index );

extern orl_rva              ORLENTRY ORLExportTableRVA( orl_file_handle );

#endif

/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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


#ifndef COFF_ORL_INCLUDED
#define COFF_ORL_INCLUDED

#include "cofftype.h"

extern coff_handle          COFFENTRY CoffInit( orl_funcs * );
extern orl_return           COFFENTRY CoffFini( coff_handle );

extern orl_return           COFFENTRY CoffFileInit( coff_handle, orl_io_handle, coff_file_handle * );
extern orl_return           COFFENTRY CoffFileFini( coff_file_handle );
extern orl_return           COFFENTRY CoffFileScan( coff_file_handle, const char *, orl_sec_return_func );
extern orl_machine_type     COFFENTRY CoffFileGetMachineType( coff_file_handle );
extern orl_file_flags       COFFENTRY CoffFileGetFlags( coff_file_handle );
extern orl_file_type        COFFENTRY CoffFileGetType( coff_file_handle );
extern orl_file_size        COFFENTRY CoffFileGetSize( coff_file_handle );
extern coff_sec_handle      COFFENTRY CoffFileGetSymbolTable( coff_file_handle );

extern const char           * COFFENTRY CoffSecGetName( coff_sec_handle );
extern coff_sec_base        COFFENTRY CoffSecGetBase( coff_sec_handle );
extern coff_sec_size        COFFENTRY CoffSecGetSize( coff_sec_handle );
extern orl_sec_type         COFFENTRY CoffSecGetType( coff_sec_handle );
extern orl_sec_flags        COFFENTRY CoffSecGetFlags( coff_sec_handle );
extern orl_sec_alignment    COFFENTRY CoffSecGetAlignment( coff_sec_handle );
extern coff_sec_handle      COFFENTRY CoffSecGetStringTable( coff_sec_handle );
extern coff_sec_handle      COFFENTRY CoffSecGetSymbolTable( coff_sec_handle );
extern coff_sec_handle      COFFENTRY CoffSecGetRelocTable( coff_sec_handle );
extern orl_linnum           COFFENTRY CoffSecGetLines( coff_sec_handle );
extern orl_table_index      COFFENTRY CoffSecGetNumLines( coff_sec_handle );
extern coff_sec_offset      COFFENTRY CoffSecGetOffset( coff_sec_handle );
extern orl_return           COFFENTRY CoffSecGetContents( coff_sec_handle, unsigned char ** );
extern orl_return           COFFENTRY CoffSecQueryReloc( coff_sec_handle, coff_sec_offset, orl_reloc_return_func );
extern orl_return           COFFENTRY CoffSecScanReloc( coff_sec_handle, orl_reloc_return_func );

extern orl_table_index      COFFENTRY CoffCvtSecHdlToIdx( coff_sec_handle );
extern coff_sec_handle      COFFENTRY CoffCvtIdxToSecHdl( coff_file_handle, orl_table_index );

extern orl_return           COFFENTRY CoffRelocSecScan( coff_sec_handle, orl_reloc_return_func );
extern orl_return           COFFENTRY CoffSymbolSecScan( coff_sec_handle, orl_symbol_return_func );
extern orl_return           COFFENTRY CoffNoteSecScan( coff_sec_handle, orl_note_callbacks *, void * );

extern const char           * COFFENTRY CoffSymbolGetName( coff_symbol_handle );
extern coff_symbol_value    COFFENTRY CoffSymbolGetValue( coff_symbol_handle );
extern orl_symbol_binding   COFFENTRY CoffSymbolGetBinding( coff_symbol_handle );
extern orl_symbol_type      COFFENTRY CoffSymbolGetType( coff_symbol_handle );
extern coff_sec_handle      COFFENTRY CoffSymbolGetSecHandle( coff_symbol_handle );
extern coff_symbol_handle   COFFENTRY CoffSymbolGetAssociated( coff_symbol_handle );

extern coff_rva             COFFENTRY CoffExportTableRVA( coff_file_handle );

#endif

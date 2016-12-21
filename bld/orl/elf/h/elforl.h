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


#ifndef ELF_ORL_INCLUDED
#define ELF_ORL_INCLUDED

#include "elftype.h"

elf_handle              ELFENTRY ElfInit( orl_funcs * );
orl_return              ELFENTRY ElfFini( elf_handle );

orl_return              ELFENTRY ElfFileInit( elf_handle, void *, elf_file_handle * );
orl_return              ELFENTRY ElfFileFini( elf_file_handle );
orl_return              ELFENTRY ElfFileScan( elf_file_handle, const char *, orl_sec_return_func );
orl_machine_type        ELFENTRY ElfFileGetMachineType( elf_file_handle );
orl_file_flags          ELFENTRY ElfFileGetFlags( elf_file_handle );
orl_file_type           ELFENTRY ElfFileGetType( elf_file_handle );
orl_file_size           ELFENTRY ElfFileGetSize( elf_file_handle );
elf_sec_handle          ELFENTRY ElfFileGetSymbolTable( elf_file_handle );

char *                  ELFENTRY ElfSecGetName( elf_sec_handle );
orl_sec_offset          ELFENTRY ElfSecGetBase( elf_sec_handle );
orl_sec_size            ELFENTRY ElfSecGetSize( elf_sec_handle );
orl_sec_type            ELFENTRY ElfSecGetType( elf_sec_handle );
orl_sec_flags           ELFENTRY ElfSecGetFlags( elf_sec_handle );
orl_sec_alignment       ELFENTRY ElfSecGetAlignment( elf_sec_handle );
elf_sec_handle          ELFENTRY ElfSecGetStringTable( elf_sec_handle );
elf_sec_handle          ELFENTRY ElfSecGetSymbolTable( elf_sec_handle );
elf_sec_handle          ELFENTRY ElfSecGetRelocTable( elf_sec_handle );
orl_sec_offset          ELFENTRY ElfSecGetNonLocalSymbolLoc( elf_sec_handle );
orl_return              ELFENTRY ElfSecGetContents( elf_sec_handle, unsigned char ** );
orl_return              ELFENTRY ElfSecQueryReloc( elf_sec_handle, elf_sec_offset, orl_reloc_return_func );
orl_return              ELFENTRY ElfSecScanReloc( elf_sec_handle, orl_reloc_return_func );

orl_table_index         ELFENTRY ElfCvtSecHdlToIdx( elf_sec_handle );
elf_sec_handle          ELFENTRY ElfCvtIdxToSecHdl( elf_file_handle,
                                                     orl_table_index );

orl_return              ELFENTRY ElfRelocSecScan( elf_sec_handle, orl_reloc_return_func );
orl_return              ELFENTRY ElfSymbolSecScan( elf_sec_handle, orl_symbol_return_func );
orl_return              ELFENTRY ElfNoteSecScan( elf_sec_handle, orl_note_callbacks *, void * );

char *                  ELFENTRY ElfSymbolGetName( elf_symbol_handle );
orl_symbol_value        ELFENTRY ElfSymbolGetValue( elf_symbol_handle );
orl_symbol_binding      ELFENTRY ElfSymbolGetBinding( elf_symbol_handle );
orl_symbol_type         ELFENTRY ElfSymbolGetType( elf_symbol_handle );
unsigned char           ELFENTRY ElfSymbolGetRawInfo( elf_symbol_handle );
elf_sec_handle          ELFENTRY ElfSymbolGetSecHandle( elf_symbol_handle );
elf_symbol_handle       ELFENTRY ElfSymbolGetAssociated( elf_symbol_handle elf_symbol_hnd );

#endif

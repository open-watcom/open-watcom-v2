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


#include "elfexe.h"

#define ELFENTRY

typedef enum {
    ELF_SECT_CODE       = 0x0001,       /* section contains executable code */
    ELF_SECT_ALLOC      = 0x0002,       /* section occupies memory at execution time */
    ELF_SECT_WRITE      = 0x0004,       /* section is writeable */
} elf_section_flags;

typedef enum {
    ELF_SYM_LOCAL       = 0x0000,       /* local to object file */
    ELF_SYM_GLOBAL      = 0x0001,       /* global symbol */
    ELF_SYM_WEAK        = 0x0002,       /* weak global symbol */
    ELF_SYM_EXPORT      = 0x0004,       /* symbol is exported */
    ELF_SYM_FUNCTION    = 0x0008,       /* symbol is a function */
} elf_sym_flags;

typedef enum {
    ELF_RELOC_HALF,                     /* lower-half of a word */
    ELF_RELOC_UPPER_HALF,               /* upper-half of a word */      /* wee... */
    ELF_RELOC_JUMP,                     /* 26-bit offset */             /* not so wee... */
    ELF_RELOC_WORD,                     /* entire word */               /* friggin' HUGE! */
} elf_reloc_type;

typedef Elf32_Addr      elf_offset;
typedef Elf32_Word      elf_size;

typedef struct {
    int                 (*write)( void *, const char *, int );
    long                (*tell)( void * );
    long                (*lseek)( void *, long int, int );
    void *              (*alloc)( size_t );
    void                (*free)( void * );
} elf_funcs;

typedef struct elf_info         *elf_handle;
typedef struct elf_file         *elf_file_handle;
typedef struct elf_section      *elf_section_handle;

elf_handle              ELFENTRY ELFInit( elf_funcs *funcs );
void                    ELFENTRY ELFFini( elf_handle handle );

elf_file_handle         ELFENTRY ELFFileInit( elf_handle handle, void *file_handle );
void                    ELFENTRY ELFFileFini( elf_file_handle file );

elf_section_handle      ELFENTRY ELFSectionInit( elf_file_handle file, const char *name, elf_section_flags flags );
void                    ELFENTRY ELFSectionFini( elf_section_handle section );

void                    ELFENTRY ELFEmitLabel( elf_section_handle sect, const char *name, elf_size size, elf_sym_flags flags );
void                    ELFENTRY ELFEmitData( elf_section_handle sect, const char *buffer, elf_size size );
void                    ELFENTRY ELFEmitReloc( elf_section_handle sect, elf_offset loc, const char *symbol, elf_reloc_type type );
elf_offset              ELFENTRY ELFTellOffset( elf_section_handle sect );

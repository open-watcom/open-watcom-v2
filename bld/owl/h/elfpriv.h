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


typedef struct elf_info elf_info;
typedef struct elf_file_info elf_file_inf;
typedef struct elf_section_info elf_section_info;

typedef void *client_file_handle;

typedef struct {
    uint_32             used;
    uint_32             size;
    const char          *buffer;
} elf_string_table;

typedef struct {
} elf_symbol_table;

struct elf_info {
    elf_funcs           *client_funcs;
    elf_file_info       *files;
};

struct elf_file_info {
    elf_file_info       *next;
    elf_info            *info;
    client_file_handle  file_handle;
    elf_section_flags   flags;
    elf_symbol_table    *symbols;
    elf_string_table    *strings;
    elf_section_info    *sections;
    elf_section_index   user_sections;
};

typedef uint_32 elf_section_index;

struct elf_section_info {
    elf_section_info    *next;
    elf_file_info       *file;
    elf_section_index   index;
};

// macros to hide client function usage give an elf_file_info pointer (a)

#define _ClientAlloc( a, b )            ((a)->info->client_funcs.alloc( b )
#define _ClientFree( a, b )             ((a)->info->client_funcs.free( b ))
#define _ClientWrite( a, b, c )         ((a)->info->client_funcs.write( (a)->file_handle, b, c ))
#define _ClientTell( a )                ((a)->info->client_funcs.tell( (a)->file_handle ))
#define _ClientSeek( a, b, c )          ((a)->info->client_funcs.lseek( (a)->file_handle, b, c ))

/*
 * We always lay out our elf file as follows:
 *              <file header>
 *              <section headers>
 *              <string table>
 *              <symbol table>
 *              <reloc information>
 *              <user section 1...>
 */

#define STRING_TABLE_INDEX              1
#define SYMBOL_TABLE_INDEX              2

#define ELF_MAGIC_NUMBER                0x7f

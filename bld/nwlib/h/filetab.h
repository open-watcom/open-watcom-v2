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
* Description:  File table interface.
*
****************************************************************************/


typedef struct sym_table_struct sym_table;

typedef struct sym_file_struct  sym_file;

typedef struct sym_entry_struct sym_entry;

typedef struct import_sym_struct import_sym;

typedef struct elf_import_sym_struct elf_import_sym;

struct sym_table_struct {
    sym_file    *first;
    sym_file    **add_to;
};

typedef enum {
    IMPORT_DESCRIPTOR,
    NULL_IMPORT_DESCRIPTOR,
    NULL_THUNK_DATA,
    ORDINAL,    // ordinal and name
    NAMED,      // name only
    ELF,        // name only or name and ordinal
    ELFRENAMED, // renamed entry,
}importType;

struct elf_import_sym_struct {
    char            *name;
    long            ordinal;
    long            len; // To save some calculations
    elf_import_sym  *next;
};

struct import_sym_struct{
    importType      type;
    processor_type  processor;
    char            *DLLName;
    union {
        struct {
            long        ordinal;
            char        *symName;
            char        *exportedName;
        } sym;
        struct {
            elf_import_sym  *symlist;
            long            numsyms;
        } elf;
    } u;
};

struct sym_file_struct {
    sym_file    *next;
    sym_entry   *first;
    sym_entry   **add_to;
    input_lib   *inlib;
    file_offset inlib_offset;
    file_offset new_offset;
    arch_header arch;
    int         index;
    long        name_offset;
    unsigned    name_length;
    unsigned    ffname_length;
    char        *full_name;
    import_sym  *import;
    file_type   obj_type;
};

typedef enum {
    SYM_WEAK    = 0, // WEAK must be < STRONG
    SYM_STRONG  = 1,
} symbol_strength;

struct sym_entry_struct {
    sym_entry           *next;
    sym_file            *file;
    sym_entry           *hash;
    unsigned short      len;
    unsigned char       info;
    symbol_strength     strength;
    char                name[1];
};

extern void InitFileTab( void );
extern void FiniFileTab( void );
extern void ResetFileTab( void );
extern void CleanFileTab( void );
extern void ListContents( void );
extern void AddObjectSymbols( arch_header *arch, libfile io, long offset );
extern bool RemoveObjectSymbols( char *name );
extern void SymCalcNewOffsets( void );
extern void WriteFileTable( void );
extern void WriteFileBody( sym_file *sfile );
extern void AddSym( const char *name, symbol_strength strength, unsigned char info );

#ifndef NDEBUG
extern void DumpFileTable( void );
extern void DumpHashTable( void );
#endif

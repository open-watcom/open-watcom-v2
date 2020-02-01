/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#ifdef _ELF

#include "strtab.h"

typedef struct {
    Elf32_Ehdr  eh;
    Elf32_Shdr  *strhdr;
    Elf32_Phdr  *ph;
    unsigned    ph_size;
    Elf32_Shdr  *sh;
    unsigned    sh_size;
    stringtable secstrtab;
    struct {
        unsigned    secstr;  // Index of strings section for section names
        unsigned    grpbase; // Index base for Groups in section
        unsigned    grpnum;  // Number of groups
        unsigned    relbase; // Index base for relocation sections
        unsigned    relnum;  // number of relocations
        unsigned    symstr;  // Index of symbol's string table
        unsigned    symtab;  // Index of symbol table
        unsigned    symhash; // Index of symbol hash table
        unsigned    dbgbegin;// Index of first debug section
        unsigned    dbgnum;  // Number of debug sections
    } i;  // Indexes into sh
    unsigned_32 curr_off;
} ElfHdr;

typedef struct {
    symbol **   table;
    unsigned    maxElems;          // Size of table
    unsigned    numElems;          // number of elements
    stringtable *strtab;
    Elf32_Word  *buckets;
    unsigned    numBuckets;
    Elf32_Word  *chains;
} ElfSymTable;

extern ElfSymTable *    CreateElfSymTable( unsigned maxElems, stringtable *strtab );
extern void             AddSymElfSymTable( ElfSymTable *tab, symbol *sym );
extern unsigned         FindSymIdxElfSymTable( ElfSymTable *tab, symbol *sym );
extern void             WriteElfSymTable( ElfSymTable *tab,
                                          ElfHdr *hdr,
                                          unsigned hashidx,
                                          unsigned symtabidx,
                                          unsigned strtabidx );
extern void             ZapElfSymTable( ElfSymTable *tab);
extern size_t           AddSecName( ElfHdr *hdr, const char *name );

#endif

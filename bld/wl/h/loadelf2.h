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


#include "strtab.h"

typedef struct {
    Elf32_Ehdr eh;
    Elf32_Shdr *strhdr;
    Elf32_Phdr *ph;
    unsigned   ph_size;
    Elf32_Shdr *sh;
    unsigned   sh_size;
    stringtable secstrtab;
    struct {
        int        secstr;  // Index of strings section for section names
        int        grpbase; // Index base for Groups in section
        int        grpnum;  // Number of groups
        int        relbase; // Index base for relocation sections
        int        relnum;  // number of relocations
        int        symstr;  // Index of symbol's string table
        int        symtab;  // Index of symbol table
        int        symhash; // Index of symbol hash table
        int        dbgbegin;// Index of first debug section
        int        dbgnum;  // Number of debug sections
    } i;  // Indexes into sh
    unsigned_32 curr_off;
} ElfHdr;

typedef struct {
    symbol **   table;
    int         maxElems;          // Size of table
    int         numElems;          // number of elements
    stringtable *strtab;
    Elf32_Word  *buckets;
    int         numBuckets;
    Elf32_Word  *chains;
} ElfSymTable;

extern ElfSymTable *    CreateElfSymTable( int maxElems, stringtable *strtab );
extern void             AddSymElfSymTable( ElfSymTable *tab, symbol *sym );
extern int              FindSymIdxElfSymTable( ElfSymTable *tab, symbol *sym );
extern void             WriteElfSymTable( ElfSymTable *tab,
                                          ElfHdr *hdr,
                                          int hashidx,
                                          int symtabidx,
                                          int strtabidx );
extern void             ZapElfSymTable( ElfSymTable *tab);
extern void             AddSecName( ElfHdr *hdr, Elf32_Shdr *sh, char *name );

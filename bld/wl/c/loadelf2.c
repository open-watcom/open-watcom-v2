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
* Description:  Additional routines for creating ELF load files.
*
****************************************************************************/


#include "linkstd.h"
#include "exeelf.h"
#include "loadelf.h"
#include "loadelf2.h"
#include "reloc.h"
#include "alloc.h"
#include "loadfile.h"
#include "wlnkmsg.h"
#include "msg.h"
#include "impexp.h"
#include "objcalc.h"
#include "toc.h"
#include "strtab.h"
#include <string.h>

static unsigned_8       PrimeNos[] = {
  2,      3,      5,      7,     11,     13,     17,     19,     23,     29,
 31,     37,     41,     43,     47,     53,     59,     61,     67,     71,
 73,     79,     83,     89,     97,    101,    103,    107,    109,    113,
127,    131,    137,    139,    149,    151,    157,    163,    167,    173,
179,    181,    191,    193,    197,    199,    211,    223,    227,    229,
233,    239,    241,    251,      0 /* terminator */
};

static unsigned FindClosestPrime( unsigned num )
/**********************************************/
{
    unsigned_8 * primetab;
    unsigned_8   prime;

    primetab = PrimeNos;
    do {
        prime = *primetab;
        if( prime > num ) break;
        primetab++;
    } while( *primetab > 0 );
    return prime;
}

static unsigned_32 ElfHash( char *sym_name )
/******************************************/
// stolen from the ELF spec.
{
    unsigned_32 h;
    unsigned_32 g;
    char        *name;

    h = 0;
    for( name = sym_name; *name != '\0'; ++name ) {
        h = (h << 4) + *(unsigned_8 *)name;
        g = h & 0xF0000000;
        if( g != 0 ) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

ElfSymTable *CreateElfSymTable( int maxElems, stringtable *strtab )
/************************************************************************/
{
    ElfSymTable *tab;

    _ChkAlloc( tab, sizeof(ElfSymTable) );
    tab->numBuckets = FindClosestPrime( maxElems / 2 );
    tab->maxElems = maxElems+1; // element 0 is NIL
    tab->numElems = 1;
    _ChkAlloc( tab->table, (tab->maxElems) * sizeof tab->table[0] );
    memset( tab->table, 0, (tab->maxElems) * sizeof tab->table[0] );
    _ChkAlloc( tab->buckets, tab->numBuckets * sizeof(Elf32_Word) );
    memset( tab->buckets, 0, tab->numBuckets * sizeof(unsigned_32) );
    _ChkAlloc( tab->chains, (tab->maxElems) * sizeof(unsigned_32) );
    memset( tab->chains, 0, (tab->maxElems) * sizeof(unsigned_32) );
    tab->strtab = strtab;
    if( GetStringTableSize( tab->strtab ) == 0 ) {
        AddCharStringTable( tab->strtab, '\0' );
    }
    return tab;
}

void AddSymElfSymTable( ElfSymTable *tab, symbol *sym )
/************************************************************/
{
    unsigned_32 hash;

    DbgAssert(tab->numElems < tab->maxElems);
    tab->table[tab->numElems] = sym;
    hash = ElfHash( sym->name ) % tab->numBuckets;
    tab->chains[tab->numElems] = tab->buckets[hash];
    tab->buckets[hash] = tab->numElems;
    tab->numElems++;
}

int FindSymIdxElfSymTable( ElfSymTable *tab, symbol *sym )
/***************************************************************/
{
    unsigned_32 hash;
    unsigned_32 idx;
    symbol *s;

    hash = ElfHash(sym->name) % tab->numBuckets;
    idx = tab->buckets[hash];
    DbgAssert(idx < tab->maxElems);
    for(s = tab->table[idx]; s != NULL && s != sym;
        idx = tab->chains[idx], s = tab->table[idx] ) {
        DbgAssert(idx < tab->maxElems);
    }
    return idx;
}

static unsigned_16 ElfSymSecNum( ElfHdr *hdr, symbol *sym, group_entry *group )
/*****************************************************************************/
{
    int secnum;

    secnum = hdr->i.grpbase;
    if( IsSymElfImported(sym) ) {
        return SHN_UNDEF;
    } else if( IS_SYM_COMMUNAL(sym) ) {
        return SHN_COMMON;
    } else if( group == NULL ) {
        return SHN_ABS;
    } else {
        secnum += group->grp_addr.seg-1;
    }
    DbgAssert(group != NULL);
    return secnum;
}

static void SetElfSym( ElfHdr *hdr, Elf32_Sym *elfsym, symbol *sym )
/******************************************************************/
{
    int         bind;
    int         type;
    group_entry *group;

    elfsym->st_value = SymbolAbsAddr(sym);
    elfsym->st_size = 0;
    bind = STB_GLOBAL;
    if( sym->info & SYM_STATIC ) {       // determine binding
        bind = STB_LOCAL;
    } else if( IS_SYM_REGULAR(sym) || IS_SYM_IMPORTED(sym) ||
               IS_SYM_ALIAS(sym) || IS_SYM_COMMUNAL(sym) ) {
    } else if( IS_SYM_WEAK_REF(sym) ) {
        bind = STB_WEAK;
    } else {
        DbgAssert(0);
    }
    if( IS_SYM_GROUP(sym) ) {
        type = STT_SECTION;
    } else {
        type = STT_NOTYPE;
    }
    elfsym->st_info = ELF32_ST_INFO(bind, type);
    elfsym->st_other = 0;
    group = SymbolGroup(sym);
    elfsym->st_shndx = ElfSymSecNum( hdr, sym, group );

}

void WriteElfSymTable( ElfSymTable *tab, ElfHdr *hdr, int hashidx,
                                int symtabidx, int strtabidx )
/***********************************************************************/
{
    int         i;
    long        off;
    unsigned    len;
    Elf32_Sym   elfsym;
    Elf32_Shdr  *hashSH;
    Elf32_Shdr  *tableSH;
    symbol      *sym;

    hashSH = hdr->sh + hashidx;
    tableSH = hdr->sh + symtabidx;

    // Table should be filled, though this condition is not necessary
    DbgAssert(tab->numElems == tab->maxElems);

    // write symbol section:
    tableSH->sh_info = 0;
    memset(&elfsym, 0, sizeof elfsym);
    elfsym.st_shndx = SHN_UNDEF;
    WriteLoad(&elfsym, sizeof elfsym);
    off = GetStringTableSize( tab->strtab );
    for( i = 1; i < tab->numElems; i++ ) {
        sym = tab->table[i];
        len = strlen( sym->name ) + 1;
        AddBufferStringTable( tab->strtab, sym->name, len );
        elfsym.st_name = off;
        off += len;
        if( tableSH->sh_info == 0 && (sym->info & SYM_STATIC) == 0 ) {
            tableSH->sh_info = i;
        }
        SetElfSym( hdr, &elfsym, sym );
        WriteLoad( &elfsym, sizeof(elfsym) );
    }
    len = tab->numElems * sizeof elfsym;
    tableSH->sh_offset = hdr->curr_off;
    tableSH->sh_size = len;
    tableSH->sh_entsize = sizeof elfsym;
    hdr->curr_off += len;

    // write hash section:
    len = (1 + 1 + tab->numBuckets + tab->numElems) * sizeof(unsigned_32);
    hashSH->sh_offset = hdr->curr_off;
    hashSH->sh_size = len;
    WriteLoad( &tab->numBuckets, sizeof(unsigned_32 ) );
    WriteLoad( &tab->numElems, sizeof(unsigned_32) );
    WriteLoad( tab->buckets, tab->numBuckets * sizeof(unsigned_32) );
    WriteLoad( tab->chains, tab->numElems * sizeof(unsigned_32) );
    hdr->curr_off += len;

    // set section headers:
    hashSH->sh_type = SHT_HASH;
    hashSH->sh_link = symtabidx;
    tableSH->sh_type = SHT_SYMTAB;
    tableSH->sh_link = strtabidx;
    AddSecName(hdr, hashSH, ".hash");
}

void ZapElfSymTable( ElfSymTable *tab )
/********************************************/
{
    _LnkFree( tab->table );
    _LnkFree( tab->buckets );
    _LnkFree( tab->chains );
    _LnkFree( tab );
}

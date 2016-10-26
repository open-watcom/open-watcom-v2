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
* Description:  Routines for creating ELF executable images.
*
****************************************************************************/


/*
-----------
Historical note - the ELF output support in wlink was initially developed
for IBM's OS/2 for PowerPC which used ELF, hence references to OS/2.
-----------

Layout of OS/2 ELF Executable:

----------------------------------------------------------------------------
Elf header
----------------------------------------------------------------------------
Program headers:
----------------------------------------------------------------------------
Groups data
----------------------------------------------------------------------------
Various symbol tables
----------------------------------------------------------------------------
Debug data (for dwarf)
----------------------------------------------------------------------------
Section headers:    - Empty unused section
                    - Section for each group in Groups
                    - Various symbol tables sections
                    - Relocations
----------------------------------------------------------------------------
String table for sections
----------------------------------------------------------------------------
*/

#include <string.h>
#include "walloca.h"
#include "linkstd.h"
#include "exeelf.h"
#include "loadelf.h"
#include "loadelf2.h"
#include "reloc.h"
#include "alloc.h"
#include "strtab.h"
#include "impexp.h"
#include "specials.h"
#include "loadfile.h"
#include "wlnkmsg.h"
#include "msg.h"
#include "virtmem.h"
#include "fileio.h"
#include "dbgcomm.h"
#include "dbgall.h"
#include "dbgdwarf.h"
#include "objcalc.h"

static stringtable      SymStrTab;
static ElfSymTable *    ElfSymTab;

/* Put debugging info into section WITHIN the file instead of appending a
 * separate elf file at the end */

#define INJECT_DEBUG ( SymFileName == NULL && (LinkFlags & DWARF_DBI_FLAG) )

static void AddSecIdxName( ElfHdr *hdr, int idx, char *name )
/***********************************************************/
{
    if( idx == 0 )
        return;
    AddSecName( hdr, hdr->sh+idx, name );
}


static void InitSections( ElfHdr *hdr)
/************************************/
{
    int         num;
    group_entry *group;

    num = 1;
    memset( &hdr->i, 0, sizeof hdr->i );
    hdr->i.secstr = num++;
    hdr->eh.e_shstrndx = hdr->i.secstr;
    hdr->i.grpbase = num;
    hdr->i.grpnum = NumGroups;
    if( FmtData.dgroupsplitseg != NULL ) {
        hdr->i.grpnum++;
        if( StackSegPtr != NULL ) {
            hdr->i.grpnum++;
        }
    }
    num += hdr->i.grpnum;
    hdr->i.relbase = num;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->g.grp_relocs != NULL ) {
            hdr->i.relnum++;
        }
    }
    num += hdr->i.relnum;
    hdr->i.symtab = num++;
    hdr->i.symstr = num++;
    hdr->i.symhash = num++;
    if( INJECT_DEBUG ) {
        hdr->i.dbgnum = DwarfCountDebugSections();
    }
    hdr->i.dbgbegin = num;
    num += hdr->i.dbgnum;
    num += FmtData.u.elf.extrasects;
    hdr->eh.e_shnum = num;
    hdr->sh_size = sizeof(Elf32_Shdr) * hdr->eh.e_shnum;
    _ChkAlloc( hdr->sh, hdr->sh_size );
    memset( hdr->sh, 0, hdr->sh_size );
    AddSecIdxName(hdr, hdr->i.symtab, ".symtab");
}

static void SetHeaders( ElfHdr *hdr )
/***********************************/
{
    memcpy( hdr->eh.e_ident, ELF_SIGNATURE, ELF_SIGNATURE_LEN );
    hdr->eh.e_ident[EI_CLASS] = ELFCLASS32;
#ifdef __BIG_ENDIAN__
    hdr->eh.e_ident[EI_DATA] = ELFDATA2MSB;
#else
    hdr->eh.e_ident[EI_DATA] = ELFDATA2LSB;
#endif
    hdr->eh.e_ident[EI_VERSION] = EV_CURRENT;
    hdr->eh.e_ident[EI_OSABI] = FmtData.u.elf.abitype;
    hdr->eh.e_ident[EI_ABIVERSION] = FmtData.u.elf.abiversion;
    memset( &hdr->eh.e_ident[EI_PAD], 0, EI_NIDENT - EI_PAD );
    hdr->eh.e_type = ET_EXEC;
    if( LinkState & HAVE_PPC_CODE ) {
        hdr->eh.e_machine = EM_PPC;
    } else if( LinkState & HAVE_MIPS_CODE ) {
        hdr->eh.e_machine = EM_MIPS;
    } else if( LinkState & HAVE_X64_CODE ) {
        hdr->eh.e_machine = EM_X86_64;
    } else {
        hdr->eh.e_machine = EM_386;
    }
    hdr->eh.e_version = EV_CURRENT;
    if( StartInfo.type == START_UNDEFED ) {
        hdr->eh.e_entry = 0;
    } else {
        hdr->eh.e_entry = FindLinearAddr2( &StartInfo.addr );
    }
    hdr->eh.e_flags = 0;
    hdr->eh.e_ehsize = sizeof(Elf32_Ehdr);
    hdr->eh.e_phentsize = sizeof(Elf32_Phdr);
    hdr->eh.e_shentsize = sizeof(Elf32_Shdr);
    hdr->eh.e_phnum = NumGroups + 1;
    hdr->eh.e_phoff = sizeof(Elf32_Ehdr);
    hdr->ph_size = sizeof(Elf32_Phdr) * hdr->eh.e_phnum;
    _ChkAlloc( hdr->ph, hdr->ph_size );
    hdr->ph->p_type = PT_PHDR;
    hdr->ph->p_offset = sizeof(Elf32_Ehdr);
    hdr->ph->p_vaddr = sizeof(Elf32_Ehdr) + FmtData.base;
    hdr->ph->p_paddr = 0;
    hdr->ph->p_filesz = hdr->ph_size;
    hdr->ph->p_memsz = hdr->ph_size;
    hdr->ph->p_flags = PF_R | PF_X;
    hdr->ph->p_align = 0;
    InitStringTable( &hdr->secstrtab, false );
    AddCharStringTable( &hdr->secstrtab, '\0' );
    InitSections( hdr );
    hdr->curr_off = hdr->eh.e_ehsize + hdr->ph_size;
    hdr->curr_off = ROUND_UP( hdr->curr_off, 0x100 );
    SeekLoad( hdr->curr_off );
}

unsigned GetElfHeaderSize( void )
/**************************************/
{
    unsigned    size;

    size = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) * (NumGroups + 1);
    return ROUND_UP( size, 0x100 );
}

void AddSecName( ElfHdr *hdr, Elf32_Shdr *sh, char *name )
/***************************************************************/
{
    sh->sh_name = GetStringTableSize( &hdr->secstrtab );
    AddStringStringTable( &hdr->secstrtab, name );
}


static void WriteSHStrings( ElfHdr *hdr, int str_idx, stringtable *strtab )
/*************************************************************************/
{
    Elf32_Shdr *sh;

    sh = hdr->sh+str_idx;
    AddSecName(hdr, sh, ".shstrtab");

    sh->sh_offset = hdr->curr_off;
    sh->sh_type = SHT_STRTAB;
    sh->sh_size = GetStringTableSize( strtab );
    hdr->curr_off += sh->sh_size;
    WriteStringTable( strtab, WriteLoad3, NULL );
}

static void SetGroupHeaders( group_entry *group, offset off, Elf32_Phdr *ph,
                             Elf32_Shdr *sh )
/**************************************************************************/
{
    sh->sh_type = SHT_PROGBITS;
    sh->sh_flags = SHF_ALLOC;
    ph->p_flags = PF_R;
    if( group->segflags & SEG_DATA ) {
        sh->sh_flags |= SHF_WRITE;
        ph->p_flags |= PF_W;
    } else { // if code group
        sh->sh_flags |= SHF_EXECINSTR;
        ph->p_flags |= PF_X;
    }
    sh->sh_addr = ph->p_vaddr = group->linear + FmtData.base;
    ph->p_type = PT_LOAD;
    ph->p_filesz = sh->sh_size = group->size;
    ph->p_memsz = group->totalsize;
    if( group == DataGroup && StackSegPtr != NULL ) {
        ph->p_memsz -= StackSize;
    }
    sh->sh_link = SHN_UNDEF;
    sh->sh_info = 0;
    sh->sh_addralign = 4;
    sh->sh_entsize = 0;
    ph->p_offset = sh->sh_offset = off;
    ph->p_paddr = 0;
    ph->p_align = FmtData.objalign;
}

static void InitBSSSect( Elf32_Shdr *sh, offset off, offset size, offset start )
/******************************************************************************/
{
    sh->sh_type = SHT_NOBITS;
    sh->sh_flags = SHF_ALLOC | SHF_WRITE;
    sh->sh_addr = start;
    sh->sh_offset = off;
    sh->sh_size = size;
    sh->sh_link = SHN_UNDEF;
    sh->sh_info = 0;
    sh->sh_addralign = 4;
    sh->sh_entsize = 0;
}

static char * GroupSecName( group_entry *group )
/**********************************************/
{
    if( group->segflags & SEG_DATA ) {
        return ".data";
    } else {
        return ".text";
    }
}

static void WriteELFGroups( ElfHdr *hdr )
/***************************************/
{
    group_entry *group;
    Elf32_Shdr *sh;
    Elf32_Phdr *ph;
    offset      off;
    offset      linear;

    sh = hdr->sh + hdr->i.grpbase;
    ph = hdr->ph + 1;
    off = hdr->curr_off;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->totalsize == 0 ) continue;   // DANGER DANGER DANGER <--!!!
        SetGroupHeaders( group, off, ph, sh );
        WriteGroupLoad( group );
        off = OffsetAlign( off + group->size, FmtData.objalign );
        AddSecName( hdr, sh, GroupSecName(group) );
        sh++;
        if( group == DataGroup && FmtData.dgroupsplitseg != NULL ) {
            AddSecName( hdr, sh, ".bss" );
            linear = ph->p_vaddr + ROUND_UP( ph->p_filesz, FmtData.objalign );
            InitBSSSect( sh, off, CalcSplitSize(), linear );
            linear = ROUND_UP( linear + sh->sh_size, FmtData.objalign);
            sh++;
            if( StackSegPtr != NULL ) {
                AddSecName( hdr, sh, ".stack" );
                InitBSSSect( sh, off, StackSize, linear );
                sh++;
            }
        }
        ph++;
    }
    hdr->curr_off = off;
}

#define RELA_NAME_SIZE sizeof(RelASecName)

static void SetRelocSectName( ElfHdr *hdr, Elf32_Shdr *sh, char *secname )
/************************************************************************/
{
    size_t      len;
    char        *name;

    len = strlen( secname );
    name = alloca( RELA_NAME_SIZE + len );
    memcpy( name, RelASecName, RELA_NAME_SIZE - 1 );
    memcpy( name + RELA_NAME_SIZE - 1, secname, len + 1 );
    AddSecName( hdr, sh, name );
}


static void WriteRelocsSections( ElfHdr *hdr )
/********************************************/
{
    group_entry *group;
    int         currgrp;
    Elf32_Shdr  *sh;
    void        *relocs;
    char        *secname;

    currgrp = hdr->i.grpbase;
    sh = hdr->sh + hdr->i.relbase;
    for( group = Groups; group != NULL; group = group->next_group ) {
        relocs = group->g.grp_relocs;
        if( relocs != NULL ) {
            sh->sh_offset = hdr->curr_off;
            sh->sh_entsize = sizeof(elf_reloc_item);
            sh->sh_type = SHT_RELA;
            sh->sh_flags = SHF_ALLOC;
            sh->sh_addr = 0;
            sh->sh_link = hdr->i.symtab;
            sh->sh_info = currgrp;
            sh->sh_addralign = 4;
            sh->sh_size = RelocSize( relocs );
            secname = GroupSecName( group );
            SetRelocSectName( hdr, sh, secname );
            DumpRelocList( relocs );
            hdr->curr_off += sh->sh_size;
            sh++;
        }
        currgrp++;
    }
}

void FiniELFLoadFile( void )
/*********************************/
{
    ElfHdr      hdr;

    SetHeaders( &hdr );
#if 0
    if( (LinkState & HAVE_PPC_CODE) && (FmtData.type & MK_OS2) ) {
        // Development temporarly on hold
        // BuildOS2Imports(); // Build .got section
    }
#endif

    WriteELFGroups( &hdr ); // Write out all groups
    WriteRelocsSections( &hdr );        // Relocations
    if( INJECT_DEBUG ) {                // Debug info
        hdr.curr_off = DwarfWriteElf( hdr.curr_off, &hdr.secstrtab,
                                hdr.sh+hdr.i.dbgbegin );
    }
    if( ElfSymTab != NULL ) {           // Symbol tables
        WriteElfSymTable( ElfSymTab, &hdr, hdr.i.symhash, hdr.i.symtab,
                          hdr.i.symstr);
        ZapElfSymTable( ElfSymTab );
    }
    if( hdr.i.symstr != 0 ) {           // String sections
        WriteSHStrings( &hdr, hdr.i.symstr, &SymStrTab );
    }
    WriteSHStrings( &hdr, hdr.i.secstr, &hdr.secstrtab );
    hdr.eh.e_shoff = hdr.curr_off;
    WriteLoad( hdr.sh, hdr.sh_size );
    hdr.curr_off += hdr.sh_size;
    if( !INJECT_DEBUG ) {
        DBIWrite();
    }
    SeekLoad( 0 );
    WriteLoad( &hdr.eh, sizeof(Elf32_Ehdr) );
    WriteLoad( hdr.ph, hdr.ph_size );
    _LnkFree( hdr.sh );
    _LnkFree( hdr.ph );
    FiniStringTable( &hdr.secstrtab );
    FiniStringTable( &SymStrTab );
    SeekLoad( hdr.curr_off );
}

void ChkElfData( void )
/****************************/
{
    group_entry *group;
    symbol *    sym;

    NumExports = NumImports = 0;
    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IsSymElfImported(sym) ) {
            NumImports++;
        } else if( IsSymElfExported(sym) ) {
            if( (sym->info & SYM_DEFINED) == 0 ) {
                LnkMsg( ERR+MSG_EXP_SYM_NOT_FOUND, "s", sym->name );
            }
            NumExports++;
        }
    }
    InitStringTable( &SymStrTab, false );
    AddCharStringTable( &SymStrTab, '\0' );
    ElfSymTab = CreateElfSymTable( NumImports + NumExports + NumGroups,
                                   &SymStrTab);
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->totalsize != 0 ) {
            AddSymElfSymTable( ElfSymTab, group->sym );
        }
    }
    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IsSymElfImpExp(sym) ) {
            AddSymElfSymTable(ElfSymTab, sym);
        }
    }

}

int FindElfSymIdx( symbol *sym )
/*************************************/
{
    return FindSymIdxElfSymTable( ElfSymTab, sym );
}


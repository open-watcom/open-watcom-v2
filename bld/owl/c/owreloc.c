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


/* Functions for CPU specific relocations */

#include "owlpriv.h"
#include "owreloc.h"

// These correspond to owl_reloc_type
static uint_32 coffRelocTypesPPC[] = {
    IMAGE_REL_PPC_ABSOLUTE,
    IMAGE_REL_PPC_ADDR32,
    IMAGE_REL_PPC_ADDR16,       // Hi: correct?
    IMAGE_REL_PPC_ADDR16,       // Pair: needed?
    IMAGE_REL_PPC_ADDR16,       // Lo: correct?
    IMAGE_REL_PPC_REL14,
    IMAGE_REL_PPC_ADDR14,
    IMAGE_REL_PPC_REL24,
    IMAGE_REL_PPC_ADDR24,
    IMAGE_REL_PPC_SECTION,
    IMAGE_REL_PPC_SECREL,
    IMAGE_REL_PPC_TOCREL16,
    IMAGE_REL_PPC_IFGLUE,
};

static uint_32 coffRelocTypesAlpha[] = {
    IMAGE_REL_ALPHA_ABSOLUTE,
    IMAGE_REL_ALPHA_REFLONG,
    IMAGE_REL_ALPHA_REFHI,
    IMAGE_REL_ALPHA_PAIR,
    IMAGE_REL_ALPHA_REFLO,
    IMAGE_REL_ALPHA_BRADDR,
    0,                          // unused
    IMAGE_REL_ALPHA_HINT,
    0,                          // unused
    IMAGE_REL_ALPHA_SECTION,
    IMAGE_REL_ALPHA_SECREL,
    0,
    0,
};

static Elf32_Word elfRelocTypesPPC[] = {
    R_PPC_NONE,
    R_PPC_ADDR32,
    R_PPC_ADDR16_HI,
    R_PPC_NONE,
    R_PPC_ADDR16_LO,
    R_PPC_REL14,
    R_PPC_ADDR14,
    R_PPC_REL24,
    R_PPC_ADDR24,
    R_PPC_NONE,
    R_PPC_NONE,
    R_PPC_GOT16,
    R_PPC_NONE,
};

// Someone should really make these up...
static Elf32_Word elfRelocTypesAlpha[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

Elf32_Word OWLENTRY ElfRelocType( owl_reloc_type reloc_type, owl_cpu cpu ) {
//**************************************************************************

    Elf32_Word  *elf_relocs;

    switch( cpu ) {
    case OWL_CPU_PPC:
        elf_relocs = elfRelocTypesPPC;
        break;
    case OWL_CPU_ALPHA:
        elf_relocs = elfRelocTypesAlpha;
        break;
    default:
        assert( 0 );
    }
    return( elf_relocs[ reloc_type ] );
}

uint_32 OWLENTRY CoffRelocType( owl_reloc_type reloc_type, owl_cpu cpu ) {
//************************************************************************

    uint_32     *coff_relocs;

    switch( cpu ) {
    case OWL_CPU_PPC:
        coff_relocs = coffRelocTypesPPC;
        break;
    case OWL_CPU_ALPHA:
        coff_relocs = coffRelocTypesAlpha;
        break;
    default:
        assert( 0 );
    }
    return( coff_relocs[ reloc_type ] );
}

static unsigned ppcMasks[] = {
    0xffffffff,         /* OWL_RELOC_ABSOLUTE */
    0xffffffff,         /* OWL_RELOC_WORD */
    0x0000ffff,         /* OWL_RELOC_HALF_HI */
    0x00000000,         /* OWL_RELOC_PAIR */
    0x0000ffff,         /* OWL_RELOC_HALF_LO */
    0x0000fffc,         /* OWL_RELOC_BRANCH_REL */
    0x0000fffc,         /* OWL_RELOC_BRANCH_ABS */
    0x03fffffc,         /* OWL_RELOC_JUMP_REL */
    0x03fffffc,         /* OWL_RELOC_JUMP_ABS */
    0x0000ffff,         /* OWL_RELOC_SECTION_INDEX */
    0xffffffff,         /* OWL_RELOC_SECTION_OFFSET */
    0x0000ffff,         /* OWL_RELOC_TOC_OFFSET */
    0xffffffff,         /* OWL_RELOC_GLUE */
};

static unsigned alphaMasks[] = {
    0xffffffff,         /* OWL_RELOC_ABSOLUTE */
    0xffffffff,         /* OWL_RELOC_WORD */
    0x0000ffff,         /* OWL_RELOC_HALF_HI */
    0x00000000,         /* OWL_RELOC_PAIR */
    0x0000ffff,         /* OWL_RELOC_HALF_LO */
    0x001fffff,         /* OWL_RELOC_BRANCH_REL */
    0x001fffff,         /* OWL_RELOC_BRANCH_ABS, unused */
    0x00003fff,         /* OWL_RELOC_JUMP_REL */
    0x00003fff,         /* OWL_RELOC_JUMP_ABS, unused */
    0x0000ffff,         /* OWL_RELOC_SECTION_INDEX */
    0xffffffff,         /* OWL_RELOC_SECTION_OFFSET */
    0x0000ffff,         /* OWL_RELOC_TOC_OFFSET */
    0xffffffff,         /* OWL_RELOC_GLUE */
};

unsigned OWLENTRY OWLRelocBitMask( owl_file_handle file, owl_reloc_info *reloc ) {

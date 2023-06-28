/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Functions for CPU specific relocations.
*
****************************************************************************/


#include "owlpriv.h"
#include "owreloc.h"

#define PPC_RELOCS                          /* coff                         elf                 mask */ \
/* OWL_RELOC_ABSOLUTE */        PPC_RELOC( COFF_IMAGE_REL_PPC_ABSOLUTE, R_PPC_NONE,         0xffffffff ) \
/* OWL_RELOC_WORD */            PPC_RELOC( COFF_IMAGE_REL_PPC_ADDR32,   R_PPC_ADDR32,       0xffffffff ) \
/* OWL_RELOC_HALF_HI */         PPC_RELOC( COFF_IMAGE_REL_PPC_ADDR16,   R_PPC_ADDR16_HI,    0x0000ffff ) \
/* OWL_RELOC_HALF_HA */         PPC_RELOC( COFF_IMAGE_REL_PPC_ADDR16,   R_PPC_ADDR16_HA,    0x0000ffff ) \
/* OWL_RELOC_PAIR */            PPC_RELOC( COFF_IMAGE_REL_PPC_ADDR16,   R_PPC_NONE,         0x00000000 ) \
/* OWL_RELOC_HALF_LO */         PPC_RELOC( COFF_IMAGE_REL_PPC_ADDR16,   R_PPC_ADDR16_LO,    0x0000ffff ) \
/* OWL_RELOC_BRANCH_REL */      PPC_RELOC( COFF_IMAGE_REL_PPC_REL14,    R_PPC_REL14,        0x0000fffc ) \
/* OWL_RELOC_BRANCH_ABS */      PPC_RELOC( COFF_IMAGE_REL_PPC_ADDR14,   R_PPC_ADDR14,       0x0000fffc ) \
/* OWL_RELOC_JUMP_REL */        PPC_RELOC( COFF_IMAGE_REL_PPC_REL24,    R_PPC_REL24,        0x03fffffc ) \
/* OWL_RELOC_JUMP_ABS */        PPC_RELOC( COFF_IMAGE_REL_PPC_ADDR24,   R_PPC_ADDR24,       0x03fffffc ) \
/* OWL_RELOC_SECTION_INDEX */   PPC_RELOC( COFF_IMAGE_REL_PPC_SECTION,  R_PPC_NONE,         0x0000ffff ) \
/* OWL_RELOC_SECTION_OFFSET */  PPC_RELOC( COFF_IMAGE_REL_PPC_SECREL,   R_PPC_NONE,         0xffffffff ) \
/* OWL_RELOC_TOC_OFFSET */      PPC_RELOC( COFF_IMAGE_REL_PPC_TOCREL16, R_PPC_GOT16,        0x0000ffff ) \
/* OWL_RELOC_GLUE */            PPC_RELOC( COFF_IMAGE_REL_PPC_IFGLUE,   R_PPC_NONE,         0xffffffff )

#define MIPS_RELOCS                         /* coff                         elf                 mask */ \
/* OWL_RELOC_ABSOLUTE */        MIPS_RELOC( COFF_IMAGE_REL_MIPS_ABSOLUTE,   R_MIPS_NONE,    0xffffffff ) \
/* OWL_RELOC_WORD */            MIPS_RELOC( COFF_IMAGE_REL_MIPS_REFWORD,    R_MIPS_32,      0xffffffff ) \
/* OWL_RELOC_HALF_HI */         MIPS_RELOC( COFF_IMAGE_REL_MIPS_REFHI,      R_MIPS_HI16,    0x0000ffff ) \
/* OWL_RELOC_HALF_HA */         MIPS_RELOC( 0,                              R_MIPS_NONE,    0x0000ffff ) \
/* OWL_RELOC_PAIR */            MIPS_RELOC( COFF_IMAGE_REL_MIPS_PAIR,       R_MIPS_NONE,    0x00000000 ) \
/* OWL_RELOC_HALF_LO */         MIPS_RELOC( COFF_IMAGE_REL_MIPS_REFLO,      R_MIPS_LO16,    0x0000ffff ) \
/* OWL_RELOC_BRANCH_REL */      MIPS_RELOC( COFF_IMAGE_REL_MIPS_REFLO,      R_MIPS_PC16,    0x0000ffff ) \
/* OWL_RELOC_BRANCH_ABS */      MIPS_RELOC( 0,                              R_MIPS_NONE,    0x0000ffff ) \
/* OWL_RELOC_JUMP_REL */        MIPS_RELOC( 0,                              R_MIPS_NONE,    0x03ffffff ) \
/* OWL_RELOC_JUMP_ABS */        MIPS_RELOC( COFF_IMAGE_REL_MIPS_JMPADDR,    R_MIPS_26,      0x03ffffff ) \
/* OWL_RELOC_SECTION_INDEX */   MIPS_RELOC( COFF_IMAGE_REL_MIPS_SECTION,    R_MIPS_NONE,    0x0000ffff ) \
/* OWL_RELOC_SECTION_OFFSET */  MIPS_RELOC( COFF_IMAGE_REL_MIPS_SECREL,     R_MIPS_NONE,    0xffffffff ) \
/* OWL_RELOC_TOC_OFFSET */      MIPS_RELOC( COFF_IMAGE_REL_MIPS_GPREL,      R_MIPS_GOT16,   0x0000ffff ) \
/* OWL_RELOC_GLUE */            MIPS_RELOC( 0,                              R_MIPS_NONE,    0xffffffff )

#define ALPHA_RELOCS                        /* coff                         elf                 mask */ \
/* OWL_RELOC_ABSOLUTE */        ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_ABSOLUTE, R_ALPHA_NONE,       0xffffffff ) \
/* OWL_RELOC_WORD */            ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_REFLONG,  R_ALPHA_REFLONG,    0xffffffff ) \
/* OWL_RELOC_HALF_HI */         ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_REFHI,    R_ALPHA_GPRELHIGH,  0x0000ffff ) \
/* OWL_RELOC_HALF_HA */         ALPHA_RELOC( 0,                             R_ALPHA_NONE,       0x0000ffff ) \
/* OWL_RELOC_PAIR */            ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_PAIR,     R_ALPHA_NONE,       0x00000000 ) \
/* OWL_RELOC_HALF_LO */         ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_REFLO,    R_ALPHA_GPRELLOW,   0x0000ffff ) \
/* OWL_RELOC_BRANCH_REL */      ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_BRADDR,   R_ALPHA_BRADDR,     0x001fffff ) \
/* OWL_RELOC_BRANCH_ABS */      ALPHA_RELOC( 0,                             R_ALPHA_NONE,       0x001fffff ) \
/* OWL_RELOC_JUMP_REL */        ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_HINT,     R_ALPHA_SREL16,     0x00003fff ) \
/* OWL_RELOC_JUMP_ABS */        ALPHA_RELOC( 0,                             R_ALPHA_NONE,       0x00003fff ) \
/* OWL_RELOC_SECTION_INDEX */   ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_SECTION,  R_ALPHA_NONE,       0x0000ffff ) \
/* OWL_RELOC_SECTION_OFFSET */  ALPHA_RELOC( COFF_IMAGE_REL_ALPHA_SECREL,   R_ALPHA_NONE,       0xffffffff ) \
/* OWL_RELOC_TOC_OFFSET */      ALPHA_RELOC( 0,                             R_ALPHA_GPREL16,    0x0000ffff ) \
/* OWL_RELOC_GLUE */            ALPHA_RELOC( 0,                             R_ALPHA_NONE,       0xffffffff )

#define I386_RELOCS                         /* coff                         elf */ \
/* OWL_RELOC_ABSOLUTE */        I386_RELOC( COFF_IMAGE_REL_I386_ABSOLUTE,   R_386_NONE ) \
/* OWL_RELOC_WORD */            I386_RELOC( COFF_IMAGE_REL_I386_DIR32,      R_386_32 ) \
/* OWL_RELOC_HALF_HI */         I386_RELOC( 0,                              R_386_NONE ) \
/* OWL_RELOC_HALF_HA */         I386_RELOC( 0,                              R_386_NONE ) \
/* OWL_RELOC_PAIR */            I386_RELOC( 0,                              R_386_NONE ) \
/* OWL_RELOC_HALF_LO */         I386_RELOC( 0,                              R_386_NONE ) \
/* OWL_RELOC_BRANCH_REL */      I386_RELOC( COFF_IMAGE_REL_I386_REL32,      R_386_PC32 ) \
/* OWL_RELOC_BRANCH_ABS */      I386_RELOC( 0,                              R_386_NONE ) \
/* OWL_RELOC_JUMP_REL */        I386_RELOC( 0,                              R_386_NONE ) \
/* OWL_RELOC_JUMP_ABS */        I386_RELOC( 0,                              R_386_NONE ) \
/* OWL_RELOC_SECTION_INDEX */   I386_RELOC( COFF_IMAGE_REL_I386_SECTION,    R_386_32 ) \
/* OWL_RELOC_SECTION_OFFSET */  I386_RELOC( COFF_IMAGE_REL_I386_SECREL,     R_386_32 ) \
/* OWL_RELOC_TOC_OFFSET */      I386_RELOC( 0,                              R_386_GOT32 ) \
/* OWL_RELOC_GLUE */            I386_RELOC( 0,                              R_386_NONE )

// These correspond to owl_reloc_type
static uint_32 coffRelocTypesPPC[] = {
    #define PPC_RELOC(coff,elf,mask)    coff,
    PPC_RELOCS
    #undef PPC_RELOC
};

static uint_32 coffRelocTypesMIPS[] = {
    #define MIPS_RELOC(coff,elf,mask)   coff,
    MIPS_RELOCS
    #undef MIPS_RELOC
};

static uint_32 coffRelocTypesAlpha[] = {
    #define ALPHA_RELOC(coff,elf,mask)  coff,
    ALPHA_RELOCS
    #undef ALPHA_RELOC
};

static uint_32 coffRelocTypes386[] = {
    #define I386_RELOC(coff,elf)        coff,
    I386_RELOCS
    #undef I386_RELOC
};

static Elf32_Word elfRelocTypesPPC[] = {
    #define PPC_RELOC(coff,elf,mask)    elf,
    PPC_RELOCS
    #undef PPC_RELOC
};

static Elf32_Word elfRelocTypesMIPS[] = {
    #define MIPS_RELOC(coff,elf,mask)    elf,
    MIPS_RELOCS
    #undef MIPS_RELOC
};

// Someone should really make these up...
static Elf32_Word elfRelocTypesAlpha[] = {
    #define ALPHA_RELOC(coff,elf,mask)  elf,
    ALPHA_RELOCS
    #undef ALPHA_RELOC
};

static Elf32_Word elfRelocTypes386[] = {
    #define I386_RELOC(coff,elf)        elf,
    I386_RELOCS
    #undef I386_RELOC
};

Elf32_Word OWLENTRY ElfRelocType( owl_reloc_type reloc_type, owl_cpu cpu ) {
//**************************************************************************

    Elf32_Word  *elf_relocs = NULL;

    switch( cpu ) {
    case OWL_CPU_PPC:
        elf_relocs = elfRelocTypesPPC;
        break;
    case OWL_CPU_MIPS:
        elf_relocs = elfRelocTypesMIPS;
        break;
    case OWL_CPU_ALPHA:
        elf_relocs = elfRelocTypesAlpha;
        break;
    case OWL_CPU_X86:
        elf_relocs = elfRelocTypes386;
        break;
    default:
        assert( 0 );
    }
    return( elf_relocs[ reloc_type ] );
}

uint_32 OWLENTRY CoffRelocType( owl_reloc_type reloc_type, owl_cpu cpu ) {
//************************************************************************

    uint_32     *coff_relocs = NULL;

    switch( cpu ) {
    case OWL_CPU_PPC:
        coff_relocs = coffRelocTypesPPC;
        break;
    case OWL_CPU_MIPS:
        coff_relocs = coffRelocTypesMIPS;
        break;
    case OWL_CPU_ALPHA:
        coff_relocs = coffRelocTypesAlpha;
        break;
    case OWL_CPU_X86:
        coff_relocs = coffRelocTypes386;
        break;
    default:
        assert( 0 );
    }
    return( coff_relocs[ reloc_type ] );
}

static unsigned ppcMasks[] = {
    #define PPC_RELOC(coff,elf,mask)    mask,
    PPC_RELOCS
    #undef PPC_RELOC
};

static unsigned mipsMasks[] = {
    #define MIPS_RELOC(coff,elf,mask)    mask,
    MIPS_RELOCS
    #undef MIPS_RELOC
};

static unsigned alphaMasks[] = {
    #define ALPHA_RELOC(coff,elf,mask)  mask,
    ALPHA_RELOCS
    #undef ALPHA_RELOC
};

unsigned OWLENTRY OWLRelocBitMask( owl_file_handle file, owl_reloc_info *reloc ) {
//********************************************************************************

    unsigned    *mask_array = NULL;

    assert( reloc != NULL );
    switch( file->info->cpu ) {
    case OWL_CPU_PPC:
        mask_array = ppcMasks;
        break;
    case OWL_CPU_ALPHA:
        mask_array = alphaMasks;
        break;
    case OWL_CPU_X86:
        return( 0xffffffff );
    case OWL_CPU_MIPS:
        mask_array = mipsMasks;
        break;
    default:
        assert( 0 );
    }
    return( mask_array[ reloc->type ] );
}

owl_offset OWLENTRY OWLRelocTargetDisp( owl_section_handle section, owl_offset from, owl_offset to ) {
//****************************************************************************************************

    owl_offset  ret;
    owl_cpu     cpu;

    cpu = section->file->info->cpu;
    if( cpu == OWL_CPU_ALPHA || cpu == OWL_CPU_X86 || cpu == OWL_CPU_MIPS ) {
        from += 4;  // Intel, Alpha and MIPS use updated PC
    } // PowerPC uses current PC
    if( cpu != OWL_CPU_X86 ) {    // no alignment restrictions for Intel
        assert( ( to % 4 ) == 0 );
        assert( ( from % 4 ) == 0 );
    }
    ret = to - from;
    if( cpu == OWL_CPU_PPC || cpu == OWL_CPU_X86 ) {
        return( ret );
    }
    return( ret >> 2 ); // Alpha and MIPS chop off the low two bits
}

uint_8 OWLENTRY OWLRelocIsRelative( owl_file_handle file, owl_reloc_info *reloc ) {
//*********************************************************************************

    /* unused parameters */ (void)file;

    return( reloc->type == OWL_RELOC_BRANCH_REL || reloc->type == OWL_RELOC_JUMP_REL );
}

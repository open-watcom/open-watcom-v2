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
* Description:  Functions for CPU specific relocations.
*
****************************************************************************/


#include "owlpriv.h"
#include "owreloc.h"

// These correspond to owl_reloc_type
static uint_32 coffRelocTypesPPC[] = {
    IMAGE_REL_PPC_ABSOLUTE,     /* OWL_RELOC_ABSOLUTE */
    IMAGE_REL_PPC_ADDR32,       /* OWL_RELOC_WORD */
    IMAGE_REL_PPC_ADDR16,       /* OWL_RELOC_HALF_HI */            // Hi: correct?
    IMAGE_REL_PPC_ADDR16,       /* OWL_RELOC_HALF_HA */
    IMAGE_REL_PPC_ADDR16,       /* OWL_RELOC_PAIR */               // Pair: needed?
    IMAGE_REL_PPC_ADDR16,       /* OWL_RELOC_HALF_LO */            // Lo: correct?
    IMAGE_REL_PPC_REL14,        /* OWL_RELOC_BRANCH_REL */
    IMAGE_REL_PPC_ADDR14,       /* OWL_RELOC_BRANCH_ABS, unused */
    IMAGE_REL_PPC_REL24,        /* OWL_RELOC_JUMP_REL */
    IMAGE_REL_PPC_ADDR24,       /* OWL_RELOC_JUMP_ABS, unused */
    IMAGE_REL_PPC_SECTION,      /* OWL_RELOC_SECTION_INDEX */
    IMAGE_REL_PPC_SECREL,       /* OWL_RELOC_SECTION_OFFSET */
    IMAGE_REL_PPC_TOCREL16,     /* OWL_RELOC_TOC_OFFSET */
    IMAGE_REL_PPC_IFGLUE,       /* OWL_RELOC_GLUE */
};

static uint_32 coffRelocTypesMIPS[] = {
    IMAGE_REL_MIPS_ABSOLUTE,    /* OWL_RELOC_ABSOLUTE */
    IMAGE_REL_MIPS_REFWORD,     /* OWL_RELOC_WORD */
    IMAGE_REL_MIPS_REFHI,       /* OWL_RELOC_HALF_HI */
    0,                          /* OWL_RELOC_HALF_HA, unused */
    IMAGE_REL_MIPS_PAIR,        /* OWL_RELOC_PAIR */
    IMAGE_REL_MIPS_REFLO,       /* OWL_RELOC_HALF_LO */
    IMAGE_REL_MIPS_REFLO,       /* OWL_RELOC_BRANCH_REL */  // Is this right??
    0,                          /* OWL_RELOC_BRANCH_ABS, unused */
    0,                          /* OWL_RELOC_JUMP_REL, unused */
    IMAGE_REL_MIPS_JMPADDR,     /* OWL_RELOC_JUMP_ABS */
    IMAGE_REL_MIPS_SECTION,     /* OWL_RELOC_SECTION_INDEX */
    IMAGE_REL_MIPS_SECREL,      /* OWL_RELOC_SECTION_OFFSET */
    IMAGE_REL_MIPS_GPREL,       /* OWL_RELOC_TOC_OFFSET */
    0,                          /* OWL_RELOC_GLUE, unused */
};

static uint_32 coffRelocTypesAlpha[] = {
    IMAGE_REL_ALPHA_ABSOLUTE,   /* OWL_RELOC_ABSOLUTE */
    IMAGE_REL_ALPHA_REFLONG,    /* OWL_RELOC_WORD */
    IMAGE_REL_ALPHA_REFHI,      /* OWL_RELOC_HALF_HI */
    0,                          /* OWL_RELOC_HALF_HA */
    IMAGE_REL_ALPHA_PAIR,       /* OWL_RELOC_PAIR */
    IMAGE_REL_ALPHA_REFLO,      /* OWL_RELOC_HALF_LO */
    IMAGE_REL_ALPHA_BRADDR,     /* OWL_RELOC_BRANCH_REL */
    0,                          /* OWL_RELOC_BRANCH_ABS */
    IMAGE_REL_ALPHA_HINT,       /* OWL_RELOC_JUMP_REL */
    0,                          /* OWL_RELOC_JUMP_ABS */
    IMAGE_REL_ALPHA_SECTION,    /* OWL_RELOC_SECTION_INDEX */
    IMAGE_REL_ALPHA_SECREL,     /* OWL_RELOC_SECTION_OFFSET */
    0,                          /* OWL_RELOC_TOC_OFFSET */
    0,                          /* OWL_RELOC_GLUE */
};

static uint_32 coffRelocTypes386[] = {
    IMAGE_REL_I386_ABSOLUTE,    /* OWL_RELOC_ABSOLUTE */
    IMAGE_REL_I386_DIR32,       /* OWL_RELOC_WORD */
    0,                          /* OWL_RELOC_HALF_HI */
    0,                          /* OWL_RELOC_HALF_HA */
    0,                          /* OWL_RELOC_PAIR */
    0,                          /* OWL_RELOC_HALF_LO */
    IMAGE_REL_I386_REL32,       /* OWL_RELOC_BRANCH_REL */
    0,                          /* OWL_RELOC_BRANCH_ABS */
    0,                          /* OWL_RELOC_JUMP_REL */
    0,                          /* OWL_RELOC_JUMP_ABS */
    IMAGE_REL_I386_SECTION,     /* OWL_RELOC_SECTION_INDEX */
    IMAGE_REL_I386_SECREL,      /* OWL_RELOC_SECTION_OFFSET */
    0,                          /* OWL_RELOC_TOC_OFFSET */
    0                           /* OWL_RELOC_GLUE */
};

static Elf32_Word elfRelocTypesPPC[] = {
    R_PPC_NONE,                 /* OWL_RELOC_ABSOLUTE */
    R_PPC_ADDR32,               /* OWL_RELOC_WORD */
    R_PPC_ADDR16_HI,            /* OWL_RELOC_HALF_HI */
    R_PPC_ADDR16_HA,            /* OWL_RELOC_HALF_HA */
    R_PPC_NONE,                 /* OWL_RELOC_PAIR */
    R_PPC_ADDR16_LO,            /* OWL_RELOC_HALF_LO */
    R_PPC_REL14,                /* OWL_RELOC_BRANCH_REL */
    R_PPC_ADDR14,               /* OWL_RELOC_BRANCH_ABS */
    R_PPC_REL24,                /* OWL_RELOC_JUMP_REL */
    R_PPC_ADDR24,               /* OWL_RELOC_JUMP_ABS */
    R_PPC_NONE,                 /* OWL_RELOC_SECTION_INDEX */
    R_PPC_NONE,                 /* OWL_RELOC_SECTION_OFFSET */
    R_PPC_GOT16,                /* OWL_RELOC_TOC_OFFSET */
    R_PPC_NONE,                 /* OWL_RELOC_GLUE */
};

static Elf32_Word elfRelocTypesMIPS[] = {
    R_MIPS_NONE,                /* OWL_RELOC_ABSOLUTE */
    R_MIPS_32,                  /* OWL_RELOC_WORD */
    R_MIPS_HI16,                /* OWL_RELOC_HALF_HI */
    R_MIPS_NONE,                /* OWL_RELOC_HALF_HA */
    R_MIPS_NONE,                /* OWL_RELOC_PAIR */
    R_MIPS_LO16,                /* OWL_RELOC_HALF_LO */
    R_MIPS_PC16,                /* OWL_RELOC_BRANCH_REL */
    R_MIPS_NONE,                /* OWL_RELOC_BRANCH_ABS */
    R_MIPS_NONE,                /* OWL_RELOC_JUMP_REL */
    R_MIPS_26,                  /* OWL_RELOC_JUMP_ABS */
    R_MIPS_NONE,                /* OWL_RELOC_SECTION_INDEX */
    R_MIPS_NONE,                /* OWL_RELOC_SECTION_OFFSET */
    R_MIPS_GOT16,               /* OWL_RELOC_TOC_OFFSET */
    R_MIPS_NONE,                /* OWL_RELOC_GLUE */
};

// Someone should really make these up...
static Elf32_Word elfRelocTypesAlpha[] = {
    R_ALPHA_NONE,               /* OWL_RELOC_ABSOLUTE */
    R_ALPHA_REFLONG,            /* OWL_RELOC_WORD */
    R_ALPHA_GPRELHIGH,          /* OWL_RELOC_HALF_HI */
    R_ALPHA_NONE,               /* OWL_RELOC_HALF_HA */
    R_ALPHA_NONE,               /* OWL_RELOC_PAIR */
    R_ALPHA_GPRELLOW,           /* OWL_RELOC_HALF_LO */
    R_ALPHA_BRADDR,             /* OWL_RELOC_BRANCH_REL */
    R_ALPHA_NONE,               /* OWL_RELOC_BRANCH_ABS */
    R_ALPHA_SREL16,             /* OWL_RELOC_JUMP_REL */
    R_ALPHA_NONE,               /* OWL_RELOC_JUMP_ABS */
    R_ALPHA_NONE,               /* OWL_RELOC_SECTION_INDEX */
    R_ALPHA_NONE,               /* OWL_RELOC_SECTION_OFFSET */
    R_ALPHA_GPREL16,            /* OWL_RELOC_TOC_OFFSET */
    R_ALPHA_NONE,               /* OWL_RELOC_GLUE */
};

static Elf32_Word elfRelocTypes386[] = {
    R_386_NONE,                 /* OWL_RELOC_ABSOLUTE */
    R_386_32,                   /* OWL_RELOC_WORD */
    R_386_NONE,                 /* OWL_RELOC_HALF_HI */
    R_386_NONE,                 /* OWL_RELOC_HALF_HA */
    R_386_NONE,                 /* OWL_RELOC_PAIR */
    R_386_NONE,                 /* OWL_RELOC_HALF_LO */
    R_386_PC32,                 /* OWL_RELOC_BRANCH_REL */
    R_386_NONE,                 /* OWL_RELOC_BRANCH_ABS, unused */
    R_386_NONE,                 /* OWL_RELOC_JUMP_REL */
    R_386_NONE,                 /* OWL_RELOC_JUMP_ABS, unused */
    R_386_32,                   /* OWL_RELOC_SECTION_INDEX */
    R_386_32,                   /* OWL_RELOC_SECTION_OFFSET */
    R_386_GOT32,                /* OWL_RELOC_TOC_OFFSET */
    R_386_NONE,                 /* OWL_RELOC_GLUE */
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
    case OWL_CPU_INTEL:
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
    case OWL_CPU_INTEL:
        coff_relocs = coffRelocTypes386;
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
    0x0000ffff,         /* OWL_RELOC_HALF_HA */
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

static unsigned mipsMasks[] = {
    0xffffffff,         /* OWL_RELOC_ABSOLUTE */
    0xffffffff,         /* OWL_RELOC_WORD */
    0x0000ffff,         /* OWL_RELOC_HALF_HI */
    0x0000ffff,         /* OWL_RELOC_HALF_HA */
    0x00000000,         /* OWL_RELOC_PAIR */
    0x0000ffff,         /* OWL_RELOC_HALF_LO */
    0x0000ffff,         /* OWL_RELOC_BRANCH_REL */
    0x0000ffff,         /* OWL_RELOC_BRANCH_ABS */
    0x03ffffff,         /* OWL_RELOC_JUMP_REL */
    0x03ffffff,         /* OWL_RELOC_JUMP_ABS */
    0x0000ffff,         /* OWL_RELOC_SECTION_INDEX */
    0xffffffff,         /* OWL_RELOC_SECTION_OFFSET */
    0x0000ffff,         /* OWL_RELOC_TOC_OFFSET */
    0xffffffff,         /* OWL_RELOC_GLUE */
};

static unsigned alphaMasks[] = {
    0xffffffff,         /* OWL_RELOC_ABSOLUTE */
    0xffffffff,         /* OWL_RELOC_WORD */
    0x0000ffff,         /* OWL_RELOC_HALF_HI */
    0x0000ffff,         /* OWL_RELOC_HALF_HA */
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
    case OWL_CPU_INTEL:
        return 0xffffffff;
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
    if( cpu == OWL_CPU_ALPHA || cpu == OWL_CPU_INTEL || cpu == OWL_CPU_MIPS ) {
        from += 4;  // Intel, Alpha and MIPS use updated PC
    } // PowerPC uses current PC
    if( cpu != OWL_CPU_INTEL ) {    // no alignment restrictions for Intel
        assert( ( to % 4 ) == 0 );
        assert( ( from % 4 ) == 0 );
    }
    ret = to - from;
    if( cpu == OWL_CPU_PPC || cpu == OWL_CPU_INTEL ) {
        return( ret );
    }
    return( ret >> 2 ); // Alpha and MIPS chop off the low two bits
}

uint_8 OWLENTRY OWLRelocIsRelative( owl_file_handle file, owl_reloc_info *reloc ) {
//*********************************************************************************

    file = file;
    return( reloc->type == OWL_RELOC_BRANCH_REL || reloc->type == OWL_RELOC_JUMP_REL );
}

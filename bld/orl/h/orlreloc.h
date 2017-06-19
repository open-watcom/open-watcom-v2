pick( ORL_RELOC_TYPE_NONE,        "NONE"      ) // error type
pick( ORL_RELOC_TYPE_ABSOLUTE,    "ABSOLUTE"  ) // ref to a 32-bit absolute address
pick( ORL_RELOC_TYPE_WORD_16,     "WORD16"    ) // a direct ref to a 16-bit address
pick( ORL_RELOC_TYPE_WORD_32,     "WORD32"    ) // a direct ref to a 32-bit address
pick( ORL_RELOC_TYPE_WORD_32_NB,  "WORD32NB"  ) // a direct ref to a 32-bit address (no base added)
pick( ORL_RELOC_TYPE_HALF_HI,     "HALFHI"    ) // ref to high half of 32-bit address
pick( ORL_RELOC_TYPE_HALF_HA,     "HALFHA"    ) // ref to high half of 32-bit address adjusted for signed low half
pick( ORL_RELOC_TYPE_HALF_LO,     "HALFLO"    ) // ref to low half of 32-bit address
pick( ORL_RELOC_TYPE_PAIR,        "PAIR"      ) // reloc connecting a HALF_HI and HALF_LO
pick( ORL_RELOC_TYPE_JUMP,        "JUMP"      ) // ref to the part of a 32-bit address valid for jump
pick( ORL_RELOC_TYPE_SECTION,     "SECTION"   ) // ref to an offset from a section address
pick( ORL_RELOC_TYPE_SEC_REL,     "SECREL"    ) // direct ref to a 32-bit address relative to the image base
pick( ORL_RELOC_TYPE_REL_16,      "REL16"     ) // relative reference to 16-bit address
pick( ORL_RELOC_TYPE_REL_21_SH,   "REL21SH"   ) // relative ref. to a 21-bit address shifted 2
pick( ORL_RELOC_TYPE_WORD_64,     "WORD64"    ) // NYI: direct ref to a 64-bit address
pick( ORL_RELOC_TYPE_SEGMENT,     "SEGMENT"   ) // 16-bit segment relocation
pick( ORL_RELOC_TYPE_WORD_14,     "WORD14"    ) // a direct ref to a 14-bit address shifted 2
pick( ORL_RELOC_TYPE_WORD_24,     "WORD24"    ) // a direct ref to a 24-bit address shifted 2
pick( ORL_RELOC_TYPE_WORD_26,     "WORD26"    ) // a direct ref to a 28-bit address shifted 2
pick( ORL_RELOC_TYPE_REL_14,      "REL14"     ) // relative ref to a 14-bit address shifted 2
pick( ORL_RELOC_TYPE_REL_24,      "REL24"     ) // relative ref to a 24-bit address shifted 2
pick( ORL_RELOC_TYPE_REL_32,      "REL32"     ) // relative ref to a 32-bit address
pick( ORL_RELOC_TYPE_REL_32_NOADJ, "REL32NOA" ) // relative ref to a 32-bit address without -4 adjustment

pick( ORL_RELOC_TYPE_TOCREL_16,   "TOCREL16"  ) // relative ref to 16-bit offset from TOC base.
pick( ORL_RELOC_TYPE_TOCREL_14,   "TOCREL14"  ) // relative ref to a 14-bit offset from TOC base shl 2
pick( ORL_RELOC_TYPE_TOCVREL_16,  "TOCVREL16" ) // like TOCREL16, data explicitly defined in .tocd
pick( ORL_RELOC_TYPE_TOCVREL_14,  "TOCVREL14" ) // like TOCREL14, data explicitly defined in .tocd
pick( ORL_RELOC_TYPE_GOT_32,      "GOT32"     ) // direct ref to 32-bit offset from GOT base.
pick( ORL_RELOC_TYPE_GOT_16,      "GOT16"     ) // direct ref to 16-bit offset from GOT base.
pick( ORL_RELOC_TYPE_GOT_16_HI,   "GOT16HI"   ) // direct ref to hi 16-bits of offset from GOT base.
pick( ORL_RELOC_TYPE_GOT_16_HA,   "GOT16HA"   ) // ditto adjusted for signed low 16-bits
pick( ORL_RELOC_TYPE_GOT_16_LO,   "GOT16LO"   ) // direct ref to lo 16-bits of offset from GOT base.
pick( ORL_RELOC_TYPE_PLTREL_24,   "PLTREL24"  ) // relative ref to 24-bit offset from PLT base
pick( ORL_RELOC_TYPE_PLTREL_32,   "PLTREL32"  ) // relative ref to 32-bit offset from PLT base
pick( ORL_RELOC_TYPE_PLT_32,      "PLT32"     ) // direct ref to 32-bit offset from PLT base.
pick( ORL_RELOC_TYPE_PLT_16_HI,   "PLT16HI"   ) // direct ref to hi 16-bits of offset from PLT base.
pick( ORL_RELOC_TYPE_PLT_16_HA,   "PLT16HA"   ) // ditto adjusted for signed low 16-bits.
pick( ORL_RELOC_TYPE_PLT_16_LO,   "PLT16LO"   ) // direct ref to lo 16-bits of offset from PLT base.
pick( ORL_RELOC_TYPE_IFGLUE,      "IFGLUE"    ) // Substitute TOC restore instruction iff symbol is glue code
pick( ORL_RELOC_TYPE_IMGLUE,      "IMGLUE"    ) // symbol is glue code; virtual address is TOC restore instruction

    // OMF specific relocations
pick( ORL_RELOC_TYPE_WORD_8,      "OFS8LO"    ) // low byte of 16-bit offset
pick( ORL_RELOC_TYPE_REL_8,       "REL8LO"    ) // low byte of 16-bit relative reference
pick( ORL_RELOC_TYPE_WORD_16_SEG, "OFS16SEG"  ) // 16:16 segment:offset
pick( ORL_RELOC_TYPE_REL_16_SEG,  "REL16SEG"  ) // relative reference 16:16 segment:offset
pick( ORL_RELOC_TYPE_WORD_HI_8,   "OFS8HI"    ) // high byte of 16-bit offset
pick( ORL_RELOC_TYPE_REL_HI_8,    "REL8HI"    ) // high byte of 16-bit relative reference
pick( ORL_RELOC_TYPE_WORD_32_SEG, "OFS32SEG"  ) // 16:32 segment:offset
pick( ORL_RELOC_TYPE_REL_32_SEG,  "REL32SEG"  ) // relative reference 16:32 segment:offset

    // special relocations for x64 coff files, there are only used in ndisasm yet!
    // for more information see watcom/h/coff.h in amd64 section
pick( ORL_RELOC_TYPE_REL_32_ADJ1, "REL32ADJ1" ) // relative ref to a 32-bit address, need special adjustment
pick( ORL_RELOC_TYPE_REL_32_ADJ2, "REL32ADJ2" ) // relative ref to a 32-bit address, need special adjustment
pick( ORL_RELOC_TYPE_REL_32_ADJ3, "REL32ADJ3" ) // relative ref to a 32-bit address, need special adjustment
pick( ORL_RELOC_TYPE_REL_32_ADJ4, "REL32ADJ4" ) // relative ref to a 32-bit address, need special adjustment
pick( ORL_RELOC_TYPE_REL_32_ADJ5, "REL32ADJ5" ) // relative ref to a 32-bit address, need special adjustment

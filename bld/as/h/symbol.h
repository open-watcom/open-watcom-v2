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
* Description:  Assembler symbol table interface.
*
****************************************************************************/


typedef enum {
    SYM_INSTRUCTION,
    SYM_DIRECTIVE,
    SYM_LABEL,
} sym_class;

typedef void    *sym_section;
typedef void    *sym_obj_hdl;
typedef uint_32 sym_offset;
typedef void    *sym_link;

typedef enum {
    SF_EXTERN           = 0x00,         // it has been declared as extern (default)
    SF_LOC_KNOWN        = 0x01,         // location and section are known (we have seen definition)
    SF_SECTION          = 0x02,         // it is a section symbol
} sym_flags;

typedef enum {
    SL_UNKNOWN,
    SL_GLOBAL,
    SL_STATIC,
} sym_linkage;

typedef struct {
    sym_section         section;        // section handle from the OWL library
    sym_offset          offset;         // offset into the section
} sym_location;

typedef struct sym_reloc_entry *sym_reloc;

struct sym_reloc_entry {
    sym_location        location;       // where the reloc should be emitted
    sym_reloc           prev;
    sym_reloc           next;
    bool                named;          // is it a named symbol
};

struct asm_symbol;
typedef struct asm_symbol *sym_handle;

typedef struct asm_symbol {
    sym_handle          next;
    sym_class           class;
    sym_flags           flags;
    sym_linkage         linkage;        // for SYM_LABEL class only
    sym_reloc           hi_relocs;      // LABEL only: all the hi and lo reloc
    sym_reloc           lo_relocs;      //  references that hasn't been paired
    sym_obj_hdl         obj_hdl;        // for SYM_LABEL class, a obj handle
    union {
        sym_location    location;       // valid for class == SYM_LABEL
        sym_link        link;           // valid for class == SYM_INSTRUCTION || class == SYM_DIRECTIVE
    } u;
    char                name[1];
} asm_symbol;

typedef enum {
    ASM_RELOC_UNSPECIFIED,
    ASM_RELOC_WORD,
    ASM_RELOC_HALF_HI,
    ASM_RELOC_HALF_HA,                  // high half adjusted for signed addition
    ASM_RELOC_HALF_LO,
    ASM_RELOC_JUMP,                     // j^ reloc
    ASM_RELOC_BRANCH,                   // PPC: 14-bit reloc; Alpha: jump hint
} asm_reloc_type;


extern void             AsSymInit( void );
extern sym_handle       AsSymLookup( const char *sym );
extern sym_handle       AsSymAdd( const char *sym, sym_class class );
#ifndef _STANDALONE_
extern sym_handle       AsSymSetLocation( sym_handle, sym_location );
extern sym_location     AsSymGetLocation( sym_handle );
extern bool             AsSymLocationKnown( sym_handle );
#endif
#ifdef _STANDALONE_
extern void             AsSymSetSection( sym_handle );
extern bool             AsSymIsSectionSymbol( sym_handle );
#endif
extern sym_handle       AsSymSetLink( sym_handle, sym_link );
extern sym_link         AsSymGetLink( sym_handle );
extern sym_class        AsSymClass( sym_handle );
extern sym_linkage      AsSymGetLinkage( sym_handle );
extern sym_handle       AsSymSetLinkage( sym_handle, sym_linkage );
extern char *           AsSymName( sym_handle );
extern void             AsSymStackReloc( bool is_high, sym_handle, sym_section, sym_offset, bool );
extern void             AsSymDestroyReloc( sym_handle, sym_reloc );
extern sym_reloc        AsSymMatchReloc( bool is_high, sym_handle, sym_section );
extern sym_reloc        AsSymGetReloc( bool is_high, sym_handle *get_hdl );
#ifdef AS_DEBUG_DUMP
extern bool             AsSymRelocIsClean( bool is_clean );
#endif
extern sym_obj_hdl      AsSymObjHandle( sym_handle );
extern void             AsSymFini( void );

#ifdef _STANDALONE_
#ifdef AS_DEBUG_DUMP
extern void             DumpSymbolTable( void );
#endif
#endif

#define SymInit                         AsSymInit
#define SymLookup                       AsSymLookup
#define SymAdd                          AsSymAdd
#ifndef _STANDALONE_
#define SymSetLocation                  AsSymSetLocation
#define SymGetLocation                  AsSymGetLocation
#define SymLocationKnown                AsSymLocationKnown
#endif
#ifdef _STANDALONE_
#define SymSetSection                   AsSymSetSection
#define SymIsSectionSymbol              AsSymIsSectionSymbol
#endif
#define SymSetLink                      AsSymSetLink
#define SymGetLink                      AsSymGetLink
#define SymClass                        AsSymClass
#define SymGetLinkage                   AsSymGetLinkage
#define SymSetLinkage                   AsSymSetLinkage
#define SymName                         AsSymName
#define SymStackReloc                   AsSymStackReloc
#define SymDestroyReloc                 AsSymDestroyReloc
#define SymMatchReloc                   AsSymMatchReloc
#define SymGetReloc                     AsSymGetReloc
#ifdef AS_DEBUG_DUMP
#define SymRelocIsClean                 AsSymRelocIsClean
#endif
#define SymObjHandle                    AsSymObjHandle
#define SymFini                         AsSymFini

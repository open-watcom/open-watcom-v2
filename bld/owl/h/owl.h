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
* Description:  Object Writer Library public interface.
*
****************************************************************************/


#ifndef OWL_H_INCLUDED
#define OWL_H_INCLUDED

#define OWLENTRY

#include "watcom.h"
#include <stdio.h>

typedef enum {
    OWL_SYM_UNDEFINED,
    OWL_SYM_FUNCTION,
    OWL_SYM_STATIC,
    OWL_SYM_GLOBAL,
    OWL_SYM_WEAK,
} owl_sym_linkage;

#define _OwlLinkageGlobal( x )  ( (x) == OWL_SYM_GLOBAL || (x) == OWL_SYM_WEAK || (x) == OWL_SYM_UNDEFINED )

typedef enum {
    OWL_TYPE_FUNCTION,
    OWL_TYPE_OBJECT,
    OWL_TYPE_SECTION,           // internal - should not be used by client
    OWL_TYPE_FILE,              // internal - ditto
} owl_sym_type;

#define _OwlMetaSymbol( x )     ( (x) == OWL_TYPE_SECTION || (x) == OWL_TYPE_FILE )

typedef enum {
    OWL_FORMAT_ELF,
    OWL_FORMAT_COFF,
} owl_format;

typedef enum {
    OWL_CPU_PPC,
    OWL_CPU_ALPHA,
    OWL_CPU_MIPS,
    OWL_CPU_INTEL
} owl_cpu;

typedef enum {
    OWL_RELOC_ABSOLUTE,                 // ref to a 32-bit absolute address
    OWL_RELOC_WORD,                     // a direct ref to a 32-bit address
    OWL_RELOC_HALF_HI,                  // ref to high half of 32-bit address
    OWL_RELOC_HALF_HA,                  // ditto adjusted for signed low 16 bits
    OWL_RELOC_PAIR,                     // pair - used to indicate prev hi and next lo linked
    OWL_RELOC_HALF_LO,                  // ref to low half of 32-bit address
    OWL_RELOC_BRANCH_REL,               // relative branch (Alpha: 21-bit; PPC: 14-bit)
    OWL_RELOC_BRANCH_ABS,               // absolute branch (Alpha: not used; PPC: 14-bit)
    OWL_RELOC_JUMP_REL,                 // relative jump (Alpha: 14-bit hint; PPC: 24-bit)
    OWL_RELOC_JUMP_ABS,                 // absolute jump (Alpha: not used; PPC: 24-bit)
    OWL_RELOC_SECTION_OFFSET,           // offset of item within it's section

    // meta reloc
    OWL_RELOC_SECTION_INDEX,            // index of section within COFF file

    OWL_RELOC_TOC_OFFSET,               // 16-bit offset within TOC (PPC)
    OWL_RELOC_GLUE,                     // location of NOP for GLUE code
    OWL_RELOC_FP_OFFSET,                // cheesy hack for inline assembler
} owl_reloc_type;

typedef enum {
    OWL_SEC_ATTR_NONE                   = 0x0000,
    OWL_SEC_ATTR_CODE                   = 0x0001,   // has code
    OWL_SEC_ATTR_DATA                   = 0x0002,   // has initialized data
    OWL_SEC_ATTR_BSS                    = 0x0004,   // has uninitialized data
    OWL_SEC_ATTR_INFO                   = 0x0008,   // directives etc.
    OWL_SEC_ATTR_DISCARDABLE            = 0x0010,   // can be discarded
    OWL_SEC_ATTR_REMOVE                 = 0x0020,   // remove at link time
    OWL_SEC_ATTR_COMDAT                 = 0x0040,   // comdat section
    OWL_SEC_ATTR_PERM_READ              = 0x0100,   // readable
    OWL_SEC_ATTR_PERM_WRITE             = 0x0200,   // writable
    OWL_SEC_ATTR_PERM_EXEC              = 0x0400,   // executable
    OWL_SEC_ATTR_PERM_SHARE             = 0x0800,   // sharable
} owl_section_attribute;

typedef enum {
    OWL_SECTION_INFO            = OWL_SEC_ATTR_INFO | OWL_SEC_ATTR_REMOVE | OWL_SEC_ATTR_PERM_READ,
    OWL_SECTION_CODE            = OWL_SEC_ATTR_CODE | OWL_SEC_ATTR_PERM_READ | OWL_SEC_ATTR_PERM_EXEC,
    OWL_SECTION_DATA            = OWL_SEC_ATTR_DATA | OWL_SEC_ATTR_PERM_READ | OWL_SEC_ATTR_PERM_WRITE,
    OWL_SECTION_BSS             = OWL_SEC_ATTR_BSS | OWL_SEC_ATTR_PERM_READ | OWL_SEC_ATTR_PERM_WRITE,
    OWL_SECTION_DEBUG           = OWL_SEC_ATTR_DATA | OWL_SEC_ATTR_DISCARDABLE | OWL_SEC_ATTR_PERM_READ,
    OWL_SECTION_PDATA           = OWL_SEC_ATTR_DATA | OWL_SEC_ATTR_PERM_READ,
    OWL_SECTION_COMDAT_CODE     = OWL_SECTION_CODE | OWL_SEC_ATTR_COMDAT,
    OWL_SECTION_COMDAT_DATA     = OWL_SECTION_DATA | OWL_SEC_ATTR_COMDAT,
    OWL_SECTION_COMDAT_BSS      = OWL_SECTION_BSS | OWL_SEC_ATTR_COMDAT,
    OWL_SECTION_COMDAT_DEBUG    = OWL_SECTION_DEBUG | OWL_SEC_ATTR_COMDAT,
    OWL_SECTION_COMDAT_PDATA    = OWL_SECTION_PDATA | OWL_SEC_ATTR_COMDAT,
} owl_section_type;

#define _OwlSectionBSS( x )             ( (x)->type & OWL_SEC_ATTR_BSS )
#define _OwlSectionComdat( x )          ( (x)->type & OWL_SEC_ATTR_COMDAT )

typedef enum {
    OWL_FILE_OBJECT,
    OWL_FILE_EXE,
    OWL_FILE_DLL,
    OWL_FILE_LIB,
} owl_file_type;

typedef enum {
    OWL_WKSYM_NORMAL            = 0x0000,   // regular weak symbol
    OWL_WKSYM_LAZY              = 0x0001,   // lazy weak symbol
    OWL_WKSYM_ALIAS             = 0x0002,   // alias symbol
} owl_wksym_flags;

typedef uint_32         owl_line_num;
typedef int_32          owl_offset;
typedef owl_offset      owl_alignment;

typedef struct owl_info         *owl_handle;
typedef struct owl_file_info    *owl_file_handle;
typedef struct owl_section_info *owl_section_handle;
typedef struct owl_symbol_info  *owl_func_handle;
typedef struct owl_symbol_info  *owl_symbol_handle;

typedef void *owl_client_file;

// if you add a field, update owl@copyClientFuncs
typedef struct {
    int                 (*write)( owl_client_file, const char *, uint );
    long                (*tell)( owl_client_file );
    long                (*lseek)( owl_client_file, long int, int );
    void *              (*alloc)( size_t );
    void                (*free)( void * );
} owl_client_funcs;

extern owl_handle OWLENTRY OWLInit( owl_client_funcs *funcs, owl_cpu cpu );
extern void OWLENTRY OWLFini( owl_handle handle );
extern void OWLENTRY OWLComdatDep( owl_section_handle section, owl_section_handle dep );

extern owl_file_handle OWLENTRY OWLFileInit( owl_handle handle, const char *name, owl_client_file file, owl_format format, owl_file_type type );
extern void OWLENTRY OWLFileSymbol( owl_file_handle file, const char *name );
extern void OWLENTRY OWLFileFini( owl_file_handle file );

extern owl_section_handle OWLENTRY OWLSectionInit( owl_file_handle file, const char *name, owl_section_type type, owl_alignment align );
extern void OWLENTRY OWLSectionFini( owl_section_handle section );

extern owl_symbol_handle OWLENTRY OWLSymbolInit( owl_file_handle file, const char *name );
extern void OWLENTRY OWLSymbolFini( owl_symbol_handle );

extern void OWLENTRY OWLEmitLabel( owl_section_handle section, owl_symbol_handle sym, owl_sym_type type, owl_sym_linkage linkage );
extern void OWLENTRY OWLEmitData( owl_section_handle section, const char *buffer, int size );
extern void OWLENTRY OWLEmitReloc( owl_section_handle section, owl_offset offset, owl_symbol_handle sym, owl_reloc_type type );
extern void OWLENTRY OWLEmitMetaReloc( owl_section_handle section, owl_offset offset, void *data, owl_reloc_type type );
extern void OWLENTRY OWLEmitRelocAddend( owl_section_handle section, owl_reloc_type type, owl_offset addend );
extern void OWLENTRY OWLEmitImport( owl_file_handle file, const char *name );
extern void OWLENTRY OWLEmitExport( owl_file_handle file, owl_symbol_handle sym );

#define OWLTellOffset OWLTellLocation

extern void OWLENTRY OWLWeakExt( owl_file_handle file, owl_symbol_handle wk, owl_symbol_handle alt, owl_wksym_flags flags );
extern void OWLENTRY OWLSetLocation( owl_section_handle section, owl_offset location );
extern owl_offset OWLENTRY OWLTellLocation( owl_section_handle section );
extern owl_offset OWLENTRY OWLTellSize( owl_section_handle section );
extern owl_section_type OWLENTRY OWLTellSectionType( owl_section_handle section );
extern owl_sym_linkage OWLENTRY OWLTellSymbolLinkage( owl_file_handle file, owl_symbol_handle name );
extern void OWLENTRY OWLTellData( owl_section_handle section, owl_offset location, char *dst, owl_offset len );

extern owl_func_handle OWLENTRY OWLDebugFuncBegin( owl_section_handle, owl_symbol_handle, owl_line_num, owl_offset  );
extern void OWLENTRY OWLDebugFuncLine( owl_func_handle, owl_line_num, owl_offset );
extern void OWLENTRY OWLDebugFuncFile( owl_func_handle func, char const *name  );
extern void OWLENTRY OWLDebugFuncEnd( owl_func_handle, owl_line_num, owl_offset );

extern void OWLENTRY OWLLogEnable( owl_file_handle file, owl_client_file client_file );
extern void OWLENTRY OWLLog( owl_file_handle file, char *fmt, ... );
#endif

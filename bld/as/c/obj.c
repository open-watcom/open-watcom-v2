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


#include "as.h"
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>

#ifdef AS_PPC
  #define OBJ_OWL_CPU   OWL_CPU_PPC
#else
  #define OBJ_OWL_CPU   OWL_CPU_ALPHA
#endif

extern int              ExitStatus;

owl_handle              OwlHandle;
owl_file_handle         OwlFile;

FILE                    *ErrorFile;
static char             errorFilename[ _MAX_PATH2 ];

static bool             objectDefined = FALSE;
static int              objFile;
static char             objName[ _MAX_PATH2 ];

owl_section_handle      CurrentSection;
uint_8                  CurrAlignment;

struct asm_label {
    owl_section_handle  section;
    char                *sym_name;
    uint_32             is_numeric:1;
    uint_32             label_num:31;
    owl_sym_type        sym_type;
    owl_sym_linkage     sym_linkage;
    owl_symbol_handle   sym_hdl;
    struct asm_label    *next;
};

typedef struct asm_label *label_list;

static label_list       labelList = NULL;       // The list of pending labels

void ObjSetObjFile( char *obj_name ) {
//************************************

    strcpy( objName, obj_name );
    objectDefined = TRUE;
}

typedef struct {
    char                *name;
    owl_section_type    type;
    owl_alignment       alignment;
} sect_info;

static sect_info reservedSecInfo[] = {
    #define PICK( a, b, c, d )  { b, c, d },
    #include "sections.inc"
    #undef PICK
};

void ObjSwitchSection( reserved_section section ) {
//*************************************************
// Switch to a reserved section

    sect_info   *secinfo;

    secinfo = &reservedSecInfo[ section ];
    if( SectionLookup( secinfo->name ) ) {
        SectionSwitch( secinfo->name, NULL, 0 );
    } else {
        SectionNew( secinfo->name, &secinfo->type, secinfo->alignment );
    }
}

bool ObjInit( char *fname ) {
//***************************

    owl_client_funcs    funcs = {
        (int (*)( owl_client_file, const char *, int ))write,
        (long (*)( owl_client_file ))tell,
        (long (*)( owl_client_file, long, int ))lseek,
        MemAlloc,
        MemFree
    };
    char                name[ _MAX_FNAME ];
    owl_format          obj_format;

    SectionInit();
    _splitpath( fname, NULL, NULL, name, NULL );
    if( !objectDefined ) {
        _makepath( objName, NULL, NULL, name, ".obj" );
    }
    objectDefined = FALSE;      // so that the /fo applies only to the 1st obj
    _makepath( errorFilename, NULL, NULL, name, ".err" );
    objFile = open( objName, O_CREAT | O_TRUNC | O_BINARY | O_WRONLY,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if( objFile == -1 ) {
        AsOutMessage( stderr, UNABLE_TO_CREATE, objName );
        fputc( '\n', stderr );
        return( FALSE );
    }
    ErrorFile = fopen( errorFilename, "wt" );
    OwlHandle = OWLInit( &funcs, OBJ_OWL_CPU );
    obj_format = ( _IsOption( OBJ_COFF ) ? OWL_FORMAT_COFF : OWL_FORMAT_ELF );
    OwlFile = OWLFileInit( OwlHandle, fname, (owl_client_file)objFile, obj_format, OWL_FILE_OBJECT );
    ObjSwitchSection( AS_SECTION_TEXT );
    CurrAlignment = 0;
    return( TRUE );
}

static owl_sym_linkage linkage_table[] = {
    OWL_SYM_STATIC,     // Unknown
    OWL_SYM_GLOBAL,     // Global
    OWL_SYM_STATIC      // Static
};

bool ObjLabelDefined( sym_handle sym ) {
//**************************************

    owl_sym_linkage     linkage;
    label_list          curr_label;
    char                *sym_name;

    linkage = OWLTellSymbolLinkage( OwlFile, SymObjHandle( sym ) );
    if( linkage != OWL_SYM_UNDEFINED ) {
        return( TRUE );
    }
    // Still need the check the labelList
    curr_label = labelList;
    sym_name = SymName( sym );
    while( curr_label ) {
        if( (!curr_label->is_numeric) && strcmp( curr_label->sym_name, sym_name ) == 0 ) {
            return( TRUE );
        }
        curr_label = curr_label->next;
    }
    return( FALSE );
}

static void doStackLabel( sym_handle sym, owl_sym_type type, owl_sym_linkage linkage ) {
//**************************************************************************************

    label_list          new_label;

    new_label = MemAlloc( sizeof( struct asm_label ) );
    new_label->section = CurrentSection;
    new_label->sym_name = SymName( sym );
    new_label->sym_type = type;
    new_label->sym_linkage = linkage;
    new_label->sym_hdl = SymObjHandle( sym );
    new_label->is_numeric = 0;
    new_label->next = labelList;
    labelList = new_label;
}

void doStackNumericLabel( int_32 label_num, owl_sym_type type, owl_sym_linkage linkage ) {
//****************************************************************************************

    label_list          new_label;

    new_label = MemAlloc( sizeof( struct asm_label ) );
    new_label->section = CurrentSection;
    new_label->sym_name = NULL;
    new_label->sym_type = type;
    new_label->sym_linkage = linkage;
    new_label->is_numeric = 1;
    new_label->label_num = label_num;
    new_label->next = labelList;
    labelList = new_label;
}

void ObjEmitLabel( sym_handle sym ) {
//***********************************
// Stacks up the label in the list for ObjEmitData to emit

    owl_sym_type        type;
    owl_sym_linkage     linkage;
    sym_linkage         symbol_linkage;
    owl_section_type    section_type;

    if( ObjLabelDefined( sym ) ) {
        Error( SYM_ALREADY_DEFINED, SymName( sym ) );
        return;
    }
    section_type =  OWLTellSectionType( CurrentSection );
    if( section_type & OWL_SEC_ATTR_CODE ) {
        type = OWL_TYPE_FUNCTION;
    } else {
        type = OWL_TYPE_OBJECT;
    }
    symbol_linkage = SymGetLinkage( sym );
    if( symbol_linkage == SL_UNKNOWN ) {
        SymSetLinkage( sym, SL_STATIC );    // from now on it's static
    }
    linkage = linkage_table[symbol_linkage];
    doStackLabel( sym, type, linkage );
}

void ObjEmitNumericLabel( int_32 label_num ) {
//********************************************
// Stacks up the numeric label in the list for ObjEmitData to emit

    owl_sym_type        type;
    owl_section_type    section_type;

    section_type =  OWLTellSectionType( CurrentSection );
    if( section_type & OWL_SEC_ATTR_CODE ) {
        type = OWL_TYPE_FUNCTION;
    } else {
        type = OWL_TYPE_OBJECT;
    }
    doStackNumericLabel( label_num, type, OWL_SYM_STATIC );
}

void ObjFlushLabels( void ) {
//***************************

    label_list  next_label;

    while( labelList ) {
        next_label = labelList->next;
        if( labelList->is_numeric ) {
            AsNumLabelEmit( labelList->label_num,
                            labelList->section,
                            OWLTellOffset( labelList->section ),
                            labelList->sym_type );
                            //labelList->sym_linkage );
            _DBGMSG2( "Numeric label #%d emitted\n", labelList->label_num );
        } else {
            OWLEmitLabel( labelList->section, labelList->sym_hdl,
                          labelList->sym_type, labelList->sym_linkage );
            _DBGMSG2( "Label '%s' emitted\n", labelList->sym_name );
        }
        MemFree( labelList );
        labelList = next_label;
    }
}

void ObjEmitData( owl_section_handle section, void *buffer, int size, bool align ) {
//**********************************************************************************
// Aligns to proper address, emits all pending labels, then emits the data

    if( align ) {
        (void)ObjAlign( section, CurrAlignment );
    }
    ObjFlushLabels();
    OWLEmitData( section, buffer, size );
    // printf( "real data emitted.\n" );
}

void ObjNopPad( owl_section_handle section, uint_8 count ) {
//**********************************************************
// Emits count no-ops

    uint_32     nop_opcode = INS_NOP;

    while( count-- > 0 ) {
        OWLEmitData( section, (char *)&nop_opcode, sizeof( nop_opcode ) );
    }
}

void ObjNullPad( owl_section_handle section, uint_8 count ) {
//***********************************************************
// Emits count bytes of zeros

    char        byte = 0;

    while( count-- > 0 ) {
        OWLEmitData( section, &byte, sizeof( byte ) );
    }
}

owl_offset ObjAlign( owl_section_handle section, uint_8 alignment ) {
//*******************************************************************
// Aligns the offset to 2^alignment boundary. Returns the offset for
// convenience.

    owl_offset  offset;

    offset = OWLTellOffset( section );
    if( alignment == 0 ) return( offset );    // alignment disabled
    alignment = 1 << alignment;
    alignment = ( alignment - ( offset % alignment ) ) % alignment;
    if( alignment == 0 ) return( offset );
    if( OWLTellSectionType( section ) & OWL_SEC_ATTR_CODE ) {
        ObjNopPad( section, alignment / 4 );
        _DBGMSG2( "Align: %d nops emitted\n", alignment / 4 );
        ObjNullPad( section, alignment % 4 );
        _DBGMSG2( "Align: %d nulls emitted\n", alignment % 4 );
    } else {
        ObjNullPad( section, alignment );
        _DBGMSG2( "Align: %d nulls emitted\n", alignment );
    }
    assert( offset + alignment == OWLTellOffset( section ) );
    return( offset + alignment );
}

static void doEmitReloc( owl_section_handle section, owl_offset offset,
                         void *target, owl_reloc_type type,
                         bool named_sym ) {
//***************************************************************************************************************************

    obj_section_handle  ref_section;
    sym_handle          sym;
    int_32              label_num;

    if( named_sym ) {
        if( ( ref_section = SectionLookup( target ) ) != NULL ) {
            // We only handle backward reference to a section
            // So we have to define a section before we can refer to it.
            OWLEmitMetaReloc( section, offset, SectionOwlHandle( ref_section ), type );
        } else {
            sym = SymLookup( target );
            assert( sym != NULL );
            OWLEmitReloc( section, offset, SymObjHandle( sym ), type );
        }
    } else {
        label_num = *(int_32 *)target;
        AsNumLabelReloc( section, offset, label_num, type );
    }
}

void ObjDirectEmitReloc( owl_section_handle section, owl_offset offset, void *target, owl_reloc_type type, bool named_sym ) {
//***************************************************************************************************************************
// Just emit the reloc. No alignment check, no l^-h^ reloc pairing etc.

    doEmitReloc( section, offset, target, type, named_sym );
}

/*
owl_offset ObjTellOffset( owl_section_handle section ) {
//******************************************************

    return( OWLTellOffset( section ) );
}
*/

void ObjEmitReloc( owl_section_handle section, void *target, owl_reloc_type type, bool align, bool named_sym ) {

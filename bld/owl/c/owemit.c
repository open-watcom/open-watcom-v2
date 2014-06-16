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
* Description:  Implementation of externally callable OWL emit routines.
*
****************************************************************************/


#include "owlpriv.h"
#include "owreloc.h"

void OWLENTRY OWLEmitLabel( owl_section_handle section, owl_symbol_handle symbol, owl_sym_type type, owl_sym_linkage linkage ) {
//******************************************************************************************************************************

    _Log(( section->file, "OWLEmitLabel( %x, %x, %x, %x )\n", section, symbol, type, linkage ));
    assert( symbol->section == NULL );
    // might have had a reloc to it and created an undef'd entry
    // mark it as defined here (override type/linkage/location from reloc)
    OWLSymbolDefine( section->file->symbol_table, symbol,
                            section, section->location, type, linkage );
}

void OWLENTRY OWLWeakExt( owl_file_handle file, owl_symbol_handle wk_sym, owl_symbol_handle alt_sym, owl_wksym_flags flags ) {
//****************************************************************************************************************************

#ifdef NDEBUG
    file = file;
#endif
    _Log(( file, "OWLWeakExt( %x, %x, %x, %x )\n", file, wk_sym, alt_sym, flags ));
    assert( alt_sym->linkage != OWL_SYM_WEAK );
    wk_sym->linkage = OWL_SYM_WEAK;
    wk_sym->x.alt_sym = alt_sym;
    switch( flags ) {
    case OWL_WKSYM_NORMAL:  
        break;
    case OWL_WKSYM_LAZY:  
        wk_sym->flags |= OWL_SYM_LAZY;
        break;
    case OWL_WKSYM_ALIAS:  
        wk_sym->flags |= OWL_SYM_ALIAS;
        break;
    default:
        assert( 0 );
    }
}

void OWLENTRY OWLEmitData( owl_section_handle section, const char *buffer, int size ) {
//*************************************************************************************

    _Log(( section->file, "OWLEmitData( %x, %x, %x )\n", section, buffer, size ));
    section->location += size;
    if( section->location > section->size ) {
        section->size = section->location;
    }
    if( section->buffer != NULL ) {
        OWLBufferWrite( section->buffer, buffer, size );
        // assert( section->size == OWLBufferSize( section->buffer ) );
    }
}


static void addReloc( owl_section_handle section, owl_reloc_info *reloc ) {
//*************************************************************************

    if( section->last_reloc != NULL ) {
        section->last_reloc->next = reloc;
        section->last_reloc = reloc;
    } else {
        section->first_reloc = reloc;
        section->last_reloc = reloc;
    }
}

static void doNewReloc( owl_section_handle section, owl_symbol_handle sym, owl_offset offset, owl_reloc_type type ) {
//*******************************************************************************************************************

    owl_reloc_info      *reloc;

    reloc = _ClientAlloc( section->file, sizeof( owl_reloc_info ) );
    reloc->symbol = sym;
    reloc->type = type;
    reloc->location = offset;
    reloc->next = NULL;
    addReloc( section, reloc );
    section->num_relocs += 1;
}

void OWLENTRY OWLEmitReloc( owl_section_handle section, owl_offset offset, owl_symbol_handle sym, owl_reloc_type type ) {
//***********************************************************************************************************************

    _Log(( section->file, "OWLEmitReloc( %x, %x, %x, %x )\n",
                section, offset, sym, type ));
    if( section->file->format == OWL_FORMAT_ELF && type == OWL_RELOC_PAIR ) {
        // ELF has no pair relocs and they wreak havoc later on. It's easiest
        // to get rid of them right here.
        return;
    }
    doNewReloc( section, sym, offset, type );
}

void OWLENTRY OWLEmitMetaReloc( owl_section_handle section, owl_offset offset, void *data, owl_reloc_type type ) {
//****************************************************************************************************************

    owl_section_handle  target;

    target = (owl_section_handle)data;
    _Log(( section->file, "OWLEmitMetaReloc( %x, %x, %x, %x )\n",
                section, offset, target, type ));
    doNewReloc( section, target->sym, offset, type );
}

void OWLENTRY OWLEmitRelocAddend( owl_section_handle section, owl_reloc_type type, owl_offset addend ) {
//******************************************************************************************************

    uint_32             bit_mask;
    uint_32             data;
    owl_reloc_info      reloc;

    _Log(( section->file, "OWLEmitRelocAddend( %x, %x, %x )\n",
                section, type, addend ));
    OWLTellData( section, section->location, (char *)&data, sizeof( data ) );
    reloc.type = type;
    bit_mask = OWLRelocBitMask( section->file, &reloc );
    data = (data&~bit_mask)|(((addend&bit_mask)+(data&bit_mask))&bit_mask);
    OWLEmitData( section, (char *)&data, sizeof( data ) );
}

void OWLENTRY OWLEmitImport( owl_file_handle file, const char *name ) {
//*********************************************************************

    owl_symbol_handle   sym;

    sym = OWLSymbolInit( file, name );
    OWLSymbolDefine( file->symbol_table, sym, NULL, 0, OWL_TYPE_OBJECT, OWL_SYM_UNDEFINED );
}

void OWLENTRY OWLEmitExport( owl_file_handle file, owl_symbol_handle sym ) {
//**************************************************************************

    assert( sym != NULL );
    file = file;
    sym->flags |= OWL_SYM_EXPORT;
}

void OWLENTRY OWLSetLocation( owl_section_handle section, owl_offset loc ) {
//**************************************************************************

    _Log(( section->file, "OWLSetLocation( %x, %x )\n", section, loc ));
    section->location = loc;
    if( section->location > section->size ) {
        section->size = section->location;
    }
    if( section->buffer != NULL ) {
        OWLBufferSeek( section->buffer, loc );
    }
}

owl_offset OWLENTRY OWLTellLocation( owl_section_handle section ) {
//*****************************************************************

    _Log(( section->file, "OWLTellOffset( %x ) -> %x\n", section, section->location ));
    return( section->location );
}

owl_offset OWLENTRY OWLTellSize( owl_section_handle section ) {
//*****************************************************************

    _Log(( section->file, "OWLTellSize( %x ) -> %x\n", section, section->size ));
    return( section->size );
}

owl_section_type OWLENTRY OWLTellSectionType( owl_section_handle section ) {
//**************************************************************************

    return( section->type );
}

owl_sym_linkage OWLENTRY OWLTellSymbolLinkage( owl_file_handle file, owl_symbol_handle symbol ) {
//***********************************************************************************************

    file = file;
    if( symbol != NULL ) {
        return( symbol->linkage );
    }
    return( OWL_SYM_UNDEFINED );
}

void OWLENTRY OWLTellData( owl_section_handle section, owl_offset location, char *dst, owl_offset len ) {
//*******************************************************************************************************

    _Log(( section->file, "OWLTellData( %x, %x, %x, %x )\n", section, location, dst, len ));
    OWLBufferRead( section->buffer, location, dst, len );
}

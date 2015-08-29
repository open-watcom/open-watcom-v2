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

#include "clibext.h"

typedef struct numlab_reloc numlab_reloc;

struct numlab_reloc {
    owl_section_handle  section;        // location of the reloc
    owl_offset          offset;         // ...
    owl_reloc_type      type;
    numlab_reloc        *next;
};

typedef struct numlab numlab;

struct numlab {
    owl_section_handle  section;        // location of the numeric label
    owl_offset          offset;         // ...
    owl_sym_type        type;           // type of label
    numlab_reloc        *relocs;        // list of relocs to it
    numlab              *prev;          // prev defined label (same #)
    numlab              *next;          // next defined label (same #)
};

typedef struct {
    numlab              *first;         // First in the list
    numlab              *last;          // Last in the list
    numlab_reloc        *next_refs;     // List of [0-9]f relocs
} numeric_label_list;


static numeric_label_list numericLabels[10];    // 0: - 9:
static uint_32 numLabelCounts[10] = { 0 };  // to keep track of their parity

extern sym_handle AsNumLabelSymLookup( int_32 *label_num ) {
//**********************************************************
// Look/Cook up symbol handles for both forward and backward references for
// each label_num.

    sym_handle  sym;
    char        *sym_name;

    if( *label_num > 0 ) {
        ++numLabelCounts[ *label_num - 1 ];     // flip "e"/"o" if forward
    }
    sym_name = AsNumLabelMakeName( *label_num );
    if( *label_num > 0 ) {
        --numLabelCounts[ *label_num - 1 ];     // change back "e"/"o"
    }
    sym = SymLookup( sym_name );
    if( sym ) return( sym );
    return( SymAdd( sym_name, SYM_LABEL ) );
}

extern char *AsNumLabelMakeName( int_32 num ) {
//*********************************************
// Make a name for this unnamed label (must be an impossible identifier)
// Use it to match h^ and l^ relocs.
// For each label number, there are 2 possible labels that a reloc
// can refer to (forward and backward). So two unique symbols are needed for
// each labelnums. Their roles interchange as new labels are emitted.
// (Since forward ref becomes backward ref once a new label is generated)

    static char buffer[12];

    num = abs( num );
    sprintf( buffer, ( numLabelCounts[num - 1] & 1 ) ? "%do" : "%de", num );
    return( buffer );
}

extern int_32 AsNumLabelGetNum( const char *name ) {
//**************************************************
// The inverse function of AsNumLabelMakeName()

    char        *ptr;
    int_32      ret;

    ret = strtoul( name, &ptr, 10 );
    if( *ptr == 'e' ) {
        if( (numLabelCounts[ ret - 1 ] & 1) == 0 ) {
            ret = -ret;
        }
    } else {
        assert( *ptr == 'o' );
        if( (numLabelCounts[ ret - 1 ] & 1) == 1 ) {
            ret = -ret;
        }
    }
    return( ret );
}

static void doEmitNumericLabel( uint_32 label_num, owl_section_handle section, owl_offset offset, owl_sym_type type ) {
//*********************************************************************************************************************

    numeric_label_list  *label_list;
    numlab              *label;

    label_list = &numericLabels[ label_num - 1 ];

    label = MemAlloc( sizeof( *label ) );
    label->section = section;
    label->offset = offset;
    label->type = type;
    label->relocs = label_list->next_refs;  // Give it the forward refs
    label->prev = label_list->last;
    label->next = NULL;

    if( label_list->first == NULL ) {
        label_list->first = label;
    } else {
        label_list->last->next = label;
    }
    label_list->last = label;
    label_list->next_refs = NULL;
}

extern void AsNumLabelEmit( uint_32 label_num, owl_section_handle section, owl_offset offset, owl_sym_type type ) {
//*****************************************************************************************************************

    // These symbols are used for matching l^ & h^ garbage.
    sym_handle          new_fw_sym;     // forward
    char                *new_fw_name;
    sym_reloc           reloc;
    int_32              numlabel_ref;

    new_fw_name = AsNumLabelMakeName( label_num );
    new_fw_sym = SymLookup( new_fw_name );
    if( new_fw_sym ) {
        // check if it still has some unemitted relocs
        if( new_fw_sym->hi_relocs ) {
            Error( UNMATCHED_HIGH_RELOC, "<numeric reference>" );
            while( new_fw_sym->hi_relocs ) {
                SymDestroyReloc( new_fw_sym, new_fw_sym->hi_relocs );
            }
        }
        if( new_fw_sym->lo_relocs ) {
            _DBGMSG1( "emitting all the unemitted l^relocs\n" );
            while( ( reloc = new_fw_sym->lo_relocs ) != NULL ) {
                numlabel_ref = AsNumLabelGetNum( SymName( new_fw_sym ) );
                #ifdef _STANDALONE_
                ObjDirectEmitReloc( reloc->location.section,
                #else
                ObjDirectEmitReloc(
                #endif
                  reloc->location.offset, &numlabel_ref, OWL_RELOC_HALF_LO, FALSE );
                SymDestroyReloc( new_fw_sym, reloc );
                reloc = new_fw_sym->lo_relocs;
            }
        }
        assert( new_fw_sym->hi_relocs == NULL && new_fw_sym->lo_relocs == NULL );
    } else {
        new_fw_sym = SymAdd( new_fw_name, SYM_LABEL );
    }
    // So the original forward symbol is now the backward symbol.

    // Then append this new label into the numericLabels table
    doEmitNumericLabel( label_num, section, offset, type );
    numLabelCounts[ label_num - 1 ]++;
}

extern void AsNumLabelReloc( owl_section_handle section, owl_offset offset, int_32 label_ref, owl_reloc_type type ) {
//*******************************************************************************************************************

    numeric_label_list          *label_list;
    numlab_reloc                *reloc;

    label_list = &numericLabels[ abs(label_ref) - 1 ];

    reloc = MemAlloc( sizeof( *reloc ) );
    reloc->section = section;
    reloc->offset = offset;
    reloc->type = type;

    if( label_ref > 0 ) {
        // a forward reference; append it to the list.
        reloc->next = label_list->next_refs;
        label_list->next_refs = reloc;
    } else {
        // backward reference; make sure such label has been defined
        if( label_list->last ) {
            reloc->next = label_list->last->relocs;
            label_list->last->relocs = reloc;
        } else {
            Error( UNRESOLVED_BACK_NUMREF );
            MemFree( reloc );
        }
    }
}

extern void AsNumLabelFini( void ) {
//**********************************
// Emit all the relocs and free all data structures

    int                 ctr;
    numeric_label_list  *label_list;
    numlab              *label;
    numlab_reloc        *reloc;

    for( ctr = 0; ctr < 10; ctr++ ) {
        numLabelCounts[ ctr ] = 0;
        label_list = &numericLabels[ ctr ];
        label = label_list->first;
        while( label ) {
            reloc = label->relocs;
            while( reloc ) {
                #ifdef _STANDALONE_
                OWLSetLocation( reloc->section, reloc->offset );
                OWLEmitMetaReloc( reloc->section, reloc->offset, label->section, reloc->type );
                // Need to put label->offset into reloc's location!
                #ifdef AS_ALPHA
                // If reloc is relative, we shift addend left 2 bits.
                if( IS_RELOC_RELATIVE( reloc->type ) ) {
                    OWLEmitRelocAddend( reloc->section, reloc->type,
                                        label->offset >> 2 );
                } else {
                #endif
                    OWLEmitRelocAddend( reloc->section, reloc->type, label->offset );
                #ifdef AS_ALPHA
                }
                #endif
                #else
                ObjSetLocation( reloc->offset );
                // Emit a reloc to beginning of code stream
                if( ObjEmitMetaReloc( reloc->type, TRUE ) ) {
                    // Patch up addend
                    #ifdef AS_ALPHA
                    // Reloc must be relative. So shift addend left 2 bits.
                    ObjEmitRelocAddend( reloc->type, label->offset >> 2 );
                    #else
                    ObjEmitRelocAddend( reloc->type, label->offset );
                    #endif
                } else {
                    Error( ABS_REF_NOT_ALLOWED, "<numeric label>" );
                }
                #endif
                label->relocs = reloc->next;
                MemFree( reloc );
                reloc = label->relocs;
            }
            label_list->first = label->next;
            if( label_list->first == NULL ) {
                label_list->last = NULL;
            }
            MemFree( label );
            label = label_list->first;
        }
        if( label_list->next_refs ) {
            // Then we got some relocs to a non-existant label
            Error( NON_EXISTANT_FORWARD_REF, ctr );
            do {
                reloc = label_list->next_refs->next;
                MemFree( label_list->next_refs );
                label_list->next_refs = reloc;
            } while( reloc );
        }
    }
}

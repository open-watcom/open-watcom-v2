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
* Description:  Inline assembler support for various front ends.
*
****************************************************************************/


#include "as.h"
#include "asinline.h"

#ifdef _STANDALONE_
#error For inline-assembler only!
#endif

#define ASMCODESTART    "0ASM"          // must be a non-usable string

unsigned char           *AsmCodeBuffer;
uint_32                 AsmCodeAddress;
uint_32                 AsmLastAddress;
uint_32                 AsmCodeLimit;
asmreloc                *AsmRelocs;
uint_8                  CurrAlignment;

struct asm_label {
    sym_handle          sym;            // NULL if is_numeric
    struct asm_label    *next;
    uint_32             is_numeric : 1;
    uint_32             label_num : 31;
};

typedef struct asm_label *label_list;

static label_list       labelList = NULL;       // The list of pending labels
static asmreloc         *lastReloc;

static owl_offset tellOffset( void ) {
//************************************

    return( AsmCodeAddress );
}

static void doEmitData( char *buffer, int size ) {
//************************************************

    memcpy( &AsmCodeBuffer[AsmCodeAddress], buffer, size );
    AsmCodeAddress += size;
    if( AsmCodeAddress > AsmLastAddress ) {
        AsmLastAddress = AsmCodeAddress;
    }
}

static void doEmitLabel( label_list label ) {
//*******************************************

    sym_location    loc;

    labelList = label->next;
    //label->next = NULL;
    if( label->is_numeric ) {
        AsNumLabelEmit( label->label_num, NULL, AsmCodeAddress, 0 );
    } else {
        loc.offset = AsmCodeAddress;
        SymSetLocation( label->sym, loc );
    }
}

static owl_offset getSymOffset( sym_handle sym ) {
//************************************************

    sym_location    loc;

    assert( SymLocationKnown( sym ) );
    loc = SymGetLocation( sym );
    return( loc.offset );
}

static owl_offset relocTargetDisp( owl_offset from, owl_offset to ) {
//*******************************************************************

    owl_offset  ret;

#ifdef AS_ALPHA
    from += 4;  // Alpha uses updated PC
#endif // PPC & MIPS uses current PC
    assert( ( to % 4 ) == 0 );
    assert( ( from % 4 ) == 0 );
    ret = to - from;
#if defined( AS_PPC )
    return( ret );
#elif defined( AS_ALPHA )
    return( ret >> 2 );
#elif defined( AS_MIPS )
    // TODO
    return( ret >> 2 );
#else
    #error Unknown CPU type for assembler!
#endif
}

static unsigned relocMasks[] = {
#if defined( AS_PPC )
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
#elif defined( AS_ALPHA )
    0xffffffff,         /* OWL_RELOC_ABSOLUTE */
    0xffffffff,         /* OWL_RELOC_WORD */
    0x0000ffff,         /* OWL_RELOC_HALF_HI */
    0x0000ffff,         /* OWL_RELOC_HALF_HA, unused */
    0x00000000,         /* OWL_RELOC_PAIR */
    0x0000ffff,         /* OWL_RELOC_HALF_LO */
    0x001fffff,         /* OWL_RELOC_BRANCH_REL */
    0x001fffff,         /* OWL_RELOC_BRANCH_ABS, unused */
    0x00003fff,         /* OWL_RELOC_JUMP_REL */
    0x00003fff,         /* OWL_RELOC_JUMP_ABS, unused */
    0x0000ffff,         /* OWL_RELOC_SECTION_INDEX */
    0xffffffff,         /* OWL_RELOC_SECTION_OFFSET */
#elif defined( AS_MIPS )
    // TODO
    0xffffffff,         /* OWL_RELOC_ABSOLUTE */
    0xffffffff,         /* OWL_RELOC_WORD */
    0x0000ffff,         /* OWL_RELOC_HALF_HI */
    0x0000ffff,         /* OWL_RELOC_HALF_HA, unused? */
    0x00000000,         /* OWL_RELOC_PAIR */
    0x0000ffff,         /* OWL_RELOC_HALF_LO */
    0x001fffff,         /* OWL_RELOC_BRANCH_REL */
    0x001fffff,         /* OWL_RELOC_BRANCH_ABS, unused */
    0x00003fff,         /* OWL_RELOC_JUMP_REL */
    0x00003fff,         /* OWL_RELOC_JUMP_ABS, unused */
    0x0000ffff,         /* OWL_RELOC_SECTION_INDEX */
    0xffffffff,         /* OWL_RELOC_SECTION_OFFSET */
#endif
};

static unsigned relocBitMask( asmreloc *reloc ) {
//***********************************************

    assert( reloc != NULL );
    return( relocMasks[ reloc->type ] );
}

static void doReloc( asmreloc *reloc ) {
//**************************************

    sym_handle  sym;
    uint_32     bit_mask;
    uint_32     *data;
    owl_offset  displacement;

    sym = SymLookup( reloc->name );
    displacement = relocTargetDisp( reloc->offset, getSymOffset( sym ) );
    bit_mask = relocBitMask( reloc );
    data = (uint_32 *)&AsmCodeBuffer[ reloc->offset ];
    *data = (*data&~bit_mask)|(((displacement&bit_mask)+(*data&bit_mask))&bit_mask);
}

static void resolveRelativeRelocs( void ) {
//*****************************************
// Do all relative relocs within the inline code and
// complain about references that are not internal and not defined by the
// compiler.

    asmreloc            *curr_reloc;
    asmreloc            **last;
    char                *keep_name;
    enum sym_state      state;

    curr_reloc = AsmRelocs;
    last = &AsmRelocs;
    while( curr_reloc ) {
        if( SymLocationKnown( SymLookup( curr_reloc->name ) ) ) {
            if( IS_RELOC_RELATIVE( curr_reloc->type ) ) {
                doReloc( curr_reloc );
            } else {
                // If not relative, we won't do it! (?)
                Error( ABS_REF_NOT_ALLOWED, curr_reloc->name );
            }
        } else {
            // See if it's defined outside.
            state = AsmQueryState( AsmQuerySymbol( curr_reloc->name ) );
            if( state == SYM_UNDEFINED ) {
                Error( SYMBOL_NOT_DECLARED, curr_reloc->name );
            } else {
                if( IS_RELOC_RELATIVE( curr_reloc->type ) && state == SYM_STACK ) {
                    Error( CANNOT_JUMP_TO_STACKVAR );
                } else {
                    // Leave these in the list...
                    //Warning( "'%s' is left for CC to take care of", curr_reloc->name );
                    keep_name = MemAlloc( strlen( curr_reloc->name ) + 1 );
                    strcpy( keep_name, curr_reloc->name );
                    curr_reloc->name = keep_name;
                    last = &curr_reloc->next;
                    curr_reloc = curr_reloc->next;
                    continue;
                }
            }
        }
        *last = curr_reloc->next;
        MemFree( curr_reloc );
        curr_reloc = *last;
    }
}

static asmreloc *newReloc( owl_offset offset, char *name, owl_reloc_type type ) {
//*******************************************************************************

    asmreloc    *reloc;

    reloc = MemAlloc( sizeof( asmreloc ) );
    reloc->next = NULL;
    reloc->name = name;
    reloc->offset = offset;
    reloc->type = type;
    return( reloc );
}

static void doEmitReloc( owl_offset offset, void *target, owl_reloc_type type, bool named_sym ) {
//***********************************************************************************************

    asmreloc    *reloc;
    int_32      label_num;

    if( named_sym ) {
        assert( SymLookup( target ) != NULL );
        reloc = newReloc( offset, target, type );
        if( AsmRelocs == NULL ) {
            AsmRelocs = reloc;
            lastReloc = reloc;
        } else {
            lastReloc->next = reloc;
            lastReloc = reloc;
        }
    } else {
        label_num = *(int_32 *)target;
        AsNumLabelReloc( NULL, offset, label_num, type );
    }
}

static void doStackLabel( sym_handle sym ) {
//******************************************

    label_list          new_label;

    new_label = MemAlloc( sizeof( struct asm_label ) );
    new_label->sym = sym;
    new_label->next = labelList;
    new_label->is_numeric = 0;
    new_label->label_num = 0;
    labelList = new_label;
}

extern void ObjInit( void ) {
//***************************

    CurrAlignment = 0;  // alignment disabled
    // Here we add a label marking the beginning of the code stream.
    // Relocs to this label can be done by ObjEmitMetaReloc().
    doStackLabel( SymAdd( ASMCODESTART, SYM_LABEL ) );
}

extern void ObjSetLocation( owl_offset offset ) {
//***********************************************

    assert( AsmLastAddress > offset );
    AsmCodeAddress = offset;
}

extern bool ObjEmitMetaReloc( owl_reloc_type type, bool align ) {
//***************************************************************
// Emit a reloc to the beginning of code.
// Must be a relative reloc. If not, FALSE is returned.

    if( !IS_RELOC_RELATIVE( type ) ) return( FALSE );
    ObjEmitReloc( ASMCODESTART, type, align, TRUE );
    return( TRUE );
}

extern void ObjEmitRelocAddend( owl_reloc_type type, uint_32 addend ) {
//*********************************************************************

    uint_32     bit_mask;
    uint_32     *pdata;

    bit_mask = relocMasks[ type ];
    pdata = (uint_32 *)&AsmCodeBuffer[ AsmCodeAddress ];
    *pdata = (*pdata&~bit_mask)|(((addend&bit_mask)+(*pdata&bit_mask))&bit_mask);
}

static bool findLabel( label_list labels, char *label_name ) {
//************************************************************

    label_list          curr_label;

    curr_label = labels;
    while( curr_label ) {
        if( strcmp( SymName( curr_label->sym ), label_name ) == 0 ) {
            return( TRUE );
        }
        curr_label = curr_label->next;
    }
    return( FALSE );
}

extern bool ObjLabelDefined( sym_handle sym ) {
//*********************************************

    enum sym_state      state;
    char                *sym_name;

    // Check if it's been emitted by us
    if( sym && SymLocationKnown( sym ) ) return( TRUE );
    // See if it's defined outside
    sym_name = SymName( sym );
    state = AsmQueryState( AsmQuerySymbol( sym_name ) );
    if( state != SYM_UNDEFINED ) {
        return( TRUE );
    }
    // Still need to check the labelList
    if( findLabel( labelList, sym_name ) ) return( TRUE );
    return( FALSE );
}

static void doStackNumericLabel( uint_32 label_num ) {
//****************************************************
// Numeric labels have label_num going from 1 to 10 (corresponds to 0: - 9:)

    label_list          new_label;

    new_label = MemAlloc( sizeof( struct asm_label ) );
    new_label->sym = NULL;
    new_label->next = labelList;
    new_label->is_numeric = 1;
    new_label->label_num = label_num;
    labelList = new_label;
}

extern void ObjEmitLabel( sym_handle sym ) {
//******************************************
// Stacks up the label in the list for ObjEmitData to emit

    if( ObjLabelDefined( sym ) ) {
        Error( SYM_ALREADY_DEFINED, SymName( sym ) );
        return;
    }
    doStackLabel( sym );
}

extern void ObjEmitNumericLabel( uint_32 label_num ) {
//****************************************************

    doStackNumericLabel( label_num );
}

extern void ObjFlushLabels( void ) {
//**********************************

    label_list  curr_label, next_label;

    curr_label = labelList;
    while( curr_label ) {
        next_label = curr_label->next;
        doEmitLabel( curr_label );
        MemFree( curr_label );
        curr_label = next_label;
    }
}

extern void ObjEmitData( void *buffer, int size, bool align ) {
//*************************************************************
// Aligns to proper address, emits all pending labels, then emits the data

    if( align ) {
        (void)ObjAlign( CurrAlignment );
    }
    ObjFlushLabels();
    doEmitData( buffer, size );
}

extern void ObjDirectEmitData( void *buffer, int size ) {
//*******************************************************

    doEmitData( buffer, size );
}

extern void ObjNopPad( uint_8 count ) {
//*************************************
// Emits count no-ops

    uint_32     nop_opcode = INS_NOP;

    while( count-- > 0 ) {
        doEmitData( (char *)&nop_opcode, sizeof( nop_opcode ) );
    }
}

extern void ObjNullPad( uint_8 count ) {
//**************************************
// Emits count bytes of zeros

    char        byte = 0;

    while( count-- > 0 ) {
        doEmitData( &byte, sizeof( byte ) );
    }
}

extern owl_offset ObjAlign( uint_8 alignment ) {
//**********************************************
// Aligns the offset to 2^alignment boundary. Returns the offset for
// convenience.

    owl_offset  offset;

    offset = tellOffset();
    if( alignment == 0 ) return( offset );    // alignment disabled
    alignment = 1 << alignment;
    alignment = ( alignment - ( offset % alignment ) ) % alignment;
    if( alignment == 0 ) return( offset );
    ObjNopPad( alignment / 4 );
    ObjNullPad( alignment % 4 );
    assert( offset + alignment == tellOffset() );
    return( offset + alignment );
}

#if 0
extern owl_offset ObjTellOffset( void ) {
//***************************************

    return( tellOffset() );
}
#endif

extern void ObjDirectEmitReloc( owl_offset offset, void *target, owl_reloc_type type, bool named_sym ) {
//******************************************************************************************************

    doEmitReloc( offset, target, type, named_sym );
}

extern void ObjEmitReloc( void *target, owl_reloc_type type, bool align, bool named_sym ) {
//*****************************************************************************************
// Should be called before emitting the data that has the reloc.
// (named_sym == TRUE) iff the target is a named label

    owl_offset          offset;

    if( align ) { // If data is aligned, we should also align this reloc offset!
        offset = ObjAlign( CurrAlignment );
    } else {
        offset = tellOffset();
    }
    ObjFlushLabels();
#ifdef AS_PPC   // ?
    doEmitReloc( offset, target, type, named_sym );
#else
    {
        sym_reloc       reloc;
        bool            match_high;
        owl_offset      offset_hi, offset_lo;
        sym_handle      (*lookup_func)( void * );

        if( type != OWL_RELOC_HALF_HI && type != OWL_RELOC_HALF_LO ) {
            doEmitReloc( offset, target, type, named_sym );
        } else {
            lookup_func = named_sym ?
                (sym_handle (*)( void * ))SymLookup :
                (sym_handle (*)( void * ))AsNumLabelSymLookup;
            match_high = ( type == OWL_RELOC_HALF_LO );    // hi match lo etc.
            reloc = SymMatchReloc( match_high, lookup_func( target ), NULL );
            if( reloc ) {       // got a match
                if( match_high ) {
                    offset_hi = reloc->location.offset;
                    offset_lo = offset;
                } else {
                    offset_hi = offset;
                    offset_lo = reloc->location.offset;
                }
                doEmitReloc( offset_hi, target, OWL_RELOC_HALF_HI, named_sym );
                doEmitReloc( offset_lo, target, OWL_RELOC_PAIR, named_sym );
                doEmitReloc( offset_lo, target, OWL_RELOC_HALF_LO, named_sym );
                SymDestroyReloc( lookup_func( target ), reloc );
            } else {    // no match; stack it up with the (aligned) offset!
                SymStackReloc( !match_high, lookup_func( target ), NULL, offset, named_sym );
            }
        }
    }
#endif
}

extern void ObjRelocsFini( void ) {
//*********************************
// After all lines have been parsed, we need to check whether there're any
// unmatched relocs still hanging around. If there're unmatched h^relocs,
// we issue an error. If there're unmatched l^relocs, we should be able
// to emit them.

    sym_reloc   reloc;
    sym_handle  sym;
    int_32      numlabel_ref;

    reloc = SymGetReloc( TRUE, &sym );
    while( reloc != NULL ) {
        if( reloc->named ) {
            Error( UNMATCHED_HIGH_RELOC, SymName( sym ) );
        } else {
            Error( UNMATCHED_HIGH_RELOC, "<numeric reference>" );
        }
        SymDestroyReloc( sym, reloc );
        reloc = SymGetReloc( TRUE, &sym );
    }
    reloc = SymGetReloc( FALSE, &sym );
    while( reloc != NULL ) {
        if( reloc->named ) {
            doEmitReloc( reloc->location.offset,
                SymName( sym ), OWL_RELOC_HALF_LO, TRUE );
        } else {
            numlabel_ref = AsNumLabelGetNum( SymName( sym ) );
            doEmitReloc( reloc->location.offset,
                &numlabel_ref, OWL_RELOC_HALF_LO, FALSE );
        }
        SymDestroyReloc( sym, reloc );
        reloc = SymGetReloc( FALSE, &sym );
    }
#ifdef AS_DEBUG_DUMP
    (void)SymRelocIsClean( TRUE );
#endif
    AsNumLabelFini();       // resolve all numeric label relocs
    resolveRelativeRelocs();
}

extern void ObjFini( void ) {
//********************

    ObjFlushLabels();       // In case there're still pending labels
    ObjRelocsFini();
    AsmCodeAddress = AsmLastAddress;    // points to end of code
}

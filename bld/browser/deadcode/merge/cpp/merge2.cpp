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


// System includes --------------------------------------------------------

#include <dwarf.h>
#include <dr.h>
#include <stdio.h>

// Project includes -------------------------------------------------------

#include "drmisc.h"
#include "merge.h"
#include "errors.h"
#include "memstrm.h"
#include "syminfo.h"

// .debug_info processing --------------------------------------------------

/*
 | Basic idea is to use pass 1 to decide what symbols to keep.  In pass 2
 | we decide what symbols can be replaced.
 */

void DwarfFileMerger::processInfoPass1( int mbrIndex )
/****************************************************/
{
    uint_8 * pSyms;
    unsigned long length;

    _mbrFileTable[ mbrIndex ]->getSectionRawData( DR_DEBUG_INFO, pSyms, &length );

    processSymbolsPass1( pSyms, mbrIndex, length );
}

// Pass 1 ------------------------------------------------------------------

/*
 | In Pass 1, the idea is to go through all the symbols and see which symbols
 | we *definitely* want to keep.  For example, if the symbol is an externally
 | visible defined subroutine, we'll add it to the _definedSymbols list.
 |
 | Note that it's quite possible and desirable to have symbols which may
 | have the same name and tag in the _definedSymbol list because symbols
 | may be multiply defined.
 |
 | In Pass 2, whenever we process a symbol we have to decide whether or
 | not to keep it.  To decide, we check if we can replace it with a symbol
 | in _definedSymbols.  We get a list of symbols which match the name
 | and tag of the symbol we're checking.  We can choose from the list to
 | decide which one (if any) matches the best.
 |
 */
void DwarfFileMerger::processSymbolsPass1( uint_8 * pSyms,
                                           int mbrIndex,
                                           unsigned long length )
/***************************************************************/
{

    uint_8 *symStart;                   // offset to start of symbol
    uint_8 *    p;

    uint_32     unit_length;
    uint_32     unit_base;
    uint_32     abbrev_code;
    uint_32     tag;
    uint_32     ui32;
    uint_8 *    pAb;
    uint_8      ui8;
    Abbreviation * pAbbrev;
    bool        skip;

    p = pSyms;

    for( ;; ) {
        if( p >= pSyms + length ) break;

        unit_length = *(uint_32 *)p;
        unit_base = ( uint_32 )p + sizeof( uint_32 );

        /*
         * Get address size
         */
        p += 10;
        ui8 = *p;
        p++;
        // SetGlobalParms( ui8 );


        while( (uint_32)p - unit_base < unit_length ) {
            skip = TRUE;
            symStart = p;
            p = DecodeULEB128( p, &abbrev_code );

            if( abbrev_code == 0 ) continue;

            pAbbrev = _abbrevList[ abbrev_code ];
            if( pAbbrev == NULL ) {
                throw ErrInvalidFormat;
            }
            pAb = pAbbrev->data;

            pAb = DecodeULEB128( pAb, &ui32 );
            pAb = DecodeULEB128( pAb, &tag );

            /*
             | Some symbols we can automatically skip over since we don't
             | want to do any replacement using them ( although, in the
             | future we may ).  Other symbols we have to process their
             | attributes to decide whether or not to keep them.
             */
            switch( tag ) {
                case DW_TAG_array_type :
                case DW_TAG_base_type :
                case DW_TAG_class_type :
                case DW_TAG_constant :
                case DW_TAG_enumeration_type :
                case DW_TAG_structure_type :
                case DW_TAG_subprogram :
                case DW_TAG_subroutine_type :
                case DW_TAG_typedef :
                case DW_TAG_union_type :
                case DW_TAG_variable :
                    skip = FALSE;
                    break;

                case DW_TAG_const_type :
                case DW_TAG_pointer_type :
                case DW_TAG_reference_type :
                case DW_TAG_volatile_type :
                    skip = TRUE;        // Skip these for now...
                    break;

                default :
                    break;
            }

            if( skip ) {
                skipSymbol( p, pAbbrev );
            } else {
                processSymbolPass1( p, mbrIndex, pAbbrev, ( uint_32 )( symStart - pSyms ) );
            }
        }
    }
}

void DwarfFileMerger::skipSymbol( uint_8 * & p, Abbreviation * pAbbrev )
/**********************************************************************/
{
    uint_32 attr;
    uint_32 form;
    uint_32 ui32;
    uint_8 * pAb( pAbbrev->data );

    // skip code, tag, and sibling flag

    pAb = DecodeULEB128( pAb, &ui32 );
    pAb = DecodeULEB128( pAb, &ui32 );
    pAb++;

    for( ;; ) {
        pAb = DecodeULEB128( pAb, &attr );
        pAb = DecodeULEB128( pAb, &form );

        if( attr == 0 ) break;

        p = SkipForm( p, ( uint_16 )form );
    }
}


void DwarfFileMerger::processSymbolPass1( uint_8 * & p,
                                          int mbrIndex,
                                          Abbreviation * pAbbrev,
                                          uint_32 offset )
/***************************************************************/
{
    uint_32 attr;
    uint_32 form;
    uint_32 ui32;
    uint_32 tag;
    uint_8 * pAb( pAbbrev->data );
    char * name;
    bool skip;
    uint_8 flag;
    uint_8 isExternal;

    skip = FALSE;
    name = NULL;
    isExternal = 1;

    /*
     | skip code, tag, and sibling flag
     */

    pAb = DecodeULEB128( pAb, &ui32 );
    pAb = DecodeULEB128( pAb, &tag );
    pAb++;

    for( ;; ) {
        pAb = DecodeULEB128( pAb, &attr );
        pAb = DecodeULEB128( pAb, &form );

        if( attr == 0 ) break;

        if( skip ) {
            p = SkipForm( p, ( uint_16 )form );
            continue;
        }

        /*
         | Save the relevant attributes.  We can decide either in this
         | switch statement or after processing the symbol whether or not
         | to save the symbol in _definedSymbols or not
         */
        switch( attr ) {
            case DW_AT_name :
                p = readString( p, &name );
                break;
            case DW_AT_declaration :

                /*
                 | If it's a declaration, we don't care about it in
                 | pass 1.  We handle these in pass 2.
                 */
                flag = *p;
                p++;
                if( flag ) {
                    skip = TRUE;
                }
                break;

            case DW_AT_external :
                isExternal = *p;
                p++;

                /*
                 | If symbol is only visible locally, then in pass2 we
                 | should only use it as a replacement within the same
                 | file.
                 */
                break;
            default :
                p = SkipForm( p, ( uint_16 )form );
                break;
        }
    }

    if( !skip && name != NULL && *name != '\0' ) {
        /*
         | Note that symbols in defined symbol table are stored without
         | the raw data.
         */
        if( !_definedSymbols.hasDefinition( name, tag )) {
            SymbolInfo * sym = new SymbolInfo( mbrIndex, offset, NULL, NULL, name, isExternal );
            if( sym == NULL ) {
                throw ErrOutOfMemory;
            }
            sym->setTag( tag );
            _definedSymbols.add( name, sym );
        } else {
            delete [] name;
        }
    } else {

        /*
         | This symbol isn't added to the defined symbol table because
         | it's only a declared sym or it has no name
         */
        if( name != NULL ) {
            delete [] name;
        }
    }
}


void DwarfFileMerger::updateSymbolStream( int mbrIndex,
                                          uint_32 oldOff,
                                          uint_32 newOff )
/********************************************************/
{
    int i;
    int j;
    MergeReference * ref;

    for( i = 0; i < _symbolStream.count(); i += 1 ) {
        RefVector & refs = _symbolStream[ i ]->refs();
        for( j = 0; j < refs.entries(); j += 1 ) {
            ref = refs[ j ];
            if(( ref->mbrIndex() == mbrIndex ) && ( ref->value() == oldOff )) {
                ref->setNewOffset( newOff );
            }
        }
    }
}

void DwarfFileMerger::updateFileIndex( uint_8 * & p, uint_16 form, int mbrIndex )
/*******************************************************************************/
{
    uint_32 oldFileIndex;
    int newFileIndex;
    uint_8 * pData;

    pData = p;
    p = readDataForm( p, form, &oldFileIndex );

    #if DEBUG_DUMP
    // _globalFileTable.dumpReplacementTable();
    #endif

    newFileIndex = _globalFileTable.find( mbrIndex, oldFileIndex );
    writeDataForm( pData, form, newFileIndex );
}

void DwarfFileMerger::writeDataForm( uint_8 * & p,
                                     uint_16 form,
                                     uint_32 data )
/*************************************************/
{
    switch( form ) {
        case DW_FORM_block1:
        case DW_FORM_flag:
        case DW_FORM_data1:
        case DW_FORM_ref1:
            *p = (uint_8) data;
            p++;
            break;
        case DW_FORM_block2:
        case DW_FORM_data2:
        case DW_FORM_ref2:
            *( uint_16 * )p = (uint_16) data;
            p += sizeof( uint_16 );
            break;
        case DW_FORM_block4:
        case DW_FORM_ref_addr:  // NYI: non-standard behaviour for form_ref
        case DW_FORM_data4:
        case DW_FORM_ref4:
            *( uint_32 * ) p = data;
            p += sizeof( uint_32 );
            break;
        case DW_FORM_sdata:
            p = EncodeLEB128( p, ( int_32 ) data );
            break;
        case DW_FORM_udata:
        case DW_FORM_ref_udata:
            p = EncodeULEB128( p, data );
            break;
        default:
            throw ErrInvalidFormat;
    }
}

uint_8 * DwarfFileMerger::readDataForm( uint_8 * p,
                                        uint_16 form,
                                        uint_32 * data )
/******************************************************/
// Reads any integer data form that will fit into a uint_32
//
{
    switch( form ) {
        case DW_FORM_block1:
        case DW_FORM_flag:
        case DW_FORM_data1:
        case DW_FORM_ref1:
            *data = *p;
            p += sizeof( uint_8 );
            break;
        case DW_FORM_block2:
        case DW_FORM_data2:
        case DW_FORM_ref2:
            *data = *( uint_16 * ) p;
            p += sizeof( uint_16 );
            break;
        case DW_FORM_block4:
        case DW_FORM_ref_addr:  // NYI: non-standard behaviour for form_ref
        case DW_FORM_data4:
        case DW_FORM_ref4:
            *data = *( uint_32 * ) p;
            p += sizeof( uint_32 );
            break;
        case DW_FORM_sdata:
            p = DecodeLEB128( p, ( int_32 * )data );
            break;
        case DW_FORM_udata:
        case DW_FORM_ref_udata:
            p = DecodeULEB128( p, data );
            break;
        case DW_FORM_indirect:
            uint_32 indirectForm;
            p = DecodeULEB128( p, &indirectForm );
            p = readDataForm( p, ( uint_16 ) indirectForm, data );
            break;
        default:
            throw ErrInvalidFormat;
    }
    return p;
}

MergeReference * DwarfFileMerger::processReference( uint_8 * & p,
                                                    uint_16 form,
                                                    uint_32 pos,
                                                    SymbolInfo * sym,
                                                    int mbrIndex )
/*******************************************************************/
// Parameters :
//  p - pointer to data in DIE
//  form - form of reference of data p points to
//  pos - current offset from beginning of DIE of reference
//  sym - the symbol of the DIE this is for
//
{
    uint_32 value;

    switch( form ) {
        case DW_FORM_ref1:
            value = *p;
            p += 1;
            break;
        case DW_FORM_ref2:
            value = *( uint_16 * )p;
            p += 2;
            break;
        case DW_FORM_ref_addr:  // NYI: non-standard behaviour for form_ref
        case DW_FORM_ref4:
            value = *( uint_32 * )p;
            p += 4;
            break;
        case DW_FORM_ref_udata:
            p = DecodeULEB128( p, &value );
            break;
        default:
            throw ErrInvalidFormat;
    }


    MergeReference * ref;
    ref = new MergeReference( mbrIndex, pos, value, form, sym );

    return( ref );
}

#if DEBUG_DUMP
/*
 | For debugging purposes :
 */

void DwarfFileMerger::dumpData()
/******************************/
{
    int i, j;
    printf( "+---- Symbol stream -------------------------------+\n" );

    for( i = 0; i < _symbolStream.count(); i += 1 ) {
        SymbolInfo * sym = _symbolStream[ i ];
        RefVector & refs = sym->refs();

        printf( " Symbol : %s\n", sym->name() );
        printf( "          Old Offset : 0x%lx\n", sym->oldOffset() );
        printf( "          New Offset : 0x%lx\n", sym->newOffset() );
        printf( "          References :\n" );
        printf( "                       (MBRIndex,old offset, new offset)\n" );

        for( j = 0; j < refs.entries(); j += 1 ) {
            MergeReference * ref;

            ref = refs[ j ];
            printf( "                       (%d, 0x%lx, 0x%lx)\n", ref->mbrIndex(), ref->value(), ref->newOffset() );
        }
    }

    printf( "+--------------------------------------------------+\n" );
}
#endif

void DwarfFileMerger::skipSymbol( uint_8 * & p, uint_8 * abbrev )
/***************************************************************/
{
    uint_32 form;
    uint_32 attr;

    for( ;; ) {
        abbrev = DecodeULEB128( abbrev, &attr );
        abbrev = DecodeULEB128( abbrev, &form );

        if( attr == 0 ) break;

        p = SkipForm( p, form );
    }

}

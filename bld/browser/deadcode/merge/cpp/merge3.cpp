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

void DwarfFileMerger::processInfoPass2( int mbrIndex )
/****************************************************/
{
    uint_8 * pSyms;
    unsigned long length;

    _mbrFileTable[ mbrIndex ]->getSectionRawData( DR_DEBUG_INFO, pSyms, &length );
    processSymbolsPass2( pSyms, mbrIndex, length );

}

// Pass 2 -----------------------------------------------------------------

void DwarfFileMerger::processSymbolsPass2( uint_8 * pSyms,
                                           int mbrIndex,
                                           unsigned long length )
/***************************************************************/
// In pass 1, we made a list of all the defined symbols out there that
// we want to retain.
// Now we go through the symbols again, trying to replace declared symbols
// with defined symbols.
//
// Still to do : if we keep a defined symbol, we can keep all its children
//               too; this could speed things up a bit
{
    uint_8 * pStart;
    SymbolInfo * sym;

    pStart = pSyms;

    pSyms += 11;

    while( pSyms - pStart < length ) {
        sym = new SymbolInfo( mbrIndex, pSyms - pStart, pSyms );
        if( sym == NULL ) {
            throw ErrOutOfMemory;
        }
        if( getSymbol( pSyms, mbrIndex, sym, length ) == FALSE ) {
            delete sym;
        }
    }
}

bool DwarfFileMerger::getSymbol( uint_8 * & p,
                                 int mbrIndex,
                                 SymbolInfo * sym,
                                 uint_32 length )
/************************************************/
// Returns FALSE if symbol should be deleted, TRUE otherwise
{
    bool rc;
    Abbreviation * pAbbrev;
    uint_32 abbrev_code;
    uint_32 tag;
    uint_8 * pAbData;
    uint_8 * pSym;
    uint_32 ui32;

    rc = TRUE;
    pSym = p;

    p = DecodeULEB128( p, &abbrev_code );

    if( abbrev_code == 0 ) {

        /*
         | We don't want to copy out the last NULL DIE, because it is the
         | sibling of the the comp unit header -- which is skipped
         */
        if(( sym->oldOffset() + ( p - pSym )) >= length ) {
            return FALSE;
        }
        sym->setLength(( uint_32 )( p - pSym ));
        _refUpdateTable.add( sym->mbrIndex(), sym->oldOffset(), _symbolStream.offset() );
        updateSymbolStream( sym->mbrIndex(), sym->oldOffset(), _symbolStream.offset() );
        sym->setNewOffset( _symbolStream.offset() );
        _symbolStream.addSymbol( sym );
        return rc;
    }

    pAbbrev = _abbrevList[ abbrev_code ];
    if( pAbbrev == NULL ) {
        throw ErrInvalidFormat;
    }
    sym->setAbbrev( pAbbrev );

    pAbData = pAbbrev->data;

    // Skip code
    pAbData = DecodeULEB128( pAbData, &ui32 );
    pAbData = DecodeULEB128( pAbData, &tag );
    sym->setTag( tag );

    if( tag == DW_TAG_compile_unit ) {
        pAbData++;
        skipSymbol( p, pAbData );

        if( _compUnitSymbol == NULL ) {
            /*
             | This is a tricky bit of business.  We don't write the offset
             | of the compilation unit header until later, but we need to
             | know its size in order to calculate the rest of the offsets
             | properly, so we assume that the compilation unit used will
             | be the first DW_TAG_compile_unit encountered.
             */
            sym->setLength( p - pSym );
            _compUnitSymbol = sym;

            // "+11" for the .debug_info header bytes to be written later

            _symbolStream.setOffset( sym->length() + 11 );

        } else {
            rc = FALSE;
        }
        return rc;                              // <-- early return!
    }

    sym->setHasChildren( *pAbData );

    pAbData++;

    /*
     * Process the attributes...
     */

    uint_32 attr;
    uint_32 form;
    bool    declaration = FALSE;
    bool    defined = FALSE;
    char *  name;

    name = NULL;
    for( ;; ) {
        pAbData = DecodeULEB128( pAbData, &attr );
        pAbData = DecodeULEB128( pAbData, &form );

        if( attr == 0 ) break;

        switch( attr ) {
            case DW_AT_sibling :
                MergeReference * ref;

                /*
                 | The sibling reference is a special type of reference.
                 | If we remove the symbol referenced as a sibling, we
                 | don't want to refer to the replacing symbol -- we want
                 | the reference to point at the sibling of the deleted
                 | symbol, which should be whatever symbol comes right
                 | after the deleted symbol and whatever kids it has
                 */
                ref = processReference( p, form, p - pSym, sym, mbrIndex );
                sym->setSiblingRef( ref );
                break;

            case DW_AT_name :
                p = readString( p, &name );
                sym->setName( name );

                break;
            case DW_AT_declaration :
                declaration = *p;
                p++;
                break;

            case DW_AT_decl_file :
                updateFileIndex( p, form, mbrIndex );
                break;

            case DW_AT_macro_info :
            case DW_AT_WATCOM_references_start :
                p = SkipForm( p, ( uint_16 ) form );
                break;

            default :
                /*
                 | Process references. We build a list of references that are made
                 | by the symbol.  At the end of processing of this symbol, this
                 | list may be used to determine which references need to be
                 | updated, which are OK; finally, what is left is a list of
                 | unresolved references.
                 */
                switch( form ) {
                    case DW_FORM_ref_addr :
                    case DW_FORM_ref1 :
                    case DW_FORM_ref2 :
                    case DW_FORM_ref4 :
                    case DW_FORM_ref8 :
                    case DW_FORM_ref_udata :
                        MergeReference * ref;

                        ref = processReference( p, form, p - pSym, sym, mbrIndex );
                        sym->addReference( ref );

                        break;

                    default :
                        p = SkipForm( p, ( uint_16 )form );
                        break;
                }
                break;
        }

    }

    sym->setLength(( uint_32 )( p - pSym ));

    if( sym->named() && _definedSymbols.hasDefinition( sym->name(), tag )) {

        SymbolInfo * defSym;

        defSym = _definedSymbols.isDefinition( sym );

        if( defSym != NULL ) {

            /*
             | Symbol defined -- need to check symbol stream to see
             | if anything needs updating, add an entry to the ref
             | replacement table, etc.
             */
            processDefinedSymbol( p, sym, defSym );

        } else {

            /*
             | Symbol is a declaration which can be replaced with
             | a defined symbol (hopefully!)
             */
            rc = processDeclaredSymbol( p, sym );
        }
    } else {

        /*
         | Symbol that we just want to pass through -- no deletion
         | necessary.
         */

        processBasicSymbol( p, sym );
    }

    return rc;
}

void DwarfFileMerger::processDefinedSymbol( uint_8 * & p,
                                            SymbolInfo * sym,
                                            SymbolInfo * defSym )
/***************************************************************/
// sym == symbol as read in pass 2
// defSym == symbol as read in pass 1 (in defined symbol table)
{
    /*
     | Add symbol to the stream, which should define the offset of
     | the symbol in the new, merged file.  We can use that to
     | process the waiting list of replaced symbols.
     */
    processBasicSymbol( p, sym );

    /*
     | Some previous symbols may need to be updated to refer to
     | this symbol.  In particular, if a previous symbol <P> needed to
     | be replaced by this symbol <S>, then we need to go back and update
     | any symbols in the symbol stream that refer to <P> so that they
     | refer to <S> instead.
     |
     */

    RefVector & list = defSym->refs();
    int i;

    for( i = 0; i < list.entries(); i += 1 ) {
        list[ i ]->setNewOffset( sym->newOffset() );
    }

    _waitingList.remove( &list );

}

bool DwarfFileMerger::processDeclaredSymbol( uint_8 * & p, SymbolInfo * sym )
/***************************************************************************/
// Returns TRUE if symbol was kept, FALSE is it wasn't (and should be deleted)
//
// When this is called, we know that sym is not a defined symbol to keep,
// since it isn't in the _definedSymbol table, but that there's at
// least one defined symbol with the same name
{
    AnyVector list;
    int i, j;
    SymbolInfo * other;
    SymbolInfo * replacement;

    other = NULL;
    replacement = NULL;

    /*
     | This symbol can possibly be replaced by a defined symbol.
     | We need to find one that's a good match.  Ie. our first
     | choice would be one that is defined in the same file; otherwise,
     | we'd choose any externally visible defined symbol in any other
     | file
     */

    _definedSymbols.find( sym->name(), list );
    for( i = 0; i < list.entries(); i += 1 ) {
        other = ( SymbolInfo * ) list[ i ];

        if( other->mbrIndex() == sym->mbrIndex() ) {
            replacement = other;
            break;
        }
        if( replacement == NULL && other->isExternal() ) {
            replacement = other;
        }
    }

    if( replacement == NULL ) {
        /*
         | No possible replacement, so just treat the symbol normally
         | and return.
         */
        processBasicSymbol( p, sym );
        return TRUE;
    }

    uint_32 newOffset;

    if( _refUpdateTable.getNewOffset( replacement->mbrIndex(),
                                      replacement->oldOffset(), newOffset )) {
        /*
         | The replacement has already been processed -- so we can
         | add to the reference update table.
         */
        _refUpdateTable.add( sym->mbrIndex(), sym->oldOffset(), newOffset );
        updateSymbolStream( sym->mbrIndex(), sym->oldOffset(), newOffset );

        sym->setNewOffset( newOffset );
        if( sym->hasChildren() ) {
            #if DEBUG_DUMP
            printf( "DAGLO ALERT CLASS <%d,(0x%lx,0x%lx)>\n", sym->mbrIndex(), sym->oldOffset(), newOffset );
            #endif
            processChildren( p, sym );
        }
    } else {

        /*
         | The replacement hasn't been encountered yet, so we don't know
         | what it's new offset is.  We have to store any references to
         | this symbol by any symbol already in the symbol stream in a list so
         | that we may update them later.
         */
        RefVector * refsWaiting;

        /*
         | Add <mbr Index, declared sym offset> = <replacement->refs>
         | to the waiting list.  Add the reference to replacement->refs
         |
         | When any reference to the declared sym is encountered, we
         | should check the waiting list first to see if we should just
         | add that reference to the waiting list.
         |
         | When the defined symbol is encountered, we should resolve all
         | the references in its reference list -- ie. just set their
         | value to the offset of the defined symbol.  Also, we should
         | go through the waiting list and remove all the pointers to
         | the defined sym's reference list.
         |
         */

        refsWaiting = & replacement->refs();
        _waitingList.add( sym->mbrIndex(), sym->oldOffset(), refsWaiting );

        /*
         | Any references to this symbol that occurred in any symbols
         | encountered previously need to be updated.
         */
        for( i = 0; i < _symbolStream.count(); i += 1 ) {
            RefVector & symRefs = _symbolStream[ i ]->refs();
            for( j = 0; j < symRefs.entries(); j += 1 ) {
                if( symRefs[ j ]->value() == replacement->oldOffset() ) {
                    refsWaiting->insert( symRefs[ j ]);
                }
            }
        }

        if( sym->hasChildren() ) {
            skipChildren( p, sym );
        }
    }

    return FALSE;
}

void DwarfFileMerger::skipChildren( uint_8 * & p, SymbolInfo * prt )
/******************************************************************/
// ITB -- I think that this has to put in updated references for
//        the children
{
    uint_32 abbrev_code;
    uint_32 ui32;
    Abbreviation * pAbbrev;
    uint_8 * pAbData;
    uint_32 level;
    uint_32 oldOff;
    uint_32 newOff;

    level = 0;

    for( ;; ) {
        // ITB -- here's where I add the reference update
        oldOff = prt->oldOffset() + (uint_32)(p - prt->data());
        newOff = prt->newOffset() + (uint_32)(p - prt->data());
        _refUpdateTable.add( prt->mbrIndex(), oldOff, newOff );
        updateSymbolStream( prt->mbrIndex(), oldOff, newOff );
        #if DEBUG_DUMP
        printf( "DAGLO HAZARD CLASS <%d, (0x%lx, 0x%lx)>\n", prt->mbrIndex(), oldOff, newOff );
        #endif

        p = DecodeULEB128( p, &abbrev_code );
        if( abbrev_code == 0 ) {
            if( level == 0 ) {
                break;
            }
            level -= 1;
            continue;
        }


        pAbbrev = _abbrevList[ abbrev_code ];
        if( pAbbrev == NULL ) {
            throw ErrInvalidFormat;
        }

        pAbData = pAbbrev->data;

        // Skip code, tag
        pAbData = DecodeULEB128( pAbData, &ui32 );
        pAbData = DecodeULEB128( pAbData, &ui32 );

        // Check if symbol has children
        if( *pAbData ) {
            level += 1;
        }

        pAbData++;

        skipSymbol( p, pAbData );
    }
}

void DwarfFileMerger::processBasicSymbol( uint_8 * & p, SymbolInfo * sym )
/************************************************************************/
// When we merge all these symbols together, it's certain that
// the fields that have reference forms will have to be updated
// because the offsets in the file will have changed for all
// DIEs (except possibly the first file processed)
{
    int i;
    uint_32 newOffset;
    RefVector & refs = sym->refs();
    MergeReference * ref;

    for( i = 0; i < refs.entries(); i += 1 ) {
        ref = refs[ i ];

        /*
         | The referred symbol has already been processed.  We don't
         | know whether that referred symbol was replaced by a
         | definition or not.  If it was replaced by a defined symbol
         | which was processed previous to this, or if the symbol
         | wasn't replaced at all, then there will be an
         | entry for <mbrIndex, referred symbol offset> in the
         | reference update table.
         |
         | If the referred symbol needs to be replaced, *but* the
         | defining symbol has not appeared yet, then it will appear
         | in the _waitingList, in which case, we add the reference
         | to that waiting list.
         */

        if( _refUpdateTable.getNewOffset( ref->mbrIndex(), ref->value(), newOffset )) {

            ref->setNewOffset( newOffset );

        } else {
            RefVector * refsWaiting;

            refsWaiting = _waitingList.findList( ref->mbrIndex(),
                                                 ref->value() );
            if( refsWaiting == NULL ) {
                /*
                 | This would mean that the referred symbol has not been
                 | processed yet (since it wasn't in the update table) and
                 | so we don't know the referred symbol's offset, nor do
                 | we know if the referred symbol needs to be replaced.  So,
                 | we can't do anything about this yet...
                 */
                 continue;
            }

            refsWaiting->insert( ref );
        }
    }
    /*
     | Add the new offset of this symbol to the reference update table so
     | that other symbols can be properly updated.
     */
    _refUpdateTable.add( sym->mbrIndex(), sym->oldOffset(), _symbolStream.offset() );
    updateSymbolStream( sym->mbrIndex(), sym->oldOffset(), _symbolStream.offset() );

    sym->setNewOffset( _symbolStream.offset() );

    if( sym->hasChildren() ) {
        processChildren( p, sym );

        /*
         | Length has changed because we've processed the children
         */
        sym->setLength( p - sym->data() );

        /*
         | Add the symbol to the symbol stream.  The symbol won't actually
         | be written out until all its references are resolved and all
         | previous symbols have been written out.
         */
        _symbolStream.addSymbol( sym );

        ref = sym->siblingRef();
        if( ref ) {
            ref->setNewOffset( _symbolStream.offset() );
        }
    } else {
        _symbolStream.addSymbol( sym );
    }

}

void DwarfFileMerger::processChildren( uint_8 * & p, SymbolInfo * sym )
/*********************************************************************/
{
    bool isLastSib;
    bool isChild;
    int level = 1;      // # of levels of children
    int firstChildRef;
    uint_32 offset;     // to calculate file offset once all children processed

    firstChildRef = sym->refs().entries();

    for( ;; ) {
        offset = ( p - sym->data() ) + sym->oldOffset();

        processChildSymbol( p, sym, isLastSib, isChild );
        if( isLastSib ) {
            level--;
            if( level == 0 ) {
                break;
            }
        } else if( isChild ) {
            level++;
        }
    }

    /*
     | We've got a list of all the references made by the child
     | symbols.  We resolve as many as possible; those that we can't
     | resolve, we'll add to the parent's symbol reference list
     |
     | Recall that sibling references are stored in refs as well, but we
     | know that those must all be resolved somehow, since no child
     | symbols are deleted.
     */
    int i;
    uint_32 newOffset;
    MergeReference * ref;

    for( i = firstChildRef; i < sym->refs().entries(); i += 1 ) {
        ref = sym->refs()[ i ];

        if( ref->value() <= offset ) {
            /*
             | The referred symbol has already been processed.  We don't
             | know whether that referred symbol was replaced by a
             | definition or not.  If it was replaced by a defined symbol
             | which was processed previous to this, or if the symbol
             | wasn't replaced at all, then there will be an
             | entry for <mbrIndex, referred symbol offset> in the
             | reference update table.
             |
             | If the referred symbol needs to be replaced, *but* the
             | defining symbol has not appeared yet, then it will appear
             | in the _waitingList, in which case, we add the reference
             | to the waiting list for the referred symbol.
             */

            if( _refUpdateTable.getNewOffset( ref->mbrIndex(), ref->value(), newOffset )) {

                ref->setNewOffset( newOffset );

            } else {
                RefVector * refsWaiting;

                refsWaiting = _waitingList.findList( ref->mbrIndex(),
                                                     ref->value() );
                if( refsWaiting == NULL ) {
                    /*
                     | This would mean that the referred symbol
                     | was processed and needs to be replaced by a
                     | defining symbol, but no symbol has referenced
                     | the referred symbol before this.  This is probably
                     | caused by a reference to the child of a deleted
                     | symbol.  We'll just set it to 0 and print a warning
                     | here, although something will have to be done about
                     | this in the future.
                     */

                    ref->setNewOffset( 0 );
                    const char * fname = _mbrFileTable[ sym->mbrIndex() ]->getFileName();
                    printf( "%s (0x%lX)\n", fname, sym->oldOffset() + ref->position() );
                    printf( "         Warning: [.debug_info]\n" );
                    printf( "         Unable to resolve reference for child of symbol.\n" );

                    #if DEBUG_DUMP
                    sym->dumpData();
                    printf( "Stopped on reference # %d\n", i );
                    dumpData();
                    _refUpdateTable.dumpData();
                    _definedSymbols.dumpData();
                    #endif
                    throw ErrInvalidFormat;
                } else {
                    refsWaiting->insert( ref );
                }
            }
        } else {
            /*
             | Just leave reference to be resolved later.
             */
        }
    }
}

void DwarfFileMerger::processChildSymbol( uint_8 * & p,
                                          SymbolInfo * sym,
                                          bool & isLastSib,
                                          bool & isChild )
/************************************************************************/
// If we decide that a symbol can be kept and has been written out to
// the symbol stream, we can also keep all the children.
//
// Children can just be read in as data (sort of appending all the
// data onto the parent symbol), except that we still need to resolve
// all the references and update the refUpdateTable with their position;
// other tables, such as _waitingList can be updated as if the references
// belonged to the parent.
//
{
    Abbreviation * pAbbrev;
    uint_32 abbrev_code;
    uint_32 tag;
    uint_8 * pAbData;
    uint_8 * pSym;
    uint_32 ui32;

    isLastSib = FALSE;
    isChild = FALSE;

    /*
     | Set start = start of parent symbol
     */
    pSym = sym->data();

    /*
     | The parent symbol has not been added to the symbol stream yet.
     | The offset of this child symbol is the offset of the original
     | parent + the difference in their offsets.  Similarly, the
     | offset in the merged file is the new offset of the parent + the
     | difference in their original offsets
     */
    _refUpdateTable.add( sym->mbrIndex(), sym->oldOffset() + p - pSym, _symbolStream.offset() + p - pSym );

    p = DecodeULEB128( p, &abbrev_code );

    if( abbrev_code == 0 ) {
        isLastSib = TRUE;
        return;
        // end of string of children
    }

    pAbbrev = _abbrevList[ abbrev_code ];
    if( pAbbrev == NULL ) {
        throw ErrInvalidFormat;
    }

    pAbData = pAbbrev->data;

    // Skip code
    pAbData = DecodeULEB128( pAbData, &ui32 );
    pAbData = DecodeULEB128( pAbData, &tag );

    isChild = *pAbData;

    pAbData++;

    /*
     * Process the attributes...
     */

    uint_32 attr;
    uint_32 form;
    bool    declaration = FALSE;
    bool    defined = FALSE;
    char *  name;

    name = NULL;
    for( ;; ) {
        pAbData = DecodeULEB128( pAbData, &attr );
        pAbData = DecodeULEB128( pAbData, &form );

        if( attr == 0 ) break;

        switch( attr ) {
            /*
             | Since all the children of the parent are guaranteed to
             | be kept, we know that the sibling pointers will be
             | resolved by the time the last child of the parent is
             | processed. Thus we shouldn't have to treat siblings
             | as special references in this case.
             */

            case DW_AT_decl_file :
                /*
                 | Be sneaky and update the actual memory location now.
                 */
                updateFileIndex( p, form, sym->mbrIndex() );
                break;

            case DW_AT_macro_info :
            case DW_AT_WATCOM_references_start :
                /*
                 | These are references, but they aren't references to
                 | symbols
                 */
                p = SkipForm( p, ( uint_16 ) form );
                break;

            default :
                /*
                 | Process references. We build a list of references that are made
                 | by the symbol.  At the end of processing of this symbol, this
                 | list may be used to determine which references need to be
                 | updated, which are OK; finally, what is left is a list of
                 | unresolved references.
                 */
                switch( form ) {
                    case DW_FORM_ref_addr :
                    case DW_FORM_ref1 :
                    case DW_FORM_ref2 :
                    case DW_FORM_ref4 :
                    case DW_FORM_ref8 :
                    case DW_FORM_ref_udata :
                        MergeReference * ref;

                        ref = processReference( p, form, p - pSym, sym, sym->mbrIndex() );
                        sym->addReference( ref );

                        break;

                    default :
                        p = SkipForm( p, ( uint_16 )form );
                        break;
                }
                break;
        }

    }
}

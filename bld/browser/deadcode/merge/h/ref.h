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


#ifndef _MergeReference_H
#define _MergeReference_H

// System includes --------------------------------------------------------

#include <wstd.h>
#include <string.h>
#include <wcvector.h>

class SymbolInfo;

class MergeReference {
public:
                        MergeReference( int mbrIndex, uint_32 position, uint_32 value, uint_16 form, SymbolInfo * symbol );
                        ~MergeReference();
    uint_32             position();
    uint_32             value();
    uint_16             form();
    int                 mbrIndex();
    void                setNewOffset( uint_32 newOffset );
    uint_32             newOffset();

    #if DEBUG_DUMP
    void                dumpData();
    #endif

protected:

    /*
     * This is the index of the MBR file associated with this MergeReference
     */
    int                 _mbrIndex;

    /*
     * This is the address of the data location holding the MergeReference
     * relative to the start of the symbol's address.
     *  ie. location of MergeReference = symbol address + position
     */
    uint_32             _position;

    /*
     * This is the value of the MergeReference.  IE. the address of the symbol
     * to which the MergeReference is pointing to.
     */
    uint_32             _value;

    /*
     * This is the type of MergeReference.  We need to keep track of this because
     * if we needed to convert a DW_FORM_ref1 to a DW_FORM_ref_addr, we
     * would be in trouble... Shouldn't happen for ours though.
     */
    uint_16             _form;

    /*
     * Pointer back to the symbol that owns this MergeReference
     */

    SymbolInfo *        _symbol;

    /*
     * Value of MergeReference = offset of referred DIE in merged file
     */
    uint_32             _newOffset;

};

typedef WCValOrderedVector<MergeReference *> RefVector;
typedef WCPtrOrderedVector<RefVector *> RefTable;

#endif

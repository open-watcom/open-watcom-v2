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


#include <wclistit.h>
#include "common.hpp"
#include "cverror.hpp"
#include "typerecord.hpp"
#include "subsect.hpp"
#include "typemap.hpp"

extern TypeIndexMap TypeMap;

MyNodeStack         LFGlobalTypeRecord::_nodeStack        = MyNodeStack();
Recorder            LFGlobalTypeRecord::_recorder         = Recorder();

PTF LFTypeRecord::_typeConstructorTable[] = {
    LFLeafStruct::Error,      // no 0th index.
    LFModifier::Construct,
    LFPointer::Construct,
    LFArray::Construct,
    LFClass::Construct,
    LFClass::Construct,
    LFUnion::Construct,
    LFEnum::Construct,
    LFProcedure::Construct,
    LFMFunction::Construct,
    LFVtShape::Construct,
    LFCobol0::Construct,
    LFCobol1::Construct,
    LFBArray::Construct,
    LFLabel::Construct,
    LFNull::Construct,
    LFNotTrans::Construct,
    LFDimArray::Construct,
    LFVftPath::Construct,
    LFLeafStruct::Error,      // precompile types.
    LFLeafStruct::Error,      // end of precompie types.
    LFLeafStruct::Error,      // oem generic types.
    LFLeafStruct::Error,      // undefined record lf_typeserve.
    LFLeafStruct::Error,      // skip records.
    LFArgList::Construct,
    LFDefArg::Construct,
    LFLeafStruct::Error,      // arbitrary list.
    LFFieldList::Construct,
    LFDerived::Construct,
    LFBitField::Construct,
    LFMethodList::Construct,
    LFDimConu::Construct,
    LFDimConlu::Construct,
    LFLeafStruct::Error,      // dim array with variable upper bound.
    LFLeafStruct::Error,      // dim array with variable upp/low bound.
    // refsym.
};

// unused function section :
bool LFTypeRecord::operator == ( const LFTypeRecord& ) const
{
    throw InternalError("LFTypeRecord == gets call");
}
// end section.

uint LFTypeRecord::ConvertIndex( const leaf_index leaf )
/******************************************************/
{
    // 0x0016 is LF_TYPESERVER, which hasn't been defined in cv headers.
    if ( leaf <= 0x0016 ) {
        return leaf;
    }
    if ( leaf >= LF_SKIP && leaf <= LF_REFSYM ) {
        return 0x17 + ( leaf - LF_SKIP );
    }
    return LF_NOTTRANS;
}

LFTypeRecord::LFTypeRecord( const type_index typeIndex,
                            const char*      buffer )
        : _length( * (unsigned_16 *) buffer ),
          _typeStr( NULL ),
          _typeIndex( typeIndex )
/*****************************************************/
{
    buffer += WORD;  // skip the length field.
    leaf_index index = * (unsigned_16 *) buffer;
    _typeStr = _typeConstructorTable[ConvertIndex(index)](buffer,_length);
}

LFTypeRecord::LFTypeRecord( LFTypeRecord* typeRecord )
        : _length ( typeRecord->_length ),
          _typeStr ( typeRecord->_typeStr),
          _typeIndex ( TypeMap.Lookup(typeRecord->_typeIndex) )
{
}

LFGlobalTypeRecord::LFGlobalTypeRecord( const type_index ti,
                                        const char* buffer )
        : LFTypeRecord( ti, buffer),
          _targetAdr( NULL )
/**********************************************************/
{
    _flags.isInLoop =  FALSE;
    _flags.isStopPoint = FALSE;
}

LFGlobalTypeRecord::LFGlobalTypeRecord( LFLocalTypeRecord* typeRecord )
        : LFTypeRecord( typeRecord ),
          _targetAdr( NULL )
/*********************************************************************/
{
    _flags.isInLoop =  FALSE;
    _flags.isStopPoint = FALSE;
}

bool LFGlobalTypeRecord::IsEquivalent(LFLocalTypeRecord& target)
/**************************************************************/
{
    if ( Length() != target.Length() ) {
        return FALSE;
    }
    //
    // If this node has been visited before, check if the target addresses
    // are the same. If they are not, that means the loop is not
    // equivalent between the two graphs in consideration.
    //
    if ( HasVisited() ) {
        if ( &target == _targetAdr ) {
            //
            // tell all the nodes that have been visited that we are now in
            // a loop, so that correct action will take place.
            //
            _nodeStack.MarkLoop(this);
            //
            // make an assumption that the loop node is equivalent.
            //
            return TRUE;
        }
        return FALSE;
    }
    MarkNode(&target);
    //
    // push the current node onto the stack for loop determination.
    //
    _nodeStack.Push(this);
    //
    // check to see if the actual type record is equivalent.
    //
    bool retVal = TypeString() -> IsEquivalent( *target.TypeString() );
    //
    // release the current node from the stack.
    //
    _nodeStack.Pop();
    if ( retVal ) {
        //
        // if equivalent, insert it as an old type.
        //
        SstGlobalTypes::InsertOldType(TypeIndex(), target.TypeIndex());
        if ( IsInLoop() ) {
            //
            // if this is a stop point of a loop, unmark all the nodes in
            // the loop for being in a loop and clear whatever the recorder
            // recorded.
            //
            if ( IsStopPoint() ) {
                _recorder.Clear();
                UnMarkNode();
                return TRUE;
            }
            //
            // if not a stop point, then record its index in case some node
            // in the loop prove to be not equivalent, so that things can
            // be undo.
            //
            _recorder.Record(target.TypeIndex());
            UnMarkNode();
            return TRUE;
        }
        UnMarkNode();
        return TRUE;
    }
    //
    // If in loop and the type record prove to be not equivalent, then
    // ask the recorder to unmark all the types that have been check in
    // into the mapping table.
    //
    if ( IsInLoop() ) {
        _recorder.RewindAndUndo();
    }
    UnMarkNode();
    return FALSE;
}

void Recorder::RewindAndUndo()
/****************************/
{
    while ( ! _recordLst.isEmpty() ) {
        TypeMap.UnMarkDone( _recordLst.pop() );
    }
}

void MyNodeStack::MarkLoop( LFGlobalTypeRecord* endAdr )
/******************************************************/
{
    WCPtrSListIter< LFGlobalTypeRecord >  iter(_nodeLst);
    while ( ++iter ) {
        if ( iter.current() == endAdr ) {
            break;
        }
        iter.current() -> MarkLoop(FALSE);
    }
    iter.current() -> MarkLoop(TRUE);
}

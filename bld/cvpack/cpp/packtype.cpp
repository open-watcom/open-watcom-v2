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


#include <string.h>

#include <wclistit.h>
#include "common.hpp"
#include "cverror.hpp"
#include "makeexe.hpp"
#include "packtype.hpp"
#include "typemap.hpp"
#include "typearay.hpp"

extern uint NumLeafLength( const char* );

extern TypeIndexMap TypeMap;

TypeArray*  LFLeafStruct::_localTypeArray;
TypeArray*  LFLeafStruct::_globalTypeArray;

PSF LFFieldList::_subFieldConstructorTable[] = {
    LFSubField::Error,
    LFBClass::Construct,
    LFVBClass::Construct,
    LFIVBClass::Construct,
    LFEnumerate::Construct,
    LFFriendFcn::Construct,
    LFIndex::Construct,
    LFMember::Construct1,
    LFStMember::Construct2,
    LFMethod::Construct,
    LFNestedType::Construct,
    LFVFuncTab::Construct,
    LFFriendCls::Construct,
    LFOneMethod::Construct,
    LFVFuncOff::Construct,
};

// unused function section :
bool LFLeafStruct::operator == ( const LFLeafStruct& ) const
{
    throw InternalError("LFLeafStruct == gets call");
}

bool LFSubField::operator == ( const LFSubField& ) const
{
    throw InternalError("LFSubField == gets call");
}

bool my_ct_mlist::operator == ( const my_ct_mlist& ) const
{
    throw InternalError("my_ct_mlist == gets call");
}
// end section.

LFLeafStruct::LFLeafStruct( const leaf_index  leaf,
                            const char*       rawBuffer,
                            const uint        variantSize )
        : _leaf(leaf),
          _variantString(rawBuffer,variantSize) { }

bool LFLeafStruct::IsEquivalent( const LFLeafStruct& LFRecord ) const
/*******************************************************************/
{
    return ( _leaf == LFRecord._leaf &&
             _variantString == LFRecord._variantString &&
             IsEqual(LFRecord) );
}

LFLeafStruct* LFLeafStruct::Error( const char*, const uint )
{
    throw InternalError("Invalid/Unsupported type index detected.");
}

void LFLeafStruct::SetLocalTypeArray( TypeArray& t ) {
    _localTypeArray = &t;
}

void LFLeafStruct::SetGlobalTypeArray( TypeArray& t ) {
    _globalTypeArray = &t;
}

void LFLeafStruct::Put( ExeMaker& eMaker ) const
{
    eMaker.DumpToExe(_leaf);
    DerivedPut(eMaker);
    eMaker.DumpToExe(_variantString);
}

bool LFLeafStruct::TypeEqual( const type_index refIndex,
                              const type_index targetIndex )
/**********************************************************/
{
    if ( refIndex < CV_FIRST_NONPRIM || targetIndex < CV_FIRST_NONPRIM ) {
        return ( refIndex == targetIndex );
    }
    if ( TypeMap.IsDone(targetIndex) ) {
        return ( refIndex == TypeMap.Lookup(targetIndex) );
    }
    return ( (*_globalTypeArray)[refIndex] -> IsEquivalent(*(*_localTypeArray)[targetIndex]));
}


bool LFLeafStruct::TypeListEqual( const type_index* refIndices,
                                  const type_index* tIndices,
                                  const uint        count ) {
    for ( uint i = 0; i < count; i++ ) {
        if ( ! TypeEqual(refIndices[i], tIndices[i] ) ) {
            return FALSE;
        }
    }
    return TRUE;
}

bool LFLeafStruct::AttrEqual( const cv_fldattr ref,
                              const cv_fldattr target )
/*****************************************************/
{
    return ( ref.f.access == target.f.access &&
             ref.f.mprop == target.f.mprop &&
             ref.f.pseudo == target.f.pseudo &&
             ref.f.noinherit == target.f.noinherit &&
             ref.f.noconstruct == target.f.noconstruct );
}

bool LFLeafStruct::AttrEqual( const cv_attrib ref,
                              const cv_attrib target )
/****************************************************/
{
    return ( ref.f.isconst == target.f.isconst &&
             ref.f.isvol == target.f.isvol &&
             ref.f.unalign == target.f.unalign );
}

bool LFLeafStruct::AttrEqual( const cv_ptrattr ref,
                              const cv_ptrattr target )
/*****************************************************/
{
    return ( ref.f.type == target.f.type &&
             ref.f.mode == target.f.mode &&
             ref.f.isflat32 == target.f.isflat32 &&
             ref.f.isvol == target.f.isvol &&
             ref.f.isconst == target.f.isconst &&
             ref.f.unaligned == target.f.unaligned );
}

bool LFLeafStruct::AttrEqual( const cv_sprop ref,
                              const cv_sprop target )
/***************************************************/
{
    return ( ref.f.packed == target.f.packed &&
             ref.f.ctor == target.f.ctor &&
             ref.f.overops == target.f.overops &&
             ref.f.isnested == target.f.isnested &&
             ref.f.cnested == target.f.cnested &&
             ref.f.opassign == target.f.opassign &&
             ref.f.opcast == target.f.opcast &&
             ref.f.fwdref == target.f.fwdref &&
             ref.f.scoped == target.f.scoped );
}

LFSubField::LFSubField( const leaf_index  leaf,
                        const uint        recLen,
                        const char*       var,
                        const uint        varLen )
        : LFLeafStruct( leaf, var, varLen ),
          _recordLength( recLen )
/************************************************/
{
}

bool LFModifier::IsEqual( const LFLeafStruct& target ) const
/**********************************************************/
{
    return ( AttrEqual(_modifier.attr, dc(target)._modifier.attr ) &&
             TypeEqual(_modifier.index, dc(target)._modifier.index) );
}

bool LFPointer::IsEqual( const LFLeafStruct& target ) const
/*********************************************************/
{
    return ( AttrEqual(_pointer.attr, dc(target)._pointer.attr) &&
             TypeEqual( _pointer.type, dc(target)._pointer.type ) &&
             TypeEqual( _trailingType, dc(target)._trailingType) );
}

bool LFArray::IsEqual( const LFLeafStruct& target ) const
/*******************************************************/
{
    return ( TypeEqual( _array.elemtype, dc(target)._array.elemtype) &&
             TypeEqual( _array.idxtype,dc(target)._array.idxtype) );
}

bool LFClass::IsEqual( const LFLeafStruct& target ) const
/*******************************************************/
{
    return ( _class.count == dc(target)._class.count &&
             AttrEqual( _class.property, dc(target)._class.property ) &&
             TypeEqual( _class.field, dc(target)._class.field ) &&
             TypeEqual( _class.vshape, dc(target)._class.vshape ) );
}

bool LFUnion::IsEqual( const LFLeafStruct& target ) const
/*******************************************************/
{
    return ( _union.count == dc(target)._union.count &&
             AttrEqual( _union.property, dc(target)._union.property ) &&
             TypeEqual( _union.field, dc(target)._union.field ) );
}

bool LFEnum::IsEqual( const LFLeafStruct& target ) const
/******************************************************/
{
    return ( _enum.count == dc(target)._enum.count &&
             AttrEqual( _enum.property, dc(target)._enum.property ) &&
             TypeEqual( _enum.type, dc(target)._enum.type) &&
             TypeEqual( _enum.fList, dc(target)._enum.fList ) );
}

bool LFProcedure::IsEqual( const LFLeafStruct& target ) const
/***********************************************************/
{
    return ( _procedure.call == dc(target)._procedure.call &&
             _procedure.parms == dc(target)._procedure.parms &&
             TypeEqual( _procedure.rvtype, dc(target)._procedure.rvtype ) &&
             TypeEqual( _procedure.arglist, dc(target)._procedure.arglist ) );
}

bool LFMFunction::IsEqual( const LFLeafStruct& target ) const
/***********************************************************/
{
    return ( _mFunction.call == dc(target)._mFunction.call &&
             _mFunction.parms == dc(target)._mFunction.parms &&
             _mFunction.thisadjust == dc(target)._mFunction.thisadjust &&
             TypeEqual( _mFunction.thisptr, dc(target)._mFunction.thisptr ) &&
             TypeEqual( _mFunction.rvtype, dc(target)._mFunction.rvtype ) &&
             TypeEqual( _mFunction.class_idx, dc(target)._mFunction.class_idx ) &&
             TypeEqual( _mFunction.arglist, dc(target)._mFunction.arglist ) );
}

bool LFVtShape::IsEqual( const LFLeafStruct& target ) const
/*********************************************************/
{
    return ( _vtShape.count == dc(target)._vtShape.count );
}

bool LFCobol0::IsEqual( const LFLeafStruct& target ) const
/********************************************************/
{
    return TypeEqual( _cobol0.parent, dc(target)._cobol0.parent );
}

bool LFBArray::IsEqual( const LFLeafStruct& target ) const
/********************************************************/
{
    return TypeEqual( _bArray.type, dc(target)._bArray.type );
}

bool LFLabel::IsEqual( const LFLeafStruct& target ) const
/*******************************************************/
{
    return ( _label.mode == dc(target)._label.mode );
}

bool LFDimArray::IsEqual( const LFLeafStruct& target ) const
/**********************************************************/
{
    return ( TypeEqual( _dimArray.diminfo, dc(target)._dimArray.diminfo ) &&
             TypeEqual( _dimArray.utype, dc(target)._dimArray.utype ) );
}

bool LFVftPath::IsEqual( const LFLeafStruct& target ) const
/*********************************************************/
{
    if ( _vftPath.count == dc(target)._vftPath.count ) {
        return TypeListEqual(_bases,dc(target)._bases,_vftPath.count);
    }
    return FALSE;
}

bool LFArgList::IsEqual( const LFLeafStruct& target ) const
/*********************************************************/
{
    if ( _argList.argcount == dc(target)._argList.argcount ) {
        return TypeListEqual(_indices,dc(target)._indices,_argList.argcount);
    }
    return FALSE;
}

bool LFDefArg::IsEqual( const LFLeafStruct& target ) const
/********************************************************/
{
    return TypeEqual( _defArg.index, dc(target)._defArg.index );
}

bool LFDerived::IsEqual( const LFLeafStruct& target ) const
/*********************************************************/
{
    if ( _derived.count == dc(target)._derived.count ) {
        return TypeListEqual(_indices, dc(target)._indices, _derived.count);
    }
    return FALSE;
}

bool LFBitField::IsEqual( const LFLeafStruct& target ) const
/**********************************************************/
{
    return ( _bitField.length == dc(target)._bitField.length &&
             _bitField.position == dc(target)._bitField.position  &&
             TypeEqual( _bitField.type, dc(target)._bitField.type) );
}

bool LFMethodList::IsEqual( const LFLeafStruct& target ) const
/************************************************************/
{
    if ( _mList -> entries() != dc(target)._mList -> entries() ) {
        return FALSE;
    }
    WCPtrConstSListIter<my_ct_mlist> refIter(*_mList);
    WCPtrConstSListIter<my_ct_mlist> tIter(*dc(target)._mList);
    my_ct_mlist* refPtr;
    my_ct_mlist* tPtr;

    while ( ++refIter && ++tIter ) {
        refPtr = refIter.current();
        tPtr = tIter.current();
        if ( refPtr -> vtab != tPtr-> vtab ||
             ! AttrEqual( refPtr -> attr, tPtr -> attr ) ||
             ! TypeEqual( refPtr -> type, tPtr -> type ) ) {
            return FALSE;
        }
    }
    return TRUE;
}

bool LFDimConu::IsEqual( const LFLeafStruct& target ) const
/*********************************************************/
{
    return ( _dimConu.rank == dc(target)._dimConu.rank &&
             TypeEqual(_dimConu.index, dc(target)._dimConu.index) );
}

bool LFDimConlu::IsEqual( const LFLeafStruct& target ) const
/**********************************************************/
{
    return ( _dimConlu.rank == dc(target)._dimConlu.rank &&
             TypeEqual(_dimConlu.index, dc(target)._dimConlu.index) );
}

// RefSym.

bool LFBClass::IsSubFieldEqual( const LFSubField& target ) const
/**************************************************************/
{
    return ( AttrEqual(_bClass.attr, dc(target)._bClass.attr) &&
             TypeEqual(_bClass.type, dc(target)._bClass.type) );
}

bool LFVBClass::IsSubFieldEqual( const LFSubField& target ) const
/***************************************************************/
{
    return ( AttrEqual(_vbClass.attr, dc(target)._vbClass.attr) &&
             TypeEqual(_vbClass.btype, dc(target)._vbClass.btype) &&
             TypeEqual(_vbClass.vtype, dc(target)._vbClass.vtype) );
}

bool LFEnumerate::IsSubFieldEqual( const LFSubField& target ) const
/*****************************************************************/
{
    return ( AttrEqual(_enumerate.attr, dc(target)._enumerate.attr) );
}

bool LFFriendFcn::IsSubFieldEqual( const LFSubField& target ) const
/*****************************************************************/
{
    return TypeEqual(_friendFcn.type, dc(target)._friendFcn.type);
}

bool LFIndex::IsSubFieldEqual( const LFSubField& target ) const
/*************************************************************/
{
    return ( TypeEqual(_index.index, dc(target)._index.index) );
}

bool LFMember::IsSubFieldEqual( const LFSubField& target ) const
/**************************************************************/
{
    return ( AttrEqual(_member.attr, dc(target)._member.attr) &&
             TypeEqual(_member.type, dc(target)._member.type) );
}

bool LFMethod::IsSubFieldEqual( const LFSubField& target ) const
/**************************************************************/
{
    return ( _method.count == dc(target)._method.count &&
             TypeEqual(_method.mList,dc(target)._method.mList) );
}

bool LFNestedType::IsSubFieldEqual( const LFSubField& target ) const
/******************************************************************/
{
    return TypeEqual(_nestedType.index, dc(target)._nestedType.index);
}

bool LFVFuncTab::IsSubFieldEqual( const LFSubField& target ) const
/****************************************************************/
{
    return TypeEqual(_vFuncTab.type, dc(target)._vFuncTab.type);
}

bool LFFriendCls::IsSubFieldEqual( const LFSubField& target ) const
/*****************************************************************/
{
    return TypeEqual(_friendCls.type, dc(target)._friendCls.type);
}

bool LFOneMethod::IsSubFieldEqual( const LFSubField& target ) const
/*****************************************************************/
{
    return ( AttrEqual(_oneMethod.attr, dc(target)._oneMethod.attr) &&
             TypeEqual(_oneMethod.type, dc(target)._oneMethod.type) );
}

bool LFVFuncOff::IsSubFieldEqual( const LFSubField& target ) const
/****************************************************************/
{
    return ( _vFuncOff.offset == dc(target)._vFuncOff.offset &&
             TypeEqual(_vFuncOff.type, dc(target)._vFuncOff.type) );
}

bool LFSubField::IsEqual( const LFLeafStruct& target ) const
/**********************************************************/
{
    return ( _recordLength == dc(target)._recordLength &&
             IsSubFieldEqual(dc(target)) );
}

bool LFFieldList::IsEqual( const LFLeafStruct& target ) const
/***********************************************************/
{
    WCPtrConstSListIter<LFSubField> refIter( _subFieldList );
    WCPtrConstSListIter<LFSubField> targetIter( dc(target)._subFieldList );
    while ( ++targetIter && ++refIter ) {
        if ( ! refIter.current() -> IsEquivalent( *targetIter.current() ) ) {
            return FALSE;
        }
    }
    return TRUE;
}

void LFLeafStruct::FixTypeList( type_index*       tList,
                                const uint        count )
/*******************************************************/
{
    for ( uint i = 0; i < count; i++ ) {
        tList[i] = TypeMap.Lookup(tList[i]);
    }
}

void LFModifier::FixTypeIndex()
/*****************************/
{
    _modifier.index = TypeMap.Lookup(_modifier.index);
}

void LFPointer::FixTypeIndex()
/****************************/
{
    _pointer.type = TypeMap.Lookup(_pointer.type);
}

void LFArray::FixTypeIndex()
/**************************/
{
    _array.elemtype = TypeMap.Lookup(_array.elemtype);
    _array.idxtype = TypeMap.Lookup(_array.idxtype);
}

void LFClass::FixTypeIndex()
/**************************/
{
    _class.field = TypeMap.Lookup(_class.field);
    _class.dList = TypeMap.Lookup(_class.dList);
    _class.vshape = TypeMap.Lookup(_class.vshape);
}

void LFUnion::FixTypeIndex()
/**************************/
{
    _union.field = TypeMap.Lookup(_union.field);
}

void LFEnum::FixTypeIndex()
/*************************/
{
    _enum.type = TypeMap.Lookup(_enum.type);
    _enum.fList = TypeMap.Lookup(_enum.fList);
}

void LFProcedure::FixTypeIndex()
/******************************/
{
    _procedure.rvtype = TypeMap.Lookup(_procedure.rvtype);
    _procedure.arglist = TypeMap.Lookup(_procedure.arglist);
}

void LFMFunction::FixTypeIndex()
/******************************/
{
    _mFunction.rvtype = TypeMap.Lookup(_mFunction.rvtype);
    _mFunction.class_idx = TypeMap.Lookup(_mFunction.class_idx);
    _mFunction.thisptr = TypeMap.Lookup(_mFunction.thisptr);
    _mFunction.arglist = TypeMap.Lookup(_mFunction.arglist);
}

void LFCobol0::FixTypeIndex()
/***************************/
{
    _cobol0.parent = TypeMap.Lookup(_cobol0.parent);
}

void LFCobol1::FixTypeIndex()
/***************************/
{
}

void LFBArray::FixTypeIndex()
/***************************/
{
    _bArray.type = TypeMap.Lookup(_bArray.type);
}

void LFLabel::FixTypeIndex()
/***************************/
{
}

void LFDimArray::FixTypeIndex()
/*****************************/
{
    _dimArray.utype = TypeMap.Lookup(_dimArray.utype);
    _dimArray.diminfo = TypeMap.Lookup(_dimArray.diminfo);
}

void LFVftPath::FixTypeIndex()
/****************************/
{
    LFLeafStruct::FixTypeList(_bases,_vftPath.count);
}

void LFArgList::FixTypeIndex()
/****************************/
{
    LFLeafStruct::FixTypeList(_indices,_argList.argcount);
}

void LFDefArg::FixTypeIndex()
/***************************/
{
    _defArg.index = TypeMap.Lookup(_defArg.index);
}

void LFFieldList::FixTypeIndex()
/******************************/
{
    WCPtrSListIter<LFSubField> iter(_subFieldList);
    while ( ++iter ) {
        iter.current() -> FixTypeIndex();
    }
}

void LFDerived::FixTypeIndex()
/**************************/
{
    LFLeafStruct::FixTypeList(_indices, _derived.count);
}

void LFBitField::FixTypeIndex()
/***************************/
{
    _bitField.type = TypeMap.Lookup(_bitField.type);
}

void LFMethodList::FixTypeIndex()
/*******************************/
{
    WCPtrConstSListIter<my_ct_mlist> iter(*_mList);
    while ( ++iter ) {
        iter.current() -> type = TypeMap.Lookup(iter.current()->type);
    }
}

void LFDimConu::FixTypeIndex()
/****************************/
{
    _dimConu.index = TypeMap.Lookup(_dimConu.index);
}

void LFDimConlu::FixTypeIndex()
/*****************************/
{
    _dimConlu.index = TypeMap.Lookup(_dimConlu.index);
}

void LFBClass::FixTypeIndex()
/***************************/
{
    _bClass.type = TypeMap.Lookup(_bClass.type);
}

void LFVBClass::FixTypeIndex()
/****************************/
{
    _vbClass.btype = TypeMap.Lookup(_vbClass.btype);
    _vbClass.vtype = TypeMap.Lookup(_vbClass.vtype);
}

void LFEnumerate::FixTypeIndex()
/*****************************/
{
}

void LFFriendFcn::FixTypeIndex()
/******************************/
{
    _friendFcn.type = TypeMap.Lookup(_friendFcn.type);
}

void LFIndex::FixTypeIndex()
/**************************/
{
    _index.index = TypeMap.Lookup(_index.index);
}

void LFMember::FixTypeIndex()
/***************************/
{
    _member.type = TypeMap.Lookup(_member.type);
}

void LFMethod::FixTypeIndex()
/***************************/
{
    _method.mList = TypeMap.Lookup(_method.mList);
}

void LFNestedType::FixTypeIndex()
/*******************************/
{
    _nestedType.index = TypeMap.Lookup(_nestedType.index);
}

void LFVFuncTab::FixTypeIndex()
/*****************************/
{
    _vFuncTab.type = TypeMap.Lookup(_vFuncTab.type);
}

void LFFriendCls::FixTypeIndex()
/******************************/
{
    _friendCls.type = TypeMap.Lookup(_friendCls.type);
}

void LFOneMethod::FixTypeIndex()
/******************************/
{
    _oneMethod.type = TypeMap.Lookup(_oneMethod.type);
}

void LFVFuncOff::FixTypeIndex()
/*****************************/
{
    _vFuncOff.type = TypeMap.Lookup(_vFuncOff.type);
}


LFLeafStruct* LFNotTrans::Construct( const char*, const uint )
/************************************************************/
{
    return new LFNotTrans();
}

LFLeafStruct* LFNull::Construct( const char*, const uint)
/*******************************************************/
{
    return new LFNull();
}

LFLeafStruct* LFModifier::Construct( const char* ptr,
                                     const uint  length )
/*******************************************************/
{
    return new LFModifier ( *(lf_modifier *) (ptr),
                            ptr + sizeof(lf_modifier),
                            length - sizeof(lf_modifier) );
}

LFLeafStruct* LFPointer::Construct( const char* ptr,
                                    const uint  length )
/******************************************************/
{
    lf_pointer pointer = * (lf_pointer*) ptr;
    ptr += sizeof(lf_pointer);
    type_index trailingType = NO_TYPING_INFO;
    uint       trailingLen = length - sizeof(lf_pointer);
    if ( pointer.f.attr.f.type == CV_BASETYPE ||
         pointer.f.attr.f.mode == CV_PTRTOMEMBER ||
         pointer.f.attr.f.mode == CV_PTRTOMETHOD ) {
        trailingType = * (type_index *) ptr;
        ptr += WORD;
        trailingLen -= WORD;
    }
    return new LFPointer (  pointer, ptr, trailingLen, trailingType );
}

LFLeafStruct* LFArray::Construct( const char* ptr,
                                  const uint  length )
/****************************************************/
{
    return new LFArray ( *(lf_array *) (ptr),
                         ptr + sizeof(lf_array),
                         length - sizeof(lf_array) );
}

LFLeafStruct* LFClass::Construct( const char* ptr,
                                  const uint  length )
/****************************************************/
{
    return new LFClass ( *(lf_class *) (ptr),
                         ptr + sizeof(lf_class),
                         length - sizeof(lf_class) );
}

LFLeafStruct* LFUnion::Construct( const char* ptr,
                                  const uint  length )
/****************************************************/
{
    return new LFUnion ( *(lf_union *) (ptr),
                         ptr + sizeof(lf_union),
                         length - sizeof(lf_union) );
}

LFLeafStruct* LFEnum::Construct( const char* ptr,
                                 const uint  length )
/***************************************************/
{
    return new LFEnum ( *(lf_enum *) (ptr),
                        ptr + sizeof(lf_enum),
                        length - sizeof(lf_enum) );
}

LFLeafStruct* LFProcedure::Construct( const char* ptr,
                                      const uint  length )
/********************************************************/
{
    return new LFProcedure ( *(lf_procedure *) (ptr),
                             ptr + sizeof(lf_procedure),
                             length - sizeof(lf_procedure) );
}

LFLeafStruct* LFMFunction::Construct( const char* ptr,
                                      const uint  length )
/********************************************************/
{
    return new LFMFunction ( *(lf_mfunction *) (ptr),
                             ptr + sizeof(lf_mfunction),
                             length - sizeof(lf_mfunction) );
}

LFLeafStruct* LFVtShape::Construct( const char* ptr,
                                    const uint  length )
/******************************************************/
{
    return new LFVtShape ( *(lf_vtshape *) (ptr),
                           ptr + sizeof(lf_vtshape),
                           length - sizeof(lf_vtshape) );
}

LFLeafStruct* LFCobol0::Construct( const char* ptr,
                                   const uint  length )
/*****************************************************/
{
    return new LFCobol0 ( *(lf_cobol0 *) (ptr),
                          ptr + sizeof(lf_cobol0),
                          length - sizeof(lf_cobol0) );
}

LFLeafStruct* LFCobol1::Construct( const char* ptr,
                                   const uint  length )
/*****************************************************/
{
    return new LFCobol1 ( *(lf_cobol1 *) (ptr),
                          ptr + sizeof(lf_cobol1),
                          length - sizeof(lf_cobol1) );
}

LFLeafStruct* LFBArray::Construct( const char* ptr,
                                   const uint  length )
/*****************************************************/
{
    return new LFBArray ( *(lf_barray *) (ptr),
                          ptr + sizeof(lf_barray),
                          length - sizeof(lf_barray) );
}

LFLeafStruct* LFLabel::Construct( const char* ptr,
                                  const uint  length )
/****************************************************/
{
    return new LFLabel ( *(lf_label *) (ptr),
                         ptr + sizeof(lf_label),
                         length - sizeof(lf_label) );
}

LFLeafStruct* LFDimArray::Construct( const char* ptr,
                                     const uint  length )
/*******************************************************/
{
    return new LFDimArray ( *(lf_dimarray *) (ptr),
                            ptr + sizeof(lf_dimarray),
                            length - sizeof(lf_dimarray) );
}

LFLeafStruct* LFVftPath::Construct( const char* ptr,
                                    const uint  length )
/******************************************************/
{
    const char* end = &ptr[length];
    lf_vftpath vftPath = * (lf_vftpath *) ptr;
    ptr += sizeof( lf_vftpath );
    type_index* indices = NULL;
    if ( vftPath.f.count > 0 ) {
        indices = new type_index [vftPath.f.count];
        for ( uint i = 0; i < vftPath.f.count; i++ ) {
            indices[i] = * ( type_index * ) ptr;
            ptr += sizeof( type_index );
        }
    }
    return new LFVftPath ( vftPath, ptr, end-ptr, indices );
}

LFLeafStruct* LFArgList::Construct( const char* ptr,
                                    const uint  length )
/******************************************************/
{
    const char* end = &ptr[length];
    lf_arglist argList = * (lf_arglist *) ptr;
    ptr += sizeof( lf_arglist );
    type_index* indices = NULL;
    if ( argList.f.argcount > 0 ) {
        indices = new type_index [argList.f.argcount];
        for ( uint i = 0; i < argList.f.argcount; i++ ) {
            indices[i] = * ( type_index * ) ptr;
            ptr += sizeof( type_index );
        }
    }
    return new LFArgList ( argList, ptr, end-ptr, indices );
}

LFLeafStruct* LFDefArg::Construct( const char* ptr,
                                   const uint  length )
/*****************************************************/
{
    return new LFDefArg ( *(lf_defarg *) (ptr),
                          ptr + sizeof(lf_defarg),
                          length - sizeof(lf_defarg) );
}

LFLeafStruct* LFDerived::Construct( const char* ptr,
                                    const uint  length )
/******************************************************/
{
    const char* end = &ptr[length];
    lf_derived derived = * (lf_derived *) ptr;
    ptr += sizeof( lf_derived );
    type_index* indices = NULL;
    if ( derived.f.count > 0 ) {
        indices = new type_index [derived.f.count];
        for ( uint i = 0; i < derived.f.count; i++ ) {
            indices[i] = * ( type_index * ) ptr;
            ptr += sizeof( type_index );
        }
    }
    return new LFDerived ( derived, ptr, end-ptr, indices );
}

LFLeafStruct* LFBitField::Construct( const char* ptr,
                                     const uint  length )
/*******************************************************/
{
    return new LFBitField ( *(lf_bitfield *) (ptr),
                            ptr + sizeof(lf_bitfield),
                            length - sizeof(lf_bitfield) );
}

LFLeafStruct* LFMethodList::Construct( const char* ptr,
                                       const uint  length )
/*********************************************************/
{
    //
    // The structure itself are already align, no need to consider pad bytes.
    //
    const char*              end  = &ptr[length];
    leaf_index               leaf = * (leaf_index *) ptr;
    my_ct_mlist*             currentPtr;
    WCPtrSList<my_ct_mlist>* mLnkLst = new WCPtrSList<my_ct_mlist>;
    ptr += WORD;          // skip leaf.
    while ( ptr < end ) {
        currentPtr = new my_ct_mlist;
        currentPtr -> attr = (( my_ct_mlist * ) ptr) -> attr;
        currentPtr -> type = (( my_ct_mlist * ) ptr) -> type;
        if ( ( ((my_ct_mlist *) ptr) -> attr).f.mprop == CV_INTROVIRT ) {
            currentPtr -> vtab = (( my_ct_mlist * ) ptr) -> vtab;
            ptr += sizeof(my_ct_mlist);
        } else {
            currentPtr -> vtab = NO_VTAB_OFFSET;
            ptr += sizeof(my_ct_mlist) - LONG_WORD;
        }
        mLnkLst -> append( currentPtr );
    }
    return new LFMethodList( leaf, mLnkLst );
}

LFLeafStruct* LFDimConu::Construct( const char* ptr,
                                    const uint  length )
/******************************************************/
{
    return new LFDimConu ( *(lf_dimconu *) (ptr),
                           ptr + sizeof(lf_dimconu),
                           length - sizeof(lf_dimconu) );
}

LFLeafStruct* LFDimConlu::Construct( const char* ptr,
                                     const uint  length )
/*******************************************************/
{
    return new LFDimConlu ( *(lf_dimconlu *) (ptr),
                            ptr + sizeof(lf_dimconlu),
                            length - sizeof(lf_dimconlu) );
}

LFFieldList::LFFieldList( const char* ptr,
                          const uint  length )
/********************************************/
        : LFLeafStruct( * ( leaf_index *) ptr )
        // assume there are no pad bytes follow as sub fields are align.
        // makesure the compiler align subfield though...
{
    leaf_index  index;
    const char* end = &ptr[length];
    LFSubField* subFieldPtr = NULL;
    ptr += WORD;    // skip leaf of fieldlist.
    while ( ptr < end ) {
        index = * ( leaf_index * ) ptr;
        subFieldPtr = _subFieldConstructorTable[ConvertIndex(index)](ptr);
        ptr += subFieldPtr -> RecordLength();
        _subFieldList.append(subFieldPtr);
    }
}

uint LFFieldList::ConvertIndex( const leaf_index leaf )
/*****************************************************/
{
    if ( leaf >= LF_BCLASS && leaf <= LF_VFUNCOFF ) {
        return ( leaf - LF_BCLASS + 1 );
    }
    throw InternalError("Undefined leaf index encountered in fieldlist.");
}

LFLeafStruct* LFFieldList::Construct( const char* ptr,
                                      const uint  length )
/********************************************************/
{
    return new LFFieldList( ptr, length );
}

LFSubField* LFSubField::Error( const char* )
{
    throw InternalError("Invalid/Unsupported leaf index detected.");
}

unsigned_8 LFSubField::PadLen( const char* ptr )
/**********************************************/
{
    return (unsigned_8) ( (*ptr > LF_PAD0) ? (*ptr - LF_PAD0 ) : 0 );
}

LFSubField* LFBClass::Construct( const char* ptr )
/************************************************/
{
    uint offsetLen = ::NumLeafLength(ptr+sizeof(lf_bclass));
    return new LFBClass( * (lf_bclass *) ptr,
                         ptr + sizeof(lf_bclass),
                         offsetLen + PadLen(ptr+sizeof(lf_bclass)+offsetLen));

}

LFSubField* LFVBClass::Construct( const char* ptr )
/*************************************************/
{
    uint vbpoffLen = ::NumLeafLength( ptr+sizeof(lf_vbclass) );
    uint vboffLen  = ::NumLeafLength( ptr+sizeof(lf_vbclass)+vbpoffLen );
    return new LFVBClass( * (lf_vbclass *) ptr,
                          ptr + sizeof(lf_vbclass),
                          vbpoffLen + vboffLen +
                          PadLen(ptr+sizeof(lf_vbclass)+vbpoffLen+vboffLen));
}

LFSubField* LFEnumerate::Construct( const char* ptr )
/***************************************************/
{
    uint valueLen = ::NumLeafLength( ptr + sizeof(lf_enumerate) );
    uint nameLen  = ::PrefixStrLen(ptr+sizeof(lf_enumerate)+valueLen);
    return new LFEnumerate( * (lf_enumerate *) ptr,
                            ptr + sizeof(lf_enumerate),
                            valueLen + nameLen +
                            PadLen(ptr+sizeof(lf_enumerate)+valueLen+nameLen));
}

LFSubField* LFFriendFcn::Construct( const char* ptr )
/***************************************************/
{
    uint nameLen = ::PrefixStrLen(ptr+sizeof(lf_friendfcn));
    return new LFFriendFcn( * (lf_friendfcn *) ptr,
                            ptr + sizeof(lf_friendfcn),
                            nameLen + PadLen(ptr+sizeof(lf_friendfcn)+nameLen));

}

LFSubField* LFIndex::Construct( const char* ptr )
/***********************************************/
{
    return new LFIndex( * (lf_index *) ptr ); // struct already align.
}

LFSubField* LFMember::Construct1( const char* ptr )
/*************************************************/
{
    uint offsetLen = ::NumLeafLength( ptr + sizeof(lf_member) );
    uint nameLen   = ::PrefixStrLen(ptr+sizeof(lf_member)+offsetLen);
    return new LFMember( * (lf_member *) ptr,
                          ptr + sizeof(lf_member),
                          offsetLen + nameLen +
                          PadLen(ptr+sizeof(lf_member)+offsetLen+nameLen));
}

LFSubField* LFMember::Construct2( const char* ptr )
/*************************************************/
{
    uint nameLen = PrefixStrLen(ptr+sizeof(lf_member));
    return new LFMember( * (lf_member *) ptr,
                         ptr + sizeof(lf_member),
                         nameLen + PadLen(ptr+sizeof(lf_member)+nameLen));

}

LFSubField* LFMethod::Construct( const char* ptr )
/************************************************/
{
    uint nameLen = PrefixStrLen(ptr+sizeof(lf_method));
    return new LFMethod( * (lf_method *) ptr,
                         ptr + sizeof(lf_method),
                         nameLen + PadLen(ptr+sizeof(lf_method)+nameLen));

}

LFSubField* LFNestedType::Construct( const char* ptr )
/****************************************************/
{
    uint nameLen = PrefixStrLen(ptr+sizeof(lf_nestedtype));
    return new LFNestedType( * (lf_nestedtype *) ptr,
                             ptr + sizeof(lf_nestedtype),
                             nameLen+PadLen(ptr+sizeof(lf_nestedtype)+nameLen));

}

LFSubField* LFVFuncTab::Construct( const char* ptr )
/**************************************************/
{
    return new LFVFuncTab( * (lf_vfunctab *) ptr ); // struct already align.
}

LFSubField* LFFriendCls::Construct( const char* ptr )
/***************************************************/
{
    return new LFFriendCls( * (lf_friendcls *) ptr ); // already align.
}

LFSubField* LFOneMethod::Construct( const char* ptr )
/***************************************************/
{
    uint nameLen = PrefixStrLen(ptr+sizeof(lf_onemethod));
    return new LFOneMethod( * (lf_onemethod *) ptr,
                            ptr + sizeof(lf_onemethod),
                            nameLen+PadLen(ptr+sizeof(lf_onemethod)+nameLen));

}

LFSubField* LFVFuncOff::Construct( const char* ptr )
/**************************************************/
{
    return new LFVFuncOff( * (lf_vfuncoff *) ptr ); // already align.
}


//////
void LFNotTrans::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe( (unsigned_16) 0 ); // padding.
}

void LFNull::DerivedPut( ExeMaker& eMaker ) const
/***********************************************/
{
    eMaker.DumpToExe( (unsigned_16) 0 ); // padding.
}

void LFModifier::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_modifier,sizeof(ct_modifier));
}

void LFPointer::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_pointer,sizeof(ct_pointer));
    if ( _trailingType != NO_TYPING_INFO ) {
        eMaker.DumpToExe( (unsigned_16) _trailingType);
    }
}

void LFArray::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_array,sizeof(ct_array));
}

void LFClass::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_class,sizeof(ct_class));
}

void LFUnion::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_union,sizeof(ct_union));
}

void LFEnum::DerivedPut( ExeMaker& eMaker ) const
/***********************************************/
{
    eMaker.DumpToExe(&_enum,sizeof(ct_enum));
}

void LFProcedure::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_procedure,sizeof(ct_procedure));
}

void LFMFunction::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_mFunction,sizeof(ct_mfunction));
}

void LFVtShape::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_vtShape,sizeof(ct_vtshape));
}

void LFCobol0::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_cobol0,sizeof(ct_cobol0));
}

void LFBArray::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_bArray,sizeof(ct_barray));
}

void LFLabel::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_label,sizeof(ct_label));
}

void LFDimArray::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_dimArray,sizeof(ct_dimarray));
}

void LFVftPath::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_vftPath,sizeof(ct_vftpath));
    eMaker.DumpToExe(_bases, sizeof(type_index)*_vftPath.count);
}

void LFArgList::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_argList,sizeof(ct_arglist));
    eMaker.DumpToExe(_indices, sizeof(type_index)*_argList.argcount);
}

void LFDefArg::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_defArg,sizeof(ct_defarg));
}

void LFFieldList::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    WCPtrConstSListIter<LFSubField> iter(_subFieldList);
    while ( ++iter ) {
        iter.current() -> Put(eMaker);
    }
}

void LFDerived::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_derived,sizeof(ct_derived));
    eMaker.DumpToExe(_indices,sizeof(type_index)*_derived.count);
}

void LFBitField::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_bitField,sizeof(ct_bitfield));
}

void LFMethodList::DerivedPut( ExeMaker& eMaker ) const
/*****************************************************/
{
    WCPtrConstSListIter<my_ct_mlist> iter(*_mList);
    my_ct_mlist*   currentPtr;
    while ( ++iter ) {
        currentPtr = iter.current();
        eMaker.DumpToExe( &(currentPtr->attr),sizeof(cv_fldattr));
        eMaker.DumpToExe( currentPtr -> type );
        if ( currentPtr -> vtab != NO_VTAB_OFFSET ) {
            eMaker.DumpToExe(currentPtr -> vtab);
        }
    }
}

void LFDimConu::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_dimConu,sizeof(ct_dimconu));
}

void LFDimConlu::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_dimConlu,sizeof(ct_dimconlu));
}

void LFBClass::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_bClass,sizeof(ct_bclass));
}

void LFVBClass::DerivedPut( ExeMaker& eMaker ) const
/**************************************************/
{
    eMaker.DumpToExe(&_vbClass,sizeof(ct_vbclass));
}

void LFEnumerate::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_enumerate,sizeof(ct_enumerate));
}

void LFFriendFcn::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_friendFcn,sizeof(ct_friendfcn));
}

void LFIndex::DerivedPut( ExeMaker& eMaker ) const
/************************************************/
{
    eMaker.DumpToExe(&_index,sizeof(ct_index));
}

void LFMember::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_member,sizeof(ct_member));
}

void LFMethod::DerivedPut( ExeMaker& eMaker ) const
/*************************************************/
{
    eMaker.DumpToExe(&_method,sizeof(ct_method));
}

void LFNestedType::DerivedPut( ExeMaker& eMaker ) const
/*****************************************************/
{
    eMaker.DumpToExe(&_nestedType,sizeof(ct_nestedtype));
}

void LFVFuncTab::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_vFuncTab,sizeof(ct_vfunctab));
}

void LFFriendCls::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_friendCls,sizeof(ct_friendcls));
}

void LFOneMethod::DerivedPut( ExeMaker& eMaker ) const
/****************************************************/
{
    eMaker.DumpToExe(&_oneMethod,sizeof(ct_onemethod));
}

void LFVFuncOff::DerivedPut( ExeMaker& eMaker ) const
/***************************************************/
{
    eMaker.DumpToExe(&_vFuncOff,sizeof(ct_vfuncoff));
}

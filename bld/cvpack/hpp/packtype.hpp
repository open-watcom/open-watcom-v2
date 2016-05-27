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
* Description:  Definitions of type classes and other supporting classes
*               for CV4 type packing.
*
****************************************************************************/


//
// Please note that the leaf structures are defined prior to
// this program for other CV applications, this structure are reused such
// that every leaf class contains its corresponding pre-defined structure.
//

#ifndef _PACKTYPE_H_INCLUDED
#define _PACKTYPE_H_INCLUDED

#include <wclist.h>

#ifndef _CV4W_H_INCLUDED
#define _CV4W_H_INCLUDED
#include "cv4w.h"
#endif

typedef unsigned_16 leaf_index;

static const leaf_index NO_LEAF = 0;

//
// To indicate the optional vtab_offset field in LF_METHODLIST does not exist.
//
static const int NO_VTAB_OFFSET     = -1;

//
// To indicate that no derivation information are available for a
// class/sttructure type record.
//
static const int NO_DER_INFO        = 0x0000;

static const int NO_TYPING_INFO     = 0;

class TypeArray;

class LFLeafStruct {

    public :

        LFLeafStruct( const leaf_index  leaf,
                      const char*       rawBuffer=0,
                      const uint        variantSize=0 );

        virtual ~LFLeafStruct() { }

        virtual void FixTypeIndex() { }

        // for the sake of putting lfleafstruct in wclist.
        bool operator==( const LFLeafStruct& ) const;

        bool IsEquivalent ( const LFLeafStruct& target ) const;

        void Put( ExeMaker& ) const;

        leaf_index Index() const {
            return _leaf;
        }

        VariantString& Variant() {
            return _variantString;
        }

        static LFLeafStruct* Error( const char*, const uint );

        static void SetLocalTypeArray( TypeArray& );
        static void SetGlobalTypeArray( TypeArray& t );

    protected :

        virtual bool IsEqual( const LFLeafStruct& ) const {
            return true;
        }

        virtual void DerivedPut( ExeMaker& ) const { }

        static bool TypeEqual( const type_index, const type_index );
        static bool TypeListEqual( const type_index*,
                                   const type_index*,
                                   const uint );

        static bool AttrEqual( const cv_attrib, const cv_attrib );
        static bool AttrEqual( const cv_ptrattr, const cv_ptrattr );
        static bool AttrEqual( const cv_sprop, const cv_sprop );
        static bool AttrEqual( const cv_fldattr, const cv_fldattr );

        static void FixTypeList( type_index*, const uint );

    private :

        const leaf_index        _leaf;
        VariantString           _variantString;

        static TypeArray*       _localTypeArray;
        static TypeArray*       _globalTypeArray;
};

class LFSubField : public LFLeafStruct {

    public :

        LFSubField( const leaf_index  leaf,
                    const uint        recLen,
                    const char*       var = NULL,
                    const uint        varLen = 0 );

        virtual ~LFSubField() { }

        virtual void FixTypeIndex() { }

        // for the sake of putting lfsubfield in wclist.
        bool operator == ( const LFSubField& ) const;

        uint RecordLength() const {
            return _recordLength;
        }

        static LFSubField* Error( const char* );

    protected :

        bool IsEqual( const LFLeafStruct& ) const;

        virtual bool IsSubFieldEqual( const LFSubField& ) const {
            return true;
        }

        virtual void DerivedPut( ExeMaker& ) const { }

        static unsigned_8 PadLen( const char* ptr );

    private :

        const LFSubField& dc( const LFLeafStruct& t ) const {
            return ( const LFSubField& ) t;
        }

        //
        // Record the length of a sub field record.  This is required
        // because for sub fields inside a field list, there is no record
        // length info append to the record.  The address of the next
        // sub field is calculate by adding the length of the current
        // record to the beginning address of the current record.
        //
        uint  _recordLength;
};

class LFNotTrans : public LFLeafStruct {

    public :

        LFNotTrans() : LFLeafStruct(LF_NOTTRANS) { }
        virtual ~LFNotTrans() { }

        static LFLeafStruct* Construct( const char*, const uint );

    protected :

        void DerivedPut( ExeMaker& ) const;
};

class LFNull : public LFLeafStruct {

    public :

        LFNull() : LFLeafStruct(LF_NULL) { }
        virtual ~LFNull() { }

        static LFLeafStruct* Construct( const char*, const uint );

    protected :

        void DerivedPut( ExeMaker& ) const;
};

class LFModifier : public LFLeafStruct {

    public :

        LFModifier( const lf_modifier& modifier,
                    const char*        var,
                    const uint         varLen )
                : LFLeafStruct( modifier.common.code, var, varLen ),
                  _modifier( modifier.f ) { }

        virtual ~LFModifier() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

        static LFLeafStruct* Construct( const char* );

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFModifier& dc( const LFLeafStruct& t ) const {
            return ( const LFModifier& ) t;
        }

        ct_modifier     _modifier;
};

class LFPointer : public LFLeafStruct {

    public :

        LFPointer( const lf_pointer&  pointer,
                   const char*        var,
                   const uint         varLen,
                   const type_index   trailingType )
                : LFLeafStruct( pointer.common.code, var, varLen ),
                  _pointer( pointer.f ),
                  _trailingType( trailingType )  { }

        virtual ~LFPointer() { }

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFPointer& dc( const LFLeafStruct& t ) const {
            return ( const LFPointer& ) t;
        }

        ct_pointer     _pointer;
        type_index     _trailingType;
};

class LFArray : public LFLeafStruct {

    public :

        LFArray( const lf_array&    array,
                 const char*        var,
                 const uint         varLen )
                : LFLeafStruct( array.common.code, var, varLen ),
                  _array( array.f ) { }

        virtual ~LFArray() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFArray& dc( const LFLeafStruct& t ) const {
            return ( const LFArray& ) t;
        }

        ct_array    _array;  // Main array record structure.
};

class LFClass : public LFLeafStruct {

    public :

        LFClass( const lf_class&    classStruct,
                 const char*        var,
                 const uint         varLen )
                : LFLeafStruct( classStruct.common.code, var, varLen ),
                  _class( classStruct.f ) { }

        virtual ~LFClass() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFClass& dc( const LFLeafStruct& t ) const {
            return ( const LFClass& ) t;
        }

        ct_class     _class;
};

typedef LFClass LFStructure;

class LFUnion : public LFLeafStruct {

    public :

        LFUnion( const lf_union&    unionStruct,
                 const char*        var,
                 const uint         varLen )
                : LFLeafStruct( unionStruct.common.code, var, varLen ),
                  _union( unionStruct.f ) { }

        virtual ~LFUnion() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFUnion& dc( const LFLeafStruct& t ) const {
            return ( const LFUnion& ) t;
        }

        ct_union     _union;
};

class LFEnum : public LFLeafStruct {

    public :

        LFEnum( const lf_enum&     enumStruct,
                const char*        var,
                const uint         varLen )
                : LFLeafStruct( enumStruct.common.code, var, varLen ),
                  _enum( enumStruct.f ) { }

        virtual ~LFEnum() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFEnum& dc( const LFLeafStruct& t ) const {
            return ( const LFEnum& ) t;
        }

        ct_enum     _enum;
};

class LFProcedure : public LFLeafStruct {

    public :

        LFProcedure( const lf_procedure& procedure,
                     const char*         var,
                     const uint          varLen )
                : LFLeafStruct( procedure.common.code, var, varLen ),
                  _procedure( procedure.f ) { }

        virtual ~LFProcedure() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFProcedure& dc( const LFLeafStruct& t ) const {
            return ( const LFProcedure& ) t;
        }

        ct_procedure     _procedure;
};

class LFMFunction : public LFLeafStruct {

    public :

        LFMFunction( const lf_mfunction& mFunction,
                     const char*         var,
                     const uint          varLen )
                : LFLeafStruct( mFunction.common.code, var, varLen ),
                  _mFunction( mFunction.f ) { }

        virtual ~LFMFunction() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFMFunction& dc( const LFLeafStruct& t ) const {
            return ( const LFMFunction& ) t;
        }

        ct_mfunction     _mFunction;
};

class LFVtShape : public LFLeafStruct {

    public :

        LFVtShape( const lf_vtshape& vtShape,
                   const char*       var,
                   const uint        varLen )
                : LFLeafStruct( vtShape.common.code, var, varLen ),
                  _vtShape( vtShape.f ) { }

        virtual ~LFVtShape() {}

        static LFLeafStruct* Construct( const char*, const uint );


    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFVtShape& dc( const LFLeafStruct& t ) const {
            return ( const LFVtShape& ) t;
        }

        ct_vtshape     _vtShape;
};

class LFCobol0 : public LFLeafStruct {

    public :

        LFCobol0( const lf_cobol0&   cobol0,
                  const char*        var,
                  const uint         varLen )
                : LFLeafStruct( cobol0.common.code, var, varLen ),
                  _cobol0( cobol0.f ) { }

        virtual ~LFCobol0() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFCobol0& dc( const LFLeafStruct& t ) const {
            return ( const LFCobol0& ) t;
        }

        ct_cobol0     _cobol0;
};

class LFCobol1 : public LFLeafStruct {

    public :

        LFCobol1( const lf_cobol1&   cobol1,
                  const char*        var,
                  const uint         varLen )
                : LFLeafStruct( cobol1.common.code, var, varLen ),
                  _cobol1( cobol1.f ) { }

        virtual ~LFCobol1() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    private :

        const LFCobol1& dc( const LFLeafStruct& t ) const {
            return ( const LFCobol1& ) t;
        }

        // Note: defn in cv4f.h does not conform to spec.
        ct_cobol1     _cobol1;
};

class LFBArray : public LFLeafStruct {

    public :

        LFBArray( const lf_barray&   bArray,
                  const char*        var,
                  const uint         varLen )
                : LFLeafStruct( bArray.common.code, var, varLen ),
                  _bArray( bArray.f ) { }

        virtual ~LFBArray() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFBArray& dc( const LFLeafStruct& t ) const {
            return ( const LFBArray& ) t;
        }

        ct_barray     _bArray;
};

class LFLabel : public LFLeafStruct {

    public :

        LFLabel( const lf_label&    label,
                 const char*        var,
                 const uint         varLen )
                : LFLeafStruct( label.common.code, var, varLen ),
                  _label( label.f ) { }

        virtual ~LFLabel() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFLabel& dc( const LFLeafStruct& t ) const {
            return ( const LFLabel& ) t;
        }

        ct_label     _label;
};

class LFDimArray : public LFLeafStruct {

    public :

        LFDimArray( const lf_dimarray& dimArray,
                    const char*        var,
                    const uint         varLen )
                : LFLeafStruct( dimArray.common.code, var, varLen ),
                  _dimArray( dimArray.f ) { }

        virtual ~LFDimArray() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFDimArray& dc( const LFLeafStruct& t ) const {
            return ( const LFDimArray& ) t;
        }

        ct_dimarray     _dimArray;
};

class LFVftPath : public LFLeafStruct {

    public :

        LFVftPath( const lf_vftpath&  vftPath,
                   const char*        var,
                   const uint         varLen,
                   type_index*        bases )
                : LFLeafStruct( vftPath.common.code, var, varLen ),
                  _vftPath( vftPath.f ),
                  _bases( bases ) { }

        virtual ~LFVftPath() {
            delete [] _bases;
        }

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFVftPath& dc( const LFLeafStruct& t ) const {
            return ( const LFVftPath& ) t;
        }

        ct_vftpath         _vftPath;
        type_index*        _bases;
};

class LFArgList : public LFLeafStruct {

    public :

        LFArgList( const lf_arglist&  argList,
                   const char*        var,
                   const uint         varLen,
                   type_index*        indices )
                : LFLeafStruct( argList.common.code, var, varLen ),
                  _argList( argList.f ),
                  _indices( indices ) { }

        virtual ~LFArgList() {
            delete [] _indices;
        }

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFArgList& dc( const LFLeafStruct& t ) const {
            return ( const LFArgList& ) t;
        }

        ct_arglist           _argList;
        type_index*          _indices;
};

class LFDefArg : public LFLeafStruct {

    public :

        LFDefArg( const lf_defarg&   defArg,
                  const char*        var,
                  const uint         varLen )
                : LFLeafStruct( defArg.common.code, var, varLen ),
                  _defArg( defArg.f ) { }

        virtual ~LFDefArg() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFDefArg& dc( const LFLeafStruct& t ) const {
            return ( const LFDefArg& ) t;
        }

        ct_defarg     _defArg;
};

typedef LFSubField* (*PSF)( const char* );

class LFFieldList : public LFLeafStruct {


    public :

        LFFieldList( const char*, const uint);

        virtual ~LFFieldList() {
            _subFieldList.clearAndDestroy();
        }

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFFieldList& dc( const LFLeafStruct& t ) const {
            return ( const LFFieldList& ) t;
        }

        static uint ConvertIndex( const leaf_index );

        WCPtrSList<LFSubField>    _subFieldList;
        static PSF                _subFieldConstructorTable[];
};

class LFDerived : public LFLeafStruct {

    public :

        LFDerived( const lf_derived&  derived,
                   const char*        var,
                   const uint         varLen,
                   type_index*        indices )
                : LFLeafStruct( derived.common.code, var, varLen ),
                  _derived( derived.f ),
                  _indices( indices ) { }

        virtual ~LFDerived() {
            delete [] _indices;
        }

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFDerived& dc( const LFLeafStruct& t ) const {
            return ( const LFDerived& ) t;
        }

        ct_derived             _derived;
        type_index*            _indices;
};

class LFBitField : public LFLeafStruct {

    public :

        LFBitField( const lf_bitfield& bitField,
                    const char*        var,
                    const uint         varLen )
                : LFLeafStruct( bitField.common.code, var, varLen ),
                  _bitField( bitField.f ) { }

        virtual ~LFBitField() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFBitField& dc( const LFLeafStruct& t ) const {
            return ( const LFBitField& ) t;
        }

        ct_bitfield       _bitField;
};

// for the sake of putting it in lists in C++ class library.
struct my_ct_mlist {
    cv_fldattr  attr;
    u2          type;
    u4          vtab;

    my_ct_mlist() { }
    ~my_ct_mlist() { }

    bool operator == ( const my_ct_mlist& ) const;
};

class LFMethodList : public LFLeafStruct {

    public :

        LFMethodList( const leaf_index       leaf,
                      WCPtrSList<my_ct_mlist>*  recList )
                : LFLeafStruct( leaf ),
                  _mList( recList ) { }

        virtual ~LFMethodList() {
            _mList -> clearAndDestroy();
            delete _mList;
        }

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFMethodList& dc( const LFLeafStruct& t ) const {
            return ( const LFMethodList& ) t;
        }

        WCPtrSList<my_ct_mlist>*     _mList;
};

class LFDimConu : public LFLeafStruct {

    public :

        LFDimConu( const lf_dimconu&  dimConu,
                   const char*        var,
                   const uint         varLen )
                : LFLeafStruct( dimConu.common.code, var, varLen ),
                  _dimConu( dimConu.f ) { }

        virtual ~LFDimConu() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFDimConu& dc( const LFLeafStruct& t ) const {
            return ( const LFDimConu& ) t;
        }

        ct_dimconu       _dimConu;
};

class LFDimConlu : public LFLeafStruct {

    public :

        LFDimConlu( const lf_dimconlu& dimConlu,
                    const char*        var,
                    const uint         varLen )
                : LFLeafStruct( dimConlu.common.code, var, varLen ),
                  _dimConlu( dimConlu.f ) { }

        virtual ~LFDimConlu() {}

        static LFLeafStruct* Construct( const char*, const uint );

        void FixTypeIndex();

    protected :

        virtual void DerivedPut( ExeMaker& ) const;

        bool IsEqual( const LFLeafStruct& ) const;

    private :

        const LFDimConlu& dc( const LFLeafStruct& t ) const {
            return ( const LFDimConlu& ) t;
        }

        ct_dimconlu       _dimConlu;
};

// RefSym.

class LFBClass : public LFSubField {

    public :

        LFBClass( const lf_bclass&  bclass,
                  const char*       var,
                  const uint        varLen )
                : LFSubField( bclass.common.code,
                              sizeof(lf_bclass) + varLen,
                              var, varLen ),
                  _bClass( bclass.f ) { }

        virtual ~LFBClass() { }

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFBClass& dc( const LFSubField& t ) const {
            return ( const LFBClass& ) t;
        }

        ct_bclass       _bClass;
};

class LFVBClass : public LFSubField {

    public :

        LFVBClass( const lf_vbclass& vbClass,
                   const char*       var,
                   const uint        varLen )
                : LFSubField( vbClass.common.code,
                              sizeof(lf_vbclass) + varLen,
                              var, varLen ),
                  _vbClass( vbClass.f ) { }

        LFVBClass(const leaf_index, const char*);
        virtual ~LFVBClass() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFVBClass& dc( const LFSubField& t ) const {
            return ( const LFVBClass& ) t;
        }

        ct_vbclass       _vbClass;
};

typedef LFVBClass LFIVBClass;

class LFEnumerate : public LFSubField {

    public :

        LFEnumerate( const lf_enumerate   enumerate,
                     const char*          var,
                     const uint           varLen )
                : LFSubField( enumerate.common.code,
                              sizeof(lf_enumerate) + varLen,
                              var, varLen ),
                  _enumerate( enumerate.f ) { }

        LFEnumerate(const leaf_index, const char*);
        virtual ~LFEnumerate() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFEnumerate& dc( const LFSubField& t ) const {
            return ( const LFEnumerate& ) t;
        }

        ct_enumerate       _enumerate;
};

class LFFriendFcn : public LFSubField {

    public :

        LFFriendFcn( const lf_friendfcn   friendFcn,
                     const char*          var,
                     const uint           varLen )
                : LFSubField( friendFcn.common.code,
                              sizeof(lf_friendfcn) + varLen,
                              var, varLen ),
                  _friendFcn( friendFcn.f ) { }

        virtual ~LFFriendFcn() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFFriendFcn& dc( const LFSubField& t ) const {
            return ( const LFFriendFcn& ) t;
        }

        ct_friendfcn       _friendFcn;
};

class LFIndex : public LFSubField {

    public :

        LFIndex( const lf_index    index )
                : LFSubField(index.common.code, sizeof(lf_index)),
                  _index( index.f ) { }

        virtual ~LFIndex() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFIndex& dc( const LFSubField& t ) const {
            return ( const LFIndex& ) t;
        }

        ct_index       _index;
};

class LFMember : public LFSubField {

    public :

        LFMember( const lf_member&  member,
                  const char*       var,
                  const uint        varLen )
                : LFSubField( member.common.code,
                              sizeof(lf_member) + varLen,
                              var, varLen ),
                  _member( member.f ) { }

        virtual ~LFMember() {}

        virtual void DerivedPut( ExeMaker& ) const;

        void FixTypeIndex();

        static LFSubField* Construct1( const char* );
        static LFSubField* Construct2( const char* );

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFMember& dc( const LFSubField& t ) const {
            return ( const LFMember& ) t;
        }

        ct_member       _member;
};

typedef LFMember LFStMember;

class LFMethod : public LFSubField {

    public :

        LFMethod( const lf_method&  method,
                  const char*       var,
                  const uint        varLen )
                : LFSubField( method.common.code,
                              sizeof(lf_method) + varLen,
                              var, varLen ),
                  _method( method.f ) { }

        virtual ~LFMethod() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFMethod& dc( const LFSubField& t ) const {
            return ( const LFMethod& ) t;
        }

        ct_method       _method;
};

class LFNestedType : public LFSubField {

    public :

        LFNestedType( const lf_nestedtype   nestedType,
                      const char*           var,
                      const uint            varLen )
                : LFSubField( nestedType.common.code,
                              sizeof(lf_nestedtype) + varLen,
                              var, varLen ),
                  _nestedType( nestedType.f ) { }

        virtual ~LFNestedType() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFNestedType& dc( const LFSubField& t ) const {
            return ( const LFNestedType& ) t;
        }

        ct_nestedtype       _nestedType;
};

class LFVFuncTab : public LFSubField {

    public :

        LFVFuncTab( const lf_vfunctab   vFuncTab )
                : LFSubField( vFuncTab.common.code, sizeof(lf_vfunctab) ),
                  _vFuncTab( vFuncTab.f ) { }

        virtual ~LFVFuncTab() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFVFuncTab& dc( const LFSubField& t ) const {
            return ( const LFVFuncTab& ) t;
        }

        ct_vfunctab       _vFuncTab;
};

class LFFriendCls : public LFSubField {

    public :

        LFFriendCls( const lf_friendcls   friendCls )
                : LFSubField( friendCls.common.code, sizeof(lf_friendcls) ),
                  _friendCls( friendCls.f ) { }

        virtual ~LFFriendCls() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFFriendCls& dc( const LFSubField& t ) const {
            return ( const LFFriendCls& ) t;
        }

        ct_friendcls       _friendCls;
};

class LFOneMethod : public LFSubField {

    public :

        LFOneMethod( const lf_onemethod&  oneMethod,
                     const char*          var,
                     const uint           varLen )
                : LFSubField( oneMethod.common.code,
                              sizeof(lf_onemethod) + varLen,
                              var, varLen ),
                  _oneMethod( oneMethod.f ) { }

        virtual ~LFOneMethod() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFOneMethod& dc( const LFSubField& t ) const {
            return ( const LFOneMethod& ) t;
        }

        ct_onemethod       _oneMethod;
};

class LFVFuncOff : public LFSubField {

    public :

        LFVFuncOff( const lf_vfuncoff&  vFuncOff )
                : LFSubField( vFuncOff.common.code, sizeof(lf_vfuncoff) ),
                  _vFuncOff( vFuncOff.f ) { }

        LFVFuncOff(const leaf_index, const char*);
        virtual ~LFVFuncOff() {}

        virtual void DerivedPut( ExeMaker& ) const;

        static LFSubField* Construct( const char* );

        void FixTypeIndex();

    protected :

        bool IsSubFieldEqual( const LFSubField& ) const;

    private :

        const LFVFuncOff& dc( const LFSubField& t ) const {
            return ( const LFVFuncOff& ) t;
        }

        ct_vfuncoff       _vFuncOff;
};
#endif

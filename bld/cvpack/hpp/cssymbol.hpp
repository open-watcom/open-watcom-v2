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


#ifndef CSSYMBOL_H_INCLUDED
#define CSSYMBOL_H_INCLUDED

#include <stdlib.h>
#include <string.hpp>
#include "typemap.hpp"
#include "retrieve.hpp"
#include "makeexe.hpp"

extern TypeIndexMap TypeMap;

class SymbolStruct;

typedef unsigned_16 symbol_index;

static const int NO_SEGMENT = 0xffff;
static const int NO_OFFSET = 0xffff;

//
// basic size of a symbol record.
//
static const int BASICSIZE = WORD;
static const int REF_RECORD_LENGTH = 12;
static const int NO_CHKSUM = 0;

class SymbolStruct {

    public :

        SymbolStruct() : _namePlusVar(NULL,0) { }

        SymbolStruct( const unsigned_16, const symbol_index );

        SymbolStruct( const s_common common );

        SymbolStruct( const s_common, const char*, const uint );

        virtual ~SymbolStruct() { }

        void Put( ExeMaker& ) const;

        virtual uint CodeSegment() const {
            return NO_SEGMENT;
        }

        virtual uint DataSegment() const {
            return NO_SEGMENT;
        }

        virtual uint MemOffset() const {
            return NO_OFFSET;
        }

        virtual void SetParent( const unsigned_32 ) { }
        virtual void SetNext( const unsigned_32 ) { }
        virtual void SetEnd( const unsigned_32 ) { }
        virtual void FixType() { }

        virtual bool IsStartSym() const {
            return FALSE;
        }

        virtual unsigned_32 cSum() const {
            if ( _namePlusVar._strLen > 0 ) {
                if ( *_namePlusVar._string > 0 ) {
                    return checkSum(_namePlusVar._string);
                }
            }
            return NO_CHKSUM;
        }

        // for the sake of link list.
        bool operator == ( const SymbolStruct& ) const;

        void SetOffset( const unsigned_32 offset ) {
            _offset = offset;
        }

        // length including length field.
        uint Length() const {
            return _length + WORD;
        }

        unsigned_32 Offset() const {
            return _offset;
        }

        bool IsGlobalProc() const {
            return ( _leaf == S_GPROC16 || _leaf == S_GPROC32 );
        }

        bool IsStaticData() const {
            return ( _leaf == S_LDATA16 ||
                     _leaf == S_LDATA32 ||
                     _leaf == S_LTHREAD32 );
        }

        bool IsStaticProc() const {
            return ( _leaf == S_LPROC16 || _leaf == S_LPROC32 );
        }

        bool IsEndSym() const {
            return ( _leaf == S_END );
        }

        bool IsModuleSpecific() const {
            return ( _leaf == S_OBJNAME || _leaf == S_COMPILE );
        }

/*        const unsigned_8 NameLen() const { // to be deleted.
            if ( _name != NULL ) {
                return *_name+1;
            }
            return 0;
        } */

        // Virtual func since ref syms define a different version.
        virtual const char* Name() const {
            return ( _namePlusVar._string );
        }

        static SymbolStruct* Error( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const { }

        static uint GetVarLength( const uint totalLen,
                                  const uint structLen ) {
            return ( totalLen - BASICSIZE - structLen );
        }

    private :

        unsigned_8 PadToAlign( const uint length ) {
            if ( length % LONG_WORD != 0 ) {
                return (unsigned_8) ( LONG_WORD - length % LONG_WORD );
            }
            return 0;
        }

        void Init() {
            _pad = PadToAlign(_length + WORD);
            _length += _pad;
        }
        static unsigned_8 byt_toupper( const unsigned_8 );
        static unsigned_32 dwrd_toupper( const unsigned_32 );
        static unsigned_32 checkSum( const char* );

        static char*    _padTable[];

        unsigned_16     _length;
        symbol_index    _leaf;
        unsigned_32     _offset;
        unsigned_8      _pad;
        VariantString   _namePlusVar;
};

class CSCompile : public SymbolStruct {

    public :

        CSCompile ( const s_common     common,
                    const cs_compile   compile,
                    const char*        lpVersion )
                : SymbolStruct( common ),
                  _compile( compile ) {
            _verlen = common.length - sizeof(cs_compile) - WORD;
            _version = new char [_verlen];
            memcpy( _version,lpVersion, _verlen );
        }

        ~CSCompile() {
            delete [] _version;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_compile      _compile;
        char*           _version;
        unsigned        _verlen;
};

//
// Does not implement register tracking.  Should be added once its format
// is available.
//
class CSRegister : public SymbolStruct {

    public :

        CSRegister ( const s_common     common,
                     const cs_register  registerStruct,
                     const char*        var,
                     const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _register( registerStruct ) { }

        ~CSRegister() { }

        void FixType() {
            _register.type = TypeMap.Lookup(_register.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_register      _register;
};

class CSConstant : public SymbolStruct {

    public :

        CSConstant ( const s_common     common,
                     const cs_constant  constant,
                     const char*        valuePtr,
                     const uint         valueLen ,
                     const char*        var,
                     const uint         varLen )
                : SymbolStruct( common, var, varLen),
                  _constant( constant ),
                  _value( valuePtr, valueLen ) { }

        ~CSConstant() { }

        void FixType() {
            _constant.type = TypeMap.Lookup(_constant.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_constant      _constant;
        VariantString    _value;
};

class CSUdt : public SymbolStruct {

    public :

        CSUdt ( const s_common     common,
                const cs_udt       udt,
                const char*        var,
                const uint         varLen )
            : SymbolStruct( common, var, varLen ),
              _udt( udt ) { }

        ~CSUdt() { }

        void FixType() {
            _udt.type = TypeMap.Lookup(_udt.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_udt      _udt;
};

class CSStartSearch : public SymbolStruct {

    public :

        CSStartSearch ( const s_common     common,
                        const cs_ssearch&  ssearch )
                : SymbolStruct( common ),
                  _ssearch( ssearch ) { }

        CSStartSearch( const unsigned_32 seg )
                : SymbolStruct( 8, S_SSEARCH ) {
            _ssearch.segment = seg;
        }

        void SetNext( const unsigned_32 offset ) {
            _ssearch.sym_off = offset;
        }

        ~CSStartSearch() { }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_ssearch      _ssearch;
};

class CSEndBlock : public SymbolStruct {

    public :

        CSEndBlock ( const s_common common )
                : SymbolStruct( common ) { }

        ~CSEndBlock() { }

        static SymbolStruct* Construct( const char* );
};

class CSSkipRecord {

    public :

        static SymbolStruct* Construct( const char* );
};

class CSObjName : public SymbolStruct {

    public :

        CSObjName ( const s_common     common,
                    const cs_objname   objName,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _objName( objName ) { }

        ~CSObjName() { }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_objname      _objName;
};

class CSEndOfArg : public SymbolStruct {

    public :

        CSEndOfArg ( const s_common common )
                : SymbolStruct( common ) { }

        ~CSEndOfArg() { }

        static SymbolStruct* Construct( const char* );
};

class CSCobolUdt : public SymbolStruct {

    public :

        CSCobolUdt ( const s_common     common,
                     const cs_coboludt  cobolUdt,
                     const char*        var,
                     const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _cobolUdt( cobolUdt ) { }

        ~CSCobolUdt() { }

        void FixType() {
            _cobolUdt.type = TypeMap.Lookup(_cobolUdt.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_coboludt      _cobolUdt;
};

class CSManyReg : public SymbolStruct {

    public :

        CSManyReg ( const s_common     common,
                    const cs_manyreg   manyReg,
                    const char*        buffer,
                    const uint         len )
                : SymbolStruct( common,buffer+manyReg.count,len-manyReg.count),
                  _manyReg( manyReg ),
                  _regList( buffer, manyReg.count ) { }

        ~CSManyReg() { }

        void FixType() {
            _manyReg.type = TypeMap.Lookup(_manyReg.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_manyreg      _manyReg;
        VariantString   _regList;
};

class CSReturn : public SymbolStruct {

    public :

        CSReturn ( const s_common     common,
                   const cs_return    returnStruct,
                   const char*        data,
                   const uint         dataLen )
                : SymbolStruct( common ),
                  _return( returnStruct ),
                  _data( data, dataLen ) { }

        ~CSReturn() { }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_return      _return;
        VariantString  _data;
};

class CSEntryThis : public SymbolStruct {

    public :

        CSEntryThis ( const s_common common,
                      SymbolStruct*  symPtr )
                : SymbolStruct( common ),
                  _symPtr( symPtr ) { }

        ~CSEntryThis() {
            delete _symPtr;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        SymbolStruct*   _symPtr;
};

class CSBPRel16 : public SymbolStruct {

    public :

        CSBPRel16 ( const s_common     common,
                    const cs_bprel16   bpRel16,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _bpRel16( bpRel16 ) { }

        ~CSBPRel16() { }

        void FixType() {
            _bpRel16.type = TypeMap.Lookup(_bpRel16.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_bprel16      _bpRel16;
};

class CSLData16 : public SymbolStruct {

    public :

        CSLData16 ( const s_common     common,
                    const cs_ldata16   lData16,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _lData16( lData16 ) { }

        ~CSLData16() { }

        uint DataSegment() const {
            return _lData16.segment;
        }

        uint MemOffset() const {
            return _lData16.offset;
        }

        void FixType() {
            _lData16.type = TypeMap.Lookup(_lData16.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_ldata16      _lData16;
};

typedef CSLData16 CSGData16;

class CSPub16 : public SymbolStruct {

    public :

        CSPub16 ( const s_common    common,
                  const cs_pub16    pub16,
                  const char*       var,
                  const uint        varLen )
                : SymbolStruct( common, var, varLen ),
                  _pub16( pub16 ) { }

        ~CSPub16() { }

        // don't know if it's data or code seg for public sym.
        uint DataSegment() const {
            return _pub16.segment;
        }

        uint CodeSegment() const {
            return _pub16.segment;
        }

        uint MemOffset() const {
            return _pub16.offset;
        }

        void FixType() {
            _pub16.type = TypeMap.Lookup(_pub16.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_pub16      _pub16;
};

class CSLProc16 : public SymbolStruct {

    public :

        CSLProc16 ( const s_common     common,
                    const cs_lproc16&  lProc16,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _lProc16( lProc16 ) { }

        ~CSLProc16() { }

        uint CodeSegment() const {
            return _lProc16.segment;
        }

        uint MemOffset() const {
            return _lProc16.offset;
        }

        bool IsStartSym() const {
            return TRUE;
        }

        void SetParent( const unsigned_32 pParent ) {
            _lProc16.pParent = pParent;
        }

        void SetNext( const unsigned_32 pNext ) {
            _lProc16.pNext = pNext;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _lProc16.pEnd = pEnd;
        }

        void FixType() {
            _lProc16.proctype = TypeMap.Lookup(_lProc16.proctype);
        }

        static SymbolStruct* Construct(const char*);

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_lproc16      _lProc16;
};

typedef CSLProc16 CSGProc16;

class CSThunk16 : public SymbolStruct {

    public :

        CSThunk16 ( const s_common     common,
                    const cs_thunk16&  thunk16,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _thunk16( thunk16 ) { }

        ~CSThunk16() { }

        bool IsStartSym() const {
            return TRUE;
        }

        uint CodeSegment() const {
            return _thunk16.segment;
        }

        uint MemOffset() const {
            return _thunk16.offset;
        }

        void SetParent( const unsigned_32 pParent ) {
            _thunk16.pParent = pParent;
        }

        void SetNext( const unsigned_32 pNext ) {
            _thunk16.pNext = pNext;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _thunk16.pEnd = pEnd;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_thunk16      _thunk16;
};

class CSBlock16 : public SymbolStruct {

    public :

        CSBlock16 ( const s_common     common,
                    const cs_block16&  block16,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _block16( block16 ) { }

        ~CSBlock16() { }

        bool IsStartSym() const {
            return TRUE;
        }
        uint CodeSegment() const {
            return _block16.segment;
        }

        uint MemOffset() const {
            return _block16.offset;
        }

        void SetParent( const unsigned_32 pParent ) {
            _block16.pParent = pParent;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _block16.pEnd = pEnd;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_block16      _block16;
};

class CSWith16 : public SymbolStruct {

    public :

        CSWith16 ( const s_common     common,
                   const cs_with16&   with16,
                   const char*        expr )
                : SymbolStruct( common ),
                  _with16( with16 ) {
            _expr = new char [*expr+1];
            memcpy(_expr,expr,*expr+1);
        }

        ~CSWith16() {
            delete [] _expr;
        }

        bool IsStartSym() const {
            return TRUE;
        }

        uint CodeSegment() const {
            return _with16.segment;
        }

        uint MemOffset() const {
            return _with16.offset;
        }

        void SetParent( const unsigned_32 pParent ) {
            _with16.pParent = pParent;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _with16.pEnd = pEnd;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_with16      _with16;
        char*          _expr;
};

class CSLabel16 : public SymbolStruct {

    public :

        CSLabel16 ( const s_common     common,
                    const cs_label16&  label16,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _label16( label16 ) { }

        ~CSLabel16() { }

        uint CodeSegment() const {
            return _label16.segment;
        }

        uint MemOffset() const {
            return _label16.offset;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_label16      _label16;
};

class CSCExModel16 : public SymbolStruct {

    public :

        CSCExModel16 ( const s_common       common,
                       const cs_cexmodel16& cExModel16,
                       const char*          var,
                       const uint           varLen )
                : SymbolStruct( common ),
                  _cExModel16( cExModel16 ),
                  _variant( var, varLen )  { }

        ~CSCExModel16() { }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_cexmodel16      _cExModel16;
        VariantString      _variant;
};

class CSVftPath16 : public SymbolStruct {

    public :

        CSVftPath16 ( const s_common       common,
                      const cs_vftpath16&  vftPath16 )
                : SymbolStruct( common ),
                  _vftPath16( vftPath16 ) { }

        ~CSVftPath16() { }

        uint DataSegment() const {
            return _vftPath16.segment;
        }

        uint MemOffset() const {
            return _vftPath16.offset;
        }

        void FixType() {
            _vftPath16.root = TypeMap.Lookup(_vftPath16.root);
            _vftPath16.path = TypeMap.Lookup(_vftPath16.path);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_vftpath16      _vftPath16;
};

class CSRegRel16 : public SymbolStruct {

    public :

        CSRegRel16 ( const s_common     common,
                     const cs_regrel16& regRel16,
                     const char*        var,
                     const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _regRel16( regRel16 ) { }

        ~CSRegRel16() { }

        void FixType() {
            _regRel16.type = TypeMap.Lookup(_regRel16.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_regrel16      _regRel16;
};

class CSBPRel32 : public SymbolStruct {

    public :

        CSBPRel32 ( const s_common     common,
                    const cs_bprel32   bpRel32,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _bpRel32( bpRel32 ) { }

        ~CSBPRel32() { }

        void FixType() {
            _bpRel32.type = TypeMap.Lookup(_bpRel32.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_bprel32      _bpRel32;
};

class CSLData32 : public SymbolStruct {

    public :

        CSLData32 ( const s_common     common,
                    const cs_ldata32   lData32,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _lData32( lData32 ) { }

        ~CSLData32() { }

        uint DataSegment() const {
            return _lData32.segment;
        }

        uint MemOffset() const {
            return _lData32.offset;
        }

        void FixType() {
            _lData32.type = TypeMap.Lookup(_lData32.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_ldata32      _lData32;
};

typedef CSLData32 CSGData32;

class CSPub32 : public SymbolStruct {

    public :

        CSPub32 ( const s_common    common,
                  const cs_pub32    pub32,
                  const char*       var,
                  const uint        varLen )
                : SymbolStruct( common, var, varLen ),
                  _pub32( pub32 ) { }

        ~CSPub32() { }

        // don't know if it's data or code seg for public sym.
        uint DataSegment() const {
            return _pub32.segment;
        }

        uint CodeSegment() const {
            return _pub32.segment;
        }

        uint MemOffset() const {
            return _pub32.offset;
        }

        void FixType() {
            _pub32.type = TypeMap.Lookup(_pub32.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_pub32      _pub32;
};

class CSPub32_new : public SymbolStruct {

    public :

        CSPub32_new ( const s_common    common,
                  const cs_pub32_new    pub32_new,
                  const char*       var,
                  const uint        varLen )
                : SymbolStruct( common, var, varLen ),
                  _pub32_new( pub32_new ) { }

        ~CSPub32_new() { }

        // don't know if it's data or code seg for public sym.
        uint DataSegment() const {
            return _pub32_new.segment;
        }

        uint CodeSegment() const {
            return _pub32_new.segment;
        }

        uint MemOffset() const {
            return _pub32_new.offset;
        }

        void FixType() {
            _pub32_new.type = TypeMap.Lookup(_pub32_new.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_pub32_new      _pub32_new;
};

class CSLProc32 : public SymbolStruct {

    public :

        CSLProc32 ( const s_common     common,
                    const cs_lproc32&  lProc32,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _lProc32( lProc32 ) { }

        ~CSLProc32() { }

        uint CodeSegment() const {
            return _lProc32.segment;
        }

        uint MemOffset() const {
            return _lProc32.offset;
        }

        bool IsStartSym() const {
            return TRUE;
        }

        void SetParent( const unsigned_32 pParent ) {
            _lProc32.pParent = pParent;
        }

        void SetNext( const unsigned_32 pNext ) {
            _lProc32.pNext = pNext;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _lProc32.pEnd = pEnd;
        }

        void FixType() {
            _lProc32.proctype = TypeMap.Lookup(_lProc32.proctype);
        }

        static SymbolStruct* Construct(const char*);

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_lproc32      _lProc32;
};

typedef CSLProc32 CSGProc32;

class CSThunk32 : public SymbolStruct {

    public :

        CSThunk32 ( const s_common     common,
                    const cs_thunk32&  thunk32,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _thunk32( thunk32 ) { }

        ~CSThunk32() { }

        bool IsStartSym() const {
            return TRUE;
        }

        uint CodeSegment() const {
            return _thunk32.segment;
        }

        uint MemOffset() const {
            return _thunk32.offset;
        }

        void SetParent( const unsigned_32 pParent ) {
            _thunk32.pParent = pParent;
        }

        void SetNext( const unsigned_32 pNext ) {
            _thunk32.pNext = pNext;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _thunk32.pEnd = pEnd;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_thunk32      _thunk32;
};

class CSBlock32 : public SymbolStruct {

    public :

        CSBlock32 ( const s_common     common,
                    const cs_block32&  block32,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _block32( block32 ) { }

        ~CSBlock32() { }

        bool IsStartSym() const {
            return TRUE;
        }
        uint CodeSegment() const {
            return _block32.segment;
        }

        uint MemOffset() const {
            return _block32.offset;
        }

        void SetParent( const unsigned_32 pParent ) {
            _block32.pParent = pParent;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _block32.pEnd = pEnd;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_block32      _block32;
};

class CSWith32 : public SymbolStruct {

    public :

        CSWith32 ( const s_common     common,
                   const cs_with32&   with32,
                   const char*        expr )
                : SymbolStruct( common ),
                  _with32( with32 ) {
            _expr = new char [*expr+1];
            memcpy(_expr,expr,*expr+1);
        }

        ~CSWith32() {
            delete [] _expr;
        }

        bool IsStartSym() const {
            return TRUE;
        }

        uint CodeSegment() const {
            return _with32.segment;
        }

        uint MemOffset() const {
            return _with32.offset;
        }

        void SetParent( const unsigned_32 pParent ) {
            _with32.pParent = pParent;
        }

        void SetEnd( const unsigned_32 pEnd ) {
            _with32.pEnd = pEnd;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_with32      _with32;
        char*          _expr;
};

class CSLabel32 : public SymbolStruct {

    public :

        CSLabel32 ( const s_common     common,
                    const cs_label32&  label32,
                    const char*        var,
                    const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _label32( label32 ) { }

        ~CSLabel32() { }

        uint CodeSegment() const {
            return _label32.segment;
        }

        uint MemOffset() const {
            return _label32.offset;
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_label32      _label32;
};

class CSCExModel32 : public SymbolStruct {

    public :

        CSCExModel32 ( const s_common       common,
                       const cs_cexmodel32& cExModel32,
                       const char*          var,
                       const uint           varLen )
                : SymbolStruct( common ),
                  _cExModel32( cExModel32 ),
                  _variant( var, varLen )  { }

        ~CSCExModel32() { }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_cexmodel32      _cExModel32;
        VariantString      _variant;
};

class CSVftPath32 : public SymbolStruct {

    public :

        CSVftPath32 ( const s_common       common,
                      const cs_vftpath32&  vftPath32 )
                : SymbolStruct( common ),
                  _vftPath32( vftPath32 ) { }

        ~CSVftPath32() { }

        uint DataSegment() const {
            return _vftPath32.segment;
        }

        uint MemOffset() const {
            return _vftPath32.offset;
        }

        void FixType() {
            _vftPath32.root = TypeMap.Lookup(_vftPath32.root);
            _vftPath32.path = TypeMap.Lookup(_vftPath32.path);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_vftpath32      _vftPath32;
};

class CSRegRel32 : public SymbolStruct {

    public :

        CSRegRel32 ( const s_common     common,
                     const cs_regrel32& regRel32,
                     const char*        var,
                     const uint         varLen )
                : SymbolStruct( common, var, varLen ),
                  _regRel32( regRel32 ) { }

        ~CSRegRel32() { }

        void FixType() {
            _regRel32.type = TypeMap.Lookup(_regRel32.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_regrel32      _regRel32;
};

class CSLThread32 : public SymbolStruct {

    public :

        CSLThread32 ( const s_common       common,
                      const cs_lthread32&  lThread32,
                      const char*          var,
                      const uint           varLen )
                : SymbolStruct( common, var, varLen ),
                  _lThread32( lThread32 ) { }

        ~CSLThread32() { }

        uint DataSegment() const {
            return _lThread32.segment;
        }

        uint MemOffset() const {
            return _lThread32.offset;
        }

        void FixType() {
            _lThread32.type = TypeMap.Lookup(_lThread32.type);
        }

        static SymbolStruct* Construct( const char* );

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        cs_lthread32      _lThread32;
};

typedef CSLThread32 CSGThread32;

/*class CSProcRef : public SymbolStruct {

    public :

        CSProcRef( SymbolStruct* sym,
                   const unsigned_16   module )
                : SymbolStruct( REF_RECORD_LENGTH, S_PROCREF ),
                  _checkSum(sym->cSum()),
                  _module( module ),
                  _refSym(sym) { }
  //                _testing( (char*) sym ) { }

        ~CSProcRef() { }

        unsigned_32 cSum() const {
            return _checkSum;
        }

        unsigned_16 CodeSegment() const {
            return _refSym->CodeSegment();
        }

        unsigned_16 MemOffset() const {
            return _refSym->MemOffset();
        }

        const char* Name() {
            return _refSym->Name();
        }

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :


        unsigned_32           _checkSum;
//        unsigned_32     _symOffset;
        unsigned_16           _module;
        SymbolStruct*   _refSym;
//        char*           _testing;

//        unsigned_16     _memSeg;
//        unsigned_16     _memOffset;
}; */

class CSProcRef : public SymbolStruct {

    public :

        CSProcRef( const SymbolStruct* sym,
                   const uint          module )
                : SymbolStruct( REF_RECORD_LENGTH, S_PROCREF ),
                  _checkSum(sym->cSum()),
                  _symOffset(sym->Offset()),
                  _module( module ),
                  _memSeg(sym->CodeSegment()),
                  _memOffset(sym->MemOffset()),
                  _refName(NULL) {
            const char* name = sym -> Name();
            if ( name != NULL ) {
                if ( *name > 0 ) {
                    _refName = new char [*name+1];
                    memcpy(_refName,name,*name+1);
                }
            }
        }

        ~CSProcRef() {
            delete [] _refName;
        }

        unsigned_32 cSum() const {
            return _checkSum;
        }

        uint DataSegment() const {
            return _memSeg;
        }

        uint MemOffset() const {
            return _memOffset;
        }

        const char* Name() const {
            return _refName;
        }

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        unsigned_32     _checkSum;
        unsigned_32     _symOffset;
        uint            _module;

        uint            _memSeg;
        uint            _memOffset;
        char*           _refName;
};

class CSDataRef : public SymbolStruct {

    public :

        CSDataRef( const SymbolStruct* sym,
                   const uint          module )
                : SymbolStruct( REF_RECORD_LENGTH, S_DATAREF ),
                  _checkSum(sym->cSum()),
                  _symOffset(sym->Offset()),
                  _module( module ),
                  _memSeg(sym->DataSegment()),
                  _memOffset(sym->MemOffset()),
                  _refName(NULL) {
            const char* name = sym -> Name();
            if ( name != NULL ) {
                if ( *name > 0 ) {
                    _refName = new char [*name+1];
                    memcpy(_refName,name,*name+1);
                }
            }
        }

        ~CSDataRef() {
            delete [] _refName;
        }

        unsigned_32 cSum() const {
            return _checkSum;
        }

        uint DataSegment() const {
            return _memSeg;
        }

        uint MemOffset() const {
            return _memOffset;
        }

        const char* Name() const {
            return _refName;
        }

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        unsigned_32     _checkSum;
        unsigned_32     _symOffset;
        uint            _module;

        uint            _memSeg;
        uint            _memOffset;
        char*           _refName;
};

class CSPageAlign : public SymbolStruct {

    public :

        CSPageAlign( const uint        padLength,
                     const unsigned_8  padValue = 0 )
                : SymbolStruct( padLength + BASICSIZE, S_ALIGN ),
                  _padLength( padLength ),
                  _padValue( padValue ) { }

        ~CSPageAlign() { }

    protected:

        virtual void DerivedPut( ExeMaker& ) const;

    private :

        uint            _padLength;
        unsigned_8      _padValue;
};
#endif

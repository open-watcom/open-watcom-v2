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


#ifndef _BRMDUMP_HPP
#define _BRMDUMP_HPP

struct FileP {
    FILE *      fp;

    FileP() { fp = NULL; }
    ~FileP() { if( fp != NULL ) fclose(fp); }

    FileP & operator=( FILE *p ) { fp = p; return *this; }
    int operator!() { return fp==NULL; }
    operator FILE * () { return fp; }
};


struct OrderedData {
//    OrderedData *     _next;

    OrderedData() {}
    virtual ~OrderedData() {}

    virtual void DumpData() = 0;
    virtual void ReIndex() {};
};


struct StringRec : public OrderedData {
    BRI_StringID        _index;
    uint_8 *            _buf;

    StringRec() { _buf = NULL; }
    ~StringRec() { if( _buf ) delete[] _buf; }

    void DumpData();
};


struct TypeRec;

struct DeclRec : public OrderedData {
    BRI_SymbolID                _index;
    BRI_SymbolAttributes        attributes;
    BRI_StringID                name_id;
    BRI_TypeID                  type_id;

    StringRec *                 name_ptr;
    TypeRec *                   type_ptr;

    DeclRec() { name_ptr = NULL; type_ptr = NULL; }

    void DumpData();
    void ReIndex();
};


struct TypeRec : public OrderedData {
    BRI_TypeID                  _index;
    BRI_TypeCode                typecode;
    uint_32                     num_ops;
    uint_32 *                   ops;

    union {
        struct {                                // BaseType
            BRI_BaseTypes       basetype;
        } bt;
        struct {                                // Modifier
            uint_32             flags;
            TypeRec *           parent;
        } fp;
        struct {                                // Pointer, Reference, Typedef
            TypeRec *           parent;
        } p;
        struct {                                // PtrToMember
            TypeRec *           host;
            TypeRec *           member;
        } cm;
        struct {                                // Array
            uint_32             size;
            TypeRec *           element;
        } se;
        struct {                                // Function
            TypeRec **          args;   // args[0] == return type
        } ra;
        struct {                                // Class, Struct, Union, Enum
            StringRec *         name_ptr;
            DeclRec *           symbol_ptr;
        } ns;
        struct {                                // BitField
            uint_32             width;
        } w;
    } info;

    TypeRec() { ops = NULL; }
    ~TypeRec();

    void ReIndex();
    void DumpData();
    void WriteType();
};


struct FileRec : public OrderedData {
    BRI_StringID        filename_id;

    StringRec *         filename_ptr;

    FileRec() { filename_ptr = NULL; }

    void DumpData();
    void ReIndex();
};


struct PCHRec : public OrderedData {
    BRI_StringID        filename_id;

    StringRec *         filename_ptr;

    PCHRec() { filename_ptr = NULL; }

    void DumpData();
    void ReIndex();
};


struct FileEndRec : public OrderedData {
    void DumpData();
};


struct TemplateRec : public OrderedData {
    BRI_StringID        filename_id;

    StringRec *         filename_ptr;

    TemplateRec() { filename_ptr = NULL; }

    void DumpData();
    void ReIndex();
};


struct TemplateEndRec : public OrderedData {
    void DumpData();
};


struct ScopeRec : public OrderedData {
    BRI_ScopeID         _index;
    BRI_ScopeType       flags;
    union {
        struct {
            BRI_StringID        fn_name_index;
            BRI_TypeID          fn_type_index;
        } fn_id;
        BRI_TypeID      type_index;
    };
    union {
        struct {
            StringRec           *fn_name_ptr;
            TypeRec             *fn_type_ptr;
        } fn_ptr;
        TypeRec *               type_ptr;
    };

    ScopeRec() { fn_ptr.fn_name_ptr = NULL; fn_ptr.fn_type_ptr = NULL; }

    void DumpData();
    void ReIndex();
};


struct ScopeEndRec : public OrderedData {
    void DumpData();
};


struct DefnRec : public OrderedData {
    uint_32             delta_col;
    uint_32             delta_line;
    BRI_StringID        filename_id;
    BRI_SymbolID        symbol_index;

    StringRec *         filename_ptr;
    DeclRec *           symbol_ptr;

    DefnRec() { filename_ptr = NULL; symbol_ptr = NULL; }

    void DumpData();
    void ReIndex();
};


struct DeltaRec : public OrderedData {
    int_8               delta_col;
    int_16              delta_line;

    void DumpData();
};


struct UsageRec : public OrderedData {
    BRI_ReferenceType   type;
    int_8               delta_col;
    int_16              delta_line;
    union {
        BRI_StringID    target_index;
        BRI_TypeID      type_index;
    };
    union {
        DeclRec *       target_ptr;
        TypeRec *       type_ptr;
    };

    UsageRec() { target_ptr = NULL; }

    void DumpData();
    void ReIndex();
};


struct GuardRec : public OrderedData {
    BRI_GuardTypes      type;
    BRI_StringID        string_id;
    uint_32             num_params;
    uint_32             length;
    uint_8 *            defn;

    StringRec *         string_ptr;

    GuardRec() { defn = NULL; string_ptr = NULL; }
    ~GuardRec() { if( defn != NULL ) delete[] defn; }

    void DumpData();
    void ReIndex();
};


class OrderedList {
    protected:
        struct Link {
            Link *              _next;
            OrderedData *       _data;
        };

        Link *  _first;
        Link *  _last;
        Link *  _current;
        int     _count;
        int     _del_entries;

    public:
        OrderedList(int delete_entries);
        ~OrderedList();

        void            Append( OrderedData * data );
        OrderedData *   First();
        OrderedData *   Next();
        int             Count() { return _count; }
};


template<class T>
class HashTable {
    protected:
        struct Link {
            Link *      _next;
            T *         _data;
        };

        Link **         _table;

        int             Hash( uint_32 id );
    public:
        HashTable();
        ~HashTable() { delete[] _table; }

        void            Insert( T *hashdata );
        T *             Lookup( uint_32 id );
};


#endif  // _BRMDUMP_HPP

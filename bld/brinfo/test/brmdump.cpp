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


#include <stdio.h>
#include <string.h>

extern "C" {
#include "brmtypes.h"
}
#include "brmdump.hpp"

#define _PCH_HEADER_ONLY
#include "pcheader.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define USAGE           "USAGE:  brmdump [/pch] <filename> [<output filename>]\n"
#define FILE_ERR        "Unable to open %s\n"
#define BAD_CHECKSUM    "This is not a valid .BRM file.\n"
#define EOF_ERR         "Premature EOF at position %d.\n"
#define UNKNOWN_FLAG    "Unknown record flag %02X at position %d.\n"

#define NUM_BUCKETS     251

#define ANONYMOUS       "<ANONYMOUS>"


static FileP            infile;
static FileP            outfile;
static BRI_Header       header;
static OrderedList      record_list(TRUE);
static HashTable<DeclRec>       decl_list;
static HashTable<ScopeRec>      scope_list;
static HashTable<TypeRec>       type_list;
static HashTable<StringRec>     string_table;


void StringRec::DumpData()
/************************/
{
    fprintf( outfile, "String:  %08X\n", _index );
    fprintf( outfile, "    %s\n", _buf );
}


void DeclRec::DumpData()
/**********************/
{
    uint_8 *    buf;

    if( name_ptr != NULL ){
        buf = name_ptr->_buf;
    } else {
        buf = (uint_8 *) ANONYMOUS;
    }

    fprintf( outfile, "Declaration:  %08X\n", _index );
    fprintf( outfile, "    Flags:  %08X\n", attributes );
    fprintf( outfile, "    Name:   %s\n", buf );
    if( type_ptr != NULL ){
        fprintf( outfile, "    Type:   " );
        type_ptr->WriteType();
        fprintf( outfile, "  (%08X)\n", type_id );
    } else {
        fprintf( outfile, "    Type:   %08X\n", type_id );
    }
}


void DeclRec::ReIndex()
/*********************/
{
    name_ptr = string_table.Lookup( name_id );
    type_ptr = type_list.Lookup( type_id );
}


TypeRec::~TypeRec()
/*****************/
{
    if( ops != NULL ){
        delete[] ops;
    }
    if( typecode == BRI_TC_Function ){
        delete[] info.ra.args;
    }
}


void TypeRec::DumpData()
/**********************/
{
    fprintf( outfile, "Type:  %08X\n", _index );
    fprintf( outfile, "    " );
    WriteType();
    fprintf( outfile, "\n" );
}


void TypeRec::ReIndex()
/*********************/
{
    int         i;

    switch( typecode ){
        case BRI_TC_None:
        break;

        case BRI_TC_BaseType:
            info.bt.basetype = (BRI_BaseTypes) ops[0];
        break;

        case BRI_TC_Modifier:
            info.fp.flags = ops[0];
            info.fp.parent = type_list.Lookup( ops[1] );
        break;

        case BRI_TC_Pointer:
        case BRI_TC_Reference:
        case BRI_TC_TypeDef:
            info.p.parent = type_list.Lookup( ops[0] );
        break;

        case BRI_TC_PtrToMember:
            info.cm.host = type_list.Lookup( ops[0] );
            info.cm.member = type_list.Lookup( ops[1] );
        break;

        case BRI_TC_Array:
            info.se.size = ops[0];
            info.se.element = type_list.Lookup( ops[1] );
        break;

        case BRI_TC_Function:
            info.ra.args = new TypeRec *[num_ops];
            for( i=0; i<num_ops; i++ ){
                info.ra.args[i] = type_list.Lookup( ops[i] );
            }
        break;

        case BRI_TC_Class:
        case BRI_TC_Struct:
        case BRI_TC_Union:
        case BRI_TC_Enum:
            info.ns.name_ptr = string_table.Lookup( ops[0] );
            info.ns.symbol_ptr = decl_list.Lookup( ops[1] );
        break;

        case BRI_TC_BitField:
            info.w.width = ops[0];
        break;
    }
}


#define WRITE_TYPE(p,d)  if((p)!=NULL) \
                             (p)->WriteType(); \
                         else \
                             fprintf(outfile,"%08X",(d))

void TypeRec::WriteType()
/***********************/
{
    int         i;

    switch( typecode ){
        case BRI_TC_None:
        break;

        case BRI_TC_BaseType:
            switch( ops[0] ){
                case BRI_TYP_BOOL:
                    fprintf( outfile, "bool" );
                break;

                case BRI_TYP_CHAR:
                    fprintf( outfile, "char" );
                break;

                case BRI_TYP_SCHAR:
                    fprintf( outfile, "signed char" );
                break;

                case BRI_TYP_UCHAR:
                    fprintf( outfile, "unsigned char" );
                break;

                case BRI_TYP_WCHAR:
                    fprintf( outfile, "WCHAR" );
                break;

                case BRI_TYP_SSHORT:
                    fprintf( outfile, "short" );
                break;

                case BRI_TYP_USHORT:
                    fprintf( outfile, "unsigned short" );
                break;

                case BRI_TYP_SINT:
                    fprintf( outfile, "int" );
                break;

                case BRI_TYP_UINT:
                    fprintf( outfile, "unsigned int" );
                break;

                case BRI_TYP_SLONG:
                    fprintf( outfile, "long" );
                break;

                case BRI_TYP_ULONG:
                    fprintf( outfile, "unsigned long" );
                break;

                case BRI_TYP_SLONG64:
                    fprintf( outfile, "SLONG64" );
                break;

                case BRI_TYP_ULONG64:
                    fprintf( outfile, "ULONG64" );
                break;

                case BRI_TYP_FLOAT:
                    fprintf( outfile, "float" );
                break;

                case BRI_TYP_DOUBLE:
                    fprintf( outfile, "double" );
                break;

                case BRI_TYP_LONG_DOUBLE:
                    fprintf( outfile, "long double" );
                break;

                case BRI_TYP_VOID:
                    fprintf( outfile, "void" );
                break;

                case BRI_TYP_DOT_DOT_DOT:
                    fprintf( outfile, "..." );
                break;

                case BRI_TYP_GENERIC:
                    fprintf( outfile, "<GENERIC>" );
                break;
            }
        break;

        case BRI_TC_Modifier:
            if( info.fp.flags & 0x00000001 ){   // TF1_CONST
                fprintf( outfile, "const " );
            }
            WRITE_TYPE( info.fp.parent, ops[1] );
        break;

        case BRI_TC_Pointer:
            fputc('(', outfile);
            WRITE_TYPE( info.p.parent, ops[0] );
            fprintf( outfile, ") *" );
        break;

        case BRI_TC_PtrToMember:
            fputc('(', outfile);
            WRITE_TYPE( info.cm.host, ops[0] );
            fprintf( outfile, ")::(" );
            WRITE_TYPE( info.cm.member, ops[1] );
            fputc(')', outfile);
        break;

        case BRI_TC_Reference:
            fputc('(', outfile);
            WRITE_TYPE( info.p.parent, ops[0] );
            fprintf( outfile, ") &" );
        break;

        case BRI_TC_Array:
            fputc('(', outfile);
            WRITE_TYPE( info.se.element, ops[1] );
            fprintf( outfile, ")[%d]", info.se.size );
        break;

        case BRI_TC_Function:
            fputc('(', outfile);
            WRITE_TYPE( info.ra.args[0], ops[0] );
            fprintf( outfile, ")(" );
            for( i=1; i<num_ops; i++ ){
                WRITE_TYPE( info.ra.args[i], ops[i] );
                if( i < num_ops-1 ){
                    fputc(',', outfile);
                }
            }
            fputc(')', outfile);
        break;

        case BRI_TC_Class:
        case BRI_TC_Struct:
        case BRI_TC_Union:
        case BRI_TC_Enum:
            {
                uint_8 * buf;

                if( info.ns.name_ptr != NULL ){
                    buf = info.ns.name_ptr->_buf;
                } else {
                    buf = (uint_8 *)ANONYMOUS;
                }

                fprintf( outfile, "class %s", buf );
            }
        break;

        case BRI_TC_TypeDef:
            fprintf( outfile, "typedef " );
            WRITE_TYPE( info.p.parent, ops[0] );
        break;

        case BRI_TC_BitField:
            fprintf( outfile, "int : %d", info.w.width );
        break;
    }
}


void FileRec::DumpData()
/**********************/
{
    if( filename_ptr != NULL ){
        fprintf( outfile, "File name:  %s\n", filename_ptr->_buf );
    } else {
        fprintf( outfile, "File name:  %08X\n", filename_id );
    }
}


void FileRec::ReIndex()
/*********************/
{
    filename_ptr = string_table.Lookup( filename_id );
}


void PCHRec::DumpData()
/**********************/
{
    if( filename_ptr != NULL ){
        fprintf( outfile, "PCH name:  %s\n", filename_ptr->_buf );
    } else {
        fprintf( outfile, "PCH name:  %08X\n", filename_id );
    }
}


void PCHRec::ReIndex()
/*********************/
{
    filename_ptr = string_table.Lookup( filename_id );
}


void FileEndRec::DumpData()
/*************************/
{
    fprintf( outfile, "End file.\n" );
}


void TemplateRec::DumpData()
/**************************/
{
    if( filename_ptr != NULL ){
        fprintf( outfile, "Template source file:  %s\n", filename_ptr->_buf );
    } else {
        fprintf( outfile, "Template source file:  %08X\n", filename_id );
    }
}


void TemplateRec::ReIndex()
/*************************/
{
    filename_ptr = string_table.Lookup( filename_id );
}


void TemplateEndRec::DumpData()
/*****************************/
{
    fprintf( outfile, "End template.\n" );
}


void ScopeRec::DumpData()
/***********************/
{
    uint_8 *    buf;
    fprintf( outfile, "Scope: %08X\n", _index );
    fprintf( outfile, "    Flags:  %02X\n", flags );
    if( flags == BRI_ST_Function && fn_ptr.fn_name_ptr != NULL ){
        buf = fn_ptr.fn_name_ptr->_buf;
        fprintf( outfile, "    Symbol: %s\n", buf );
        if( fn_ptr.fn_type_ptr != NULL ){
            fprintf( outfile, "    Type:  " );
            fn_ptr.fn_type_ptr->WriteType();
            fprintf( outfile, " (%08X)\n", fn_id.fn_type_index );
        }
    } else if( flags == BRI_ST_Class && type_ptr != NULL ){
        fprintf( outfile, "    Type:   " );
        type_ptr->WriteType();
        fprintf( outfile, " (%08X)\n", type_index );
    } else {
        fprintf( outfile, "    Index:  %08X\n", type_index );
    }
}


void ScopeRec::ReIndex()
/**********************/
{
    if( flags == BRI_ST_Function ){
        fn_ptr.fn_name_ptr = string_table.Lookup( fn_id.fn_name_index );
        fn_ptr.fn_type_ptr = type_list.Lookup( fn_id.fn_type_index );
    } else {
        type_ptr = type_list.Lookup( type_index );
    }
}


void ScopeEndRec::DumpData()
/**************************/
{
    fprintf( outfile, "End Scope.\n" );
}


void DefnRec::DumpData()
/**********************/
{
    uint_8 *    name_buf;

    if( symbol_ptr != NULL ){
        if( symbol_ptr->name_ptr != NULL ){
            name_buf = symbol_ptr->name_ptr->_buf;
        } else {
            name_buf = (uint_8 *) ANONYMOUS;
        }
        fprintf( outfile, "Definition: %s (%08X)\n", name_buf, symbol_index );
    } else {
        fprintf( outfile, "Definition: %08X\n", symbol_index );
    }

    if( filename_ptr != NULL ){
        fprintf( outfile, "    Filename: %s\n", filename_ptr->_buf );
    } else {
        fprintf( outfile, "    Filename: %08X\n", filename_id );
    }
    fprintf( outfile, "    Column:   %d\n", delta_col );
    fprintf( outfile, "    Line:     %d\n", delta_line );
}


void DefnRec::ReIndex()
/*********************/
{
    filename_ptr = string_table.Lookup( filename_id );
    symbol_ptr = decl_list.Lookup( symbol_index );
}


void DeltaRec::DumpData()
/***********************/
{
    fprintf( outfile, "Delta:\n" );
    fprintf( outfile, "    DCol:   %d\n", delta_col );
    fprintf( outfile, "    DLine:  %d\n", delta_line );
}


void UsageRec::DumpData()
/***********************/
{
    uint_8 *    name_buf;
    fprintf( outfile, "Reference:\n" );
    fprintf( outfile, "    Flags:  %X\n", type );
    fprintf( outfile, "    DCol:   %d\n", delta_col );
    fprintf( outfile, "    DLine:  %d\n", delta_line );
    if( type == BRI_RT_TypeOf ){
        if( type_ptr != NULL ){
            fprintf( outfile, "    Target Type:  " );
            type_ptr->WriteType();
            fprintf( outfile, " (%08X)\n", type_index );
        } else {
            fprintf( outfile, "    Target Type:  %08X\n", type_index );
        }
    } else {
        if( target_ptr != NULL ){
            if( target_ptr->name_ptr != NULL ){
                name_buf = target_ptr->name_ptr->_buf;
            } else {
                name_buf = (uint_8 *) ANONYMOUS;
            }
            fprintf( outfile, "    Target Symbol:  %s (%08X)\n",
                     name_buf, target_index );
        } else {
            fprintf( outfile, "    Target Symbol:  %08X\n", target_index );
        }
    }
}


void UsageRec::ReIndex()
/**********************/
{
    if( type == BRI_RT_TypeOf ){
        type_ptr = type_list.Lookup( type_index );
    } else {
        target_ptr = decl_list.Lookup( target_index );
    }
}


void GuardRec::DumpData()
/***********************/
{
    int         i;

    fprintf( outfile, "Guard (type %02X):\n", type );
    if( string_ptr != NULL ){
        fprintf( outfile, "    %s\n", string_ptr->_buf );
    } else {
        fprintf( outfile, "    %08X\n", string_id );
    }
    if( type == BRI_GT_Value || type == BRI_GT_RefValue ||
        type == BRI_GT_Declaration ){
        fprintf( outfile, "    # Parameters: %d\n", num_params );
        if( length > 0 ){
            fprintf( outfile, "    Definition:" );
            for( i=0; i<length; i++ ){
                fprintf( outfile, " %02X", defn[i] );
            }
            fputc( '\n', outfile );
        }
    }
}


void GuardRec::ReIndex()
/**********************/
{
    string_ptr = string_table.Lookup( string_id );
}


OrderedList::OrderedList(int delete_entries)
/******************************************/
{
    _first = NULL;
    _last = NULL;
    _current = NULL;
    _count = 0;
    _del_entries = delete_entries;
}


OrderedList::~OrderedList()
/*************************/
{
    Link *      current = _first;
    Link *      prev = NULL;

    while( current != NULL ) {
        prev = current;
        current = current->_next;
        if( _del_entries ){
            delete prev->_data;
        }
        delete prev;
    }
}


void OrderedList::Append( OrderedData * data )
/********************************************/
{
    Link *      link = new Link;

    link->_next = NULL;
    link->_data = data;

    if( _last == NULL ){
        _first = _last = link;
    } else {
        _last->_next = link;
        _last = link;
    }
    _count += 1;
}


OrderedData * OrderedList::First()
/********************************/
{
    OrderedData *       result = NULL;

    _current = _first;
    if( _current != NULL ){
        result = _current->_data;
    }
    return result;
}


OrderedData * OrderedList::Next()
/*******************************/
{
    OrderedData *       result = NULL;

    if( _current != NULL ){
        _current = _current->_next;
        if( _current != NULL ){
            result = _current->_data;
        }
    }
    return result;
}


template<class T>
HashTable<T>::HashTable()
/************************/
{
    _table = new Link *[NUM_BUCKETS];
    memset( _table, 0, NUM_BUCKETS*sizeof(Link *));
}


template<class T>
int HashTable<T>::Hash( uint_32 id )
/*********************************/
{
    //return ((int) (id << 8)) % NUM_BUCKETS;
    uint_32 s = id;
    uint_32 c;
    uint_32 g;
    uint_32 h;

    h = 0x4;
    c = 0x4;
    c += s;
    h = ( h << 4 ) + c;
    g = h & ~0x0ffffff;
    h ^= g;
    h ^= g >> (4+4+4+4+4);
    g = h & ~0x0fff;
    h ^= g;
    h ^= g >> (4+4+4);
    h ^= h >> (2+4);
    return( (int) (h%NUM_BUCKETS) );
}


template<class T>
void HashTable<T>::Insert( T *hashdata )
/**************************************/
{
    Link *      current;
    Link *      prev;
    Link *      link = new Link;
    int         index = Hash(hashdata->_index);

    current = _table[ index ];
    prev = NULL;
    while( current != NULL && current->_data->_index < hashdata->_index ){
        prev = current;
        current = current->_next;
    }
    if( current != NULL ){
        link->_next = current;
    } else {
        link->_next = NULL;
    }
    if( prev != NULL ){
        prev->_next = link;
    } else {
        _table[ index ] = link;
    }
    link->_data = hashdata;
}


template<class T>
T * HashTable<T>::Lookup( uint_32 id )
/************************************/
{
    int         index = Hash( id );
    Link *      current;
    T *         result = NULL;

    current = _table[ index ];
    while( current != NULL && current->_data->_index < id ){
        current = current->_next;
    }

    if( current != NULL && current->_data->_index == id ){
        result = current->_data;
    }
    return result;
}


void DumpHeader()
/***************/
{
    fprintf( outfile
           , "Watcom browse file v%d.%d\n"
           , header.major_ver
           , header.minor_ver );
    if( header.num_declaration > 0 ){
        fprintf( outfile, "Declaration nodes: %2d\n", header.num_declaration);
    }
    if( header.num_file > 0 ){
        fprintf( outfile, "File nodes:        %2d\n", header.num_file );
    }
    if( header.num_scope > 0 ){
        fprintf( outfile, "Scope nodes:       %2d\n", header.num_scope );
    }
    if( header.num_definition > 0 ){
        fprintf( outfile, "Definition nodes:  %2d\n", header.num_definition );
    }
    if( header.num_delta > 0 ){
        fprintf( outfile, "Delta nodes:       %2d\n", header.num_delta );
    }
    if( header.num_usage > 0 ){
        fprintf( outfile, "Usage nodes:       %2d\n", header.num_usage );
    }
    if( header.num_string > 0 ){
        fprintf( outfile, "String nodes:      %2d\n", header.num_string );
    }
    if( header.num_type > 0 ){
        fprintf( outfile, "Type nodes:        %2d\n", header.num_type );
    }
    if( header.num_guard > 0 ){
        fprintf( outfile, "Guard nodes:       %2d\n", header.num_guard );
    }
    if( header.num_template > 0 ){
        fprintf( outfile, "Template nodes:    %2d\n", header.num_template );
    }
}


void DumpHeaderFinal()
/********************/
{
    OrderedData *       current;

    current = record_list.First();
    while( current != NULL ){
        current->ReIndex();
        current = record_list.Next();
    }
    current = record_list.First();
    while( current != NULL ){
        current->DumpData();
        current = record_list.Next();
    }

    if( header.num_declaration != 0 ){
        fprintf( outfile, "The number of declaration nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_declaration );
    }
    if( header.num_file != 0 ){
        fprintf( outfile, "The number of file nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_file );
    }
    if( header.num_scope != 0 ){
        fprintf( outfile, "The number of scope nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_scope );
    }
    if( header.num_delta != 0 ){
        fprintf( outfile, "The number of delta nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_delta );
    }
    if( header.num_definition != 0 ){
        fprintf( outfile, "The number of definition nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_definition );
    }
    if( header.num_string != 0 ){
        fprintf( outfile, "The number of string nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_string );
    }
    if( header.num_usage != 0 ){
        fprintf( outfile, "The number of usage nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_usage );
    }
    if( header.num_type != 0 ){
        fprintf( outfile, "The number of type nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_type );
    }
    if( header.num_guard != 0 ){
        fprintf( outfile, "The number of guard nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_guard );
    }
    if( header.num_template != 0 ){
        fprintf( outfile, "The number of template nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_template );
    }
    if( header.num_pch != 0 ){
        fprintf( outfile, "The number of PCH nodes was incorrect.\n" );
        fprintf( outfile, "    It was off by %d.\n", header.num_pch );
    }
}


int ReadDeclaration()
/*******************/
{
    static const int    result = sizeof(BRI_SymbolID)
                               + BRI_SIZE_SYMBOLATTRIBUTES
                               + sizeof(BRI_StringID)
                               + sizeof(BRI_TypeID);
    DeclRec *   decl_rec = new DeclRec;

    fread( &decl_rec->_index, sizeof(BRI_SymbolID), 1, infile );
    fread( &decl_rec->attributes, BRI_SIZE_SYMBOLATTRIBUTES, 1, infile );
    fread( &decl_rec->name_id, sizeof(BRI_StringID), 1, infile );
    fread( &decl_rec->type_id, sizeof(BRI_TypeID), 1, infile );

    record_list.Append( decl_rec );
    decl_list.Insert( decl_rec );

    return result;
}


int ReadFile()
/************/
{
    static const int    result = sizeof(BRI_StringID);
    FileRec *           file_rec = new FileRec;

    fread( &file_rec->filename_id, sizeof(BRI_StringID), 1, infile );
    record_list.Append( file_rec );

    return result;
}


int ReadPCH()
/***********/
{
    static const int    result = sizeof(BRI_StringID);
    PCHRec *            pch_rec = new PCHRec;

    fread( &pch_rec->filename_id, sizeof(BRI_StringID), 1, infile );
    record_list.Append( pch_rec );

    return result;
}


int ReadFileEnd()
/***************/
{
    record_list.Append( new FileEndRec );
    return 0;
}


int ReadTemplate()
/****************/
{
    static const int    result = sizeof(BRI_StringID);
    TemplateRec *       template_rec = new TemplateRec;

    fread( &template_rec->filename_id, sizeof(BRI_StringID), 1, infile );

    record_list.Append( template_rec );

    return result;
}


int ReadTemplateEnd()
/*******************/
{
    record_list.Append( new TemplateEndRec );
    return 0;
}


int ReadScope()
/*************/
{
    static const int    result = sizeof(BRI_ScopeID)
                               + BRI_SIZE_SCOPETYPE;
    ScopeRec *          scope_rec = new ScopeRec;

    fread( &scope_rec->_index, sizeof(BRI_ScopeID), 1, infile );
    fread( &scope_rec->flags, BRI_SIZE_SCOPETYPE, 1, infile );
    if( scope_rec->flags == BRI_ST_Function ){
        fread( &scope_rec->fn_id.fn_name_index, sizeof(BRI_StringID), 1, infile );
        fread( &scope_rec->fn_id.fn_type_index, sizeof(BRI_TypeID), 1, infile );
    } else {
        fread( &scope_rec->type_index, sizeof(BRI_TypeID), 1, infile );
    }
    record_list.Append( scope_rec );
    scope_list.Insert( scope_rec );

    if( scope_rec->flags == BRI_ST_Function ){
        return result + sizeof(BRI_StringID) + sizeof(BRI_TypeID);
    } else {
        return result + sizeof(BRI_TypeID);
    }
}


int ReadScopeEnd()
/****************/
{
    record_list.Append( new ScopeEndRec );
    return 0;
}

int ReadDelta()
/*************/
{
    static const int    result = sizeof(uint_8)
                               + sizeof(uint_16);
    DeltaRec *          delta_rec = new DeltaRec;

    fread( &delta_rec->delta_col, sizeof(int_8), 1, infile );
    fread( &delta_rec->delta_line, sizeof(int_16), 1, infile );
    record_list.Append( delta_rec );

    return result;
}

int ReadUsage()
/*************/
{
    static const int    result = BRI_SIZE_REFERENCETYPE
                               + sizeof(int_8)
                               + sizeof(int_16)
                               + sizeof(BRI_StringID);
    UsageRec *          usage_rec = new UsageRec;

    fread( &usage_rec->type, BRI_SIZE_REFERENCETYPE, 1, infile );
    fread( &usage_rec->delta_col, sizeof(int_8), 1, infile );
    fread( &usage_rec->delta_line, sizeof(int_16), 1, infile );
    fread( &usage_rec->target_index, sizeof(BRI_StringID), 1, infile );
    record_list.Append( usage_rec );

    return result;
}


int ReadString()
/**************/
{
    static const int    result = sizeof(BRI_StringID)
                               + sizeof(uint_32);
    uint_32             string_length;
    StringRec           *hstring = new StringRec;

    fread( &hstring->_index, sizeof(BRI_StringID), 1, infile );
    fread( &string_length, sizeof(uint_32), 1, infile );
    hstring->_buf = new uint_8[ string_length ];
    fread( hstring->_buf, 1, string_length, infile );

    record_list.Append( hstring );
    string_table.Insert( hstring );

    return result + string_length;
}


int ReadType()
/************/
{
    static const int    result = sizeof(BRI_TypeID)
                               + BRI_SIZE_TYPECODE
                               + sizeof(uint_32);
    TypeRec *           type_rec = new TypeRec;

    fread( &type_rec->_index, sizeof(BRI_TypeID), 1, infile );
    fread( &type_rec->typecode, BRI_SIZE_TYPECODE, 1, infile );
    fread( &type_rec->num_ops, sizeof(uint_32), 1, infile );
    type_rec->ops = new uint_32[ type_rec->num_ops ];
    fread( type_rec->ops, sizeof(uint_32), type_rec->num_ops, infile );

    record_list.Append( type_rec );
    type_list.Insert( type_rec );

    return result + type_rec->num_ops * sizeof(uint_32);
}


int ReadGuard()
/*************/
{
    static const int    result = BRI_SIZE_GUARDTYPES
                               + sizeof(BRI_StringID)
                               + sizeof( uint_32 )
                               + sizeof( uint_32 );
    GuardRec *          guard_rec = new GuardRec;

    fread( &guard_rec->type, BRI_SIZE_GUARDTYPES, 1, infile );
    fread( &guard_rec->string_id, sizeof(BRI_StringID), 1, infile );
    fread( &guard_rec->num_params, sizeof(uint_32), 1, infile );
    fread( &guard_rec->length, sizeof(uint_32), 1, infile );
    if( guard_rec->length > 0 ){
        guard_rec->defn = new uint_8[guard_rec->length];
        fread( guard_rec->defn, sizeof(uint_8), guard_rec->length, infile );
    }

    record_list.Append( guard_rec );

    return result + guard_rec->length;
}


int ReadDefinition()
/******************/
{
    static const int    result = sizeof(uint_32)
                               + sizeof(uint_32)
                               + sizeof(BRI_StringID)
                               + sizeof(BRI_SymbolID);
    DefnRec *           defn_rec = new DefnRec;

    fread( &defn_rec->delta_col, sizeof(uint_32), 1, infile );
    fread( &defn_rec->delta_line, sizeof(uint_32), 1, infile );
    fread( &defn_rec->filename_id, sizeof(BRI_StringID), 1, infile );
    fread( &defn_rec->symbol_index, sizeof(BRI_SymbolID), 1, infile );

    record_list.Append( defn_rec );
    return result;
}


int main(int argc, char *argv[])
/******************************/
{
    int                 file_len;
    int                 file_pos;
    int                 record_flag;

    int                 delta;
    int                 decl_size;
    int                 usage_size;
    int                 file_size;
    int                 scope_size;
    int                 string_size;
    int                 type_size;
    int                 delta_size;
    int                 defn_size;
    int                 guard_size;
    int                 template_size;

    int                 arg;
    int                 pch_file;

    if( argc < 2 || argc > 4 ){
        fprintf( stderr, USAGE );
        return 0;
    }

    // Command-line processing

    arg = 1;

    if( stricmp( argv[arg], "/pch" ) == 0 ) {
        if( argc < 3 ){
            fprintf( stderr, USAGE );
            return 0;
        }
        pch_file = TRUE;
        arg++;
    } else {
        if( argc > 3 ){
            fprintf( stderr, USAGE );
            return 0;
        }
        pch_file = FALSE;
    }

    infile = fopen( argv[arg], "rb" );
    if( !infile ){
        fprintf( stderr, FILE_ERR, argv[1] );
        return -1;
    }
    arg++;
    if( arg < argc ){
        outfile = fopen( argv[arg], "w" );
        if( !outfile ){
            fprintf( stderr, FILE_ERR, argv[2] );
            return -1;
        }
    } else {
        outfile = stdout;
    }

    fseek( infile, 0, SEEK_END );
    file_len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

    if( pch_file ){
        auto precompiled_header_header hdr;
        fread( &hdr, sizeof( hdr ), 1, infile );
        fseek( infile, hdr.browse_info, SEEK_SET );
        file_len -= ftell( infile );
    }

    fread( &header, sizeof( header ), 1, infile );
    if( header.magic != BRI_MAGIC || header.file_len != file_len ){
        fprintf( stderr, BAD_CHECKSUM );
        return -1;
    }

    DumpHeader();
    decl_size = 0;
    usage_size = 0;
    file_size = 0;
    scope_size = 0;
    string_size = 0;
    type_size = 0;
    delta_size = 0;
    defn_size = 0;
    guard_size = 0;
    template_size = 0;

    file_pos = sizeof( header );
    while( file_pos < file_len ){
        record_flag = fgetc( infile );
        if( record_flag == EOF ){
            fprintf( stderr, EOF_ERR, file_len );
            return -1;
        }
        file_pos ++;
        switch( record_flag ){
            case BRI_Rec_Declaration:
                delta = ReadDeclaration();
                file_pos += delta;
                decl_size += delta;
                header.num_declaration--;
            break;

            case BRI_Rec_File:
                delta = ReadFile();
                file_pos += delta;
                file_size += delta;
                header.num_file--;
            break;

            case BRI_Rec_FileEnd:
                delta = ReadFileEnd();
                file_pos += delta;
                file_size += delta;
            break;

            case BRI_Rec_Template:
                delta = ReadTemplate();
                file_pos += delta;
                template_size += delta;
                header.num_template--;
            break;

            case BRI_Rec_TemplateEnd:
                delta = ReadTemplateEnd();
                file_pos += delta;
                template_size += delta;
            break;

            case BRI_Rec_Scope:
                delta = ReadScope();
                file_pos += delta;
                scope_size += delta;
                header.num_scope--;
            break;

            case BRI_Rec_ScopeEnd:
                delta = ReadScopeEnd();
                file_pos += delta;
                scope_size += delta;
            break;

            case BRI_Rec_Delta:
                delta = ReadDelta();
                file_pos += delta;
                delta_size += delta;
                header.num_delta--;
            break;

            case BRI_Rec_Usage:
                delta = ReadUsage();
                file_pos += delta;
                usage_size += delta;
                header.num_usage--;
            break;

            case BRI_Rec_String:
                delta = ReadString();
                file_pos += delta;
                string_size += delta;
                header.num_string--;
            break;

            case BRI_Rec_Type:
                delta = ReadType();
                file_pos += delta;
                type_size += delta;
                header.num_type--;
            break;

            case BRI_Rec_Guard:
                delta = ReadGuard();
                file_pos += delta;
                guard_size += delta;
                header.num_guard--;
            break;

            case BRI_Rec_Definition:
                delta = ReadDefinition();
                file_pos += delta;
                defn_size += delta;
                header.num_definition--;
            break;

            case BRI_Rec_PCHInclude:
                delta = ReadPCH();
                file_pos += delta;
                header.num_pch--;
            break;

            default:
                fprintf( stderr, UNKNOWN_FLAG, record_flag, file_pos );
                return -1;
        }
    }
    fprintf( outfile, "decl_size = %d\n", decl_size);
    fprintf( outfile, "usage_size = %d\n", usage_size);
    fprintf( outfile, "file_size = %d\n", file_size);
    fprintf( outfile, "scope_size = %d\n", scope_size);
    fprintf( outfile, "string_size = %d\n", string_size);
    fprintf( outfile, "type_size = %d\n", type_size);
    fprintf( outfile, "delta_size = %d\n", delta_size);
    fprintf( outfile, "defn_size = %d\n", defn_size);
    fprintf( outfile, "guard_size = %d\n", guard_size);
    fprintf( outfile, "template_size = %d\n", template_size );

    DumpHeaderFinal();

    return 0;
}

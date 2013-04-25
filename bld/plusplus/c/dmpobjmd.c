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


#include "plusplus.h"
#include "errdefns.h"
#include "pragdefn.h"
#include "vbuf.h"
#include "vstk.h"

typedef enum dump_struct_control {
    DS_BASE     = 0x0001,
    DS_NULL     = 0x0000,
} ds_control;

typedef struct                  // DUMP INFORMATION
{   target_size_t offset;       // - offset of current structure
    VSTK_CTL stack;             // - parentage stack
    VBUF buffer;                // - buffer for printing
    TYPE original;              // - original type
} DUMP_INFO;


static DUMP_INFO* bufferRewind(   // INITIALIZE BUFFER
    DUMP_INFO* di )             // - dump information
{
    VbufRewind( &di->buffer );
    return di;
}


static DUMP_INFO* bufferChr(    // CONCATENATE CHAR TO BUFFER
    DUMP_INFO* di,              // - dump information
    char chr )                  // - to be concatenated
{
    VbufConcChr( &di->buffer, chr );
    return di;
}


static DUMP_INFO* bufferStr(    // CONCATENATE STRING TO BUFFER
    DUMP_INFO* di,              // - dump information
    char* str )                 // - to be concatenated
{
    VbufConcStr( &di->buffer, str );
    return di;
}


static DUMP_INFO* bufferNmb(    // CONCATENATE NUMBER TO BUFFER
    DUMP_INFO* di,              // - dump information
    unsigned numb )             // - to be concatenated
{
    char buf[16];               // - buffer

    VbufConcDecimal( &di->buffer, numb );
    if( numb >= 10 ) {
        itoa( numb, buf, 16 );
        di = bufferStr( di, "/0x" );
        di = bufferStr( di, buf );
    }
    return di;
}


static DUMP_INFO* bufferInit(   // INITIALIZE BUFFER (NON-TITLE LINE)
    DUMP_INFO* di )             // - dump information
{
    di = bufferRewind( di );
    di = bufferStr( di, "    " );
    return di;
}


static void vbufWrite(          // WRITE A VBUFFER
    VBUF* vbuf )                // - the VBUF to be written
{
    MsgDisplayLine( VbufString( vbuf ) );
}


static DUMP_INFO* bufferWrite(  // WRITE A BUFFER
    DUMP_INFO* di )             // - dump information
{
    vbufWrite( &di->buffer );
    return di;
}


static void dumpDirect( BASE_CLASS*, void * );
static void dumpVirtual( BASE_CLASS*, void * );


static DUMP_INFO* dumpTitle(    // DUMP A TITLE LINE
    DUMP_INFO* di,              // - dump information
    char* title,                // - title line
    char* class_name )          // - name of class
{
    di = bufferRewind( di );
    di = bufferChr( di, '\n' );
    di = bufferStr( di, title );
    di = bufferChr( di, ' ' );
    di = bufferStr( di, class_name );
    di = bufferWrite( di );
    return di;
}


static DUMP_INFO* dumpOffset(   // DUMP OFFSET LINE
    DUMP_INFO* di )             // - dump information
{
    di = bufferInit( di );
    di = bufferStr( di, "offset of class: " );
    di = bufferNmb( di, di->offset );
    di = bufferWrite( di );
    return di;
}


static DUMP_INFO* dumpParentage( // DUMP PARENTAGE
    DUMP_INFO* di )             // - dump information
{
    char**daughter;             // - daughter class

    for( daughter = VstkTop( &di->stack ); ; ) {
        daughter = VstkNext( &di->stack, daughter );
        if( daughter == NULL ) break;
        di = bufferInit( di );
        di = bufferStr( di, "base of: " );
        di = bufferStr( di, *daughter );
        di = bufferWrite( di );
    }
    return di;
}


static DUMP_INFO* dumpBitMemb(  // DUMP A BITFIELD MEMBER
    DUMP_INFO* di,              // - dump information
    char* kind,                 // - kind of field
    char* name,                 // - field name
    target_offset_t offset,     // - field offset
    target_size_t start,        // - field start
    target_size_t size )        // - field size
{
    di = bufferInit( di );
    di = bufferStr( di, kind );
    di = bufferChr( di, ' ' );
    di = bufferStr( di, name );
    di = bufferStr( di, ", offset = " );
    di = bufferNmb( di, offset );
    di = bufferStr( di, ", bit offset =" );
    di = bufferNmb( di, start );
    di = bufferStr( di, ", bit width =" );
    di = bufferNmb( di, size );
    di = bufferWrite( di );
    return di;
}


static DUMP_INFO* dumpDataMemb( // DUMP A DATA MEMBER
    DUMP_INFO* di,              // - dump information
    char* kind,                 // - kind of field
    char* name,                 // - field name
    target_offset_t offset,     // - field offset
    target_size_t size )        // - field size
{
    di = bufferInit( di );
    di = bufferStr( di, kind );
    di = bufferChr( di, ' ' );
    di = bufferStr( di, name );
    di = bufferStr( di, ", offset = " );
    di = bufferNmb( di, offset );
    di = bufferStr( di, ", size = " );
    di = bufferNmb( di, size );
    di = bufferWrite( di );
    return di;
}


static void dumpMember(         // DUMP A MEMBER
    SYMBOL memb,                // - member
    void *_di )                 // - dump information
{
    DUMP_INFO* di = _di;
    target_offset_t offset;     // - offset of symbol
    TYPE type;                  // - type of symbol
    NAME name;                  // - symbol's name

    offset = di->offset + memb->u.member_offset;
    name = memb->name->name;
    type = TypedefModifierRemove( memb->sym_type );
    if( type->id == TYP_BITFIELD ) {
        di = dumpBitMemb( di
                        , "bit member:"
                        , name
                        , offset
                        , type->u.b.field_start
                        , type->u.b.field_width );
    } else {
        di = dumpDataMemb( di
                         , "member:"
                         , name
                         , offset
                         , CgMemorySize( type ) );
    }
}


static DUMP_INFO* dumpStruct(   // DUMP A STRUCTURE
    TYPE type,                  // - structure type
    DUMP_INFO* di,              // - dump information
    char* title,                // - title for dump
    ds_control control )        // - control word
{
    CLASSINFO* info;            // - class information
    NAME *parent;               // - where parent ptr is stored

    control = control;
    type = StructType( type );
    info = type->u.c.info;
    parent = VstkPush( &di->stack );
    *parent = info->name;
    di = dumpTitle( di, title, info->name );
    if( type != di->original ) {
        di = bufferInit( di );
        di = bufferStr( di, "embedded size: " );
        di = bufferNmb( di, info->vsize );
        di = bufferWrite( di );
        di = dumpOffset( di );
        di = dumpParentage( di );
    } else {
        di = bufferInit( di );
        di = bufferStr( di, "size: " );
        di = bufferNmb( di, info->size );
        di = bufferWrite( di );
    }
    if( info->has_vbptr ) {
        di = dumpDataMemb( di
                         , "[virtual"
                         , "base pointer]"
                         , info->vb_offset + di->offset
                         , CgMemorySize( TypePtrToVoid() ) );
    }
    if( info->has_vfptr ) {
        di = dumpDataMemb( di
                         , "[virtual"
                         , "functions pointer]"
                         , info->vf_offset + di->offset
                         , CgMemorySize( TypePtrToVoid() ) );
    }
    ScopeWalkDataMembers( type->u.c.scope, dumpMember, di );
    if( type == di->original ) {
        ScopeWalkVirtualBases( type->u.c.scope, dumpVirtual, di );
    }
    ScopeWalkDirectBases( type->u.c.scope, dumpDirect, di );
    VstkPop( &di->stack );
    return di;
}


static void dumpBase(           // DUMP BASE
    BASE_CLASS* base,           // - base information
    DUMP_INFO* di,              // - dump information
    char* title )               // - title
{
    di->offset += base->delta;
    di = dumpStruct( base->type, di, title, DS_BASE );
    di->offset -= base->delta;
}


static void dumpVirtual(        // DUMP VIRTUAL BASE
    BASE_CLASS* vbase,          // - virtual base
    void * _di )             // - dump information
{
    DUMP_INFO* di = _di;
    dumpBase( vbase, di, "Virtual Base:" );
}


static void dumpDirect(         // DUMP DIRECT BASE
    BASE_CLASS* dbase,          // - direct base
    void * _di )             // - dump information
{
    DUMP_INFO* di = _di;
    dumpBase( dbase, di, "Direct Base:" );
}


void DumpObjectModelClass(      // DUMP OBJECT MODEL: CLASS
    TYPE type )                 // - structure type
{
    DUMP_INFO di;               // - dump information

    if( ! type->u.c.info->corrupted ) {
        CompFlags.log_note_msgs = TRUE;
        di.original = type;
        di.offset = 0;
        VbufInit( &di.buffer );
        VstkOpen( &di.stack, sizeof( char* ), 16 );
        dumpStruct( type, &di, "Object Model for:", DS_NULL );
        VbufFree( &di.buffer );
        VstkClose( &di.stack );
        CompFlags.log_note_msgs = FALSE;
    }
}


void DumpObjectModelEnum(       // DUMP OBJECT MODEL: ENUM
    TYPE type )                 // - enum type
{
    SYMBOL sym;                 // - current symbol
    TYPE base;                  // - base type
    VBUF buffer;                // - printing buffer
    char buf[16];               // - buffer
    long numb;                  // - a numeric value
    NAME name;                  // - name to be printed
    boolean sign;               // - TRUE ==> signed enum
    unsigned long mask;         // - used to mask to true size
    unsigned long val;          // - value as unsigned

    CompFlags.log_note_msgs = TRUE;
    base = TypedefModifierRemoveOnly( type );
    sym = base->u.t.sym;
    VbufInit( &buffer );
    VbufConcStr( &buffer, "Object Model for: " );
    name = sym->name->name;
    if( NULL == name || name[0] == '.' ) {
        name = "anonymous enum type";
    }
    VbufConcStr( &buffer, name );
    switch( TypedefModifierRemove( base->of ) -> id ) {
      case TYP_CHAR :
      case TYP_UCHAR :
        name = "unsigned char";
        sign = FALSE;
        break;
      case TYP_SCHAR :
        name = "signed char";
        sign = TRUE;
        break;
      case TYP_SSHORT :
        name = "signed short";
        sign = TRUE;
        break;
      case TYP_USHORT :
        name = "unsigned short";
        sign = FALSE;
        break;
      case TYP_SINT :
        name = "signed int";
        sign = TRUE;
        break;
      case TYP_UINT :
        name = "unsigned int";
        sign = FALSE;
        break;
      case TYP_SLONG :
        name = "signed long";
        sign = TRUE;
        break;
      case TYP_ULONG :
        name = "unsigned long";
        sign = FALSE;
        break;
      case TYP_SLONG64 :
        name = "__int64";
        sign = TRUE;
        break;
      case TYP_ULONG64 :
        name = "unsigned __int64";
        sign = FALSE;
        break;
      DbgDefault( "DumpObjectModelEnum -- bad underlying type" );
    }
    VbufConcStr( &buffer, ", base type is " );
    VbufConcStr( &buffer, name );
    vbufWrite( &buffer );
    mask = CgMemorySize( base );
    if( mask == sizeof( unsigned long ) ) {
        mask = -1;
    } else {
        mask = ( 1 << ( mask * 8 ) ) - 1;
    }
    for( ; ; ) {
        sym = sym->thread;
        if( ! SymIsEnumeration( sym ) ) break;
        VbufRewind( &buffer );
        VbufConcStr( &buffer, "    " );
        VbufConcStr( &buffer, sym->name->name );
        VbufConcStr( &buffer, " = " );
        numb = sym->u.sval;
        if( sign && numb < 0 ) {
            VbufConcChr( &buffer, '-' );
            VbufConcDecimal( &buffer, -numb );
        } else {
            VbufConcDecimal( &buffer, numb );
        }
        val = mask & numb;
        if( val > 10 ) {
            itoa( val, buf, 16 );
            VbufConcStr( &buffer, " /0x" );
            VbufConcStr( &buffer, buf );
        }
        vbufWrite( &buffer );
    }
    VbufFree( &buffer );
    CompFlags.log_note_msgs = FALSE;
}

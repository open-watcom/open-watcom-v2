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
* Description:  Watcom format input routines.
*
****************************************************************************/


#include <malloc.h>
#include <string.h>
#include "watcom.h"
#include "womp.h"
#include "genutil.h"
#include "watdbg.h"
#include "segment.h"
#include "myassert.h"
#include "cantype.h"
#include "canaddr.h"
#include "memutil.h"
#include "namemgr.h"
#include "cansymb.h"
#include "canmisc.h"
#include "array.h"
#ifndef NDEBUG
#include <io.h>
#include <fcntl.h>
#endif

/*
    The type map is implemented with variable sized arrays. (array.h)
*/
typedef struct {
    type_handle hdl;
} type_map_elm;

/*
    The offset map is a list of DDSymbols->data offsets of CODE_BLOCKs and
    their symb_handle.
*/
typedef struct offset_map {
    struct offset_map   *next;
    uint_16             offset;
    symb_handle         hdl;
} offset_map;

/*
    These structures maintain the current position within ddTypes->data
    or DDSymbols->data according to the boolean parsingTypes.
*/
struct pos {
    struct pos          *next;      /* stack of positions */
    uint_8              *ptr;       /* pointer to current position */
    uint_8              *start_rec; /* start of the current record */
    uint_8              *finish;    /* done when wat->ptr == wat->finish */
    uint_16             index;      /* index of the current record */
    uint_8              rec_len;    /* length of the current record*/
};

#define endOfRecord()   ( wat->ptr - wat->start_rec == wat->rec_len )

/*
    Some pointers we'll allocate memory for
*/
STATIC type_handle      unDefdType;
STATIC type_handle      *stbHdl;    /* table of 128 type_handles */
STATIC struct pos       *wat;
STATIC int              parsingTypes; /* boolean */
STATIC offset_map       *offsetMap;
STATIC symb_handle      headSymb;
STATIC seghdr           *ddTypes;
STATIC seghdr           *ddSymbols;
STATIC array_hdr        *typeMap;
STATIC type_map_elm     typeMapDef;

/*
    When we've processed a record, we splat the record type.  The different
    types indicate whether to count the record towards indexes or not.
    (indicies are not used during the symbol parsing)
*/
enum {
    SPLAT               = 0xf0,
    SPLAT_COUNT         = 0xf0,
    SPLAT_DONT_COUNT    = 0xf1
};
#define splatRec( ch )  ( wat->ptr[-1] = (ch) )

STATIC void parseType( void );

/*
    Routines to move through types buffer
*/
STATIC uint_8 get8( void ) {

    return( *(wat->ptr++) );
}

STATIC uint_16 get16( void ) {

    uint_16 word;

    word = ReadU16( wat->ptr );
    wat->ptr += 2;
    return( word );
}

STATIC uint_32 get32( void ) {

    uint_32 dword;

    dword = ReadU32( wat->ptr );
    wat->ptr += 4;
    return( dword );
}

STATIC uint_16 getIndex( void ) {

    uint_16 index;

    index = get8();
    if( index & 0x80 ) {
        index = ( ( index & 0x7f ) << 8 ) | get8();
    }

    return( index );
}

STATIC addr_handle getAddr32( void ) {

    addr_handle addr;

    if( parsingTypes ) {
        addr = CanACreateHdl( ddTypes, wat->ptr - ddTypes->data, 4 );
    } else {
        addr = CanACreateHdl( ddSymbols, wat->ptr - ddSymbols->data, 4 );
    }
    wat->ptr += 4;
    return( addr );
}

STATIC addr_handle getAddr48( void ) {

    addr_handle addr;

    if( parsingTypes ) {
        addr = CanACreateHdl( ddTypes, wat->ptr - ddTypes->data, 6 );
    } else {
        addr = CanACreateHdl( ddSymbols, wat->ptr - ddSymbols->data, 6 );
    }
    wat->ptr += 6;
    return( addr );
}

STATIC name_handle hdlName( void ) {

    unsigned    len;
    uint_8      *p;

    len = wat->rec_len - ( wat->ptr - wat->start_rec );
    if( len > 0 ) {
        p = wat->ptr;
        wat->ptr += len;
        return( NameAdd( (char *)p, len ) );
    } else {
        return( NAME_NULL );
    }
}

STATIC uint_8 hdlScope( void ) {

    unsigned    len;
    uint_8      *p;

    len = wat->rec_len - ( wat->ptr - wat->start_rec );
    if( len > 0 ) {
        p = wat->ptr;
        wat->ptr += len;
        if( memcmp( p, "struct", len ) == 0 ) {
            return( CANT_SCOPE_STRUCT );
        } else if( memcmp( p, "union", len ) == 0 ) {
            return( CANT_SCOPE_UNION );
        } else if( memcmp( p, "enum", len ) == 0 ) {
            return( CANT_SCOPE_ENUM );
        } else if( memcmp( p, "class", len ) == 0 ) {
            Fatal( MSG_CPP_EXTENSION );
        } else {
            p[len] = 0;
            Fatal( MSG_UNS_SCOPE, p );
        }
    }
    return( CANT_SCOPE_NULL );
}

/*
    Routines to manage the stack of buffer positions
*/
STATIC void pushPosn( void ) {

    struct pos *new;

    new = MemAlloc( sizeof( *new ) );
    *new = *wat;
    new->next = wat;
    wat = new;
}

STATIC void popPosn( void ) {

    struct pos *next;

    next = wat->next;
/**/myassert( next != NULL );
    MemFree( wat );
    wat = next;
}

/*
    Routines to move about in the buffer
*/
STATIC int countOrNot( uint_8 type_class ) {

/**/myassert( parsingTypes );
    switch( type_class & 0xf0 ) {
    case WAT_STRUCTURE:
    case WAT_ENUMERATED:
    case SPLAT:
        if( type_class & 0x0f ) {
            return( 0 );
        }
        break;
    }
    return( 1 );
}

STATIC void seekIndex( uint_16 index ) {

    uint_16     walk_index;
    uint_8      *walk_ptr;

/**/myassert( parsingTypes );
    if( wat->index > index ) {
        walk_ptr = ddTypes->data;
        walk_index = 1;
    } else {
        walk_ptr = wat->start_rec;
        walk_index = wat->index;
    }
    while( walk_index < index ) {
        if( walk_ptr[0] > 1 ) {
            walk_index += countOrNot( walk_ptr[1] );
        }
        walk_ptr += walk_ptr[0];
    }
    while( countOrNot( walk_ptr[1] ) == 0 ) {
        walk_ptr += walk_ptr[0];
    }
    wat->index = index;
    wat->ptr = walk_ptr;
}

STATIC uint_8 nextRec( void ) {

    for(;;) {
        wat->start_rec = wat->ptr;
        wat->rec_len = get8();
        if( wat->rec_len > 1 ) {
            return( get8() );
        }
/**/    never_reach();
    }
}

/*
    Routines to handle the mapping of a watcom index to a type_handle.
    mapIdx() takes care of forward references.
*/

STATIC void mapAdd( type_handle hdl ) {
/*
    add to map the mapping wat->index --> hdl
*/
    type_map_elm    *elm;

    elm = ArrNewElm( typeMap, wat->index );
    elm->hdl = hdl;
}

STATIC void initTypeMap( void ) {

    type_map_elm    *elm;

    typeMapDef.hdl = unDefdType;
    typeMap = ArrCreate( sizeof( type_map_elm ), &typeMapDef );
    elm = ArrNewElm( typeMap, 0 );
    elm->hdl = CANT_NULL;
}

STATIC type_handle mapIdx( uint_16 index ) {

    type_map_elm    *elm;
    type_handle     hdl;

    elm = ArrNewElm( typeMap, index );
    hdl = elm->hdl;
    if( hdl == unDefdType ) {       /* aha! forward reference!          */
/**/    myassert( parsingTypes );
        pushPosn();                 /* recursively take care of it      */
        seekIndex( index );
        parseType();
        popPosn();
        elm = ArrAccess( typeMap, index );
        hdl = elm->hdl;
/**/    myassert( hdl != unDefdType );
    }
    return( hdl );
}

STATIC void finiTypeMap( void ) {

    ArrDestroy( typeMap );
}


/*
    Routines for parsing Watcom Type information
*/

STATIC type_handle stbToCanT( uint_8 stb ) {
/*
    convert a scalar_type_byte to a scalar cant
*/
    bitsize     size;
    uint_8      class;
    type_handle hdl;
#define SS(x)   ((x)>>WAT_STB_CLASS_SHIFT)

    if( stbHdl[ stb ] != unDefdType ) {
        return( stbHdl[ stb ] );
    }

    class = SS( stb & WAT_STB_CLASS_FIELD );
/**/myassert( class <= 4 );
    size = ( ( stb & WAT_STB_SIZE_FIELD ) + 1 ) * 8;
    switch( class ) {
    case SS(WAT_STB_CLASS_SINT):    hdl = CanTInteger( size,1 );break;
    case SS(WAT_STB_CLASS_UINT):    hdl = CanTInteger( size,0 );break;
    case SS(WAT_STB_CLASS_FLOAT):   hdl = CanTReal( size );     break;
    case SS(WAT_STB_CLASS_VOID):    hdl = CanTVoid();           break;
    case SS(WAT_STB_CLASS_COMPLEX): hdl = CanTComplex( size );  break;
    }
    stbHdl[ stb ] = hdl;
    return( hdl );
#undef SS
}

STATIC void typeTypeName( uint_8 type_class ) {

    type_handle new_hdl;
    uint_8      scope;
    type_handle type_hdl;
    name_handle name_hdl;

    type_class &= 0x0f; /* we're interested in low nibble */
    switch( type_class ) {
    case WAT_TN_SCALAR:
        type_hdl = stbToCanT( get8() );
        name_hdl = hdlName();
        if( name_hdl == NAME_NULL ) {   /* no name for type */
            mapAdd( type_hdl );
        } else {
            mapAdd( CanTTypeDef( CANT_NULL, type_hdl, name_hdl ) );
        }
        break;
    case WAT_TN_SCOPE:
            /* FIXME this is a kludge!! */
        mapAdd( 0xff00 | hdlScope() );
        break;
    case WAT_TN_NAME:
        new_hdl = CanTReserve();
        mapAdd( new_hdl );
        scope = (uint_8)mapIdx( getIndex() );
        type_hdl = mapIdx( getIndex() );
        name_hdl = hdlName();
        CanTReUse( new_hdl );
        CanTTypeDef( type_hdl, name_hdl, scope );
        break;
    default:
/**/    never_reach();
    }
/**/myassert( endOfRecord() );
}

STATIC void typeArray( uint_8 type_class ) {

    type_handle new_hdl;
    type_handle index_type;
    type_handle base_type;
    type_handle lo_type;
    type_handle hi_type;
    addr_handle bounds;
    uint_32     high;

    new_hdl = CanTReserve();
    mapAdd( new_hdl );
    type_class &= 0x0f; /* we're interested in low nibble */
    switch( type_class ) {
    case WAT_AY_BYTE_INDEX:
        high = (uint_32)get8();
        base_type = mapIdx( getIndex() );
        CanTReUse( new_hdl );
        CanTArrayZ( base_type, high );
        break;
    case WAT_AY_WORD_INDEX:
        high = (uint_32)get16();
        base_type = mapIdx( getIndex() );
        CanTReUse( new_hdl );
        CanTArrayZ( base_type, high );                  /* 01-may-91 AFS */
        break;
    case WAT_AY_LONG_INDEX:
        high = get32();
        base_type = mapIdx( getIndex() );
        CanTReUse( new_hdl );
        CanTArrayZ( base_type, high );                  /* 01-may-91 AFS */
        break;
    case WAT_AY_TYPE_INDEX:
        index_type = mapIdx( getIndex() );
        base_type = mapIdx( getIndex() );
        CanTReUse( new_hdl );
        CanTArray( base_type, index_type );
        break;
    case WAT_AY_DESC_INDEX:
        lo_type = stbToCanT( get8() );
        hi_type = stbToCanT( get8() );
        bounds = getAddr32();
        base_type = mapIdx( getIndex() );
        CanTReUse( new_hdl );
        CanTArrayD( base_type, lo_type, hi_type, bounds );
        break;
    case WAT_AY_DESC_INDEX_386:
        lo_type = stbToCanT( get8() );
        hi_type = stbToCanT( get8() );
        bounds = getAddr48();
        base_type = mapIdx( getIndex() );
        CanTReUse( new_hdl );
        CanTArrayD( base_type, lo_type, hi_type, bounds );
        break;
    default:
/**/    never_reach();
    }
/**/myassert( endOfRecord() );
}

STATIC void typeSubRange( uint_8 type_class ) {

    uint_32     lo;
    uint_32     hi;
    type_handle base_type;
    type_handle new_hdl;

    switch( type_class & 0x0f ) {
    case WAT_SR_BYTE_RANGE:
        lo = get8();
        hi = get8();
        break;
    case WAT_SR_WORD_RANGE:
        lo = get16();
        hi = get16();
        break;
    case WAT_SR_LONG_RANGE:
        lo = get32();
        hi = get32();
        break;
    default:
/**/    never_reach();
    }
    new_hdl = CanTReserve();
    mapAdd( new_hdl );
    base_type = mapIdx( getIndex() );
    CanTReUse( new_hdl );
    CanTSubRange( base_type, lo, hi );
/**/myassert( endOfRecord() );
}

STATIC uint_8 ptrConvTab[] = {
    0,
    CANT_PTR_FAR,
    CANT_PTR_HUGE | CANT_PTR_FAR,
    CANT_PTR_DEREF,
    CANT_PTR_FAR | CANT_PTR_DEREF,
    CANT_PTR_HUGE | CANT_PTR_FAR | CANT_PTR_DEREF,
    CANT_PTR_386,
    CANT_PTR_386 | CANT_PTR_FAR,
    CANT_PTR_386 | CANT_PTR_DEREF,
    CANT_PTR_386 | CANT_PTR_DEREF | CANT_PTR_FAR
};

STATIC void typePointer( uint_8 type_class ) {

    type_handle base_type;
    type_handle new_hdl;

    type_class &= 0x0f;
/**/myassert( type_class < sizeof( ptrConvTab ) / sizeof( uint_8 ) );
    new_hdl = CanTReserve();
    mapAdd( new_hdl );
    base_type = mapIdx( getIndex() );
    CanTReUse( new_hdl );
    CanTPointer( base_type, ptrConvTab[ type_class ] );
/**/myassert( endOfRecord() );
}

STATIC void typeEnumerated( uint_8 type_class ) {

    uint_16     num_consts;
    type_handle base_type;
    enum_const  *enum_c;
    enum_const  *enum_cstop;
    cantype     *type;

/**/myassert( (type_class & 0x0f) == WAT_EN_LIST );
    num_consts = get16();
    base_type = stbToCanT( get8() );
    type = CanTEnum( base_type, num_consts );
    mapAdd( type->hdl );
    enum_c = type->d.enumr.consts;
    if( enum_c != NULL ) {
        enum_cstop = enum_c + num_consts;
        while( enum_c < enum_cstop ) {
            type_class = nextRec();
/**/        myassert( (type_class & 0xf0) == WAT_ENUMERATED );
            splatRec( SPLAT_DONT_COUNT );
            switch( type_class & 0x0f ) {
            case WAT_EN_CONST_BYTE: enum_c->value = get8();     break;
            case WAT_EN_CONST_WORD: enum_c->value = get16();    break;
            case WAT_EN_CONST_LONG: enum_c->value = get32();    break;
            default:
/**/            never_reach();
            }
            enum_c->name = hdlName();
/**/        myassert( endOfRecord() );
            ++enum_c;
        }
    }
    CanTGraph( type );
/**/myassert( endOfRecord() );
}

STATIC void typeStructure( uint_8 type_class ) {

    uint_16         num_fields;
    bitsize         bit_offset;
    uint_8          bit_size;
    cantype         *type;
    struct_field    *field;
    struct_field    *field_stop;

/**/myassert( ( type_class & 0x0f ) == WAT_ST_LIST );
    num_fields = get16();
    if( wat->rec_len > sizeof(unsigned_8) + sizeof(unsigned_16) ) {
        get32();        // skip the size field
    }
    type = CanTStruct( num_fields );
    mapAdd( type->hdl );
    field = type->d.strct.fields;
    if( field != NULL ) {
        field_stop = field + num_fields;
        while( field < field_stop ) {
            type_class = nextRec();
/**/        myassert( ( type_class & 0xf0 ) == WAT_STRUCTURE );
            splatRec( SPLAT_DONT_COUNT );
            switch( type_class & 0x0f ) {
            case WAT_ST_FIELD_BYTE:
            case WAT_ST_BIT_BYTE:
                bit_offset = get8();
                break;
            case WAT_ST_FIELD_WORD:
            case WAT_ST_BIT_WORD:
                bit_offset = get16();
                break;
            case WAT_ST_FIELD_LONG:
            case WAT_ST_BIT_LONG:
                bit_offset = get32();
                break;
            default:
                Fatal( MSG_CPP_EXTENSION );
            }
            bit_offset <<= 3;
            field->bitfield = 0;
            switch( type_class & 0x0f ) {
            case WAT_ST_FIELD_BYTE:
            case WAT_ST_FIELD_WORD:
            case WAT_ST_FIELD_LONG:
                field->type = mapIdx( getIndex() );
                break;
            case WAT_ST_BIT_BYTE:
            case WAT_ST_BIT_WORD:
            case WAT_ST_BIT_LONG:
                field->bitfield = 1;
                bit_offset += (bitsize)get8();
                bit_size = (bitsize)get8();
                field->type = CanTDupSize( mapIdx( getIndex() ), bit_size );
                break;
            default:
/**/            never_reach();
            }
            field->name = hdlName();
            field->bit_offset = bit_offset;
/**/        myassert( endOfRecord() );
            ++field;
        }
    }
    CanTGraph( type );
/**/myassert( endOfRecord() );
}

STATIC void typeProcedure( uint_8 type_class ) {

    type_handle ret_type;
    uint_8      num_parms;
    type_handle proc_type;
    uint_8      proc_class;
    cantype     *type;
    proc_parm   *parm;
    proc_parm   *parm_stop;
    type_handle parm_type;
    uint_16     ret_idx;

    proc_class = 0;
    switch( type_class & 0x0f ) {
    case WAT_PR_NEAR:   proc_class = 0;                         break;
    case WAT_PR_FAR:    proc_class = CANT_PROC_FAR;             break;
    case WAT_PR_NEAR386:proc_class = CANT_PROC_386;             break;
    case WAT_PR_FAR386: proc_class = CANT_PROC_FAR | CANT_PROC_386;break;
    default:
/**/    never_reach();
    }
    proc_type = CanTReserve();
    mapAdd( proc_type );
    ret_idx = getIndex();
    if( ret_idx == 0 ) {
            /* fortran does this! */
        ret_type = stbToCanT( WAT_STB_CLASS_VOID );
    } else {
        ret_type = mapIdx( ret_idx );
    }
    num_parms = get8();
    if( num_parms == 0 ) {
        CanTReUse( proc_type );
        type = CanTProcedure( ret_type, proc_class, num_parms );
    } else {
        parm_type = mapIdx( getIndex() );
        if( num_parms == 1 && ( CanTFind( parm_type ) )->class == CANT_VOID ) {
            /* function has one parameter -- a void parameter... */
            num_parms = 0;
            CanTReUse( proc_type );
            type = CanTProcedure( ret_type, proc_class, num_parms );
        } else {
            CanTReUse( proc_type );
            type = CanTProcedure( ret_type, proc_class, num_parms );
            parm = type->d.proc.parms;
            parm_stop = parm + num_parms;
            for(;;) {
                parm->type = parm_type;
                ++parm;
                if( parm == parm_stop ) break;
                if( endOfRecord() ) {
                    /* too many parameters to fit in one record
                       -- next will be an extended parms record */
                    nextRec();
                }
                parm_type = mapIdx( getIndex() );
            }
        }
    }
    CanTGraph( type );
/**/myassert( endOfRecord() );
}

STATIC void typeCharacterBlock( uint_8 type_class ) {

    uint_32     length_val;
    type_handle length_type;
    addr_handle length_addr;

    switch( type_class & 0x0f ) {
    case WAT_CB_CHAR_BYTE:
        length_val = get8();
        mapAdd( CanTCharB( length_val ) );
        break;
    case WAT_CB_CHAR_WORD:
        length_val = get16();
        mapAdd( CanTCharB( length_val ) );
        break;
    case WAT_CB_CHAR_LONG:
        length_val = get32();
        mapAdd( CanTCharB( length_val ) );
        break;
    case WAT_CB_CHAR_IND:
        length_type = stbToCanT( get8() );
        length_addr = getAddr32();
        mapAdd( CanTCharBI( length_type, length_addr ) );
        break;
    case WAT_CB_CHAR_IND_386:
        length_type = stbToCanT( get8() );
        length_addr = getAddr48();
        mapAdd( CanTCharBI( length_type, length_addr ) );
        break;
    default:
/**/    never_reach();
    }
/**/myassert( endOfRecord() );
}

STATIC void parseType( void ) {

    uint_8      type_class;

    type_class = nextRec();
    if( (type_class & 0xf0) == SPLAT ) {
        wat->ptr += wat->rec_len - 2;
        if( ( type_class & 0x0f ) == 0 ) {
            ++wat->index;               /* we count this splat record */
        }
    } else {
        splatRec( SPLAT_COUNT );
        switch( type_class & 0xf0 ) {
        case WAT_TYPE_NAME:     typeTypeName( type_class );         break;
        case WAT_ARRAY:         typeArray( type_class );            break;
        case WAT_SUBRANGE:      typeSubRange( type_class );         break;
        case WAT_POINTER:       typePointer( type_class );          break;
        case WAT_ENUMERATED:    typeEnumerated( type_class );       break;
        case WAT_STRUCTURE:     typeStructure( type_class );        break;
        case WAT_PROCEDURE:     typeProcedure( type_class );        break;
        case WAT_CHARACTER_BLOCK:typeCharacterBlock( type_class );  break;
        default:
/**/        never_reach();
        }
        ++wat->index;
    }
/**/myassert( endOfRecord() );
}


/*
    Routines for processing Watcom symbolic information

    We use the extra field in the cansymb records for BLOCKs to indicate
    the offset from the start of the code segment of the block.
*/
STATIC symb_handle offMapCur( void ) {

    symb_handle hdl;
    offset_map  *om;

/**/myassert( !parsingTypes );
    hdl = CanSNew( CANS_BLOCK );
    hdl->d.block.end = CANS_NULL;       /* for recursion protection */
    om = MemAlloc( sizeof( *om ) );
    om->next = offsetMap;
    offsetMap = om;
    om->offset = wat->start_rec - ddSymbols->data;
    om->hdl = hdl;
    return( hdl );
}

FORWARD STATIC symb_handle symbCode( uint_8 symb_class );

STATIC symb_handle offMap( uint_16 offset ) {

    offset_map  *om;
    uint_8      symb_class;
    symb_handle ret_hdl;

/**/myassert( !parsingTypes );
    om = offsetMap;
    while( om ) {
        if( om->offset == offset ) {
            return( om->hdl );
        }
        om = om->next;
    }
    /* hasn't been defined yet */
    pushPosn();
    wat->ptr = wat->start_rec = ddSymbols->data + offset;
    wat->rec_len = 0;
    symb_class = nextRec();
    splatRec( SPLAT_COUNT );
/**/myassert( ( symb_class & 0xf0 ) == WAT_CODE );
    ret_hdl = symbCode( symb_class );
    popPosn();
    return( ret_hdl );
}

STATIC void destroyOffsetMap( void ) {

    offset_map  *cur;
    offset_map  *next;

    cur = offsetMap;
    while( cur ) {
        next = cur->next;
        MemFree( cur );
        cur = next;
    }
}

STATIC const register_type watRegMap[] = {
/*WAT_REG_AL    */  CANS_REG_AL,
/*WAT_REG_AH    */  CANS_REG_AH,
/*WAT_REG_BL    */  CANS_REG_BL,
/*WAT_REG_BH    */  CANS_REG_BH,
/*WAT_REG_CL    */  CANS_REG_CL,
/*WAT_REG_CH    */  CANS_REG_CH,
/*WAT_REG_DL    */  CANS_REG_DL,
/*WAT_REG_DH    */  CANS_REG_DH,
/*WAT_REG_AX    */  CANS_REG_AX,
/*WAT_REG_BX    */  CANS_REG_BX,
/*WAT_REG_CX    */  CANS_REG_CX,
/*WAT_REG_DX    */  CANS_REG_DX,
/*WAT_REG_SI    */  CANS_REG_SI,
/*WAT_REG_DI    */  CANS_REG_DI,
/*WAT_REG_BP    */  CANS_REG_BP,
/*WAT_REG_SP    */  CANS_REG_SP,
/*WAT_REG_CS    */  CANS_REG_CS,
/*WAT_REG_SS    */  CANS_REG_SS,
/*WAT_REG_DS    */  CANS_REG_DS,
/*WAT_REG_ES    */  CANS_REG_ES,
/*WAT_REG_ST0   */  CANS_REG_ST0,
/*WAT_REG_ST1   */  CANS_REG_ST1,
/*WAT_REG_ST2   */  CANS_REG_ST2,
/*WAT_REG_ST3   */  CANS_REG_ST3,
/*WAT_REG_ST4   */  CANS_REG_ST4,
/*WAT_REG_ST5   */  CANS_REG_ST5,
/*WAT_REG_ST6   */  CANS_REG_ST6,
/*WAT_REG_ST7   */  CANS_REG_ST7,
/*WAT_REG_EAX   */  CANS_REG_EAX,
/*WAT_REG_EBX   */  CANS_REG_EBX,
/*WAT_REG_ECX   */  CANS_REG_ECX,
/*WAT_REG_EDX   */  CANS_REG_EDX,
/*WAT_REG_ESI   */  CANS_REG_ESI,
/*WAT_REG_EDI   */  CANS_REG_EDI,
/*WAT_REG_EBP   */  CANS_REG_EBP,
/*WAT_REG_ESP   */  CANS_REG_ESP,
/*WAT_REG_FS    */  CANS_REG_FS,
/*WAT_REG_GS    */  CANS_REG_GS
};

STATIC register_type watReg2CanReg( uint_8 wat ) {

/**/myassert( wat <= WAT_REG_GS );
    return( watRegMap[ wat ] );
}

STATIC symb_handle hdlLocation( void ) {

    symb_handle hdl;
    uint_8      loc_class;
    int_32      offset;
    addr_handle addr_hdl;
    int         i;

    loc_class = get8();
    if( loc_class == 0 ) {
        return( CANS_NULL );
    }
    switch( loc_class & 0xf0 ) {
    case WAT_BP_OFFSET:
        switch( loc_class & 0x0f ) {
        case WAT_BP_BYTE:   offset = (int_8)get8();     break;
        case WAT_BP_WORD:   offset = (int_16)get16();   break;
        case WAT_BP_DWORD:  offset = (int_32)get32();   break;
        default:
/**/        never_reach();
        }
        hdl = CanSNew( CANS_BP_OFFSET );
        hdl->d.bpoff.offset = offset;
        break;

    case WAT_MEMORY:
        switch( loc_class & 0x0f ) {
        case WAT_MEM_ADDR_286:  addr_hdl = getAddr32(); break;
        case WAT_MEM_ADDR_386:  addr_hdl = getAddr48(); break;
        default:
            Fatal( MSG_CPP_EXTENSION );
        }
        hdl = CanSNew( CANS_MEM_LOC );
        hdl->d.memloc.mem_hdl = addr_hdl;
        hdl->d.memloc.is_static = 1;
        break;

    case WAT_MULTI_REG:
        loc_class &= 0x0f;
        ++loc_class;
        if( loc_class > CANS_MAX_REGISTERS ) {
            Fatal( MSG_TOO_MANY_REGS );
        }
        hdl = CanSNew( CANS_REGISTER );
        hdl->d.reg.num_regs = loc_class;
        for( i=0; i < loc_class; ++i ) {
            hdl->d.reg.reg[ i ] = watReg2CanReg( get8() );
        }
        break;

    case WAT_REG:
        hdl = CanSNew( CANS_REGISTER );
        hdl->d.reg.num_regs = 1;
        hdl->d.reg.reg[ 0 ] = watReg2CanReg( loc_class & 0x0f );
        break;

    case WAT_IND_REG:
        hdl = CanSNew( CANS_IND_REG );
        hdl->d.indr.off = watReg2CanReg( get8() );
        switch( loc_class & 0x0f ) {
        case WAT_IR_CALLOC_NEAR:
            hdl->d.indr.calloc = 1;
            hdl->d.indr.far_ptr = 0;
            break;
        case WAT_IR_CALLOC_FAR:
            hdl->d.indr.calloc = 1;
            hdl->d.indr.far_ptr = 0;
            hdl->d.indr.seg = watReg2CanReg( get8() );
            break;
        case WAT_IR_RALLOC_NEAR:
            hdl->d.indr.calloc = 0;
            hdl->d.indr.far_ptr = 0;
            break;
        case WAT_IR_RALLOC_FAR:
            hdl->d.indr.calloc = 0;
            hdl->d.indr.far_ptr = 1;
            hdl->d.indr.seg = watReg2CanReg( get8() );
            break;
        default:
/**/        never_reach();
        }
        break;

    default:
        Fatal( MSG_CPP_EXTENSION );
    }
    return( hdl );
}

STATIC void symbOneVariable( uint_8 symb_class, name_handle *hdl ) {

    symb_handle var_hdl;

    switch( symb_class & 0x0f ) {
    case WAT_VAR_MODULE:
        var_hdl = CanSNew( CANS_MEM_LOC );
        var_hdl->d.memloc.mem_hdl = getAddr32();
        var_hdl->d.memloc.is_static = 1;
        var_hdl->d.nat.type_hdl = mapIdx( getIndex() );
        *hdl = hdlName();
        var_hdl->d.nat.name_hdl = *hdl;
        headSymb = CanSIBefore( headSymb, var_hdl );
        break;

    case WAT_VAR_LOCAL:
/**/    never_reach();
        break;

    case WAT_VAR_MODULE_386:
        var_hdl = CanSNew( CANS_MEM_LOC );
        var_hdl->d.memloc.mem_hdl = getAddr48();
        var_hdl->d.memloc.is_static = 1;
        var_hdl->d.nat.type_hdl = mapIdx( getIndex() );
        *hdl = hdlName();
        var_hdl->d.nat.name_hdl = *hdl;
        headSymb = CanSIBefore( headSymb, var_hdl );
        break;

    default:
        Fatal( MSG_CPP_EXTENSION );
    }
/**/myassert( endOfRecord() );
}

STATIC void symbVariable( uint_8 symb_class ) {

    name_handle garbage;

    symbOneVariable( symb_class, &garbage );
}

STATIC int symbOneLocal( symb_handle block, name_handle *hdl ) {
/*
    Assumes that block has no BLOCK_END or EPILOG yet
*/
    uint_8  symb_class;

/**/myassert( block != NULL );
    if( wat->ptr == wat->finish ) {
        return( 0 );
    }
    symb_class = nextRec();
    switch( symb_class & 0xf0 ) {
    case WAT_VARIABLE:
        splatRec( SPLAT_COUNT );
        if( ( symb_class & 0x0f ) == WAT_VAR_LOCAL ) {
            symb_handle var_hdl;
            var_hdl = hdlLocation();
            var_hdl->d.nat.type_hdl = mapIdx( getIndex() );
            *hdl = hdlName();
            var_hdl->d.nat.name_hdl = *hdl;
            CanSIBefore( block, var_hdl );
        } else {
            symbOneVariable( symb_class, hdl );
        }
/**/    myassert( endOfRecord() );
        return( 1 );
        break;
    case WAT_CODE:
    case WAT_NEW_BASE:
    case SPLAT:
        return( 0 );
        break;
    default:
/**/    never_reach();
        return( 0 );
    }
}

STATIC void symbParseLocals( symb_handle block, uint_8 num_parms ) {
/*
    We need to give names to the parameters to a function.  Unfortunately,
    there is no direct provision for this in the WATCOM debugging info.
    The following assumption is made:

        name of parm 0 is name of last local
        name of parm 1 is name of 2nd last local
        ...
        name of parm num_parms-1 is name of (last-num_parms+1)st local

    We'll keep track of num_parms locals in a vector.  When done, we'll
    flip the vector a few times and assign to the parms.
*/

    name_handle *name_vec;
    uint_8      cur_off;
    symb_handle walk;
    uint_8      walk_off;

/**/myassert( block != NULL && num_parms > 0 );
    name_vec = alloca( num_parms * sizeof( name_handle ) );
    for( cur_off = 0; cur_off < num_parms; ++cur_off ) {
        name_vec[ cur_off ] = NAME_NULL;
    }
    cur_off = 0;
    for(;;) {
        if( symbOneLocal( block, name_vec + cur_off ) == 0 ) break;
        ++cur_off;
        if( cur_off == num_parms ) {
            cur_off = 0;
        }
    }
    /* name_vec[ cur_off ] is the name of the last parm */
    /* name_vec[ ( cur_off + 1 ) % num_parms ] is the name of the 2nd last... */
    walk = CanSFwd( block );
    walk_off = 0;
    do {
        if( cur_off == 0 ) {
            cur_off = num_parms;
        }
        --cur_off;
/**/    myassert(   walk->class == CANS_MEM_LOC ||
                    walk->class == CANS_BP_OFFSET ||
                    walk->class == CANS_REGISTER );
        walk->d.nat.name_hdl = name_vec[ cur_off ];
        walk = CanSFwd( walk );
        ++walk_off;
    } while( walk_off < num_parms );
}


STATIC symb_handle symbCode( uint_8 symb_class ) {

    symb_handle     block;          /* the block we are creating            */
    uint_16         parent_offset;  /* offset of parent block in ddSymbols  */
    symb_handle     parent_hdl;     /* handle for parent block              */
    symb_handle     work_hdl;       /* a temporary working handle           */
    symb_handle     prolog_hdl;     /* handle for the prolog/ret_val        */
    uint_8          epilog_size;    /* size of the epilog                   */
    uint_8          num_parms;      /* number of parameters in registers    */
    unsigned        i;              /* general counter                      */
    int             is32rec;        /* is this one of the 32bit records?    */
    symb_handle     parm_hdl;
    cantype         *blk_type;
    proc_parm       *parm_list;     /* blk_type->d.proc.parms               */
    cantype         *parm_type;
    int_32          bp_offset;

    block = offMapCur();
    symb_class &= 0x0f;
    switch( symb_class ) {
    case WAT_CODE_BLOCK:
    case WAT_CODE_NEAR_RTN:
    case WAT_CODE_FAR_RTN:
        is32rec = 0;
        block->extra = get16();
        block->d.block.size = get16();
        break;
    case WAT_CODE_BLOCK_386:
    case WAT_CODE_NEAR_RTN_386:
    case WAT_CODE_FAR_RTN_386:
        is32rec = 1;
        block->extra = get32();
        block->d.block.size = get32();
        break;
    default:
        Fatal( MSG_CPP_EXTENSION );
    }
    parent_offset = get16();
    if( parent_offset != 0 ) {
        parent_hdl = offMap( parent_offset );
        block->d.block.start_offset = block->extra - parent_hdl->extra;
    } else {
        parent_hdl = CANS_NULL;
        block->d.block.start_offset = block->extra;
    }

    prolog_hdl = CanSNew( CANS_PROLOG );
    switch( symb_class ) {
    case WAT_CODE_BLOCK:
    case WAT_CODE_BLOCK_386:
        num_parms = 0;
        prolog_hdl->d.prolog.size = 0;
        prolog_hdl->d.prolog.has_ret_val = 0;
        prolog_hdl->d.prolog.has_ret_addr = 0;
        prolog_hdl->d.prolog.ret_addr_offset = 0;
        CanSIAfter( block, prolog_hdl );
        epilog_size = 0;
        break;
    case WAT_CODE_NEAR_RTN:
    case WAT_CODE_FAR_RTN:
    case WAT_CODE_NEAR_RTN_386:
    case WAT_CODE_FAR_RTN_386:
        prolog_hdl->d.prolog.size = get8();
        epilog_size = get8();               /* save for later */
        prolog_hdl->d.prolog.has_ret_addr = 1;
        if( is32rec ) {
            bp_offset = get32();
        } else {
            bp_offset = (int_32)get16();
        }
        prolog_hdl->d.prolog.ret_addr_offset = bp_offset;
        block->d.nat.type_hdl = mapIdx( getIndex() );
        blk_type = CanTFind( block->d.nat.type_hdl );
/**/    myassert( blk_type == NULL || blk_type->class == CANT_PROCEDURE );
        work_hdl = hdlLocation();
        if( work_hdl != CANS_NULL ) {
            CanSIAfter( prolog_hdl, work_hdl );
            prolog_hdl->d.prolog.has_ret_val = 1;
        } else {
            prolog_hdl->d.prolog.has_ret_val = 0;
        }
        num_parms = get8();
/**/    myassert( blk_type == NULL ||
            num_parms <= blk_type->d.proc.num_parms );
        if( blk_type != NULL ) {
            parm_list = blk_type->d.proc.parms;
        } else {
            parm_list = NULL;
        }
        for( i = 0; i < num_parms; ++i ) {
            parm_hdl = hdlLocation();
            CanSIBefore( block, parm_hdl );
            if( parm_list != NULL ) {
                parm_hdl->d.nat.type_hdl = parm_list[ i ].type;
            }
        }
        /* note we need the value of i from above loop inside this if() {} */
        num_parms = blk_type->d.proc.num_parms;
        if( i < num_parms && blk_type != NULL ) {
            /*
               Now we have to build the stack-based parms, bp_offset was set
               to the bp offset of the return address above.  We add 2, 4, or 6
               depending on near/far/386/386far
            */
            switch( symb_class ) {      /* note the major fall through! */
            case WAT_CODE_FAR_RTN_386:
                bp_offset += 2;
            case WAT_CODE_NEAR_RTN_386:
            case WAT_CODE_FAR_RTN:
                bp_offset += 2;
            case WAT_CODE_NEAR_RTN:
                bp_offset += 2;
            }
/**/        myassert( parm_list == blk_type->d.proc.parms );
            for(;;) {
                parm_hdl = CanSNew( CANS_BP_OFFSET );
                CanSIBefore( block, parm_hdl );
                parm_hdl->d.bpoff.offset = bp_offset;
                parm_hdl->d.nat.type_hdl = parm_list[ i ].type;
                parm_type = CanTFind( parm_hdl->d.nat.type_hdl );
                ++i;
                if( i == num_parms ) break;
                if( is32rec ) { /* determine positioning of next arg */
                    bp_offset += ( ( parm_type->size + 31 ) &~(int_32)31 ) / 8;
                } else {
                    bp_offset += ( ( parm_type->size + 15 ) &~(int_32)15 ) / 8;
                }
            }
        }
        block->d.nat.name_hdl = hdlName();
        CanSIBefore( block, prolog_hdl );   /* add the PROLOG */
        break;
    }

/**/myassert( endOfRecord() );
    if( num_parms > 0 ) {
        symbParseLocals( block, num_parms );
    } else {
        name_handle garbage;

        while( symbOneLocal( block, &garbage ) ) {
            /* nop */
        }
    }
    wat->ptr = wat->start_rec;
    wat->rec_len = 0;
/**/myassert( endOfRecord() );

    work_hdl = CanSNew( CANS_EPILOG );      /* add the EPILOG */
    work_hdl->d.epilog.size = epilog_size;
    CanSIBefore( block, work_hdl );
    work_hdl = CanSNew( CANS_BLOCK_END );   /* add the BLOCK_END */
    work_hdl->d.end.start = block;
    block->d.block.end = work_hdl;
    CanSIBefore( block, work_hdl );         /* complete the block */

        /* attach to parent or to headSymb as appropriate */
    if( parent_hdl != CANS_NULL ) {
/**/    myassert( parent_hdl->class == CANS_BLOCK );
        work_hdl = parent_hdl->d.block.end;
/**/    myassert( work_hdl != NULL );
/**/    myassert( work_hdl->class == CANS_BLOCK_END );
        work_hdl = CanSBwd( work_hdl );
/**/    myassert( work_hdl->class == CANS_EPILOG );
        CanSIBefore( work_hdl, block );
    } else {
        headSymb = CanSIBefore( headSymb, block );
    }

    return( block );
}

STATIC void symbNewBase( uint_8 symb_class ) {

    symb_handle seg_hdl;

    seg_hdl = CanSNew( CANS_CHANGE_SEG );
    switch( symb_class & 0x0f ) {
    case WAT_NB_ADD_PREV_SEG:
        Fatal( MSG_ADD_PREV_SEG );
    case WAT_NB_SET_BASE:
        seg_hdl->d.cseg.seg = getAddr32();
        break;
    case WAT_NB_SET_BASE_386:
        seg_hdl->d.cseg.seg = getAddr48();
        break;
    default:
/**/    never_reach();
    }
    headSymb = CanSIBefore( headSymb, seg_hdl );
}

STATIC void parseSymbol( void ) {

    uint_8  symb_class;

    symb_class = nextRec();
    splatRec( SPLAT_COUNT );
    switch( symb_class & 0xf0 ) {
    case WAT_VARIABLE:  symbVariable( symb_class );         break;
    case WAT_CODE:      symbCode( symb_class );             break;
    case WAT_NEW_BASE:  symbNewBase( symb_class );          break;
    case SPLAT:         wat->ptr += wat->rec_len - 2;       break;
    default:
/**/    never_reach();
    }
/**/myassert( endOfRecord() );
}

void Wat2CanTandS( seghdr *dd_types, seghdr *dd_symbols ) {
/*******************************************************/

    int         index;
    uint_8      *finish;

    /* setup */
    ddTypes = dd_types;
    ddSymbols = dd_symbols;
    headSymb = CANS_NULL;
    unDefdType = CanTReserve();
    stbHdl = MemAlloc( 128 * sizeof( type_handle ) );
    for( index = 0; index < 128; ++index ) {
        stbHdl[ index ] = unDefdType;
    }
    initTypeMap();
    wat = MemAlloc( sizeof( *wat ) );
    offsetMap = NULL;

    if( dd_types != NULL ) {
        /* parse types */
        parsingTypes = 1;
        wat->next = NULL;
        wat->index = 1;
        wat->start_rec = wat->ptr = ddTypes->data;
        wat->rec_len = 0;
        finish = ddTypes->data + ddTypes->alloc;
        wat->finish = finish;
#ifndef NDEBUG
        {
            int fh;
            fh = open( "debug.seg", O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, S_IWRITE|S_IREAD );
            if( fh != -1 ) {
                write( fh, ddTypes->data, ddTypes->alloc );
                close( fh );
            }
        }
#endif
        while( wat->ptr < finish ) {
            parseType();
        }
/**/    myassert( wat->next == NULL );
        SegDetachData( ddTypes );
        CanTMunge();
        CanMisc.types_present = 1;
    }
    MemFree( stbHdl );

    if( dd_symbols != NULL ) {
        /* parse symbols */
        parsingTypes = 0;
        wat->next = NULL;
        wat->index = 1;
        wat->start_rec = wat->ptr = ddSymbols->data;
        wat->rec_len = 0;
        finish = ddSymbols->data + ddSymbols->alloc;
        wat->finish = finish;
#ifndef NDEBUG
        {
            int fh;
            fh = open( "symbols.seg", O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, S_IWRITE|S_IREAD );
            if( fh != -1 ) {
                write( fh, ddSymbols->data, ddSymbols->alloc );
                close( fh );
            }
        }
#endif
        while( wat->ptr < finish ) {
            parseSymbol();
        }
/**/    myassert( wat->next == NULL );
        SegDetachData( ddSymbols );
        CanSSetHead( headSymb );
        CanMisc.symbs_present = 1;
    }

    /* cleanup */
    destroyOffsetMap();
    MemFree( wat );
    finiTypeMap();
    CanTReUse( unDefdType );
}

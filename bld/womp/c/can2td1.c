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
* Description:  Turbo Debugger style output.
*
****************************************************************************/


#include <string.h>
#include "womp.h"
#include "turbodbg.h"
#include "genutil.h"
#include "memutil.h"
#include "myassert.h"
#include "objrec.h"
#include "queue.h"
#include "carve.h"
#include "objprs.h"
#include "canmisc.h"

#define BITS_TO_BYTES( b )  ( ( (b) + 7 ) / 8 )

STATIC name_handle  realHdl;
STATIC name_handle  imagHdl;

/*
    Functions for handling the writing of typing information
*/

typedef uint_16     idx_t;

typedef struct type_rec type_rec;
struct type_rec {
    type_rec    *next;
    type_rec    *member;    /* for type_recs with members */
    idx_t       idx;        /* index of this type */
    uint_16     alloc;      /* allocated length */
    uint_16     len;        /* written to length */
    uint_8      *data;      /* pointer to data */
};
#define TYPE_REC_INC    128

STATIC carve_t  typeRecCarver;  /* we carve type_recs from here */
STATIC idx_t    nextIdx;        /* next available index */

STATIC int typePass2( void *type, void *force );

STATIC uint_8 *getTrData( type_rec *tr, uint_16 len ) {

    uint_16         new_len;
    uint_8          *p;

    new_len = tr->len + len;
    if( new_len > tr->alloc ) {
        tr->alloc = new_len + TYPE_REC_INC;
        tr->data = MemRealloc( tr->data, new_len + TYPE_REC_INC );
    }
    p = tr->data + tr->len;
    tr->len = new_len;
    return( p );
}

STATIC void put8( type_rec *tr, uint_8 byte ) {

    uint_8  *p;

    p = getTrData( tr, 1 );
    p[0] = byte;
}

STATIC void put16( type_rec *tr, uint_16 word ) {

    uint_8  *p;

    p = getTrData( tr, 2 );
    WriteU16( p, word );
}

STATIC void put32( type_rec *tr, uint_32 dword ) {

    uint_8  *p;

    p = getTrData( tr, 4 );
    WriteU32( p, dword );
}

STATIC void putIndex( type_rec *tr, uint_16 word ) {

    uint_8  *p;

    if( word > 0x7f ) {
        p = getTrData( tr, 2 );
        p[0] = 0x80 | ( word >> 8 );
        p[1] = word & 0xff;
    } else {
        p = getTrData( tr, 1 );
        p[0] = word;
    }
}

STATIC void putName( type_rec *tr, name_handle name_hdl ) {

    const char  *name;
    uint_8      *p;
    uint_16     len;

    name = NameGet( name_hdl );
    if( name == NULL ) {
        len = 0;
    } else {
        len = strlen( name );
    }
    p = getTrData( tr, len + 1 );
    p[0] = len;
    memcpy( p + 1, name, len );
}

STATIC type_rec *newRec( void ) {

    type_rec    *new;

    new = CarveAlloc( typeRecCarver );
    new->alloc = 0;
    new->len = 0;
    new->data = NULL;
    return( new );
}

STATIC type_rec *newType( name_handle name, bitsize size, uint_8 type_id ) {

    type_rec    *new;
    idx_t       idx;
    type_rec    *member;

    if( nextIdx < 24 ) {
        Fatal( MSG_TOO_MANY_TYPES );
    }
    new = newRec();
    idx = nextIdx;
    new->idx = idx;
    putIndex( new, idx );
    putName( new, name );
    put16( new, (uint_16)BITS_TO_BYTES( size ) );
    put8( new, type_id );
    ++nextIdx;
    switch( type_id ) {
    case TD_ID_SCHAR:
    case TD_ID_SINT:
    case TD_ID_SLONG:
    case TD_ID_UCHAR:
    case TD_ID_UINT:
    case TD_ID_ULONG:
    case TD_ID_PCHAR:
    case TD_ID_PARRAY:
    case TD_ID_VLSTRUCT:
    case TD_ID_VLUNION:
    case TD_ID_ENUM:
        ++nextIdx;  /* skip an index */
        break;
    }
    switch( type_id ) {
    case TD_ID_STRUCT:
    case TD_ID_UNION:
    case TD_ID_VLSTRUCT:
    case TD_ID_VLUNION:
        member = newRec();
        member->idx = TD_CMT_STRUCT_DEFN;
        break;
    case TD_ID_ENUM:
        member = newRec();
        member->idx = TD_CMT_ENUM_DEFN;
        break;
    default:
        member = NULL;
        break;
    }
    new->member = member;
    return( new );
}

STATIC void createComent( type_rec *rec, uint_8 class ) {

    obj_rec *coment;
    uint_16 len;

    coment = Can2TDNewRec( class, 0 );
    len = rec->len;
/**/myassert( len > 0 );
    ObjAttachData( coment, MemRealloc( rec->data, len ), len );
    ObjCanFree( coment );
    ObjRSeek( coment, 0 );      /* FIXME should be able to call Can2TDEndRec */
    CarveFree( typeRecCarver, rec );
}

STATIC void endType( type_rec *tr ) {

    type_rec    *member;

    member = tr->member;
    if( member != NULL ) {
        createComent( member, member->idx );
    }
    createComent( tr, TD_CMT_TYPE_DEFN );
}


/*
    Here comes the type conversion code
*/

STATIC type_rec *newLarge( name_handle name, bitsize size,
    uint_8 small, uint_8 large ) {

    type_rec    *tr;

    if( size >= 0x10000UL * 8UL ) {
        tr = newType( name, size, large );
        put16( tr, (uint_32)BITS_TO_BYTES( size ) >> 16 );
    } else {
        tr = newType( name, size, small );
    }
    return( tr );
}

STATIC idx_t buildArray( bitsize size ) {
/*
    For types which we can't convert we create a C array of uint_8's of
    the appropriate size.
*/
    type_rec    *tr;
    idx_t       idx;

    tr = newLarge( NAME_NULL, size, TD_ID_CARRAY, TD_ID_VLARRAY );
    putIndex( tr, TD_TYPE_UNSIGNED_8 );
    idx = tr->idx;
    endType( tr );
    return( idx );
}

STATIC void typ1Integer( cantype *type ) {
/*
    Set the extra field to the appropriate Turbo reserved type, or to a type
    which we create now
*/
    int         is_signed;
    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_INTEGER );
    is_signed = type->sgned;
    if( type->size <= 32 ) {
        if( type->size <= 8 ) {
            type->extra = TD_TYPE_SIGNED_8;
        } else if( type->size <= 16 ) {
            type->extra = TD_TYPE_SIGNED_16;
        } else {
            type->extra = TD_TYPE_SIGNED_32;
        }
        if( ! is_signed ) {
            type->extra += ( TD_TYPE_UNSIGNED_8 - TD_TYPE_SIGNED_8 );
        }
    } else if( type->size == 64 ) {
        tr = newType( NAME_NULL, 1, is_signed ? TD_ID_SQUAD : TD_ID_UQUAD );
        type->extra = tr->idx;
        endType( tr );
    } else {
        PrtMsg( WRN|MSG_UNS_SCALAR_SIZE );
        type->extra = buildArray( type->size );
    }
}

STATIC void typ1Real( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_REAL );
    switch( type->size ) {
    case 32:    type->extra = TD_TYPE_REAL_32;      break;
    case 64:    type->extra = TD_TYPE_REAL_64;      break;
    case 80:    type->extra = TD_TYPE_REAL_80;      break;
    case 48:    type->extra = TD_TYPE_REAL_48;      break;
    default:
        PrtMsg( WRN|MSG_UNS_SCALAR_SIZE );
        type->extra = buildArray( type->size );
        break;
    }
}

STATIC void typ1Void( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_VOID );
    type->extra = TD_TYPE_VOID;
}

STATIC void typ1Complex( cantype *type ) {

    type_rec    *tr;
    type_rec    *member;
    idx_t       member_type;

/**/myassert( type != NULL && type->class == CANT_COMPLEX );
    /*
        Since turbo doesn't have a complex type, we'll create a struct
        with 2 floats named "real" and "imag".
    */
    switch( type->size ) {
    case  64:   member_type = TD_TYPE_REAL_32;      break;
    case 128:   member_type = TD_TYPE_REAL_64;      break;
    case 160:   member_type = TD_TYPE_REAL_80;      break;
    case  96:   member_type = TD_TYPE_REAL_48;      break;
    default:
        PrtMsg( WRN|MSG_UNS_SCALAR_SIZE );
        member_type = buildArray( type->size / 2 );
        break;
    }
    tr = newLarge( NAME_NULL, type->size, TD_ID_STRUCT, TD_ID_VLSTRUCT );
    type->extra = tr->idx;
    member = tr->member;
    put8( member, 0 );
    putName( member, realHdl );
    putIndex( member, member_type );
    put8( member, TD_STRUCT_LAST_MEMBER );
    putName( member, imagHdl );
    putIndex( member, member_type );
    endType( tr );
}

STATIC int typePass1( void *_type, void *parm ) {

    cantype *type = _type;
/**/myassert( type != NULL && parm == NULL );
    parm = parm;
    switch( type->class ) {
    case CANT_INTEGER:      typ1Integer( type );    break;
    case CANT_REAL:         typ1Real( type );       break;
    case CANT_VOID:         typ1Void( type );       break;
    case CANT_COMPLEX:      typ1Complex( type );    break;
    default:
        type->extra = 0;
    }
    return( 0 );
}

/*
    Pass 2
*/

STATIC idx_t resolveType( type_handle hdl ) {

    cantype *type;

    type = CanTFind( hdl );
/**/myassert( type != NULL );
    if( type->extra == 0 ) {
        int force;

        while( type->class == CANT_TYPEDEF &&
            type->d.typdef.scope == CANT_SCOPE_NULL ) {
            type = CanTFind( type->d.typdef.type );
        }
        force = 1;
        typePass2( type, &force );
    }
/**/myassert( type->extra != 0 );
    return( type->extra );
}

STATIC void doStruct( cantype *type, type_rec *tr ) {
/*
    Assumes that caller has done tr = newType( name_hdl, size, TD_ID_STRUCT )
    but has not written any type info into the new type yet.  Does an
    endType( tr ).
*/
    struct_field    *field;
    struct_field    *field_stop;
    type_rec        *member;
    uint_8          bits;
    uint_32         offset;
    uint_8          bit_start;
    uint_32         new_offset;
    uint_8          new_bit_start;
    cantype         *member_type;

/**/myassert( type != NULL && type->class == CANT_STRUCT );

    /* write the field types and name/offset lists */
    member = tr->member;
    offset = 0;
    bit_start = 0;
    field = type->d.strct.fields;
    if( field != NULL ) {
        field_stop = field + type->d.strct.num_fields;
        while( field < field_stop ) {
            member_type = CanTFind( field->type );
/**/        myassert( member_type != NULL );
            new_offset = field->bit_offset >> 3;
            new_bit_start = field->bit_offset & 7;
            if( member_type->size >= 32 ||
                member_type->size == 16 ||
                member_type->size == 8 ) {
                bits = 0;
            } else {
                bits = member_type->size & 0x1f;
            }
            if( new_offset != offset || new_bit_start < bit_start ) {
                /* must create a NewOffset record */
                put8( member, TD_STRUCT_NEW_OFFSET );
                put32( member, new_offset );
                bit_start = 0;
            }
            if( new_bit_start > bit_start ) {
                /* must generate a padding field */
                put8( member, (new_bit_start-bit_start)<<TD_STRUCT_BITS_SHIFT );
                putName( member, NAME_NULL );
                putIndex( member, TD_ID_VOID );
            }
            if( field + 1 == field_stop ) {     /* last member */
                bits |= TD_STRUCT_LAST_MEMBER;
            }
            put8( member, bits );
            putName( member, field->name );
            putIndex( member, resolveType( field->type ) );
            offset = new_offset + ( member_type->size >> 3 );
            bit_start = new_bit_start + ( member_type->size & 7 );
            ++field;
        }
    }
    endType( tr );
}

STATIC void doEnum( cantype *type, type_rec *tr ) {
/*
    Assumes caller has done tr = newType( name, size, TD_ID_ENUM );
    Does an endType( tr ).

    FIXME there is no support for unsigned enumerated types, or for enumerated
    types of non-int size.
*/
    enum_const  *enum_c;
    enum_const  *enum_cstop;
    type_rec    *member;

/**/myassert( type != NULL && type->class == CANT_ENUM );
    putIndex( tr, resolveType( type->d.enumr.base_type ) );
    if( type->size > 16 ) {
        Fatal( MSG_ENUM_TOO_LARGE );
    }
    enum_c = type->d.enumr.consts;
    if( enum_c != NULL ) {
        enum_cstop = enum_c + type->d.enumr.num_consts;
        put16( tr, (int_16)enum_c->value );             /* low value */
        put16( tr, (int_16)(enum_cstop - 1)->value );   /* high value */
        member = tr->member;
        while( enum_c < enum_cstop ) {
            put8( member, 0 );
            putName( member, enum_c->name );
            put16( member, (int_16)enum_c->value );
            ++enum_c;
        }
    }
    endType( tr );
}

STATIC void typ2TypeDef( cantype *type ) {

    cantype     *base_type;
    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_TYPEDEF );
    base_type = CanTFind( type->d.typdef.type );
    switch( type->d.typdef.scope ) {
    case CANT_SCOPE_NULL:
        /*
            This next line won't cause recursion because of the special
            case loop in resolveType().
        */
        type->extra = resolveType( base_type->hdl );
        break;
    case CANT_SCOPE_STRUCT:
/**/    myassert( base_type->class == CANT_STRUCT );
        tr = newLarge( type->d.typdef.name, base_type->size, TD_ID_STRUCT,
                TD_ID_VLSTRUCT );
        base_type->extra = type->extra = tr->idx;
        doStruct( base_type, tr );
        break;
    case CANT_SCOPE_UNION:
/**/    myassert( base_type->class == CANT_STRUCT );
        tr = newLarge( type->d.typdef.name, base_type->size, TD_ID_UNION,
                TD_ID_VLUNION );
        base_type->extra = type->extra = tr->idx;
        doStruct( base_type, tr );
        break;
    case CANT_SCOPE_ENUM:
/**/    myassert( base_type->class == CANT_ENUM );
        tr = newType( type->d.typdef.name, base_type->size, TD_ID_ENUM );
        base_type->extra = type->extra = tr->idx;
        doEnum( base_type, tr );
        break;
    default:
/**/    never_reach();
    }
}

STATIC void typ2SubRange( cantype *type ) {

    type_rec    *tr;
    uint_8      tid;

/**/myassert( type != NULL && type->class == CANT_SUBRANGE );
    if( type->sgned ) {
        if( type->size <= 8 ) {
            tid = TD_ID_SCHAR;
        } else if( type->size <= 16 ) {
            tid = TD_ID_SINT;
        } else if( type->size <= 32 ) {
            tid = TD_ID_SLONG;
        } else {
            Fatal( MSG_SUBR_TOO_LARGE );
        }
    } else {
        if( type->size <= 8 ) {
            tid = TD_ID_UCHAR;
        } else if( type->size <= 16 ) {
            tid = TD_ID_UINT;
        } else if( type->size <= 32 ) {
            tid = TD_ID_ULONG;
        } else {
            Fatal( MSG_SUBR_TOO_LARGE );
        }
    }
    switch( CanMisc.src_language ) {
    case CAN_LANG_C:        /* these can't use subrange types */
    case CAN_LANG_ASM:
        type->extra = tid;
        break;
    default:
        tr = newType( NAME_NULL, type->size, tid );
        type->extra = tr->idx;
        putIndex( tr, resolveType( type->d.subrng.base_type ) );
        put32( tr, type->d.subrng.low );
        put32( tr, type->d.subrng.high );
        endType( tr );
        break;
    }
}

STATIC void typ2ArrayZ( cantype *type ) {

    type_rec    *tr;

/**/myassert( type != NULL && ( type->class == CANT_ARRAY ||
        type->class == CANT_ARRAY_ZERO ) );
    tr = newLarge( NAME_NULL, type->size, TD_ID_CARRAY, TD_ID_VLARRAY );
    type->extra = tr->idx;
    putIndex( tr, resolveType( type->d.array.base_type ) );
    endType( tr );
}

STATIC void typ2Array( cantype *type ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_ARRAY );
    switch( CanMisc.src_language ) {
    case CAN_LANG_C:        /* these can't use PARRAY */
    case CAN_LANG_ASM:
        typ2ArrayZ( type );
        break;
    default:
        tr = newType( NAME_NULL, type->size, TD_ID_PARRAY );
        type->extra = tr->idx;
        putIndex( tr, resolveType( type->d.array.base_type ) );
        putIndex( tr, resolveType( type->d.array.index_type ) );
        endType( tr );
        break;
    }
}

STATIC void typ2ArrayD( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_ARRAY_DESC );
    PrtMsg( WRN|MSG_UNS_ARRAY_DESC );
    type->extra = TD_TYPE_VOID;
}

STATIC void typ2Pointer( cantype *type ) {

    type_rec    *tr;
    uint_8      tid;

    if( type->d.pointr.class & CANT_PTR_386 ) {
        if( type->d.pointr.class & CANT_PTR_FAR ) {
            tid = TD_ID_FAR386;
        } else {
            tid = TD_ID_NEAR386;
        }
    } else {
        if( type->d.pointr.class & CANT_PTR_FAR ) {
            tid = TD_ID_FAR;
        } else {
            tid = TD_ID_NEAR;
        }
    }
    tr = newType( NAME_NULL, type->size, tid );
    type->extra = tr->idx;
    putIndex( tr, resolveType( type->d.pointr.base_type ) );
    if( type->d.pointr.class & CANT_PTR_HUGE ) {
        put8( tr, 1 );
    } else {
        put8( tr, 0 );
    }
    endType( tr );
}

STATIC void typ2Enum( cantype *type, int force ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_ENUM );
    if( force ) {
        tr = newType( NAME_NULL, type->size, TD_ID_ENUM );
        type->extra = tr->idx;
        doEnum( type, tr );
    }
}

STATIC void typ2Struct( cantype *type, int force ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_STRUCT );
    if( force ) {
        tr = newLarge( NAME_NULL, type->size, TD_ID_STRUCT, TD_ID_VLSTRUCT );
        type->extra = tr->idx;
        doStruct( type, tr );
    }
}

STATIC void typ2Procedure( cantype *type ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_PROCEDURE );
    tr = newType( NAME_NULL, type->size, TD_ID_FUNCTION );
    type->extra = tr->idx;
    putIndex( tr, resolveType( type->d.proc.ret_type ) );
    if( CanMisc.src_language == CAN_LANG_PASCAL ) {
        if( type->d.proc.class & CANT_PROC_FAR ) {
            put8( tr, TD_FCN_PASCAL_FAR );
        } else {
            put8( tr, TD_FCN_PASCAL_NEAR );
        }
    } else {
        if( type->d.proc.class & CANT_PROC_FAR ) {
            put8( tr, TD_FCN_C_FAR );
        } else {
            put8( tr, TD_FCN_C_NEAR );
        }
    }
    put8( tr, 0 ); /* varargs indicator */
    endType( tr );
}

STATIC void typ2CharBlock( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_CHARBLOCK );
    type->extra = buildArray( type->size );
}

STATIC void typ2CharBlockI( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_CHARBLOCK_IND );
    PrtMsg( WRN|MSG_UNS_CHARBLOCK_IND );
    type->extra = TD_TYPE_VOID;
}

STATIC int typePass2( void *_type, void *_force ) {

    cantype *type = _type;
    int *force = _force;
/**/myassert( type != NULL );
    if( type->extra != 0 ) {    /* already processed */
        return( 0 );
    }
    switch( type->class ) {
    case CANT_TYPEDEF:      typ2TypeDef( type );            break;
    case CANT_SUBRANGE:     typ2SubRange( type );           break;
    case CANT_ARRAY:        typ2Array( type );              break;
    case CANT_ARRAY_ZERO:   typ2ArrayZ( type );             break;
    case CANT_ARRAY_DESC:   typ2ArrayD( type );             break;
    case CANT_POINTER:      typ2Pointer( type );            break;
    case CANT_ENUM:         typ2Enum( type, *force );       break;
    case CANT_STRUCT:       typ2Struct( type, *force );     break;
    case CANT_PROCEDURE:    typ2Procedure( type );          break;
    case CANT_CHARBLOCK:    typ2CharBlock( type );          break;
    case CANT_CHARBLOCK_IND:typ2CharBlockI( type );         break;
    }
    return( 0 );
}

void Can2TDT( void ) {
/******************/
    int force;

    if( CanMisc.types_present ) {
        realHdl = NameAdd( "real", 4 );
        imagHdl = NameAdd( "imag", 4 );
        nextIdx = 24;
        typeRecCarver = CarveCreate( sizeof( type_rec ), 8 );

        CanTWalk( NULL, typePass1 );
        force = 0;
        CanTWalk( &force, typePass2 );
        force = 1;
        CanTWalk( &force, typePass2 );

        CarveDestroy( typeRecCarver );
    }
}

STATIC int doTypeDef( void *_type, void *parm ) {

    cantype     *type = _type;
    obj_rec     *coment;
    const char  *name;
    size_t      name_len;

/**/myassert( type != NULL && parm == NULL );
    parm = parm;

    if( type->class == CANT_TYPEDEF &&
        type->d.typdef.scope == CANT_SCOPE_NULL ) {
        name = NameGet( type->d.typdef.name );
        name_len = strlen( name );
    /**/myassert( name_len < 256 );
        /* 1 for the name_len, 2 for the type_idx, 1 for the class */
        coment = Can2TDNewRec( TD_CMT_LOCAL_DEFN, name_len + (1+2+1) );
        ObjPutName( coment, name, name_len );
        ObjPutIndex( coment, type->extra );
        ObjPut8( coment, TD_LOCAL_TYPEDEF );
        Can2TDEndRec( coment );
    }
    return( 0 );
}

void Can2TDTypeDef( void ) {
/************************/

    if( CanMisc.types_present ) {
        CanTWalk( NULL, doTypeDef );
    }
}

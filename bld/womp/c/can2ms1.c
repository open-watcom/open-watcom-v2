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
#include <watcom.h>
#include "womp.h"
#include "msdbg.h"
#include "canaddr.h"
#include "cansymb.h"
#include "cantype.h"
#include "namemgr.h"
#include "memutil.h"
#include "myassert.h"
#include "genutil.h"
#include "objio.h"
#include "pcobj.h"
#include "canmisc.h"
#include "objprs.h"
#include "carve.h"

/*
    These routines do the conversion from canonical types to Microsoft (MS)
    typing information.  Here's a quick rundown:

    Writing types:
        The procedure newType( start_leaf ) returns a pointer to a type_rec.
        The idx field in the type_rec is the MS index for the type_rec.
        Then the put* functions (put8, put16, put32, putIndex,
        putName, putUnsigned, and putSigned) are used to write to the
        type leaf.  The call to endType( idx ) finishes the type record.
        getTrData(), maybeWriteType(), and flushLedata() are used by these
        functions to achieve their results.

    Pass 1:
        functions typePass1, typ1Signed, ...

        We use the extra field in the cantypes to store the MS index that the
        cantype has been converted to.  This pass sets the extra fields to 0
        which indicates a type record that has not been processed yet. During
        pass 1 we can write out the basic SCALAR types since they do not
        require any other types to be resolved; these types have their extra
        fields set to some non-zero value.

        Anything that goes on in pass1 cannot resolve other types in the type
        graph.  ie: they cannot use any values in the extra fields for other
        types.

    Pass 2:
        functions typePass2, resolveType, doStruct, doEnum, typ2TypeDef,
        typ2SubRange, typ2Array, typ2ArrayD, typ2Pointer, typ2Enum,
        typ2Struct, typ2Procedure, typ2CharBlock, typ2CharBlockI

        This pass does the majority of the work.  The type graph is traversed
        setting type->extra for all types.  The function resolveType() is used
        to resolve any cantype handle into an MS index.  In order to use
        resolveType, the caller must set his type->extra field to a valid
        MS type index.  This is to ensure that the program does not
        recurse to death.

        STRUCTs and ENUMs are not dumped unless a TYPEDEF for them is
        found. If a TYPEDEF is found, the TYPEDEF and the STRUCT/ENUM will
        have their extra fields set the same.

        In typ2Enum, typ2Struct, and typePass2, force refers to whether or
        not the routine is forced to create a type.  typ2Enum and typ2Structs
        will not do anything unless force is non-zero.  This is to eliminate
        extra records for structs/enums that are never referenced.  Pass 3
        will force any stragglers to be created.

    Pass 3:
        functions typePass3, typ2Struct, typ2Enum

        This pass is basically just cleanup.  It ensures that every STRUCT
        and ENUM has been dumped at least once.  This is just in case their
        is a STRUCT/ENUM that has no associated TYPEDEF.

    The remaining functions setupObjIO and finishObjIO just create and
    destroy the ledata buffers which the newType/endType routines use.

    23-Oct-90 DJG
    19-Dec-90 DJG
*/

/*
    If Microsoft would only follow their own documentation we wouldn't run
    into so many problems.

    The MS OMF doc says that we can use NEWTYPE for a typedef; however,
    CodeView 3.00 does not understand NEWTYPE when it is applied to one of
    the predefined types (0000-01ff).  It gets all huffy and pouts and
    insists it does not know how to display the value of one of these
    NEWTYPEd variables.

    MS C 6.00 gets around this in Microsofts usual kludgy kinda way - it
    never outputs a NEWTYPE record.  So, for example, how does it handle

    typedef struct blah {
        int a;
        long b;
    } splat;

    splat dope;

    ??

    Well, dope will have the type index of "struct blah", not the type
    index of "typedef struct blah splat", and the typedef won't even be
    generated.

    So we will bend over backwards and eliminate all NEWTYPE's so that
    CodeView doesn't have to do something that would be incredibly easy
    for it to do.

    I wrote the code originally to use NEWTYPE, so I'll leave that code in
    with #ifdef USE_NEWTYPE.

    23-Oct-90 DJG
    19-Dec-90 DJG
*/

typedef uint_16     idx_t;      /* index type for MS types */


/*
    Functions for handling the writing of typing information
*/

#define TYPE_REC_INC    128
typedef struct type_rec type_rec;
struct type_rec {
    type_rec    *next;      /* used to queue type_recs for writing */
    idx_t       idx;
    uint_16     alloc;      /* allocated length */
    uint_16     len;
    uint_8      *data;      /* data buffer */
};

STATIC obj_rec      *mySegdef;      /* the segdef for $$TYPES */
STATIC type_rec     *waitingRecs;   /* ordered list of records waiting to be
                                       written into LEDATA's */
STATIC carve_t      trCarver;       /* carver for type_recs */
STATIC idx_t        nextIdx;        /* next available index */
STATIC uint_8       *ledataBuf;     /* 1024 byte buffer for building LEDATAs */
STATIC size_t       ledataOffset;   /* current offset into ledataBuf */
STATIC idx_t        nextWriteIdx;   /* next index to be written */
STATIC uint_16      writeLEOffset;  /* offset into $$TYPES for next LEDATA */
STATIC idx_t        voidIdx;        /* index of the void type */

#define MAX_LEDATA      1024        /* maximum amount of data in an LEDATA */

STATIC char *getTrData( type_rec *tr, uint_16 len ) {

    uint_16 new_len;

    new_len = tr->len + len;
    if( new_len > tr->alloc ) {
        tr->alloc = new_len + TYPE_REC_INC;
        tr->data = MemRealloc( tr->data, new_len + TYPE_REC_INC );
    }
    tr->len = new_len;
    return( tr->data + new_len - len );
}

STATIC void put8( type_rec *tr, uint_8 byte ) {

    uint_8  *p;

    p = getTrData( tr, 1 );
    p[0] = byte;
}

STATIC void putIndex( type_rec *tr, uint_16 word ) {

    uint_8  *p;

    p = getTrData( tr, 3 );
    p[0] = MS_BCL_INDEX;
    WriteU16( p + 1, word );
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
    p = getTrData( tr, len + 2 );
    p[0] = MS_BCL_STRING;
    p[1] = len;
    memcpy( p + 2, name, len );
}

STATIC void putUnsigned( type_rec *tr, uint_32 num ) {

    uint_8  *p;

    if( num < 0x80 ) {
        p = getTrData( tr, 1 );
        p[0] =   num & 0x000000ff;
    } else if( num < 0x10000 ) {
        p = getTrData( tr, 3 );
        p[0] = MS_BCL_UINT_16;
        WriteU16( p+1, num );
    } else {
        p = getTrData( tr, 5 );
        p[0] = MS_BCL_UINT_32;
        WriteU32( p+1, num );
    }
}

STATIC void putSigned( type_rec *tr, int_32 num ) {

    uint_32 mag;
    uint_8  *p;

    mag = ( num > 0 ) ? num : -num;
    if( mag < 0x80 ) {
        p = getTrData( tr, 2 );
        p[0] = MS_BCL_INT_8;
        p[1] = (int_8)num;
    } else if( mag < 0x8000 ) {
        p = getTrData( tr, 3 );
        p[0] = MS_BCL_INT_16;
        WriteS16( p+1, num );
    } else {
        p = getTrData( tr, 5 );
        p[0] = MS_BCL_INT_32;
        WriteS32( p+1, num );
    }
}

STATIC type_rec *newType( uint_8 start_leaf ) {

    type_rec    *new;

    if( nextIdx == 0 ) {
        Fatal( MSG_TOO_MANY_TYPES );
    }
    new = CarveAlloc( trCarver );
    new->alloc = TYPE_REC_INC;
    new->len = 1;
    new->data = MemAlloc( TYPE_REC_INC );
    new->data[0] = start_leaf;
    new->idx = nextIdx;
    ++nextIdx;

    return( new );
}

STATIC void setupObjIO( void ) {

    waitingRecs = NULL;
    nextIdx = 512;
    ledataBuf = MemAlloc( MAX_LEDATA );
    ledataOffset = 0;
    nextWriteIdx = 512;
    writeLEOffset = 0;  /* offset of next LEDATA to be written */
    trCarver = CarveCreate( sizeof( type_rec ), 8 );
    mySegdef = Can2MsSegdef( MS_DDTYPES MS_DEBTYP,
        MS_DDTYPES_LEN + MS_DEBTYP_LEN );
}

STATIC void flushLedata( void ) {

    uint_16 length;
    obj_rec *ledata;

    if( ledataOffset > 0 ) {
        ledata = Can2MsRec( CMD_LEDATA );
        ledata->d.ledata.idx = mySegdef->d.segdef.idx;
        ledata->d.ledata.offset = writeLEOffset;
        length = ledataOffset;
        ObjAllocData( ledata, length );
        ObjPut( ledata, ledataBuf, length );
        ObjTruncRec( ledata );
        ObjRSeek( ledata, 0 );  /* so filters can find the data */

            /* update state */
        ledataOffset = 0;
        writeLEOffset += length;
    }
}

STATIC void finishObjIO( void ) {

    flushLedata();
    mySegdef->d.segdef.seg_length = writeLEOffset;
    MemFree( ledataBuf );
    CarveDestroy( trCarver );
}

STATIC void endType( type_rec *rec ) {

    type_rec    **walk;
    unsigned    rec_len;    /* equivalent to rec->len */
    unsigned    rec_offset; /* offset into rec->data */
    unsigned    write_len;  /* length we can write */

    if( rec->idx > nextWriteIdx ) {
        /* ok, we can't write yet so store it a while */
/**/    myassert( rec->len > 0 );
        rec->alloc = rec->len;
        rec->data = MemRealloc( rec->data, rec->len );
        walk = &waitingRecs;
        while( *walk ) {
            if( rec->idx < (*walk)->idx ) {
                break;
            }
/**/        myassert( rec->idx != (*walk)->idx );   /* no duplicates */
            walk = &(*walk)->next;
        }
        rec->next = *walk;
        *walk = rec;
        return;
    }

    for(;;) {   /* all right! we can write this type out! */
/**/    myassert( rec->idx == nextWriteIdx );
        if( ledataOffset + 3 > MAX_LEDATA ) {
            flushLedata();
        }
        /* write the linkage and length fields */
        ledataBuf[ ledataOffset++ ] = 1;    /* linkage always 1 */
        rec_len = rec->len;
        WriteU16( ledataBuf + ledataOffset, rec_len );
        ledataOffset += 2;
        rec_offset = 0;
        while( rec_offset < rec_len ) {
            write_len = rec_len - rec_offset;
            if( write_len > MAX_LEDATA - ledataOffset ) {
                write_len = MAX_LEDATA - ledataOffset;
                if( write_len == 0 ) {
                    flushLedata();
                    continue;   /* try this again */
                }
            }
            memcpy( ledataBuf + ledataOffset, rec->data + rec_offset,write_len);
            ledataOffset += write_len;
            rec_offset += write_len;
        }
        MemFree( rec->data );       /* discard the data */
        CarveFree( trCarver, rec ); /* free the record */
        ++nextWriteIdx;
        rec = waitingRecs;  /* check next waiting record */
        if( rec == NULL || rec->idx > nextWriteIdx ) break;
        /* we can also write out the next type_rec */
        waitingRecs = rec->next;
    }
}

STATIC idx_t buildArray( bitsize size ) {

    type_rec    *tr;
    idx_t       idx;

    tr = newType( MS_SL_ARRAY );
    putUnsigned( tr, size );
    putIndex( tr,   MS_RT_I_FIELD |
                    MS_RT_MD_DIRECT |
                    MS_RT_TYP_UNSIGNED |
                    MS_SZ_8BIT );
    idx = tr->idx;
    endType( tr );
    return( idx );
}

/*
    Functions for Type Pass 1.  (Initialize the extra field, and other small
    tasks).
*/

STATIC void typ1Integer( cantype *type ) {
/*
    Set the extra field to the appropriate Microsoft reserved type.
*/
    uint_32 mstype;

/**/myassert( type != NULL && type->class == CANT_INTEGER );
    if( type->sgned ) {
        mstype = MS_RT_I_FIELD | MS_RT_MD_DIRECT | MS_RT_TYP_SIGNED;
    } else {
        mstype = MS_RT_I_FIELD | MS_RT_MD_DIRECT | MS_RT_TYP_UNSIGNED;
    }
    switch( type->size ) {
    case  8:    mstype |= MS_SZ_8BIT;   break;
    case 16:    mstype |= MS_SZ_16BIT;  break;
    case 32:    mstype |= MS_SZ_32BIT;  break;
    /* we delay generating non-standard sizes till 2nd pass */
    default:    mstype  = 0;            break;
    }
    type->extra = mstype;
}

STATIC void typ1Real( cantype *type ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_REAL );
    switch( type->size ) {
    case 32:
        type->extra=MS_RT_I_FIELD|MS_RT_MD_DIRECT|MS_RT_TYP_REAL|MS_SZ_REAL_4;
        break;
    case 64:
        type->extra=MS_RT_I_FIELD|MS_RT_MD_DIRECT|MS_RT_TYP_REAL|MS_SZ_REAL_8;
        break;
    case 80:
        type->extra=MS_RT_I_FIELD|MS_RT_MD_DIRECT|MS_RT_TYP_REAL|MS_SZ_REAL_10;
        break;
    default:
        /* non standard size - must create a type for it */
        tr = newType( MS_SL_SCALAR );
        type->extra = tr->idx;
        putUnsigned( tr, (uint_32)type->size );
        put8( tr, MS_BTL_REAL );
        endType( tr );
        break;
    }
}

STATIC void typ1Void( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_VOID );
    type->extra = voidIdx;
}

STATIC void typ1Complex( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_COMPLEX );
    switch( type->size ) {
    case  64:
        type->extra = MS_RT_I_FIELD|MS_RT_MD_DIRECT|MS_RT_TYP_COMPLEX|
            MS_SZ_COMPLEX_8;
        break;
    case 128:
        type->extra = MS_RT_I_FIELD|MS_RT_MD_DIRECT|MS_RT_TYP_COMPLEX|
            MS_SZ_COMPLEX_16;
        break;
    case 160:
        type->extra = MS_RT_I_FIELD|MS_RT_MD_DIRECT|MS_RT_TYP_COMPLEX|
            MS_SZ_COMPLEX_20;
        break;
    default:
        type->extra = buildArray( type->size );
        break;
    }
}

STATIC int typePass1( cantype *type, void *parm ) {

/**/myassert( type != NULL );
    parm = parm;
    switch( type->class ) {
    case CANT_INTEGER:      typ1Integer( type );    break;
    case CANT_REAL:         typ1Real( type );       break;
    case CANT_VOID:         typ1Void( type );       break;
    case CANT_COMPLEX:      typ1Complex( type );    break;
    default:
        type->extra = 0;    /* initialize to 0 */
    }
    return( 0 );
}

/*
    Functions for type pass 2.  This pass does most of the work of converting
    cantypes to ms types.
*/

FORWARD STATIC int typePass2( cantype *type, int *force );

STATIC idx_t resolveType( type_handle hdl ) {

    cantype *type;

    type = CanTFind( hdl );
/**/myassert( type != NULL );

    if( type->extra == 0 ) {
        int force;

#ifndef USE_NEWTYPE
        while( type->class == CANT_TYPEDEF &&
            type->d.typdef.scope == CANT_SCOPE_NULL ) {
            type = CanTFind( type->d.typdef.type );
        }
#endif
        force = 1;
        typePass2( type, &force );
    }
/**/myassert( type->extra != 0 );
    return( type->extra );
}

STATIC void doStruct( cantype *type, name_handle name_hdl, type_rec *tr ) {
/*
    Assumes that caller has done tr = newType( MS_SL_STRUCTURE ), but has not
    written any type info into the new type yet.  Does an endType( tr );
*/
    type_rec        *tlist;
    type_rec        *nlist;
    struct_field    *field;
    struct_field    *field_stop;
    cantype         *field_type;

/**/myassert( type != NULL && type->class == CANT_STRUCT );

    /* write the structure info */
    putUnsigned( tr, type->size );                  /* length of structure */
    putUnsigned( tr, type->d.strct.num_fields );    /* number of fields */
    tlist = newType( MS_SL_LIST );
    putIndex( tr, tlist->idx );
    nlist = newType( MS_SL_LIST );
    putIndex( tr, nlist->idx );
    putName( tr, name_hdl );
    put8( tr, MS_OL_PACKED );
    endType( tr );

    /* write the field types and name/offset lists */
    field = type->d.strct.fields;
    if( field != NULL ) {
        field_stop = field + type->d.strct.num_fields;
        while( field < field_stop ) {
            if( field->bitfield ) {
                field_type = CanTFind( field->type );
                tr = newType( MS_SL_BITFIELD );
                field_type->extra = tr->idx;
                putUnsigned( tr, field_type->size );
                put8( tr, ( field_type->sgned ) ?
                    MS_BTL_SIGNED_INT : MS_BTL_UNSIGNED_INT );
                put8( tr, field->bit_offset & 0x07 );
                endType( tr );
                putIndex( tlist, tr->idx );
            } else {
                putIndex( tlist, resolveType( field->type ) );
            }
            putName( nlist, field->name );
            putUnsigned( nlist, field->bit_offset >> 3 );
            ++field;
        }
    }
    endType( tlist );
    endType( nlist );
}

STATIC void doEnum( cantype *type, name_handle name_hdl, type_rec *tr ) {
/*
    Assumes that tr = newType( MS_SL_SCALAR );. Does an endType( tr );
*/
    type_rec    *elist;
    enum_const  *enum_c;
    enum_const  *enum_cstop;
    int         is_signed;
    int_32      slo, shi;
    uint_32     ulo, uhi;

/**/myassert( type != NULL && type->class == CANT_ENUM );
    putUnsigned( tr, type->size ); /* length of enum type */
    if( type->sgned ) {
        is_signed = 1;
        put8( tr, MS_BTL_SIGNED_INT );
    } else {
        is_signed = 0;
        put8( tr, MS_BTL_UNSIGNED_INT );
    }
    putName( tr, name_hdl );
    elist = newType( MS_SL_LIST );
    putIndex( tr, elist->idx );

    /* walk through constants */
    enum_c = type->d.enumr.consts;
    if( enum_c != NULL ) {
        if( is_signed ) {
            slo = enum_c->value;
            shi = enum_c->value;
        } else {
            ulo = enum_c->value;
            uhi = enum_c->value;
        }
        enum_cstop = enum_c + type->d.enumr.num_consts;
        while( enum_c < enum_cstop ) {
            putName( elist, enum_c->name );
            if( is_signed ) {
                if( (int_32)enum_c->value < slo ) {
                    slo = enum_c->value;
                } else if( (int_32)enum_c->value > shi ) {
                    shi = enum_c->value;
                }
                putSigned( elist, (int_32)enum_c->value );
            } else {
                if( (uint_32)enum_c->value < ulo ) {
                    ulo = enum_c->value;
                } else if( (uint_32)enum_c->value > uhi ) {
                    uhi = enum_c->value;
                }
                putUnsigned( elist, enum_c->value );
            }
            ++enum_c;
        }
        if( is_signed ) {
            putSigned( tr, slo );
            putSigned( tr, shi );
        } else {
            putUnsigned( tr, ulo );
            putUnsigned( tr, uhi );
        }
    } else {
        putUnsigned( tr, 0 );
        putUnsigned( tr, 1 );
    }
    endType( elist );
    endType( tr );
}

STATIC void typ2Integer( cantype *type ) {
/*
    We delay creating non-standard integer sizes until the second pass so that
    they may possibly be created by doStruct... KLUDGE! FIXME - we might
    still generate an extraneous type...
*/
    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_INTEGER );
    tr = newType( MS_SL_BITFIELD );
    type->extra = tr->idx;
    putUnsigned( tr, type->size );
    put8( tr, ( type->sgned ) ? MS_BTL_SIGNED_INT : MS_BTL_UNSIGNED_INT );
    put8( tr, 0 );
    endType( tr );
}

STATIC void typ2TypeDef( cantype *type ) {

    type_rec    *tr;
    cantype     *base_type;

/**/myassert( type != NULL && type->class == CANT_TYPEDEF );
    if( type->d.typdef.scope == CANT_SCOPE_NULL ) {
#ifdef USE_NEWTYPE
        tr = newType( MS_SL_NEWTYPE );
        type->extra = tr->idx;
        putIndex( tr, resolveType( type->d.typdef.type ) );
        putName( tr, type->d.typdef.name );
        endType( tr );
#else
        /* this is safe because of the special case loop in resolveType */
        type->extra = resolveType( type->d.typdef.type );
#endif
        return;
    }
    base_type = CanTFind( type->d.typdef.type );
/**/myassert( base_type != NULL );
    switch( type->d.typdef.scope ) {
    case CANT_SCOPE_STRUCT:
/**/    myassert( base_type->class == CANT_STRUCT );
        tr = newType( MS_SL_STRUCTURE );
        base_type->extra = type->extra = tr->idx;
        doStruct( base_type, type->d.typdef.name, tr );
        break;
    case CANT_SCOPE_UNION:
/**/    myassert( base_type->class == CANT_STRUCT );
        tr = newType( MS_SL_STRUCTURE );
        base_type->extra = type->extra = tr->idx;
        doStruct( base_type, type->d.typdef.name, tr );
        break;
    case CANT_SCOPE_ENUM:
/**/    myassert( base_type->class == CANT_ENUM );
        tr = newType( MS_SL_SCALAR );
        base_type->extra = type->extra = tr->idx;
        doEnum( base_type, type->d.typdef.name, tr );
        break;
    default:
/**/    never_reach();
    }
}

STATIC void typ2SubRange( cantype *type ) {

    type_rec    *tr;
    int         is_signed;

/**/myassert( type != NULL && type->class == CANT_SUBRANGE );
    tr = newType( MS_SL_SCALAR );
    type->extra = tr->idx;
    put8( tr, type->size );
    if( type->sgned ) {
        is_signed = 1;
        put8( tr, MS_BTL_SIGNED_INT );
    } else {
        is_signed = 0;
        put8( tr, MS_BTL_UNSIGNED_INT );
    }
    putName( tr, NAME_NULL );
    put8( tr, MS_BCL_NIL );
    if( is_signed ) {
        putSigned( tr, (int_32)type->d.subrng.low );
        putSigned( tr, (int_32)type->d.subrng.high );
    } else {
        putUnsigned( tr, type->d.subrng.low );
        putUnsigned( tr, type->d.subrng.high );
    }
    endType( tr );
}

STATIC void typ2Array( cantype *type ) {

    type_rec    *tr;

/**/myassert( type != NULL && ( type->class == CANT_ARRAY ||
        type->class == CANT_ARRAY_ZERO ) );
    tr = newType( MS_SL_ARRAY );
    type->extra = tr->idx;
    putUnsigned( tr, type->size ); /* size of array */
    putIndex( tr, resolveType( type->d.array.base_type ) );
    if( type->class == CANT_ARRAY ) {
        putIndex( tr, resolveType( type->d.array.index_type ) );
    }
    /* we don't emit the optional array name */
    endType( tr );
}

STATIC void typ2ArrayD( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_ARRAY_DESC );
    PrtMsg( WRN|MSG_UNS_ARRAY_DESC );
    type->extra = voidIdx;
}

STATIC uint_8 ptrClass( uint_8 class ) {

    if( class & CANT_PTR_HUGE ) {
        return( MS_RT_I_FIELD | MS_RT_MD_HUGE_PTR );
    } else if( class & CANT_PTR_FAR ) {
        return( MS_RT_I_FIELD | MS_RT_MD_FAR_PTR );
    }
    return( MS_RT_I_FIELD | MS_RT_MD_NEAR_PTR );
}

STATIC void typ2Pointer( cantype *type ) {

    type_rec    *tr;
    cantype     *base_type;
    idx_t       base_idx;

/**/myassert( type != NULL && type->class == CANT_POINTER &&
        type->extra == 0 );
/*
    We ignore the CANT_PTR_386 bit because Microsoft has no specific 386
    pointer type.  dumb.  We'll also ignore the DEREF bit since there is
    no support for it.
*/
    base_type = CanTElimTypeDef( type->d.pointr.base_type );
    switch( base_type->class ) {
    case CANT_INTEGER:
    case CANT_REAL:
    case CANT_COMPLEX:
            /* we'll try to use the reserved types here */
        base_idx = resolveType( base_type->hdl );
        if( base_idx < 256 && base_idx > 127 ) {
            type->extra = ptrClass( type->d.pointr.class ) |
                base_idx;
            return;
        }
            /* otherwise we have to create a pointer for it */
        break;
    case CANT_VOID:
        /* another Microsoft booboo... no "void *" type */
        type->extra = ptrClass( type->d.pointr.class ) |
            MS_RT_TYP_SIGNED | MS_SZ_8BIT;
        return;
    }
    /* if we get to here then create a pointer type */
    tr = newType( MS_SL_POINTER );
    type->extra = tr->idx;
    if( type->d.pointr.class & CANT_PTR_HUGE ) {
        put8( tr, MS_OL_HUGE );
    } else if( type->d.pointr.class & CANT_PTR_FAR ) {
        put8( tr, MS_OL_PLM_FAR );
    } else {
        put8( tr, MS_OL_PLM_NEAR );
    }
    putIndex( tr, resolveType( type->d.pointr.base_type ) );
    endType( tr );
}

STATIC void typ2Enum( cantype *type, int force ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_ENUM );
    if( force ) {
        tr = newType( MS_SL_SCALAR );
        type->extra = tr->idx;
        doEnum( type, NAME_NULL, tr );
    }
}

STATIC void typ2Struct( cantype *type, int force ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_STRUCT && type->extra == 0 );
    if( force ) {
        tr = newType( MS_SL_STRUCTURE );
        type->extra = tr->idx;
        doStruct( type, NAME_NULL, tr );
    }
}

STATIC void typ2Procedure( cantype *type ) {

    type_rec    *tr;
    type_rec    *list;
    proc_parm   *parm;
    proc_parm   *parm_stop;

/**/myassert( type != NULL && type->class == CANT_PROCEDURE &&
        type->extra == 0 );
/*
    We ignore the CANT_PROC_386 bit because Microsoft does not have any 386
    specific procedure type.
*/
    tr = newType( MS_SL_PROCEDURE );
    type->extra = tr->idx;
    put8( tr, MS_BCL_NIL );
    putIndex( tr, resolveType( type->d.proc.ret_type ) );
    if( type->d.proc.class & CANT_PROC_FAR ) {
        put8( tr, MS_OL_C_FAR );  /* FIXME support other styles */
    } else {
        put8( tr, MS_OL_C_NEAR );
    }
    putUnsigned( tr, type->d.proc.num_parms );
    if( type->d.proc.num_parms == 0 ) {
        putIndex( tr, voidIdx );    /* nil leaf for no parms */
    } else {
        list = newType( MS_SL_LIST );
        /* output parm types */
        parm = type->d.proc.parms;
        parm_stop = parm + type->d.proc.num_parms;
        while( parm < parm_stop ) {
            putIndex( list, resolveType( parm->type ) );
            ++parm;
        }
        putIndex( tr, list->idx );
        endType( list );
    }
    endType( tr );
}

STATIC void typ2CharBlock( cantype *type ) {

    type_rec    *tr;

/**/myassert( type != NULL && type->class == CANT_CHARBLOCK );
    tr = newType( MS_SL_FSTRING );
    type->extra = tr->idx;
    put8( tr, 0 );
    putUnsigned( tr, type->d.charb.length );
    endType( tr );
}

STATIC void typ2CharBlockI( cantype *type ) {

/**/myassert( type != NULL && type->class == CANT_CHARBLOCK_IND );
    PrtMsg( WRN|MSG_UNS_CHARBLOCK_IND );
    type->extra = voidIdx;
}

STATIC int typePass2( cantype *type, int *force ) {

/**/myassert( type != NULL );
    if( type->extra != 0 ) {    /* already processed */
        return( 0 );
    }
    switch( type->class ) {
    case CANT_RESERVED:                                     break;
    case CANT_INTEGER:      typ2Integer( type );            break;
    case CANT_TYPEDEF:      typ2TypeDef( type );            break;
    case CANT_SUBRANGE:     typ2SubRange( type );           break;
    case CANT_ARRAY:        /* FALL THROUGH */
    case CANT_ARRAY_ZERO:   typ2Array( type );              break;
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

STATIC void makeVoid( void ) {

    type_rec    *tr;

    tr = newType( MS_BCL_NIL );
    voidIdx = tr->idx;
    endType( tr );
}

void Can2MsT( void ) {
/******************/
    int force;

    if( CanMisc.types_present ) {
        setupObjIO();
        makeVoid();

        CanTWalk( NULL, typePass1 );
/**/    myassert( waitingRecs == NULL );

        force = 0;  /* don't force definition of everything */
        CanTWalk( &force, typePass2 );
/**/    myassert( waitingRecs == NULL );

        force = 1;
        CanTWalk( &force, typePass2 );
/**/    myassert( waitingRecs == NULL );

        finishObjIO();
    }
}

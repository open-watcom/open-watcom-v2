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
#include <stdlib.h>
#include "womp.h"
#include "genutil.h"
#include "myassert.h"
#include "memutil.h"
#include "cantype.h"
#include "namemgr.h"
#include "array.h"

STATIC array_hdr    *cantArr;
STATIC type_handle  nextHdl;
STATIC type_handle  reUseHdl;

#define validHdl( hdl )         ( (hdl) < nextHdl )

void CanTInit( void ) {
/*******************/

    cantArr = ArrCreate( sizeof( cantype ), NULL );
    reUseHdl = CANT_NULL;
    nextHdl = CANT_NULL;
    CanTReserve();
}

STATIC int freeElm( cantype *node, void *parm ) {

    parm = parm;
    switch( node->class ) {
    case CANT_ENUM:
        if( node->d.enumr.consts != NULL ) {
            MemFree( node->d.enumr.consts );
        }
        break;
    case CANT_STRUCT:
        if( node->d.strct.fields != NULL ) {
            MemFree( node->d.strct.fields );
        }
        break;
    case CANT_PROCEDURE:
        if( node->d.proc.parms != NULL ) {
            MemFree( node->d.proc.parms );
        }
        break;
    case CANT_ARRAY_DESC:
        CanADestroyHdl( node->d.arrayd.bounds );
        break;
    case CANT_CHARBLOCK_IND:
        CanADestroyHdl( node->d.charbi.length );
        break;
    }
    return( 0 );
}

void CanTFini( void ) {
/*******************/
    ArrWalk( cantArr, NULL, freeElm );
    ArrDestroy( cantArr );
}

cantype *CanTFind( type_handle hdl ) {
/**********************************/
/**/myassert( validHdl( hdl ) );
    return( (cantype *)ArrAccess( cantArr, (size_t)hdl ) );
}

STATIC cantype *newNode( uint_8 class ) {

    cantype *type;

    if( reUseHdl != CANT_NULL ) {
        type = (cantype *)ArrAccess( cantArr, (size_t)reUseHdl );
        type->hdl = reUseHdl;
        reUseHdl = CANT_NULL;
    } else {
        type = (cantype *)ArrNewElm( cantArr, (size_t)nextHdl );
        type->hdl = nextHdl;
        ++nextHdl;
    }
/**/myassert( nextHdl != CANT_NULL );   /* FIXME overflow */
    type->class = class;
    return( type );
}

type_handle CanTReserve( void ) {
/*****************************/
    cantype *type;

    type = newNode( CANT_RESERVED );
    return( type->hdl );
}

void CanTReUse( type_handle hdl ) {
/*******************************/
/**/myassert( reUseHdl == CANT_NULL );
    reUseHdl = hdl;
}

#ifndef NDEBUG
STATIC int isOrdinalType( type_handle hdl ) {

    cantype *type;

/**/myassert( validHdl( hdl ) );
    if( hdl == CANT_NULL ) {
        return( 0 );
    }
    type = CanTFind( hdl );
/**/myassert( type != NULL );
    switch( type->class ) {
    case CANT_INTEGER:
    case CANT_SUBRANGE:
    case CANT_ENUM:
        return( 1 );
    }
    return( 0 );
}
#endif

STATIC type_handle newScalar( bitsize size, uint_8 class ) {

    cantype *type;

    type = newNode( class );
    type->size = size;
    return( type->hdl );
}

type_handle CanTInteger( bitsize size, int sgned ) {
/************************************************/

    cantype *type;

    type = newNode( CANT_INTEGER );
    type->size = size;
    type->sgned = sgned;
    return( type->hdl );
}

type_handle CanTReal( bitsize size ) {
/**********************************/

    return( newScalar( size, CANT_REAL ) );
}

type_handle CanTComplex( bitsize size ) {
/*************************************/

    return( newScalar( size, CANT_COMPLEX ) );
}

type_handle CanTVoid( void ) {
/**************************/

    return( newScalar( 0, CANT_VOID ) );
}

type_handle CanTTypeDef( type_handle base_type, name_handle name,
    uint_8 scope ) {
/****************************************************************/
    cantype *type;
    cantype *base;

/**/myassert( validHdl( base_type ) );

    type = newNode( CANT_TYPEDEF );
    type->d.typdef.type = base_type;
    type->d.typdef.name = name;
    type->d.typdef.scope = scope;
    base = CanTFind( base_type );
    type->size = base->size;
    return( type->hdl );
}

STATIC int_32 signExtend( bitsize size, uint_32 field ) {
/*
    Field is a signed number of size bits.  Return field sign extended to
    32 bits
*/
    uint_32 mask;

    switch( size ) {        /* these are the common cases */
    case  8:    return( (int_8)field );
    case 16:    return( (int_16)field );
    case 32:    return( (int_32)field );
    }
                            /* now handle special cases */
/**/myassert( size < 32 );
    mask = ~(0UL) << ( size - 1 );  /* turn on all sign bits */

    /* An assumption we will make for speed is that if any bit in field&mask
       is on then field is negative.  This assertion just ensures that this
       assumption is valid. */
/**/myassert( ( ( field & mask ) != 0 ) ==
        ( ( field & ( 1 << ( size - 1 ) ) ) != 0 ) );

    return( ( field & mask ) ?
        ( field | mask )
        : field );
}

type_handle CanTSubRange( type_handle base_type, uint_32 low, uint_32 high ) {
/**************************************************************************/
    cantype *type;
    cantype *base;

/**/myassert( validHdl( base_type ) );
    base = CanTFind( base_type );
/**/myassert(   base->class == CANT_INTEGER ||
                base->class == CANT_ENUM ||
                base->class == CANT_SUBRANGE );
    type = newNode( CANT_SUBRANGE );
    type->d.subrng.base_type = base_type;
    type->size = base->size;
    type->sgned = base->sgned;
    if( type->sgned ) {
        type->d.subrng.low = signExtend( type->size, low );
        type->d.subrng.high = signExtend( type->size, high );
    } else {
        type->d.subrng.low = low;
        type->d.subrng.high = high;
    }
    return( type->hdl );
}

STATIC unsigned log2( uint_32 num ) {

    unsigned log2;

    log2 = 0;
    for(;;) {
        num >>= 1;
        if( num == 0 ) break;
        ++log2;
    }
    return( log2 );
}

type_handle CanTArray( type_handle base_hdl, type_handle idx_hdl ) {
/****************************************************************/
    cantype *type;
    cantype *idx_type;
    uint_32 num_elm;
    bitsize base_size;
    cantype *base_type;

/**/myassert( validHdl( idx_hdl ) && isOrdinalType( idx_hdl ) &&
        validHdl( base_hdl ) );
    type = newNode( CANT_ARRAY );
    type->d.array.index_type = idx_hdl;
    type->d.array.base_type = base_hdl;
    idx_type = CanTFind( idx_hdl );
/**/myassert( idx_type != NULL );
    switch( idx_type->class ) {
    case CANT_INTEGER:
        num_elm = 1 << idx_type->size;
        break;
    case CANT_ENUM:
        {
            /* this assumes the constants have been sorted by CanTGraph() */
            enum_const  *low;
            enum_const  *high;

            low = idx_type->d.enumr.consts;
            high = low + idx_type->d.enumr.num_consts - 1;
            if( idx_type->sgned ) {
                num_elm = (int_32)high->value - (int_32)low->value;
            } else {
                num_elm = high->value - low->value;
            }
            ++num_elm;  /* add 1 */
        }
        break;
    case CANT_SUBRANGE:
        if( idx_type->sgned ) {
            num_elm = (int_32)( idx_type->d.subrng.high ) -
                        (int_32)( idx_type->d.subrng.low );
        } else {
            num_elm = ( idx_type->d.subrng.high ) -
                        ( idx_type->d.subrng.low );
        }
        ++num_elm;  /* add 1 */
        break;
    }
    base_type = CanTFind( base_hdl );
    base_size = base_type->size;
    /* FIXME - this is a bad heuristic! */
    if( log2( num_elm ) + log2( base_size ) >= 32 ) {
        Fatal( MSG_ARRAY_TOO_LARGE, num_elm, base_size );
    }
    type->size = num_elm * base_size;
    return( type->hdl );
}

type_handle CanTArrayZ( type_handle base_hdl, uint_32 high ) {
/***********************************************************/

    cantype *type;
    cantype *base_type;
    bitsize base_size;

/**/myassert( validHdl( base_hdl ) );
    type = newNode( CANT_ARRAY_ZERO );
    type->d.arrayz.base_type = base_hdl;
    type->d.arrayz.high = high;
    base_type = CanTFind( base_hdl );
    base_size = base_type->size;
    if( log2( high ) + log2( base_size ) >= 32 ) {
        Fatal( MSG_ARRAY_TOO_LARGE, high, base_size );
    }
    type->size = ( high + 1 ) * base_size;
    return( type->hdl );
}

type_handle CanTArrayD( type_handle base_type, type_handle lo,
    type_handle hi, addr_handle bounds ) {
/*************************************************************************/
    cantype *type;

/**/myassert(   validHdl( lo ) && isOrdinalType( lo ) &&
                validHdl( hi ) && isOrdinalType( hi ) &&
                validHdl( base_type ) );
    type = newNode( CANT_ARRAY_DESC );
    type->size = 0;         /* FIXME can we do better than this? */
    type->d.arrayd.lo_type = lo;
    type->d.arrayd.hi_type = hi;
    type->d.arrayd.bounds = bounds;
    type->d.arrayd.base_type = base_type;
    return( type->hdl );
}

type_handle CanTPointer( type_handle base_type, uint_8 class ) {
/************************************************************/
    cantype *type;

/**/myassert( validHdl( base_type ) );
    type = newNode( CANT_POINTER );
    if( class & ( CANT_PTR_FAR | CANT_PTR_HUGE ) ) {
        type->size = 4*8;
    } else {
        type->size = 2*8;
    }
    if( class & (CANT_PTR_386) ) {
        type->size += 2*8;
    }
    type->d.pointr.base_type = base_type;
    type->d.pointr.class = class;
    return( type->hdl );
}

cantype *CanTEnum( type_handle base_type, uint_16 num_consts ) {
/************************************************************/
    cantype *type;
    cantype *base;

/**/myassert( validHdl( base_type ) && num_consts > 0 );
    base = CanTFind( base_type );
/**/myassert( base->class == CANT_INTEGER || base->class == CANT_SUBRANGE );
    type = newNode( CANT_ENUM );
    type->size = base->size;
    type->sgned = base->sgned;
    type->d.enumr.base_type = base_type;
    type->d.enumr.num_consts = num_consts;
    if( num_consts > 0 ) {
        type->d.enumr.consts = MemAlloc( num_consts * sizeof( enum_const ) );
    } else {
        type->d.enumr.consts = NULL;
    }
    return( type );
}

cantype *CanTStruct( uint_16 num_fields ) {
/***************************************/
    cantype *type;

    type = newNode( CANT_STRUCT );
    /* type->size is determined later */
    type->d.strct.num_fields = num_fields;
    if( num_fields > 0 ) {
        type->d.strct.fields = MemAlloc( num_fields * sizeof( struct_field ) );
    } else {
        type->d.strct.fields = NULL;
    }
    return( type );
}

cantype *CanTProcedure( type_handle ret_type, uint_8 class,
    uint_8 num_parms ) {
/************************************************************/
    cantype *type;

/**/myassert( validHdl( ret_type ) );
    type = newNode( CANT_PROCEDURE );
    type->size = 0;
    type->d.proc.ret_type = ret_type;
    type->d.proc.class = class;
    type->d.proc.num_parms = num_parms;
    if( num_parms > 0 ) {
        type->d.proc.parms = MemAlloc( num_parms * sizeof( proc_parm ) );
    } else {
        type->d.proc.parms = NULL;
    }
    return( type );
}

STATIC int sortStructFields( const struct_field *f1, const struct_field *f2 ) {

    cantype *t1;
    cantype *t2;

/**/myassert( f1 != NULL && f2 != NULL );
    if( f1->bit_offset > f2->bit_offset ) {
        return( 1 );
    } else if( f1->bit_offset < f2->bit_offset ) {
        return( -1 );
    }
    t1 = CanTFind( f1->type );
    t2 = CanTFind( f2->type );
    if( t1->size > t2->size ) {
        return( 1 );
    } else if( t1->size < t2->size ) {
        return( -1 );
    }
    return( 0 );
}

STATIC void graphStruct( cantype *type ) {

    struct_field    *field;
    struct_field    *field_stop;
    uint_32         offset_size;
    bitsize         max_offset_size;
    cantype         *fieldptr;

/**/myassert( type != NULL && type->class == CANT_STRUCT );
/*
    We look for the max_offset_size here... we can't just use the last entry
    in the array because of unions.
*/
    max_offset_size = 0;
    field = type->d.strct.fields;
    field_stop = field + type->d.strct.num_fields;
    while( field < field_stop ) {
        fieldptr = CanTFind( field->type );
        offset_size = fieldptr->size;
        offset_size += field->bit_offset;
        if( offset_size > max_offset_size ) {
            max_offset_size = offset_size;
        }
        ++field;
    }
    type->size = max_offset_size;
    qsort( type->d.strct.fields, type->d.strct.num_fields,
            sizeof( struct_field ), sortStructFields );
}

STATIC int sortSignedEnum( const enum_const *c1, const enum_const *c2 ) {

    if( (int_32)c1->value > (int_32)c2->value ) {
        return( 1 );
    } else if( (int_32)c1->value == (int_32)c2->value ) {
        return( 0 );
    }
    return( -1 );
}

STATIC int sortUnsignedEnum( const enum_const *c1, const enum_const *c2 ) {

    if( (uint_32)c1->value > (uint_32)c2->value ) {
        return( 1 );
    } else if( (uint_32)c1->value == (uint_32)c2->value ) {
        return( 0 );
    }
    return( -1 );
}

STATIC void graphEnum( cantype *type ) {

    enum_const  *enum_c;
    enum_const  *enum_cstop;
    uint_16     num_consts;
    bitsize     size;

/**/myassert( type != NULL && type->class == CANT_ENUM );
    if( type->sgned ) {
        enum_c = type->d.enumr.consts;
        num_consts = type->d.enumr.num_consts;
        enum_cstop = enum_c + num_consts;
        size = type->size;
        while( enum_c < enum_cstop ) {
            enum_c->value = signExtend( size, enum_c->value );
            ++enum_c;
        }
    }
    enum_c = type->d.enumr.consts;
    num_consts = type->d.enumr.num_consts;
    qsort( enum_c, num_consts, sizeof( enum_const ),
        type->sgned ? sortSignedEnum : sortUnsignedEnum );
}

STATIC void graphProcedure( cantype *type ) {

    proc_parm       *parm;
    proc_parm       *parm_stop;

    if( type->d.proc.num_parms > 0 ) {
        parm = type->d.proc.parms;
        parm_stop = parm + type->d.proc.num_parms;
        while( parm < parm_stop ) {
            ++parm;
        }
    }
}

void CanTGraph( cantype *type ) {
/*****************************/

    switch( type->class ) {
    case CANT_STRUCT:       graphStruct( type );        break;
    case CANT_PROCEDURE:    graphProcedure( type );     break;
    case CANT_ENUM:         graphEnum( type );          break;
    default:
/**/    myassert( 0 );
    }
}

type_handle CanTCharB( uint_32 length ) {
/*************************************/
    cantype *type;

    type = newNode( CANT_CHARBLOCK );
    type->size = length * 8;
    type->d.charb.length = length;
    return( type->hdl );
}

type_handle CanTCharBI( type_handle length_type, addr_handle length ) {
/*******************************************************************/
    cantype *type;

/**/myassert( validHdl( length_type ) );
    type = newNode( CANT_CHARBLOCK_IND );
    type->size = 0;     /* FIXME can we do better than this? */
    type->d.charbi.length_type = length_type;
    type->d.charbi.length = length;
    return( type->hdl );
}

type_handle CanTDupSize( type_handle type_hdl, bitsize newsize ) {
/**************************************************************/
    cantype *type;
    cantype *newtype;

/**/myassert( validHdl( type_hdl ) );
    type = CanTFind( type_hdl );
/**/myassert( type != NULL );
    if( type->size == newsize ) {
        return( type_hdl );
    }
    newtype = newNode( type->class );
    newtype->size = newsize;
    newtype->busy = 0;
    newtype->d = type->d;
    return( newtype->hdl );
}

void CanTMunge( void ) {
/********************/
    /* Currently we don't need to do much here... it is all handled while
       building the graph. */
}

int CanTWalk( void *parm, int (*func)(cantype *type, void *parm ) ) {
/*****************************************************************/
    return( ArrWalk( cantArr, parm, func ) );
}

#if 0
cantype *CanTElimTypes( type_handle start_hdl, uint_16 elim ) {
/***********************************************************/
/*
    Chase a type handle as far back as we can eliminating certain types
    along the way.
*/
    type_handle walk;
    cantype     *type;

/**/myassert( validHdl( start_hdl ) );
    if( start_hdl == CANT_NULL ) {
        return( NULL );
    }

    walk = start_hdl;
    for(;;) {
        type = CanTFind( walk );
/**/    myassert( type != NULL );
        switch( type->class ) {
        case CANT_TYPEDEF:
            if( elim & CANT_ELIM_TYPEDEF ) {
                walk = type->d.typdef.type;
            } else {
                return( type );
            }
            break;
        case CANT_SUBRANGE:
            if( elim & CANT_ELIM_SUBRANGE ) {
                walk = type->d.subrng.base_type;
            } else {
                return( type );
            }
            break;
        case CANT_ENUM:
            if( elim & CANT_ELIM_ENUM ) {
                walk = type->d.enumr.base_type;
            } else {
                return( type );
            }
            break;
        default:
            return( type );
        }
    }
    return( NULL );
}
#endif

cantype *CanTElimTypeDef( type_handle start_hdl ) {
/***********************************************/
/*
    Chase a type handle as far back as we can eliminating typedefs
    along the way.
*/
    type_handle walk;
    cantype     *type;

/**/myassert( validHdl( start_hdl ) );
    if( start_hdl == CANT_NULL ) {
        return( NULL );
    }

    walk = start_hdl;
    for(;;) {
        type = CanTFind( walk );
        if( type == NULL ) break;
        if( type->class != CANT_TYPEDEF ) {
            return( type );
        }
        walk = type->d.typdef.type;
    }
    return( NULL );
}


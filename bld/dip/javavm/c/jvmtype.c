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


#include "jvmdip.h"
#include <ctype.h>
#include "java_lang_String.h"


walk_result DIPIMPENTRY( WalkTypeList )( imp_image_handle *iih,
                    imp_mod_handle imh, DIP_IMP_TYPE_WALKER *wk, imp_type_handle *ith,
                    void *d )
{
    ith->sig = iih->cc + offsetof( ClassClass, name );
    ith->kind = JT_RAWNAME;
    return( wk( iih, ith, d ) );
}

imp_mod_handle DIPIMPENTRY( TypeMod )( imp_image_handle *iih, imp_type_handle *ith )
{
    return( IMH_JAVA );
}

dip_status ImpInfoFromSig( ji_ptr sig, dig_type_info *ti )
{
    ti->kind = TM_NONE;
    ti->size = 0;
    switch( GetU8( sig ) ) {
    case SIGNATURE_ANY:
    case SIGNATURE_ENUM:
        ti->kind = TK_NONE;
        break;
    case SIGNATURE_ARRAY:
        ti->kind = TK_ARRAY;
        break;
    case SIGNATURE_BYTE:
        ti->size = 1;
        ti->kind = TK_INTEGER;
        ti->kind = TM_SIGNED;
        break;
    case SIGNATURE_CHAR:
        ti->size = 2;
        ti->kind = TK_CHAR;
        ti->kind = TM_UNSIGNED;
        break;
    case SIGNATURE_CLASS:
        ti->kind = TK_STRUCT;
        break;
    case SIGNATURE_FLOAT:
        ti->size = 4;
        ti->kind = TK_REAL;
        break;
    case SIGNATURE_DOUBLE:
        ti->size = 8;
        ti->kind = TK_REAL;
        break;
    case SIGNATURE_FUNC:
        ti->kind = TK_FUNCTION;
        break;
    case SIGNATURE_INT:
        ti->size = 4;
        ti->kind = TK_INTEGER;
        ti->kind = TM_SIGNED;
        break;
    case SIGNATURE_LONG:
        ti->size = 8;
        ti->kind = TK_INTEGER;
        ti->kind = TM_SIGNED;
        break;
    case SIGNATURE_SHORT:
        ti->size = 2;
        ti->kind = TK_INTEGER;
        ti->kind = TM_SIGNED;
        break;
    case SIGNATURE_VOID:
        ti->kind = TK_VOID;
        break;
    case SIGNATURE_BOOLEAN:
        ti->size = 1;
        ti->kind = TK_BOOL;
        break;
    default:
        return( DS_FAIL );
    }
    return( DS_OK );
}

static unsigned long ElementCount( ji_ptr off )
{
    off += offsetof( JHandle, methods );
    return( GetPointer( off ) >> METHOD_FLAG_BITS );
}

static unsigned ElementSize( ji_ptr sig )
{
    dig_type_info   ti;

    do {
    } while( isdigit( GetU8( ++sig ) ) );
    if( ImpInfoFromSig( sig, &ti ) != DS_OK )
        return( 0 );
    switch( ti.kind ) {
    case TK_ARRAY:
    case TK_STRUCT:
        return( sizeof( ji_ptr ) );
    }
    return( ti.size );
}

static dip_status ImpTypeInfo( imp_image_handle *iih, imp_type_handle *ith,
                        location_context *lc, dig_type_info *ti )
{
    ji_ptr                      clazz;
    ji_ptr                      name;
    dip_status                  ds;
    Classjava_lang_String       str;

    if( ith->kind == JT_WANTOBJECT ) {
        location_list   ll;

        ds = ImpSymLocation( iih, &ith->u.ish, lc, &ll, &ith->u.object );
        if( ds != DS_OK )
            return( ds );
        ith->kind = JT_SIGNATURE;
    }
    switch( ith->kind ) {
    case JT_INTEGER:
        ti->size = 4;
        ti->kind = TK_INTEGER;
        ti->kind = TM_SIGNED;
        return( DS_OK );
    case JT_SIGNATURE:
        ds = ImpInfoFromSig( ith->sig, ti );
        if( ds != DS_OK )
            return( ds );
        switch( ti->kind ) {
        case TK_STRUCT:
            name = ith->sig + 1;
            break;
        case TK_ARRAY:
            ti->size = ElementCount( ith->u.object ) * ElementSize( ith->sig );
            return( DS_OK );
        default:
            return( DS_OK );
        }
        break;
    case JT_RAWNAME:
        name = ith->sig;
        ti->kind = TK_STRUCT;
        ti->modifier = TM_NONE;
        ti->size = 0;
        break;
    default:
        return( DS_FAIL );
    }
    GetString( name, NameBuff, sizeof( NameBuff ) );
    if( memcmp( NameBuff, JAVA_STRING_NAME, sizeof( JAVA_STRING_NAME )-1 ) == 0 ) {
        ti->kind = TK_STRING;
        ti->modifier = TM_UNICODE;
        ds = GetData( ith->u.object, &str, sizeof( str ) );
        if( ds != DS_OK )
            return( ds );
        ti->size = str.count * sizeof( unicode );
    } else {
        clazz = GetClass( name );
        if( clazz != 0 ) {
            ti->size = GetU16( clazz + offsetof( ClassClass, instance_size ) );
        }
    }
    return( DS_OK );
}


dip_status DIPIMPENTRY( TypeInfo )( imp_image_handle *iih,
                imp_type_handle *ith, location_context *lc, dig_type_info *ti )
{
    return( ImpTypeInfo( iih, ith, lc, ti ) );

}

dip_status DIPIMPENTRY( TypeBase )( imp_image_handle *iih,
                        imp_type_handle *ith, imp_type_handle *base_ith,
                        location_context *lc, location_list *ll )
{
    char            *p;
    dip_status      ds;

    if( ith->kind == JT_WANTOBJECT ) {
        location_list   ll;

        ds = ImpSymLocation( iih, &ith->u.ish, lc, &ll, &ith->u.object );
        if( ds != DS_OK )
            return( ds );
        ith->kind = JT_SIGNATURE;
    }
    *base_ith = *ith;
    switch( base_ith->kind ) {
    case JT_SIGNATURE:
        switch( GetU8( base_ith->sig ) ) {
        case SIGNATURE_ARRAY:
            do {
            } while( isdigit( GetU8( ++base_ith->sig ) ) );
            if( ll != NULL ) {
                return( FollowObject( base_ith->sig, ll, &base_ith->u.object ) );
            }
            base_ith->u.object = 0;
            break;
        case SIGNATURE_FUNC:
            GetString( base_ith->sig, NameBuff, sizeof( NameBuff ) );
            p = strchr( NameBuff, SIGNATURE_ENDFUNC );
            if( p == NULL )
                p = &NameBuff[strlen( NameBuff ) - 1];
            base_ith->sig += (p - NameBuff) + 1;
            break;
        }
        break;
    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeArrayInfo )( imp_image_handle *iih,
                        imp_type_handle *ith, location_context *lc,
                        array_info *ai, imp_type_handle *index_ith )
{
    if( ith->kind == JT_WANTOBJECT ) {
        location_list   ll;
        dip_status      ds;

        ds = ImpSymLocation( iih, &ith->u.ish, lc, &ll, &ith->u.object );
        if( ds != DS_OK )
            return( ds );
        ith->kind = JT_SIGNATURE;
    }
    ai->stride = ElementSize( ith->sig );
    ai->num_elts = ElementCount( it->u.object );
    ai->low_bound = 0;
    ai->num_dims = 1;
    ai->column_major = 0; /* 1 for fortran */
    if( index_ith != NULL )
        index_ith->kind = JT_INTEGER;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeProcInfo )( imp_image_handle *iih,
                imp_type_handle *proc_ith, imp_type_handle *parm_ith, unsigned n )
{
    *parm_ith = *proc_ith;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypePtrAddrSpace )( imp_image_handle *iih,
                    imp_type_handle *ith, location_context *lc, address *a )
{
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( TypeThunkAdjust )( imp_image_handle *iih,
                        imp_type_handle *base_ith, imp_type_handle *derived_ith,
                        location_context *lc, address *addr )
{
    return( DS_OK );
}

int DIPIMPENTRY( TypeCmp )( imp_image_handle *iih, imp_type_handle *ith1, imp_type_handle *ith2 )
{
    if( ith1->sig < ith2->sig )
        return( -1 );
    if( ith1->sig > ith2->sig )
        return( 1 );
    return( 0 );
}

size_t DIPIMPENTRY( TypeName )( imp_image_handle *iih, imp_type_handle *ith,
                unsigned num, symbol_type *tag, char *buff, size_t buff_size )
{
    char        *p;
    size_t      len;

    *tag = ST_NONE;
    if( num != 0 )
        return( 0 );
    len = GetString( ith->sig, NameBuff, sizeof( NameBuff ) );
    switch( ith->kind ) {
    case JT_RAWNAME:
        NormalizeClassName( NameBuff, len );
        return( NameCopy( buff, NameBuff, buff_size, len ) );
    case JT_WANTOBJECT:
    case JT_SIGNATURE:
        if( NameBuff[0] != SIGNATURE_CLASS )
            return( 0 );
        p = strchr( NameBuff, SIGNATURE_ENDCLASS );
        if( p == NULL )
            p = &NameBuff[len];
        len = p - &NameBuff[1];
        NormalizeClassName( &NameBuff[1], len );
        return( NameCopy( buff, &NameBuff[1], buff_size, len ) );
    }
    return( 0 );
}

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


walk_result     DIGENTRY DIPImpWalkTypeList( imp_image_handle *ii,
                    imp_mod_handle im, IMP_TYPE_WKR *wk, imp_type_handle *it,
                    void *d )
{
    it->sig = ii->cc + offsetof( ClassClass, name );
    it->kind = JT_RAWNAME;
    return( wk( ii, it, d ) );
}

imp_mod_handle  DIGENTRY DIPImpTypeMod( imp_image_handle *ii,
                                imp_type_handle *it )
{
    return( IMH_JAVA );
}

dip_status ImpInfoFromSig( ji_ptr sig, dip_type_info *ti )
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
    dip_type_info   ti;

    do {
    } while( isdigit( GetU8( ++sig ) ) );
    if( ImpInfoFromSig( sig, &ti ) != DS_OK ) return( 0 );
    switch( ti.kind ) {
    case TK_ARRAY:
    case TK_STRUCT:
        return( sizeof( ji_ptr ) );
    }
    return( ti.size );
}

static dip_status ImpTypeInfo( imp_image_handle *ii, imp_type_handle *it,
                        location_context *lc, dip_type_info *ti )
{
    ji_ptr                      clazz;
    ji_ptr                      name;
    dip_status                  ds;
    Classjava_lang_String       str;

    if( it->kind == JT_WANTOBJECT ) {
        location_list   ll;
        dip_status      ds;

        ds = ImpSymLocation( ii, &it->u.is, lc, &ll, &it->u.object );
        if( ds != DS_OK ) return( ds );
        it->kind = JT_SIGNATURE;
    }
    switch( it->kind ) {
    case JT_INTEGER:
        ti->size = 4;
        ti->kind = TK_INTEGER;
        ti->kind = TM_SIGNED;
        return( DS_OK );
    case JT_SIGNATURE:
        ds = ImpInfoFromSig( it->sig, ti );
        if( ds != DS_OK ) return( ds );
        switch( ti->kind ) {
        case TK_STRUCT:
            name = it->sig + 1;
            break;
        case TK_ARRAY:
            ti->size = ElementCount( it->u.object ) * ElementSize( it->sig );
            return( DS_OK );
        default:
            return( DS_OK );
        }
        break;
    case JT_RAWNAME:
        name = it->sig;
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
        ds = GetData( it->u.object, &str, sizeof( str ) );
        if( ds != DS_OK ) return( ds );
        ti->size = str.count * sizeof( unicode );
    } else {
        clazz = GetClass( name );
        if( clazz != 0 ) {
            ti->size = GetU16( clazz + offsetof( ClassClass, instance_size ) );
        }
    }
    return( DS_OK );
}


dip_status      DIGENTRY DIPImpTypeInfo( imp_image_handle *ii,
                imp_type_handle *it, location_context *lc, dip_type_info *ti )
{
    return( ImpTypeInfo( ii, it, lc, ti ) );

}

dip_status      DIGENTRY DIPImpTypeBase( imp_image_handle *ii,
                        imp_type_handle *it, imp_type_handle *base,
                        location_context *lc, location_list *ll )
{
    char        *p;

    if( it->kind == JT_WANTOBJECT ) {
        location_list   ll;
        dip_status      ds;

        ds = ImpSymLocation( ii, &it->u.is, lc, &ll, &it->u.object );
        if( ds != DS_OK ) return( ds );
        it->kind = JT_SIGNATURE;
    }
    *base = *it;
    switch( base->kind ) {
    case JT_SIGNATURE:
        switch( GetU8( base->sig ) ) {
        case SIGNATURE_ARRAY:
            do {
            } while( isdigit( GetU8( ++base->sig ) ) );
            if( ll != NULL ) {
                return( FollowObject( base->sig, ll, &base->u.object ) );
            }
            base->u.object = 0;
            break;
        case SIGNATURE_FUNC:
            GetString( base->sig, NameBuff, sizeof( NameBuff ) );
            p = strchr( NameBuff, SIGNATURE_ENDFUNC );
            if( p == NULL ) p = &NameBuff[strlen(NameBuff)-1];
            base->sig += (p - NameBuff) + 1;
            break;
        }
        break;
    }
    return( DS_OK );
}

dip_status      DIGENTRY DIPImpTypeArrayInfo( imp_image_handle *ii,
                        imp_type_handle *it, location_context *lc,
                        array_info *ai, imp_type_handle *index )
{
    if( it->kind == JT_WANTOBJECT ) {
        location_list   ll;
        dip_status      ds;

        ds = ImpSymLocation( ii, &it->u.is, lc, &ll, &it->u.object );
        if( ds != DS_OK ) return( ds );
        it->kind = JT_SIGNATURE;
    }
    ai->stride = ElementSize( it->sig );
    ai->num_elts = ElementCount( it->u.object );
    ai->low_bound = 0;
    ai->num_dims = 1;
    ai->column_major = 0; /* 1 for fortran */
    if( index != NULL ) index->kind = JT_INTEGER;
    return( DS_FAIL );
}

dip_status      DIGENTRY DIPImpTypeProcInfo( imp_image_handle *ii,
                imp_type_handle *proc, imp_type_handle *parm, unsigned n )
{
    *parm = *proc;
    return( DS_FAIL );
}

dip_status      DIGENTRY DIPImpTypePtrAddrSpace( imp_image_handle *ii,
                    imp_type_handle *it, location_context *lc, address *a )
{
    return( DS_FAIL );
}

dip_status      DIGENTRY DIPImpTypeThunkAdjust( imp_image_handle *ii,
                        imp_type_handle *base, imp_type_handle *derived,
                        location_context *lc, address *addr )
{
    return( DS_OK );
}

int DIGENTRY DIPImpTypeCmp( imp_image_handle *ii, imp_type_handle *it1,
                                imp_type_handle *it2 )
{
    if( it1->sig < it2->sig ) return( -1 );
    if( it1->sig > it2->sig ) return( +1 );
    return( 0 );
}

unsigned DIGENTRY DIPImpTypeName( imp_image_handle *ii, imp_type_handle *it,
                unsigned num, symbol_type *tag, char *buff, unsigned buff_size )
{
    char        *p;
    unsigned    len;

    *tag = ST_NONE;
    if( num != 0 ) return( 0 );
    len = GetString( it->sig, NameBuff, sizeof( NameBuff ) );
    switch( it->kind ) {
    case JT_RAWNAME:
        NormalizeClassName( NameBuff, len );
        return( NameCopy( buff, NameBuff, buff_size, len ) );
    case JT_WANTOBJECT:
    case JT_SIGNATURE:
        if( NameBuff[0] != SIGNATURE_CLASS ) return( 0 );
        p = strchr( NameBuff, SIGNATURE_ENDCLASS );
        if( p == NULL ) p = &NameBuff[len];
        len = p - &NameBuff[1];
        NormalizeClassName( &NameBuff[1], len );
        return( NameCopy( buff, &NameBuff[1], buff_size, len ) );
    }
    return( 0 );
}

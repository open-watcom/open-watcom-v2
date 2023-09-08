/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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

#define OBJECT_SCOPE    (~0L)

typedef walk_result WALK_GLUE( imp_image_handle *, sym_walk_info, imp_sym_handle *, void * );

static walk_result WalkObject( imp_image_handle *iih, bool statics_only,
                ji_ptr clazz, WALK_GLUE *wk, imp_sym_handle *ish, void *d )
{
    ji_ptr      super;
    ji_ptr      super_name;
    ji_ptr      block;
    unsigned    num;
    unsigned    i;
    walk_result wr;
    unsigned    acc;

    super = GetPointer( clazz + offsetof( ClassClass, superclass ) );
    if( super != 0 ) {
        super = GetPointer( super + offsetof( JHandle, obj ) );
        ish->kind = JS_TYPE;
        ish->u.cn = super;
        if( iih->object_class == 0 ) {
            super_name = GetPointer( super + offsetof( ClassClass, name ) );
            GetString( super_name, NameBuff, sizeof( NameBuff ) );
            if( strcmp( NameBuff, JAVA_OBJECT_NAME ) == 0 ) {
                iih->object_class = super;
            }
        }
        /*
         * Don't bother walking java.lang.Object fields - nothing interesting
         */
        if( super != iih->object_class ) {
            wr = wk( iih, SWI_INHERIT_START, ish, d );
            if( wr == WR_CONTINUE ) {
                wr = WalkObject( iih, statics_only, super, wk, ish, d );
                if( wr != WR_CONTINUE )
                    return( wr );
                wk( iih, SWI_INHERIT_END, NULL, d );
            }
        }
    }
    /*
     * walk the fields
     */
    block = GetPointer( clazz + offsetof( ClassClass, fields ) );
    num = GetU16( clazz + offsetof( ClassClass, fields_count ) );
    ish->kind = JS_FIELD;
    for( i = 0; i < num; ++i, block += sizeof( struct fieldblock ) ) {
        if( statics_only ) {
            acc = GetU16( block + offsetof( struct fieldblock, access ) );
            if( !(acc & ACC_STATIC) ) {
                continue;
            }
        }
        ish->u.fb = block;
        wr = wk( iih, SWI_SYMBOL, ish, d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    /*
     * walk the methods
     */
    block = GetPointer( clazz + offsetof( ClassClass, methods ) );
    num = GetU16( clazz + offsetof( ClassClass, methods_count ) );
    ish->kind = JS_METHOD;
    for( i = 0; i < num; ++i ) {
        acc = GetU16( block + offsetof( struct methodblock, fb.access ) );
        if( !(acc & ACC_NATIVE) ) {
            /*
             * Don't bother telling debugger about native methods
             */
            ish->u.mb = block;
            wr = wk( iih, SWI_SYMBOL, ish, d );
            if( wr != WR_CONTINUE ) {
                return( wr );
            }
        }
        block += sizeof( struct methodblock );
    }
    return( WR_CONTINUE );
}

static walk_result WalkAScope( imp_image_handle *iih, unsigned mb_idx,
            scope_block *scope, WALK_GLUE *wk, imp_sym_handle *ish, void *d )
{
    ji_ptr              lv_tbl;
    unsigned            lv_num;
    unsigned            idx;
    struct localvar     var;
    ji_ptr              code_start;
    walk_result         wr;

    ish->kind = JS_LOCAL;
    code_start = (ji_ptr)iih->methods[mb_idx].code;
    lv_tbl = (ji_ptr)iih->methods[mb_idx].localvar_table;
    lv_num = iih->methods[mb_idx].localvar_table_length;
    for( idx = scope->unique; idx < lv_num; ++idx ) {
        ish->u.lv = lv_tbl + idx * sizeof( var );
        if( GetData( ish->u.lv, &var, sizeof( var ) ) != DS_OK ) {
            return( WR_FAIL );
        }
        if( var.length != scope->len || (code_start+var.pc0) != scope->start.mach.offset ) {
            return( WR_CONTINUE );
        }
        wr = wk( iih, SWI_SYMBOL, ish, d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    return( WR_CONTINUE );
}

static walk_result WalkAllScopes( imp_image_handle *iih, unsigned mb_idx,
            addr_off off, WALK_GLUE *wk, imp_sym_handle *ish, void *d )
{
    ji_ptr              lv_tbl;
    unsigned            lv_num;
    unsigned            idx;
    struct localvar     var;
    walk_result         wr;

    ish->kind = JS_LOCAL;
    off -= (ji_ptr)iih->methods[mb_idx].code;
    lv_tbl = (ji_ptr)iih->methods[mb_idx].localvar_table;
    lv_num = iih->methods[mb_idx].localvar_table_length;
    for( idx = 0; idx < lv_num; ++idx ) {
        ish->u.lv = lv_tbl + idx * sizeof( var );
        if( GetData( ish->u.lv, &var, sizeof( var ) ) != DS_OK ) {
            return( WR_FAIL );
        }
        if( (off >= var.pc0) && (off < (var.pc0+var.length)) ) {
            wr = wk( iih, SWI_SYMBOL, ish, d );
            if( wr != WR_CONTINUE ) {
                return( wr );
            }
        }
    }
    return( WalkObject( iih, FALSE, iih->cc, wk, ish, d ) );
}

struct walk_data {
    DIP_IMP_SYM_WALKER  *wk;
    void                *d;
};

static walk_result WalkSymGlue( imp_image_handle *iih, sym_walk_info swi,
                        imp_sym_handle *ish, void *d )
{
    struct walk_data    *wd = d;

    return( wd->wk( iih, swi, ish, wd->d ) );
}


walk_result DIPIMPENTRY( WalkSymList )( imp_image_handle *iih,
                symbol_source ss, void *source, DIP_IMP_SYM_WALKER *wk,
                imp_sym_handle *ish, void *d )
{
    scope_block         *scope;
    address             *a;
    walk_result         wr;
    unsigned            i;
    ji_ptr              clazz;
    imp_type_handle     *ith;
    struct walk_data    data;

    data.wk = wk;
    data.d  = d;
    switch( ss ) {
    case SS_BLOCK:
        scope = source;
        switch( FindMBIndex( iih, scope->start.mach.offset, &i ) ) {
        case SR_EXACT:
        case SR_CLOSEST:
            break;
        default:
            return( WR_CONTINUE );
        }
        if( scope->unique == OBJECT_SCOPE ) {
            wr = WalkObject( iih, FALSE, iih->cc, WalkSymGlue, ish, &data );
        } else {
            wr = WalkAScope( iih, i, scope, WalkSymGlue, ish, &data );
        }
        return( wr );
    case SS_MODULE:
       /*
        * no module scope symbols in Java
        */
       break;
    case SS_SCOPED:
        a = source;
        switch( FindMBIndex( iih, a->mach.offset, &i ) ) {
        case SR_EXACT:
        case SR_CLOSEST:
            break;
        default:
            return( WR_CONTINUE );
        }
        wr = WalkAllScopes( iih, i, a->mach.offset, WalkSymGlue, ish, &data );
        break;
    case SS_TYPE:
        ith = (imp_type_handle)source;
        switch( ith->kind ) {
        case JT_RAWNAME:
            clazz = GetClass( ith->sig );
            break;
        case JT_WANTOBJECT:
        case JT_SIGNATURE:
            if( GetU8( ith->sig ) != SIGNATURE_CLASS )
                return( WR_CONTINUE );
            clazz = GetClass( ith->sig + 1 );
            break;
        default:
            return( WR_CONTINUE );
        }
        if( clazz != 0 ) {
            return( WalkObject( iih, FALSE, clazz, WalkSymGlue, ish, &data ) );
        }
        break;
    }
    return( WR_CONTINUE );
}

imp_mod_handle DIPIMPENTRY( SymMod )( imp_image_handle *iih,
                        imp_sym_handle *ish )
{
    return( IMH_JAVA );
}

static unsigned GetName( imp_image_handle *iih, imp_sym_handle *ish )
{
    ji_ptr      name;
    unsigned    cp_idx;

    switch( ish->kind ) {
    case JS_METHOD:
        name = GetPointer( ish->u.mb + offsetof( struct methodblock, fb.name ) );
        break;
    case JS_FIELD:
        name = GetPointer( ish->u.fb + offsetof( struct fieldblock, name ) );
        break;
    case JS_LOCAL:
        cp_idx = GetU16( ish->u.lv + offsetof( struct localvar, nameoff ) );
        name = GetPointer( iih->cp + cp_idx * sizeof( union cp_item_type ) );
        break;
    case JS_TYPE:
    case JS_PACKAGE:
        name = GetPointer( ish->u.cn + offsetof( ClassClass, name ) );
        break;
    }
    return( GetString( name, NameBuff, sizeof( NameBuff ) ) );
}

static ji_ptr GetSignature( imp_image_handle *iih, imp_sym_handle *ish )
{
    ji_ptr      sig;
    unsigned    cp_idx;

    sig = 0;
    switch( ish->kind ) {
    case JS_METHOD:
        sig = GetPointer( ish->u.mb + offsetof( struct methodblock, fb.signature ) );
        break;
    case JS_FIELD:
        sig = GetPointer( ish->u.fb + offsetof( struct fieldblock, signature ) );
        break;
    case JS_LOCAL:
        cp_idx = GetU16( ish->u.lv + offsetof( struct localvar, sigoff ) );
        sig = GetPointer( iih->cp + cp_idx * sizeof( union cp_item_type ) );
        break;
    case JS_TYPE:
    case JS_PACKAGE:
        sig = GetPointer( ish->u.cn + offsetof( ClassClass, name ) );
        break;
    }
    return( sig );
}

static unsigned Insert( char *name, unsigned *add, unsigned *len, char *str )
{
    unsigned    str_len;
    unsigned    addv;

    addv = *add;
    str_len = strlen( str );
    memmove( &name[addv+str_len], &name[addv], *len - addv );
    memcpy( &name[addv], str, str_len );
    *len += str_len;
    return( str_len );
}

static char *DoDemangle( char *name, unsigned *add, unsigned *len, char *sig )
{
    char        *p;
    unsigned    first;

    switch( *sig++ ) {
    case SIGNATURE_ARRAY:
        Insert( name, add, len, " []" );
        while( isdigit( *sig ) )
            ++sig;
        sig = DoDemangle( name, add, len, sig );
        break;
    case SIGNATURE_CLASS:
        p = strchr( sig, SIGNATURE_ENDCLASS );
        *p = '\0';
        Insert( name, add, len, sig );
        sig = p + 1;
        break;
    case SIGNATURE_FUNC:
        *add += Insert( name, add, len, "(" );
        first = 1;
        for( ;; ) {
            if( *sig == SIGNATURE_ENDFUNC )
                break;
            if( !first ) {
                *add += Insert( name, add, len, "," );
            }
            sig = DoDemangle( name, add, len, sig );
            *add = *len;
            first = 0;
        }
        Insert( name, add, len, ")" );
        *add = 0;
        Insert( name, add, len, " " );
        sig = DoDemangle( name, add, len, sig+1 );
        *add = *len;
        break;
    case SIGNATURE_BYTE:
        *add += Insert( name, add, len, "byte" );
        break;
    case SIGNATURE_CHAR:
        *add += Insert( name, add, len, "char" );
        break;
    case SIGNATURE_FLOAT:
        *add += Insert( name, add, len, "float" );
        break;
    case SIGNATURE_DOUBLE:
        *add += Insert( name, add, len, "double" );
        break;
    case SIGNATURE_INT:
        *add += Insert( name, add, len, "int" );
        break;
    case SIGNATURE_LONG:
        *add += Insert( name, add, len, "long" );
        break;
    case SIGNATURE_SHORT:
        *add += Insert( name, add, len, "short" );
        break;
    case SIGNATURE_VOID:
        *add += Insert( name, add, len, "void" );
        break;
    case SIGNATURE_BOOLEAN:
        *add += Insert( name, add, len, "boolean" );
        break;
    }
    return( sig );
}

static unsigned Demangle( char *name, unsigned len, ji_ptr sig_ptr )
{
    char        sig[MAX_NAME];
    unsigned    add;

    GetString( sig_ptr, sig, sizeof( sig ) );
    add = len;
    DoDemangle( name, &add, &len, sig );
    return( len );
}

size_t DIPIMPENTRY( SymName )( imp_image_handle *iih, imp_sym_handle *ish,
    location_context *lc, symbol_name_type snt, char *buff, size_t buff_size )
{
    size_t      len;
    ji_ptr      sig;

    len = GetName( iih, ish );
    switch( ish->kind ) {
    case JS_METHOD:
        if( snt == SNT_DEMANGLED ) {
            sig = GetSignature( iih, ish );
            if( sig != 0 ) {
                len = Demangle( NameBuff, len, sig );
            }
        }
        break;
    case JS_TYPE:
    case JS_PACKAGE:
        NormalizeClassName( NameBuff, len );
        break;
    }
    return( NameCopy( buff, NameBuff, buff_size, len ) );
}

dip_status DIPIMPENTRY( SymType )( imp_image_handle *iih,
                imp_sym_handle *ish, imp_type_handle *ith )
{
    ith->sig = GetSignature( iih, ish );
    switch( ish->kind ) {
    case JS_TYPE:
    case JS_PACKAGE:
        ith->kind = JT_RAWNAME;
        return( DS_OK );
    }
    switch( GetU8( ith->sig ) ) {
    case SIGNATURE_ARRAY:
    case SIGNATURE_CLASS:
        ith->u.ish = *ish;
        ith->kind = JT_WANTOBJECT;
        break;
    default:
        ith->kind = JT_SIGNATURE;
        break;
    }

    return( DS_OK );
}

dip_status FollowObject( ji_ptr sig, location_list *ll, ji_ptr *handle )
{
    location_list               var;
    unsigned_32                 off;
    dip_status                  ds;
    unsigned                    len;
    Classjava_lang_String       str;

    /*
     * follow the object pointer(s)
     */
    if( sig == 0 )
        return( DS_OK );
    len = GetString( sig, NameBuff, sizeof( NameBuff ) );
    if( len == 0 )
        return( DS_ERR | DS_FAIL );
    switch( NameBuff[0] ) {
    case SIGNATURE_CLASS:
    case SIGNATURE_ARRAY:
        LocationCreate( &var, LT_INTERNAL, &off );
        ds = DCAssignLocation( &var, ll, sizeof( off ) );
        if( ds != DS_OK )
            return( ds );
        ll->e[0].u.addr.mach.offset = GetPointer( off + offsetof( JHandle, obj ) );
        *handle = off;
        break;
    default:
        *handle = 0;
        break;
    }
    if( NameBuff[0] == SIGNATURE_CLASS
     && memcmp( &NameBuff[1], JAVA_STRING_NAME, sizeof( JAVA_STRING_NAME ) - 1 ) == 0 ) {
        /*
         * Got a string, get pointer to actual character storage
         */
        *handle = ll->e[0].u.addr.mach.offset;
        ds = GetData( ll->e[0].u.addr.mach.offset, &str, sizeof( str ) );
        if( ds != DS_OK )
            return( ds );
        ll->e[0].u.addr.mach.offset = GetPointer( (ji_ptr)str.value + offsetof( JHandle, obj ) )
                                        + str.offset * sizeof( unicode );
    }
    return( DS_OK );
}

dip_status ImpSymLocation( imp_image_handle *iih, imp_sym_handle *ish,
                location_context *lc, location_list *ll, ji_ptr *obj_handle )
{
    address             a;
    dip_status          ds;
    location_list       var;
    unsigned            acc;
    unsigned            cp_idx;
    unsigned_32         off;
    ji_ptr              sig;

    switch( ish->kind ) {
    case JS_METHOD:
        sig = 0;
        acc = GetU16( ish->u.mb + offsetof( struct methodblock, fb.access ) );
        if( acc & ACC_NATIVE )
            return( DS_ERR | DS_BAD_LOCATION );
        a = DefCodeAddr;
        a.mach.offset = GetPointer( ish->u.mb + offsetof( struct methodblock, code ) );
        LocationCreate( ll, LT_ADDR, &a );
        break;
    case JS_FIELD:
        sig = GetPointer( ish->u.fb + offsetof( struct fieldblock, signature ) );
        acc = GetU16( ish->u.fb + offsetof( struct fieldblock, access ) );
        if( acc & ACC_STATIC ) {
            a = DefDataAddr;
            switch( GetU8( sig ) ) {
            case SIGNATURE_LONG:
            case SIGNATURE_DOUBLE:
                a.mach.offset = GetPointer( ish->u.fb
                            + offsetof( struct fieldblock, u.static_address ) );
                break;
            default:
                a.mach.offset = ish->u.fb
                            + offsetof( struct fieldblock, u.static_value );
                break;
            }
            LocationCreate( ll, LT_ADDR, &a );
        } else {
            ds = DCItemLocation( lc, CI_OBJECT, ll );
            if( ds != DS_OK )
                return( ds );
            off = GetU32( ish->u.fb + offsetof( struct fieldblock, u.offset ) );
            LocationAdd( ll, off * 8 );
        }
        break;
    case JS_LOCAL:
        ds = DCItemLocation( lc, CI_JVM_vars, &var );
        if( ds != DS_OK )
            return( ds );
        a = DefDataAddr;
        LocationCreate( ll, LT_INTERNAL, &a.mach.offset );
        ds = DCAssignLocation( ll, &var, sizeof( a.mach.offset ) );
        if( ds != DS_OK )
            return( ds );
        a.mach.offset += GetU32( ish->u.lv + offsetof( struct localvar, slot ) )
                                * sizeof( unsigned_32 );
        LocationCreate( ll, LT_ADDR, &a );
        cp_idx = GetU16( ish->u.lv + offsetof( struct localvar, sigoff ) );
        sig = GetPointer( iih->cp + cp_idx * sizeof( union cp_item_type ) );
        break;
    case JS_TYPE:
    case JS_PACKAGE:
        return( DS_ERR | DS_BAD_LOCATION );
    }
    *obj_handle = 0;
    return( FollowObject( sig, ll, obj_handle ) );
}

dip_status DIPIMPENTRY( SymLocation )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, location_list *ll )
{
    ji_ptr      handle;

    return( ImpSymLocation( iih, ish, lc, ll, &handle ) );
}

dip_status DIPIMPENTRY( SymValue )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, void *buff )
{
    return( DS_FAIL );
}

//NYI: sym_info fields not set
//    unsigned          is_global               : 1;
//    unsigned          compiler                : 1;
//    /* only valid for SK_PROCEDURE */
//    unsigned          rtn_calloc              : 1;
//    unsigned          ret_modifier            : 3;
//    unsigned          ret_size                : 4;
//    unsigned short    num_parms;
//    addr_off          ret_addr_offset;
//    addr_off          prolog_size;
//    addr_off          epilog_size;

dip_status DIPIMPENTRY( SymInfo )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, sym_info *si )
{
    unsigned    acc;

    memset( si, 0, sizeof( si ) );
    switch( ish->kind ) {
    case JS_METHOD:
        si->kind = SK_PROCEDURE;
        si->is_member = 1;
        si->rtn_size = GetU32( ish->u.mb + offsetof( struct methodblock, code_length ) );
        break;
    case JS_FIELD:
        si->kind = SK_DATA;
        si->is_member = 1;
        break;
    case JS_LOCAL:
        si->kind = SK_DATA;
        return( DS_OK );
    case JS_TYPE:
        si->kind = SK_TYPE;
        return( DS_OK );
    case JS_PACKAGE:
        si->kind = SK_NAMESPACE;
        return( DS_OK );
    }
    acc = GetU16( ish->u.fb + offsetof( struct fieldblock, access ) );
    if( acc & ACC_PUBLIC )
        si->is_public = 1;
    if( acc & ACC_PRIVATE )
        si->is_private = 1;
    if( acc & ACC_PROTECTED )
        si->is_protected = 1;
    if( acc & ACC_STATIC )
        si->is_static = 1;
    return( DS_OK );
}

dip_status DIPIMPENTRY( SymParmLocation )( imp_image_handle *iih,
                    imp_sym_handle *ish, location_context *lc,
                    location_list *ll, unsigned n )
{
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymObjType )( imp_image_handle *iih,
                    imp_sym_handle *ish, imp_type_handle *ith, dig_type_info *ti )
{
    struct methodblock  method;
    dip_status          ds;

    if( ish->kind != JS_METHOD )
        return( DS_FAIL );
    ds = GetData( ish->u.mb, &method, sizeof( method ) );
    if( ds != DS_OK )
        return( ds );
    if( method.fb.access & ACC_NATIVE )
        return( DS_FAIL );
    if( ti != NULL ) {
        if( method.fb.access & ACC_STATIC ) {
            ti->kind = TK_NONE;
            ti->size = 0;
        } else {
            ti->kind = TK_POINTER;
            ti->size = sizeof( ji_ptr );
        }
        ti->modifier = TM_NONE;
        ti->deref = false;
    }
    ith->sig = GetPointer( GetPointer( (ji_ptr)method.fb.clazz ) + offsetof( ClassClass, name ) );
    ith->kind = JT_RAWNAME;
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymObjLocation )( imp_image_handle *iih, imp_sym_handle *ish,
                                            location_context *lc, location_list *ll )
{
    struct methodblock  method;
    dip_status          ds;
    unsigned            i;
    imp_sym_handle      this_ish;
    ji_ptr              cp;
    ji_ptr              name;
    unsigned            cp_idx;
    ji_ptr              handle;

    if( ish->kind != JS_METHOD )
        return( DS_FAIL );
    ds = GetData( ish->u.mb, &method, sizeof( method ) );
    if( ds != DS_OK )
        return( ds );
    if( method.fb.access & (ACC_NATIVE | ACC_STATIC) )
        return( DS_FAIL );
    cp = GetPointer( (ji_ptr)method.fb.clazz + offsetof( ClassClass, constantpool ) );
    this_ish.kind = JS_LOCAL;
    this_ish.u.lv = (ji_ptr)method.localvar_table;
    /*
     * guessing "this" pointer is near start of local var list
     */
    for( i = 0; i < method.localvar_table_length; ++i ) {
        cp_idx = GetU16( this_ish.u.lv + offsetof( struct localvar, nameoff ) );
        name = GetPointer( cp + cp_idx * sizeof( union cp_item_type ) );
        GetString( name, NameBuff, sizeof( NameBuff ) );
        if( strcmp( NameBuff, "this" ) == 0 ) {
            return( ImpSymLocation( iih, &this_ish, lc, ll, &handle ) );
        }
        this_ish.u.lv += sizeof( struct localvar );
    }
    return( DS_FAIL );
}

search_result DIPIMPENTRY( AddrSym )( imp_image_handle *iih,
                            imp_mod_handle imh, address a, imp_sym_handle *ish )
{
    search_result       sr;
    unsigned            i;

    sr = FindMBIndex( iih, a.mach.offset, &i );
    switch( sr ) {
    case SR_EXACT:
    case SR_CLOSEST:
        ish->u.mb = IDX_TO_METHOD_BLOCK( iih, i );
        ish->kind = JS_METHOD;
        break;
    }
    return( sr );
}

struct lookup_data {
    lookup_item         *li;
    strcompn_fn         *scompn;
    void                *d;
    char                *unmatched;
    search_result       sr;
    int                 static_only;
};

static walk_result CheckOneSym( imp_image_handle *iih, sym_walk_info swi,
                        imp_sym_handle *ish, void *d )
{
    struct lookup_data  *ld = d;
    imp_sym_handle      *new;
    size_t              len;

    if( swi != SWI_SYMBOL ) {
        return( ld->static_only ? WR_STOP : WR_CONTINUE );
    }
    len = GetName( iih, ish );
    if( ld->li->name.len != len )
        return( WR_CONTINUE );
    if( ld->cmp( NameBuff, ld->li->name.start, len ) != 0 )
        return( WR_CONTINUE );
    new = DCSymCreate( iih, ld->d );
    if( new == NULL )
        return( WR_FAIL );
    *new = *ish;
    ld->sr = SR_EXACT;
    return( WR_CONTINUE );
}

static search_result CheckScopeName( struct lookup_data *ld )
{
    char        scope[MAX_NAME];
    size_t      pk_len;
    size_t      i;
    size_t      j;
    enum {
        SEP_NO,
        SEP_MAYBE,
        SEP_YES
    }           need_sep;
    int         have_multi;
    char        c;

    have_multi = 0;
    need_sep = SEP_NO;
    j = 0;
    for( i = 0; i < ld->li->scope.len; ++i ) {
        c = ld->li->scope.start[i];
        if( isalnum( c ) || c == '_' ) {
            if( need_sep == SEP_YES ) {
                have_multi = 1;
                scope[j++] = '/';
            }
            scope[j++] = c;
            need_sep = SEP_MAYBE;
        } else if( need_sep == SEP_MAYBE ) {
            need_sep = SEP_YES;
        }
    }
    pk_len = strlen( NameBuff );
    if( j > pk_len )
        return( SR_NONE );
    if( j == 0 ) {
        switch( NameBuff[0] ) {
        case '\0':
            return( SR_EXACT );
        default:
            ld->unmatched = NameBuff;
            return( SR_CLOSEST );
        }
    } else if( ld->cmp( NameBuff, scope, j ) == 0 ) {
        switch( NameBuff[j] ) {
        case '/':
            ld->unmatched = &NameBuff[j+1];
            return( SR_CLOSEST );
        case '\0':
            return( SR_EXACT );
        }
    }
    if( have_multi )
        return( SR_NONE );
    /*
     * Try the last element of the package name
     */
    i = pk_len - j;
    if( i == 0 )
        return( SR_NONE );
    if( ld->cmp( &NameBuff[i], scope, j ) != 0 )
        return( SR_NONE );
    if( NameBuff[i-1] != '/' )
        return( SR_NONE );
    return( SR_EXACT );
}

search_result DIPIMPENTRY( LookupSym )( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    struct lookup_data  data;
    unsigned            i;
    imp_sym_handle      ish;
    address             *a;
    imp_type_handle     *ith;
    ji_ptr              clazz;
    imp_sym_handle      *new;
    search_result       sr;
    size_t              len;
    char                *p;

    if( MH2IMH( li->mod ) != IMH_NOMOD && MH2IMH( li->mod ) != IMH_JAVA ) {
        return( SR_NONE );
    }
    data.li = li;
    data.d = d;
    data.sr = SR_NONE;
    data.scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    data.static_only = FALSE;
    len = GetString( GetPointer( iih->cc + offsetof( ClassClass, name ) ), NameBuff, sizeof( NameBuff ) );
    p = strrchr( NameBuff, '/' );
    if( p == NULL ) {
        memmove( &NameBuff[1], &NameBuff[0], len + 1 );
        p = NameBuff;
        sr = SR_EXACT;
    } else {
        sr = SR_CLOSEST;
    }
    *p++ = '\0';        /* Split the package name from the type name */
    data.unmatched = NameBuff;
    sr = CheckScopeName( &data );
    switch( sr ) {
    case SR_EXACT:
    case SR_CLOSEST:
        break;
    default:
        return( sr );
    }
    if( li->type == ST_NAMESPACE ) {
        if( sr != SR_CLOSEST )
            return( SR_NONE );
        len = strlen( data.unmatched );
        if( len < li->name.len )
            return( SR_NONE );
        if( data.cmp( li->name.start, data.unmatched, li->name.len ) != 0 )
            return( SR_NONE );
        switch( data.unmatched[li->name.len] ) {
        case '/':
        case '\0':
            break;
        default:
            return( SR_NONE );
        }
        new = DCSymCreate( iih, data.d );
        if( new == NULL )
            return( SR_FAIL );
        new->kind = JS_PACKAGE;
        new->u.pk = iih->cc;
        return( SR_EXACT );
    }
    if( sr != SR_EXACT )
        return( SR_NONE );
    switch( ss ) {
    case SS_MODULE:
        data.static_only = 1;
//        return( SR_NONE );
        /* fall through */
    case SS_SCOPED:
        switch( li->type ) {
        case ST_NONE:
            break;
        case ST_TYPE:
            len = strlen( p );
            if( len != li->name.len )
                return( SR_NONE );
            if( data.cmp( li->name.start, p, len ) != 0 )
                return( SR_NONE );
            new = DCSymCreate( iih, data.d );
            if( new == NULL )
                return( SR_FAIL );
            new->kind = JS_TYPE;
            new->u.cn = iih->cc;
            return( SR_EXACT );
        default:
            return( SR_NONE );
        }
        if( ss == SS_MODULE ) {
            if( WalkObject( iih, TRUE, iih->cc, CheckOneSym, &ish, &data ) == WR_FAIL ) {
                return( SR_FAIL );
            }
        } else {
            a = source;
            switch( FindMBIndex( iih, a->mach.offset, &i ) ) {
            case SR_EXACT:
            case SR_CLOSEST:
                break;
            case SR_NONE:
#if 1
                data.static_only = 1;
                if( WalkObject( iih, TRUE, iih->cc, CheckOneSym, &ish, &data ) == WR_FAIL ) {
                    return( SR_FAIL );
                }
                return( data.sr );
#else
return( SR_NONE );
#endif
            default:
                return( SR_FAIL );
            }
            if( WalkAllScopes( iih, i, a->mach.offset, CheckOneSym, &ish, &data ) == WR_FAIL ) {
                return( SR_FAIL );
            }
        }
        return( data.sr );
    case SS_TYPE:
        if( li->type != ST_NONE )
            return( SR_NONE );
        ith = (imp_type_handle)source;
        switch( ith->kind ) {
        case JT_RAWNAME:
            clazz = GetClass( ith->sig );
            break;
        case JT_WANTOBJECT:
        case JT_SIGNATURE:
            if( GetU8( ith->sig ) != SIGNATURE_CLASS )
                return( SR_NONE );
            clazz = GetClass( ith->sig + 1 );
            break;
        default:
            return( SR_NONE );
        }
        if( clazz == 0 )
            return( SR_NONE );
        if( WalkObject( iih, FALSE, clazz, CheckOneSym, &ish, &data ) == WR_FAIL ) {
            return( SR_FAIL );
        }
        return( data.sr );
    }
    return( SR_NONE );
}

#define LV_CACHE_SIZE   50

static search_result FindAScope( imp_image_handle *iih, scope_block *scope )
{
    search_result       sr;
    unsigned            i;
    unsigned            mb_idx;
    ji_ptr              lv_tbl;
    unsigned            lv_num;
    unsigned            get;
    struct localvar     cache[LV_CACHE_SIZE];
    struct {
        ji_ptr          pc;
        unsigned long   len;
        unsigned        idx;
    }                   best;
    unsigned            idx;
    addr_off            off;

    sr = FindMBIndex( iih, scope->start.mach.offset, &mb_idx );
    switch( sr ) {
    case SR_EXACT:
    case SR_CLOSEST:
        break;
    default:
        return( sr );
    }
    off = scope->start.mach.offset - (ji_ptr)iih->methods[mb_idx].code;
    best.pc = 0L;
    best.len = ~0L;
    best.idx = ~0;
    lv_tbl = (ji_ptr)iih->methods[mb_idx].localvar_table;
    lv_num = iih->methods[mb_idx].localvar_table_length;
    for( idx = 0; idx < lv_num; idx += get ) {
        get = lv_num - idx;
        if( get > LV_CACHE_SIZE )
            get = LV_CACHE_SIZE;
        if( GetData( lv_tbl, cache, get * sizeof( cache[0] ) ) != DS_OK ) {
            return( SR_NONE );
        }
        for( i = 0; i < get; ++i ) {
            if( (off >= cache[i].pc0)
             && (off < (cache[i].pc0 + cache[i].length))
             && (scope->unique > (idx+i)) ) {
                /*
                 * in range & not the same as a previous scope
                 */
                if( (cache[i].pc0 > best.pc)
                 || (cache[i].pc0 == best.pc && cache[i].length < best.len) ) {
                    /*
                     * better than best
                     */
                    best.pc = cache[i].pc0;
                    best.len = cache[i].length;
                    best.idx = idx + i;
                }
            }
        }
        lv_tbl += get * sizeof( cache[0] );
    }
    scope->start.mach.offset = (ji_ptr)iih->methods[mb_idx].code;
    if( best.idx == ~0 ) {
        scope->len = iih->methods[mb_idx].code_length;
        scope->unique = OBJECT_SCOPE;
        return( (off == 0) ? SR_EXACT : SR_CLOSEST );
    }
    scope->start.mach.offset += best.pc;
    scope->len = best.len;
    scope->unique = best.idx;
    return( (best.pc == off) ? SR_EXACT : SR_CLOSEST );
}

search_result DIPIMPENTRY( AddrScope )( imp_image_handle *iih,
                imp_mod_handle imh, address a, scope_block *scope )
{

    scope->start = a;
    scope->unique = OBJECT_SCOPE;
    return( FindAScope( iih, scope ) );
}

search_result DIPIMPENTRY( ScopeOuter )( imp_image_handle *iih,
                imp_mod_handle imh, scope_block *in, scope_block *out )
{
    if( in->unique == OBJECT_SCOPE )
        return( SR_NONE );
    *out = *in;
    return( FindAScope( iih, out ) );
}

int DIPIMPENTRY( SymCmp )( imp_image_handle *iih, imp_sym_handle *ish1, imp_sym_handle *ish2 )
{
    if( ish1->u.fb < ish2->u.fb )
        return( -1 );
    if( ish1->u.fb > ish2->u.fb )
        return( 1 );
    return( 0 );
}

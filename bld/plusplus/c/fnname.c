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

#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>

#include "ppintnam.h"
#include "memmgr.h"
#include "class.h"
#include "ppops.h"
#include "vbuf.h"
#include "specname.h"
#include "name.h"
#include "toggle.h"
#include "cgfront.h"
#include "fmttype.h"
#include "initdefs.h"
#include "preproc.h"
#include "pcheader.h"
#include "stats.h"
#include "srcfile.h"
#include "pragdefn.h"

#ifdef ZZ_LEN_3
#define ZZ_LEN_LIMIT    (36 * 36 * 36)
#else
#define ZZ_LEN_LIMIT    (36 * 36)
#endif

#define IsInternalName(n) nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM )

typedef enum {                  // type mangling control
    TM_INCLUDE_FIRST_DIM= 0x01, // - include first dimension in array type mangling
    TM_NO_INITIAL_MOD   = 0x02, // - don't include initial mods
    TM_FIRST_DIM        = 0x80, // * local flag used in appendTypeContinue
    TM_NULL             = 0x00
} tm_control;

static char *operatorNamesStr[] = {
    #include "ppopfstr.h"
};
#define MAX_OP_NAMES ARRAY_SIZE( operatorNamesStr )

static CGOP const operatorSameAs[] = {
    #define PPOPOP( s, sa )     delim CO_##sa
    #include "ppopsdef.h"
};

static char *specialNamesStr[] = {
    #define SPECNAME_DEFINE
    #include "specname.h"
};
#define MAX_SPECIAL_NAMES ARRAY_SIZE( specialNamesStr )

static VBUF mangled_name;       // buffer for working on name
static char *objNameBuff;
static NAME specialNames[MAX_SPECIAL_NAMES];
static NAME operatorNames[MAX_OP_NAMES + 1]; // one more for sentinel

#define MAX_REPLICATE   10
typedef struct replicate_desc {
    const char      *ptr;
    size_t          len;
} replicate_desc;
static replicate_desc replicate[MAX_REPLICATE];
static int            next_replicate;

ExtraRptCtr( ctr_lookups );
ExtraRptCtr( ctr_lookups_slow );
ExtraRptCtr( ctr_debug_names );
ExtraRptCtr( ctr_debug_scoped_names );

static void replicateInit( void )
{
    next_replicate = 0;
}

static int replicateSearch( const char *name )
{
    int i;
    int len = strlen( name );

#if 0   // fix assertion failures next release
    // demangler will not insert these names into its replicate table
    DbgAssert( name[0] != '$' );
#endif
    for( i = 0 ; i < next_replicate ; i++ ) {
         if( len == replicate[i].len ) {
             if( strcmp( name, replicate[i].ptr ) == 0 ) return( i );
         }
    }
    if( i < MAX_REPLICATE ) {
        replicate[i].len = len;
        replicate[i].ptr = name;
        next_replicate++;
    }
    return( -1 );
}

static uint_32 objNameHash( uint_32 h, char *s )
{
    uint_32 c;
    uint_32 g;

    // don't change this in a patch
    for(;;) {
        /* ( h & ~0x0ffffff ) == 0 is always true here */
        c = *s;
        if( c == 0 ) break;
        h = (h << 4) + c;
        g = h & ~0x0ffffff;
        h ^= g;
        h ^= g >> 24;
        ++s;
    }
    return( h );
}

static TYPE typeLookAhead( TYPE type, type_flag *flags, void **base )
{
    return TypeModExtract( type
                         , flags
                         , base
                         , TC1_NOT_MEM_MODEL | TC1_NOT_ENUM_CHAR );
}

static bool nameHasPrefix(      // TEST IF NAME HAS A PREFIX
    const NAME name,            // - name to be tested
    const char *prefix )        // - prefix
{
    bool retn = TRUE;           // - TRUE ==> has prefix
    const char *sname = NameStr( name );

    for( ; *prefix != '\0'; ++sname, ++prefix ) {
        if( *prefix != *sname ) {
            retn = FALSE;
            break;
        }
    }
    return( retn );
}

static void appendChar(         // APPEND A CHARACTER
    char chr )                  // - the character
{
    VbufConcChr( &mangled_name, chr );
}


static void appendStr(          // APPEND A STRING
    const char *str )           // - the string
{
    if( str != NULL ) {
        VbufConcStr( &mangled_name, str );
    }
}

static void prependStr(         // PREPEND A STRING
    const char *str )           // - the string
{
    if( str != NULL ) {
        VbufPrepStr( &mangled_name, str );
    }
}


static char __Alphabet36[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char *utoa_zz( unsigned value, char *buffer )
{
    char        *p = buffer;
#ifdef ZZ_LEN_3
    unsigned    rem;

    rem = value % 36;
    value = value / 36;
#endif
    *p++ = __Alphabet36[ value / 36 ];
    *p++ = __Alphabet36[ value % 36 ];
#ifdef ZZ_LEN_3
    *p++ = __Alphabet36[ rem ];
#endif
    *p = '\0';
    return( buffer );
}

static char *xtoa( unsigned value, char *p, unsigned base )
{
    unsigned    rem;

    rem = value % base;
    value = value / base;
    if( value != 0 ) {
        p = xtoa( value, p, base );
    }
    *p++ = __Alphabet36[rem];
    return( p );
}

static char *my_ultoa( unsigned value, char *buffer, unsigned base )
{
    char        *p = buffer;

    if( value == 0 ) {
        *p++ = '0';
    } else {
        p = xtoa( value, p, base );
    }
    *p++ = '\0';
    return( buffer );
}

static void appendStrWithLen(   // APPEND A STRING FOR LENGTH CHARS
    char *str,                  // - the string
    unsigned len )              // - how many chars to concatenate
{
    while( len != 0 ) {
        appendChar( *str );
        ++str;
        --len;
    }
}

static void appendInt(          // APPEND AN INTEGER
    int val )                   // - value
{
    VbufConcDecimal( &mangled_name, val );
}

static void appendBase36UInt(   // APPEND A BASE 36 INTEGER
    unsigned val )              // - value
{
    char sbuf[16];              // - buffer

    my_ultoa( val, sbuf, 36 );
    appendStr( sbuf );
}

static void appendReplName(     // APPEND A REPLICATIBLE NAME
    const char *id )            // - the name
{
    int index;

    index = replicateSearch( id );
    if( index >= 0 ) {
        appendChar( index + '0' );
    } else {
        appendStr( id );
        appendStr( IN_NAME_SUFFIX );
    }
}

static void appendNameSpaceName(// APPEND A NAMESPACE NAME
    SCOPE scope )               // - the scope
{
    NAME name;

    name = ScopeNameSpaceName( scope );
    if( name != NULL ) {
        if( ScopeIsUnnamedNameSpace( scope ) != NULL ) {
            // we don't want unnamed namespace name in replicate table
            // (demangler never stores anything with '$' prefix in rep table)
            appendChar( IN_CLASS_DELIM );
            appendStr( NameStr( name ) );
            appendStr( IN_NAME_SUFFIX );
        } else {
            appendChar( IN_CLASS_DELIM );
            appendReplName( NameStr( name ) );
        }
    }
}

static void appendSymName(      // APPEND A SYMBOL's MANGLED NAME
    SYMBOL sym )                // - the symbol
{
    NAME    name;

    name = sym->name->name;
    if( nameHasPrefix( name, IN_OP_PREFIX ) ) {
        appendStr( IN_NAME_PREFIX );
        appendStr( &( NameStr( name )[sizeof( IN_NAME_PREFIX )] ) );
    } else {
        appendReplName( NameStr( name ) );
    }
}

static void appendScopeMangling(// APPEND CLASS SCOPES
    SCOPE scope );              // - current scope

static void appendScopedSymName(// APPEND A SCOPED SYMBOL NAME
    SYMBOL sym )
{
    appendSymName( sym );
    appendScopeMangling( sym->name->containing );
}

static NAME className(          // GET CLASS' NAME
    TYPE class_type )           // - the class type
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    if( info->name == NULL ) {
        ClassMakeUniqueName( class_type, NULL );
    }
    return( info->name );
}

static void appendClassName(    // APPEND A CLASS' NAME
    TYPE class_type )           // - the class type
{
    appendReplName( NameStr( className( class_type ) ) );
}

static void appendBasedMod(     // APPEND A BASED MODIFIER
    type_flag flags,            // - the flags
    void *base )                // - base modifier
{
    STRING_CONSTANT str;

    appendChar( IN_BASED );
    flags &= TF1_BASED;
    switch( flags ) {
    case TF1_BASED_SELF:
        appendChar( IN_BASED_SELF );
        break;
    case TF1_BASED_VOID:
        appendChar( IN_BASED_VOID );
        break;
    case TF1_BASED_STRING:
#ifndef NDEBUG
        if( base == NULL ) {
            CFatal( "invalid based modifier" );
        }
#endif
        appendChar( IN_BASED_STRING );
        str = base;
        if( str->len < ZZ_LEN_LIMIT ) {
            char    sbuf[10];

            utoa_zz( str->len, sbuf );
            appendStr( sbuf );
        } else {
            char    sbuf[60];

            sprintf( sbuf, "internal name length=%d is > %d (1)", str->len, ZZ_LEN_LIMIT );
            CFatal( sbuf );
        }
        appendStrWithLen( str->string, str->len );
        break;
    case TF1_BASED_FETCH:
#ifndef NDEBUG
        if( base == NULL ) {
            CFatal( "invalid based modifier" );
        }
#endif
        appendChar( IN_BASED_FETCH );
        appendStr( IN_NAME_PREFIX );
        appendScopedSymName( base );
        break;
    case TF1_BASED_ADD:
#ifndef NDEBUG
        if( base == NULL ) {
            CFatal( "invalid based modifier" );
        }
#endif
        appendChar( IN_BASED_ADD );
        appendStr( IN_NAME_PREFIX );
        appendScopedSymName( base );
        break;
#ifndef NDEBUG
    default:
        CFatal( "invalid based modifier" );
#endif
    }
}

static void appendModifier(     // APPEND A MODIFIER
    type_flag flags,            // - the flags
    void *base )                // - base modifier
{
    if( flags & TF1_BASED ) {
        appendBasedMod( flags, base );
    }
    if( flags & TF1_FAR ) {
        appendChar( IN_FAR );
    }
    if( flags & TF1_FAR16 ) {
        appendChar( IN_FAR16 );
    }
    if( flags & TF1_HUGE ) {
        appendChar( IN_HUGE );
    }
    if( flags & TF1_NEAR ) {
        appendChar( IN_NEAR );
    }
    if( flags & TF1_VOLATILE ) {
        appendChar( IN_VOLATILE );
    }
    if( flags & TF1_CONST ) {
        appendChar( IN_CONST );
    }
}

static void appendTypeFlags(    // APPEND TYPE FLAGS
    type_flag flags )           // - the flags
{
    appendModifier( flags, NULL );
}

static void appendTypeContinue( // APPEND A TYPE MANGLING (NO NEAR/FAR PREFIX)
    TYPE type,                  // - type
    NAME name,                  // - original name of symbol
    tm_control control )        // - control mask
{
    TYPE *aptr;                 // - arg.s structure
    unsigned acount;            // - arguments count
    type_flag flags;            // - look ahead flags
    void *base;                 // - look ahead base

    control |= TM_FIRST_DIM;
    while( type != NULL ) {
        type = BoundTemplateClass( type );
        switch( type->id ) {
        case TYP_BOOL:
            appendChar( IN_BOOL );
            type = TypeError;
            break;
        case TYP_CHAR:
            appendChar( IN_CHAR );
            type = TypeError;
            break;
        case TYP_SCHAR:
            appendChar( IN_SCHAR );
            break;
        case TYP_UCHAR:
            appendChar( IN_UNSIGNED );
            appendChar( IN_UCHAR );
            break;
        case TYP_WCHAR:
            appendChar( IN_WCHAR );
            break;
        case TYP_SSHORT:
            appendChar( IN_SSHORT );
            break;
        case TYP_USHORT:
            appendChar( IN_UNSIGNED );
            appendChar( IN_USHORT );
            break;
        case TYP_SINT:
            appendChar( IN_SINT );
            break;
        case TYP_UINT:
            appendChar( IN_UNSIGNED );
            appendChar( IN_UINT );
            break;
        case TYP_SLONG:
            appendChar( IN_SLONG );
            break;
        case TYP_ULONG:
            appendChar( IN_UNSIGNED );
            appendChar( IN_ULONG );
            break;
        case TYP_SLONG64:
            appendChar( IN_SLONG64 );
            break;
        case TYP_ULONG64:
            appendChar( IN_UNSIGNED );
            appendChar( IN_ULONG64 );
            break;
        case TYP_FLOAT:
            appendChar( IN_FLOAT );
            break;
        case TYP_DOUBLE:
            appendChar( IN_DOUBLE );
            break;
        case TYP_LONG_DOUBLE:
            appendChar( IN_LONG_DOUBLE );
            break;
        case TYP_POINTER:
            control &= ~TM_FIRST_DIM;
            if( type->flag & TF1_REFERENCE ) {
                appendChar( IN_REFER );
            } else {
                appendChar( IN_POINTER );
            }
            type = typeLookAhead( type->of, &flags, &base );
            appendModifier( flags, base );
            continue;
        case TYP_TYPEDEF:
            /* typedefs are synonyms for other types (ignore them) */
            break;
        case TYP_ENUM:
            appendStr( IN_NAME_PREFIX );
            name = SimpleTypeName( type );
            if( name != NULL ) {
                appendReplName( NameStr( name ) );
            } else {
                name = AnonymousEnumExtraName( type );
                appendStr( IN_NAME_PREFIX IN_ANON_ENUM );
                if( name != NULL ) {
                    appendStr( NameStr( name ) );
                }
                appendStr( IN_NAME_SUFFIX );
            }
            appendScopeMangling( type->u.t.scope );
            appendStr( IN_NAME_SUFFIX );
            // terminate mangle for this type
            name = NULL;
            type = TypeError;
            break;
        case TYP_CLASS:
            appendStr( IN_NAME_PREFIX );
            appendClassName( type );
            appendScopeMangling( type->u.c.scope->enclosing );
            appendStr( IN_NAME_SUFFIX );
            break;
        case TYP_FUNCTION:
            flags = type->u.f.args->qualifier;
            if( flags != TF1_NULL ) {
                appendChar( IN_THIS_QUAL );
                appendTypeFlags( flags );
            }
            appendChar( IN_FUNCTION );
            acount = type->u.f.args->num_args;
            aptr = type->u.f.args->type_list;
            for( ; acount > 0; --acount ) {
                appendTypeContinue( *aptr++, NULL, TM_NULL );
            }
            appendChar( IN_FUNCTION_END );
            if( name == CppConstructorName() || name == CppDestructorName() ) {
                appendChar( IN_NO_TYPE );
                name = NULL;
                type = TypeError;
            }
            break;
        case TYP_ARRAY:
            appendChar( IN_ARRAY );
            if( control & TM_FIRST_DIM ) {
                control &= ~TM_FIRST_DIM;
                if( control & TM_INCLUDE_FIRST_DIM ) {
                    appendInt( type->u.a.array_size );
                }
            } else {
                appendInt( type->u.a.array_size );
            }
            appendChar( IN_ARRAY_END );
            break;
        case TYP_DOT_DOT_DOT:
            appendChar( IN_DOT_DOT_DOT );
            break;
        case TYP_VOID:
            appendChar( IN_VOID );
            break;
        case TYP_MODIFIER:
            appendModifier( type->flag, type->u.m.base );
            break;
        case TYP_MEMBER_POINTER:
            control &= ~TM_FIRST_DIM;
            appendChar( IN_MEMBER );
            appendTypeContinue( MemberPtrClass( type ), NULL, TM_NULL );
            type = typeLookAhead( type->of, &flags, &base );
            appendModifier( flags, base );
            continue;
        }
        type = type->of;
    }
}


static void appendType(         // APPEND A TYPE MANGLING
    TYPE type,                  // - type
    NAME name,                  // - original name of symbol
    tm_control control )        // - control mask
{
    type_flag flags;            // - look ahead flags
    void *base;                 // - look ahead base

    type = typeLookAhead( type, &flags, &base );
    if(( control & TM_NO_INITIAL_MOD ) == 0 ) {
        appendModifier( flags, base );
    }
    appendTypeContinue( type, name, control );
}

static void appendTypedSymName( // APPEND A SYMBOL's TYPED MANGLED NAME
    SYMBOL sym )                // - the symbol
{
    appendSymName( sym );
    appendType( sym->sym_type, sym->name->name, TM_NULL );
}

static void appendFullSymName(  // APPEND A SYMBOL's FULL MANGLED NAME
    SYMBOL sym )                // - the symbol
{
    appendSymName( sym );
    appendScopeMangling( sym->name->containing );
    appendType( sym->sym_type, sym->name->name, TM_NULL );
}

static void appendBase32UInt(   // CONCATENATE A BASE 32 TARGET UNSIGNED LONG
    target_ulong number )       // - the number
{
    char buff[16];

    my_ultoa( number, buff, 32 );
    appendStr( buff );
}

static void appendDelta(        // CONCATENATE A DELTA OPERATION
    target_offset_t offset )    // - the offset
{
    char buff[16];

    buff[0] = 'O';
    my_ultoa( offset, buff + 1, 31 );
    appendStr( buff );
}

static void appendTemplateParm( // APPEND A TEMPLATE PARM
    SYMBOL sym )
{
    target_long val;
    char delim;

    if( SymIsConstantInt( sym ) ) {
        val = sym->u.sval;
        appendChar( IN_TEMPARG_INT );
        delim = IN_TEMPARG_POSITIVE_INT;
        if( val < 0 ) {
            delim = IN_TEMPARG_NEGATIVE_INT;
            val = -val;
        }
        appendBase32UInt( val );
        appendChar( delim );
    } else if( SymIsTypedef( sym ) ) {
        appendChar( IN_TEMPARG_TYPE );
        appendType( sym->sym_type, sym->name->name, TM_INCLUDE_FIRST_DIM );
    } else {
        sym = SymAddressOf( sym );
        if( sym != NULL ) {
            appendStr( IN_MANGLE2 );
            appendFullSymName( sym );
        }
    }
}

static void appendScopeMangling(// APPEND CLASS SCOPES
    SCOPE scope )               // - current scope
{
    SCOPE next;
    SYMBOL curr;
    SYMBOL stop;
    SYMBOL fn_symbol;
    char buff[1 + sizeof( unsigned ) * 2 + 1];

    for(;;) {
        if( scope == NULL ) break;
        switch( ScopeId( scope ) ) {
        case SCOPE_FILE:
            appendNameSpaceName( scope );
            break;
        case SCOPE_CLASS:
            appendChar( IN_CLASS_DELIM );
            appendClassName( ScopeClass( scope ) );
            break;
        case SCOPE_FUNCTION:
            fn_symbol = ScopeFunction( scope );
            DbgAssert( fn_symbol != NULL );
            appendChar( IN_CLASS_DELIM );
            appendStr( IN_MANGLE2 );
            appendTypedSymName( fn_symbol );
            break;
        case SCOPE_BLOCK:
            buff[0] = '.';
            my_ultoa( ScopeIndex( scope ), buff + 1, 31 );
            appendChar( IN_CLASS_DELIM );
            appendStr( buff );
            appendStr( IN_NAME_SUFFIX );
            for(;;) {
                next = scope->enclosing;
                if( ScopeId( next ) != SCOPE_BLOCK ) break;
                scope = next;
                DbgAssert( ScopeId( scope ) == SCOPE_BLOCK );
            }
            break;
        case SCOPE_TEMPLATE_PARM:
            curr = NULL;
            stop = ScopeOrderedStart( scope );
            appendChar( IN_CLASS_DELIM );
            appendChar( IN_CLASS_DELIM );
            for(;;) {
                curr = ScopeOrderedNext( stop, curr );
                if( curr == NULL ) break;
                appendTemplateParm( curr );
            }
            break;
        }
        scope = scope->enclosing;
    }
}

static char *formatGlobalName(  // GET C++ GLOBAL NAME
    SYMBOL sym )                // - symbol
{
    replicateInit();
    VbufRewind( &mangled_name );
    appendStr( IN_MANGLE );
    appendFullSymName( sym );
    return( VbufString( &mangled_name ) );
}


static char *cppFunctionName(   // GET C++ FUNCTION NAME
    SYMBOL sym )                // - function symbol
{
    return( formatGlobalName( sym ) );
}


static char *cppDataName(       // GET C++ DATA NAME
    SYMBOL sym )                // - function symbol
{
    return( formatGlobalName( sym ) );
}

bool CppLookupOperatorName(     // FIND OPERATOR FOR NAME (FALSE IF NOT FOUND)
    NAME name,                  // - name to find
    CGOP *oper )                // - index found
{
    CGOP index;
#ifndef NDEBUG
    CGOP i;
#endif

    ExtraRptIncrementCtr( ctr_lookups );
    if( name != NULL ) {
        if( nameHasPrefix( name, IN_OP_PREFIX ) ) {
            // name is an operator
            ExtraRptIncrementCtr( ctr_lookups_slow );
            index = (CGOP)( NameHash( name ) - NameHash( operatorNames[ 0 ] ) );
#ifndef NDEBUG
            operatorNames[ MAX_OP_NAMES ] = name;
            i = 0;
            for(;;) {
                if( operatorNames[ i ] == name ) break;
                ++i;
                if( operatorNames[ i ] == name ) break;
                ++i;
            }
            DbgAssert( i != MAX_OP_NAMES );
    #if 0   // should be useless
            if( index == MAX_OP_NAMES ) {
                // found the sentinel
                return( FALSE );
            }
    #endif
            DbgAssert( i == index );
#endif
            *oper = index;
            return( TRUE );
        }
    }
    return( FALSE );
}

NAME CppOperatorName(       // GET FUNCTION NAME FOR AN OPERATOR
    CGOP oper )             // - operator
{
    DbgVerify( oper < MAX_OP_NAMES, "Illegal operator name" );
    return( operatorNames[oper] );
}

NAME CppConversionName(         // GET CONVERSION NAME
    void )
{
    return( operatorNames[CO_CONVERT] );
}

NAME CppDestructorName(         // CREATE NAME OF DESTRUCTOR
    void )
{
    return( operatorNames[CO_DTOR] );
}

NAME CppConstructorName(        // CREATE NAME OF CONSTRUCTOR
    void )
{
    return( operatorNames[CO_CTOR] );
}

NAME CppSpecialName(            // CREATE NAME OF SPECIAL INTERNAL ID
    unsigned index )            // - index of special name
{
    return( specialNames[index] );
}

bool IsCppSpecialName(          // TEST IF NAME IS SPECIAL NAME
    NAME name,                  // - name to find
    unsigned *idx )             // - index found
{
    unsigned index;

    for( index = 0; index < MAX_SPECIAL_NAMES; index++ ) {
        if( specialNames[index] == name ) {
            *idx = index;
            return( TRUE );
        }
    }
    return( FALSE );
}

static void setPrefix(          // SET A PREFIX FOR NAME
    char *last )                // - last segment of prefix
{
    replicateInit();
    VbufRewind( &mangled_name );
    appendStr( IN_NAME_PREFIX IN_SYM_WATCOM );
    appendStr( last );
}

static char* setMangling(       // SET FOR MANGLING
    char *last )                // - last segment of prefix
{
    char *save;                 // - saved name

    setPrefix( last );
    save = strsave( VbufString( &mangled_name ) );
    VbufRewind( &mangled_name );
    return save;
}

static NAME retMangling(        // RETURN MANGLED NAME
    char* save )                // - saved name
{
    unsigned    len;

    len = VbufLen( &mangled_name );
    if( len < ZZ_LEN_LIMIT ) {
        char    sbuf[10];

        utoa_zz( len, sbuf );
        prependStr( sbuf );
    } else {
        char    sbuf[60];

        sprintf( sbuf, "internal name length=%d is > %d (2)", len, ZZ_LEN_LIMIT );
        CFatal( sbuf );
    }
    prependStr( save );
    CMemFree( save );
    return( NameCreateLen( VbufString( &mangled_name ), VbufLen( &mangled_name ) ) );
}

static NAME mangledNameType(    // MAKE MANGLED NAME, TYPE
    TYPE type,                  // - type used
    char *last )                // - last segment of prefix
{
    char *save;                 // - saved name

    save = setMangling( last );
    appendType( type, NULL, TM_NULL );
    return( retMangling( save ) );
}

static NAME tableName( SCOPE scope, target_offset_t delta, char *table_type )
{
    char *save;

    save = setMangling( table_type );
    appendDelta( delta );
    appendScopeMangling( scope );
    return( retMangling( save ) );
}

NAME CppTypeidName(             // CREATE NAME FOR TYPEID
    unsigned *len,              // - addr( len of type mangle )
    TYPE type )                 // - type for typeid
{
    char *save;

    save = setMangling( IN_TYPEID_NAME );
    appendType( type, NULL, TM_NO_INITIAL_MOD );
    *len = VbufLen( &mangled_name );
    return( retMangling( save ) );
}

char *CppGetTypeidContents(     // CREATE CONTENTS FOR TYPEID STRUCTURE
    TYPE type,                  // - typeid type
    unsigned *len )             // - addr( strlen of contents )
{
    VbufRewind( &mangled_name );
    appendType( type, NULL, TM_NO_INITIAL_MOD );
    *len = VbufLen( &mangled_name );
    return( VbufString( &mangled_name ) );
}

NAME CppVATableName(            // CREATE NAME OF VIRTUAL FN ADJUSTOR TABLE
    SCOPE scope )               // - class table is used in
{
    return( tableName( scope, 0, IN_VATABLE_NAME ) );
}

NAME CppVBTableName(            // CREATE NAME OF VIRTUAL BASE OFFSET TABLE
    SCOPE scope,                // - class table is used in
    target_offset_t delta )     // - offset of vbptr table is used for
{
    return( tableName( scope, delta, IN_VBTABLE_NAME ) );
}

NAME CppVFTableName(            // CREATE NAME OF VIRTUAL FN ADDR TABLE
    SCOPE scope,                // - class table is used in
    target_offset_t delta )     // - offset of vfptr table is used for
{
    return( tableName( scope, delta, IN_VFTABLE_NAME ) );
}

bool IsVftName(                 // TEST IF SYMBOL IS VFT NAME
    NAME name )                 // - name to be tested
{
    return( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_VFTABLE_NAME ) );
}

NAME CppThunkName(              // CREATE NAME OF VIRTUAL FN THUNK
    SCOPE scope,                // - exact class that needs vftables
    THUNK_ACTION *thunk )       // - thunk being created
{
    char    *save;
    SYMBOL  sym;

    save = setMangling( IN_VFTHUNK_NAME );
    appendStr( NameStr( thunk->sym->name->name ) );
    appendScopeMangling( scope );
    if( thunk->delta ) {
        appendDelta( thunk->delta );
    }
    if( thunk->ctor_disp ) {
        appendChar( 'C' );
        if( thunk->input_virtual ) {
            appendChar( 'I' );
            appendDelta( thunk->in.vb_offset );
            appendDelta( thunk->in.vb_index );
        }
        if( thunk->in.delta ) {
            appendDelta( thunk->in.delta );
        }
    }
    appendStr( IN_NAME_PREFIX );
    sym = thunk->override;
    appendScopeMangling( SymScope( sym ) );
    appendTypedSymName( sym );
    return( retMangling( save ) );
}

NAME CppIndexMappingName(       // CREATE NAME OF VIRTUAL BASE INDEX MAPPING
    SCOPE from,                 // - from scope
    SCOPE to )                  // - to scope
{
    char *save;

    save = setMangling( IN_VMTABLE_NAME );
    appendScopeMangling( from );
    appendStr( IN_NAME_PREFIX );
    appendScopeMangling( to );
    return( retMangling( save ) );
}


NAME CppMembPtrOffsetName(      // CREATE NAME FOR MEMBER-PTR OFFSET FUNCTION
    SYMBOL sym )                // - field for offsetting
{
    char *save;

    save = setMangling( IN_NAME_MEMPTR_FUNC );
    appendTypedSymName( sym );
    return( retMangling( save ) );
}


bool IsCppMembPtrOffsetName(    // TEST IF NAME IS MEMBER-PTR OFFSET FUNCTION
    NAME name )                 // - name of field for offsetting
{
    return( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_MEMPTR_FUNC ) );
}


NAME CppStaticInitName(         // CREATE NAME FOR INITIALIZING AUTO AGGREGATES
    SYMBOL sym )                // - variable to be initialized
{
    char *save;

    save = setMangling( IN_NAME_STATIC_INIT );
    appendTypedSymName( sym );
    return( retMangling( save ) );
}


NAME CppStaticOnceOnlyName(     // CREATE NAME FOR ONCE ONLY CHECK OF STATICS
    void )
{
    setPrefix( IN_NAME_STATIC_ONCE "00" );
    return( NameCreateLen( VbufString( &mangled_name ), VbufLen( &mangled_name ) ) );
}


NAME CppArrayDtorName(          // CREATE NAME FOR ARRAY DTOR
    TYPE ar_type )              // - array type
{
    char *save;

    save = setMangling( IN_NAME_ARRAY_DTOR );
    ar_type = ArrayType( ar_type );
    appendInt( ArrayTypeNumberItems( ar_type ) );
    return( retMangling( save ) );
}

NAME CppNameTypeSig(            // NAME OF TYPE SIGNATURE
    TYPE type )                 // - the type
{
    char *save;

    save = setMangling( IN_NAME_TYPE_SIG );
    if( type == NULL ) {
        appendStr( IN_TYPE_SIG_ELLIPSIS );
    } else {
        appendType( type, NULL, TM_NULL );
    }
    return( retMangling( save ) );
}

NAME CppNameUniqueNS(           // NAME OF UNIQUE NAMESPACE
    TOKEN_LOCN *locn )          // - location of start of unique namespace
{
    SRCFILE src;
    SRCFILE primary;
    char *save;
    uint_32 h;

    // this can change in a patch since it is an externally unique name
    save = setMangling( IN_NAME_UNNAMED );
    appendBase36UInt( NameNextDummyIndex() );
    appendChar( '_' );
    // we now have enough to ensure an unique internal name
    // but we have to try to create an unique external name...
    h = TimeOfCompilation();
    appendBase36UInt( h );
    src = locn->src_file;
    h ^= SrcFileTimeStamp( src );
    h ^= locn->line;
    h ^= locn->column;
    primary = SrcFileEnclosingPrimary( src );
    if( primary != src ) {
        h ^= SrcFileTimeStamp( primary );
    }
    h = objNameHash( h, SrcFileFullName( primary ) );
    appendBase36UInt( h );
    return( retMangling( save ) );
}

NAME CppNameStateTableCmd(      // NAME OF STATE-TABLE COMMAND
    unsigned index )            // - command index
{
    char *save;

    save = setMangling( IN_NAME_STAB_CMD );
    appendInt( index );
    return( retMangling( save ) );
}

NAME CppNameThrowRo(            // NAME OF THROW R/O BLOCK
    TYPE type )                 // - the type
{
    return( mangledNameType( type, IN_NAME_THROW_RO ) );
}

static char *cppNameCgop(       // PRODUCE PRINTABLE OPERATOR NAME
    CGOP oper )                 // - operator
{
    char *name;                 // - name

    static char *opNames[] ={   // - opcode long names
        #include "ppopslnm.h"
    };

    if( oper >= ( sizeof( opNames ) / sizeof( opNames[0] ) ) ) {
        name = "***INVALID CGOP***";
    } else {
        name = opNames[oper];
    }
    return( name );
}

bool IsCppNameInterestingDebug( // CHECK FOR INTERNAL NAMES
    SYMBOL sym )                // - symbol
{
    NAME name;

    name = sym->name->name;
    if( IsInternalName( name ) ) {
        // internal symbol names
        return( FALSE );
    }
    if( NameStr( name )[0] == NAME_OPERATOR_OR_DUMMY_PREFIX1 ) {
        if( name == specialNames[ SPECIAL_RETURN_VALUE ] ) {
            // special case for ".return"
            return( TRUE );
        }
        if( NameStr( name )[1] == NAME_OPERATOR_PREFIX2 ) {
            // operator function names
            return( TRUE );
        }
        return( FALSE );
    }
    return( TRUE );
}


char *CppNameDebug(             // TRANSLATE INTERNAL NAME TO DEBUGGER NAME
    SYMBOL sym )                // - symbol
{
    CGOP    oper;

    ExtraRptIncrementCtr( ctr_debug_names );
    replicateInit();
    VbufRewind( &mangled_name );
    if( CppLookupOperatorName( sym->name->name, &oper ) ) {
        switch( oper ) {
        case CO_CONVERT:
        {
            VBUF prefix, suffix;
            appendStr( "operator " );
            FormatFunctionType( SymFuncReturnType( sym ), &prefix, &suffix,
                                0, FormatTypeDefault|FF_TYPEDEF_STOP );
            appendStr( VbufString( &prefix ) );
            appendStr( VbufString( &suffix ) );
            VbufFree( &prefix );
            VbufFree( &suffix );
        }   break;
        case CO_CTOR:
            appendStr( NameStr( SimpleTypeName( ScopeClass( SymScope( sym ) ) ) ) );
            break;
        case CO_DTOR:
            appendStr( "~" );
            appendStr( NameStr( SimpleTypeName( ScopeClass( SymScope( sym ) ) ) ) );
            break;
        default:
            appendStr( "operator " );
            appendStr( cppNameCgop( oper ) );
        }
    } else {
        NAME name = sym->name->name;
        if( IsInternalName( name ) ) {
            if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_MEMPTR_FUNC ) ) {
                appendStr( "__mbrptrthunk" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_VBTABLE_NAME ) ) {
                appendStr( "__vbtbl" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_VFTABLE_NAME ) ) {
                appendStr( "__vftbl" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_VATABLE_NAME ) ) {
                appendStr( "__rtti" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_VMTABLE_NAME ) ) {
                appendStr( "__vmtbl" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_VFTHUNK_NAME ) ) {
                appendStr( "__vfthunk");
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_TYPEID_NAME ) ) {
                appendStr( "__typeid");
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_DTOR_OBJ ) ) {
                appendStr( "__dtorobjblk" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_THROW_RO ) ) {
                appendStr( "__throwblk" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_TYPE_SIG ) ) {
                appendStr( "__typesig" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_ARRAY_DTOR ) ) {
                appendStr( "__arrdtorblk" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_STATIC_ONCE ) ) {
                appendStr( "__onceonly" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_STATIC_INIT ) ) {
                appendStr( "__staticinit" );
            } else if( nameHasPrefix( name, IN_NAME_PREFIX IN_SYM_WATCOM IN_NAME_DEBUG_INFO ) ) {
                appendStr( "__debuginfo" );
            } else {
                appendStr( "__internal" );
            }
        } else if( IsNameDummy( name ) ) {
            appendStr( "<unnamed>" );
        } else {
            appendStr( NameStr( name ) );
        }
    }
    return( VbufString( &mangled_name ) );
}

char *CppClassPathDebug(  //TRANSLATE INTERNAL NAME TO CLASS PREFIXED DEBUGGER NAME
    SYMBOL sym )
{
    NAME name;
    SCOPE scope;

    ExtraRptIncrementCtr( ctr_debug_scoped_names );
    CppNameDebug( sym );
    scope = SymScope( sym );
    for( scope = SymScope( sym ); scope != NULL; scope = scope->enclosing ) {
        name = NULL;
        switch( ScopeId( scope ) ) {
        case SCOPE_FILE:
            name = ScopeNameSpaceName( scope );
            break;
        case SCOPE_CLASS:
            name = className( ScopeClass( scope ) );
            break;
        }
        if( name != NULL ) {
            prependStr( "::" );
            prependStr( NameStr( name ) );
        }
    }
    return( VbufString( &mangled_name ) );
}

const char *GetMangledName(     // MANGLE SYMBOL NAME
    SYMBOL sym )                // - symbol to mangle
{
    const char *sym_name;       // - symbol's name
    SCOPE scope;                // - scope for function
    TYPE fn_type;               // - symbol's function type

    if( sym == NULL || sym->name == NULL ) {
        sym_name = "*** NULL ***";
    } else {
        scope = SymScope( sym );
        fn_type = FunctionDeclarationType( sym->sym_type );
        if( fn_type != NULL ) {
            if( ( scope->id != SCOPE_FILE ) || LinkageIsCpp( sym ) && ( fn_type->flag & TF1_PLUSPLUS ) ) {
                sym_name = cppFunctionName( sym );
            } else {
                sym_name = NameStr( sym->name->name );
            }
        } else {
            if( ( scope->id != SCOPE_FILE ) || LinkageIsCpp( sym ) ) {
                sym_name = cppDataName( sym );
            } else {
                sym_name = NameStr( sym->name->name );
            }
        }
    }
    return( sym_name );
}

NAME CppTsName(                 // MANGLED NAME FOR TYPE SIGNATURE
    TYPE type )                 // - type being signified
{
    return( CppNameTypeSig( type ) );
}


NAME CppPCHDebugInfoName(       // MANGLED NAME FOR PCH DEBUG INFO
    char *include_file )        // - include file of PCH
{
    time_t curr_time;
    char *save;
    char *p;
    char buff[_MAX_PATH];

    save = setMangling( IN_NAME_DEBUG_INFO );
    curr_time = time( NULL );
    appendBase32UInt( curr_time );
    appendChar( '@' );
    p = IoSuppFullPath( include_file, buff, sizeof( buff ) );
    for( p = buff; *p; ++p ) {
        if( ! isalnum( *p ) ) {
            *p = '_';
        }
    }
    appendStr( buff );
    return( retMangling( save ) );
}


static void cppNamesInit(       // INITIALIZE NAMES FOR NAMES PROCESSING
    INITFINI* defn )            // - definition
{
    CGOP const *ap;
    NAME *op;
    char **cp;
    NAME *tp;
    NAME op_name;
    NAME place_holder;

    defn = defn;
    DbgStmt( { unsigned n = MAX_OP_NAMES; DbgAssert( n == ARRAY_SIZE( operatorSameAs )); } );
    place_holder = NameDummy();
    for( cp = operatorNamesStr, op = operatorNames, ap = operatorSameAs
       ; cp < &operatorNamesStr[MAX_OP_NAMES]
       ; ++cp, ++op, ++ap ) {
        if( strcmp( *cp, IN_OP_PREFIX ) == 0 ) {
            *op = place_holder;
        } else {
            op_name = NameCreateNoLen( *cp );
            if( *ap != CO_INVALID ) {
                DbgAssert( *ap < ( op - operatorNames ) );
                DbgAssert( strcmp( *cp, NameStr( operatorNames[*ap] ) ) == 0 );
                // we still allocate the name so that the trick we
                // use in CppLookupOperatorName will still work
                *op = operatorNames[*ap];
            } else {
#ifndef NDEBUG
                {
                    char **t;
                    for( t = operatorNamesStr; t < cp; ++t ) {
                        DbgAssert( strcmp( *t, *cp ) != 0 );
                    }
                }
#endif
                *op = op_name;
            }
        }
    }
    for( cp = specialNamesStr, tp = specialNames
       ; cp < &specialNamesStr[MAX_SPECIAL_NAMES]
       ; ++cp, ++tp ) {
        *tp = NameCreateNoLen( *cp );
    }
    VbufInit( &mangled_name );
    ExtraRptRegisterCtr( &ctr_lookups, "# calls to CppLookupOperatorName" );
    ExtraRptRegisterCtr( &ctr_lookups_slow, "# calls to CppLookupOperatorName for operator names" );
    ExtraRptRegisterCtr( &ctr_debug_names, "# calls to CppNameDebug" );
    ExtraRptRegisterCtr( &ctr_debug_scoped_names, "# calls to CppClassPathDebug" );
}


static void cppNamesFini(       // COMPLETION OF NAMES PROCESSING
    INITFINI* defn )            // - definition
{
    defn = defn;
    VbufFree( &mangled_name );
    CMemFreePtr( &objNameBuff );
}


INITDEFN( cpp_names, cppNamesInit, cppNamesFini )

pch_status PCHReadOpNames( void )
{
    NAME *op;
    NAME *cp;

    for( op = operatorNames; op < &operatorNames[MAX_OP_NAMES]; ++op ) {
        *op = NamePCHRead();
    }
    for( cp = specialNames; cp < &specialNames[MAX_SPECIAL_NAMES]; ++cp ) {
        *cp = NamePCHRead();
    }
    return( PCHCB_OK );
}

pch_status PCHWriteOpNames( void )
{
    NAME *op;
    NAME *cp;

    for( op = operatorNames; op < &operatorNames[MAX_OP_NAMES]; ++op ) {
        NamePCHWrite( *op );
    }
    for( cp = specialNames; cp < &specialNames[MAX_SPECIAL_NAMES]; ++cp ) {
        NamePCHWrite( *cp );
    }
    return( PCHCB_OK );
}

pch_status PCHInitOpNames( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniOpNames( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

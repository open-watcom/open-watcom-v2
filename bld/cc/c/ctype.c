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


#include "cvars.h"

TYPEPTR StructDecl(int,int);

extern  unsigned SymTypedef;

#if _CPU == 386
#define _CHECK_SIZE( s )
#else
#define _CHECK_SIZE( s ) \
        if( (s) > 0xffff ) {\
            CErr1( ERR_MAX_STRUCT_SIZE_IS_64K );\
        }
#endif

static  char    CTypeSizes[] = {
        TARGET_CHAR,    /* CHAR         */
        TARGET_CHAR,    /* UCHAR        */
        TARGET_SHORT,   /* SHORT        */
        TARGET_SHORT,   /* USHORT       */
        TARGET_INT,     /* INT          */
        TARGET_INT,     /* UINT         */
        TARGET_LONG,    /* LONG         */
        TARGET_LONG,    /* ULONG        */
        TARGET_LONG64,  /* LONGLONG    */
        TARGET_LONG64,  /* ULONGLONG    */
        TARGET_FLOAT,   /* FLOAT        */
        TARGET_DOUBLE,  /* DOUBLE       */
        0, /*TARGET_POINTER,*/ /* POINTER       */
        0,              /* ARRAY        */
        0,              /* STRUCT       */
        0,              /* UNION        */
        0,              /* FUNCTION     */
        0,              /* FIELD        */
        0, /*TARGET_CHAR,*/     /* VOID */
        0,              /* ENUM         */
        0,              /* TYPEDEF      */
        0,              /* UFIELD       */
        0,              /* DOT_DOT_DOT  */
        TARGET_CHAR     /* PLAIN_CHAR   */
    };

TYPEPTR CTypeHash[TYPE_PLAIN_CHAR+1];
TYPEPTR PtrTypeHash[TYPE_PLAIN_CHAR+1];

TAGPTR  TagHash[TAG_HASH_SIZE + 1];

enum {
        M_CHAR          = 0x0001,
        M_INT           = 0x0002,
        M_SHORT         = 0x0004,
        M_LONG          = 0x0008,
        M_SIGNED        = 0x0010,
        M_UNSIGNED      = 0x0020,
        M_FLOAT         = 0x0040,
        M_DOUBLE        = 0x0080,
        M_LONG_LONG     = 0x0100,
        M_VOID          = 0x0200,
        M___LAST        = 0
 };

#define TYPE_PLAIN_INT  TYPE_UFIELD                     /* 19-mar-91 */

signed char Valid_Types[] = {
        -1,             //
        TYPE_PLAIN_CHAR,//                                          M_CHAR
        TYPE_PLAIN_INT, //                                    M_INT
        -1,             //                                    M_INT M_CHAR
        TYPE_SHORT,     //                            M_SHORT
        -1,             //                            M_SHORT       M_CHAR
        TYPE_SHORT,     //                            M_SHORT M_INT
        -1,             //                            M_SHORT M_INT M_CHAR
        TYPE_LONG,      //                     M_LONG
        -1,             //                     M_LONG               M_CHAR
        TYPE_LONG,      //                     M_LONG         M_INT
        -1,             //                     M_LONG         M_INT M_CHAR
        -1,             //                     M_LONG M_SHORT
        -1,             //                     M_LONG M_SHORT       M_CHAR
        -1,             //                     M_LONG M_SHORT M_INT
        -1,             //                     M_LONG M_SHORT M_INT M_CHAR
        TYPE_INT,       //            M_SIGNED
        TYPE_CHAR,      //            M_SIGNED                      M_CHAR
        TYPE_INT,       //            M_SIGNED                M_INT
        -1,             //            M_SIGNED                M_INT M_CHAR
        TYPE_SHORT,     //            M_SIGNED        M_SHORT
        -1,             //            M_SIGNED        M_SHORT       M_CHAR
        TYPE_SHORT,     //            M_SIGNED        M_SHORT M_INT
        -1,             //            M_SIGNED        M_SHORT M_INT M_CHAR
        TYPE_LONG,      //            M_SIGNED M_LONG
        -1,             //            M_SIGNED M_LONG               M_CHAR
        TYPE_LONG,      //            M_SIGNED M_LONG         M_INT
        -1,             //            M_SIGNED M_LONG         M_INT M_CHAR
        -1,             //            M_SIGNED M_LONG M_SHORT
        -1,             //            M_SIGNED M_LONG M_SHORT       M_CHAR
        -1,             //            M_SIGNED M_LONG M_SHORT M_INT
        -1,             //            M_SIGNED M_LONG M_SHORT M_INT M_CHAR
        TYPE_UINT,      // M_UNSIGNED
        TYPE_UCHAR,     // M_UNSIGNED                               M_CHAR
        TYPE_UINT,      // M_UNSIGNED                         M_INT
        -1,             // M_UNSIGNED                         M_INT M_CHAR
        TYPE_USHORT,    // M_UNSIGNED                 M_SHORT
        -1,             // M_UNSIGNED                 M_SHORT       M_CHAR
        TYPE_USHORT,    // M_UNSIGNED                 M_SHORT M_INT
        -1,             // M_UNSIGNED                 M_SHORT M_INT M_CHAR
        TYPE_ULONG,     // M_UNSIGNED          M_LONG
        -1,             // M_UNSIGNED          M_LONG               M_CHAR
        TYPE_ULONG,     // M_UNSIGNED          M_LONG         M_INT
        -1,             // M_UNSIGNED          M_LONG         M_INT M_CHAR
        -1,             // M_UNSIGNED          M_LONG M_SHORT
        -1,             // M_UNSIGNED          M_LONG M_SHORT       M_CHAR
        -1,             // M_UNSIGNED          M_LONG M_SHORT M_INT
        -1,             // M_UNSIGNED          M_LONG M_SHORT M_INT M_CHAR
        -1,             // M_UNSIGNED M_SIGNED
        -1,             // M_UNSIGNED M_SIGNED                      M_CHAR
        -1,             // M_UNSIGNED M_SIGNED                M_INT
        -1,             // M_UNSIGNED M_SIGNED                M_INT M_CHAR
        -1,             // M_UNSIGNED M_SIGNED        M_SHORT
        -1,             // M_UNSIGNED M_SIGNED        M_SHORT       M_CHAR
        -1,             // M_UNSIGNED M_SIGNED        M_SHORT M_INT
        -1,             // M_UNSIGNED M_SIGNED        M_SHORT M_INT M_CHAR
        -1,             // M_UNSIGNED M_SIGNED M_LONG
        -1,             // M_UNSIGNED M_SIGNED M_LONG               M_CHAR
        -1,             // M_UNSIGNED M_SIGNED M_LONG         M_INT
        -1,             // M_UNSIGNED M_SIGNED M_LONG         M_INT M_CHAR
        -1,             // M_UNSIGNED M_SIGNED M_LONG M_SHORT
        -1,             // M_UNSIGNED M_SIGNED M_LONG M_SHORT       M_CHAR
        -1,             // M_UNSIGNED M_SIGNED M_LONG M_SHORT M_INT
        -1,             // M_UNSIGNED M_SIGNED M_LONG M_SHORT M_INT M_CHAR
};

void InitTypeHashTables()
{
    int         index;
    int         base_type;

    for( index = 0; index <= MAX_PARM_LIST_HASH_SIZE; ++index ) {
        FuncTypeHead[ index ] = NULL;
    }
    for( base_type = TYPE_CHAR; base_type <= TYPE_PLAIN_CHAR; ++base_type ) {
        CTypeHash[ base_type ] = NULL;
        PtrTypeHash[ base_type ] = NULL;
    }
    for( index = 0; index <= TAG_HASH_SIZE; ++index ) {
        TagHash[ index ] = NULL;
    }
}

void CTypeInit()
{
    DATA_TYPE   base_type;
    int         size;

    TypeHead = NULL;
    TagCount = 0;
    FieldCount = 0;
    EnumCount = 0;
    InitTypeHashTables();
    for( base_type = TYPE_CHAR; base_type <= TYPE_DOT_DOT_DOT; ++base_type ) {
        CTypeCounts[ base_type ] = 0;
        size = CTypeSizes[ base_type ];
        if( size != 0  ||  base_type == TYPE_VOID  ||
                            base_type == TYPE_DOT_DOT_DOT ) {
            BaseTypes[ base_type ] = TypeNode( base_type, NULL );
        } else {
            BaseTypes[ base_type ] = NULL;
        }
    }
    SetPlainCharType( TYPE_UCHAR );
    StringArrayType = NULL;
    VoidParmList[0] = BaseTypes[ TYPE_VOID ];   /* 27-dec-88 */
    VoidParmList[1] = NULL;
}


TYPEPTR GetType( DATA_TYPE base_type )
{
    TYPEPTR     typ;

    typ = BaseTypes[ base_type ];
    if( typ == NULL ) {
        typ = TypeNode( base_type, NULL );
    }
    return( typ );
}

void WalkFuncTypeList( void (*func)(TYPEPTR,int) )
{
    TYPEPTR     typ;
    int         index;

    for( index = 0; index <= MAX_PARM_LIST_HASH_SIZE; index++ ) {
        for( typ = FuncTypeHead[ index ]; typ; typ = typ->next_type ) {
            func( typ, index );
        }
    }
}

void WalkTypeList( void (*func)(TYPEPTR) )
{
    TYPEPTR     typ;
    int         base_type;

    for( base_type = TYPE_CHAR; base_type <= TYPE_PLAIN_CHAR; ++base_type ) {
        for( typ = CTypeHash[ base_type ]; typ; typ = typ->next_type ) {
            func( typ );
        }
    }
    for( base_type = TYPE_CHAR; base_type <= TYPE_PLAIN_CHAR; ++base_type ) {
        for( typ = PtrTypeHash[ base_type ]; typ; typ = typ->next_type ) {
            func( typ );
        }
    }
}
#if 0
TYPEPTR DupType( TYPEPTR typ, type_modifiers flags, int force_duplicate )
{
    TYPEPTR     newtype;
    TYPEPTR     next;

    if( ! force_duplicate ) {
        if( typ->decl_type == TYPE_POINTER ) {
            next = PtrTypeHash[ typ->object->decl_type ];
        } else {
            next = CTypeHash[ typ->decl_type ];
        }
        for( ; next; next = next->next_type ) {
            if( next->decl_type == typ->decl_type  &&
                next->object    == typ->object     &&
                next->u.tag     == typ->u.tag      &&
                next->decl_flags == flags  ) {
                return( next );
            }
        }
    }
    newtype = TypeNode( typ->decl_type, typ->object );
    next = newtype->next_type;
    memcpy( newtype, typ, sizeof(TYPEDEFN) );
    newtype->next_type = next;
    newtype->decl_flags = flags;
    return( newtype );
}
#endif
static void SetPlainCharType( int char_type )
{
    TYPEPTR     typ;

    typ = TypeNode( char_type, NULL );
    typ->type_flags = TF2_TYPE_PLAIN_CHAR;
    BaseTypes[ TYPE_PLAIN_CHAR ] = typ;
    StringType = PtrNode( typ, 0, SEG_DATA );
    ConstCharType =  typ;
}

void SetSignedChar()
{
    SetPlainCharType( TYPE_CHAR );
}


int TypeQualifier()
{
    int         flags, bit;

    flags = 0;
    bit = 0;
    for( ;; ) {
        if( flags & bit )  CErr1( ERR_REPEATED_MODIFIER );       /* 24-mar-91 */
        flags |= bit;
        if( CurToken == T_CONST ) {
            bit = FLAG_CONST;
            NextToken();
            continue;
        }
        if( CurToken == T_VOLATILE ) {
            bit = FLAG_VOLATILE;
            NextToken();
            continue;
        }
        if( CurToken == T___UNALIGNED ) {
            bit = FLAG_UNALIGNED;
            NextToken();
            continue;
        }
        break;
    }
    return( flags );
}


local TYPEPTR GetScalarType( char *plain_int, int bmask )
{
    DATA_TYPE   data_type;
    TYPEPTR     typ;

    data_type = -1;
    if( bmask & (M_VOID | M_FLOAT | M_DOUBLE | M_LONG_LONG ) ) {
        if( bmask == M_VOID ) {
            data_type = TYPE_VOID;
        } else if( bmask == M_LONG_LONG ) {
            data_type = TYPE_LONG64;
        } else if( bmask == (M_LONG_LONG | M_SIGNED) ) {
            data_type = TYPE_LONG64;
        } else if( bmask == (M_LONG_LONG | M_UNSIGNED) ) {
            data_type = TYPE_ULONG64;
        } else if( bmask == M_FLOAT ) {
            data_type = TYPE_FLOAT;
        } else if( bmask == M_DOUBLE ) {
            data_type = TYPE_DOUBLE;
        } else if( bmask == (M_LONG | M_DOUBLE) ) {
            data_type = TYPE_LONG_DOUBLE;
        } else {
            data_type = -1;
        }
    } else if( bmask == 0 ) {
        data_type = TYPE_INT;
        *plain_int = 1;
    } else {
        data_type = Valid_Types[ bmask ];
        if( data_type == TYPE_PLAIN_INT ) {             /* 19-mar-91 */
            data_type = TYPE_INT;
            *plain_int = 1;
        }
    }
    if( data_type == -1 ) {
        CErr1( ERR_INV_TYPE );
        data_type = TYPE_INT;
    }
    typ = GetType( data_type );
    return( typ );
}


local void AdvanceToken()
{
    if( CurToken == T_SAVED_ID ) {
        CMemFree( SavedId );
        SavedId = NULL;
        CurToken = LAToken;
    } else {
        NextToken();
    }
}

static void DeclSpecifiers( char    *plain_int,
                              decl_info *info   )
{
    TYPEPTR             typ;
    int                 bmask;
    int                 bit;
    int                 flags;
    int                 packed;
    SYM_HANDLE          sym_handle;
    stg_classes         stg_class;
    stg_classes         specified_stg_class;
    auto SYM_ENTRY      sym;

    *plain_int = 0;
    info->mod = FLAG_NONE;
    info->decl = DECLSPEC_NONE;
    info->naked = FALSE;
    info->seg = 0;
    bmask = 0;
    flags = FLAG_NONE;
    packed = FALSE;
    typ = NULL;
    specified_stg_class = SC_NULL;
    for(;;) {
        stg_class = SC_NULL;
        bit = 0;
        switch( CurToken ) {
        case T_CHAR:      bit = M_CHAR;         break;
        case T_INT:       bit = M_INT;          break;
        case T_SHORT:     bit = M_SHORT;        break;
        case T_LONG:      bit = M_LONG;         break;
        case T___INT64:   bit = M_LONG_LONG;    break;
        case T_SIGNED:    bit = M_SIGNED;       break;
        case T_UNSIGNED:  bit = M_UNSIGNED;     break;
        case T_FLOAT:     bit = M_FLOAT;        break;
        case T_DOUBLE:    bit = M_DOUBLE;       break;
        case T_VOID:      bit = M_VOID;         break;

        case T_CONST:
            if( flags & FLAG_CONST ) CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_CONST;
            break;
        case T_VOLATILE:
            if( flags & FLAG_VOLATILE ) CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_VOLATILE;
            break;
        case T___UNALIGNED:
            if( flags & FLAG_UNALIGNED )CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_UNALIGNED;
            break;
        case T___INLINE:
            if( flags & FLAG_INLINE )CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_INLINE;
            break;
        case T__PACKED:
            if( packed  ) CErr1( ERR_REPEATED_MODIFIER );
            packed = TRUE;
            break;
        case T_EXTERN:
           stg_class = SC_EXTERN;
           break;
        case T_STATIC:
            stg_class = SC_STATIC;
            break;
        case T_TYPEDEF:
            stg_class = SC_TYPEDEF;
            break;
        case T_AUTO:
            stg_class = SC_AUTO;
            break;
        case T_REGISTER:
            stg_class = SC_REGISTER;
            break;
        case T_STRUCT:
            if( typ != NULL )  CErr1( ERR_INV_TYPE );
            typ = StructDecl( TYPE_STRUCT, packed );
            packed = 0;
            continue;
        case T_UNION:
            if( typ != NULL )  CErr1( ERR_INV_TYPE );
            typ = StructDecl( TYPE_UNION, packed );
            packed = 0;
            continue;
        case T_ENUM:
            if( typ != NULL )  CErr1( ERR_INV_TYPE );
            typ = EnumDecl( flags );
            continue;
        case T___SEGMENT:                               /* 21-oct-91 */
            bit = M_UNSIGNED | M_SHORT;
            flags |= FLAG_SEGMENT;                      /* 15-nov-91 */
            break;

        case T___DECLSPEC:
            if( info->stg == 0 ){
                CErr1( ERR_INVALID_DECLARATOR );
            }
            AdvanceToken();                   // declspec( dllimport naked )
            MustRecog( T_LEFT_PAREN );
            {
                declspec_class decl;
                type_modifiers  modifier;

                decl = DECLSPEC_NONE;
                modifier = 0;
                while( CurToken != T_RIGHT_PAREN ){
                    switch( CurToken ){
                    case T___CDECL:
                        modifier = LANG_CDECL;
                        break;
                    case T___PASCAL:
                        modifier = LANG_PASCAL;
                        break;
                    case T___FORTRAN:
                        modifier = LANG_FORTRAN;       // TC_FORTRAN
                        break;
                    case T__SYSCALL:
                        modifier = LANG_SYSCALL;       // TC_SYSCALL           /* 04-jul-91 */
                        break;
                    case T___STDCALL:
                        modifier = LANG_STDCALL;       // TC_STDCALL
                        break;
                    case T___FASTCALL:
                        modifier = LANG_FASTCALL;      // TC_FASTCALL
                        break;
                    case T__OPTLINK:
                        modifier = LANG_OPTLINK;       // TC_OPTLINK
                        break;
                    case T_ID:
                        decl = DECLSPEC_NONE;
                        if( strcmp( Buffer, "dllimport" ) == 0 ) {
                            decl = DECLSPEC_DLLIMPORT;
                        } else if( strcmp( Buffer, "overridable" ) == 0 ) {
                            decl = DECLSPEC_DLLIMPORT;
                        } else if( strcmp( Buffer, "dllexport" ) == 0 ) {
                            decl = DECLSPEC_DLLEXPORT;
                        } else if( strcmp( Buffer, "thread" ) == 0 ) {
                            decl = DECLSPEC_THREAD;
                        } else if( strcmp( Buffer, "naked" ) == 0 ) {
                            if( info->naked ){
                                CErr1( ERR_INVALID_DECLSPEC );
                            }else{
                                info->naked = TRUE;
                            }
                        } else {
                            CErr1( ERR_INVALID_DECLSPEC );
                        }
                        if( decl != DECLSPEC_NONE ){
                            if( info->decl == DECLSPEC_NONE ){
                                info->decl = decl;
                            }else{
                                CErr1( ERR_INVALID_DECLSPEC );
                            }
                        }
                        break;
                    default:
                        CErr1( ERR_INVALID_DECLSPEC );
                        goto done;
                    }
                    if( modifier & FLAG_LANGUAGES ){
                        if( flags & FLAG_LANGUAGES ){
                            CErr1( ERR_INVALID_DECLSPEC );
                        }else{
                            flags |= modifier;
                        }
                    }
                    NextToken();
                }
            }
         done:
            MustRecog( T_RIGHT_PAREN );
            continue;
        case T_SAVED_ID:
        case T_ID:
            if( typ != NULL || bmask != 0 ) goto got_specifier;
            /* lookup id in symbol table */
            /* if valid type identifier then OK */
            if( CurToken == T_ID ) {
                sym_handle = SymLookTypedef( HashValue, Buffer, &sym );
            } else {    /* T_SAVED_ID */
                sym_handle = SymLookTypedef( SavedHash, SavedId, &sym );
            }
            if( sym_handle == 0 )                 goto got_specifier;
            if( sym.stg_class != SC_TYPEDEF ) goto got_specifier;
            if( SymLevel != 0 && flags == 0 ) {
                if( CurToken == T_ID ) {
                    LookAhead();
                    if( LAToken == T_COLON )  goto got_specifier;
                }
            }
            ++SymTypedef;
            typ = sym.sym_type;
            SymGet( &sym, sym_handle );  // get rest of sym from nutty sym table
            if( flags & sym.attrib ) {      /* 24-mar-91, 12-may-91 */
                CErr1( ERR_INV_TYPE );
            }
            flags |= sym.attrib;
            if( sym.attrib & FLAG_BASED ){
                info->seg = sym.u.var.segment;
            }
            if( sym.declspec != DECLSPEC_NONE ){
                if( info->decl == DECLSPEC_NONE ){
                    info->decl = sym.declspec;
                }else{
                    CErr1( ERR_INVALID_DECLSPEC );
                }
            }
            if( sym.naked ){
                if( info->naked ){
                    CErr1( ERR_INVALID_DECLSPEC );
                }else{
                    info->naked = TRUE;
                }
            }
            AdvanceToken();
            continue;
        default:          goto got_specifier;
        }
        if( stg_class != SC_NULL ) {
            if( info->stg == SC_NULL ) break;       // don't want any stg class
            if( specified_stg_class != SC_NULL ) {
                CErr1( ERR_TOO_MANY_STORAGE_CLASS_SPECIFIERS );
            }
            specified_stg_class = stg_class;
        }
        if( bmask & bit ) {
            if( bit == M_LONG ) {
                // long long found
                bmask &= ~M_LONG;
                bit = M_LONG_LONG;
            } else {
                CErr1( ERR_INV_TYPE );
            }
        }
        bmask |= bit;
        NextToken();
    }
got_specifier:
    info->stg = specified_stg_class;
    if( typ != NULL ) {
        /* already have a type (TYPE_STRUCT, TYPE_UNION, TYPE_ENUM) */
        /* or an ID that was a typedef name */
        if( bmask != 0 )  CErr1( ERR_INV_TYPE );  // picked up an int
    } else {
        if( flags != FLAG_NONE || bmask != 0 ){  // not just id hanging there
            typ = GetScalarType( plain_int, bmask );
        }
    }
    info->typ = typ;
    info->mod = flags;
}

void TypeSpecifier( decl_info *info )
{
    char                plain_int;

    info->stg = SC_NULL;      // indicate don't want any storage class specifiers
    DeclSpecifiers( &plain_int, info );
}

void GetFieldTypeSpecifier( char *plain_int, decl_info *info )
{

    info->stg = SC_NULL;      // indicate don't want any storage class specifiers
    DeclSpecifiers( plain_int, info );
}

void FullDeclSpecifier( decl_info *info )
{
    char        plain_int;

    info->stg = SC_FORWARD;    // indicate want storage class specifiers
    DeclSpecifiers( &plain_int, info );
}

TYPEPTR TypeDefault()
{
    return( GetType( TYPE_INT ) );
}


static TAGPTR NewTag( char *name, int hash )
{
    TAGPTR      tag;

    tag = (TAGPTR) CPermAlloc( sizeof( TAGDEFN ) + strlen( name ) );
    tag->level = SymLevel;
    tag->hash = hash;
    tag->next_tag = TagHash[ hash ];
    TagHash[ hash ] = tag;
    strcpy( tag->name, name );
    ++TagCount;
    return( tag );
}


TAGPTR NullTag()
{
    return( NewTag( "", TAG_HASH_SIZE ) );
}


TAGPTR VfyNewTag( TAGPTR tag, int tag_type )
{
    if( tag->sym_type != NULL ) {               /* tag already exists */
        if( tag->level != SymLevel ) {
            tag = NewTag( tag->name, tag->hash );
        } else if( tag->size != 0  ||   /* already defined */
                   tag->sym_type->decl_type != tag_type ) { /* 18-jan-89 */
            CErr2p( ERR_DUPLICATE_TAG, tag->name );
        }
    }
    return( tag );
}


local FIELDPTR NewField( FIELDPTR new_field, TYPEPTR decl )
{
    FIELDPTR    field;
    FIELDPTR    prev_field;
    TYPEPTR     typ;
    TAGPTR      tag;

    ++FieldCount;
    typ = new_field->field_type;
    if( typ != NULL ) {
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    }
    if( new_field->name[0] == '\0' ) {
        /* allow nameless structs and unions;  15-sep-90 */
        if( (typ->decl_type != TYPE_STRUCT      &&
             typ->decl_type != TYPE_UNION) ||
             ! CompFlags.extensions_enabled ) {
            CErr1( ERR_INVALID_DECLARATOR );
        }
    }
    if( typ == decl ) {
        CErr1( ERR_STRUCT_OR_UNION_INSIDE_ITSELF );
    } else if( SizeOfArg( typ ) == 0 ) {   /* was TypeSize(typ) 15-may-90*/
        /* can't have an array of incomplete type   24-aug-90 */
        if( (typ->decl_type == TYPE_ARRAY  &&
        (SizeOfArg( typ->object ) == 0 || !CompFlags.extensions_enabled ) )
        ||      typ->decl_type != TYPE_ARRAY ) { /* JFD 15-jun-90 */
            CErr( ERR_INCOMPLETE_TYPE, (SYM_NAMEPTR)(new_field->name) );
        }
    }
    tag = decl->u.tag;
    for( field = tag->u.field_list; field; field = field->next_field ) {
        if( new_field->name[0] != '\0' ) {  /* only check non-empty names */
            if( strcmp( field->name, new_field->name ) == 0 ) {
                CErr2p( ERR_DUPLICATE_FIELD_NAME, field->name );
            }
        }
        prev_field = field;
    }
    if( tag->u.field_list == NULL ) {
        tag->u.field_list = new_field;
    } else {
        prev_field->next_field = new_field;
        if( SizeOfArg( prev_field->field_type ) == 0 ) { /* 05-jun-92 */
            CErr( ERR_INCOMPLETE_TYPE, (SYM_NAMEPTR)(prev_field->name) );
        }
    }
    return( new_field );
}


local TYPEPTR EnumFieldType( TYPEPTR ftyp,
                             char plain_int,            /* 19-mar-91 */
                             unsigned start,
                             unsigned width )
{
    TYPEPTR     typ;
    unsigned char data_type;

    typ = TypeNode( TYPE_FIELD, NULL );
    typ->u.f.field_start = start;
    typ->u.f.field_width = width;
    if( plain_int ) {
        data_type = TYPE_INT;   /* default to signed bit fields */
    } else {
        while( ftyp->decl_type == TYPE_TYPEDEF ) ftyp = ftyp->object;
        if( ftyp->decl_type == TYPE_ENUM ) {
            ftyp = ftyp->object;
        }
        data_type = ftyp->decl_type;
    }
    typ->u.f.field_type = data_type;
    switch( data_type ) {
    case TYPE_UCHAR:
    case TYPE_USHORT:
    case TYPE_UINT:
    case TYPE_ULONG:
    case TYPE_ULONG64:
        typ->decl_type = TYPE_UFIELD;
        break;
    }
    return( typ );
}


unsigned GetTypeAlignment( TYPEPTR typ )
{
    unsigned size;

    for(;;) {
        if(( typ->decl_type == TYPE_TYPEDEF ) ||
           ( typ->decl_type == TYPE_ARRAY )) {
            typ = typ->object;
        } else {
            break;
        }
    }
    if( typ->decl_type == TYPE_STRUCT  ||       /* 25-jul-91 */
        typ->decl_type == TYPE_UNION ) {
        size = typ->u.tag->alignment;
    #if _MACHINE == _ALPHA
        if( CompFlags.align_structs_on_qwords ) {
            size = 8;
        }
    #endif
    } else {
        size = TypeSize( typ );
    }
    return( size );
}


local unsigned long FieldAlign( unsigned long next_offset,
                                FIELDPTR field,
                                unsigned int *worst_alignment )
{                               /* 05-jan-89 */
    unsigned    pack_adjustment;
    unsigned    align;

    pack_adjustment = PackAmount;
    align = GetTypeAlignment( field->field_type );
    if( align > pack_adjustment ){ // can't be any bigger than pack( x )
        align = pack_adjustment;
    }
    if( align > *worst_alignment ) {    /* 24-jul-91 */
        *worst_alignment = align;
    }
    if( align != 1 ) {
        unsigned long old_offset = next_offset;

        next_offset += align - 1;
        next_offset &= - (long)align;
        if( CompFlags.slack_byte_warning && (next_offset - old_offset)  ){
            CWarn2( WARN_LEVEL_1,
                    ERR_SLACK_ADDED, (next_offset - old_offset)  );
        }
    }
    field->offset = next_offset;
    return( next_offset );
}

local int UnQualifiedType( TYPEPTR typ )                        /* 21-mar-91 */
{
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( typ->decl_type == TYPE_ENUM ) {
        typ = typ->object;
    }
    switch( typ->decl_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
        return( TYPE_CHAR );
    case TYPE_SHORT:
    case TYPE_USHORT:
        return( TYPE_SHORT );
    case TYPE_INT:
    case TYPE_UINT:
        return( TYPE_INT );
    case TYPE_LONG:                     /* + AFS 05-mar-91 */
    case TYPE_ULONG:
        return( TYPE_LONG );
    case TYPE_LONG64:
    case TYPE_ULONG64:
        return( TYPE_LONG64 );
    }
    return( 0 );
}


local unsigned long GetFields( TYPEPTR decl )
{
    unsigned long       start = 0;
    TYPEPTR             typ;
    decl_state          state;
    FIELDPTR            field;
    unsigned            scalar_size;
    unsigned            bits_available, bits_total;
    unsigned long       struct_size;
    unsigned long       next_offset;
    int                 width;
    unsigned int        worst_alignment;                /* 24-jul-91 */
    char                unqualified_type, prev_unqualified_type;
    char                plain_int;
    decl_info           info;

    prev_unqualified_type = TYPE_VOID;   /* so it doesn't match 1st time */
    worst_alignment = 1;                                /* 24-jul-91 */
    bits_available = 1;
    bits_total = 0;
    /* assertion: bits_available != bits_total && bits_total >> 3 == 0 */
    struct_size = start;
    next_offset = start;
    for(;;) {
       if( CurToken == T_SEMI_COLON && CompFlags.extensions_enabled ){
            NextToken();
            if( CurToken == T_RIGHT_BRACE ) break;
            continue;
        }
        GetFieldTypeSpecifier( &plain_int, &info );
        typ = info.typ;
        state = DECL_STATE_NONE;
        if( typ == NULL ) {
            state |= DECL_STATE_NOTYPE;
            CErr2p( ERR_MISSING_DATA_TYPE, Buffer );
            typ = TypeDefault();
            if( CurToken == T_ID ) NextToken();
        }
        unqualified_type = UnQualifiedType( typ );
        if( bits_available == bits_total ||
            decl->decl_type == TYPE_UNION ) {           /* 12-nov-94 */
            bits_available = TypeSize( typ ) * 8;
            bits_total = bits_available;
        } else if( unqualified_type != prev_unqualified_type ) {
            next_offset += bits_total >> 3;
            bits_available = TypeSize( typ ) * 8;
            bits_total = bits_available;
        }
        prev_unqualified_type = unqualified_type;
        for( ;; ) {
            field = NULL;
            if( CurToken != T_COLON ) {
                field = NewField( FieldDecl( typ,info.mod, state ), decl );
            }
            if( CurToken == T_COLON ) {
                if( field != NULL ){
                    next_offset = FieldAlign( next_offset, field,
                                              &worst_alignment );
                }
                CheckBitfieldType( typ );
                NextToken();
                width = ConstExpr();
                if( width == 0  &&      field != NULL ) {
                    CErr1( ERR_WIDTH_0 );
                }
                if( width > TARGET_BITS ) {
                    CErr1( ERR_FIELD_TOO_WIDE );
                    width = TARGET_BITS;
                } else if( width < 0 ) {
                    CErr1( ERR_WIDTH_NEGATIVE );
                    width = 0;
                }
                if( width > bits_available      ||  width == 0 ) {
                    scalar_size = TypeSize( typ );
                    if( bits_available != bits_total ) {
                        /* some bits have been used; abandon this unit */
                        next_offset += bits_total >> 3;
                    } else if( width == 0 ) {
                        /* no bits have been used; align to base type */
                        next_offset += scalar_size - 1;
                        next_offset &= ~( scalar_size - 1 );
                    }
                    bits_available = scalar_size * 8;
                    bits_total = bits_available;
                }
                if( field != NULL ) {
                    if( unqualified_type == TYPE_LONG64 ){
                        int bit_offset = bits_total - bits_available;

                        if( bit_offset >= 32 ){
                            field->offset = next_offset+4;
                            bit_offset -= 32;
                        }else{
                            field->offset = next_offset;
                        }
                        field->field_type = EnumFieldType( GetType( TYPE_LONG ), plain_int,
                                  bit_offset, width );
                    }else{
                        field->offset = next_offset;
                        field->field_type = EnumFieldType( typ, plain_int,
                                  bits_total - bits_available, width );
                    }
                }
                bits_available -= width;
            } else {
                if( bits_available != bits_total ) { //changed from bit field to non
                    next_offset += bits_total >> 3;
                    field->offset = next_offset;
                    bits_available = TypeSize( typ ) * 8;
                    bits_total = bits_available;
                }
                next_offset = FieldAlign( next_offset, field,
                                          &worst_alignment );
                next_offset += SizeOfArg( field->field_type );
            }
            if( next_offset > struct_size )  struct_size = next_offset;
            if( decl->decl_type == TYPE_UNION ){
                next_offset = start;
                bits_available = bits_total;
            }
            if( CurToken != T_COMMA ) break;
            NextToken();
        }
        if( CurToken == T_RIGHT_BRACE ) {
            CWarn1( WARN_MISSING_LAST_SEMICOLON,
                    ERR_MISSING_LAST_SEMICOLON );
        } else {
            MustRecog( T_SEMI_COLON );
        }
        if( CurToken == T_RIGHT_BRACE ) break;
    }
    if( bits_available != bits_total ) { /* if last field was bit field */
        next_offset += bits_total >> 3;
        if( next_offset > struct_size )  struct_size = next_offset;
    }
    decl->u.tag->alignment = worst_alignment;   /* 25-jul-91 */
    struct_size += worst_alignment - 1;         /* 01-may-92 */
    struct_size &= - (long)worst_alignment;
    _CHECK_SIZE( struct_size );
    NextToken();
    return( struct_size );
}


local TYPEPTR StructDecl( int decl_typ, int packed )
{
    TYPEPTR     typ;
    TAGPTR      tag;
    int         saved_packamount;
    TAGPTR      TagLookup();

    saved_packamount = PackAmount;                      /* 20-nov-91 */
    if( packed )  PackAmount = 1;
    NextToken();
    if( CurToken == T_LEFT_BRACE ) {
        tag = NullTag();
        NextToken();
    } else {
        for(;;) {
            if( CurToken == T_ID ) break;
            if( CurToken == T_LEFT_BRACE ) break;
            if( CurToken == T_EOF ) break;
            ExpectStructUnionTag();
            NextToken();
        }
        if( CurToken == T_ID ) {        /* structure or union tag */
            tag = TagLookup();
            NextToken();
        } else {
            tag = NullTag();
        }
        if( CurToken != T_LEFT_BRACE ) {
            if( CurToken == T_SEMI_COLON ) {
                if( tag->level != SymLevel ) {
                    tag = NewTag( tag->name, tag->hash );
                }
            }
            typ = tag->sym_type;
            if( typ == NULL ) {
                typ = TypeNode( decl_typ, NULL );
            } else {
                if( typ->decl_type != decl_typ ) {              /* 18-jan-89 */
                    CErr2p( ERR_DUPLICATE_TAG, tag->name );
                }
            }
            tag->sym_type = typ;
            typ->u.tag = tag;   /* FWC 19-jan-87 */
            PackAmount = saved_packamount;
            return( typ );
        }
        NextToken();
        tag = VfyNewTag( tag, decl_typ );
    }
    typ = tag->sym_type;
    if( typ == NULL ) {
        typ = TypeNode( decl_typ, NULL );
        tag->sym_type = typ;
    }
    typ->u.tag = tag;
    tag->u.field_list = NULL;
    tag->size = GetFields( typ );
    PackAmount = saved_packamount;                      /* 20-nov-91 */
    return( typ );
}


local void CheckBitfieldType( TYPEPTR typ )
{
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( CompFlags.extensions_enabled ) {
        if( typ->decl_type == TYPE_ENUM ) {
            typ = typ->object;
        }
    }
    switch( typ->decl_type ) {
    case TYPE_INT:
    case TYPE_UINT:
        /* ANSI C only allows int and unsigned [int] */
        return;
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_LONG:                     /* + AFS 05-mar-91 */
    case TYPE_ULONG:
    case TYPE_LONG64:
    case TYPE_ULONG64:
        if( CompFlags.extensions_enabled ) {
            return;
        }
    }
    CErr1( ERR_INVALID_TYPE_FOR_FIELD );
}


void VfyNewSym( int hash_value, char *name )
{
    int         enum_var;
    SYM_HANDLE  sym_handle;
    auto SYM_ENTRY sym;
    auto struct enum_info ei;

    enum_var = EnumLookup( hash_value, name, &ei );
    if( enum_var ) {
        if( ei.level != SymLevel )      enum_var = 0;
    }
    sym_handle = SymLook( hash_value, name );
    if( sym_handle != 0 ) {
        SymGet( &sym, sym_handle );
        if( sym.level != SymLevel )  sym_handle = 0;
    }
    if( sym_handle != 0  ||  enum_var != 0 ) {
        CErr2p( ERR_SYM_ALREADY_DEFINED, name );
    }
}


TAGPTR TagLookup()
{
    TAGPTR      tag;
    int         hash;

    hash = HashValue;
    tag = TagHash[ hash ];;
    while( tag != NULL ) {
        if( strcmp( Buffer, tag->name ) == 0 ) return( tag );
        tag = tag->next_tag;
    }
    return( NewTag( Buffer, hash ) );
}

void FreeTags()
{
    TAGPTR      tag;
    int         hash;

    for( hash = 0; hash <= TAG_HASH_SIZE; ++hash ) {
        for( ; tag = TagHash[ hash ]; ) {
            if( tag->level < SymLevel ) break;
            TagHash[ hash ] = tag->next_tag;
            tag->next_tag = DeadTags;
            DeadTags = tag;
        }
    }
}

void WalkTagList( void (*func)(TAGPTR) )
{
    TAGPTR      tag;
    int         index;

    for( index = 0; index <= TAG_HASH_SIZE; ++index ) {
        for( tag = TagHash[index]; tag; tag = tag->next_tag ) {
            func( tag );
        }
    }
}

void AddTypeHash( TYPEPTR typ )
{
    if( typ->decl_type == TYPE_POINTER ) {
        if( typ->object != NULL ) {
            typ->next_type = PtrTypeHash[ typ->object->decl_type ];
            PtrTypeHash[ typ->object->decl_type ] = typ;
        }
    } else if( typ->decl_type == TYPE_FUNCTION ) {
        typ->next_type = NULL;
    } else {
        typ->next_type = CTypeHash[ typ->decl_type ];
        CTypeHash[ typ->decl_type ] = typ;
    }
}

void AddPtrTypeHash( TYPEPTR typ )
{
    typ->next_type = PtrTypeHash[ typ->object->decl_type ];
    PtrTypeHash[ typ->object->decl_type ] = typ;
}

TYPEPTR TypeNode( DATA_TYPE type_spec, TYPEPTR the_object )
{
    TYPEPTR     typ;

    typ = (TYPEPTR) CPermAlloc( sizeof(TYPEDEFN) );
    typ->decl_type = type_spec;
    typ->object = the_object;
    typ->u.tag = NULL;
    AddTypeHash( typ );
    ++TypeCount;
    ++CTypeCounts[type_spec];
    return( typ );
}

TYPEPTR ArrayNode( TYPEPTR the_object )
{
    TYPEPTR     typ;

    typ = TypeNode( TYPE_ARRAY, the_object );
    typ->u.array = CPermAlloc( sizeof( struct array_info ) );
    return( typ );
}


local TYPEPTR MkPtrNode( TYPEPTR typ, type_modifiers flags,
        int segid, SYM_HANDLE base, BASED_KIND based_kind )
{
    TYPEPTR     ptrtyp;

    if( typ != NULL ) {
        ptrtyp = PtrTypeHash[ typ->decl_type ];
        for( ; ptrtyp; ptrtyp = ptrtyp->next_type ) {
            if( ptrtyp->decl_type   == TYPE_POINTER &&
                ptrtyp->object  == typ          &&
                ptrtyp->u.p.segment == segid    &&
                ptrtyp->u.p.based_sym == base   &&
                ptrtyp->u.p.based_kind == based_kind   &&
                ptrtyp->u.p.decl_flags  == flags ) {
                return( ptrtyp );
            }
        }
    }
    ptrtyp = TypeNode( TYPE_POINTER, typ );
    ptrtyp->u.p.decl_flags = flags;
    ptrtyp->u.p.segment = segid;
    ptrtyp->u.p.based_sym = base;
    ptrtyp->u.p.based_kind = based_kind;
    return( ptrtyp );
}

TYPEPTR PtrNode( TYPEPTR typ, int flags, int segid )
{
    return( MkPtrNode( typ, flags, segid, 0, BASED_NONE ) );
}

TYPEPTR BPtrNode( TYPEPTR typ, int flags, int segid,
                  SYM_HANDLE base, BASED_KIND kind )
{
    return( MkPtrNode( typ, flags, segid, base, kind  ) );
}

int FuncHeadIndex( TYPEPTR *parm_types )
{
    int         index;

    index = 0;
    if( parm_types != NULL ) {
        while( *parm_types != NULL ) {
            ++parm_types;
            ++index;
            if( index == MAX_PARM_LIST_HASH_SIZE ) break;
        }
    }
    return( index );
}

TYPEPTR FuncNode( TYPEPTR return_typ, int flag, TYPEPTR *parm_types )
{
    TYPEPTR     typ;
    int         index;

    flag = FLAG_NONE;
    index = FuncHeadIndex( parm_types );
    if( return_typ != NULL ){
        for( typ = FuncTypeHead[ index ]; typ; typ = typ->next_type ) {
            if( typ->object == return_typ &&
                typ->u.parms        == parm_types ) {
                return( typ );
            }
        }
    }
    typ = TypeNode( TYPE_FUNCTION, return_typ );
    typ->u.parms = parm_types;
    typ->next_type = FuncTypeHead[ index ];
    FuncTypeHead[ index ] = typ;
    return( typ );
}

unsigned long TypeSize( TYPEPTR typ )
{
    unsigned long size;

    if( typ == NULL ) return( 0 );                      /* 22-feb-90 */
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    switch( typ->decl_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_INT:
    case TYPE_UINT:
    case TYPE_LONG:
    case TYPE_ULONG:
    case TYPE_LONG64:
    case TYPE_ULONG64:
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_FUNCTION:
    case TYPE_DOT_DOT_DOT:
    case TYPE_PLAIN_CHAR:
        size = CTypeSizes[ typ->decl_type ];
        break;
    case TYPE_VOID:
        if( CompFlags.unix_ext ){
            size = 1;
        }else{
            size = 0;
        }
        break;
    case TYPE_POINTER:
        if( typ->u.p.decl_flags & ( FLAG_FAR | FLAG_HUGE ) ) {
            size = TARGET_FAR_POINTER;          /* 20-feb-89 */
        } else if( typ->u.p.decl_flags & FLAG_NEAR ) {
            size = TARGET_POINTER;
        } else {
            do {
                typ = typ->object;
            } while( typ->decl_type == TYPE_TYPEDEF );
            if( typ->decl_type == TYPE_FUNCTION ) {
                size = CodePtrSize;
            } else {
                size = DataPtrSize;
            }
        }
        break;
    case TYPE_ARRAY:
        size = typ->u.array->dimension;
        break;
    case TYPE_STRUCT:
        size = typ->u.tag->size;
        if( typ->object != NULL ) {                     /* 17-mar-92 */
            /* structure has a zero length array as last field */
            typ = typ->object;  /* point to TYPE_ARRAY entry */
            size += SizeOfArg( typ );                   /* 13-jun-94 */
        }
        break;
    case TYPE_UNION:
    case TYPE_ENUM:
        size = typ->u.tag->size;
        break;
    case TYPE_FIELD:
    case TYPE_UFIELD:
        size = CTypeSizes[ typ->u.f.field_type ];
        break;
    default:
        size = 0;
    }
    return( size );
}


void TypesPurge()
{
#if 0
    /* The type entries are in permanent memory, so they can't be freed */
    TYPEPTR     temp;

    while( TypeHead != NULL ) {
        temp = TypeHead->next_type;
        CMemFree( TypeHead );
        TypeHead = temp;
    }
#endif
}

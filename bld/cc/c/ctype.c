/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Type management functions.
*
****************************************************************************/


#include "cvars.h"

extern  unsigned SymTypedef;

static TYPEPTR StructDecl(DATA_TYPE,bool);
//static TYPEPTR ComplexDecl(int,bool);
static void      SetPlainCharType( DATA_TYPE char_type );
static void CheckBitfieldType( TYPEPTR typ );

#if _CPU == 386
#define _CHECK_SIZE( s )
#else
#define _CHECK_SIZE( s ) \
        if( (s) > 0xffff ) {\
            CErr1( ERR_MAX_STRUCT_SIZE_IS_64K );\
        }
#endif

/* matches enum DataType in ctypes.h */
static unsigned char  CTypeSizes[] = {
    #define pick1(enum,cgtype,x86asmtype,name,size) size,
    #include "cdatatyp.h"
    #undef  pick1
};

TYPEPTR CTypeHash[TYPE_LAST_ENTRY];
TYPEPTR PtrTypeHash[TYPE_LAST_ENTRY];

TAGPTR  TagHash[ID_HASH_SIZE + 1];
FIELDPTR FieldHash[ID_HASH_SIZE];

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
    M_COMPLEX       = 0x0400,
    M_IMAGINARY     = 0x0800,
    M_BOOL          = 0x1000,
#if _CPU == 8086
    M_INT32         = M_LONG,
#else
    M_INT32         = M_INT,
#endif
    M___LAST        = 0
};

#define TYPE_PLAIN_INT  TYPE_UFIELD

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

void InitTypeHashTables( void )
{
    parm_hash_idx   h1;
    id_hash_idx     h2;
    DATA_TYPE       base_type;

    for( h1 = 0; h1 <= MAX_PARM_LIST_HASH_SIZE; ++h1 ) {
        FuncTypeHead[h1] = NULL;
    }
    for( base_type = TYPE_BOOL; base_type < TYPE_LAST_ENTRY; ++base_type ) {
        CTypeHash[base_type] = NULL;
        PtrTypeHash[base_type] = NULL;
    }
    for( h2 = 0; h2 < ID_HASH_SIZE; ++h2 ) {
        TagHash[h2] = NULL;
        FieldHash[h2] = NULL;
    }
    TagHash[ID_HASH_SIZE] = NULL;
}

void CTypeInit( void )
{
    DATA_TYPE   base_type;

    TypeHead = NULL;
    TagCount = 0;
    FieldCount = 0;
    EnumCount = 0;
    InitTypeHashTables();
    for( base_type = TYPE_BOOL; base_type < TYPE_LAST_ENTRY; ++base_type ) {
        CTypeCounts[base_type] = 0;
#if 0
        if ( base_type == TYPE_FCOMPLEX || base_type == TYPE_DCOMPLEX || base_type == TYPE_LDCOMPLEX ) {
            BaseTypes[base_type] = ComplexDecl( TYPE_STRUCT, false );
            BaseTypes[base_type]->decl_type = base_type;
        } else {
#endif
            if( CTypeSizes[base_type] != 0 || base_type == TYPE_VOID || base_type == TYPE_DOT_DOT_DOT ) {
                BaseTypes[base_type] = TypeNode( base_type, NULL );
            } else {
                BaseTypes[base_type] = NULL;
            }
#if 0
        }
#endif
    }
    SetPlainCharType( TYPE_UCHAR );
    StringArrayType = NULL;
    VoidParmList[0] = BaseTypes[TYPE_VOID];
    VoidParmList[1] = NULL;
}


TYPEPTR GetType( DATA_TYPE base_type )
{
    TYPEPTR     typ;

    typ = BaseTypes[base_type];
    if( typ == NULL ) {
        typ = TypeNode( base_type, NULL );
    }
    return( typ );
}

void WalkFuncTypeList( void (*func)(TYPEPTR,int) )
{
    TYPEPTR         typ;
    parm_hash_idx   h;

    for( h = 0; h <= MAX_PARM_LIST_HASH_SIZE; h++ ) {
        for( typ = FuncTypeHead[h]; typ != NULL; typ = typ->next_type ) {
            func( typ, h );
        }
    }
}

void WalkTypeList( void (*func)(TYPEPTR) )
{
    TYPEPTR     typ;
    DATA_TYPE   base_type;

    for( base_type = TYPE_BOOL; base_type < TYPE_LAST_ENTRY; ++base_type ) {
        for( typ = CTypeHash[base_type]; typ != NULL; typ = typ->next_type ) {
            func( typ );
        }
    }
    for( base_type = TYPE_BOOL; base_type < TYPE_LAST_ENTRY; ++base_type ) {
        for( typ = PtrTypeHash[base_type]; typ != NULL; typ = typ->next_type ) {
            func( typ );
        }
    }
}

static TYPEPTR DupType( TYPEPTR typ, enum type_state flags, bool force_duplicate )
{
    TYPEPTR     newtype;
    TYPEPTR     next;

    if( !force_duplicate ) {
        if( typ->decl_type == TYPE_POINTER ) {
            next = PtrTypeHash[typ->object->decl_type];
        } else {
            next = CTypeHash[typ->decl_type];
        }
        for( ; next != NULL; next = next->next_type ) {
            if( next->decl_type == typ->decl_type
              && next->object == typ->object
              && next->u.tag == typ->u.tag
              && next->type_flags == flags ) {
                return( next );
            }
        }
    }
    newtype = TypeNode( typ->decl_type, typ->object );
    next = newtype->next_type;
    memcpy( newtype, typ, sizeof( TYPEDEFN ) );
    newtype->next_type = next;
    newtype->type_flags = flags;
    return( newtype );
}

static void SetPlainCharType( DATA_TYPE char_type )
{
    TYPEPTR     typ;

    typ = TypeNode( char_type, NULL );
    typ->type_flags = TF2_TYPE_PLAIN_CHAR;
    BaseTypes[TYPE_PLAIN_CHAR] = typ;
    StringType = PtrNode( typ, FLAG_NONE, SEG_DATA );
    ConstCharType =  typ;
}

void SetSignedChar( void )
{
    SetPlainCharType( TYPE_CHAR );
}


type_modifiers TypeQualifier( void )
{
    type_modifiers   flags;
    type_modifiers   bit;

    flags = 0;
    bit = 0;
    for( ;; ) {
        if( flags & bit )
            CErr1( ERR_REPEATED_MODIFIER );
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
        if( CurToken == T_RESTRICT || CurToken == T___RESTRICT ) {
            bit = FLAG_RESTRICT;
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

static TYPEPTR GetScalarType( bool *plain_int, int bmask, type_modifiers flags )
{
    DATA_TYPE   data_type;
    TYPEPTR     typ;

    data_type = TYPE_UNDEFINED;
    if( bmask & M_LONG_LONG ) {
        bmask &= ~M_INT;
    }
    if( bmask & (M_VOID | M_FLOAT | M_DOUBLE | M_LONG_LONG | M_COMPLEX | M_IMAGINARY) ) {
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
            if( CompFlags.use_long_double ) {
                data_type = TYPE_LONG_DOUBLE;
            } else {
                data_type = TYPE_DOUBLE;
            }
        } else if( bmask == (M_COMPLEX | M_FLOAT) ) {
            data_type = TYPE_FCOMPLEX;
        } else if( bmask == (M_COMPLEX | M_DOUBLE) ) {
            data_type = TYPE_DCOMPLEX;
        } else if( bmask == (M_COMPLEX | M_LONG | M_DOUBLE) ) {
            if( CompFlags.use_long_double ) {
                data_type = TYPE_LDCOMPLEX;
            } else {
                data_type = TYPE_DCOMPLEX;
            }
        } else if( bmask == (M_IMAGINARY | M_FLOAT) ) {
            data_type = TYPE_FIMAGINARY;
        } else if( bmask == (M_IMAGINARY | M_DOUBLE) ) {
            data_type = TYPE_DIMAGINARY;
        } else if( bmask == (M_IMAGINARY | M_LONG | M_DOUBLE) ) {
            if( CompFlags.use_long_double ) {
                data_type = TYPE_LDIMAGINARY;
            } else {
                data_type = TYPE_DIMAGINARY;
            }
        } else {
            data_type = TYPE_UNDEFINED;
        }
    } else if( bmask == M_BOOL ) {
        data_type = TYPE_BOOL;
    } else if( bmask == 0 ) {
        data_type = TYPE_INT;
        *plain_int = true;
    } else {
        data_type = Valid_Types[bmask];
        if( data_type == TYPE_PLAIN_INT ) {
            data_type = TYPE_INT;
            *plain_int = true;
        }
    }
    if( data_type == TYPE_UNDEFINED ) {
        CErr1( ERR_INV_TYPE );
        data_type = TYPE_INT;
    }
    typ = GetType( data_type );
    if( flags & FLAG_SEGMENT )
        typ = DupType( typ, TF2_TYPE_SEGMENT, false );

    return( typ );
}


static void AdvanceToken( void )
{
    if( CurToken == T_SAVED_ID ) {
        CMemFree( SavedId );
        SavedId = NULL;
        CurToken = LAToken;
    } else {
        NextToken();
    }
}

static void DeclSpecifiers( bool *plain_int, decl_info *info )
{
    TYPEPTR             typ;
    int                 bmask;
    int                 bit;
    type_modifiers      flags;
    bool                packed;
    SYM_HANDLE          sym_handle;
    stg_classes         stg_class;
    stg_classes         specified_stg_class;
    SYM_ENTRY           sym;
    declspec_class      decl;
    type_modifiers      modifier;

    *plain_int = false;
    info->mod = FLAG_NONE;
    info->decl_mod = FLAG_NONE;
    info->decl = DECLSPEC_NONE;
    info->naked = false;
    info->segid = SEG_NULL;
    bmask = 0;
    flags = FLAG_NONE;
    packed = false;
    typ = NULL;
    specified_stg_class = SC_NONE;
    for(;;) {
        stg_class = SC_NONE;
        bit = 0;
        switch( CurToken ) {
        case T_CHAR:
            bit = M_CHAR;
            break;
        case T_INT:
            bit = M_INT;
            break;
        case T_SHORT:
            bit = M_SHORT;
            break;
        case T_LONG:
            bit = M_LONG;
            break;
        case T___INT8:
            bit = M_CHAR;
            break;
        case T___INT16:
            bit = M_SHORT;
            break;
        case T___INT32:
            bit = M_INT32;
            break;
        case T___INT64:
            bit = M_LONG_LONG;
            break;
        case T_SIGNED:
            bit = M_SIGNED;
            break;
        case T_UNSIGNED:
            bit = M_UNSIGNED;
            break;
        case T_FLOAT:
            bit = M_FLOAT;
            break;
        case T_DOUBLE:
            bit = M_DOUBLE;
            break;
        case T_VOID:
            bit = M_VOID;
            break;
        case T__COMPLEX:
            bit = M_COMPLEX;
            break;
        case T__IMAGINARY:
            bit = M_IMAGINARY;
            break;
        case T__BOOL:
            bit = M_BOOL;
            break;
        case T_CONST:
            if( flags & FLAG_CONST )
                CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_CONST;
            break;
        case T_VOLATILE:
            if( flags & FLAG_VOLATILE )
                CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_VOLATILE;
            break;
        case T_RESTRICT:
        case T___RESTRICT:
            if( flags & FLAG_RESTRICT )
                CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_RESTRICT;
            break;
        case T___UNALIGNED:
            if( flags & FLAG_UNALIGNED )
                CErr1( ERR_REPEATED_MODIFIER );
            flags |= FLAG_UNALIGNED;
            break;
        case T_INLINE:
        case T___INLINE:
            flags |= FLAG_INLINE;
            break;
        case T__PACKED:
            if( packed )
                CErr1( ERR_REPEATED_MODIFIER );
            packed = true;
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
            if( typ != NULL )
                CErr1( ERR_INV_TYPE );
            typ = StructDecl( TYPE_STRUCT, packed );
            packed = false;
            continue;
        case T_UNION:
            if( typ != NULL )
                CErr1( ERR_INV_TYPE );
            typ = StructDecl( TYPE_UNION, packed );
            packed = false;
            continue;
        case T_ENUM:
            if( typ != NULL )
                CErr1( ERR_INV_TYPE );
            typ = EnumDecl( flags );
            continue;
        case T___SEGMENT:
            bit = M_UNSIGNED | M_SHORT;
            flags |= FLAG_SEGMENT;
            break;

        case T___DECLSPEC:
            AdvanceToken();                   // declspec( dllimport naked )
            MustRecog( T_LEFT_PAREN );
            while( CurToken != T_RIGHT_PAREN ) {
                modifier = 0;
                switch( CurToken ) {
                case T___WATCALL:
                    modifier = LANG_WATCALL;
                    break;
                case T__CDECL:
                case T___CDECL:
                    modifier = LANG_CDECL;
                    break;
                case T__PASCAL:
                case T___PASCAL:
                    modifier = LANG_PASCAL;
                    break;
                case T___FORTRAN:
                    modifier = LANG_FORTRAN;
                    break;
                case T__SYSCALL:
                case T___SYSCALL:
                case T__SYSTEM:
                    modifier = LANG_SYSCALL;
                    break;
                case T___STDCALL:
                    modifier = LANG_STDCALL;
                    break;
                case T__FASTCALL:
                case T___FASTCALL:
                    modifier = LANG_FASTCALL;
                    break;
                case T__OPTLINK:
                    modifier = LANG_OPTLINK;
                    break;
                case T_ID:
                    decl = DECLSPEC_NONE;
                    if( info->stg == SC_NONE ) {
                        CErr1( ERR_INVALID_DECLARATOR );
                        break;
                    }
                    if( CMPLIT( Buffer, "dllimport" ) == 0 ) {
                        decl = DECLSPEC_DLLIMPORT;
                    } else if( CMPLIT( Buffer, "overridable" ) == 0 ) {
                        decl = DECLSPEC_DLLIMPORT;
                    } else if( CMPLIT( Buffer, "dllexport" ) == 0 ) {
                        decl = DECLSPEC_DLLEXPORT;
                    } else if( CMPLIT( Buffer, "thread" ) == 0 ) {
                        decl = DECLSPEC_THREAD;
                    } else if( CMPLIT( Buffer, "naked" ) == 0 ) {
                        if( info->naked ) {
                            CErr1( ERR_INVALID_DECLSPEC );
                        } else {
                            info->naked = true;
                        }
                    } else if( CMPLIT( Buffer, "aborts" ) == 0 ) {
                        modifier = FLAG_ABORTS;
                    } else if( CMPLIT( Buffer, "noreturn" ) == 0 ) {
                        modifier = FLAG_NORETURN;
                    } else if( CMPLIT( Buffer, "farss" ) == 0 ) {
                        modifier = FLAG_FARSS;
                    } else {
                        CErr1( ERR_INVALID_DECLSPEC );
                    }
                    if( decl != DECLSPEC_NONE ) {
                        if( info->decl == DECLSPEC_NONE ) {
                            info->decl = decl;
                        } else {
                            CErr1( ERR_INVALID_DECLSPEC );
                        }
                    }
                    break;
                default:
                    CErr1( ERR_INVALID_DECLSPEC );
                    goto done;
                }
                if( modifier & MASK_LANGUAGES ) {
                    if( info->decl_mod & MASK_LANGUAGES ) {
                        CErr1( ERR_INVALID_DECLSPEC );
                    } else {
                        info->decl_mod |= modifier;
                    }
                }
                if( modifier & FLAG_ABORTS ) {
                    if( info->decl_mod & FLAG_ABORTS ) {
                        CErr1( ERR_INVALID_DECLSPEC );
                    } else {
                        info->decl_mod |= modifier;
                    }
                }
                if( modifier & FLAG_NORETURN ) {
                    if( info->decl_mod & FLAG_NORETURN ) {
                        CErr1( ERR_INVALID_DECLSPEC );
                    } else {
                        info->decl_mod |= modifier;
                    }
                }
                if( modifier & FLAG_FARSS ) {
                    if( info->decl_mod & FLAG_FARSS ) {
                        CErr1( ERR_INVALID_DECLSPEC );
                    } else {
                        info->decl_mod |= modifier;
                    }
                }
                NextToken();
            }
         done:
            MustRecog( T_RIGHT_PAREN );
            continue;
        case T_SAVED_ID:
        case T_ID:
            if( typ != NULL || bmask != 0 )
                goto got_specifier;
            /* lookup id in symbol table */
            /* if valid type identifier then OK */
            if( CurToken == T_ID ) {
                sym_handle = SymLookTypedef( HashValue, Buffer, &sym );
            } else {    /* T_SAVED_ID */
                sym_handle = SymLookTypedef( SavedHash, SavedId, &sym );
            }
            if( sym_handle == SYM_NULL )
                goto got_specifier;
            if( sym.attribs.stg_class != SC_TYPEDEF )
                goto got_specifier;
            if( SymLevel != 0 && flags == 0 ) {
                if( CurToken == T_ID ) {
                    LookAhead();
                    if( LAToken == T_COLON ) {
                        goto got_specifier;
                    }
                }
            }
            ++SymTypedef;
            typ = sym.sym_type;
            SymGet( &sym, sym_handle );  // get rest of sym from nutty sym table
            if( flags & sym.mods ) {
                CErr1( ERR_INV_TYPE );
            }
            flags |= sym.mods;
            if( sym.mods & FLAG_BASED ) {
                info->segid = sym.u.var.segid;
            }
            if( sym.attribs.declspec != DECLSPEC_NONE ) {
                if( info->decl == DECLSPEC_NONE ) {
                    info->decl = sym.attribs.declspec;
                } else {
                    CErr1( ERR_INVALID_DECLSPEC );
                }
            }
            if( sym.attribs.naked ) {
                if( info->naked ) {
                    CErr1( ERR_INVALID_DECLSPEC );
                } else {
                    info->naked = true;
                }
            }
            AdvanceToken();
            continue;
        default:
            goto got_specifier;
        }
        if( stg_class != SC_NONE ) {
            if( info->stg == SC_NONE )
                break;       // don't want any stg class
            if( specified_stg_class != SC_NONE ) {
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
        if( bmask != 0 ) {
            CErr1( ERR_INV_TYPE );  // picked up an int
        }
    } else {
        if( flags != FLAG_NONE || bmask != 0 ) {  // not just id hanging there
            typ = GetScalarType( plain_int, bmask, flags );
        }
    }
    info->typ = typ;
    info->mod = flags;
}

void TypeSpecifier( decl_info *info )
{
    bool                plain_int;

    info->stg = SC_NONE;      // indicate don't want any storage class specifiers
    DeclSpecifiers( &plain_int, info );
}

static void GetFieldTypeSpecifier( bool *plain_int, decl_info *info )
{

    info->stg = SC_NONE;      // indicate don't want any storage class specifiers
    DeclSpecifiers( plain_int, info );
}

void FullDeclSpecifier( decl_info *info )
{
    bool        plain_int;

    info->stg = SC_FORWARD;    // indicate want storage class specifiers
    DeclSpecifiers( &plain_int, info );
}

TYPEPTR TypeDefault( void )
{
    return( GetType( TYPE_INT ) );
}


static TAGPTR NewTag( const char *name, id_hash_idx h )
{
    TAGPTR      tag;
    size_t      len;

    len = strlen( name ) + 1;
    tag = (TAGPTR)CPermAlloc( sizeof( TAGDEFN ) - 1 + len );
    tag->level = (id_level_type)SymLevel;
    tag->hash = h;
    tag->next_tag = TagHash[h];
    TagHash[h] = tag;
    memcpy( tag->name, name, len );
    ++TagCount;
    return( tag );
}


TAGPTR NullTag( void )
{
    return( NewTag( "", ID_HASH_SIZE ) );
}


TAGPTR VfyNewTag( TAGPTR tag, DATA_TYPE tag_type )
{
    if( tag->sym_type != NULL ) {               /* tag already exists */
        if( !ChkEqSymLevel( tag ) ) {
            tag = NewTag( tag->name, tag->hash );
        } else if( tag->size != 0 || tag->sym_type->decl_type != tag_type ) {
            CErr2p( ERR_DUPLICATE_TAG, tag->name );
        }
    }
    return( tag );
}


static FIELDPTR NewField( FIELDPTR new_field, TYPEPTR decl )
{
    FIELDPTR    field;
    FIELDPTR    prev_field;
    TYPEPTR     typ;
    TAGPTR      tag;
    size_t      len;

    ++FieldCount;
    typ = new_field->field_type;
    if( typ != NULL ) {
        SKIP_TYPEDEFS( typ );
    }
    if( new_field->name[0] == '\0' ) {
        /* allow nameless structs and unions */
        if( (typ->decl_type != TYPE_STRUCT && typ->decl_type != TYPE_UNION)
          || !CompFlags.extensions_enabled ) {
            CErr1( ERR_INVALID_DECLARATOR );
        }
    }
    if( typ == decl ) {
        CErr1( ERR_STRUCT_OR_UNION_INSIDE_ITSELF );
    } else if( SizeOfArg( typ ) == 0 ) {   /* was TypeSize(typ) */
        /* can't have an array of incomplete type */
        if( typ->decl_type == TYPE_ARRAY
          && ( SizeOfArg( typ->object ) == 0 || !CompFlags.extensions_enabled )
          || typ->decl_type != TYPE_ARRAY ) {
            CErr2p( ERR_INCOMPLETE_TYPE, new_field->name );
        }
    }
    tag = decl->u.tag;
    new_field->hash = HashValue;
    if( new_field->name[0] != '\0' ) {  /* only check non-empty names */
        len = strlen( new_field->name ) + 1;
        for( field = FieldHash[HashValue]; field != NULL; field = field->next_field_same_hash ) {
            /* fields were added at the front of the hash linked list --
               may as well stop if the level isn't the same anymore */
            if( field->level != new_field->level )
                break;
            if( memcmp( field->name, new_field->name, len ) == 0 ) {
                CErr2p( ERR_DUPLICATE_FIELD_NAME, field->name );
            }
        }
        new_field->next_field_same_hash = FieldHash[HashValue];
        FieldHash[HashValue] = new_field;
    }
    if( tag->u.field_list == NULL ) {
        tag->u.field_list = new_field;
    } else {
        prev_field = tag->u1.last_field;
        prev_field->next_field = new_field;
        if( SizeOfArg( prev_field->field_type ) == 0 ) {
            CErr2p( ERR_INCOMPLETE_TYPE, prev_field->name );
        }
    }
    tag->u1.last_field = new_field;
    return( new_field );
}


static TYPEPTR EnumFieldType( TYPEPTR ftyp, bool plain_int,
                    bitfield_width start, bitfield_width width )
{
    TYPEPTR     typ;
    DATA_TYPE   data_type;

    typ = TypeNode( TYPE_FIELD, NULL );
    typ->u.f.field_start = start;
    typ->u.f.field_width = width;
    if( plain_int ) {
        data_type = TYPE_INT;   /* default to signed bit fields */
    } else {
        SKIP_TYPEDEFS( ftyp );
        if( ftyp->decl_type == TYPE_ENUM ) {
            ftyp = ftyp->object;
        }
        data_type = ftyp->decl_type;
    }
    typ->u.f.field_type = data_type;
    switch( data_type ) {
    case TYPE_BOOL:
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


align_type GetTypeAlignment( TYPEPTR typ )
{
    align_type  size;

    while( typ->decl_type == TYPE_TYPEDEF || typ->decl_type == TYPE_ARRAY ) {
        typ = typ->object;
    }
    if( typ->decl_type == TYPE_STRUCT || typ->decl_type == TYPE_UNION ) {
        size = typ->u.tag->alignment;
#if _CPU == _AXP
        if( CompFlags.align_structs_on_qwords ) {
            size = 8;
        }
#endif
    } else {
        size = (align_type)TypeSize( typ );
    }
    return( size );
}


static target_size FieldAlign( target_size next_offset, FIELDPTR field, align_type *worst_alignment )
{
    align_type  pack_adjustment;
    align_type  align;
    target_size old_offset;

    pack_adjustment = PackAmount;
    align = GetTypeAlignment( field->field_type );
    if( align > pack_adjustment ) { // can't be any bigger than pack( x )
        align = pack_adjustment;
    }
    if( align > *worst_alignment ) {
        *worst_alignment = align;
    }
    if( align != 1 ) {
        old_offset = next_offset;
        next_offset = _RoundUp( next_offset, align );
        if( CompFlags.slack_byte_warning && (next_offset - old_offset) ) {
            CWarn2( WARN_LEVEL_1, ERR_SLACK_ADDED, (unsigned)( next_offset - old_offset ) );
        }
    }
    field->offset = next_offset;
    return( next_offset );
}

static DATA_TYPE UnQualifiedType( TYPEPTR typ )
{
    // skip typedefs, go into enum base
    typ = SkipTypeFluff( typ );
    switch( typ->decl_type ) {
    case TYPE_BOOL:
        return( TYPE_BOOL );
    case TYPE_CHAR:
    case TYPE_UCHAR:
        return( TYPE_CHAR );
    case TYPE_SHORT:
    case TYPE_USHORT:
        return( TYPE_SHORT );
    case TYPE_INT:
    case TYPE_UINT:
        return( TYPE_INT );
    case TYPE_LONG:
    case TYPE_ULONG:
        return( TYPE_LONG );
    case TYPE_LONG64:
    case TYPE_ULONG64:
        return( TYPE_LONG64 );
    default:
        break;
    }
    return( TYPE_UNDEFINED );
}

/* clear the hash table of all fields that were just defined
   in the struct with tag tag */
static void ClearFieldHashTable( TAGPTR tag )
{
    FIELDPTR field;
    FIELDPTR hash_field;
    FIELDPTR prev_field;

    for( field = tag->u.field_list; field != NULL; field = field->next_field ) {
        prev_field = NULL;
        hash_field = FieldHash[field->hash];
        if( hash_field == field ) {
            /* first entry: easy kick out */
            FieldHash[field->hash] = field->next_field_same_hash;
        } else {
            while( hash_field != NULL ) {
                /* search for candidate to kick */
                prev_field = hash_field;
                hash_field = hash_field->next_field_same_hash;
                if( hash_field == field ) {
                     hash_field = hash_field->next_field_same_hash;
                     prev_field->next_field_same_hash = hash_field;
                }
            }
        }
    }
}

static bool CheckAdjModsTypeNode( type_modifiers old_mod, type_modifiers decl_mod )
{
    bool        adjust;

    adjust = false;
    if( (decl_mod & MASK_LANGUAGES) && (old_mod & MASK_LANGUAGES) != (decl_mod & MASK_LANGUAGES) ) {
        if( old_mod & MASK_LANGUAGES ) {
            CErr1( ERR_INVALID_DECLSPEC );
        } else {
            adjust = true;
        }
    }
    if( (decl_mod & FLAG_ABORTS) && (old_mod & FLAG_ABORTS) == 0 ) {
        adjust = true;
    }
    if( (decl_mod & FLAG_NORETURN) && (old_mod & FLAG_NORETURN) == 0 ) {
        adjust = true;
    }
    if( (decl_mod & FLAG_FARSS) && (old_mod & FLAG_FARSS) == 0 ) {
        adjust = true;
    }
    return( adjust );
}

void AdjModsTypeNode( TYPEPTR *ptyp, type_modifiers decl_mod, SYMPTR sym )
{
    if( decl_mod ) {
        TYPEPTR     typ;

        if( sym != NULL ) {
            if( CheckAdjModsTypeNode( sym->mods, decl_mod ) ) {
                sym->mods |= decl_mod;
            }
        }
        typ = *ptyp;
        if( typ->decl_type == TYPE_FUNCTION ) {
            if( CheckAdjModsTypeNode( typ->u.fn.decl_flags, decl_mod ) ) {
                *ptyp = FuncNode( typ->object, typ->u.fn.decl_flags | decl_mod, typ->u.fn.parms );
            }
        }
    }
}

static target_size GetFields( TYPEPTR decl )
{
    target_size         start;
    TYPEPTR             typ;
    decl_state          state;
    FIELDPTR            field;
    unsigned            scalar_size;
    unsigned            bits_available;
    unsigned            bits_total;
    target_size         struct_size;
    target_size         next_offset;
    unsigned            width;
    align_type          worst_alignment;
    DATA_TYPE           unqualified_type;
    DATA_TYPE           prev_unqualified_type;
    bool                plain_int;
    decl_info           info;
    static field_level_stype struct_level = 0;

    struct_level++;
    prev_unqualified_type = TYPE_VOID;   /* so it doesn't match 1st time */
    start = 0;
    worst_alignment = 1;
    bits_available = 1;
    bits_total = 0;
    /* assertion: bits_available != bits_total && bits_total >> 3 == 0 */
    struct_size = start;
    next_offset = start;
    for(;;) {
        if( CurToken == T_SEMI_COLON && CompFlags.extensions_enabled ) {
            NextToken();
            if( CurToken == T_RIGHT_BRACE )
                break;
            continue;
        }
        GetFieldTypeSpecifier( &plain_int, &info );
        typ = info.typ;
        state = DECL_STATE_NONE;
        if( typ == NULL ) {
            state |= DECL_STATE_NOTYPE;
            CErr2p( ERR_MISSING_DATA_TYPE, Buffer );
            typ = TypeDefault();
            if( CurToken == T_ID ) {
                NextToken();
            }
        }
        unqualified_type = UnQualifiedType( typ );
        if( bits_available == bits_total || decl->decl_type == TYPE_UNION ) {
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
                field = FieldDecl( typ, info.mod, state );
                field->level = struct_level;
                AdjModsTypeNode( &field->field_type, info.decl_mod, NULL );
                field = NewField( field, decl );
            }
            if( CurToken == T_COLON ) {
                if( field != NULL ) {
                    next_offset = FieldAlign( next_offset, field, &worst_alignment );
                }
                CheckBitfieldType( typ );
                NextToken();
                width = ConstExpr();
                if( width == 0 && field != NULL ) {
                    CErr1( ERR_WIDTH_0 );
                }
                if( (int)width < 0 ) {
                    CErr1( ERR_WIDTH_NEGATIVE );
                    width = 0;
                } else if( width > TARGET_BITS || width > bits_total ) {
                    CErr1( ERR_FIELD_TOO_WIDE );
                    width = TARGET_BITS;
                }
                if( width > bits_available || width == 0 ) {
                    scalar_size = TypeSize( typ );
                    if( bits_available != bits_total ) {
                        /* some bits have been used; abandon this unit */
                        next_offset += bits_total >> 3;
                    } else if( width == 0 ) {
                        /* no bits have been used; align to base type */
                        next_offset = _RoundUp( next_offset, scalar_size );
                    }
                    bits_available = scalar_size * 8;
                    bits_total = bits_available;
                }
                if( field != NULL ) {
                    field->offset = next_offset;
                    field->field_type = EnumFieldType( typ, plain_int,
                              (bitfield_width)( bits_total - bits_available ),
                              (bitfield_width)width );
                }
                bits_available -= width;
            } else {
                if( bits_available != bits_total ) { //changed from bit field to non
                    next_offset += bits_total >> 3;
                    field->offset = next_offset;
                    bits_available = TypeSize( typ ) * 8;
                    bits_total = bits_available;
                }
                next_offset = FieldAlign( next_offset, field, &worst_alignment );
                next_offset += SizeOfArg( field->field_type );
            }
            if( next_offset > struct_size )
                struct_size = next_offset;
            if( decl->decl_type == TYPE_UNION ) {
                next_offset = start;
                bits_available = bits_total;
                if (field && struct_size < SizeOfArg(field->field_type))
                    struct_size = SizeOfArg(field->field_type);
            }
            if( CurToken != T_COMMA )
                break;
            NextToken();
        }
        if( CurToken == T_RIGHT_BRACE ) {
            CWarn1( WARN_MISSING_LAST_SEMICOLON, ERR_MISSING_LAST_SEMICOLON );
        } else {
            MustRecog( T_SEMI_COLON );
        }
        if( CurToken == T_RIGHT_BRACE ) {
            break;
        }
    }
    if( bits_available != bits_total ) { /* if last field was bit field */
        next_offset += bits_total >> 3;
        if( next_offset > struct_size ) {
            struct_size = next_offset;
        }
    }
    ClearFieldHashTable( decl->u.tag );
    decl->u.tag->alignment = worst_alignment;
    struct_size = _RoundUp( struct_size, worst_alignment );
    _CHECK_SIZE( struct_size );
    NextToken();
    struct_level--;
    return( struct_size );
}


static TYPEPTR StructDecl( DATA_TYPE decl_typ, bool packed )
{
    TYPEPTR     typ;
    TAGPTR      tag;
    align_type  saved_packamount;

    saved_packamount = PackAmount;
    if( packed )
        PackAmount = 1;
    NextToken();
    if( CurToken == T_LEFT_BRACE ) {
        tag = NullTag();
        NextToken();
    } else {
        for(;;) {
            if( CurToken == T_ID )
                break;
            if( CurToken == T_LEFT_BRACE )
                break;
            if( CurToken == T_EOF )
                break;
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
                if( !ChkEqSymLevel( tag ) ) {
                    tag = NewTag( tag->name, tag->hash );
                }
            }
            typ = tag->sym_type;
            if( typ == NULL ) {
                typ = TypeNode( decl_typ, NULL );
            } else {
                if( typ->decl_type != decl_typ ) {
                    CErr2p( ERR_DUPLICATE_TAG, tag->name );
                }
            }
            tag->sym_type = typ;
            typ->u.tag = tag;
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
    PackAmount = saved_packamount;
    return( typ );
}

/*
Next three functions descripe a struct that looks like
struct {
    double __ow_real;
    double _Imaginary __ow_imaginary;
}
*/

/*
static void GetComplexFieldTypeSpecifier( decl_info *info, DATA_TYPE data_type )
{
    info->stg = SC_NONE;      // indicate don't want any storage class specifiers
    info->mod = FLAG_NONE;
    info->decl_mod = FLAG_NONE;
    info->decl = DECLSPEC_NONE;
    info->naked = false;
    info->segid = SEG_NULL;
    info->typ = GetType( data_type );
}


static target_size GetComplexFields( TYPEPTR decl )
{
    target_size         start;
    TYPEPTR             typ;
    decl_state          state;
    FIELDPTR            field;
    target_size         struct_size;
    target_size         next_offset;
    align_type          worst_alignment;
    decl_info           info;

    start = 0;
    worst_alignment = 1;

    struct_size = start;
    next_offset = start;


    GetComplexFieldTypeSpecifier( &info, TYPE_DOUBLE );

    field = FieldCreate( "__ow_real" );
    field->attrib = 0;
    field->field_type = info.typ;

    field = NewField( field, decl );

    typ = info.typ;
    state = DECL_STATE_NONE;

    next_offset = FieldAlign( next_offset, field, &worst_alignment );
    next_offset += SizeOfArg( field->field_type );
    if( next_offset > struct_size )
        struct_size = next_offset;

    GetComplexFieldTypeSpecifier( &info, TYPE_DIMAGINARY );

    field = FieldCreate( "__ow_imaginary" );
    field->attrib = 0;
    field->field_type = info.typ;
    field = NewField( field, decl );

    typ = info.typ;
    state = DECL_STATE_NONE;

    next_offset = FieldAlign( next_offset, field, &worst_alignment );
    next_offset += SizeOfArg( field->field_type );
    if( next_offset > struct_size )
        struct_size = next_offset;

    decl->u.tag->alignment = worst_alignment;
    struct_size = _RoundUp( struct_size, worst_alignment );
    _CHECK_SIZE( struct_size );
    return( struct_size );
}


static TYPEPTR ComplexDecl( DATA_TYPE decl_typ, bool packed )
{
    TYPEPTR     typ;
    TAGPTR      tag;
    align_type  saved_packamount;

    saved_packamount = PackAmount;
    if( packed )
        PackAmount = 1;

    tag = NullTag();

    typ = tag->sym_type;
    if( typ == NULL ) {
        typ = TypeNode( decl_typ, NULL );
        tag->sym_type = typ;
    }

    typ->u.tag = tag;
    tag->u.field_list = NULL;
    tag->size = GetComplexFields( typ );
    PackAmount = saved_packamount;

    return( typ );
}
*/

static void CheckBitfieldType( TYPEPTR typ )
{
    // skip typedefs
    SKIP_TYPEDEFS( typ );
    if( CompFlags.extensions_enabled ) {
        // go into enum base
        SKIP_ENUM( typ );
    }
    switch( typ->decl_type ) {
    case TYPE_INT:
    case TYPE_UINT:
    case TYPE_BOOL:
        /* ANSI C only allows int and unsigned [int]; C99 adds _Bool */
        return;
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_LONG:
    case TYPE_ULONG:
    case TYPE_LONG64:
    case TYPE_ULONG64:
        if( CompFlags.extensions_enabled ) {
            return;
        }
        break;
    default:
        break;
    }
    CErr1( ERR_INVALID_TYPE_FOR_FIELD );
}


void VfyNewSym( id_hash_idx h, const char *name )
{
    SYM_HANDLE  sym_handle;
    SYM_ENTRY   sym;
    ENUMPTR     ep;

    ep = EnumLookup( h, name );
    if( ep != NULL && ChkEqSymLevel( ep->parent ) ) {
        SetDiagEnum( ep );
        CErr2p( ERR_SYM_ALREADY_DEFINED, name );
        SetDiagPop();
    }
    sym_handle = SymLook( h, name );
    if( sym_handle != SYM_NULL ) {
        SymGet( &sym, sym_handle );
        if( ChkEqSymLevel( &sym ) ) {
            SetDiagSymbol( &sym, sym_handle );
            CErr2p( ERR_SYM_ALREADY_DEFINED, name );
            SetDiagPop();
        }
    }
}


TAGPTR TagLookup( void )
{
    TAGPTR          tag;

    for( tag = TagHash[HashValue]; tag != NULL; tag = tag->next_tag ) {
        if( memcmp( Buffer, tag->name, TokenLen + 1 ) == 0 ) {
            return( tag );
        }
    }
    return( NewTag( Buffer, HashValue ) );
}

void FreeTags( void )
{
    TAGPTR          tag;
    id_hash_idx     h;

    for( h = 0; h <= ID_HASH_SIZE; ++h ) {
        for( ; (tag = TagHash[h]) != NULL; ) {
            if( ChkLtSymLevel( tag ) )
                break;
            TagHash[h] = tag->next_tag;
            tag->next_tag = DeadTags;
            DeadTags = tag;
        }
    }
}

void WalkTagList( void (*func)(TAGPTR) )
{
    TAGPTR          tag;
    id_hash_idx     h;

    for( h = 0; h <= ID_HASH_SIZE; ++h ) {
        for( tag = TagHash[h]; tag != NULL; tag = tag->next_tag ) {
            func( tag );
        }
    }
}

void AddTypeHash( TYPEPTR typ )
{
    if( typ->decl_type == TYPE_POINTER ) {
        if( typ->object != NULL ) {
            typ->next_type = PtrTypeHash[typ->object->decl_type];
            PtrTypeHash[typ->object->decl_type] = typ;
        }
    } else if( typ->decl_type == TYPE_FUNCTION ) {
        typ->next_type = NULL;
    } else {
        typ->next_type = CTypeHash[typ->decl_type];
        CTypeHash[typ->decl_type] = typ;
    }
}

void AddPtrTypeHash( TYPEPTR typ )
{
    typ->next_type = PtrTypeHash[typ->object->decl_type];
    PtrTypeHash[typ->object->decl_type] = typ;
}

TYPEPTR TypeNode( DATA_TYPE type_spec, TYPEPTR the_object )
{
    TYPEPTR     typ;

    typ = (TYPEPTR) CPermAlloc( sizeof( TYPEDEFN ) );
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
    typ->u.array = CPermAlloc( sizeof( array_info ) );
    return( typ );
}


static TYPEPTR MkPtrNode( TYPEPTR typ, type_modifiers flags,
    segment_id segid, SYM_HANDLE base, BASED_KIND based_kind )
{
    TYPEPTR     ptrtyp;

    if( typ != NULL ) {
        ptrtyp = PtrTypeHash[typ->decl_type];
        for( ; ptrtyp != NULL; ptrtyp = ptrtyp->next_type ) {
            if( ptrtyp->decl_type == TYPE_POINTER &&
                ptrtyp->object == typ &&
                ptrtyp->u.p.segid == segid &&
                ptrtyp->u.p.based_sym == base &&
                ptrtyp->u.p.based_kind == based_kind &&
                ptrtyp->u.p.decl_flags == flags ) {
                return( ptrtyp );
            }
        }
    }
    ptrtyp = TypeNode( TYPE_POINTER, typ );
    ptrtyp->u.p.decl_flags = flags;
    ptrtyp->u.p.segid = segid;
    ptrtyp->u.p.based_sym = base;
    ptrtyp->u.p.based_kind = based_kind;
    return( ptrtyp );
}

TYPEPTR PtrNode( TYPEPTR typ, type_modifiers flags, segment_id segid )
{
    return( MkPtrNode( typ, flags, segid, SYM_NULL, BASED_NONE ) );
}

TYPEPTR BPtrNode( TYPEPTR typ, type_modifiers flags, segment_id segid, SYM_HANDLE base, BASED_KIND kind )
{
    return( MkPtrNode( typ, flags, segid, base, kind ) );
}

static parm_hash_idx FuncHeadIndex( TYPEPTR *parm_types )
{
    parm_hash_idx  h;

    if( parm_types == NULL )
        return( 0 );
    for( h = 0; h < MAX_PARM_LIST_HASH_SIZE; ++h ) {
        if( *parm_types == NULL ) {
            break;
        }
        ++parm_types;
    }
    return( h );
}

TYPEPTR FuncNode( TYPEPTR return_typ, type_modifiers flag, TYPEPTR *parm_types )
{
    TYPEPTR         typ;
    parm_hash_idx   h;

    h = FuncHeadIndex( parm_types );
    if( return_typ != NULL ) {
        for( typ = FuncTypeHead[h]; typ != NULL; typ = typ->next_type ) {
            if( typ->object == return_typ &&
                typ->u.fn.decl_flags == flag &&
                typ->u.fn.parms == parm_types ) {
                return( typ );
            }
        }
    }
    typ = TypeNode( TYPE_FUNCTION, return_typ );
    typ->u.fn.decl_flags = flag;
    typ->u.fn.parms = parm_types;
    typ->next_type = FuncTypeHead[h];
    FuncTypeHead[h] = typ;
    return( typ );
}

target_size TypeSize( TYPEPTR typ )
{
    return( TypeSizeEx( typ, NULL ) );
}

target_size TypeSizeEx( TYPEPTR typ, bitfield_width *pFieldWidth )
{
    target_size size;

    if( typ == NULL )
        return( 0 );
    SKIP_TYPEDEFS( typ );
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
    case TYPE_LONG_DOUBLE:
    case TYPE_FCOMPLEX:
    case TYPE_DCOMPLEX:
    case TYPE_LDCOMPLEX:
    case TYPE_FIMAGINARY:
    case TYPE_DIMAGINARY:
    case TYPE_LDIMAGINARY:
    case TYPE_BOOL:
        size = CTypeSizes[typ->decl_type];
        break;
    case TYPE_VOID:
        if( CompFlags.unix_ext ) {
            size = 1;
        } else {
            size = 0;
        }
        break;
    case TYPE_POINTER:
        if( typ->u.p.decl_flags & ( FLAG_FAR | FLAG_HUGE ) ) {
            size = TARGET_FAR_POINTER;
        } else if( typ->u.p.decl_flags & FLAG_NEAR ) {
            size = TARGET_POINTER;
        } else {
            typ = typ->object;
            SKIP_TYPEDEFS( typ );
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
        if( typ->object != NULL ) {
            /* structure has a zero length array as last field */
            typ = typ->object;  /* point to TYPE_ARRAY entry */
            size += SizeOfArg( typ );
        }
        break;
    case TYPE_UNION:
    case TYPE_ENUM:
        size = typ->u.tag->size;
        break;
    case TYPE_FIELD:
    case TYPE_UFIELD:
        size = CTypeSizes[typ->u.f.field_type];
        if( pFieldWidth != NULL ) {
            *pFieldWidth = typ->u.f.field_width;
        }
        break;
    default:
        size = 0;
    }
    return( size );
}

/* Return an integer type of specified size, or NULL in case of failure.
 * The type will be signed if 'sign' is true. The type will have exactly
 * requested size if 'exact' is true, or the next larger type will be
 * returned (eg. 64-bit integer if 6 byte size is requested).
 */
TYPEPTR GetIntTypeBySize( target_size size, bool sign, bool exact )
{
    static const DATA_TYPE  s_types[] = { TYPE_CHAR, TYPE_SHORT, TYPE_INT, TYPE_LONG, TYPE_LONG64 };
    static const DATA_TYPE  u_types[] = { TYPE_UCHAR, TYPE_USHORT, TYPE_UINT, TYPE_ULONG, TYPE_ULONG64 };
    const DATA_TYPE         *type_list;
    DATA_TYPE               type_id;
    TYPEPTR                 typ = NULL;
    unsigned                i;

    /* Make sure the types are laid out the way we expect */
    assert( TYPE_BOOL == 0 );
    assert( TYPE_CHAR == 1 );
    assert( TYPE_FLOAT == TYPE_ULONG64 + 1 );

    if( size ) {
        type_list = ( sign ) ? s_types : u_types;
        for( i = 0; i < sizeof( s_types ) / sizeof( s_types[0] ); ++i ) {
            type_id = type_list[i];
            if( size == CTypeSizes[type_id] || !exact && ( size < CTypeSizes[type_id] ) ) {
                typ = GetType( type_id );
                assert( typ );
                break;
            }
        }
    }
    return( typ );
}

void TypesPurge( void )
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

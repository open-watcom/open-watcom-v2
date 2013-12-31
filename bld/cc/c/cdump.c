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

#define STRCHUNK_INCREMENT 512

typedef struct {
    char    *data;
    size_t  cursize;
    size_t  maxsize;
} STRCHUNK;

extern  char    *Tokens[];

static void DoDumpType( TYPEPTR realtype, char *symname, STRCHUNK *pch );

/* matches table of type in ctypes.h */
static  char    *CTypeNames[] = {
    #define pick1(enum,cgtype,x86asmtype,name,size) name,
    #include "cdatatyp.h"
    #undef  pick1
};

static  char   do_message_output; /* Optimize output for human */

static void ChunkInit( STRCHUNK *pch )
{
    pch->data = NULL;
    pch->cursize = 0;
    pch->maxsize = 0;
}

static char *ChunkToStr( STRCHUNK *pch )
{
    char    *ret;

    /* remove spaces from end of string */
    while( pch->cursize > 0 && pch->data[pch->cursize - 1] == ' ' ) {
        pch->data[--pch->cursize] = '\0';
    }
    ret = pch->data;
    if( ret == NULL ) {
        ret = CStrSave( "" );
    }
    return( ret );
}

static void ChunkSaveData( STRCHUNK *pch, const char *data, size_t len )
{
    int   bytesleft;
    int   requestbytes;
    char *newdata;

    bytesleft = pch->maxsize - (pch->cursize+len+1);
    if( bytesleft <= 0 ) {
        requestbytes = -bytesleft;
        if( requestbytes < STRCHUNK_INCREMENT ) {
            requestbytes = STRCHUNK_INCREMENT;
        }
        requestbytes += pch->maxsize;
        newdata = CMemAlloc( requestbytes );
        if( pch->data ) {
            memcpy( newdata, pch->data, pch->cursize );
            CMemFree( pch->data );
        }
        pch->data = newdata;
        pch->maxsize = requestbytes;
    }
    memcpy( pch->data+pch->cursize, data, len );
    pch->cursize += len;
    pch->data[pch->cursize] = '\0';
}

static void ChunkSaveStr( STRCHUNK *pch, const char *str )
{
    ChunkSaveData( pch, str, strlen( str ) );
}

static void ChunkSaveChar( STRCHUNK *pch, char ch )
{
    ChunkSaveData( pch, &ch, 1 );
}

static void ChunkSaveStrWord( STRCHUNK *pch, const char *str )
{
    ChunkSaveStr( pch, str );
    ChunkSaveChar( pch, ' ' );
}

#ifdef FDEBUG
void DumpToken( void )
{
    int     value;

    if( DebugFlag >= 3 ) {
        printf( "%2d: ", CurToken );
        if( CurToken == T_ID )
            printf( "%s\n", Buffer );
        else if( CurToken == T_STRING )
            printf( "\"%s\"\n", Buffer );
        else if( CurToken == T_CONSTANT ) {
            value = Constant;
            printf( "%d\n", value );
        } else
            printf( "'%s'\n", Tokens[ CurToken ] );
    }
}
#endif

#if 0
void DumpTypeCounts( void )
{
    int     i;

    for( i = TYPE_BOOL; i <= TYPE_VOID; ++i ) {
        printf( "%3d %s\n", CTypeCounts[i], CTypeNames[i] );
    }
    printf( "%d pointer nodes\n", CTypeCounts[TYPE_POINTER] );
}
#endif

static TYPEPTR TrueType( TYPEPTR typ )
{
    TYPEPTR     newtyp;

    if ( typ == NULL )
        return typ;

    if( do_message_output ) {
        /* For human: smart typedef expansion. Stop before unnamed struct */
        while( typ->decl_type == TYPE_TYPEDEF ) {
            newtyp = typ->object;
            if( newtyp->decl_type == TYPE_STRUCT ||
                newtyp->decl_type == TYPE_UNION  ||
                newtyp->decl_type == TYPE_ENUM ) {

                if( *newtyp->u.tag->name == '\0' )
                    break;
            }
            typ = newtyp;
        }
    } else {
        if( !CompFlags.dump_prototypes ) {
            /* -zg, not -v */
            SKIP_TYPEDEFS( typ );
        }
    }
    return( typ );
}

static TYPEPTR Object( TYPEPTR typ )
{
    return( TrueType( typ->object ) );
}


static void put_keyword( int keyword, STRCHUNK *pch )
{
    ChunkSaveStrWord( pch, Tokens[ keyword ] );
}


static void DumpFlags( type_modifiers flags, TYPEPTR typ, STRCHUNK *fp )
{
    SYM_NAMEPTR     p;
    SYM_ENTRY       sym;

    if( flags & FLAG_VOLATILE )
        put_keyword( T_VOLATILE, fp );
    if( flags & FLAG_CONST )
        put_keyword( T_CONST, fp );
    if( flags & FLAG_UNALIGNED )
        put_keyword( T___UNALIGNED, fp );
    if( flags & FLAG_RESTRICT )
        put_keyword( T_RESTRICT, fp );
    if( flags & FLAG_LOADDS )
        put_keyword( T___LOADDS, fp );
    if( flags & FLAG_EXPORT )
        put_keyword( T___EXPORT, fp );
    if( flags & FLAG_SAVEREGS )
        put_keyword( T___SAVEREGS, fp );
    if( ( flags & FLAG_INTERRUPT ) == FLAG_INTERRUPT ) {
        put_keyword( T___INTERRUPT, fp );
    } else if( flags & FLAG_NEAR ) {
        if( flags & FLAG_BASED ) {
            ChunkSaveStr( fp, "__based(" );
            if( typ->u.p.based_sym == 0 ) {
                ChunkSaveStr( fp, "void" );
            } else {
                SymGet( &sym, typ->u.p.based_sym );
                p = SymName( &sym, typ->u.p.based_sym );
                ChunkSaveStr( fp, p );
            }
            ChunkSaveStr( fp, ") " );
        } else {
            put_keyword( T___NEAR, fp );
        }
    } else if( flags & FLAG_FAR ) {
        put_keyword( T___FAR, fp );
    } else if( flags & FLAG_HUGE ) {
        put_keyword( T___HUGE, fp );
    } else if( flags & FLAG_FAR16 ) {
        put_keyword( T___FAR16, fp );
    }
    switch( flags & MASK_LANGUAGES ) {
    case LANG_WATCALL:
        put_keyword( T___WATCALL, fp );
        break;
    case LANG_CDECL:
        put_keyword( T___CDECL, fp );
        break;
    case LANG_PASCAL:
        put_keyword( T___PASCAL, fp );
        break;
    case LANG_FORTRAN:
        put_keyword( T___FORTRAN, fp );
        break;
    case LANG_SYSCALL:
        put_keyword( T__SYSCALL, fp );
        break;
    case LANG_STDCALL:
        put_keyword( T___STDCALL, fp );
        break;
    case LANG_OPTLINK:
        put_keyword( T__OPTLINK, fp );
        break;
    case LANG_FASTCALL:
        put_keyword( T___FASTCALL, fp );
        break;
    }
}


static void DumpArray( TYPEPTR typ, STRCHUNK *pch )
{
    unsigned long   size;
    char            tempbuf[20];

    while( typ->decl_type == TYPE_ARRAY ) {
        size = typ->u.array->dimension;
        if( size != 0 ) {
            sprintf( tempbuf, "[%lu]", size );
            ChunkSaveStr( pch, tempbuf );
        } else {
            ChunkSaveStr( pch, "[]" );
        }
        typ = Object( typ );
    }
}


static void DumpTagName( char *tag_name, STRCHUNK *pch )
{
    if( *tag_name == '\0' ) {
        if (do_message_output)
            tag_name = "{...}";
        else
            tag_name = "_no_name_";
    }
    ChunkSaveStrWord( pch, tag_name );
}


static TYPEPTR DefArgPromotion( TYPEPTR arg_typ )
{
    TYPEPTR     typ;

    /* perform default argument promotions */
    typ = arg_typ;
    while( typ->decl_type == TYPE_TYPEDEF ) typ = Object( typ );
    switch( typ->decl_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_ENUM:
        arg_typ = GetType( TYPE_INT );
        break;
    case TYPE_USHORT:
        arg_typ = GetType( TYPE_UINT );
        break;
    case TYPE_FLOAT:
        arg_typ = GetType( TYPE_DOUBLE );
        break;
    default:
        break;
    }
    return( arg_typ );
}

static void DumpBaseType( TYPEPTR typ, STRCHUNK *pch )
{
    SYM_ENTRY           sym;
    TYPEPTR             obj;

    for( ;; ) {
        if( typ->decl_type == TYPE_TYPEDEF || typ->decl_type == TYPE_ENUM )
            break;
        obj = Object( typ );
        if( obj == NULL )
            break;
        typ = obj;
    }
    SKIP_DUMMY_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_TYPEDEF ) {
        SymGet( &sym, typ->u.typedefn );
        ChunkSaveStrWord( pch, SymName( &sym, typ->u.typedefn ) );
    } else {
        if( typ->type_flags & TF2_TYPE_PLAIN_CHAR ) {   /* 25-nov-94 */
            ChunkSaveStrWord( pch, "char" );
        } else {
            ChunkSaveStrWord( pch, CTypeNames[ typ->decl_type ] );
        }
        if( typ->decl_type == TYPE_STRUCT ||
            typ->decl_type == TYPE_UNION  ||
            typ->decl_type == TYPE_ENUM  ) {

            /* if there is no tag name, then should print out the
               entire structure or union definition or enum list */

            DumpTagName( typ->u.tag->name, pch );
        }
    }
}

static void DumpParmList( TYPEPTR *parm, SYMPTR funcsym, STRCHUNK *pch )
{
    TYPEPTR            typ;
    int                parm_num;
    SYM_HANDLE         sym_handle;
    SYM_ENTRY          sym;
    char               *sym_name;
    char               temp_name[20];

    if( parm == NULL ) {
        ChunkSaveStr( pch, "void" );
    } else {
        parm_num = 1;
        if( funcsym != NULL ) {
            sym_handle = funcsym->u.func.parms;
        } else {
            sym_handle = 0;
        }
        for( ;; ) {
            typ = *parm;
            if( typ == NULL ) break;
            typ = TrueType( typ );
            if( funcsym != NULL ) {
                if( funcsym->flags & SYM_OLD_STYLE_FUNC ) {
                    typ = DefArgPromotion( typ );
                }
            }
            sym_name = NULL;
            if( sym_handle != 0 ) {
                SymGet( &sym, sym_handle );
                sym_handle = sym.handle;
                sym_name   = sym.name;
            } else {
                if( typ->decl_type != TYPE_VOID  &&
                    typ->decl_type != TYPE_DOT_DOT_DOT ) {
                        sym_name = temp_name;
                        sprintf( temp_name, "__p%d", parm_num );
                }
            }
            DoDumpType( typ, sym_name, pch );
            ++parm;
            if( *parm != NULL ) {
                ChunkSaveChar( pch, ',' );
            }
            ++parm_num;
        }
    }
}


static void DumpTail( TYPEPTR typ, SYMPTR funcsym, type_modifiers pointer_flags, STRCHUNK *pch )
{
    TYPEPTR     top_typ;
    TYPEPTR     obj;

    top_typ = typ;
    for( ;; ) {
        if( typ->decl_type == TYPE_FUNCTION ) {
            ChunkSaveChar( pch, '(' );
            if( typ == top_typ  ||  typ->u.fn.parms != NULL ) {
                DumpParmList( typ->u.fn.parms, funcsym, pch);
                funcsym = NULL;
            }
            ChunkSaveChar( pch, ')' );
        }
        typ = Object( typ );
        while( typ->decl_type == TYPE_POINTER ) {
            typ = Object( typ );
        }
        if( typ->decl_type == TYPE_ARRAY ) {
            ChunkSaveChar( pch, ')' );
            if( pointer_flags & FLAG_WAS_ARRAY ) {      /* 07-jun-94 */
                /* we don't know the dimension anymore. just put out [1] */
                ChunkSaveStr( pch, "[1]" );
                pointer_flags = 0;
            }
            DumpArray( typ, pch) ;
            for( ;; ) {
                obj = Object( typ );
                if( obj->decl_type != TYPE_ARRAY ) break;
                typ = obj;
            }
        } else {
            if( typ->decl_type != TYPE_FUNCTION ) break;
            ChunkSaveChar( pch, ')' );
        }
    }
}

static void DumpPointer( TYPEPTR typ, STRCHUNK *pch )
{
    if( typ->decl_type == TYPE_POINTER ) {
        DumpPointer( Object( typ ), pch );
        if( !( typ->u.p.decl_flags & FLAG_WAS_ARRAY) ) {
            DumpFlags( typ->u.p.decl_flags & ~MASK_QUALIFIERS, typ, pch );
            DumpFlags( typ->u.p.decl_flags & MASK_QUALIFIERS, typ, pch );
            ChunkSaveChar( pch, '*' );
        }
    }
}


static void DumpDecl( TYPEPTR typ, SYMPTR funcsym, STRCHUNK *pch )
{
    TYPEPTR         obj;
    type_modifiers  flags;

    switch( typ->decl_type ) {
    case TYPE_FUNCTION:
        DumpDecl( Object( typ ), NULL, pch );
        if ( funcsym ) {
            flags = funcsym->mods;
            if( flags & FLAG_LOADDS )
                put_keyword( T___LOADDS, pch );
            if( flags & FLAG_EXPORT )
                put_keyword( T___EXPORT, pch );
            if( flags & FLAG_SAVEREGS)
                put_keyword( T___SAVEREGS, pch );
            flags &= ~(FLAG_LOADDS | FLAG_EXPORT | FLAG_SAVEREGS);
            DumpFlags( flags, typ, pch );               /* 03-aug-88, FWC */
            ChunkSaveStr( pch, funcsym->name );
        }
    case TYPE_ARRAY:
        DumpTail( typ, funcsym, FLAG_NONE, pch );
        break;
    case TYPE_POINTER:
        obj = Object( typ );
        while( obj->decl_type == TYPE_POINTER )
            obj = Object( obj );
        switch( obj->decl_type ) {
        case TYPE_FUNCTION:
            DumpDecl( Object( obj ), NULL, pch );
            ChunkSaveChar( pch, '(' );
            break;
        case TYPE_ARRAY:
            while( obj->decl_type == TYPE_ARRAY )
                obj = Object( obj );
            DumpDecl( obj, NULL, pch );
            ChunkSaveChar( pch, '(' );
            break;
        default:
            break;
        }
        DumpPointer( typ, pch );         /* 26-may-89 */
        break;
    default:
        break;
    }
}

static void DoDumpType( TYPEPTR realtype, char *symname, STRCHUNK *pch )
{
    type_modifiers  pointer_flags;
    TYPEPTR         typ;

    realtype = TrueType( realtype );
    DumpBaseType( realtype, pch );
    DumpDecl( realtype, NULL, pch );
    if( symname )
        ChunkSaveStr( pch, symname );
    if( realtype->decl_type == TYPE_POINTER ) {
        if( realtype->u.p.decl_flags & FLAG_WAS_ARRAY ) {
            typ = Object( realtype );
            if( typ->decl_type != TYPE_ARRAY ) {
                ChunkSaveStr( pch, "[]" );
            }
        }
    }
    for( typ = realtype; typ != NULL; typ = Object( typ ) ) {
        if( typ->decl_type == TYPE_TYPEDEF ) break; /* 17-jan-91 */
        pointer_flags = 0;
        while( typ->decl_type == TYPE_POINTER ) {
            pointer_flags = typ->u.p.decl_flags;
            typ = Object( typ );
        }
        if( typ->decl_type == TYPE_ARRAY  ||
           typ->decl_type == TYPE_FUNCTION ) {
            DumpTail( realtype, NULL, pointer_flags, pch );
            break;
        }
    }
}


static void DumpParmTags( TYPEPTR *parm, FILE *fp )
{
    TYPEPTR     typ;
    STRCHUNK    chunk;
    char        *result;

    if( parm != NULL ) {
        for( ;; ) {
            typ = *parm;
            if( typ == NULL ) break;
            typ = TrueType( typ );
            if( typ->decl_type == TYPE_STRUCT  ||
                typ->decl_type == TYPE_UNION ) {
                ChunkInit(&chunk);
                ChunkSaveStrWord( &chunk, CTypeNames[ typ->decl_type ] );
                DumpTagName( typ->u.tag->name, &chunk );
                result = ChunkToStr( &chunk );
                fprintf( fp, "%s;\n", result );
                CMemFree( result );
            }
            ++parm;
        }
    }
}


void DumpFuncDefn( void )
{
    TYPEPTR     typ;
    FNAMEPTR    flist;
    STRCHUNK    chunk;
    char        *result;

    typ = CurFunc->sym_type;
    DumpParmTags( typ->u.fn.parms, DefFile );
    flist = FileIndexToFName( CurFunc->src_loc.fno );
    fprintf( DefFile, "//#line \"%s\" %u\n",            /* 07-jun-94 */
                flist->name,
                CurFunc->src_loc.line );

    ChunkInit( &chunk );

    if( CurFunc->attribs.stg_class == SC_STATIC ) {
        put_keyword( T_STATIC, &chunk );
    } else {
        put_keyword( T_EXTERN, &chunk );
    }
    if( CurFunc->flags & SYM_TYPE_GIVEN ) {
        DumpBaseType( Object( typ ), &chunk );
    }
    DumpDecl( typ, CurFunc, &chunk );

    result = ChunkToStr( &chunk );
    fprintf( DefFile, "%s;\n", result );
    CMemFree( result );
}

char *DiagGetTypeName( TYPEPTR typ )
{
    STRCHUNK    chunk;

    ChunkInit( &chunk );
    do_message_output = 1;
    DoDumpType( typ, NULL, &chunk );
    do_message_output = 0;
    return( ChunkToStr( &chunk ) );
}

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

extern  char    *Tokens[];

char    *CTypeNames[] = {
        "signed char",
        "unsigned char",
        "short",
        "unsigned short",
        "int",
        "unsigned int",
        "long",
        "unsigned long",
        "__int64",
        "unsigned __int64",
        "float",
        "double",
        "pointer",
        "array",
        "struct",
        "union",
        "function",
        "field",
        "void",
        "enum",
        "<typdef>",
        "<ufield>",
        "...",
        "<char>"
        "<wide char>"
 };


void SymDump()
{
#if 0
    if( DebugFlag >= 2 ) {
        DumpTags();
        DumpHashTable();
        DumpEnumTable();
    }
#endif
}


void DumpToken()
{
#if 0
    int value;

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
#endif
}

#if 0
void DumpTypeCounts()
{
/*
    int i;
    for( i = TYPE_CHAR; i <= TYPE_VOID; ++i ) {
        printf( "%3d %s\n", CTypeCounts[i], CTypeNames[i] );
    }
*/
    printf( "%d pointer nodes\n", CTypeCounts[TYPE_POINTER] );
}
#endif

static TYPEPTR TrueType( TYPEPTR typ )
{
    if( !CompFlags.dump_prototypes && typ != NULL ) {
        /* -zg, not -v */
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    }
    return( typ );
}

static TYPEPTR Object( TYPEPTR typ )
{
    return( TrueType( typ->object ) );
}

void DumpFuncDefn()
{
    TYPEPTR     typ;
    FNAMEPTR    flist;

    typ = CurFunc->sym_type;
    DumpParmTags( typ->u.parms );
    flist = FileIndexToFName( CurFunc->defn_file_index );
    fprintf( DefFile, "//#line \"%s\" %u\n",            /* 07-jun-94 */
                flist->name,
                CurFunc->d.defn_line );
    if( CurFunc->stg_class == SC_STATIC ) {
        put_keyword( T_STATIC, DefFile );
    } else {
        put_keyword( T_EXTERN, DefFile );
    }
    if( CurFunc->flags & SYM_TYPE_GIVEN ) {
        DumpBaseType( Object( typ ) );
    }
    DumpDecl( typ, CurFunc );
    fputs( ";\n", DefFile );
}


static void DumpPointer( TYPEPTR typ )                   /* 26-may-89 */
{
    if( typ->decl_type == TYPE_POINTER ) {
        DumpPointer( Object( typ ) );
        if( !( typ->u.p.decl_flags & FLAG_WAS_ARRAY) ) {    /* 07-jun-94 */
            DumpFlags( typ->u.p.decl_flags & ~(FLAG_CONST|FLAG_VOLATILE),
                                typ, DefFile );
            DumpFlags( typ->u.p.decl_flags & (FLAG_CONST|FLAG_VOLATILE),
                                typ, DefFile );
            fputc( '*', DefFile );
        }
    }
}


static void DumpTail( TYPEPTR typ, SYMPTR funcsym, int pointer_flags )
{
    TYPEPTR     top_typ;
    TYPEPTR     obj;

    top_typ = typ;
    for(;;) {
        if( typ->decl_type == TYPE_FUNCTION ) {
            fputc( '(', DefFile );
            if( typ == top_typ  ||  typ->u.parms != NULL ) {
                DumpParmList( typ->u.parms, funcsym );
                funcsym = NULL;
            }
            fputc( ')', DefFile );
        }
        typ = Object( typ );
        while( typ->decl_type == TYPE_POINTER ) {
            typ = Object( typ );
        }
        if( typ->decl_type == TYPE_ARRAY ) {
            fputc( ')', DefFile );
            if( pointer_flags & FLAG_WAS_ARRAY ) {      /* 07-jun-94 */
                /* we don't know the dimension anymore. just put out [1] */
                fputs( "[1]", DefFile );
                pointer_flags = 0;
            }
            DumpArray( typ );
            for( ;; ) {
                obj = Object( typ );
                if( obj->decl_type != TYPE_ARRAY ) break;
                typ = obj;
            }
        } else {
            if( typ->decl_type != TYPE_FUNCTION ) break;
            fputc( ')', DefFile );
        }
    }
}


static void DumpDecl( TYPEPTR typ, SYMPTR funcsym )
{
    TYPEPTR     obj;
    int         flags;

    switch( typ->decl_type ) {
    case TYPE_FUNCTION:
        DumpDecl( Object( typ ), NULL );
        flags = CurFunc->attrib;
        if( flags & FLAG_LOADDS )  put_keyword( T___LOADDS, DefFile );
        if( flags & FLAG_EXPORT )  put_keyword( T___EXPORT, DefFile );
        if( flags & FLAG_SAVEREGS) put_keyword( T___SAVEREGS, DefFile );
        flags &= ~(FLAG_LOADDS | FLAG_EXPORT | FLAG_SAVEREGS);
        DumpFlags( flags, typ, DefFile );               /* 03-aug-88, FWC */
        fputs( CurFunc->name, DefFile );
    case TYPE_ARRAY:
        DumpTail( typ, funcsym, 0 );
        break;
    case TYPE_POINTER:
        obj = Object( typ );
        while( obj->decl_type == TYPE_POINTER ) obj = Object( obj );
        switch( obj->decl_type ) {
        case TYPE_FUNCTION:
            DumpDecl( Object( obj ), NULL );
            fputc( '(', DefFile );
            break;
        case TYPE_ARRAY:
            while( obj->decl_type == TYPE_ARRAY ) obj = Object( obj );
            DumpDecl( obj, NULL );
            fputc( '(', DefFile );
            break;
        }
        DumpPointer( typ );         /* 26-may-89 */
        break;
    }
}


static void put_keyword( int keyword, FILE *fp )
{
    fputs( Tokens[ keyword ], fp );
    fputc( ' ', fp );
}


void DumpFlags( int flags, TYPEPTR typ, FILE *fp )
{
    SYM_NAMEPTR p;
    SYM_ENTRY   sym;

    if( flags & FLAG_VOLATILE ) put_keyword( T_VOLATILE, fp );
    if( flags & FLAG_CONST )    put_keyword( T_CONST, fp );
    if( flags & FLAG_NEAR ) {
        if( flags & FLAG_FAR  ) {
            put_keyword( T___INTERRUPT, fp );
        } else if( flags & FLAG_BASED ) {               /* 13-nov-91 */
            fputs( "__based(", fp );
            if( typ->u.p.based_sym == 0 ) {
                fputs( "void", fp );
            } else {
                SymGet( &sym, typ->u.p.based_sym );
                p = SymName( &sym, typ->u.p.based_sym );
                while( *p ) fputc( *p++, fp );
            }
            fputs( ") ", fp );
        } else {
            put_keyword( T___NEAR, fp );
        }
    }
    if( flags & FLAG_FAR  )     put_keyword( T___FAR, fp );
    if( flags & FLAG_HUGE )     put_keyword( T___HUGE, fp );
    switch( flags & FLAG_LANGUAGES ) {
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
    case LANG_STDCALL:                          /* 08-jan-92 */
        put_keyword( T___STDCALL, fp );
        break;
    case LANG_OPTLINK:                          /* 08-jan-92 */
        put_keyword( T__OPTLINK, fp );
        break;
    case LANG_FASTCALL:                         /* 08-jan-92 */
        put_keyword( T___FASTCALL, fp );
        break;
    }
}

static void DumpBaseType( TYPEPTR typ )
{
    auto SYM_ENTRY      sym;
    TYPEPTR             obj;

    for(;;) {
        if( typ->decl_type == TYPE_TYPEDEF ) break;
        obj = Object( typ );
        if( obj == NULL ) break;
        typ = obj;
    }
    for(;;) {
        if( typ->decl_type != TYPE_TYPEDEF ) break;
        if( !(typ->type_flags & TF2_DUMMY_TYPEDEF) ) break;
        typ = typ->object;                      // skip over dummy typedef
    }
    if( typ->decl_type == TYPE_TYPEDEF ) {
        SymGet( &sym, typ->u.typedefn );
        fprintf( DefFile, "%s ", SymName( &sym, typ->u.typedefn ) );
    } else {
        if( typ->type_flags & TF2_TYPE_PLAIN_CHAR ) {   /* 25-nov-94 */
            fprintf( DefFile, "char " );
        } else {
            fprintf( DefFile, "%s ", CTypeNames[ typ->decl_type ] );
        }
        if( typ->decl_type == TYPE_STRUCT ||
            typ->decl_type == TYPE_UNION  ||
            typ->decl_type == TYPE_ENUM  ) {

            /* if there is no tag name, then should print out the
               entire structure or union definition or enum list */

            DumpTagName( typ->u.tag->name );
        }
    }
}


static void DumpArray( TYPEPTR typ )
{
    unsigned long size;

    while( typ->decl_type == TYPE_ARRAY ) {
        size = typ->u.array->dimension;
        if( size != 0 ) {
            fprintf( DefFile, "[%lu]", size );
        } else {
            fputs( "[]", DefFile );
        }
        typ = Object( typ );
    }
}


static void DumpParmTags( TYPEPTR *parm )
{
    TYPEPTR     typ;

    if( parm != NULL ) {
        for(;;) {
            typ = *parm;
            if( typ == NULL ) break;
            typ = TrueType( typ );
            if( typ->decl_type == TYPE_STRUCT  ||
                typ->decl_type == TYPE_UNION ) {
                fprintf( DefFile, "%s ", CTypeNames[ typ->decl_type ]  );
                DumpTagName( typ->u.tag->name );
                fputs( ";\n", DefFile );
            }
            ++parm;
        }
    }
}


static void DumpTagName( char *tag_name )
{
    if( *tag_name == '\0' )  tag_name = "_no_name_";
    fprintf( DefFile, "%s ", tag_name );
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
    }
    return( arg_typ );
}


static void DumpParmList( TYPEPTR *parm, SYMPTR funcsym )
{
    TYPEPTR            typ;
    TYPEPTR            realtype;
    type_modifiers     pointer_flags;
    int                parm_num;
    SYM_HANDLE         sym_handle;
    SYM_ENTRY          sym;

    if( parm == NULL ) {
        fputs( "void", DefFile );
    } else {
        parm_num = 1;
        if( funcsym != NULL ) {
            sym_handle = funcsym->u.func.parms;
        } else {
            sym_handle = 0;
        }
        for( ;; ) {
            realtype = *parm;
            if( realtype == NULL ) break;
            realtype = TrueType( realtype );
            typ = realtype;
            if( funcsym != NULL ) {
                if( funcsym->flags & SYM_OLD_STYLE_FUNC ) {
                    typ = DefArgPromotion( realtype );
                }
            }
            DumpBaseType( typ );
            DumpDecl( realtype, NULL );
            if( sym_handle != 0 ) {
                SymGet( &sym, sym_handle );
                sym_handle = sym.handle;
                fprintf( DefFile, "%s", sym.name );
            } else {
                if( typ->decl_type != TYPE_VOID  &&
                    typ->decl_type != TYPE_DOT_DOT_DOT ) {
                    fprintf( DefFile, "__p%d", parm_num );
                }
            }
            if( realtype->decl_type == TYPE_POINTER ) {
                if( realtype->u.p.decl_flags & FLAG_WAS_ARRAY ) {
                    typ = Object( realtype );
                    if( typ->decl_type != TYPE_ARRAY ) {
                        fputs( "[]", DefFile );
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
                    DumpTail( realtype, NULL, pointer_flags );
                    break;
                }
            }
            ++parm;
            if( *parm != NULL )  fputc( ',', DefFile );
            ++parm_num;
        }
    }
}

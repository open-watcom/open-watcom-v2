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
* Description:  DWARF browsing information support.
*
****************************************************************************/


#include "cvars.h"
#include "browsio.h"
#include "standard.h"
#include "cg.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "cgprotos.h"

static dw_client       Client;
static dw_loc_handle   dummyLoc;
static source_loc      CurLoc;

extern  void    InitDebugTypes( void );         /* from pchdr.c */

typedef enum
{   DC_RETURN           = 0x01,         // this is a return type
    DC_DEFINE           = 0x02,         // generate definition
    DC_DEFAULT          = 0x00          // default behaviour
} DC_CONTROL;

static dw_handle dwarfType( TYPEPTR, DC_CONTROL );
static void dwarfEmitVariables( SYM_HANDLE sym_handle );

static void type_update( TYPEPTR typ, int mask, dw_handle dh )
/************************************************************/
{
    typ->type_flags = (typ->type_flags & ~TF2_DWARF) | mask;
    typ->u1.dwarf_type = dh;
}

static void dwarfFile( unsigned filenum )
/***************************************/
{
    static unsigned Current_File_Index = ~0U;
    FNAMEPTR    flist;
    char       *fname;

    // if current file changed, call DWDeclFile
    if( filenum != Current_File_Index ) {
        flist = FileIndexToFName(filenum);
        fname = FNameFullPath( flist );
        DWDeclFile( Client, fname );
        Current_File_Index = filenum;
    }
}

static void dwarfLocation( source_loc *src_loc )
/**********************************************/
{
    dwarfFile( src_loc->fno );
    DWDeclPos( Client, src_loc->line, 0 );
}

static void dwarfReference( dw_handle dh, source_loc *src_loc )
/*************************************************************/
{
    dwarfFile( src_loc->fno );
    DWReference( Client, src_loc->line, 0, dh );
}

static void dwarfStructInfo( TAGPTR tag )
/***************************************/
{
    dw_handle   dh;
    dw_handle   fld_dh;
    TYPEPTR     typ;
    FIELDPTR    field;
    XREFPTR     xref;

    for( field = tag->u.field_list; field != NULL; field = field->next_field ) {
        xref = field->xref;
        typ = field->field_type;
        if( typ->decl_type == TYPE_FIELD || typ->decl_type == TYPE_UFIELD ) {
            fld_dh = dwarfType( GetType( typ->u.f.field_type ), DC_DEFAULT );
            if( xref != NULL ) {    //stupid struct { int x; int y[] ) = init thing
                // Also watch for side effects with the DWDeclPos and a dwtype
                dwarfLocation( &xref->src_loc );
            }
            dh = DWAddBitField( Client,
                        fld_dh,
                        dummyLoc,
                        TypeSize( typ ),
                        typ->u.f.field_start,
                        typ->u.f.field_width,
                        field->name,
                        DW_FLAG_PUBLIC );
        } else {
            fld_dh =  dwarfType( typ, DC_DEFAULT );
            xref = field->xref;     // re-get in case the struct was freed during recursion
            if( xref != NULL ) {    //stupid struct { int x; int y[] ) = init thing
                dwarfLocation( &xref->src_loc );
            }
            dh = DWAddField( Client,
                        fld_dh,
                        dummyLoc,
                        field->name,
                        DW_FLAG_PUBLIC );
        }
        if( xref != NULL ) {
            for( ; (xref = xref->next_xref) != NULL; ) {
                dwarfReference( dh, &xref->src_loc );
            }
            FreeXrefs( field->xref );
            field->xref = NULL;
        }
    }
}

static dw_handle dwarfStructUnion( TYPEPTR typ, DC_CONTROL control )
/******************************************************************/
{
    dw_handle   dh;
    TAGPTR      tag;
    char        *name;
    char        defined;

    control = control;
    if( typ->type_flags & TF2_DWARF ) {
        dh = typ->u1.dwarf_type;
    } else {
        if( typ->decl_type == TYPE_STRUCT ) {
            dh = DWStruct( Client, DW_ST_STRUCT );
        } else {
            dh = DWStruct( Client, DW_ST_UNION );
        }
        type_update( typ, TF2_DWARF_FWD, dh );
    }
    tag = typ->u.tag;
    if( typ->type_flags & TF2_DWARF_FWD ) {
        type_update( typ, TF2_DWARF_DEF, dh );
        if( tag->name[0] == '\0' ) {
            name = NULL;
        } else {
            name = tag->name;
        }
        defined = (tag->size != 0);
        DWBeginStruct( Client,
                       dh,
                       tag->size,
                       name,
                       0,
                       (defined ? 0 : DW_FLAG_DECLARATION ) );
        if( defined ) {
            dwarfStructInfo( tag );
        }
        DWEndStruct( Client );
    }
    return( dh );
}

static ENUMPTR ReverseEnums( ENUMPTR esym )    /* reverse order of enums */
{
    ENUMPTR     prev_enum;
    ENUMPTR     next_enum;

    prev_enum = NULL;
    while( esym != NULL ) {
        next_enum = esym->thread;
        esym->thread = prev_enum;
        prev_enum = esym;
        esym = next_enum;
    }
    return( prev_enum );
}

static dw_handle dwarfEnum( TYPEPTR typ )
/***************************************/
{
    dw_handle   dh;
    ENUMPTR     esym;
    ENUMPTR     enum_list;

    dh = DWBeginEnumeration( Client,
                             TypeSize( typ->object ),
                             typ->u.tag->name,
                             0,
                             0 );
    enum_list = ReverseEnums( typ->u.tag->u.enum_list );
    for( esym = enum_list; esym != NULL; esym = esym->thread ) {
        DWAddConstant( Client, esym->value.u._32[L], esym->name );
    }
    ReverseEnums( enum_list );
    DWEndEnumeration( Client );
    type_update( typ, TF2_DWARF_DEF, dh );
    return( dh );
}

static dw_handle dwarfTypeArray( TYPEPTR typ )
/********************************************/
{
    dw_handle           dh;

    if( typ->type_flags & TF2_DWARF )
        return( typ->u1.dwarf_type );
    dh = DWSimpleArray( Client,
                dwarfType( typ->object, DC_DEFAULT ),
                typ->u.array->dimension );
    type_update( typ, TF2_DWARF_DEF, dh );
    return( dh );
}

static dw_handle dwarfTypeFunction( TYPEPTR typ, char *name )
/***********************************************************/
{
    dw_handle   dh;
    TYPEPTR     *parm_types;

    if( typ->type_flags & TF2_DWARF )
        return( typ->u1.dwarf_type );
    dh = dwarfType( typ->object, DC_RETURN );
    dh = DWBeginSubroutineType( Client,
                                dh,
                                name,
                                0,
                                DW_FLAG_DECLARATION | DW_FLAG_PROTOTYPED );
    type_update( typ, TF2_DWARF_DEF, dh );
    if( typ->u.fn.parms != NULL ) {
        for( parm_types = typ->u.fn.parms; (typ = *parm_types) != NULL; ++parm_types ) {
            if( typ->decl_type == TYPE_DOT_DOT_DOT ) {
                DWAddEllipsisToSubroutineType( Client );
            } else {
                DWAddParmToSubroutineType( Client,
                                       dwarfType( typ, DC_DEFAULT ),
                                       dummyLoc,
                                       dummyLoc,
                                       NULL );
            }
        }
    }
    DWEndSubroutineType( Client );
    return( dh );
}

uint dwarfTypeModifier( type_modifiers decl_flags )
/*************************************************/
{
    uint        modtype = 0;

    if( decl_flags & FLAG_CONST ) {
        modtype |= DW_MOD_CONSTANT;
    }
    if( decl_flags & FLAG_VOLATILE ) {
        modtype |= DW_MOD_VOLATILE;
    }
#if _CPU == 386
    if( decl_flags & FLAG_NEAR ) {
        modtype |= DW_MOD_NEAR32;
    }
    if( decl_flags & FLAG_FAR ) {
        modtype |= DW_MOD_FAR32;
    }
    if( decl_flags & FLAG_FAR16 ) {
        modtype |= DW_MOD_FAR16;
    }
#else
    if( decl_flags & FLAG_NEAR ) {
        modtype |= DW_MOD_NEAR16;
    }
    if( decl_flags & FLAG_FAR ) {
        modtype |= DW_MOD_FAR16;
    }
    if( decl_flags & FLAG_HUGE ) {
        modtype |= DW_MOD_HUGE16;
    }
#endif
    return( modtype  );
}

static dw_handle dwarfType( TYPEPTR typ, DC_CONTROL control )
/***********************************************************/
{
    dw_handle   dh = 0;
    SYMPTR      sym;

    control = control;
    if( typ->type_flags & TF2_DWARF )
        return( typ->u1.dwarf_type );

    switch( typ->decl_type ) {
    case TYPE_BOOL:
        dh = DWFundamental( Client, "_Bool", DW_FT_UNSIGNED_CHAR, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_CHAR:
        dh = DWFundamental( Client, "char", DW_FT_SIGNED_CHAR, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_UCHAR:
        dh = DWFundamental( Client, "unsigned char", DW_FT_UNSIGNED_CHAR, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_WCHAR:
        dh = DWFundamental( Client, "wchar_t", DW_FT_UNSIGNED_CHAR, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_SHORT:
        dh = DWFundamental( Client, "signed short", DW_FT_SIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_USHORT:
        dh = DWFundamental( Client, "unsigned short", DW_FT_UNSIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_INT:
        dh = DWFundamental( Client, "signed int", DW_FT_SIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_UINT:
        dh = DWFundamental( Client, "unsigned int", DW_FT_UNSIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_LONG:
        dh = DWFundamental( Client, "signed long", DW_FT_SIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_ULONG:
        dh = DWFundamental( Client, "unsigned long", DW_FT_UNSIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_LONG64:
        dh = DWFundamental( Client, "__int64", DW_FT_UNSIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_ULONG64:
        dh = DWFundamental( Client, "unsigned __int64", DW_FT_UNSIGNED, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_FLOAT:
        dh = DWFundamental( Client, "float", DW_FT_FLOAT, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_DOUBLE:
        dh = DWFundamental( Client, "double", DW_FT_FLOAT, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_LONG_DOUBLE:
        dh = DWFundamental( Client, "long double", DW_FT_FLOAT, TypeSize( typ ) );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_ENUM:
        dh = dwarfEnum( typ );
        break;
    case TYPE_POINTER:
        dh = DWPointer( Client, dwarfType( typ->object, DC_DEFAULT ), 0 );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_TYPEDEF:
        dh = dwarfType( typ->object, DC_DEFAULT );
        sym = SymGetPtr( typ->u.typedefn );
        dwarfLocation( &sym->src_loc );
        dh = DWTypedef( Client,
                        dh,
                        sym->name,
                        0,
                        0 );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    case TYPE_STRUCT:
    case TYPE_UNION:
        dh = dwarfStructUnion( typ, DC_DEFAULT );
        break;
    case TYPE_FUNCTION:
        dh = dwarfTypeFunction( typ, NULL );
        break;
    case TYPE_ARRAY:
        dh = dwarfTypeArray( typ );
        break;
    case TYPE_VOID:
        dh = DWFundamental( Client, "void", DW_FT_UNSIGNED, 0 );
        type_update( typ, TF2_DWARF_DEF, dh );
        break;
    default:
#ifdef FDEBUG
        DumpFullType( typ );
        CFatal( "dwarf: illegal type" );
#endif
        break;
    }
#ifdef FDEBUG
    if( dh == 0 && (control & DC_RETURN) == 0 ) {
        DumpFullType( type );
        CFatal( "dwarf: unable to define type" );
    }
#endif
    return( dh );
}

static void dwarfFunctionDefine( SYMPTR func_sym )
/************************************************/
{
    TYPEPTR     typ;
    dw_handle   return_dh;
    dw_handle   func_dh;
    dw_handle   dh;
    uint        call_type;
    uint        flags;
    SYMPTR      sym;
    SYM_HANDLE  sym_handle;

    call_type = 0;
    typ = func_sym->sym_type;
    if( func_sym->mods & FLAG_NEAR ) {
        call_type = DW_SB_NEAR_CALL;
    } else if( func_sym->mods &  FLAG_FAR ) {
        call_type = DW_SB_FAR_CALL;
    } else if( func_sym->mods & FLAG_FAR16 ) {
        call_type = DW_SB_FAR16_CALL;
    }
    flags = DW_FLAG_PROTOTYPED;
    if( func_sym->attribs.stg_class == SC_STATIC ) {
        flags |= DW_SUB_STATIC;
    }
    return_dh = dwarfType( typ->object, DC_RETURN );
    func_dh = func_sym->dwarf_handle;
    if( func_dh != 0 ) {    // was forward ref'd
        DWHandleSet( Client, func_dh );
    }
    dwarfLocation( &func_sym->src_loc );
    func_dh = DWBeginSubroutine( Client,
                   call_type,
                   return_dh,
                   dummyLoc,
                   dummyLoc,
                   dummyLoc,
                   0,
                   dummyLoc,
                   func_sym->name,
                   0,
                   flags );
    func_sym->dwarf_handle = func_dh;
    for( sym_handle = func_sym->u.func.parms; sym_handle != SYM_NULL; sym_handle = sym->handle ) {
        sym = SymGetPtr( sym_handle );
        dh = DWFormalParameter( Client,
                        dwarfType( sym->sym_type, DC_DEFAULT ),
                        NULL,
                        NULL,
                        sym->name,
                        DW_DEFAULT_NONE );
        sym->dwarf_handle = dh;
    }
    dwarfEmitVariables( func_sym->u.func.locals );
}

static dw_handle dwarfFunctionDecl( SYMPTR func_sym )
/***************************************************/
{
    TYPEPTR     typ;
    dw_handle   return_dh;
    dw_handle   func_dh;
    uint        call_type;
    uint        flags;

    call_type = 0;
    typ = func_sym->sym_type;
    if( func_sym->mods & FLAG_NEAR ) {
        call_type = DW_SB_NEAR_CALL;
    } else if( func_sym->mods &  FLAG_FAR ) {
        call_type = DW_SB_FAR_CALL;
    } else if( func_sym->mods & FLAG_FAR16 ) {
        call_type = DW_SB_FAR16_CALL;
    }
    flags = DW_FLAG_PROTOTYPED;
    flags |= DW_FLAG_DECLARATION;
    if( func_sym->attribs.stg_class == SC_STATIC ) {
        flags |= DW_SUB_STATIC;
    }
    return_dh = dwarfType( typ->object, DC_RETURN );
    dwarfLocation( &func_sym->src_loc );
    func_dh = DWBeginSubroutine( Client,
                   call_type,
                   return_dh,
                   dummyLoc,
                   dummyLoc,
                   dummyLoc,
                   0,
                   dummyLoc,
                   func_sym->name,
                   0,
                   flags );
   DWEndSubroutine( Client );
   return( func_dh );
}

static dw_handle dwarfVariable( SYMPTR sym )
/******************************************/
{
    dw_handle dh;
    uint      flags;

    flags = 0;
    if( sym->attribs.stg_class == SC_NONE ) {
        flags = DW_FLAG_GLOBAL;
    }
    dh = dwarfType( sym->sym_type, DC_DEFAULT );
    dwarfLocation( &sym->src_loc );
    dh = DWVariable( Client,
                dh,
                dummyLoc,
                0,
                dummyLoc,
                sym->name,
                0,
                flags );
    return( dh );
}


static void dwarfEmitVariables( SYM_HANDLE sym_handle )
/*****************************************************/
{
    SYMPTR      sym;
    TYPEPTR     typ;
    dw_handle   dh;

    for( ; sym_handle != SYM_NULL; sym_handle = sym->handle ) {
        sym = SymGetPtr( sym_handle );
        if( (sym->flags & SYM_TEMP) == 0 ) {
            typ = sym->sym_type;
            dh = 0;
            if( sym->flags & SYM_FUNCTION ) {
                if( (sym->flags & SYM_DEFINED) == 0 ) {
                    dh = dwarfFunctionDecl( sym );
                }
//              printf( "func: %s", sym->name );
            } else if( sym->attribs.stg_class == SC_TYPEDEF ) {
                dh = dwarfType( typ, DC_DEFAULT );
//              printf( "typedef: %s", sym->name );
            } else {
                dh = dwarfVariable( sym );
//              printf( "var:: %s", sym->name );
            }
            sym->dwarf_handle = dh;
//          printf( " defined on line %u\n", sym->d.defn_line );
        }
    }
}

static void dwarfDumpNode( TREEPTR node )
/***************************************/
{
    SYMPTR      sym;

    switch( node->op.opr ) {
    case OPR_FUNCTION:          // start of function
        sym = SymGetPtr( node->op.u2.func.sym_handle );
        dwarfFunctionDefine( sym );
        break;
    case OPR_FUNCEND:           // end of function
        DWEndSubroutine( Client );
        break;
    case OPR_NEWBLOCK:          // start of new block { vars; }
        DWBeginLexicalBlock( Client, NULL, NULL );
        dwarfEmitVariables( node->op.u2.sym_handle );
        break;
    case OPR_ENDBLOCK:          // end of new block { vars; }
        DWEndLexicalBlock( Client );
        break;
    case OPR_PUSHSYM:           // push sym_handle
    case OPR_PUSHADDR:          // push address of sym_handle
    case OPR_FUNCNAME:          // function name
        sym = SymGetPtr( node->op.u2.sym_handle );
        if( (sym->flags & SYM_TEMP) == 0 ) {
            dw_handle   dh;

            dh = sym->dwarf_handle;
            if( dh == 0 ) {     // forward ref
                dh = DWHandle( Client, DW_ST_NONE );
                sym->dwarf_handle = dh;
            }
            dwarfReference( dh, &CurLoc );
        }
        break;
    default:
        break;
    }
}


static void dwarfEmitFunctions( void )
/************************************/
{
    TREEPTR     tree;

    for( tree = FirstStmt; tree != NULL; tree = tree->left ) {
        CurLoc = tree->op.u2.src_loc;
        WalkExprTree( tree->right, dwarfDumpNode, NoOp, NoOp, dwarfDumpNode );
    }
}

void SetDwarfType( TYPEPTR typ )
{
    typ->type_flags &= ~TF2_DWARF;
    typ->u1.dwarf_type = 0;
}

void SetFuncDwarfType( TYPEPTR typ, int index )
{
    index = index;   /* unused */
    typ->type_flags &= ~TF2_DWARF;
    typ->u1.dwarf_type = 0;
}

static void EmitAType( TYPEPTR typ )
{
    dwarfType( typ, DC_DEFAULT );
}

static void dwarfInitTypes( void )
/********************************/
{
    WalkTypeList( SetDwarfType );
    WalkFuncTypeList( SetFuncDwarfType );
}

static void dwarfEmit( void )
/***************************/
{
    DATA_TYPE   i;

    dwarfInitTypes();
    for( i = TYPE_BOOL; i <= TYPE_DOUBLE; i++ ) {
        dwarfType( GetType( i ), DC_DEFAULT );
    }
    WalkTypeList( EmitAType );
    dwarfEmitVariables( GlobalSym );
    dwarfEmitFunctions();
    InitDebugTypes();
}

extern void DwarfBrowseEmit( void )
/*********************************/
{
    Client = DwarfInit();
    dummyLoc = DWLocFini( Client, DWLocInit( Client ) );
    CurLoc.fno = 0;
    CurLoc.line = 0;
    dwarfEmit();
    DWLocTrash( Client, dummyLoc );
    DwarfFini( Client );
}

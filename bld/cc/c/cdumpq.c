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
//#include "ctokens.h"

extern  void    DumpType();             /* cdump */
extern  void    QuadGet();              /* cnode */
extern  void    LeafGet();              /* cnode */
extern  TYPEPTR TypeOf();               /* cnode */
extern  void    SymGet( SYMPTR, SYM_HANDLE );
extern  void    DumpFlags( int, TYPEPTR, FILE * );

extern char     *CTypeNames[];          /* cdump */

void DmpQuads()
{
    unsigned i;

    for( i = 0; i < QuadIndex; i++ ) {
        DumpQuad( i );
    }
}

void DumpQuad( unsigned i )
    {
        auto QUAD q;

/*      printf( "quad: src#:   op1   operator   op2   result\n" ); */
        if( DebugFlag >= 1 ) {
            QuadGet( &q, i );
            printf( "%4d: ", i );
            printf( "%4d:", q.line_num );
            switch( q.opr ) {
            case T_FUNC_DEFN:
            case T_FUNC_END:
            case T_LEFT_BRACE:
            case T_RIGHT_BRACE:
            case T_CALL:
            case T_CALL_NO_PARM:
            case T_SWITCH:
            case T_MATHFUNC:
                printf( " %-8d ", q.op1 );
                printf( " %-9s ", Tokens[q.opr] );
                DumpLeaf( q.op2 );
                break;
            case T_CASE:
                DumpLeaf( q.op1 );
                printf( " %-9s ", Tokens[q.opr] );
                printf( " %-8d ", q.op2 ); /* quad # link to next case */
                break;
            default:
                DumpLeaf( q.op1 );
                printf( " %-9s ", Tokens[q.opr] );
                DumpLeaf( q.op2 );
                break;
            }
            if( q.opr == T_FUNC_DEFN ) {
                DumpParmList( q.op2 );
            }
            if( q.result != 0 ) {
                if( q.opr == T_PARM  ||  q.opr == T_PARM_LIST ) {
                    DumpLeaf( q.result );
                    DumpType( TypeOf( q.result ), 1 );
                } else if( q.opr == T_SWITCH  ||  q.opr == T_QUESTION ) {
                    DumpLeaf( q.result );
                } else {
                    DumpResultLeaf( q.result );
                }
            }
            putchar( '\n' );
            fflush( stdout ); /* get the sucker out */
        }
    }


static void DumpParmList( TREEPTR tree )
    {
        register SYM_HANDLE sym_handle;
        auto LEAF leaf;
        auto SYM_ENTRY sym;

        LeafGet( &leaf, tree );
        SymGet( &sym, leaf.u.s.sym_handle );
        sym_handle = sym.u.func.parms;
        putchar( '(' );
        while( sym_handle != 0 ) {
            SymGet( &sym, sym_handle );
            printf( "%s", sym.name );
            sym_handle = sym.handle;
            if( sym_handle == 0 ) break;
            putchar( ',' );
        }
        putchar( ')' );
    }


local void DumpLeaf( unsigned int index )
    {
        auto LEAF leaf;
        auto SYM_ENTRY sym;

        if( index != 0 ) {
            LeafGet( &leaf, index );
            switch( leaf.leaf_type ) {
            case LEAF_INT:
            case LEAF_LONG:
                printf( " %-8d ", leaf.u.long_konst );
                break;
            case LEAF_STRING:
                printf( " '%-6s' ", leaf.u.string_handle->literal );
                break;
            case LEAF_SYM:
                SymGet( &sym, leaf.u.s.sym_handle );
                printf( " %-8s ", sym.name );
                break;
            case LEAF_FUNC_RESULT:
            case LEAF_TEMP:
            case LEAF_SUBSCRIPTED:
            case LEAF_TEMP_DOT:
                printf( " T%03d", index );
                printf( "+%-3d ", leaf.u.s.offset );
                break;
            case LEAF_SYM_IND:
                SymGet( &sym, leaf.u.s.sym_handle );
                printf( " *%-7s ", sym.name );
                break;
            case LEAF_TEMP_IND:
                printf( " *T%03d", index );
                printf( "+%-2d ", leaf.u.s.offset );
                break;
            case LEAF_SYM_INDEXED:
                SymGet( &sym, leaf.u.s.sym_handle );
                printf( " %-4s", sym.name );
                printf( "->%-2d ", leaf.u.s.offset );
                break;
            case LEAF_SYM_DOT:
                SymGet( &sym, leaf.u.s.sym_handle );
                printf( " %-4s", sym.name );
                printf( ".%-2d ", leaf.u.s.offset );
                break;
            case LEAF_TEMP_INDEXED:
                printf( " T%03d", index );
                printf( "->%-2d ", leaf.u.s.offset );
                break;
            case LEAF_LABEL:
                printf( " L%-7d ", leaf.u.lbl.quad_label );
                break;
            }
        }
    }


local void DumpResultLeaf( unsigned int index )
    {
        register TYPEPTR typ;
        auto    TYPEDEFN ptrtyp;
        auto    LEAF result;

        LeafGet( &result, index );
        typ = result.data_type;
        switch( result.leaf_type ) {
        case LEAF_TEMP_IND:
        case LEAF_TEMP_INDEXED:
            ptrtyp.decl_type  = TYPE_POINTER;
            ptrtyp.decl_flags = result.leaf_flags;
            ptrtyp.object     = typ;
            typ = &ptrtyp;
        case LEAF_TEMP:
        case LEAF_FUNC_RESULT:
            printf( " T%03d ", index );
            DumpType( typ, 1 );
            break;
        case LEAF_SYM:
            DumpLeaf( index );
            break;
        default:
            putchar( '?' );
        }
    }

void DumpTags()
    {
        register TAGPTR tag;

        for( tag = TagHead; tag; tag = tag->next_tag ) {
            if( tag->sym_type->decl_type == TYPE_STRUCT ) {
                printf( "struct %s\n", tag->name );
                DumpStruct( tag->u.field_list, 0 );
            }
        }
    }


local void DumpStruct( FIELDPTR field, int indent )
    {
        for( ; field; field = field->next_field ) {
            DumpField( field, indent );
        }
    }


void DumpHashTable()
    {
        register SYM_HASHPTR hsym;
        register int i;
        auto SYM_ENTRY sym;

        puts( "SYMBOL TABLE DUMP" );
        for( i=0; i < SYM_HASH_SIZE; i++ ) {
            for( hsym = HashTab[i]; hsym; hsym = hsym->next_sym ) {
                SymGet( &sym, hsym->handle );
                if( sym.level == SymLevel ) {
                    DumpSymbol( &sym, 0 );
                }
            }
            printf( "---------%d----------\n", i );
        }
    }


void DumpSymbol( SYMPTR sym, int indent )
    {
        register int i;

        for( i = indent; i != 0; --i ) {
            printf( "    " );
        }
        printf( "%s sc=%d level=%d flags=%d is ", sym->name,
                sym->stg_class, sym->level, sym->flags );
        DumpFlags( sym->attrib, NULL, stdout );
        DumpType( sym->sym_type, indent );
        putchar( '\n' );
    }


local void DumpField( FIELDPTR field, int indent )
    {
        register int i;

        for( i = indent; i != 0; --i ) {
            printf( "    " );
        }
        printf( "%2d: %s is ", (int)field->offset, field->name );
        DumpType( field->field_type, indent );
        putchar( '\n' );
    }


void DumpType( TYPEPTR t, int indent )
    {
/*
        register TAGPTR tag;
*/
        register TYPEPTR *parm;
        auto SYM_ENTRY sym;

        while( t != NULL ) {
            DumpFlags( t->decl_flags, t, stdout );
            switch( t->decl_type ) {
            case TYPE_ENUM:
                printf( "enum: " );
                t = t->object;
            case TYPE_CHAR:
            case TYPE_UCHAR:
            case TYPE_SHORT:
            case TYPE_USHORT:
            case TYPE_INT:
            case TYPE_UINT:
            case TYPE_LONG:
            case TYPE_ULONG:
            case TYPE_FLOAT:
            case TYPE_DOUBLE:
            case TYPE_VOID:
                printf( "%s", CTypeNames[ t->decl_type ] );
                break;
            case TYPE_POINTER:  printf( "pointer to " ); break;
            case TYPE_ARRAY:
                printf( "array of [%lu] ", t->u.array->dimension );
                break;
            case TYPE_FIELD:
            case TYPE_UFIELD:
                printf( "field start=%d, width=%d", t->u.f.field_start,
                                                    t->u.f.field_width );
                break;
            case TYPE_STRUCT:
            case TYPE_UNION:
                if( t->decl_type == TYPE_STRUCT ) {
                    printf( "struct " );
                } else {
                    printf( "union " );
                }
/*
                if( SymLevel != -1 ) {
                    tag = t->u.tag;
                    if( tag->name[0] != '\0' ) {
                        printf( "%s", tag->name );
                    } else {
                        putchar( '\n' );
                        DumpStruct( tag->u.field_list, indent );
                    }
                }
*/
                break;
            case TYPE_FUNCTION:
                printf( "function(" );
                for( parm = t->u.parms; parm; ) {
                    if( *parm == NULL ) break;
                    DumpType( *parm, indent );
                    ++parm;
                    if( *parm != NULL ) putchar( ',' );
                }
                printf( ") returning " );
                break;
            case TYPE_DOT_DOT_DOT:
                printf( "..." );
                break;
            case TYPE_TYPEDEF:
                if( !(t->type_flags & TF2_DUMMY_TYPEDEF) ) { /* 25-nov-94 */
                    SymGet( &sym, t->u.typedefn );
                    printf( "typedef '%s' is ", sym.name );
                }
                break;
            }
            t = t->object;
        }
    }


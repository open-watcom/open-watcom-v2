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
#include "cgdata.h"
#include "memmgr.h"
#undef BY_C_FRONT_END
#include "cg.h"
#include "cgswitch.h"
#include "xcgops.h"
#include <stdio.h>

extern  int     __Strtold(char *,long_double *,char **);
extern  void    DBSetSymLoc( cg_sym_handle sym, long off );
extern  TREEPTR XCGAutoList;

int FELocalSym( SYMBOL sym )
{
    if( sym->id == SC_AUTO || sym->id == SC_REGISTER ) {
        return( 1 );
    }
    return( 0 );
}

int FEFuncReturnVar( SYMBOL sym )
{
    if( sym->name == NULL ) return( 0 );
    return( sym->name->name == CppSpecialName( SPECIAL_RETURN_VALUE ) );
}

signed_32 FEGetSymOffset( SYMBOL sym )
{
    return( sym->u.sym_offset );
}

void FESetSymOffset( SYMBOL sym, signed_32 offset )
{
    sym->u.sym_offset = offset;
}

// called at start of each function to process local variables
void FELocalDecls( TREEPTR autosym,
                 int (*DoAutoDecl)(void *,char *,unsigned),
                 void *auto_info )
{
    SYMBOL      sym;
    char        *name;
    int         size;

    for( ; autosym; autosym = autosym->left ) {
        sym = autosym->op.sym_handle;
        // We don't need to declare the .return variable if its address
        // is not taken
//      if( ! FEFuncReturnVar( sym ) ) {
            size = CgMemorySize( sym->sym_type );
            if( sym->name == NULL ) {
                name = "";
            } else {
                name = sym->name->name;
            }
            sym->u.sym_offset = (*DoAutoDecl)( auto_info, name, size );
            if( GenSwitches & DBG_LOCALS ) {
                DBSetSymLoc( sym, sym->u.sym_offset );
            }
//      }
    }
}

// called at start of each function to process local variables
void FEAutoDecls( SYMBOL func_handle,
                 int (*DoAutoDecl)(void *,char *,unsigned),
                 void *auto_info )
{
    func_handle;
    FELocalDecls( XCGAutoList, DoAutoDecl, auto_info );
}

// called at start of each scope to process local variables
void FEScopeDecls( TREEPTR newblock,
                 int (*DoAutoDecl)(void *,char *,unsigned),
                 void *auto_info )
{
    FELocalDecls( newblock->op.nested_syms, DoAutoDecl, auto_info );
}

// called at start of each function
void FEDumpParmSyms( TREEPTR parm )
{
#if 0
    SYM_HANDLE  parm_handle;

    while( parm != NULL ) {
        parm_handle = parm->sym.sym_handle;
        DBLocalSym( parm_handle, CGenType( parm->expr_type ) );
        parm = parm->right;
    }
#endif
}
void DumpStmt( TREEPTR tree )
{
    printf( "line %3.3u: ", tree->srclinenum );
//  WalkExprTree( tree->right, DumpOpnd, DumpPrefix, DumpInfix, DumpPostfix );
    printf( "\n" );
}
void DumpProgram()
{
}

void MakeBinaryFloat( TREEPTR opnd )
{
    FLOATVAL    *flt;
    char        *endptr;
    long_double ld;

    flt = opnd->op.float_value;
    if( flt->len != 0 ) {
        __Strtold( flt->string, &ld, &endptr );
        flt->ld = ld;
        flt->len = 0;
        flt->string[0] = '\0';
    }
}


long LongValue( TREEPTR leaf )
{
    long        value;
    FLOATVAL    *flt;
    char        *endptr;
    long_double ld;

    switch( leaf->op.const_type ) {
    case TYPE_CHAR:
        value = (signed char)leaf->op.ulong_value;
        break;
    case TYPE_UCHAR:
        value = (unsigned char)leaf->op.ulong_value;
        break;
    case TYPE_SHORT:
        value = (target_short)leaf->op.ulong_value;
        break;
    case TYPE_USHORT:
        value = (target_ushort)leaf->op.ulong_value;
        break;
    case TYPE_INT:
        value = (target_int)leaf->op.ulong_value;
        break;
    case TYPE_UINT:
        value = (target_uint)leaf->op.ulong_value;
        break;
    case TYPE_LONG:
        value = (target_long)leaf->op.ulong_value;
        break;
    case TYPE_ULONG:
        value = (target_ulong)leaf->op.ulong_value;
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        flt = leaf->op.float_value;
        if( flt->len == 0 ) {
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
#ifdef _LONG_DOUBLE_
        value = __LDI4( (long_double near *)&ld );
#else
        value = ld.value;
#endif
        break;
    default:
        value = 0;
        break;
    }
    return( value );
}

void CastFloatValue( TREEPTR leaf, int newtype )
{
    FLOATVAL    *flt;
    char        *endptr;
    double      doubleval;
    float       floatval;
    long_double ld;

    if( leaf->op.opr == OPR_PUSHFLOAT ) {
        flt = leaf->op.float_value;
        if( flt->len == 0 ) {           // if contains binary value
            ld = flt->ld;
        } else {
            __Strtold( flt->string, &ld, &endptr );
        }
    } else {    // integer
        switch( leaf->op.const_type ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
        case TYPE_LONG:
#ifdef _LONG_DOUBLE_
            __I4LD( leaf->op.long_value, (long_double near *)&ld );
#else
            ld.value = (double)leaf->op.long_value;
#endif
            break;
        default:
#ifdef _LONG_DOUBLE_
            __U4LD( leaf->op.long_value, (long_double near *)&ld );
#else
            ld.value = (double)leaf->op.ulong_value;
#endif
            break;
        }
        flt = (FLOATVAL *)CMemAlloc( sizeof(FLOATVAL) );
        leaf->op.float_value = flt;
        leaf->op.opr = OPR_PUSHFLOAT;
    }
    leaf->op.const_type = newtype;
    switch( newtype ) {
    case TYPE_FLOAT:
#ifdef _LONG_DOUBLE_
        __LDFS( (long_double near *)&ld, (float near *)&floatval );
        __FSLD( (float near *)&floatval, (long_double near *)&ld );
#else
        ld.value = (float)ld.value;
#endif
        break;
    case TYPE_DOUBLE:
#ifdef _LONG_DOUBLE_
        __LDFD( (long_double near *)&ld, (double near *)&doubleval );
        __FDLD( (double near *)&doubleval, (long_double near *)&ld );
#endif
        break;
    }
    flt->len = 0;
    flt->type = newtype;
    flt->ld = ld;
    flt->string[0] = '\0';
}

void CastConstValue( TREEPTR leaf, int newtyp )
{
    switch( newtyp ) {
    case TYPE_CHAR:
        leaf->op.ulong_value = (signed char)LongValue( leaf );
        break;
    case TYPE_UCHAR:
        leaf->op.ulong_value = (unsigned char)LongValue( leaf );
        break;
    case TYPE_SHORT:
        leaf->op.ulong_value = (target_short)LongValue( leaf );
        break;
    case TYPE_USHORT:
        leaf->op.ulong_value = (target_ushort)LongValue( leaf );
        break;
    case TYPE_INT:
        leaf->op.ulong_value = (target_int)LongValue( leaf );
        break;
    case TYPE_UINT:
        leaf->op.ulong_value = (target_uint)LongValue( leaf );
        break;
    case TYPE_LONG:
        leaf->op.ulong_value = (target_long)LongValue( leaf );
        break;
    case TYPE_ULONG:
        leaf->op.ulong_value = (target_ulong)LongValue( leaf );
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        CastFloatValue( leaf, newtyp );
        return;
    default:
        return;
    }
    leaf->op.opr = OPR_PUSHINT;
    leaf->op.const_type = newtyp;
}

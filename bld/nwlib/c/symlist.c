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
* Description:  Walk symbols accessed through ORL.
*
****************************************************************************/


#include "wlib.h"

static orl_return CheckSymbol( orl_symbol_handle orl_sym_hnd )
/************************************************************/
{
    orl_symbol_binding  binding;
    char                *name;
    orl_symbol_type     type;
    unsigned char       info;

    name = ORLSymbolGetName( orl_sym_hnd );
    binding = ORLSymbolGetBinding( orl_sym_hnd );
    if( binding == ORL_SYM_BINDING_GLOBAL || binding == ORL_SYM_BINDING_WEAK ) {
        type = ORLSymbolGetType( orl_sym_hnd );
        info = ORLSymbolGetRawInfo( orl_sym_hnd );
        if( !( type & ORL_SYM_TYPE_UNDEFINED ) ) {
            if( type & ORL_SYM_CDAT_MASK || binding == ORL_SYM_BINDING_WEAK ) {
                AddSym( name, SYM_WEAK, info );
            } else {
                AddSym( name,  SYM_STRONG, info );
            }
        } else {
            unsigned_64 val64;

            val64 = ORLSymbolGetValue( orl_sym_hnd );
            if( val64.u._32[I64LO32] != 0 || val64.u._32[I64HI32] != 0 ) {
                AddSym( name,  SYM_WEAK, info );
            }
        }
    } else if( binding == ORL_SYM_BINDING_ALIAS ) {
        AddSym( name, SYM_WEAK, 0 );
    }
    return( ORL_OKAY );
}

bool ObjWalkSymList( obj_file *ofile, sym_file *sfile )
/*****************************************************/
{
    orl_sec_handle      sym_sec_hnd;

    if( ofile->orl != NULL ) {
        sym_sec_hnd = ORLFileGetSymbolTable( ofile->orl );
        if( sym_sec_hnd == NULL )
            return( false );
        if( ORLSymbolSecScan( sym_sec_hnd, &CheckSymbol ) != ORL_OKAY ) {
            return( false );
        }
    } else {
        OMFWalkSymList( ofile, sfile );
    }
    return( true );
}

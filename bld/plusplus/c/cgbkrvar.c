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
#include "name.h"
#include "cgfront.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "cgsegid.h"


static SYMBOL makeCgVar(        // MAKE CODEGEN VARIABLE OF A CERTAIN TYPE
    TYPE type,                  // - type
    unsigned id,                // - SC_...
    NAME name,                  // - name to be used
    symbol_flag flags )         // - flags to be used
{
    SYMBOL var;                 // - new variable

    var = AllocSymbol();
    var->sym_type = type;
    var->id = id;
    var->flag = flags;
    if( name == NULL ) {
        name = NameDummy();
    }
    if( id == SC_AUTO ) {
        CGAutoDecl( (cg_sym_handle)var, CgTypeSym( var ) );
    } else {
        var = ScopeInsert( GetFileScope(), var, name );
        LinkageSet( var, "C++" );
        CgSegId( var );
    }
    return var;
}


SYMBOL CgVarRw(                 // MAKE R/W CODEGEN VARIABLE
    target_size_t size,         // - size
    unsigned id )               // - SC_...
{
    TYPE type;
    symbol_flag flags;

    type = MakeInternalType( size );
    if( id == SC_AUTO ) {
        flags = SF_CG_ADDR_TAKEN | SF_ADDR_TAKEN | SF_REFERENCED;
    } else {
        type = MakeCompilerReadWriteData( type );
        flags = SF_ADDR_TAKEN | SF_REFERENCED;
    }
    return makeCgVar( type, id, NULL, flags );
}


SYMBOL CgVarTemp(               // MAKE R/W AUTO TEMPORARY
    target_size_t size )        // - size of temp
{
    TYPE type;

    type = MakeInternalType( size );
    return makeCgVar( type, SC_AUTO, NULL, SF_REFERENCED );
}


SYMBOL CgVarTempTyped(          // MAKE R/W AUTO TEMPORARY, FOR cg_type
    cg_type cgtype )            // - type of entry
{
    TYPE type;

    type = TypeFromCgType( cgtype );
    return makeCgVar( type, SC_AUTO, NULL, SF_REFERENCED );
}


SYMBOL CgVarRo(                 // MAKE R/O CODEGEN VARIABLE
    target_size_t size,         // - size of variable
    unsigned id,                // - SC_...
    NAME name )                 // - name or NULL
{
    TYPE type;                  // - variable type

    type = MakeInternalType( size );
    if( id == SC_STATIC ) {
        type = MakeCompilerConstData( type );
    } else {
        type = MakeCompilerConstCommonData( type );
    }
    return makeCgVar( type, id, name, SF_ADDR_TAKEN | SF_REFERENCED );
}

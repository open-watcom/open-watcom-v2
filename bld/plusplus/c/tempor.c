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
#include "cgfront.h"
#include "name.h"
#include "label.h"
#include "cgsegid.h"

#ifndef NDEBUG
    #include "errdefns.h"
#endif


static TEMP_TYPE tempClass      // default TEMP_TYPE for temp.s
    = TEMP_TYPE_EXPR;


TEMP_TYPE TemporaryClass(       // SET DEFAULT CLASS FOR TEMPORARIES
    TEMP_TYPE new_type )        // - new type
{
    TEMP_TYPE retn;             // - old type

    retn = tempClass;
    tempClass = new_type;
    return retn;
}


static SYMBOL makeTemporary(    // ALLOCATE A TEMPORARY
    TYPE type,                  // - type of temporary
    boolean storage )           // - allocate storage if needed
{
    SYMBOL sym;                 // - symbol for temporary
    NAME name;                  // - name for temporary
    symbol_class id;            // - class for symbol

#if 0
    if( ( type->id == TYP_POINTER ) && ( type->flag & TF1_REFERENCE ) ) {
        type = type->of;
    }
#endif
    sym = SymMakeDummy( type, &name );
    switch( tempClass ) {
      case TEMP_TYPE_EXPR :
      case TEMP_TYPE_BLOCK :
        id = SC_AUTO;
        break;
      case TEMP_TYPE_STATIC :
        id = SC_STATIC;
        break;
      DbgDefault( "makeTemporary -- bad tempClass" );
    }
    sym->id = id;
    if( id == SC_STATIC ) {
        ScopeInsert( GetFileScope(), sym, name );
        if( storage ) {
            CgSegIdData( sym, SI_ALL_ZERO );
            DgSymbol( sym );
        }
    } else {
        ScopeInsert( ScopeForTemps(), sym, name );
    }
    return sym;
}


SYMBOL TemporaryAllocNoStorage( // ALLOCATE TEMPORARY WITHOUT STORAGE
    TYPE type )                 // - type of temporary
{
    return makeTemporary( type, FALSE );
}


SYMBOL TemporaryAlloc(          // ALLOCATE TEMPORARY IN FUNCTION
    TYPE type )                 // - type of temporary
{
    return makeTemporary( type, TRUE );
}


static SCOPE tempScope;         // NULL or scope for temporaries


SCOPE ScopeForTemps(            // FIND SCOPE FOR TEMPORARIES
    void )
{
    return tempScope == NULL ? GetCurrScope() : tempScope;
}


void ScopeGenAccessSet(         // SET ACCESS SCOPE FOR GENERATION
    TYPE cltype )               // - type for inlining
{
    DbgVerify( tempScope == NULL, "ScopeAccessSet -- tempScope != NULL" );
    tempScope = GetCurrScope();
    SetCurrScope (TypeScope( cltype ));
}


void ScopeGenAccessReset(       // RESET ACCESS SCOPE FOR GENERATION
    void )
{
    SetCurrScope (tempScope);
    tempScope = NULL;
}

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


#include <drsrchdf.h>
#include <string.h>

#include "assure.h"
#include "classtyp.h"
#include "enumtype.h"
#include "funcsym.h"
#include "typesym.h"
#include "varsym.h"
#include "macrosym.h"
#include "hotspots.h"
#include "mem.h"
#include "util.h"
#include "module.h"
#include "referenc.h"
#include "wbrwin.h"
#include "viewmgr.h"
#include "browse.h"

Symbol::Symbol( drmem_hdl drhdl, drmem_hdl drhdl_prt, Module * m, char * n )
        :_drhandle(drhdl)
        ,_parent(drhdl_prt)
        ,_module(m)
        ,_description(NULL)
        ,_name(n)       // n should be dynamic memory which symbol can free
        ,_decname(NULL)
        ,_anonymous(false)
//--------------------------------------------------------------
{
    _defined = DRIsSymDefined( drhdl );

    if( _name == NULL || _name[ 0 ] == '\0' ) {
        WBRFree( _name );
        _name = NULL;
        _anonymous = true;
    }
}

Symbol::Symbol( const Symbol &i )
        :_drhandle(i._drhandle)
        ,_parent(i._parent)
        ,_module(i._module)
        ,_description(NULL)
        ,_defined(i._defined)
        ,_anonymous(i._anonymous)
//-------------------------------
{
    if( i._name ) {
        _name = WBRStrDup( i._name );
    }
    _decname = (_decname) ? WBRStrDup( i._decname ) : NULL;
}

Symbol::~Symbol()
//---------------
{
    WBRFree( _name );
    WBRFree( _decname );
}

bool Symbol::isEqual( WObject const * o ) const
//---------------------------------------------
{
    return ((Symbol *)o)->_drhandle == _drhandle;
}

const char * Symbol::name()
//-------------------------
{
    if( _name == NULL || _name[ 0 ] == '\0' ) {
        ((Symbol *)this)->getAnonName();
    }
    return _name;
}

const char * Symbol::scopedName( bool fullScoping )
//-------------------------------------------------
// fullScoping specifies whether the name should be
// fully scoped (ie. with base::member)
{
    int i;

    if( _decname == NULL ) {
        WVList  desc;
        WString scoped;

        _description = &desc;
        DRDecoratedNameList( this, _drhandle, fullScoping ? _parent : DRMEM_HDL_NULL, descCallBack );
        _description = NULL;

        for( i = 0; i < desc.count(); i += 1 ) {
            Description * d = (Description *) desc[ i ];

            scoped.concat( d->name() );
            delete d->symbol();                 // this might be NULL, that's ok
        }

        desc.deleteContents();

        _decname = WBRStrDup( scoped.gets() );
    }
    return _decname;
}

void Symbol::loadUsers( WVList & val )
//------------------------------------
{
    _module->findRefSyms( &val, this );
}

void Symbol::loadReferences( WVList & refs )
//------------------------------------------
{
    _module->findReferences( &refs, this );
}

bool Symbol::defSourceFile( char *buff )
//--------------------------------------
// NYI: hang onto the source file name for future reference?
{
    char        *p;

    p = DRGetFileName( _drhandle );
    if( p == NULL ) return( false );
    strcpy( buff, p );
    return browseTop->makeFileName( buff );
}

static Symbol * Symbol::defineSymbol( dr_sym_type type, drmem_hdl drhdl,
                                      drmem_hdl drhdl_prt, Module * module,
                                      char * name )
//-----------------------------------------------------------------------
{
    Symbol *    newsym;

    switch( type ) {
    case DR_SYM_ENUM:
        newsym = new EnumType( drhdl, drhdl_prt, module, name );
        break;
    case DR_SYM_FUNCTION:
        newsym = new FunctionSym( drhdl, drhdl_prt, module, name );
        break;
    case DR_SYM_CLASS:
        newsym = new ClassType( drhdl, drhdl_prt, module, name );
        break;
    case DR_SYM_TYPEDEF:
        newsym = new TypeSym( drhdl, drhdl_prt, module, name );
        break;
    case DR_SYM_VARIABLE:
        newsym = new VariableSym( drhdl, drhdl_prt, module, name );
        break;
    case DR_SYM_MACRO:
        newsym = new MacroSym( drhdl, drhdl_prt, module, name );
        break;
    default:
        newsym = NULL;
    }
    return newsym;
}

int Symbol::getHotSpot( bool pressed ) const
//------------------------------------------
{
    bool opened;

    opened = ( WBRWinBase::viewManager()->findDetailView( this,
                DetailView::ViewDetail ) != NULL );

    return getHotSpot( symtype(), opened, pressed );
}

static  int Symbol::getHotSpot( dr_sym_type type, bool opened, bool pressed )
//---------------------------------------------------------------------------
{
    switch( type ) {
    case DR_SYM_FUNCTION:
        if( opened ) {
            return pressed ? FunctionOpenDown : FunctionOpenUp;
        } else {
            return pressed ? FunctionClosedDown : FunctionClosedUp;
        }
    case DR_SYM_CLASS:
        if( opened ) {
            return pressed ? ClassOpenDown : ClassOpenUp;
        } else {
            return pressed ? ClassClosedDown : ClassClosedUp;
        }
    case DR_SYM_ENUM:
        if( opened ) {
            return pressed ? EnumOpenDown : EnumOpenUp;
        } else {
            return pressed ? EnumClosedDown : EnumClosedUp;
        }
    case DR_SYM_TYPEDEF:
        if( opened ) {
            return pressed ? TypedefOpenDown : TypedefOpenUp;
        } else {
            return pressed ? TypedefClosedDown : TypedefClosedUp;
        }
    case DR_SYM_VARIABLE:
        if( opened ) {
            return pressed ? VariableOpenDown : VariableOpenUp;
        } else {
            return pressed ? VariableClosedDown : VariableClosedUp;
        }
    case DR_SYM_MACRO:
        if( opened ) {
            return pressed ? MacroOpenDown : MacroOpenUp;
        } else {
            return pressed ? MacroClosedDown : MacroClosedUp;
        }
    case DR_SYM_LABEL:
        if( opened ) {
            return pressed ? LabelOpenDown : LabelOpenUp;
        } else {
            return pressed ? LabelClosedDown : LabelClosedUp;
        }
    case DR_SYM_NOT_SYM:
    default:
        return InvalidHotSpot;
    }
}

static Symbol * Symbol::defineSymbol( const Symbol * info )
//---------------------------------------------------------
{
    Symbol *    newsym;
    dr_sym_type symtype;
    char *      newname;

    if( info->_name ) {
        newname = WBRStrDup( info->_name );
    } else {
        newname = NULL;
    }

    symtype = info->symtype();
    switch( symtype ) {
    case DR_SYM_CLASS:
        newsym = new ClassType( info->_drhandle, info->_parent, info->_module, newname );
        break;
    case DR_SYM_FUNCTION:
        newsym = new FunctionSym( info->_drhandle, info->_parent, info->_module, newname );
        break;
    case DR_SYM_TYPEDEF:
        newsym = new TypeSym( info->_drhandle, info->_parent, info->_module, newname );
        break;
    case DR_SYM_VARIABLE:
        newsym = new VariableSym( info->_drhandle, info->_parent, info->_module, newname );
        break;
    case DR_SYM_MACRO:
        newsym = new MacroSym( info->_drhandle, info->_parent, info->_module, newname );
        break;
    case DR_SYM_ENUM:
        newsym = new EnumType( info->_drhandle, info->_parent, info->_module, newname );
        break;
    default:
        newsym = NULL;
        break;
    }

    return newsym;
}


void Symbol::filePosition( ulong & line, uint & col )
//---------------------------------------------------
{
    line = DRGetLine( _drhandle );
    col = (uint) DRGetColumn( _drhandle );
    if( line == -1 ) line = 0;
    if( col == -1 ) col = 0;
}


WVList & Symbol::description( WVList & parts )
//--------------------------------------------
{
    _description = &parts;
    DRDecoratedNameList( this, _drhandle, _parent, descCallBack );
    _description = NULL;
    return parts;
}

static void Symbol::descCallBack ( void * obj, char * name,
                                   int u_def, drmem_hdl drhdl,
                                   dr_sym_type sym_type )
//------------------------------------------------------------
{
    Symbol *sptr = (Symbol *) obj;

    sptr->addDesc( name, u_def, drhdl, sym_type );
}

void Symbol::addDesc( char * name, int u_def,
                      drmem_hdl drhdl, dr_sym_type st )
//-----------------------------------------------------
// mySym is the symbol being described
{
    Description * desc;
    Symbol *      sym;

    if( drhdl == _drhandle ) {    /* our symbol */
        desc = new Description( Symbol::defineSymbol( this ) );
        delete [] name;
    } else {
        if( u_def ) {
            sym = Symbol::defineSymbol(st, drhdl, DRMEM_HDL_NULL, _module, name );
            desc = new Description( sym );
        } else {
            desc = new Description( name );
        }
    }

    _description->add( desc );
}

void Symbol::getAnonName()
//------------------------
// name an anonymous symbol
{
    ASSERTION( _name == NULL || _name[ 0 ] == '\0' );

    WVList  refs;
    WString name;
    int     i;

    WBRFree( _name );
    _name = NULL;

    loadReferences( refs );

    if( refs.count() <= 0 ) {
        _name = WBRStrDup( "[anonymous]" );
        return;
    }

    name.printf( "[" );
    name.concat( ((Reference *) refs[ 0 ])->name() );

    for( i = 1; i < refs.count(); i += 1 ) {
        name.concat( ',' );
        name.concat( ((Reference *) refs[ i ])->name() );
    }

    name.concat( ']' );

    _name = WBRStrDup( name.gets() );

    refs.deleteContents();
}

bool Symbol::isArtificial() const
//-------------------------------
{
    return DRIsArtificial( _drhandle );
}

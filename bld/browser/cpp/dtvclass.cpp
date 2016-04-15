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


#include <wcvector.h>

#include <wstring.hpp>
#include <wlistbox.hpp>
#include <wvlist.hpp>

#include "assure.h"
#include "dtvclass.h"
#include "classtyp.h"
#include "mem.h"
#include "util.h"
#include "menuids.h"
#include "viewmgr.h"

static char * AccessStr[] = {
    "",
    "  public:",
    "  protected:",
    "  private:"
};


ClassMember::ClassMember( Symbol * sym, const char * text )
            : _symbol( sym )
            , _text( text )
//---------------------------------------------------------
{
    _parent = sym->getParent();
    _access = DRGetAccess( _symbol->getHandle() );
}

ClassMember::ClassMember( Symbol * sym, dr_handle drhdl_prt, const char * text )
            : _symbol( sym )
            , _text( text )
            , _parent( drhdl_prt )
//------------------------------------------------------------------------
{
    _access = DRGetAccess( _symbol->getHandle() );
}

ClassMember::ClassMember( dr_handle drhdl_prt, dr_access access, const char * text )
            : _symbol( NULL )
            , _text( text )
            , _access( access )
            , _parent( drhdl_prt )
//---------------------------------------------------------------
{
}

bool ClassMember::operator== ( const ClassMember & other ) const
//--------------------------------------------------------------
{
    return( this == &other );
}

bool ClassMember::operator< ( const ClassMember & o ) const
//---------------------------------------------------------
{
    if( _parent < o._parent ) {
        return true;
    } else {
        if( _parent == o._parent ) {

            if( _access < o._access ) {
                return true;
            } else {
                if( _access == o._access ) {

                    if( (_symbol == NULL) && (o._symbol != NULL) ) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

//----------------------------- DTViewClass ----------------------



DTViewClass::DTViewClass( const Symbol * sym )
                : DTViewSymbol( sym, true )
                , _filter( optManager()->getMemberFilter() )
//--------------------------------------------------------------
{
    _members = new WCPtrSortedVector<ClassMember>;

    _symbolBox->onDblClick( this, (cbw) &DTViewClass::elementClicked );

    viewManager()->registerForEvents( this );

    load();
}


DTViewClass::~DTViewClass()
//-------------------------
{
    _members->clearAndDestroy();
    delete _members;
    viewManager()->unregister( this );
}

void DTViewClass::load()
//----------------------
{
    WVList          dataMembers;
    WVList          methods;
    WVList          friends;
    int             i;
    WString         str;
    ClassType *     cls = (ClassType *) _symbol;
    Symbol *        s;

    _members->clearAndDestroy();

    cls->dataMembers( dataMembers );
    cls->memberFunctions( methods );
    cls->friendFunctions( friends );

    for( i = 0; i < friends.count(); i += 1 ) {
        s = (Symbol *) friends[ i ];
        str.printf( "    friend %s;", s->scopedName( false ) );
        _members->insert( new ClassMember( s, str.gets() ) );
    }

    for( i = 0; i < dataMembers.count(); i += 1 ) {
        s = (Symbol *) dataMembers[ i ];

        str.printf( "    %s;", s->scopedName( false ) );
        _members->insert( new ClassMember( s, str.gets() ) );
    }

    for( i = 0; i < methods.count(); i += 1 ) {
        s = (Symbol *) methods[ i ];

        str.printf( "    %s;", s->scopedName( false ) );
        _members->insert( new ClassMember( s, str.gets() ) );
    }

    addDescriptions();
    fillBox();
}
void DTViewClass::fillBox()
//-------------------------
{
    ClassMember * mem;
    int           i;

    _symbolBox->reset();

    for( i = 0; i < _members->entries(); i += 1 ) {
        mem = (*_members)[ i ];
        _symbolBox->insertString( mem->_text );
    }

    if( _symbolBox->count() <= 0 ) {
        _symbolBox->insertString( "<empty>" );
    }
}

void DTViewClass::event( ViewEvent ev, View * )
//---------------------------------------------
{
    MemberFilter test = optManager()->getMemberFilter();

    switch( ev ) {
    case VEOptionValChange:
        if( memcmp( &_filter, &test, sizeof( MemberFilter ) ) ) {
            _filter = optManager()->getMemberFilter();
            load();
        }
        break;
    default:
        NODEFAULT;
    }
}

ViewEvent DTViewClass::wantEvents()
//---------------------------------
{
    return VEOptionValChange;
}

void DTViewClass::setMenus( MenuManager * mgr )
//---------------------------------------------
{
    DTViewSymbol::setMenus( mgr );

#ifdef DETAIL_STUFF_IMPLEMENTED
    mgr->registerForMenu( this, MIMenuID( MMDetail, DMInheritance ) );
    mgr->registerForMenu( this, MIMenuID( MMDetail, DMStructure ) );
#endif
}

void DTViewClass::unsetMenus( MenuManager * mgr )
//-----------------------------------------------
{
    DTViewSymbol::unsetMenus( mgr );

#ifdef DETAIL_STUFF_IMPLEMENTED
    mgr->unRegister( MIMenuID( MMDetail, DMInheritance ) );
    mgr->unRegister( MIMenuID( MMDetail, DMStructure ) );
#endif
}

void DTViewClass::menuSelected( const MIMenuID & id )
//---------------------------------------------------
{
#ifdef DETAIL_STUFF_IMPLEMENTED
    switch( id.mainID() ) {
    case MMDetail:
        switch( id.subID() ) {
        case DMInheritance:
            viewManager()->showDetailView( _symbol, ViewInherit );
            break;
        case DMStructure:
            viewManager()->showDetailView( _symbol, ViewStructure );
            break;
        }
    }
#endif

    DTViewSymbol::menuSelected( id );
}


void DTViewClass::addDescriptions()
//---------------------------------
// add the non-symbol lines
{
    int             i;
    ClassMember *   mem;
    dr_access       prevAccess = (dr_access)0;
    dr_handle       prevParent = _symbol->getHandle();
    char *          tmpName;
    WCPtrOrderedVector<ClassMember> desc;

    for( i = 0; i < _members->entries(); i += 1 ) {
        mem  = (*_members)[ i ];
        if( mem->_parent != prevParent ) {
            prevAccess = (dr_access)0;
            prevParent = mem->_parent;
            if( mem->_parent != DR_HANDLE_NUL ) {
                tmpName = DRGetName( mem->_parent );
            } else {
                tmpName = NULL;
            }
            desc.append( new ClassMember( mem->_parent, (dr_access)0, tmpName ) );
            WBRFree( tmpName );
        }
        if( mem->_access != prevAccess ) {
            prevAccess = mem->_access;
            desc.append( new ClassMember( mem->_parent, mem->_access,
                                AccessStr[ mem->_access ] ) );
        }
    }

    for( i = 0; i < desc.entries(); i += 1 ) {
        _members->insert( desc[ i ] );
    }

    desc.clear();       // don't own elements, so don't destroy
}

void DTViewClass::elementClicked( WWindow * )
//-------------------------------------------
{
    int             sel = _symbolBox->selected();
    const Symbol *  sym;

    if( sel < 0 ) {
        return;
    }

    sym = (*_members)[ sel ]->_symbol;

    if( sym ) {
        popDetail( sym );
    }
}

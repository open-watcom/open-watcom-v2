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


#include <wtext.hpp>
#include <wstring.hpp>
#include <wpshbttn.hpp>
#include <drsrchdf.h>
#include "mem.h"
#include "verblist.h"
#include "strcview.h"
#include "assure.h"
#include "descrip.h"
#include "methfilt.h"
#include "classtyp.h"
#include "varsym.h"
#include "classlat.h"
#include "projmodl.h"

const short StrucWidth = 200;
const short StrucHeight = 208;

StrucView::StrucView()
    : WBRWindow( WRect(0,0,0,0), "" )
    , _listbox(NULL)
    , _listobjs(NULL)
    , _type(NULL)
    , _lattice(NULL)
//--------------------
{
}

StrucView::StrucView( int x, int y, Symbol * info )
    :WBRWindow( WRect(x,y,StrucWidth,StrucHeight), "" )
    ,_listbox( new VerboseListBox( WRect( 5, 30, -5, -25 ), this ))
    ,_listobjs( new WVList )
    ,_type( new WText( this, WRect( 5, -20, -5, -2 ), NULL,
                       TextStyleLeftNoWrap | TextStyleAmpersands ))
    ,_treeRoot(NULL)
    ,_nodesSeen( new WVList )
    ,_lattice( new ClassLattice( info ) )
    ,_filter( activeProject->getFiltFlags( Filt_Members ))
//-------------------------------------------------------------------
{
    WString title = info->name();
    Symbol * sym;

    title.concat( " Structure" );
    setText( title );

    _listbox->setCallbacks( this, (cbw)&StrucView::itemSelected,
                        (bcbwi)&StrucView::keyPressed,
                        (cbw)&StrucView::itemDblClicked );
//  _listbox->setFont(stdFont);
    _listbox->select( 0 );
    _listbox->show();
//  _type->setFont(stdFont);
    _type->show();

    WPushButton * bttn;
    bttn = new WPushButton( this, WRect( 5, 2, 25, 25 ), "+" );
    bttn->onClick( this, (cbw) plusSelected );
    bttn->show();

    bttn = new WPushButton( this, WRect( 40, 2, 25, 25 ), "-" );
    bttn->onClick( this, (cbw) minusSelected );
    bttn->show();

    bttn = new WPushButton( this, WRect( 75, 2, 25, 25 ), "*" );
    bttn->onClick( this, (cbw) starSelected );
    bttn->show();

    bttn = new WPushButton( this, WRect( -70, 2, 65, 25 ), "&Filters..." );
    bttn->onClick( this, (cbw) filterSelected );
    bttn->show();

    sym = Symbol::defineSymbol( info );
    _treeRoot = new StrucViewItem( this, sym );
    reset();

    _listbox->setFocus();
    show();
}

StrucView::~StrucView()
//---------------------
{
    //NYI -- DetailView ViewCreator::viewDying( CreateStructView, this );
    delete _treeRoot;
    delete _listobjs;
    delete _nodesSeen;
    _lattice->deleteLattice();
}

static WBRWindow * StrucView::createView( int x, int y, Symbol * info )
//----------------------------------------------------------------------
{
    return new StrucView( x, y, info );
}

bool StrucView::isEqual( WObject * sym ) {
    return _treeRoot->isEqual( sym );
}

void StrucView::reset()
//---------------------
{
    _listbox->reset();
    _listobjs->reset();
    _nodesSeen->reset();
    _treeRoot->reset();

    char * tmpstr = _treeRoot->name();
    _listbox->insertString( tmpstr, 0 );
    delete tmpstr;

    _listobjs->insertAt( 0, _treeRoot );

    int tmp = 0;
    _treeRoot->collapseNode( tmp );
    tmp = 0;
    _treeRoot->expandNode( tmp );

    _type->setText( _treeRoot->typeStr() );

    _listbox->select(0);
}

void StrucView::filterSelected( WWindow * )
//-----------------------------------------
{
    MethodFilter filt( 150, 150, FALSE);

    if( filt.process() ) {
        _filter = filt.getCurrentFlags();
        reset();
    }
}

void StrucView::addSeen( StrucViewItem * item )
//---------------------------------------------
{
    _nodesSeen->add( item );
}

bool StrucView::isSeen( Symbol * sym )
//------------------------------------
{
    bool ret = FALSE;
    if( _nodesSeen->find( sym ) ) {
        ret = TRUE;
    }
    return ret;
}

bool StrucView::setMinimumSize( short *w, short *h )
//--------------------------------------------------
{
    *w = StrucWidth;
    *h = StrucHeight;
    return TRUE;
}

void StrucView::itemSelected( WWindow *)
//--------------------------------------
{
    int index = _listbox->selected();
    StrucViewItem * item = (StrucViewItem *)(*_listobjs)[ index ];
    if (item) {
        _type->setText( item->typeStr() );
    }
}

int StrucView::keyPressed( WWindow *, int key )
//---------------------------------------------
{
    if( key == WKeyPlus || key == WKeyMinus || key == WKeyAsterisk ){
        switch( (WKeyCode) key ) {
        case WKeyPlus:
            plusSelected( this );
            break;
        case WKeyAsterisk:
            starSelected( this );
            break;
        case WKeyMinus:
            minusSelected( this );
            break;
        default:
            REQUIRE( 0, "strcview -- keypressed with bad key!" );
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

void StrucView::plusSelected( WWindow * )
//---------------------------------------
{
    int index = _listbox->selected();
    int curr = index;
    StrucViewItem * item = (StrucViewItem *) (*_listobjs)[ index ];
    REQUIRE( item != NULL, "plusSelected -- no object selected!" );

    _type->setText( item->typeStr() );

    item->expandNode( index );
}

void StrucView::minusSelected( WWindow * )
//----------------------------------------
{
    int index = _listbox->selected();
    int curr = index;
    StrucViewItem * item = (StrucViewItem *) (*_listobjs)[ index ];
    REQUIRE( item != NULL, "plusSelected -- no object selected!" );

    _type->setText( item->typeStr() );

    if( item->state() == Expanded ) {
        item->collapseNode( index );
        char * tmp = item->name();
        _listobjs->insertAt( index, item );
        _listbox->insertString( tmp, index );
        delete tmp;
        _listbox->select( curr );
    }
}

void StrucView::starSelected( WWindow * )
//---------------------------------------
{
    int index = _listbox->selected();
    int curr = index;
    StrucViewItem * item = (StrucViewItem *) (*_listobjs)[ index ];
    REQUIRE( item != NULL, "starSelected -- no object selected!" );

    _type->setText( item->typeStr() );

    if( item->state() != LeafNode ){
        item->expandAllNode( index );
        _listbox->select( curr );
    }
}

void StrucView::itemDblClicked( WWindow * )
//-----------------------------------------
{
    int index = _listbox->selected();
    if (_treeRoot) {
        StrucViewItem * item = (StrucViewItem *) (*_listobjs)[ index ];
        if( item ) {
            popDetail( item->info() );
        }
    }
}

//////////////////////////StrucViewItem////////////////////////////////

StrucViewItem::StrucViewItem( StrucView * parent, Symbol * strucInfo,
                              uchar indent )
    :_initialized(FALSE)
    ,_expandState(LeafNode)
    ,_info(strucInfo)
    ,_indentLevel(indent)
    ,_parent(parent)
    ,_classNode(NULL)
{
}

StrucViewItem::StrucViewItem( StrucView * parent, ClassLattice * node,
                              uchar indent )
    :_initialized(FALSE)
    ,_expandState(LeafNode)
    ,_info(node->makeSymbol())
    ,_indentLevel(indent)
    ,_parent(parent)
    ,_classNode(node)
{
}

bool StrucViewItem::isEqual( WObject * sym ) {
    return _info->isEqual( sym );
}

StrucViewItem::~StrucViewItem()
{
    delete _info;
    _kids.deleteContents();
}

char * StrucViewItem::name( void )
//--------------------------------
{
    char * tmp;
    initState();

    if( _indentLevel > 0 ) {
        tmp = new char[ _indentLevel * 2 + 3 + _name.size() ];
        memset( tmp, ' ', _indentLevel * 2 );
        tmp[ _indentLevel * 2 ] = '\0';
    } else {
        tmp = new char[ _name.size() + 3 ];
        tmp[ 0 ] = '\0';
    }

    switch( _expandState ) {
        case LeafNode:
            strcat( tmp, "  " );
            break;
        case Collapsed:
            strcat( tmp, "+ " );
            break;
        case Expanded:
            strcat( tmp, "- " );
            break;
        default:
            REQUIRE( 0, "strcview name -- _expandState out of range" );
    }

    strcat( tmp, _name );
    return tmp;
}

void StrucViewItem::expandNode( int index )
//-----------------------------------------
{
    VerboseListBox * listbox = _parent->getListBox();
    WVList         * listobjs = _parent->getObjs();
    int childIndex = index + 1;
    int i;

    initState();

    if( _expandState == Collapsed ) {
        _expandState=Expanded;
    } else {
        return;
    }

    char * tmp = name();
    listbox->deleteString( index );
    listbox->insertString( tmp, index );
    delete tmp;
    listobjs->removeAt( index );
    listobjs->insertAt( index, this );

    for( i = 0; i < _kids.count(); i++ ) {
        char *tmp = ((StrucViewItem *)_kids[i])->name();
        listbox->insertString( tmp, childIndex );
        delete tmp;
        listobjs->insertAt( childIndex, _kids[i] );
        childIndex ++;
    }

    listbox->select( index );
    return;
}

void StrucViewItem::collapseNode( int &index )
//--------------------------------------------
{
    WVList *         listobjs = _parent->getObjs();
    VerboseListBox * listbox = _parent->getListBox();

    initState();

    if( _expandState == Expanded ) {
        index += 1;
        for( int i = 0; i < _kids.count();  i++ ) {
            ((StrucViewItem *)_kids[ i ])->collapseNode( index );
        }
        index -= 1;
    }

    listobjs->removeAt( index );
    listbox->deleteString( index );
    if( _expandState == Expanded ) {
        _expandState = Collapsed;
    }
}

void StrucViewItem::expandAllNode( int &index )
//---------------------------------------------
{
    WVList *         listobjs = _parent->getObjs();
    VerboseListBox * listbox = _parent->getListBox();

    initState();

    if( (*listobjs)[ index ] != this ) {
        if( _expandState == Collapsed ) {
            _expandState = Expanded;
        }
        listobjs->insertAt( index, this );
        char * tmp = name();
        listbox->insertString( tmp, index );
        delete tmp;
        if( _expandState == Collapsed ) {
            _expandState = Expanded;
        }
    } else {
        if( _expandState == Collapsed ) {
            _expandState = Expanded;
            listbox->deleteString( index );
            char * tmp = name();
            listbox->insertString( tmp, index );
            delete tmp;
        }
    }
    index += 1;

    if( _expandState != LeafNode ) {
        for( int i = 0; i < _kids.count();  i++ ) {
            ((StrucViewItem *)_kids[ i ])->expandAllNode( index );
        }
    }
}

const char * StrucViewItem::typeStr(void)
//---------------------------------------
{
    initState();
    return _type;
}

void StrucViewItem::initState()
//-----------------------------
// if i represent a variable, i must find my type
// if the base type is a ClassType, then i am Collapsed
// else i am LeafNode
{
    bool        do_inherited = FALSE;
    ClassType * classType;
    dr_sym_type stype = _info->symtype();

    REQUIRE( stype != DR_SYM_NOT_SYM,"strucview::initstate ack");

    if( !_initialized ) {
        if( _classNode != NULL ) {
            REQUIRE( stype == DR_SYM_CLASS,"strucview::initstate nak");
            classType = (ClassType *)_info;
            _type = _name;
        } else {
            classType = flattenTypeDesc( _info, _type );
            _name = _info->name();
        }

        if( classType ) {
            WVList kidInfos;
            FilterFlags flags = _parent->getFilter();
            if( ( flags & FILT_INHERITED ) || ( flags & FILT_ALL_INHERITED )) {
                do_inherited = TRUE;
                FILT_RESET_INHERITED( flags );
            }

            classType->dataMembers( kidInfos, flags );
            if( kidInfos.count() != 0 ) {
                _expandState = Collapsed;
                _parent->addSeen( this );
                for( int i = 0; i < kidInfos.count(); i++ ) {
                    _kids.add( new StrucViewItem( _parent, ((Symbol *)kidInfos[i]),
                                                  _indentLevel + 1 ) );
                }
            } else {
                _expandState = LeafNode;
            }
            if( classType != _info ) {
                delete classType;
            }

            if( do_inherited ) {
                startInherited();
            }
        } else {
            _expandState = LeafNode;
        }
        _initialized = TRUE;
    }
}

ClassType * StrucViewItem::flattenTypeDesc( Symbol * sym, WString & desc )
//------------------------------------------------------------------------
{
    ClassType *     classType = NULL;
    char *          str = NULL;
    WVList          typeParts;
    Description *   entry;
    int             i;

    REQUIRE( sym != NULL, "strucview::flatten passed null symbol!" );
    dr_sym_type stype = sym->symtype();
    REQUIRE( stype != DR_SYM_NOT_SYM,"strucview::flatten bad set");

    switch( stype ) {
    case DR_SYM_VARIABLE:
        // need to find out the type of the data member
        ((VariableSym *) sym)->loadTypeInfo( typeParts );
        for( i = 0; i < typeParts.count(); i++ ) {
            entry = (Description *) typeParts[i];
            if( entry->_nameGoesHere ) {
                desc.concat( entry->u.text );
            } else {
                if( !sym->isEqual( entry->u.sym ) ){
                    if( classType == NULL ) {
                        classType = flattenTypeDesc( entry->u.sym, desc );
                    } else {
                        desc.concat( entry->u.sym->name() );
                    }
                }
            }
        }
        break;

    case DR_SYM_TYPEDEF:
    case DR_SYM_ENUM:
        sym->description( typeParts );
        for( i = 0; i < typeParts.count(); i++ ) {
            entry = (Description *) typeParts[ i ];
            if( entry->_isUserDefined ) {
                if( entry->u.sym->symtype() == DR_SYM_CLASS && classType == NULL ) {
                    classType = (ClassType *) entry->u.sym;
                }
            }
        }
        desc.concat( sym->name() );
        break;

    case DR_SYM_CLASS:
        if( _parent->isSeen( sym ) ) {
            desc.concat( sym->name() );
            classType = NULL;
        } else {
            classType = (ClassType *) sym;
            desc.concat( classType->name() );
        }
        break;

    default: {}
    }

    for( i=0; i < typeParts.count(); i++ ) {
        entry = (Description *) typeParts[ i ];
        if( entry->_isUserDefined && entry->u.sym != classType &&
            entry->u.sym != sym ) {
            delete entry->u.sym;
        }
    }
    typeParts.deleteContents();

    return classType;
}

struct InheritedData
{
    StrucViewItem * me;
    WString *       name;
};

void StrucViewItem::startInherited( void )
//----------------------------------------
{
    WString       start( _name );
    InheritedData data;
    data.me = this;
    data.name = &start;
    if( _classNode != NULL ) {
        _classNode->enumerateBases( &inhHook, &data );
    } else {
        _parent->getLattice()->enumerateBases( &inhHook, &data );
    }
}

bool StrucViewItem::inhHook( DerivationPtr & ptr, void * info )
//-------------------------------------------------------------
{
    InheritedData * data = (InheritedData *) info;
    WString         name( *data->name );
    StrucViewItem * item = NULL;

    name.concat( "::" );
    name.concat( ptr._class->name() );

    if( ptr._virtuality == DR_VIRTUALITY_VIRTUAL ) {
        WVList * list = data->me->_parent->getSeen();
        for( int i = list->count(); i > 0; i -= 1 ) {
            StrucViewItem * test = (StrucViewItem *) (*list)[ i - 1 ];
            if( test->_info->getHandle() == ptr._class->getHandle() ) {
                item = test;
            }
        }
    }
    if( item != NULL ) {
        item->_name.concat( ", " );
        item->_name.concat( name );
    } else {
        item = new StrucViewItem( data->me->_parent, ptr._class,
                                  data->me->_indentLevel );
        item->_name = name;
        item->initState();
        data->me->_kids.add( item );
    }

    return TRUE;
}

void StrucViewItem::reset( void )
//-------------------------------
{
    _kids.deleteContents();
    _initialized = FALSE;
    initState();
}

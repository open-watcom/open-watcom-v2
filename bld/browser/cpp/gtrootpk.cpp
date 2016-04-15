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


#include "brwhelp.h"
#include "gtrootpk.h"
#include "symbol.h"

TreeRootElement::TreeRootElement( TreeRoot * root )
                    : ModuleItem( new WString( root->node()->name() ),
                                    root->enabled() )
                    , _root( root )
//--------------------------------------------------------------------
{
}

void TreeRootElement::okPressed()
//-------------------------------
{
    if( _enabled && !_root->enabled() ) {
        _root->node()->enableTree( TreeNode::Visible );
    }

    _root->setEnable( _enabled );
}


// ----------------- TreeRootSelect --------------------- //


TreeRootSelect::TreeRootSelect( WWindow * prt, TreeRootList * roots )
                    : EditModuleList( prt, "Select Root Nodes", false )
                    , _roots( roots )
//---------------------------------------------------------------------
{
}

void TreeRootSelect::okButton( WWindow * )
//----------------------------------------
{
    int i;

    for( i = 0; i < _moduleBox->count(); i += 1 ) {
        ((TreeRootElement *) (*_moduleBox)[ i ])->okPressed();
    }
    quit( true );
}

void TreeRootSelect::helpButton( WWindow * )
//------------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_SELECT_ROOTS );
}

bool TreeRootSelect::contextHelp( bool is_active_win )
//----------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_SELECT_ROOTS );
    }
    return( true );
}

void TreeRootSelect::loadBox()
//----------------------------
{
    int i;

    for( i = 0; i < _roots->count(); i += 1 ) {
        _moduleBox->insert( new TreeRootElement( (*_roots)[ i ] ) );
    }
}

// -------------------- OutlineRootElement ---------------------- //

OutlineRootElement::OutlineRootElement( OutlineElement * elm )
                    : ModuleItem( new WString( elm->symbol()->name() ),
                                    elm->enabled() )
                    , _element( elm )
//--------------------------------------------------------------------
{
}

void OutlineRootElement::okPressed()
//-------------------------------
{
    _element->setEnable( _enabled );
}


// ----------------- OutlineRootSelect --------------------- //


OutlineRootSelect::OutlineRootSelect( WWindow * prt, OutlineElement * root )
                    : EditModuleList( prt, "Select Root Nodes", false )
                    , _root( root )
//---------------------------------------------------------------------
{
}

void OutlineRootSelect::okButton( WWindow * )
//-------------------------------------------
{
    int i;

    for( i = 0; i < _moduleBox->count(); i += 1 ) {
        ((OutlineRootElement *) (*_moduleBox)[ i ])->okPressed();
    }
    quit( true );
}

void OutlineRootSelect::helpButton( WWindow * )
//---------------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_SELECT_ROOTS );
}

bool OutlineRootSelect::contextHelp( bool is_active_win )
//-------------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_SELECT_ROOTS );
    }
    return( true );
}

void OutlineRootSelect::loadBox()
//-------------------------------
{
    OutlineElement * elm;

    for( elm = _root->sibling(); elm != NULL; elm = elm->sibling() ) {
        _moduleBox->insert( new OutlineRootElement( elm ) );
    }
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

TreeRootElement::~TreeRootElement()
//---------------------------------
{
}

TreeRootSelect::~TreeRootSelect()
//-------------------------------
{
}
OutlineRootElement::~OutlineRootElement()
//---------------------------------
{
}

OutlineRootSelect::~OutlineRootSelect()
//-------------------------------------
{
}

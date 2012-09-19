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


#ifndef mitem_class
#define mitem_class

#include "wfilenam.hpp"
#include "wvlist.hpp"
#include "mrule.hpp"
#include "mstate.hpp"

#define MITEM_OWNER_IDE         0
//#define MITEM_OWNER_WINMAKER    1 Blue Sky support removed 1-Jul-2006

WCLASS MComponent;
WCLASS MAction;
WCLASS MItem : public WFileName
{
        Declare( MItem )
        public:
                WEXPORT MItem( const char* name, MComponent* comp, MRule* rule, bool isTarget=FALSE );
                WEXPORT ~MItem();
                int compare( const WObject* ) const;

                void name( WString& n );
                void absName( WFileName& fn );
                bool result( WFileName& f );
                bool absResult( WFileName& f );
                bool touchResult();

                MRule* rule() { return _rule; }
                void setRule( MRule* rule );
                bool rename( WFileName& name, MRule* rule );
                MItem* parent() { return _parent; }
                void setParent( MItem* parent ) { _parent = parent; }
                MComponent* component() { return _component; }
                void setComponent( MComponent* component ) { _component = component; }
                WVList& states() { return _states; }
                int addActions( WVList& list );
                WVList& getActionStates( MAction* action );
                WVList* findActionStates( MAction* action );
                int expandAction( const WString& actionName, WString& command );
                int expandAction( MAction* action, WString& command );
                void updateAttribs();
                bool exists() { return _exists; }
                void resetRuleRefs();
                bool ismakeable();
                void addStates( WVList& list, SwMode mode );
                int type();
                void setExpanded( bool expanded=TRUE ) { _expanded = expanded; }
                bool expanded() { return _expanded; }
                bool included();
                unsigned owner() { return( _owner ); }
                void setOwner( unsigned owner ) { _owner = owner; }
                bool wasVisited( void ) { return( _visited ); }
                void setVisited( bool state ) { _visited = state; }
        private:
                MComponent*     _component;
                WString         _ruleTag;
                MRule*          _rule;
                WVList          _states;
                MItem*          _parent;
                char            _attribs;
                bool            _exists;
                bool            _isTarget;
                WVList          _actionStates;
                bool            _visited;
                unsigned        _owner;
                bool            _expanded;
                void pruneStates( WVList& states );
                void addDecorators( WString& n );
                void addActions( WFileName& fn, WVList& list );
};

#endif


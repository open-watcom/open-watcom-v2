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


#ifndef __GTREF_H__
#define __GTREF_H__

#include <wtptlist.hpp>
#include "wbrdefs.h"
#include "gtbase.h"
#include "wbrwin.h"

class TreeRefNode;
typedef TemplateList<TreeRefNode *> TreeRefList;

class TreeRefPtr : public TreePtr
{
public:
            TreeRefPtr( TreeNode * from, TreeNode * to )
                : TreePtr( from, to )
                , _numRefs( 1 ) {}

            void        incRefs() { _numRefs += 1; }

    virtual PaintInfo * getPaintInfo();
    virtual TreePtr *   makeClone( void );

private:
            int         _numRefs;
};

class TreeRefNode : public TreeNode
{
public:
            TreeRefNode( TreeWindow * prt, TreeNode * ref, TreeNode * prnod );
    virtual ~TreeRefNode();

    virtual int         getLevel( void ) const;
    virtual drmem_hdl   getHandle( void ) const { return _ref->getHandle(); }
    virtual char *      name() { return _ref->name(); }
    virtual bool        isRelated( TreeNode * rel ) { return _ref->isRelated( rel ); }
    virtual bool        isReference( void ) const { return true; }
    virtual void        deleteTree( void ) { _ref->deleteTree(); }
    virtual Symbol *    makeSymbol( void ) { return _ref->makeSymbol(); }

    virtual const char *className() const { return "TreeRefNode"; }

            TreeNode *  refersTo( void ) const { return _ref; }

protected:
    virtual TreePtr*    getPtr( TreeList_T list, int index );
    virtual TreeNode*   getNode( TreeList_T list, int index );
    virtual int         getCount( TreeList_T list );
    virtual TreeNode *  removeNode( TreeList_T, TreeNode * );
    virtual void        addPtr( TreeList_T, TreePtr * );
    virtual void        swapNode( TreeList_T list, TreeNode * from, TreeNode * to );
    virtual PaintInfo * getPaintInfo( void ) { return _ref->getPaintInfo(); }
    virtual void        sortPrtKids(){}
private:
            TreeNode*   _ref;           // who I refer to
            TreePtrList _parents;
            short       _level;
};

#endif // __GTREF_H__

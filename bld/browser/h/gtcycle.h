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


#ifndef __GTCYCLE_H__
#define __GTCYCLE_H__

#include "symbol.h"
#include "gtbase.h"
#include "gtref.h"

class TreeCycleNode;
typedef TemplateList<TreeCycleNode*> TreeCycleList;
typedef TemplateList<TreeCycleList*> TreeCycleListList;

struct TreeCycleRelative
{
    TreeCycleNode * node;
    short           levelAt;
};
typedef TemplateList<TreeCycleRelative*> TreeCycleRelList;

class TreeCycleNode : public TreeNode
{
public:
            TreeCycleNode( TreeWindow * prt, TreeCycleList * flatNode = NULL,
                           TreeRefList * refList = NULL );
    virtual ~TreeCycleNode();

    virtual int         getLevel( void ) const { return _level; }
    virtual TreePtr*    getPtr( TreeList_T list, int index );
    virtual TreeNode*   getNode( TreeList_T list, int index );
    virtual int         getCount( TreeList_T list );
    virtual TreeNode *  removeNode( TreeList_T list, TreeNode * node );
    virtual void        addPtr( TreeList_T list, TreePtr * );
    virtual void        swapNode( TreeList_T list, TreeNode * from, TreeNode * to );
    virtual bool        isRelated( TreeNode * );
    virtual bool        hasReference( void ) const { return _hasRef; }
    virtual void        deleteTree( void );

#ifdef DEBUG
            const char *className() const { return "TreeCycleNode"; }
#endif

            void        breakCycles( void );
            void        assignLevels( void );

protected:
            int         findNode( TreeNode * node );
            int         findNode( dr_handle handle );
            int         findChild( TreeNode * node );
            bool        isChild( TreeNode * node );
            int         findParent( TreeNode * node );
            bool        isParent( TreeNode * node );

            TreePtrList     _parents;
            TreeNodeList    _children;
            bool            _hasRef;

            TreeCycleList * _flatNodes;
            TreeRefList *   _flatReferences;

private:
            // _level is used by the cycle breaking routine -- assignLevels
            // must be called after breakCycles.
            int             _level;
            bool            _entered;
            int             findComponents( TreeCycleList& seen,
                                            short& level );
            void            levelAdjust( TreeCycleList & seen,
                                         TreeCycleRelList & relatives,
                                         int seenAt );
            TreeCycleList * joinTrees( TreeCycleListList & roots,
                                       TreeCycleList * nodes,
                                       TreeCycleRelList & relatives );
};

#endif // __GTCYCLE_H__
